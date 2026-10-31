"""Regenerate native US/USSR/FIA medical overrides from the installed-game extract.
Never copies third-party mod code. Run after extracting matching native configs.
"""
from pathlib import Path
import json,re,hashlib,argparse
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser();parser.add_argument('--native-root',type=Path,default=root/'.checks/native-arsenal');args=parser.parse_args()
items=json.loads((root/'Tools/ArsenalItems.json').read_text(encoding='utf-8'))
def guid(value):return hashlib.sha256(('BCMM_Arsenal_'+value).encode()).hexdigest()[:16].upper()
def block_end(text,opening):
 depth=0;quoted=False;escaped=False
 for pos in range(opening,len(text)):
  char=text[pos]
  if escaped:escaped=False;continue
  if char=='\\' and quoted:escaped=True;continue
  if char=='"':quoted=not quoted;continue
  if quoted:continue
  if char=='{':depth+=1
  if char=='}':
   depth-=1
   if not depth:return pos+1
 raise ValueError('Unclosed native configuration block')
def write(path,text,identity):
 output=root/path;output.parent.mkdir(parents=True,exist_ok=True);output.write_text(text,encoding='utf-8')
 output.with_suffix(output.suffix+'.meta').write_text('MetaFileClass { Name "{'+identity+'}'+path.as_posix()+'" Configurations { CONFResourceClass PC {} CONFResourceClass HEADLESS : PC {} CONFResourceClass XBOX_ONE : PC {} CONFResourceClass XBOX_SERIES : PC {} CONFResourceClass PS4 : PC {} CONFResourceClass PS5 : PC {} } }\n',encoding='utf-8')
record=[]
for faction,identity in [('US','5F7EC52FC40A03E2'),('USSR','C53421647C3D0D2E'),('FIA','E908001749419691')]:
 path=Path('Configs/EntityCatalog')/faction/('InventoryItems_EntityCatalog_'+faction+'.conf');original=(args.native_root/path).read_text(encoding='utf-8');text=original;spans=[]
 for match in re.finditer(r'SCR_EntityCatalogInventoryItem\s+"[^\"]+"\s*\{',text):
  end=block_end(text,match.end()-1)
  if re.search(r'm_eItemType\s+HEAL\b',text[match.start():end]):spans.append((match.start(),end))
 if not spans:raise ValueError('No native medical entries in '+str(path))
 for start,end in reversed(spans):text=text[:start]+text[end:]
 entries=[]
 for item in items:
  entries.append('    SCR_EntityCatalogInventoryItem "{'+guid(faction+item['stem'])+'}" { m_sEntityPrefab "'+item['resource']+'" m_aEntityDataList { BCMM_ArsenalItem "{'+guid(faction+item['stem']+'data')+'}" { m_eItemType HEAL m_eItemMode CONSUMABLE m_iSupplyCost '+str(item['cost'])+' CostKey "arsenalCost'+item['stem']+'" } } }')
 position=text.rfind('}',0,text.rfind('}'))
 text=text[:position]+' SCR_EntityCatalogMultiListEntry "{'+guid(faction+'medical')+'}" { m_sIdentifier "BCM Medical" m_aEntities {\n'+'\n'.join(entries)+'\n } }\n '+text[position:]
 write(path,text,identity);record.append(dict(faction=faction,removed_native_medical=len(spans),medical_items=len(items),native_sha256=hashlib.sha256((args.native_root/path).read_bytes()).hexdigest(),override_sha256=hashlib.sha256((root/path).read_bytes()).hexdigest()))
path=Path('Configs/Arsenal/ArsenalItemTypeUIConfig.conf');text=(args.native_root/path).read_text(encoding='utf-8')
assert text.count('Name "#AR-ArsenalItemType_Heal_Name"')==1
write(path,text.replace('Name "#AR-ArsenalItemType_Heal_Name"','Name "BCM Medical"'),'4CC149958FD15308')
(root/'Docs/ARSENAL-OVERRIDE-MANIFEST.json').write_text(json.dumps(record,indent=2)+'\n',encoding='utf-8')
print('Generated three native faction overrides with',len(items),'medical inventory items each')
