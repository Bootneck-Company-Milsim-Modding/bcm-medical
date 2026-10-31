"""Original handle grips on the attributed Bohemia rig; procedural runtime pose data."""
import bpy,math,json
from mathutils import Matrix,Vector,Quaternion
from pathlib import Path
root=Path(__file__).resolve().parents[2];rig=bpy.data.objects['rig'];native=bpy.data.objects['Character'];scene=bpy.context.scene
rig.animation_data.action=bpy.data.actions['BCMM_CPR_enter_BLOCKOUT'];scene.frame_set(1);bpy.context.view_layer.update()
controls=['torso','chest','head','foot_ik.L','foot_ik.R','hand_ik.L','hand_ik.R','upper_arm_ik_target.L','upper_arm_ik_target.R']
fingers=[]
for side in ('L','R'):
 for digit in ('f_index','f_middle','f_ring','f_pinky','thumb'):
  for segment in ('01','02','03'):
   name=f'{digit}.{segment}.{side}'
   if name in rig.pose.bones:fingers.append(name)
controls+=fingers
for obj in list(bpy.data.objects):
 if obj.name.startswith('BCMM_Casualty'):bpy.data.objects.remove(obj,do_unlink=True)
neutral={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
C=Matrix(((1,0,0,0),(0,0,1,0),(0,1,0,0),(0,0,0,1)))
def palm(side):
 prefix='Left' if side=='L' else 'Right';w=native.pose.bones[prefix+'Hand'].head.copy();i=native.pose.bones[prefix+'HandIndex1'].head.copy();p=native.pose.bones[prefix+'HandPinky1'].head.copy()
 along=((i+p)*.5-w).normalized();across=(i-p) if side=='L' else (p-i)
 return (w+(i+p)*.5)*.5,along,across.cross(along).normalized()
def align_hand(side,target):
 control=rig.pose.bones['hand_ik.'+side]
 for _ in range(4):
  bpy.context.view_layer.update();centre,along,normal=palm(side)
  tilt=normal.rotation_difference(Vector((0,0,1)));forward=tilt@along
  desired=math.pi if side=='L' else 0
  correction=Quaternion((0,0,1),desired-math.atan2(forward.y,forward.x))@tilt
  m=(correction.to_matrix()@control.matrix.to_3x3()).to_4x4();m.translation=control.matrix.translation+Vector(target)-centre;control.matrix=m
 bpy.context.view_layer.update()
report={'attribution':'Bohemia Interactive sample rig, Arma Public Licence; original BCM grip poses','poses':{}}
for kind,reach in [('Front',-.4),('Rear',.4)]:
 action=bpy.data.actions.new('BCMM_Stretcher'+kind+'_grip');action.use_fake_user=True;rig.animation_data.action=action
 for frame in range(1,32):
  scene.frame_set(frame)
  for n in controls:rig.pose.bones[n].matrix_basis=neutral[n].copy()
  bpy.context.view_layer.update()
  for side,sign in [('L',1),('R',-1)]:
   parent=rig.pose.bones['upper_arm_parent.'+side];parent['IK_FK']=0.0;parent['pole_vector']=1
   pole=rig.pose.bones['upper_arm_ik_target.'+side];m=pole.matrix.copy();m.translation=Vector((sign*.55,reach*.15,1.1));pole.matrix=m
   for digit in ('f_index','f_middle','f_ring','f_pinky'):
    for segment,angle in [('01',50),('02',65),('03',45)]:
     b=rig.pose.bones.get(f'{digit}.{segment}.{side}')
     if b:b.rotation_mode='XYZ';b.rotation_euler=(math.radians(angle),0,0)
   for segment,angle in [('01',15),('02',35),('03',35)]:
    b=rig.pose.bones.get(f'thumb.{segment}.{side}')
    if b:b.rotation_mode='XYZ';b.rotation_euler=(math.radians(angle),0,0)
   bpy.context.view_layer.update();align_hand(side,(sign*.3,reach,1.055))
  for n in controls:
   b=rig.pose.bones[n];b.keyframe_insert('location',frame=frame,group=n);b.keyframe_insert('rotation_euler' if b.rotation_mode=='XYZ' else 'rotation_quaternion',frame=frame,group=n)
 scene.frame_set(1);bpy.context.view_layer.update();pose={}
 for side,prefix in [('L','Left'),('R','Right')]:
  hand=C@native.pose.bones[prefix+'Hand'].matrix@C.inverted();inv=hand.inverted();centre,_,_=palm(side);centre=C@centre
  group={b.name:(inv@(C@b.matrix@C.inverted())).transposed() for b in native.pose.bones if b.name.startswith(prefix+'Hand') and b.name!=prefix+'Hand'}
  pose[prefix]={'matrix':[list(row) for row in hand.transposed()],'palm_offset':list(inv@centre),'bones':{n:[list(row) for row in m] for n,m in group.items()}}
 report['poses'][kind]=pose
 # Simple handle rods establish scale/contact in the editable source preview.
 rods=[]
 for sign in (-1,1):
  bpy.ops.mesh.primitive_cylinder_add(vertices=20,radius=.018,depth=.5,location=(sign*.3,reach,1.03),rotation=(math.pi/2,0,0));rod=bpy.context.object;rod.name='BCMM_HandleReference';rods.append(rod)
 scene.render.engine='CYCLES';scene.cycles.samples=24;scene.render.resolution_x=1000;scene.render.resolution_y=1000;scene.render.resolution_percentage=100
 camera=scene.camera;camera.location=(2.5,-3.5,1.8);camera.rotation_euler=(Vector((0,0,.95))-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.type='ORTHO';camera.data.ortho_scale=2.25
 scene.render.filepath=str(root/'.checks'/('stretcher-grip-'+kind+'.png'));bpy.ops.render.render(write_still=True)
 for rod in rods:bpy.data.objects.remove(rod,do_unlink=True)
scene.frame_start=1;scene.frame_end=31;scene.render.fps=30
bpy.ops.wm.save_as_mainfile(filepath=str(root/'Source/Animations/BCMM_StretcherGrips.blend'))
(root/'Source/Animations/BCMM_StretcherGrips.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('BCMM_STRETCHER_GRIPS_AUTHORED poses='+str(len(report['poses'])))
