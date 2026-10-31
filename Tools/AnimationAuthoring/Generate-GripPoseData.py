import json,math
from pathlib import Path
root=Path(__file__).resolve().parents[2]
data=json.loads((root/'Source/Animations/BCMM_StretcherGrips.json').read_text(encoding='utf-8'))['poses']['Rear']
def vec(v):
 if not all(math.isfinite(x) for x in v[:3]):raise ValueError('Nonfinite pose')
 return chr(34)+' '.join(f'{x:.8f}' for x in v[:3])+chr(34)
lines=['// Generated original BCM closed-hand pose; attributed Bohemia sample rig, APL.','class BCMM_GripPoseBone { string Name; vector Matrix[4]; void BCMM_GripPoseBone(string name,vector a,vector b,vector c,vector d) { Name=name;Matrix[0]=a;Matrix[1]=b;Matrix[2]=c;Matrix[3]=d; } }','class BCMM_GripPoseData { static void Read(bool left,out vector hand[4],out vector palmOffset,notnull array<ref BCMM_GripPoseBone> bones) {']
for side,condition in [('Left','if(left)'),('Right','else')]:
 h=data[side];lines.append(condition+' {')
 for i,row in enumerate(h['matrix']):lines.append(f'hand[{i}]={vec(row)};')
 lines.append('palmOffset='+vec(h['palm_offset'])+';')
 for name,m in h['bones'].items():lines.append('bones.Insert(new BCMM_GripPoseBone('+chr(34)+name+chr(34)+','+','.join(vec(row) for row in m)+'));')
 lines.append('}')
lines.append('}')
parents=json.loads((root/'.checks/native-bone-parents.json').read_text(encoding='utf-8'))
lines.append('static string Parent(string name) { switch(name) {')
for name,parent in parents.items():
 if name.startswith(('LeftArm','LeftForeArm','LeftHand','RightArm','RightForeArm','RightHand')) or name in ('LeftShoulder','RightShoulder'):
  lines.append('case '+chr(34)+name+chr(34)+': return '+chr(34)+parent+chr(34)+';')
lines.append('} return '+chr(34)+chr(34)+'; } }')
(root/'Scripts/Game/BCM_Medical/BCMM_GripPoseData.c').write_text('\n'.join(lines)+'\n',encoding='utf-8')
print('Generated original finger poses')
