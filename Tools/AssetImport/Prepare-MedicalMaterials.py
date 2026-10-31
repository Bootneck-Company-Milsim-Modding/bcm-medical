"""Prepare native packed textures/materials from user-supplied PBR and glTF data."""
from pathlib import Path
from PIL import Image,ImageChops
import json,struct,io,hashlib
root=Path(__file__).resolve().parents[2];src=root/".checks/vendor-medical/Mod Assets"
inspection=json.loads((root/".checks/medical-model-inspection.json").read_text())
def guid(path):return hashlib.sha256(("BCMM/vendor/"+path).encode()).hexdigest()[:16].upper()
def ref(path):return "{"+guid(path)+"}"+path
def meta(path,kind):
 path=Path(path); name=path.as_posix()
 (root/(name+".meta")).write_text('MetaFileClass { Name "'+ref(name)+'" Configurations { '+kind+' PC {} '+kind+' HEADLESS : PC {} } }',encoding="utf-8")
def texture_meta(path,normal=False):
 preset="{A968DA7F9A1E3A3E}Configs/System/ResourceTypes/PC/TextureNType.conf" if normal else "{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf"
 (root/(path+".meta")).write_text('MetaFileClass { Name "'+ref(path)+'" Configurations { PNGResourceClass PC : "'+preset+'" {} PNGResourceClass HEADLESS : PC {} } }',encoding="utf-8")
def load(p,mode):return Image.open(p).convert(mode)
def channel(p,which=0):return load(p,"RGB").split()[which]
materialsets={
 "Epinephrine":dict(folder="Better Epinephrine/SurvivalKit_AutoInjectorV1_TEXTURES",base="SurvivalKit_AutoInjectorV1_Albedo.png",rough="SurvivalKit_AutoInjectorV1_Roughness.png",metal="SurvivalKit_AutoInjectorV1_Metallic.png",ao="SurvivalKit_AutoInjectorV1_AO.png",normal="SurvivalKit_AutoInjectorV1_NormalDirectX.png"),
 "Injector":dict(folder="Better Injector/SurvivalKit_AutoInjectorV2_TEXTURES",base="SurvivalKit_AutoInjectorV2_Albedo.png",rough="SurvivalKit_AutoInjectorV2_Roughness.png",metal="SurvivalKit_AutoInjectorV2_Metallic.png",ao="SurvivalKit_AutoInjectorV2_AO.png",normal="SurvivalKit_AutoInjectorV2_NormalDirectX.png"),
 "Morphine":dict(folder="Better Morphine/Textures",base="T_MorphineShot_BC.tga",rough="T_MorphineShot_R.tga",metal="T_MorphineShot_M.tga",ao="T_MorphineShot_AO.tga",normal="T_MorphineShot_N.tga"),
 "PulseOximeter":dict(folder="Better Pulse Oximiter/Pulse+Oximeter+Textures/Textures",base="Pulse Oximeter_Base_Color.png",rough="Pulse Oximeter_Roughness.png",metal="Pulse Oximeter_Metallic.png",ao="Pulse Oximeter_Mixed_AO.png",normal="Pulse Oximeter_Normal.png")}
