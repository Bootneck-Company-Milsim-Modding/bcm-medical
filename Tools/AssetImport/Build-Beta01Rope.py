"""Original compact coil; native previews ignore entity-level scale."""
import bpy, math, hashlib
from pathlib import Path
root=Path(__file__).resolve().parents[2]
def ref(path):return '{'+hashlib.sha256(('BCMM/Beta01/'+path).encode()).hexdigest()[:16].upper()+'}'+path
bpy.ops.wm.read_factory_settings(use_empty=True)
curve=bpy.data.curves.new('BCMM_CompactRope','CURVE');curve.dimensions='3D';curve.bevel_depth=.003;curve.bevel_resolution=2;curve.resolution_u=2
spline=curve.splines.new('POLY');count=480;spline.points.add(count-1)
for i,point in enumerate(spline.points):
    t=i/(count-1);angle=t*math.tau*4.5;radius=.043+.022*t
    point.co=(math.cos(angle)*radius, math.sin(angle)*radius*.85, .004*math.sin(angle*.5),1)
obj=bpy.data.objects.new('BCMM_CompactDragRope',curve);bpy.context.collection.objects.link(obj);obj.select_set(True);bpy.context.view_layer.objects.active=obj
bpy.ops.object.convert(target='MESH');obj=bpy.context.object
mat=bpy.data.materials.new('BCMM_RopeCoil');obj.data.materials.append(mat)
folder=root/'Assets/Medical/CompactDragRope';folder.mkdir(exist_ok=True)
bpy.ops.mesh.primitive_cube_add(size=1);collision=bpy.context.object;collision.name='UBX_BCMM_CompactDragRope';collision.dimensions=(.14,.12,.018);collision.hide_render=True
bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
obj.select_set(True)
bpy.ops.export_scene.fbx(filepath=str(folder/'BCMM_CompactDragRope.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',apply_unit_scale=True,mesh_smooth_type='FACE',add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
bpy.ops.wm.save_as_mainfile(filepath=str(folder/'BCMM_CompactDragRope.blend'))
model='Assets/Medical/CompactDragRope/BCMM_CompactDragRope.xob'
(root/(model+'.meta')).write_text('MetaFileClass { Name "'+ref(model)+'" Configurations { FBXResourceClass PC { MaterialAssigns { MaterialAssignClass { SourceMaterial "BCMM_RopeCoil" AssignedMaterial "'+ref('Assets/Medical/CompactDragRope/RopeCoil.emat')+'" } } GeometryParams { GeometryParam UBX_BCMM_CompactDragRope { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } Mass 0 Margin 0 } } } FBXResourceClass HEADLESS : PC {} } }')
print(ref(model))
