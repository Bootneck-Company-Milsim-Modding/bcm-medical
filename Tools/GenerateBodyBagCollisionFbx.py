import struct
import zlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Assets/BodyBags/BCM_FilledBodyBag/BCM_FilledBodyBag.fbx"
OUTPUT = ROOT / "Assets/BodyBags/BCM_FilledBodyBag/BCM_FilledBodyBag_Collision.fbx"
BAKED_ENTITY_SCALE = 3.5


def read_property(data, offset):
    kind = chr(data[offset])
    offset += 1

    scalar_formats = {
        "Y": "h",
        "C": "?",
        "I": "i",
        "F": "f",
        "D": "d",
        "L": "q",
    }
    if kind in scalar_formats:
        fmt = scalar_formats[kind]
        size = struct.calcsize("<" + fmt)
        return (kind, struct.unpack_from("<" + fmt, data, offset)[0]), offset + size

    if kind in ("S", "R"):
        length = struct.unpack_from("<I", data, offset)[0]
        offset += 4
        value = data[offset : offset + length]
        if kind == "S":
            value = value.decode("utf-8", "replace")
        return (kind, value), offset + length

    array_formats = {
        "f": "f",
        "d": "d",
        "i": "i",
        "l": "q",
        "b": "B",
    }
    if kind in array_formats:
        count, encoding, compressed_length = struct.unpack_from("<III", data, offset)
        offset += 12
        payload = data[offset : offset + compressed_length]
        if encoding:
            payload = zlib.decompress(payload)
        fmt = array_formats[kind]
        values = list(struct.unpack("<" + fmt * count, payload))
        return (kind, values), offset + compressed_length

    raise ValueError(f"Unsupported FBX property type: {kind}")


def read_nodes(data, start, end):
    nodes = []
    offset = start
    while offset + 13 <= end:
        end_offset, property_count, _, name_length = struct.unpack_from("<IIIB", data, offset)
        if end_offset == 0:
            return nodes, offset + 13

        name = data[offset + 13 : offset + 13 + name_length].decode("utf-8")
        property_offset = offset + 13 + name_length
        properties = []
        for _ in range(property_count):
            value, property_offset = read_property(data, property_offset)
            properties.append(value)

        children, _ = read_nodes(data, property_offset, end_offset - 13)
        nodes.append((name, properties, children))
        offset = end_offset

    return nodes, offset


def encode_property(prop):
    kind, value = prop
    scalar_formats = {
        "Y": "h",
        "C": "?",
        "I": "i",
        "F": "f",
        "D": "d",
        "L": "q",
    }
    if kind in scalar_formats:
        return kind.encode("ascii") + struct.pack("<" + scalar_formats[kind], value)

    if kind in ("S", "R"):
        payload = value.encode("utf-8") if kind == "S" else value
        return kind.encode("ascii") + struct.pack("<I", len(payload)) + payload

    array_formats = {
        "f": "f",
        "d": "d",
        "i": "i",
        "l": "q",
        "b": "B",
    }
    if kind in array_formats:
        payload = struct.pack("<" + array_formats[kind] * len(value), *value)
        return kind.encode("ascii") + struct.pack("<III", len(value), 0, len(payload)) + payload

    raise ValueError(f"Unsupported FBX property type: {kind}")


def encode_node(node, start_offset):
    name, properties, children = node
    name_bytes = name.encode("utf-8")
    property_bytes = b"".join(encode_property(prop) for prop in properties)
    child_offset = start_offset + 13 + len(name_bytes) + len(property_bytes)
    child_bytes = bytearray()
    for child in children:
        encoded = encode_node(child, child_offset + len(child_bytes))
        child_bytes.extend(encoded)

    payload = name_bytes + property_bytes + bytes(child_bytes) + bytes(13)
    end_offset = start_offset + 13 + len(payload)
    header = struct.pack(
        "<IIIB",
        end_offset,
        len(properties),
        len(property_bytes),
        len(name_bytes),
    )
    return header + payload


def p(kind, value):
    return kind, value