extras_path=root/"Tools/AssetImport/MedicalAssets.json"
extras=[item for item in json.loads(extras_path.read_text(encoding="utf-8")) if "source_root" in item] if extras_path.exists() else []
manifest=[]
for item in inspection:
 name=item["name"];dest=root/"Assets/Medical"/name;dest.mkdir(parents=True,exist_ok=True)
 materials={}
 if name in materialsets:
  spec=materialsets[name];folder=src/spec["folder"]
  base=load(folder/spec["base"],"RGB");size=base.size
  rough=channel(folder/spec["rough"]).resize(size);metal=channel(folder/spec["metal"]).resize(size);ao=channel(folder/spec["ao"]).resize(size);normal=load(folder/spec["normal"],"RGB").resize(size)
  materials={m:dict(base=base,rough=rough,metal=metal,ao=ao,normal=normal) for obj in item["objects"] for m in obj["materials"]}
 elif (src/item["source"]).suffix==".glb":
  b=(src/item["source"]).read_bytes();length,typ=struct.unpack_from("<II",b,12);g=json.loads(b[20:20+length]);offset=20+length;blen,btyp=struct.unpack_from("<II",b,offset);binary=b[offset+8:offset+8+blen]
  def tex(index):
   image=g["images"][g["textures"][index]["source"]];view=g["bufferViews"][image["bufferView"]];start=view.get("byteOffset",0);return Image.open(io.BytesIO(binary[start:start+view["byteLength"]])).convert("RGB")
  for mat in g["materials"]:
   pbr=mat.get("pbrMetallicRoughness",{});factor=pbr.get("baseColorFactor",[1,1,1,1]);base=tex(pbr["baseColorTexture"]["index"]) if "baseColorTexture" in pbr else Image.new("RGB",(16,16),(255,255,255));size=base.size
   base=Image.merge("RGB",[c.point(lambda v,f=factor[k]:round(v*f)) for k,c in enumerate(base.split())])
   mr=tex(pbr["metallicRoughnessTexture"]["index"]).resize(size) if "metallicRoughnessTexture" in pbr else None
   rough=mr.split()[1].point(lambda v:round(v*pbr.get("roughnessFactor",1))) if mr else Image.new("L",size,round(pbr.get("roughnessFactor",1)*255))
   metal=mr.split()[2].point(lambda v:round(v*pbr.get("metallicFactor",1))) if mr else Image.new("L",size,round(pbr.get("metallicFactor",1)*255))
   normal=tex(mat["normalTexture"]["index"]).resize(size) if "normalTexture" in mat else Image.new("RGB",size,(128,128,255));nr,ng,nb=normal.split();normal=Image.merge("RGB",(nr,ImageChops.invert(ng),nb))
   ao=tex(mat["occlusionTexture"]["index"]).resize(size).split()[0] if "occlusionTexture" in mat else Image.new("L",size,255)
   materials[mat["name"]]=dict(base=base,rough=rough,metal=metal,ao=ao,normal=normal)
 else:
  for obj in item["objects"]:
   for m in obj["materials"]: materials[m]=dict(base=Image.new("RGB",(16,16),(160,185,190)),rough=Image.new("L",(16,16),160),metal=Image.new("L",(16,16),0),ao=Image.new("L",(16,16),255),normal=Image.new("RGB",(16,16),(128,128,255)))
 assignments=[]
 for index,(source,images) in enumerate(materials.items()):
  stem=f"{name}_{index}";folder=dest/"Data";folder.mkdir(exist_ok=True)
  bcr=Image.merge("RGBA",(*images["base"].split(),images["rough"]));nmo=Image.merge("RGBA",(*images["normal"].split()[:2],images["metal"],images["ao"]))
  bcr.save(folder/(stem+"_BCR.png"));nmo.save(folder/(stem+"_NMO.png"))
  basepath=(folder.relative_to(root)/stem).as_posix();texture_meta(basepath+"_BCR.edds");texture_meta(basepath+"_NMO.edds",True)
  matpath=basepath+".emat";(root/matpath).write_text('MatPBRBasic { BCRMap "'+ref(basepath+"_BCR.edds")+'" NMOMap "'+ref(basepath+"_NMO.edds")+'" }',encoding="utf-8");meta(matpath,"EMATResourceClass")
  assignments.append(dict(source=source,material=ref(matpath),base_preview=str((folder/(stem+"_BCR.png")).relative_to(root))))
 item["materials"]=assignments;item["model"] = ref((dest.relative_to(root)/("BCMM_"+name+".xob")).as_posix());manifest.append(item)
(root/"Tools/AssetImport/MedicalAssets.json").write_text(json.dumps(manifest+extras,indent=2),encoding="utf-8")
print("Prepared native materials for",len(manifest),"medical models")
