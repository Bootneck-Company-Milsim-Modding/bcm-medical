import bpy
from pathlib import Path
root=Path(__file__).resolve().parents[2];folder=root/'Tests/Runtime/Assets';folder.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
for name in ['BCMM_TestDeck','UBX_TestDeck']:
 bpy.ops.mesh.primitive_cube_add(size=1,location=(0,0,-.1));o=bpy.context.object;o.name=name;o.dimensions=(12,12,.2);bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
bpy.ops.object.select_all(action='SELECT')
bpy.ops.export_scene.fbx(filepath=str(folder/'BCMM_TestDeck.fbx'),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',add_leaf_bones=False,bake_anim=False)
(folder/'BCMM_TestDeck.xob.meta').write_text('MetaFileClass { Name "{B6C4ED1CA1262115}Assets/BCMM_TestDeck.xob" Configurations { FBXResourceClass PC { GeometryParams { GeometryParam UBX_TestDeck { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } } } } FBXResourceClass HEADLESS : PC {} } }',encoding='utf-8')