def collision_objects(root_nodes):
    # The visible bag is approximately 2.13 m long by 0.90 m wide after the
    # baked entity scale. Leave a small wrapper allowance around that mesh.
    min_x, max_x = -30.0, 2.0
    min_y, max_y = -39.0, 28.0
    # Match the scaled wrapper's vertical bounds. The previous lower face was
    # roughly 16.5 cm below the visible mesh and made a resting bag float.
    min_z, max_z = -0.3, 9.8
    # A convex, bevelled cross-section removes the box's stable side faces.
    # The broad top and bottom remain flat, encouraging the bag to settle as a
    # wrapped body instead of balancing upright on an edge.
    bevel = 0.5 * (max_z - min_z)
    cross_section = [
        (min_x + bevel, min_z),
        (max_x - bevel, min_z),
        (max_x, min_z + bevel),
        (max_x - bevel, max_z),
        (min_x + bevel, max_z),
        (min_x, min_z + bevel),
    ]
    points = (
        [(x, min_y, z) for x, z in cross_section]
        + [(x, max_y, z) for x, z in cross_section]
    )
    vertices = [
        coordinate * BAKED_ENTITY_SCALE
        for point in points
        for coordinate in point
    ]

    section_count = len(cross_section)
    faces = [
        list(range(section_count)),
        list(reversed(range(section_count, 2 * section_count))),
    ]
    for index in range(section_count):
        next_index = (index + 1) % section_count
        faces.append([
            index,
            section_count + index,
            section_count + next_index,
            next_index,
        ])

    polygon_indices = []
    normals = []
    for face in faces:
        encoded_face = list(face)
        encoded_face[-1] = -(encoded_face[-1] + 1)
        polygon_indices.extend(encoded_face)

        point_a = points[face[0]]
        point_b = points[face[1]]
        point_c = points[face[2]]
        edge_ab = tuple(point_b[axis] - point_a[axis] for axis in range(3))
        edge_ac = tuple(point_c[axis] - point_a[axis] for axis in range(3))
        normal = (
            edge_ab[1] * edge_ac[2] - edge_ab[2] * edge_ac[1],
            edge_ab[2] * edge_ac[0] - edge_ab[0] * edge_ac[2],
            edge_ab[0] * edge_ac[1] - edge_ab[1] * edge_ac[0],
        )
        length = sum(value * value for value in normal) ** 0.5
        normal = tuple(value / length for value in normal)
        normals.extend(normal * len(face))

    source_geometry_id = 905528368
    source_model_id = 589119312
    geometry_id = 900000001
    model_id = 900000002
    source_objects = next(node for node in root_nodes if node[0] == "Objects")
    source_geometry = next(
        node for node in source_objects[2]
        if node[0] == "Geometry" and node[1][0][1] == source_geometry_id
    )
    source_model = next(
        node for node in source_objects[2]
        if node[0] == "Model" and node[1][0][1] == source_model_id
    )

    source_vertices_node = next(
        child for child in source_geometry[2] if child[0] == "Vertices"
    )
    source_vertices = source_vertices_node[1][0][1]
    axis_values = [source_vertices[index::3] for index in range(3)]
    axis_centres = [
        (min(values) + max(values)) * 0.5 for values in axis_values
    ]
    axis_pivots = (axis_centres[0], axis_centres[1], min(axis_values[2]))
    # These factors produce a player-length bag while retaining enough width
    # to visually wrap an equipped character.
    visual_scale = (1.3, 2.0, 2.4)
    scaled_visual_vertices = []
    for index in range(0, len(source_vertices), 3):
        for axis in range(3):
            value = source_vertices[index + axis]
            scaled_visual_vertices.append(
                (
                    axis_pivots[axis]
                    + (value - axis_pivots[axis]) * visual_scale[axis]
                ) * BAKED_ENTITY_SCALE
            )

    scaled_source_geometry_children = []
    for child in source_geometry[2]:
        if child[0] == "Vertices":
            scaled_source_geometry_children.append(
                (child[0], [p("d", scaled_visual_vertices)], child[2])
            )
        else:
            scaled_source_geometry_children.append(child)
    scaled_source_geometry = (
        source_geometry[0],
        source_geometry[1],
        scaled_source_geometry_children,
    )

    scaled_source_model_children = []
    for child in source_model[2]:
        if child[0] != "Properties70":
            scaled_source_model_children.append(child)
            continue

        properties = []
        for model_property in child[2]:
            values = model_property[1]
            if (
                model_property[0] == "P"
                and values
                and values[0][1] in ("GeometricTranslation", "Lcl Translation")
            ):
                values = values[:-3] + [
                    p(value[0], value[1] * BAKED_ENTITY_SCALE)
                    for value in values[-3:]
                ]
            properties.append((model_property[0], values, model_property[2]))
        scaled_source_model_children.append((child[0], child[1], properties))

    scaled_source_model = (
        source_model[0],
        source_model[1],
        scaled_source_model_children,
    )

    preserved_geometry_children = [
        child for child in source_geometry[2]
        if child[0] in ("Properties70", "GeometryVersion")
    ]
    geometry = (
        "Geometry",
        [p("L", geometry_id), source_geometry[1][1], source_geometry[1][2]],
        preserved_geometry_children
        + [
            ("Vertices", [p("d", vertices)], []),
            ("PolygonVertexIndex", [p("i", polygon_indices)], []),
            (
                "LayerElementNormal",
                [p("I", 0)],
                [
                    ("Version", [p("I", 101)], []),
                    ("Name", [p("S", "")], []),
                    ("MappingInformationType", [p("S", "ByPolygonVertex")], []),
                    ("ReferenceInformationType", [p("S", "Direct")], []),
                    ("Normals", [p("d", normals)], []),
                ],
            ),
            (
                "LayerElementMaterial",
                [p("I", 0)],
                [
                    ("Version", [p("I", 101)], []),
                    ("Name", [p("S", "")], []),
                    ("MappingInformationType", [p("S", "AllSame")], []),
                    ("ReferenceInformationType", [p("S", "IndexToDirect")], []),
                    ("Materials", [p("i", [0])], []),
                ],
            ),
            (
                "Layer",
                [p("I", 0)],
                [
                    ("Version", [p("I", 100)], []),
                    (
                        "LayerElement",
                        [],
                        [
                            ("Type", [p("S", "LayerElementMaterial")], []),
                            ("TypedIndex", [p("I", 0)], []),
                        ],
                    ),
                    (
                        "LayerElement",
                        [],
                        [
                            ("Type", [p("S", "LayerElementNormal")], []),
                            ("TypedIndex", [p("I", 0)], []),
                        ],
                    ),
                ],
            ),
        ],
    )
    model_children = []
    for child in source_model[2]:
        if child[0] != "Properties70":
            model_children.append(child)
            continue

        properties = []
        for model_property in child[2]:
            values = model_property[1]
            if (
                model_property[0] == "P"
                and values
                and values[0][1] in ("GeometricTranslation", "Lcl Translation")
            ):
                values = values[:-3] + [p("D", 0.0), p("D", 0.0), p("D", 0.0)]
            properties.append((model_property[0], values, model_property[2]))
        model_children.append((child[0], child[1], properties))

    model = (
        "Model",
        [p("L", model_id), p("S", "UCX_BCM_FilledBodyBag\x00\x01Model"), source_model[1][2]],
        model_children,
    )
    retained_source_ids = {
        source_geometry_id,
        source_model_id,
        1180717888,  # bodybag material
        1180715488,  # bodybag diffuse video
        1049636224,  # bodybag diffuse texture
        2045644208,  # animation stack retained from the source export
        1174438160,  # animation layer retained from the source export
    }
    retained_source_objects = []
    for node in source_objects[2]:
        if not node[1] or node[1][0][0] != "L" or node[1][0][1] not in retained_source_ids:
            continue
        if node[0] == "Geometry" and node[1][0][1] == source_geometry_id:
            retained_source_objects.append(scaled_source_geometry)
        elif node[0] == "Model" and node[1][0][1] == source_model_id:
            retained_source_objects.append(scaled_source_model)
        else:
            retained_source_objects.append(node)
    excluded_source_ids = {
        node[1][0][1]
        for node in source_objects[2]
        if node[1] and node[1][0][0] == "L" and node[1][0][1] not in retained_source_ids
    }
    objects = ("Objects", source_objects[1], retained_source_objects + [geometry, model])

    source_connections = next(node for node in root_nodes if node[0] == "Connections")
    retained_connections = [
        node for node in source_connections[2]
        if not any(prop[0] == "L" and prop[1] in excluded_source_ids for prop in node[1])
    ]
    collision_connections = [
        ("C", [p("S", "OO"), p("L", geometry_id), p("L", model_id)], []),
        ("C", [p("S", "OO"), p("L", model_id), p("L", 0)], []),
    ]
    connections = (
        "Connections",
        source_connections[1],
        retained_connections + collision_connections,
    )
    return objects, connections


