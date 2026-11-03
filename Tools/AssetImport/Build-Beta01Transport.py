"""Compact the inventory sling and author an original cosmetic tow line."""
import bpy, json, hashlib, math
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[2]
folder=root/'Assets/Medical/WeaponSling'
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(root/'Source/Equipment/BlackBungeeSling/BCM_Black_TwoHook_BungeeSling_Inventory.fbx'),use_anim=False)
meshes=[o for o in bpy.context.scene.objects if o.type=='MESH' and not o.name.startswith(('UBX','UCX'))]
for o in meshes:
    transform=o.matrix_world.copy();o.parent=None;o.matrix_world=transform
bpy.ops.object.select_all(action='DESELECT')
for o in meshes:o.select_set(True)
bpy.context.view_layer.objects.active=meshes[0]
bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
if len(meshes)>1:bpy.ops.object.join()
obj=bpy.context.object;obj.name='BCMM_WeaponSling'
points=[Vector(v) for v in obj.bound_box]
lo=Vector([min(v[k] for v in points) for k in range(3)]);hi=Vector([max(v[k] for v in points) for k in range(3)])
centre=(lo+hi)/2;factor=.14/max(hi-lo)
for v in obj.data.vertices:v.co=(v.co-centre)*factor
for i,material in enumerate(obj.data.materials):
    if material:material.name='BCMM_WeaponSling_M'+str(i)
obj.data.update();bpy.context.view_layer.update()
dims=obj.dimensions.copy()
bpy.ops.mesh.primitive_cube_add(size=1);collision=bpy.context.object;collision.name='UBX_BCMM_WeaponSling';collision.dimensions=dims+Vector((.002,.002,.002));collision.hide_render=True
bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
bpy.ops.object.select_all(action='DESELECT');obj.select_set(True);collision.select_set(True);bpy.context.view_layer.objects.active=obj
bpy.ops.export_scene.fbx(filepath=str(folder/'BCMM_WeaponSling.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',apply_unit_scale=True,mesh_smooth_type='FACE',add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
bpy.ops.wm.save_as_mainfile(filepath=str(folder/'BCMM_WeaponSling.blend'))
report=json.loads((folder/'import-manifest.json').read_text());report['dimensionsMetres']=list(dims);report['beta01Compact']=True;(folder/'import-manifest.json').write_text(json.dumps(report,indent=2)+'\n')
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.mesh.primitive_cylinder_add(vertices=12,radius=.006,depth=1,rotation=(math.pi/2,0,0))
rope=bpy.context.object;rope.name='BCMM_DragRopeLine'
bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
material=bpy.data.materials.new('BCMM_RopeLine');rope.data.materials.append(material)
line=root/'Assets/Medical/DragRopeLine';line.mkdir(exist_ok=True)
bpy.ops.export_scene.fbx(filepath=str(line/'BCMM_DragRopeLine.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',apply_unit_scale=True,mesh_smooth_type='FACE',add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
def ref(path):return '{'+hashlib.sha256(('BCMM/Beta01/'+path).encode()).hexdigest()[:16].upper()+'}'+path
model='Assets/Medical/DragRopeLine/BCMM_DragRopeLine.xob'
(root/(model+'.meta')).write_text('MetaFileClass { Name "'+ref(model)+'" Configurations { FBXResourceClass PC { MaterialAssigns { MaterialAssignClass { SourceMaterial "BCMM_RopeLine" AssignedMaterial "{2F5FF9812B71E31B}Assets/Medical/WeaponSling/Data/WeaponSling_0.emat" } } } FBXResourceClass HEADLESS : PC {} } }')
prefab='Prefabs/Medical/Applied/BCMM_DragRopeLine.et';(root/prefab).parent.mkdir(parents=True,exist_ok=True)
(root/prefab).write_text('GenericEntity { components { MeshObject { Object "'+ref(model)+'" } } }')
(root/(prefab+'.meta')).write_text('MetaFileClass { Name "'+ref(prefab)+'" Configurations { EntityTemplateResourceClass PC {} EntityTemplateResourceClass HEADLESS : PC {} } }')
print('BCMM_BETA_TRANSPORT sling dimensions',list(dims),'rope prefab',ref(prefab))
