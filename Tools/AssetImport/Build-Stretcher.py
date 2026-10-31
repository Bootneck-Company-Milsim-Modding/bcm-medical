"""Original BCM stationary field stretcher: metres, reusable source, no vendor assets."""
from pathlib import Path
import bpy
root=Path(__file__).resolve().parents[2]
folder=root/'Assets/Medical/Stretcher';folder.mkdir(parents=True,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
materials=[]
for name,color in [('Canvas',(.13,.17,.10,1)),('Frame',(.20,.23,.20,1)),('Straps',(.055,.065,.045,1))]:
 m=bpy.data.materials.new('BCMM_Stretcher_'+name);m.diffuse_color=color;materials.append(m)
parts=[]
def box(name,pos,size,material):
 bpy.ops.mesh.primitive_cube_add(size=1,location=pos);o=bpy.context.object;o.name=name;o.dimensions=size;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(materials[material]);parts.append(o);return o
box('Canvas',(0,0,.35),(.60,1.90,.045),0)
for x in [-.34,.34]:
 box('Rail',(x,0,.34),(.045,2.25,.045),1)
 for y in [-.72,.72]:box('Leg',(x,y,.17),(.035,.035,.34),1)
for y in [-.72,.72]:box('Crossmember',(0,y,.08),(.68,.035,.035),1)
for y in [-.48,.4]:box('Restraint',(0,y,.376),(.60,.06,.009),2)
box('Pillow',(0,.73,.39),(.40,.28,.06),0)
bpy.ops.object.select_all(action='DESELECT')
for o in parts:o.select_set(True)
bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();visual=bpy.context.object;visual.name='BCMM_Stretcher'
# Stable origin at the ground below the bed centre.
bpy.context.scene.cursor.location=(0,0,0);bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
collision=box('UBX_Stretcher',(0,0,.34),(.67,1.94,.07),1);collision.hide_render=True
bpy.ops.object.select_all(action='SELECT')
bpy.ops.export_scene.fbx(filepath=str(folder/'BCMM_Stretcher.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
bpy.ops.wm.save_as_mainfile(filepath=str(folder/'BCMM_Stretcher.blend'))
assigns=[]
for i,(name,color) in enumerate([('Canvas',(.13,.17,.10)),('Frame',(.20,.23,.20)),('Straps',(.055,.065,.045))]):
 guid='6AB4B3F3FD21A12'+str(i)
 path='Assets/Medical/Stretcher/'+name+'.emat'
 (root/path).write_text('MatPBRBasic { Color '+ ' '.join(map(str,color))+' 1 }',encoding='utf-8')
 (root/(path+'.meta')).write_text('MetaFileClass { Name "{'+guid+'}'+path+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding='utf-8')
 assigns.append('MaterialAssignClass { SourceMaterial "BCMM_Stretcher_'+name+'" AssignedMaterial "{'+guid+'}'+path+'" }')
(folder/'BCMM_Stretcher.xob.meta').write_text('MetaFileClass { Name "{6AB4B3F3FD21A100}Assets/Medical/Stretcher/BCMM_Stretcher.xob" Configurations { FBXResourceClass PC { MaterialAssigns { '+' '.join(assigns)+' } GeometryParams { GeometryParam UBX_Stretcher { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } } } } FBXResourceClass HEADLESS : PC {} } }',encoding='utf-8')
print('Original BCM stretcher exported')
