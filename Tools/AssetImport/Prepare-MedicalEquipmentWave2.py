from pathlib import Path
from PIL import Image
import json,hashlib,re
root=Path(__file__).resolve().parents[2];src=root/'.checks/vendor-medical-wave2';path=root/'Tools/AssetImport/MedicalAssets.json';items=json.loads(path.read_text(encoding='utf-8'))
def ref(p):return '{'+hashlib.sha256(('BCMM/vendor/'+p).encode()).hexdigest()[:16].upper()+'}'+p
def material(name,index,source,albedo,normal=None,rough=None,metal=None,ao=None):
 folder=root/'Assets/Medical'/name/'Data';folder.mkdir(parents=True,exist_ok=True)
 base=Image.open(src/albedo).convert('RGB');base.thumbnail((2048,2048));size=base.size
 def channel(file,default):return Image.open(src/file).convert('L').resize(size) if file else Image.new('L',size,default)
 norm=Image.open(src/normal).convert('RGB').resize(size) if normal else Image.new('RGB',size,(128,128,255))
 stem=name+'_'+str(index);Image.merge('RGBA',(*base.split(),channel(rough,180))).save(folder/(stem+'_BCR.png'))
 Image.merge('RGBA',(*norm.split()[:2],channel(metal,0),channel(ao,255))).save(folder/(stem+'_NMO.png'))
 relative=folder.relative_to(root).as_posix()+'/'+stem
 for suffix,preset in [('BCR','{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf'),('NMO','{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf')]:
  texture=relative+'_'+suffix+'.edds';(root/(texture+'.meta')).write_text('MetaFileClass { Name "'+ref(texture)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding='utf-8')
 matpath=relative+'.emat';(root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(relative+'_BCR.edds')+'" NMOMap "'+ref(relative+'_NMO.edds')+'" }',encoding='utf-8')
 (root/(matpath+'.meta')).write_text('MetaFileClass { Name "'+ref(matpath)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding='utf-8')
 return dict(source=source,material=ref(matpath),base_preview=relative+'_BCR.png')
choices=[]
choices.append(dict(name='FieldStretcher',source='Stretcher/Stretcher.fbx',desired_longest_metres=2.4,origin_offset=[0,0,.35],materials=[material('FieldStretcher',0,'Stretcher','Stretcher/FG+Stretcher+_Stretcher_albedo.jpg.jpg/FG Stretcher _Stretcher_albedo.jpg.jpg')]))
prefix='AED Use Pads Only/Textures/Textures/AED_low_'
choices.append(dict(name='AEDPads',source='AED Use Pads Only/AED.fbx',desired_longest_metres=.24,include_objects=['AED_Electrode_1','AED_Electrode_2'],materials=[material('AEDPads',0,'BCMM_Default',prefix+'BaseMap.png',prefix+'Normal.png',prefix+'Roughness.png',prefix+'Metallic.png',prefix+'AO.png')]))
materials=[]
for i,(mat,tile) in enumerate([('Plastic','1001'),('Transparent_part','1002')]):
 prefix='BVM/Texture/Texture/'+mat+'_';suffix='_'+tile+'.png'
 materials.append(material('BVM',i,mat,prefix+'Base_color'+suffix,prefix+'Normal'+suffix,prefix+'Roughness'+suffix,prefix+'Metallic'+suffix,prefix+'Mixed_AO'+suffix))
choices.append(dict(name='BVM',source='BVM/Ambu+bag.fbx',desired_longest_metres=.4,materials=materials))
prefix='Monitor/Monitor_PBR/Monitor_PBR_'
choices.append(dict(name='Monitor',source='Monitor/Monitor(1).fbx',desired_longest_metres=.28,materials=[material('Monitor',0,'Monitor',prefix+'Diffuse.png',prefix+'Normal.png',prefix+'Roughness.png')]))
for item in choices:
 item['source_root']='.checks/vendor-medical-wave2';item['model']=ref('Assets/Medical/'+item['name']+'/BCMM_'+item['name']+'.xob')
 items=[i for i in items if i['name']!=item['name']];items.append(item)
 if item['name']=='FieldStretcher':
  prefab=root/'Prefabs/Items/BCMM_FieldSurgeryStation.et';s=prefab.read_text(encoding='utf-8');s=re.sub(r'(MeshObject "\{6AB4B3F3FD218313\}" \{ Object ")[^"]+(" \})',lambda m:m[1]+item['model']+m[2],s);prefab.write_text(s,encoding='utf-8')
 else:
  prefab=root/'Prefabs/Items'/('BCMM_'+item['name']+'.et');s=prefab.read_text(encoding='utf-8')
  if 'MeshObject "{51F66EC3C5BC700D}"' not in s:s=s.replace(' components {',' components {\n  MeshObject "{51F66EC3C5BC700D}" { Object "'+item['model']+'" }',1)
  prefab.write_text(s,encoding='utf-8')
 print('Packed supplied',item['name'])
path.write_text(json.dumps(items,indent=2)+'\n',encoding='utf-8')
