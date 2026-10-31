from pathlib import Path

import bpy


PROJECT_ROOT = Path(__file__).resolve().parents[1]
SOURCE = PROJECT_ROOT / "Assets" / "BodyBags" / "BCM_FilledBodyBag" / "BCM_FilledBodyBag.fbx"
OUTPUT_DIRECTORY = PROJECT_ROOT / "Assets" / "BodyBags" / "BCM_EmptyBodyBag"
OUTPUT = OUTPUT_DIRECTORY / "BCM_EmptyBodyBag.fbx"


def main() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)

    bpy.ops.import_scene.fbx(filepath=str(SOURCE))
    bodybag = bpy.data.objects.get("bodybag")
    if bodybag is None or bodybag.type != "MESH":
        raise RuntimeError("Source FBX does not contain the bodybag mesh")

    for obj in list(bpy.context.scene.objects):
        if obj != bodybag:
            bpy.data.objects.remove(obj, do_unlink=True)

    bodybag.name = "bodybag"
    bodybag.data.name = "bodybag"
    if bodybag.data.materials:
        bodybag.data.materials[0].name = "bodybag"

    OUTPUT_DIRECTORY.mkdir(parents=True, exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT")
    bodybag.select_set(True)
    bpy.context.view_layer.objects.active = bodybag
    bpy.ops.export_scene.fbx(
        filepath=str(OUTPUT),
        use_selection=True,
        object_types={"MESH"},
        use_custom_props=True,
        add_leaf_bones=False,
        bake_anim=False,
    )

    print(
        "BCM_EMPTY_BODY_BAG_EXPORT",
        OUTPUT,
        f"vertices={len(bodybag.data.vertices)}",
        f"materials={len(bodybag.data.materials)}",
    )


if __name__ == "__main__":
    main()
