from pathlib import Path,PurePosixPath
import shutil,hashlib,json,zipfile,subprocess
root=Path(__file__).resolve().parents[2];source=Path(r'C:/Users/ruanb/Downloads/Mod Assets');dest=root/'.checks/vendor-medical-wave2';dest.mkdir(parents=True,exist_ok=True)
files=[];errors=[]
def safe(name):
 p=PurePosixPath(name.replace('\\','/'))
 if p.is_absolute() or '..' in p.parts or ':' in name:raise ValueError('Unsafe archive member: '+name)
 return p
for folder in source.iterdir():
 if folder.name=='Mod Assets':continue
 originals=[folder] if folder.is_file() else list(folder.rglob('*'))
 for f in originals:
  if not f.is_file():continue
  rel=f.relative_to(source);target=dest/rel;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(f,target)
  files.append(dict(source=rel.as_posix(),bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()))
  if f.suffix.lower() not in ('.zip','.rar'):continue
  output=target.with_suffix('');output.mkdir(exist_ok=True)
  try:
   if f.suffix.lower()=='.zip':
    with zipfile.ZipFile(f) as archive:
     if sum(i.file_size for i in archive.infolist())>2_000_000_000:raise ValueError('Archive too large')
     for i in archive.infolist():
      path=output/safe(i.filename)
      if i.is_dir():path.mkdir(parents=True,exist_ok=True);continue
      path.parent.mkdir(parents=True,exist_ok=True)
      with archive.open(i) as src,path.open('wb') as dst:shutil.copyfileobj(src,dst)
   else:
    seven=r'C:/Program Files/7-Zip/7z.exe'
    listing=subprocess.run([seven,'l','-slt',str(f)],capture_output=True,text=True,encoding='utf-8',errors='replace',check=True).stdout
    entries=listing.split('----------',1)[-1]
    for line in entries.splitlines():
     if line.startswith('Path = '):safe(line[7:])
     if line.startswith('Symbolic Link = ') and line[16:].strip():raise ValueError('Archive contains symbolic links')
    subprocess.run([seven,'x','-y','-o'+str(output),str(f)],capture_output=True,check=True)
  except Exception as e:errors.append(dict(source=rel.as_posix(),error=str(e)))
report=dict(files=files,errors=errors)
(root/'Docs/NEW-MEDICAL-ASSET-INVENTORY.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('Staged',len(files),'source files;',len(errors),'archive errors')
for e in errors:print(e)
