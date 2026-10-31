from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[2]
manifest=json.loads((root/"Tools/AssetImport/MedicalAssets.json").read_text())
for item in manifest:
 folder=root/"Assets/Medical"/item["name"]
 metadata=(folder/("BCMM_"+item["name"]+".xob.meta")).read_text()
 assigned=re.findall(r'AssignedMaterial "([^"]+)"',metadata)
 expected={a["material"] for a in item["materials"]}
 assert assigned and all(a in expected for a in assigned),(item["name"],assigned)
 compiled=folder/("BCMM_"+item["name"]+".xob")
 source=compiled.with_suffix(".fbx")
 assert compiled.stat().st_mtime>=source.stat().st_mtime,(item["name"],"Compiled model is stale")
 binary=compiled.read_bytes()
 assert ("BCMM_"+item["name"]+"_M0").encode() in binary,item["name"]
 for resource in assigned:
  material=root/resource.split("}",1)[1];text=material.read_text()
  assert "BCRMap" in text and "NMOMap" in text,material
  for texture in re.findall(r'(?:BCRMap|NMOMap) "[^"]*}([^"]+)"',text):assert (root/texture).stat().st_size>128,texture
print("Verified",len(manifest),"native models, material mappings and packed texture dependencies")
