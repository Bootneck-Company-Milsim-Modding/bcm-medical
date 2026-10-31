"""Original applied equipment meshes; vendor assets remain untouched."""
import bpy,math,json,hashlib
from pathlib import Path
root=Path(__file__).resolve().parents[2]
manifest=json.loads((root/'Tools/AssetImport/MedicalAssets.json').read_text())
def material(name,color):
 m=bpy.data.materials.new(name);m.diffuse_color=(*color,1);m.use_nodes=True;m.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value=(*color,1);return m
def cube(name,loc,scale,mat):
 bpy.ops.mesh.primitive_cube_add(size=1,location=loc);o=bpy.context.object;o.name=name;o.dimensions=scale;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(mat)
 bevel=o.modifiers.new('Soft edges','BEVEL');bevel.width=.002;bevel.segments=2;bpy.context.view_layer.objects.active=o;bpy.ops.object.modifier_apply(modifier=bevel.name)
 return o
def ring(name,radius,width,mat):
 verts=[];faces=[];n=32
 for r,z in [(radius,-width/2),(radius,width/2),(radius+.003,-width/2),(radius+.003,width/2)]:
  verts.extend((r*math.cos(i*2*math.pi/n),r*math.sin(i*2*math.pi/n),z) for i in range(n))
 for a,b in [(0,n),(2*n,3*n),(0,2*n),(n,3*n)]:
  for i in range(n):j=(i+1)%n;faces.append((a+i,a+j,b+j,b+i))
 mesh=bpy.data.meshes.new(name);mesh.from_pydata(verts,[],faces);mesh.update();o=bpy.data.objects.new(name,mesh);bpy.context.collection.objects.link(o);o.data.materials.append(mat)
 return o
def save(name):
 for o in bpy.context.scene.objects:
  if o.type=='MESH' and not o.data.uv_layers:
   bpy.context.view_layer.objects.active=o;o.select_set(True);bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project();bpy.ops.object.mode_set(mode='OBJECT');o.select_set(False)
 folder=root/'Source/MedicalModels'/name;folder.mkdir(parents=True,exist_ok=True);bpy.ops.wm.save_as_mainfile(filepath=str(folder/('BCMM_'+name+'.blend')))
bpy.ops.wm.read_factory_settings(use_empty=True)
orange=material('BCMM_SplintOrange',(.78,.24,.035));blue=material('BCMM_SplintFoam',(.055,.18,.32));black=material('BCMM_Nylon',(.025,.03,.025))
# Open U-shaped moulded board with foam lining and two fastening straps.
for name,radius,mat in [('Moulded board',.069,orange),('Foam lining',.065,blue)]:
 verts=[];faces=[];n=20
 for z in (-.215,.215):
  for i in range(n+1):
   a=math.pi*.12+i*math.pi*.76/n;verts.append((radius*math.cos(a),-radius*math.sin(a),z))
 for i in range(n):faces.append((i,i+1,i+n+2,i+n+1))
 mesh=bpy.data.meshes.new(name);mesh.from_pydata(verts,[],faces);mesh.update();o=bpy.data.objects.new(name,mesh);bpy.context.collection.objects.link(o);o.data.materials.append(mat)
 solid=o.modifiers.new('Board thickness','SOLIDIFY');solid.thickness=.002;bpy.context.view_layer.objects.active=o;bpy.ops.object.modifier_apply(modifier=solid.name)
for z in (-.135,.135):
 o=ring('Fastening strap',.073,.025,black);o.location.z=z;cube('Strap buckle',(.073,0,z),(.012,.035,.035),black)
save('AppliedSplint')
bpy.ops.wm.read_factory_settings(use_empty=True)
black=material('BCMM_Nylon',(.025,.03,.025));metal=material('BCMM_Windlass',(.09,.10,.085));red=material('BCMM_ReleaseTab',(.55,.025,.018))
ring('Tourniquet webbing',.058,.029,black);cube('Windlass receiver',(0,-.061,0),(.04,.015,.035),black)
bpy.ops.mesh.primitive_cylinder_add(vertices=16,radius=.006,depth=.10,location=(0,-.076,.006),rotation=(0,math.pi/2,0));bpy.context.object.name='Windlass';bpy.context.object.data.materials.append(metal)
cube('Release tab',(.034,-.065,.022),(.025,.012,.023),red);save('AppliedTourniquet')
print('BCMM_APPLIED_MESH_SOURCES_READY')