def collision_definitions(source_definitions, objects):
    object_counts = {}
    for node in objects[2]:
        object_counts[node[0]] = object_counts.get(node[0], 0) + 1

    children = []
    for child in source_definitions[2]:
        if child[0] == "Count":
            total = 0
            for definition in source_definitions[2]:
                if definition[0] != "ObjectType":
                    continue
                object_type = definition[1][0][1]
                total += object_counts.get(
                    object_type,
                    next(item for item in definition[2] if item[0] == "Count")[1][0][1],
                )
            children.append((child[0], [p("I", total)], child[2]))
            continue

        if child[0] != "ObjectType" or child[1][0][1] not in object_counts:
            children.append(child)
            continue

        object_children = []
        for object_child in child[2]:
            if object_child[0] == "Count":
                object_children.append(
                    (object_child[0], [p("I", object_counts[child[1][0][1]])], object_child[2])
                )
            else:
                object_children.append(object_child)
        children.append((child[0], child[1], object_children))
    return (source_definitions[0], source_definitions[1], children)


def main():
    source_data = SOURCE.read_bytes()
    if source_data[:23] != b"Kaydara FBX Binary  \x00\x1a\x00":
        raise ValueError("The reference FBX is not a supported binary FBX file")
    if struct.unpack_from("<I", source_data, 23)[0] >= 7500:
        raise ValueError("This generator expects the reference FBX 32-bit node format")

    root_nodes, footer_offset = read_nodes(source_data, 27, len(source_data))
    objects, connections = collision_objects(root_nodes)
    output_nodes = []
    for node in root_nodes:
        if node[0] == "Objects":
            output_nodes.append(objects)
        elif node[0] == "Connections":
            output_nodes.append(connections)
        elif node[0] == "Definitions":
            output_nodes.append(collision_definitions(node, objects))
        else:
            output_nodes.append(node)

    result = bytearray(source_data[:27])
    for node in output_nodes:
        result.extend(encode_node(node, len(result)))
    result.extend(bytes(13))
    result.extend(source_data[footer_offset:])
    OUTPUT.write_bytes(result)


if __name__ == "__main__":
    main()
