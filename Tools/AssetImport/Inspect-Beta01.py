import bpy, json
from mathutils import Vector
from pathlib import Path
root = Path(__file__).resolve().parents[2]
report = []
for name in ('AED', 'BVM', 'Splint', 'SurgeryKit', 'SutureKit', 'WeaponSling', 'ChestSeal', 'FluidBag', 'Epinephrine', 'NCD', 'Suction', 'VascularAccess', 'OxygenMask'):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    path = root / 'Assets/Medical' / name / ('BCMM_' + name + '.blend')
    if not path.exists(): continue
    bpy.ops.wm.open_mainfile(filepath=str(path))
    report.append({'name': name, 'objects': [{'name': o.name, 'dimensions': list(o.dimensions), 'location': list(o.location), 'materials': [m.name for m in o.data.materials if m]} for o in bpy.context.scene.objects if o.type == 'MESH']})
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(root / '.checks/vendor-medical-wave2/BVM/Ambu+bag.fbx'), use_anim=False)
report.append({'name': 'BVM-source', 'objects': [{'name': o.name, 'dimensions': list(o.dimensions), 'location': list(o.location), 'centre': list(sum((o.matrix_world @ Vector(v) for v in o.bound_box), Vector()) / 8)} for o in bpy.context.scene.objects if o.type == 'MESH']})
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(root / '.checks/vendor-medical-wave2/Suction/VAC+Device+model.fbx'), use_anim=False)
report.append({'name':'Suction-source','objects':[{'name':o.name,'dimensions':list(o.dimensions),'centre':list(sum((o.matrix_world@Vector(v) for v in o.bound_box),Vector())/8)} for o in bpy.context.scene.objects if o.type=='MESH']})
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(root / 'Assets/BodyBags/BCM_FilledBodyBag/BCM_FilledBodyBag_Collision.fbx'),use_anim=False)
report.append({'name':'BodyBag-collision-source','objects':[{'name':o.name,'dimensions':list(o.dimensions),'location':list(o.location),'bounds':[list(o.matrix_world@Vector(v)) for v in o.bound_box]} for o in bpy.context.scene.objects if o.type=='MESH']})
(root / '.checks/beta01-model-inspection.json').write_text(json.dumps(report, indent=2))
