import json,hashlib
from pathlib import Path
from PIL import Image
root=Path.cwd();p=root/'Tools/AssetImport/MedicalAssets.json';manifest=json.loads(p.read_text())
def guid(path):return hashlib.sha256(('BCMM/applied/'+path).encode()).hexdigest()[:16].upper()
def ref(path):return '{'+guid(path)+'}'+path
def meta(path,cls,base=''):
 inheritance=' : "'+base+'"' if base else ''
 (root/(path+'.meta')).write_text('MetaFileClass { Name "'+ref(path)+'" Configurations { '+cls+' PC'+inheritance+' {} '+cls+' HEADLESS : PC {} } }\n')
colors={'BCMM_SplintOrange':(220,126,52),'BCMM_SplintFoam':(65,115,154),'BCMM_Nylon':(38,43,38),'BCMM_Windlass':(80,85,76),'BCMM_ReleaseTab':(196,44,38)}
for name,materials in [('AppliedSplint',['BCMM_SplintOrange','BCMM_SplintFoam','BCMM_Nylon']),('AppliedTourniquet',['BCMM_Nylon','BCMM_Windlass','BCMM_ReleaseTab'])]:
 item=next(x for x in manifest if x['name']==name);item['materials']=[]
 folder='Assets/Medical/'+name+'/Data/';(root/folder).mkdir(parents=True,exist_ok=True)
 for i,source in enumerate(materials):
  stem=folder+name+'_'+str(i);bcr=stem+'_BCR.edds';nmo=stem+'_NMO.edds';emat=stem+'.emat'
  Image.new('RGBA',(8,8),(*colors[source],205)).save(root/(stem+'_BCR.png'))
  Image.new('RGBA',(8,8),(128,128,255,0 if source!='BCMM_Windlass' else 180)).save(root/(stem+'_NMO.png'))
  meta(bcr,'PNGResourceClass','{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf');meta(nmo,'PNGResourceClass','{A0C830E85F3C1B9F}Configs/System/ResourceTypes/PC/TextureNormalMap.conf')
  # Match the existing project normal texture resource preset exactly.
  template=(root/'Assets/Medical/ChestSeal/Data/ChestSeal_NMO.edds.meta').read_text();import re
  normalbase=re.search(r'PNGResourceClass PC : "([^"]+)"',template).group(1);meta(nmo,'PNGResourceClass',normalbase)
  (root/emat).write_text('MatPBRBasic { BCRMap "'+ref(bcr)+'" NMOMap "'+ref(nmo)+'" }\n');meta(emat,'EMATResourceClass')
  item['materials'].append({'source':source,'material':ref(emat),'base_preview':stem+'_BCR.png'})
inspection=json.loads((root/'Docs/NEW-MEDICAL-MODEL-INSPECTION.json').read_text());iv=next(x for x in manifest if x['name']=='AppliedIVCannula');donor=next(x for x in inspection if x['source']==iv['source']);used={m for o in donor['objects'] if o['name'] in iv['include_objects'] for m in o['materials']};iv['materials']=[m for m in iv['materials'] if m['source'] in used]
p.write_text(json.dumps(manifest,indent=2)+'\n');print('Applied materials staged; IV materials:',used)