from pathlib import Path
from PIL import Image
import json,hashlib
root=Path(__file__).resolve().parents[2];src=root/'.checks/vendor-medical-wave2';path=root/'Tools/AssetImport/MedicalAssets.json';items=json.loads(path.read_text(encoding='utf-8'));inspection=json.loads((root/'Docs/NEW-MEDICAL-MODEL-INSPECTION.json').read_text(encoding='utf-8'))
def ref(p):return '{'+hashlib.sha256(('BCMM/vendor/'+p).encode()).hexdigest()[:16].upper()+'}'+p
choices={'VascularAccess':('VascularAccess/TS_MDL_Cannula_V01_01.fbx',.045,.35),'Suction':('Suction/VAC+Device+model.fbx',.35,.35),'Laryngoscope':('Laryngscope/Laryngoscope.fbx',.25,1)}
for name,(model,size,ratio) in choices.items():
 row=next(r for r in inspection if r['source']==model);sources=list(dict.fromkeys(m for o in row['objects'] for m in o['materials']));materials=[]
 for i,material in enumerate(sources):
  lower=material.lower();color=(185,190,193);metal=0;rough=150;image=None
  if name=='VascularAccess':
   if 'cap' in lower or 'plug' in lower:color=(65,145,185);rough=125
   if 'needle' in lower or material=='Material.002':color=(200,204,207);metal=220;rough=85
   if 'glass' in lower:color=(207,220,223);rough=100
  if name=='Laryngoscope':color=(187,192,196);metal=220;rough=100
  if name=='Suction':
   if 'screen_vray' in lower:image='Suction/TEXTURES/TEXTURES/VAC Screen Color.png'
   elif 'vacuum_pumpvray' in lower:image='Suction/TEXTURES/TEXTURES/VAC PUMP Color.png'
   elif 'power_switch' in lower:image='Suction/TEXTURES/TEXTURES/VAC POWER SWITCH Color.png'
   elif 'led__' in lower:image='Suction/TEXTURES/TEXTURES/VAC LED Color.png'
   elif any(k in lower for k in ['dark','black','frame']):color=(45,49,51)
   elif 'metal' in lower or 'pins' in lower:color=(165,172,178);metal=210;rough=90
   elif 'cup' in lower or 'glass' in lower:color=(194,211,214);rough=110
  base=Image.open(src/image).convert('RGB') if image else Image.new('RGB',(16,16),color);base.thumbnail((1024,1024))
  folder=root/'Assets/Medical'/name/'Data';folder.mkdir(parents=True,exist_ok=True);stem=name+'_'+str(i);relative=folder.relative_to(root).as_posix()+'/'+stem
  Image.merge('RGBA',(*base.split(),Image.new('L',base.size,rough))).save(folder/(stem+'_BCR.png'));Image.new('RGBA',base.size,(128,128,metal,255)).save(folder/(stem+'_NMO.png'))
  for suffix,preset in [('BCR','{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf'),('NMO','{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf')]:
   texture=relative+'_'+suffix+'.edds';(root/(texture+'.meta')).write_text('MetaFileClass { Name "'+ref(texture)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding='utf-8')
  matpath=relative+'.emat';(root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(relative+'_BCR.edds')+'" NMOMap "'+ref(relative+'_NMO.edds')+'" }',encoding='utf-8');(root/(matpath+'.meta')).write_text('MetaFileClass { Name "'+ref(matpath)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding='utf-8')
  materials.append(dict(source=material,material=ref(matpath),base_preview=relative+'_BCR.png'))
 item=dict(name=name,source=model,source_root='.checks/vendor-medical-wave2',desired_longest_metres=size,decimate_ratio=ratio,materials=materials,model=ref('Assets/Medical/'+name+'/BCMM_'+name+'.xob'))
 items=[i for i in items if i['name']!=name];items.append(item)
 if name!='Laryngoscope':
  prefab=root/'Prefabs/Items'/('BCMM_'+name+'.et');text=prefab.read_text(encoding='utf-8')
  if 'MeshObject "{51F66EC3C5BC700D}"' not in text:text=text.replace(' components {',' components {\n  MeshObject "{51F66EC3C5BC700D}" { Object "'+item['model']+'" }',1)
  prefab.write_text(text,encoding='utf-8')
 print('Prepared reconstructed materials',name)
path.write_text(json.dumps(items,indent=2)+'\n',encoding='utf-8')
