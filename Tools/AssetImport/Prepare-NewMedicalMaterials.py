from pathlib import Path
from PIL import Image
import json,hashlib
root=Path(__file__).resolve().parents[2];src=root/'.checks/vendor-medical-wave2';path=root/'Tools/AssetImport/MedicalAssets.json';items=json.loads(path.read_text(encoding='utf-8'))
choices={
 'Splint':('Splint/SM_MedicalSplinter.fbx','Splint/Textures/T_MedicalSplint_BC.tga','Splint/Textures/T_MedicalSplint_N.tga',.22),
 'SurgeryKit':('SurgeryKit/Medical_SurvivalKit/SM_SurvivalKit.fbx','SurgeryKit/Medical_SurvivalKit/T_SurvivalKit_BC.tga','SurgeryKit/Medical_SurvivalKit/T_SurvivalKit_N.tga',.35),
 'SutureKit':('Suture Kit/Medical_MilitaryHealthPouch/SM_Medical_MilitaryHealthPouch.fbx','Suture Kit/Medical_MilitaryHealthPouch/SM_MilitaryHealthPouch_BC.tga','Suture Kit/Medical_MilitaryHealthPouch/T_MilitaryHealthPouch_N.tga',.18)}
def ref(p):return '{'+hashlib.sha256(('BCMM/vendor/'+p).encode()).hexdigest()[:16].upper()+'}'+p
for name,(model,albedo,normal,size) in choices.items():
 folder=root/'Assets/Medical'/name/'Data';folder.mkdir(parents=True,exist_ok=True)
 base=Image.open(src/albedo).convert('RGB');norm=Image.open(src/normal).convert('RGB').resize(base.size)
 rough=Image.new('L',base.size,190);metal=Image.new('L',base.size,0);ao=Image.new('L',base.size,255)
 if name=='Splint':
  rough=Image.open(src/'Splint/Textures/T_MedicalSplint_R.tga').convert('L').resize(base.size)
  metal=Image.open(src/'Splint/Textures/T_MedicalSplint_M.tga').convert('L').resize(base.size)
  ao=Image.open(src/'Splint/Textures/T_MedicalSplint_AO.tga').convert('L').resize(base.size)
 Image.merge('RGBA',(*base.split(),rough)).save(folder/(name+'_BCR.png'))
 Image.merge('RGBA',(*norm.split()[:2],metal,ao)).save(folder/(name+'_NMO.png'))
 relative=folder.relative_to(root).as_posix();matpath=relative+'/'+name+'.emat'
 for suffix,preset in [('BCR','{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf'),('NMO','{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf')]:
  texture=relative+'/'+name+'_'+suffix+'.edds';(root/(texture+'.meta')).write_text('MetaFileClass { Name "'+ref(texture)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding='utf-8')
 (root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(relative+'/'+name+'_BCR.edds')+'" NMOMap "'+ref(relative+'/'+name+'_NMO.edds')+'" }',encoding='utf-8')
 (root/(matpath+'.meta')).write_text('MetaFileClass { Name "'+ref(matpath)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding='utf-8')
 item=dict(name=name,source=model,source_root='.checks/vendor-medical-wave2',desired_longest_metres=size,materials=[dict(source='Mat_UVChecker',material=ref(matpath),base_preview=relative+'/'+name+'_BCR.png')],model=ref('Assets/Medical/'+name+'/BCMM_'+name+'.xob'))
 items=[i for i in items if i['name']!=name];items.append(item)
 prefab=root/'Prefabs/Items'/('BCMM_'+name+'.et');text=prefab.read_text(encoding='utf-8')
 if 'MeshObject "{51F66EC3C5BC700D}"' not in text:text=text.replace(' components {',' components {\n  MeshObject "{51F66EC3C5BC700D}" { Object "'+item['model']+'" }',1)
 prefab.write_text(text,encoding='utf-8');print('Packed',name)
path.write_text(json.dumps(items,indent=2)+'\n',encoding='utf-8')
