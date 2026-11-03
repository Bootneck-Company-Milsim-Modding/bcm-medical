"""Catalogue current production surfaces; all evidence starts unverified."""
from pathlib import Path
import json,re,hashlib
root=Path(__file__).resolve().parents[1];out=root/'Docs/Beta01';out.mkdir(exist_ok=True)
settings=json.loads((root/'Tools/SettingsCatalog.json').read_text())
items=json.loads((root/'Tools/ArsenalItems.json').read_text())
resources=',\n   '.join(json.dumps(item['resource']) for item in items)
(root/'Tests/Runtime/Scripts/Game/BCMM_Beta01ItemResources.c').write_text('// Generated from Tools/ArsenalItems.json. Diagnostic only.\nclass BCMM_Beta01ItemResources { static array<ResourceName> All() { return {\n   '+resources+'\n  }; } }\n')
for item in items:
    relative=re.sub(r'^\{[^}]+\}','',item['resource']);p=root/relative
    item['file']=relative
    item['nativeReference']=not p.exists()
    item['status']='UNVERIFIED';item['nativeEquipCycles']=0
    if p.exists():
        text=p.read_text();item['sha256']=hashlib.sha256(p.read_bytes()).hexdigest()
        for field in ('ItemDimensions','ItemVolume','CameraPreset','CameraDistanceToItem','ItemRenderRotation'):
            m=re.search(field+r'\s+([^}\r\n]+)',text)
            if m:item[field]=m.group(1).strip()
        item['checks']={key:'UNVERIFIED' for key in ('icon','worldScale','packing','handPose','equipUnequip','materialsLOD','arsenalCharacterPreview')}
for setting in settings:
    setting.update(stateCycles=0,gameplayCycles=0,multiplayerCycles=0,restartCycles=0,status='UNVERIFIED')
classes=[];enums=[]
for p in sorted((root/'Scripts/Game/BCM_Medical').glob('*.c')):
    text=p.read_text()
    for name,parent in re.findall(r'\bclass\s+(\w+)\s*:\s*(\w+)',text):
        if any(s in name+parent for s in ('Action','Treatment','Session','Menu','Persistence','Settings','Facility','Procedure')):
            classes.append(dict(name=name,parent=parent,file=p.relative_to(root).as_posix(),status='UNVERIFIED'))
    for name,body in re.findall(r'\benum\s+(\w+)\s*\{([^}]+)\}',text,re.S):
        enums.append(dict(name=name,members=[v.strip() for v in body.split(',') if v.strip()],file=p.relative_to(root).as_posix()))
report=dict(schema=1,sourceCommit='015e6c49c33d1aad15e945d9f83304857db8894d',settingsSchema=35,settings=settings,items=items,surfaces=classes,enumerations=enums,
            coverageNote='Discovered production surfaces, not a completeness proof or test result. State assertions do not establish gameplay effects. Reconcile with issue and native evidence ledgers.')
(out/'catalogue.json').write_text(json.dumps(report,indent=2)+'\n')
print(f'Catalogued {len(items)} inventory entries, {len(settings)} settings, {len(classes)} action/workflow classes and {len(enums)} state enumerations.')
