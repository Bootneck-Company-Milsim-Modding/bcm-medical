"""Original BCM field-procedure motion prototypes on the attributed Bohemia rig.
Run on BCMM_CPR_Blockout.blend with embedded scripts disabled. Not final art.
"""
import bpy, math, json
from pathlib import Path
from mathutils import Matrix, Vector, Quaternion
root=Path(__file__).resolve().parents[2]
out=root/'Source/Animations'
rig=bpy.data.objects['rig']
scene=bpy.context.scene
controls=['torso','chest','head','foot_ik.L','foot_ik.R','hand_ik.L','hand_ik.R','thigh_ik_target.L','thigh_ik_target.R']
rig.animation_data.action=bpy.data.actions['BCMM_CPR_enter_BLOCKOUT']
scene.frame_set(1);bpy.context.view_layer.update()
neutral={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
rig.animation_data.action=bpy.data.actions['BCMM_CPR_loop_BLOCKOUT']
scene.frame_set(1);bpy.context.view_layer.update()
kneeling={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
manifest=[]
def key_controls(frame):
 for n in controls:
  b=rig.pose.bones[n]
  b.keyframe_insert('location',frame=frame,group=n)
  b.keyframe_insert('rotation_euler' if b.rotation_mode=='XYZ' else 'rotation_quaternion',frame=frame,group=n)
def hand(side,position):
 b=rig.pose.bones['hand_ik.'+side]
 native=bpy.data.objects['Character'];prefix='Left' if side=='L' else 'Right'
 centre=(native.pose.bones[prefix+'Hand'].head+(native.pose.bones[prefix+'HandIndex1'].head+native.pose.bones[prefix+'HandPinky1'].head)*.5)*.5
 mat=b.matrix.copy();mat.translation+=Vector(position)-centre;b.matrix=mat
for kind in ('Surgery','Suture','Splint'):
 loop=bpy.data.actions.new('BCMM_'+kind+'_loop_BLOCKOUT');loop.use_fake_user=True
 rig.animation_data.action=loop
 for f in range(1,62):
  scene.frame_set(f)
  for n in controls: rig.pose.bones[n].matrix_basis=kneeling[n].copy()
  bpy.context.view_layer.update()
  phase=2*math.pi*(f-1)/60
  lift=(1-math.cos(phase))*.5
  lateral=math.sin(phase)
  if kind=='Surgery':
   hand('L',(.11+.008*lateral,-.51,.36+.008*lift))
   hand('R',(-.10+.018*lateral,-.50+.015*lift,.38+.018*lift))
  elif kind=='Suture':
   hand('L',(.09,-.52,.365))
   hand('R',(-.07-.02*lateral,-.51+.08*lift,.38+.07*lift))
  else:
   hand('L',(.14+.018*lateral,-.49,.25+.02*lift))
   hand('R',(-.13-.018*lateral,-.47+.035*lift,.25+.035*lift))
  bpy.context.view_layer.update();key_controls(f)
 scene.frame_set(1);bpy.context.view_layer.update()
 working={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
 manifest.append({'action':loop.name,'stem':'BCMM_'+kind+'_loop','frames':61,'fps':30,'loop':True})
 for phase_name,reverse in (('enter',False),('exit',True)):
  a=bpy.data.actions.new('BCMM_'+kind+'_'+phase_name+'_BLOCKOUT');a.use_fake_user=True
  rig.animation_data.action=a
  for f in range(1,32):
   scene.frame_set(f);t=(f-1)/30;t=t*t*(3-2*t)
   if reverse:t=1-t
   for n in controls:
    p0,q0,s0=neutral[n].decompose();p1,q1,s1=working[n].decompose()
    rig.pose.bones[n].matrix_basis=Matrix.LocRotScale(p0.lerp(p1,t),q0.slerp(q1,t),s0.lerp(s1,t))
   key_controls(f)
  manifest.append({'action':a.name,'stem':'BCMM_'+kind+'_'+phase_name,'frames':31,'fps':30,'loop':False})
 rig.animation_data.action=loop;scene.frame_set(16)
 scene.render.resolution_x=1000;scene.render.resolution_y=750
 scene.render.filepath=str(out/('BCMM_'+kind+'_prototype.png'))
 bpy.ops.render.render(write_still=True)
scene['BCMM_Status']='Original field procedure prototypes; native export and acceptance pending; no clinical accuracy claim'
scene['BCMM_RigAttribution']='Bohemia Interactive sample character rig, Arma Public Licence; licence alongside source'
scene.frame_start=1;scene.frame_end=61;scene.render.fps=30
bpy.ops.wm.save_as_mainfile(filepath=str(out/'BCMM_FieldProcedures_Blockout.blend'))
(root/'Tools/AnimationAuthoring/field-procedures.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print('BCMM_FIELD_PROCEDURE_SOURCE_COMPLETE clips='+str(len(manifest)))
