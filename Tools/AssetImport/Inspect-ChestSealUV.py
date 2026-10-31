import bpy,json
from pathlib import Path
root=Path(__file__).resolve().parents[2];file=root/'.checks/vendor-medical-wave2/Chest Seal/Dressing.blend'
with bpy.data.libraries.load(str(file),link=False) as (available,loaded):loaded.objects=available.objects
mesh=next(o.data for o in loaded.objects if o and o.type=='MESH');uv=mesh.uv_layers.active
rows=[[list(uv.data[i].uv) for i in poly.loop_indices] for poly in mesh.polygons]
(root/'.checks/chest-seal-uv.json').write_text(json.dumps(rows),encoding='utf-8')
