"""Export the approved original sling and native PBR resources. Blender 4.5."""
import bpy,numpy as np,hashlib,json
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[2]
folder=root/'Assets/Medical/WeaponSling';data=folder/'Data';data.mkdir(parents=True,exist_ok=True)
def guid(path):return hashlib.sha256(('BCMM/WeaponSling/'+path).encode()).hexdigest()[:16].upper()
def ref(path):return '{'+guid(path)+'}'+path
def meta(path,kind):
    (root/(path+'.meta')).write_text('MetaFileClass { Name "'+ref(path)+'" Configurations { '+kind+' PC {} '+kind+' HEADLESS : PC {} } }',encoding='utf-8')
def save_tex(path,pixels,normal=False):
    h,w=pixels.shape[:2];im=bpy.data.images.new(Path(path).stem,width=w,height=h,alpha=True)
    if normal:im.colorspace_settings.name='Non-Color'
    im.pixels.foreach_set(pixels.astype(np.float32).ravel());im.filepath_raw=str(root/path);im.file_format='PNG';im.save()
    tex=path[:-4]+'.edds';preset='{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf' if normal else '{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf'
    (root/(tex+'.meta')).write_text('MetaFileClass { Name "'+ref(tex)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding='utf-8')
    return ref(tex)
def pixels(im):
    a=np.empty(im.size[0]*im.size[1]*4,np.float32);im.pixels.foreach_get(a);return a.reshape(im.size[1],im.size[0],4)
source=bpy.data.objects['BCM_Black_TwoHook_BungeeSling_InventoryMesh']
obj=source.copy();obj.data=source.data.copy();bpy.context.scene.collection.objects.link(obj)
obj.hide_viewport=False;obj.hide_render=False;obj.hide_set(False)
transform=source.matrix_world.copy();obj.parent=None;obj.matrix_world=transform
obj.name='BCMM_WeaponSling'
bpy.ops.object.select_all(action='DESELECT');obj.select_set(True);bpy.context.view_layer.objects.active=obj
bpy.ops.object.transform_apply(location=True,rotation=True,scale=True)
points=[Vector(v) for v in obj.bound_box]
lo=Vector([min(v[k] for v in points) for k in range(3)]);hi=Vector([max(v[k] for v in points) for k in range(3)])
centre=(lo+hi)*.5
for v in obj.data.vertices:v.co-=centre
obj.data.update()
assign=[]
for i,mat in enumerate(obj.data.materials):
    if not mat:continue
    p=mat.node_tree.nodes.get('Principled BSDF');iscloth='Black woven nylon' in mat.name
    size=2048 if iscloth else 16
    bcr=np.ones((size,size,4),np.float32);nmo=np.ones_like(bcr)
    if iscloth:
        nodes=[n for n in mat.node_tree.nodes if n.type=='TEX_IMAGE' and n.image]
        bc=next(n.image for n in nodes if 'woven colour' in n.image.name)
        rm=next(n.image for n in nodes if 'roughness' in n.image.name)
        nm=next(n.image for n in nodes if 'tangent normal' in n.image.name)
        bcr[:,:,:3]=pixels(bc)[:,:,:3];bcr[:,:,3]=pixels(rm)[:,:,0]
        xyz=pixels(nm)[:,:,:3]*2-1;xyz[:,:,:2]*=1.15;xyz/=np.linalg.norm(xyz,axis=-1)[:,:,None]
        nmo[:,:,0]=xyz[:,:,0]*.5+.5;nmo[:,:,1]=.5-xyz[:,:,1]*.5
    else:
        bcr[:,:,:3]=p.inputs['Base Color'].default_value[:3];bcr[:,:,3]=p.inputs['Roughness'].default_value
        nmo[:,:,0:2]=.5
    nmo[:,:,2]=p.inputs['Metallic'].default_value;nmo[:,:,3]=1
    stem='Assets/Medical/WeaponSling/Data/WeaponSling_'+str(i)
    bcrref=save_tex(stem+'_BCR.png',bcr);nmoref=save_tex(stem+'_NMO.png',nmo,True)
    emat=stem+'.emat';(root/emat).write_text('MatPBRBasic { BCRMap "'+bcrref+'" NMOMap "'+nmoref+'" }',encoding='utf-8');meta(emat,'EMATResourceClass')
    native=mat.copy();native.name='BCMM_WeaponSling_M'+str(i);obj.data.materials[i]=native
    assign.append('MaterialAssignClass { SourceMaterial "'+native.name+'" AssignedMaterial "'+ref(emat)+'" }')
bpy.context.view_layer.update();dims=obj.dimensions.copy()
bpy.ops.mesh.primitive_cube_add(size=1);collision=bpy.context.object;collision.name='UBX_BCMM_WeaponSling';collision.dimensions=dims+Vector((.002,.002,.002))
bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
bpy.ops.object.select_all(action='DESELECT');obj.select_set(True);collision.select_set(True);bpy.context.view_layer.objects.active=obj
fbx=folder/'BCMM_WeaponSling.fbx'
bpy.ops.export_scene.fbx(filepath=str(fbx),use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',apply_unit_scale=True,mesh_smooth_type='FACE',add_leaf_bones=False,bake_anim=False,path_mode='STRIP')
model='Assets/Medical/WeaponSling/BCMM_WeaponSling.xob'
fbx.with_suffix('.xob.meta').write_text('MetaFileClass { Name "'+ref(model)+'" Configurations { FBXResourceClass PC { MaterialAssigns { '+' '.join(assign)+' } GeometryParams { GeometryParam UBX_BCMM_WeaponSling { SurfaceProperties { "{13581C35484F3000}Common/Materials/Game/fabric.gamemat" } Mass 0.2 Margin 0.001 } } } FBXResourceClass HEADLESS : PC {} } }',encoding='utf-8')
prefab='Prefabs/Items/BCMM_WeaponSling.et'
(root/prefab).write_text('''GenericEntity : "{C95E11C60810F432}Prefabs/Items/Core/Item_Base.et" {
 ID "'''+guid(prefab+'/entity')+'''"
 components {
  MeshObject "{A8DDA767AF0E0FA7}" { Object "'''+ref(model)+'''" }
  RigidBody "{C83410D6ED3C2D9B}" { Mass 0.2 }
  InventoryItemComponent "{5222EB4D0C73006B}" {
   Attributes SCR_ItemAttributeCollection "{5222EB4D0A2B466B}" {
    ItemDisplayName SCR_InventoryUIInfo "{5222EB4D07D865FA}" {
     Name "Weapon sling"
     Description "Black two-hook bungee sling. Keep in inventory; no equipping required."
    }
    ItemPhysAttributes ItemPhysicalAttributes PhysicalAttributes {
     Weight 0.2
     SizeSetupStrategy Manual
     ItemDimensions 18 12 4
     ItemVolume 500
    }
    CommonItemType "EQUIPMENT"
   }
  }
 }
}
''',encoding='utf-8');meta(prefab,'EntityTemplateResourceClass')
report={'model':ref(model),'prefab':ref(prefab),'materials':len(assign),'triangles':len(obj.data.polygons),'dimensionsMetres':list(dims),'staticInventoryOnly':True,'source':'Source/Equipment/BlackBungeeSling/BCM_Black_TwoHook_BungeeSling.blend'}
(folder/'import-manifest.json').write_text(json.dumps(report,indent=2))
print('BCMM_SLING_NATIVE_EXPORT',json.dumps(report),flush=True)
