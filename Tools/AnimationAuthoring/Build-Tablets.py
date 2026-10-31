"""Original BCM standing tablet presentation on the attributed native sample rig."""
import bpy,math,json
from pathlib import Path
from mathutils import Vector,Matrix
root=Path(__file__).resolve().parents[2];out=root/'Source/Animations';rig=bpy.data.objects['rig'];native=bpy.data.objects['Character'];scene=bpy.context.scene
rig.animation_data.action=bpy.data.actions['BCMM_CPR_enter_BLOCKOUT'];scene.frame_set(1);bpy.context.view_layer.update()
controls=['torso','chest','head','foot_ik.L','foot_ik.R','hand_ik.L','hand_ik.R','thigh_ik_target.L','thigh_ik_target.R']
neutral={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
head=native.pose.bones['Head'].head.copy();manifest=[];metrics={}
def key(frame):
 for n in controls:
  b=rig.pose.bones[n];b.keyframe_insert('location',frame=frame,group=n);b.keyframe_insert('rotation_euler' if b.rotation_mode=='XYZ' else 'rotation_quaternion',frame=frame,group=n)
def palm(side):
 prefix='Left' if side=='L' else 'Right';return (native.pose.bones[prefix+'Hand'].head+(native.pose.bones[prefix+'HandIndex1'].head+native.pose.bones[prefix+'HandPinky1'].head)*.5)*.5
def place(side,point,along=Vector((0,-1,0)),normal=Vector((0,0,-1))):
 b=rig.pose.bones['hand_ik.'+side];prefix='Left' if side=='L' else 'Right'
 along=along.normalized();normal=(normal-along*normal.dot(along)).normalized();across=along.cross(normal)
 desired=Matrix((across,along,normal)).transposed()
 for iteration in range(3):
  bpy.context.view_layer.update();wrist=native.pose.bones[prefix+'Hand'].head
  currentAlong=(native.pose.bones[prefix+'HandMiddle1'].head-wrist).normalized()
  currentAcross=(native.pose.bones[prefix+'HandIndex1'].head-native.pose.bones[prefix+'HandPinky1'].head).normalized()
  if side=='R':currentAcross=-currentAcross
  currentNormal=currentAcross.cross(currentAlong).normalized();currentAcross=currentAlong.cross(currentNormal).normalized()
  current=Matrix((currentAcross,currentAlong,currentNormal)).transposed();correction=desired@current.transposed()
  mat=(correction@b.matrix.to_3x3()).to_4x4();mat.translation=b.matrix.translation+Vector(point)-palm(side);b.matrix=mat
 bpy.context.view_layer.update()
for kind in ('TabletSelf','TabletOther'):
 a=bpy.data.actions.new('BCMM_'+kind+'_loop');a.use_fake_user=True;rig.animation_data.action=a
 min_dist=100;max_dist=0
 for frame in range(1,122):
  scene.frame_set(frame)
  for n in controls:rig.pose.bones[n].matrix_basis=neutral[n].copy()
  bpy.context.view_layer.update();t=(frame-1)/120
  # One continuous four-second cycle: packet, pinch, hand-to-mouth/offer, return.
  travel=max(0,math.sin(math.pi*max(0,min(1,(t-.15)/.7))))**2
  packet=Vector((-.07,-.24,head.z-.38));mouth=Vector((0,-.14,head.z-.075))
  offer=Vector((-.04,-.57,head.z-.30));target=mouth+Vector((0,-.04,-.065)) if kind=='TabletSelf' else offer
  right=packet.lerp(target,travel)
  left=Vector((.11,-.25,head.z-.38));left.z+=.008*math.sin(t*2*math.pi)
  place('L',left)
  along=Vector((0,-1,0)).lerp(Vector((0,0,1)),travel if kind=='TabletSelf' else 0)
  normal=Vector((0,0,-1)).lerp(Vector((0,-1,0)),travel if kind=='TabletSelf' else 0)
  place('R',right,along,normal);key(frame)
  distance=(palm('R')-mouth).length;min_dist=min(min_dist,distance);max_dist=max(max_dist,distance)
 scene.frame_set(1);bpy.context.view_layer.update();working={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
 manifest.append(dict(action=a.name,stem='BCMM_'+kind+'_loop',frames=121,fps=30,loop=True));metrics[kind]={'minimum_hand_to_mouth_metres':min_dist,'maximum_hand_to_mouth_metres':max_dist}
 for phase,reverse in (('enter',False),('exit',True)):
  clip=bpy.data.actions.new('BCMM_'+kind+'_'+phase);clip.use_fake_user=True;rig.animation_data.action=clip
  for f in range(1,32):
   scene.frame_set(f);t=(f-1)/30;t=t*t*(3-2*t)
   if reverse:t=1-t
   for n in controls:
    p0,q0,s0=neutral[n].decompose();p1,q1,s1=working[n].decompose();rig.pose.bones[n].matrix_basis=Matrix.LocRotScale(p0.lerp(p1,t),q0.slerp(q1,t),s0.lerp(s1,t))
   key(f)
  manifest.append(dict(action=clip.name,stem='BCMM_'+kind+'_'+phase,frames=31,fps=30,loop=False))
 scene.frame_set(1)
rig.animation_data.action=bpy.data.actions['BCMM_TabletSelf_loop'];scene.frame_set(61);bpy.context.view_layer.update()
scene.frame_start=1;scene.frame_end=121;scene.render.fps=30
scene['BCMM_Status']='Original tablet presentation; clinical authority separate; runtime and visual acceptance required'
bpy.ops.wm.save_as_mainfile(filepath=str(out/'BCMM_Tablets.blend'))
(root/'Tools/AnimationAuthoring/tablet-procedures.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
(root/'.checks/tablet-source-metrics.json').write_text(json.dumps(metrics,indent=2),encoding='utf-8')
print('BCMM_TABLET_SOURCE_COMPLETE',json.dumps(metrics),flush=True)
