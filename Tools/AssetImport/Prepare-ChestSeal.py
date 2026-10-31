from pathlib import Path
from PIL import Image
import hashlib,json,re
root=Path(__file__).resolve().parents[2];folder=root/'Assets/Medical/ChestSeal/Data';folder.mkdir(parents=True,exist_ok=True)
def ref(p):return '{'+hashlib.sha256(('BCMM/vendor/'+p).encode()).hexdigest()[:16].upper()+'}'+p
albedo=root/'Source/MedicalTextures/ChestSeal_Albedo_v2.png'
if not albedo.exists():albedo=root/'Source/MedicalTextures/ChestSeal_Albedo_v1.png'
base=Image.open(albedo).convert('RGB').resize((1024,1024))
Image.merge('RGBA',(*base.split(),Image.new('L',base.size,185))).save(folder/'ChestSeal_BCR.png')
Image.new('RGBA',base.size,(128,128,0,255)).save(folder/'ChestSeal_NMO.png')
relative=folder.relative_to(root).as_posix()+'/ChestSeal'
for suffix,preset in [('BCR','{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf'),('NMO','{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf')]:
 texture=relative+'_'+suffix+'.edds';(root/(texture+'.meta')).write_text('MetaFileClass { Name "'+ref(texture)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding='utf-8')
matpath=relative+'.emat';(root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(relative+'_BCR.edds')+'" NMOMap "'+ref(relative+'_NMO.edds')+'" }',encoding='utf-8')
(root/(matpath+'.meta')).write_text('MetaFileClass { Name "'+ref(matpath)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding='utf-8')
item=dict(name='ChestSeal',source='Chest Seal/Dressing.blend',source_root='.checks/vendor-medical-wave2',desired_longest_metres=.17,materials=[dict(source='BCMM_Default',material=ref(matpath),base_preview=relative+'_BCR.png')],model=ref('Assets/Medical/ChestSeal/BCMM_ChestSeal.xob'))
path=root/'Tools/AssetImport/MedicalAssets.json';items=json.loads(path.read_text(encoding='utf-8'));items=[i for i in items if i['name']!='ChestSeal'];items.append(item);path.write_text(json.dumps(items,indent=2)+'\n',encoding='utf-8')
prefab=root/'Prefabs/Items/BCMM_ChestSeal.et';s=prefab.read_text(encoding='utf-8')
if 'MeshObject "{51F66EC3C5BC700D}"' not in s:s=s.replace(' components {',' components {\n  MeshObject "{51F66EC3C5BC700D}" { Object "'+item['model']+'" }',1)
prefab.write_text(s,encoding='utf-8');print('Chest seal UV texture packed')
