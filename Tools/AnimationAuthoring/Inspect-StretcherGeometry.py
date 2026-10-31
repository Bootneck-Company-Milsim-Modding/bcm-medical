import bpy,json
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[2]
obj=bpy.data.objects['BCMM_FieldStretcher']
points=[obj.matrix_world@v.co for v in obj.data.vertices]
low=[min(v[k] for v in points) for k in range(3)];high=[max(v[k] for v in points) for k in range(3)]
report={'low':low,'high':high,'dimensions':[b-a for a,b in zip(low,high)]}
(root/'.checks/stretcher-geometry.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('BCMM_STRETCHER_GEOMETRY',report)
