"""Normalize supplied medical models and export native FBX sources with simple collision."""
import bpy,json,math,hashlib,re,sys
from pathlib import Path
from mathutils import Vector,Matrix
root=Path(__file__).resolve().parents[2];src=root/".checks/vendor-medical/Mod Assets"
manifest=json.loads((root/"Tools/AssetImport/MedicalAssets.json").read_text())
only=next((a.split("=",1)[1].split(",") for a in sys.argv if a.startswith("--bcmm-assets=")),None)
for item in manifest:
 if only and item["name"] not in only:continue
 name=item["name"];folder=root/"Assets/Medical"/name;folder.mkdir(parents=True,exist_ok=True)
 bpy.ops.wm.read_factory_settings(use_empty=True)
 file=(root/item["source_root"] if "source_root" in item else src)/item["source"]
 if file.suffix==".fbx":bpy.ops.import_scene.fbx(filepath=str(file),use_anim=False)
 elif file.suffix==".blend":
  with bpy.data.libraries.load(str(file),link=False) as (available,loaded):loaded.objects=available.objects
  for loaded_object in loaded.objects:
   if loaded_object and loaded_object.type=="MESH":bpy.context.scene.collection.objects.link(loaded_object)
 else:bpy.ops.import_scene.gltf(filepath=str(file))
 meshes=[o for o in bpy.context.scene.objects if o.type=="MESH" and not any(s in o.name for s in ("_LOD1","_LOD2"))]
 if "include_objects" in item:meshes=[o for o in meshes if o.name in item["include_objects"]]
 if "exclude_objects_prefix" in item:meshes=[o for o in meshes if not any(o.name.startswith(prefix) for prefix in item["exclude_objects_prefix"])]
 if not meshes:raise RuntimeError("No selected meshes for "+name)
 for o in meshes:
  if not o.data.materials:o.data.materials.append(bpy.data.materials.get(item["materials"][0]["source"]) or bpy.data.materials.new(item["materials"][0]["source"]))
 # Vendor injector LODs are laid out side by side; export only the first visual,
 # keeping all source LODs preserved in the original archive staging directory.
 bpy.ops.object.select_all(action="DESELECT")
 for o in meshes:
  transform=o.matrix_world.copy();o.parent=None;o.matrix_world=transform
  o.select_set(True)
 bpy.context.view_layer.objects.active=meshes[0]
 bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
 if len(meshes)>1:bpy.ops.object.join()
 obj=bpy.context.object;obj.name="BCMM_"+name
 obj.rotation_mode="XYZ"
 if item.get("decimate_ratio",1)<1:
  modifier=obj.modifiers.new("BCMM_RuntimeReduction","DECIMATE");modifier.ratio=item["decimate_ratio"];bpy.ops.object.modifier_apply(modifier=modifier.name)
 # Bring the horizontal morphine injector onto the same long axis as the other injectors.
 if name=="Morphine":obj.rotation_euler[1]=math.pi/2;bpy.ops.object.transform_apply(location=False,rotation=True,scale=False)
 if item.get("rotation_euler_degrees"):
  obj.rotation_euler=[math.radians(v) for v in item["rotation_euler_degrees"]]
  bpy.ops.object.transform_apply(location=False,rotation=True,scale=False)
 if item.get("align_long_axis")=="Y":
  longest_axis=max(range(3),key=lambda axis:max(v.co[axis] for v in obj.data.vertices)-min(v.co[axis] for v in obj.data.vertices))
  if longest_axis==2:obj.rotation_euler[0]=math.pi/2
  elif longest_axis==0:obj.rotation_euler[2]=math.pi/2
  bpy.ops.object.transform_apply(location=False,rotation=True,scale=False)
  bpy.context.view_layer.update()
 points=[obj.matrix_world@Vector(v) for v in obj.bound_box]
 low=Vector([min(v[k] for v in points) for k in range(3)]);high=Vector([max(v[k] for v in points) for k in range(3)]);centre=(low+high)*.5
 scale=item["desired_longest_metres"]/max(high-low)
 offset=Vector(item.get("origin_offset",[0,0,0]))
 for v in obj.data.vertices:v.co=(v.co-centre)*scale+offset
 obj.data.update()
 # Native material names must match the source names in MaterialAssigns.
 for m in obj.data.materials:
  if not m:continue
  assignment=next((a for a in item["materials"] if a["source"]==m.name),None)
  if assignment:
   m.use_nodes=True;bs=m.node_tree.nodes.get("Principled BSDF")
   for link in list(m.node_tree.links):
    if link.to_node==bs and link.to_socket.name=="Base Color":m.node_tree.links.remove(link)
   node=m.node_tree.nodes.new("ShaderNodeTexImage");node.image=bpy.data.images.load(str(root/assignment["base_preview"]),check_existing=True)
   m.node_tree.links.new(node.outputs["Color"],bs.inputs["Base Color"])
   bs.inputs["Metallic"].default_value=.05;bs.inputs["Roughness"].default_value=.5
   m.name="BCMM_"+name+"_M"+str(item["materials"].index(assignment))
 if item.get("accent_albedo"):
  # Keep the original neutral body; user-edited colour is used only on the moulded ends.
  accent=obj.data.materials[0].copy();accent.name="BCMM_"+name+"_M1";accent.use_nodes=True
  shader=accent.node_tree.nodes.get("Principled BSDF")
  for link in list(accent.node_tree.links):
   if link.to_node==shader and link.to_socket.name=="Base Color":accent.node_tree.links.remove(link)
  node=accent.node_tree.nodes.new("ShaderNodeTexImage");node.image=bpy.data.images.load(str(root/item["materials"][1]["base_preview"]),check_existing=True)
  accent.node_tree.links.new(node.outputs["Color"],shader.inputs["Base Color"])
  obj.data.materials.append(accent);accent_index=len(obj.data.materials)-1
  painted=0
  for polygon in obj.data.polygons:
   accent_axis=1 if item.get("align_long_axis")=="Y" else 2
   z=sum(obj.data.vertices[index].co[accent_axis] for index in polygon.vertices)/len(polygon.vertices)
   if z>.055 or z<-.0325:polygon.material_index=accent_index;painted+=1
  if painted==0:raise RuntimeError("No end-piece faces found for "+name)
  print("BCMM_CAP_VARIANT",name,"faces",painted,flush=True)
 # Preserve the grip offset while turning the injection end upward. Apply after
 # material selection so coloured plunger/cap faces retain their identities.
 if item.get('reverse_long_axis',False):
  reverse=Matrix.Rotation(math.pi,4,'X')
  for vertex in obj.data.vertices:vertex.co=reverse@(vertex.co-offset)+offset
  obj.data.update()
 bpy.context.view_layer.update()
 dimensions=obj.dimensions.copy()
 bpy.ops.mesh.primitive_cube_add(size=1,location=offset);collision=bpy.context.object;collision.name="UBX_BCMM_"+name;collision.dimensions=dimensions+Vector((.002,.002,.002));bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
 collision.hide_render=True
 bpy.ops.object.select_all(action="DESELECT");obj.select_set(True);collision.select_set(True);bpy.context.view_layer.objects.active=obj
 extraCollision=None
 if name=="FieldStretcher":
  bpy.ops.mesh.primitive_cube_add(size=1,location=offset);extraCollision=bpy.context.object;extraCollision.name="UBX_StretcherCarry";extraCollision.dimensions=Vector((.5,.65,.08));bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);extraCollision.hide_render=True
  bpy.ops.object.select_all(action="DESELECT");obj.select_set(True);collision.select_set(True);extraCollision.select_set(True);bpy.context.view_layer.objects.active=obj
 output=folder/("BCMM_"+name+".fbx")
 bpy.ops.export_scene.fbx(filepath=str(output),use_selection=True,object_types={"MESH"},axis_forward="-Z",axis_up="Y",apply_unit_scale=True,bake_space_transform=False,mesh_smooth_type="FACE",use_mesh_modifiers=True,add_leaf_bones=False,bake_anim=False,path_mode="STRIP")
 assignments=" ".join('MaterialAssignClass { SourceMaterial "'+"BCMM_"+name+"_M"+str(item["materials"].index(a))+'" AssignedMaterial "'+a["material"]+'" }' for a in item["materials"])
 meta='MetaFileClass { Name "'+item["model"]+'" Configurations { FBXResourceClass PC { MaterialAssigns { '+assignments+' } GeometryParams { GeometryParam '+collision.name+' { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } Mass 0.1 Margin 0.001 } } } FBXResourceClass HEADLESS : PC {} } }'
 if extraCollision:meta=meta.replace('Mass 0.1 Margin 0.001 } } } FBXResourceClass HEADLESS','Mass 0.1 Margin 0.001 } GeometryParam UBX_StretcherCarry { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } Mass 0.1 Margin 0.001 } } } FBXResourceClass HEADLESS')
 output.with_suffix(".xob.meta").write_text(meta,encoding="utf-8")
 # Studio preview, using the same packed albedo as native import.
 for other in list(bpy.context.scene.objects):
  if other not in (obj,collision,extraCollision):bpy.data.objects.remove(other,do_unlink=True)
 bpy.ops.object.camera_add(location=Vector((1.4,-2,1.1))*max(dimensions)*2)
 camera=bpy.context.object;camera.rotation_euler=(-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.type='ORTHO';camera.data.ortho_scale=max(dimensions)*1.65;bpy.context.scene.camera=camera
 for loc,energy,size in [((2,-3,4),450,3),((-3,-1,2),250,3)]:
  bpy.ops.object.light_add(type='AREA',location=Vector(loc)*max(dimensions));light=bpy.context.object;light.data.energy=energy*max(dimensions)**2;light.data.shape='DISK';light.data.size=size*max(dimensions);light.rotation_euler=(-light.location).to_track_quat('-Z','Y').to_euler()
 scene=bpy.context.scene;scene.world=bpy.data.worlds.new("BCMM_StudioWorld");scene.render.engine='CYCLES';scene.cycles.samples=16;scene.render.resolution_x=600;scene.render.resolution_y=600;scene.render.resolution_percentage=100;scene.world.color=(.15,.15,.15)
 scene.render.image_settings.file_format='PNG';scene.render.filepath=str(root/'.checks'/('asset-preview-'+name+'.png'));bpy.ops.render.render(write_still=True)
 bpy.ops.wm.save_as_mainfile(filepath=str(folder/("BCMM_"+name+".blend")))
 print("BCMM_ASSET_EXPORTED",name,"dimensions",list(dimensions),flush=True)
