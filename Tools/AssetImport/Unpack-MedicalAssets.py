"""Inventory user-supplied medical assets; preserve originals outside runtime content."""
from pathlib import Path
import zipfile,io,json,hashlib,struct,sys
sys.stdout.reconfigure(encoding="utf-8")
root=Path(__file__).resolve().parents[2]
archive=Path(sys.argv[1]) if len(sys.argv)>1 else Path(r"C:/Users/ruanb/Downloads/Mod Assets.zip")
out=root/".checks/vendor-medical"
out.mkdir(parents=True,exist_ok=True)
records=[]
def extract(z,folder,prefix):
 for item in z.infolist():
  if item.is_dir():continue
  rel=Path(item.filename)
  target=(folder/rel).resolve()
  if not target.is_relative_to(folder.resolve()):raise ValueError("Unsafe archive path")
  if item.file_size>100_000_000:raise ValueError("Unexpected large asset")
  data=z.read(item)
  target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
  record=dict(source=prefix+item.filename,bytes=len(data),sha256=hashlib.sha256(data).hexdigest(),local=str(target.relative_to(root)))
  if target.suffix.lower()==".glb":
   size,kind=struct.unpack_from("<II",data,12);g=json.loads(data[20:20+size]);record["metadata"]=g.get("asset",{})
  records.append(record)
  if target.suffix.lower()==".zip":
   with zipfile.ZipFile(io.BytesIO(data)) as nested:extract(nested,target.with_suffix(""),prefix+item.filename+"!/")
with zipfile.ZipFile(archive) as z:extract(z,out,"")
(root/"Docs/MEDICAL-ASSET-SOURCE-INVENTORY.json").write_text(json.dumps(dict(archive_sha256=hashlib.sha256(archive.read_bytes()).hexdigest(),files=records),indent=2,ensure_ascii=False),encoding="utf-8")
print("Inventoried and preserved",len(records),"files in",out)
