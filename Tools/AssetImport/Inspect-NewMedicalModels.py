import bpy,json
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[2];source=root/'.checks/vendor-medical-wave2';report=[]
paths=sorted(source.rglob('*.fbx'))+[source/'Chest Seal/Dressing.blend']
for file in paths:
 bpy.ops.wm.read_factory_settings(use_empty=True)
 try:
  if file.suffix.lower()=='.blend':
   with bpy.data.libraries.load(str(file),link=False) as (available,loaded):loaded.objects=available.objects
   for obj in loaded.objects:
    if obj and obj.type=='MESH':bpy.context.scene.collection.objects.link(obj)
  else:bpy.ops.import_scene.fbx(filepath=str(file),use_anim=False)
  meshes=[o for o in bpy.context.scene.objects if o.type=='MESH'];points=[o.matrix_world@Vector(v) for o in meshes for v in o.bound_box]
  row=dict(source=file.relative_to(source).as_posix(),triangles=sum(len(poly.vertices)-2 for o in meshes for poly in o.data.polygons),objects=[dict(name=o.name,triangles=sum(len(p.vertices)-2 for p in o.data.polygons),materials=[m.name for m in o.data.materials if m],uv_layers=len(o.data.uv_layers)) for o in meshes],images=[dict(name=i.name,path=i.filepath,packed=bool(i.packed_file),size=list(i.size)) for i in bpy.data.images if i.source=='FILE'])
  if points:row['dimensions']=[max(v[k] for v in points)-min(v[k] for v in points) for k in range(3)]
  report.append(row);print('BCMM_NEW_ASSET',row['source'],row['triangles'],flush=True)
 except Exception as e:report.append(dict(source=str(file),error=str(e)))
 (root/'Docs/NEW-MEDICAL-MODEL-INSPECTION.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
