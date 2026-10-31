"""Pack user-edited albedo variants, preserving vendor roughness and normal maps."""
from pathlib import Path
from PIL import Image
import json,copy,hashlib,re
root=Path(__file__).resolve().parents[2];path=root/"Tools/AssetImport/MedicalAssets.json";items=json.loads(path.read_text())
base=next(x for x in items if x["name"]=="GenericInjector")
base_material=(root/base["materials"][0]["material"].split("}",1)[1]).read_text()
normal=re.search(r'NMOMap "([^"]+)"',base_material).group(1)
original=Image.open(root/base["materials"][0]["base_preview"]).convert("RGBA")
def ref(path):return "{"+hashlib.sha256(("BCMM/vendor/"+path).encode()).hexdigest()[:16].upper()+"}"+path
for name in ("Naloxone", "NCD", "TXA"):
 folder=root/"Assets/Medical"/name/"Data";folder.mkdir(parents=True,exist_ok=True)
 editable=folder/(name+"_Albedo_EDIT.png")
 if not editable.exists():original.convert("RGB").save(editable)
 albedo=Image.open(editable).convert("RGB")
 if albedo.size!=original.size:raise ValueError("Keep original texture dimensions: "+str(editable))
 packed=Image.merge("RGBA",(*albedo.split(),original.split()[3]));packed.save(folder/(name+"_BCR.png"))
 texture=(folder.relative_to(root)/(name+"_BCR.edds")).as_posix()
 (root/(texture+".meta")).write_text('MetaFileClass { Name "'+ref(texture)+'" Configurations { PNGResourceClass PC : "{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf" {} PNGResourceClass HEADLESS : PC {} } }',encoding="utf-8")
 matpath=(folder.relative_to(root)/(name+".emat")).as_posix()
 (root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(texture)+'" NMOMap "'+normal+'" }',encoding="utf-8")
 (root/(matpath+".meta")).write_text('MetaFileClass { Name "'+ref(matpath)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }',encoding="utf-8")
 item=copy.deepcopy(base);item["name"]=name
 item["accent_albedo"]=True
 item["materials"].append(dict(source="BCMM_CapVariant",material=ref(matpath),base_preview=(folder.relative_to(root)/(name+"_BCR.png")).as_posix()))
 item["model"]=ref("Assets/Medical/"+name+"/BCMM_"+name+".xob")
 items=[x for x in items if x["name"]!=name];items.append(item)
 prefab=root/"Prefabs/Items"/("BCMM_"+name+".et");text=prefab.read_text();text=text.replace(base["model"],item["model"]);prefab.write_text(text,encoding="utf-8")
 print(name, ": preserved supplied cap/grip colours from", editable)
path.write_text(json.dumps(items,indent=2)+"\n",encoding="utf-8")
