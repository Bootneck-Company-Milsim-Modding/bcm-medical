"""Generate angle-comparison fixtures inside a disposable snapshot only."""
from pathlib import Path
import argparse, hashlib, re
p=argparse.ArgumentParser();p.add_argument('snapshot',type=Path);p.add_argument('items',nargs='+');a=p.parse_args()
root=a.snapshot.resolve()
if '.checks' not in root.parts or len(a.items)>5: raise SystemExit('Use a .checks snapshot, at most five rows.')
angles=['0 0 0','90 0 0','0 90 0','0 0 90','180 0 0','0 180 0']
resources=[]
for name in a.items:
    source=(root/f'Prefabs/Items/BCMM_{name}.et').read_text()
    for index,angle in enumerate(angles):
        relative=f'Prefabs/Beta01Angles/{name}_{index}_{angle.replace(" ","-")}.et'
        path=root/'Tests/Runtime'/relative;path.parent.mkdir(parents=True,exist_ok=True)
        guid=hashlib.sha256(relative.encode()).hexdigest()[:16].upper();resource='{'+guid+'}'+relative
        text=re.sub(r'ItemRenderRotation [\d. -]+(?= FOV)', 'ItemRenderRotation '+angle,source)
        path.write_text(text)
        Path(str(path)+'.meta').write_text('MetaFileClass { Name "'+resource+'" Configurations { EntityTemplateResourceClass PC {} EntityTemplateResourceClass HEADLESS : PC {} } }')
        resources.append(resource)
target=root/'Tests/Runtime/Scripts/Game/BCMM_Beta01ItemResources.c'
target.write_text('class BCMM_Beta01ItemResources { static array<ResourceName> All() { return {\n'+',\n'.join('"'+x+'"' for x in resources)+'\n}; } }\n')
print('Angle board generated:',len(resources),'native variants. Columns:',angles)
