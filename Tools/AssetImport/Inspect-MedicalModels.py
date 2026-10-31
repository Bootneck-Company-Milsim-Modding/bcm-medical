"""Run with Blender --background --factory-startup --python; no embedded vendor scripts."""
import bpy,json,math
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[2]
source=root/".checks/vendor-medical/Mod Assets"
choices={
 "Epinephrine":("Better Epinephrine/SurvivalKit_AutoInjectorV1_FBX/SurvivalKit_AutoInjectorV1.fbx",.14),
 "Injector":("Better Injector/SurvivalKit_AutoInjectorV2_FBX/SurvivalKit_AutoInjectorV2.fbx",.14),
 "Morphine":("Better Morphine/SM_MorphineShot.fbx",.14),
 "PulseOximeter":("Better Pulse Oximiter/Pulse+Oximeter.fbx",.065),
 "AED":("AED/ipad_sp1_automated_external_defibrillator__aed.glb",.30),
 "OxygenMask":("Oxygen/oxygen_mask.glb",.22),
 "GenericInjector":("AutoInjector1/autoinjector.glb",.14),
 "FluidBag":("blood+_bag.fbx",.24)}
report=[]
for name,(relative,desired) in choices.items():
 bpy.ops.object.select_all(action="SELECT");bpy.ops.object.delete(use_global=False)
 file=source/relative
 if file.suffix==".fbx":bpy.ops.import_scene.fbx(filepath=str(file),use_anim=False)
 else:bpy.ops.import_scene.gltf(filepath=str(file))
 meshes=[o for o in bpy.context.scene.objects if o.type=="MESH"]
 points=[o.matrix_world@Vector(v) for o in meshes for v in o.bound_box]
 low=Vector([min(v[k] for v in points) for k in range(3)]);high=Vector([max(v[k] for v in points) for k in range(3)])
 tris=sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in meshes)
 mats=[dict(object=o.name,materials=[m.name for m in o.data.materials if m],vertices=len(o.data.vertices),bounds_min=[min((o.matrix_world@Vector(v))[k] for v in o.bound_box) for k in range(3)],bounds_max=[max((o.matrix_world@Vector(v))[k] for v in o.bound_box) for k in range(3)]) for o in meshes]
 report.append(dict(name=name,source=relative,triangles=tris,bounds_min=list(low),bounds_max=list(high),dimensions=list(high-low),objects=mats,desired_longest_metres=desired))
 print("BCMM_MODEL",json.dumps(report[-1]),flush=True)
(root/".checks/medical-model-inspection.json").write_text(json.dumps(report,indent=2),encoding="utf-8")
