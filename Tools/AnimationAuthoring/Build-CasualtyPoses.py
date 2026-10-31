"""Original BCM casualty blockouts on the attributed Bohemia sample rig.
No ACE file, bone transform, curve, rig setting or animation is read here.
Run using Character_AnimationRig_Example.blend with autoexec disabled.
"""
import bpy, math, json
from pathlib import Path
from mathutils import Matrix, Vector
root=Path(__file__).resolve().parents[2]
out=root/'Source/Animations/Casualty';out.mkdir(parents=True,exist_ok=True)
if bpy.data.objects.get('CarrierReference_Head_LOD0'):
    raise RuntimeError('Open the fresh attributed Bohemia sample rig before rebuilding')
rig=bpy.data.objects['rig']; native=bpy.data.objects['Character'];scene=bpy.context.scene
for obj in bpy.data.objects:
    if obj.animation_data:
        obj.animation_data.action=None
        for t in list(obj.animation_data.nla_tracks):obj.animation_data.nla_tracks.remove(t)
for b in rig.pose.bones:
    b.location=(0,0,0);b.rotation_mode='QUATERNION';b.rotation_quaternion=(1,0,0,0);b.scale=(1,1,1)
for side in ('L','R'):
    for prefix in ('upper_arm_parent.','thigh_parent.'):
        b=rig.pose.bones[prefix+side]
        if 'IK_FK' in b:b['IK_FK']=0.0
    rig.pose.bones['thigh_parent.'+side]['pole_vector']=1
bpy.context.view_layer.update()
# Static neutral carrier for contact/anatomy review only; no animation authored.
dg=bpy.context.evaluated_depsgraph_get()
for source in list(bpy.data.objects):
    if source.type=='MESH' and source.name.endswith('_LOD0'):
        mesh=bpy.data.meshes.new_from_object(source.evaluated_get(dg),depsgraph=dg)
        carrier=bpy.data.objects.new('CarrierReference_'+source.name,mesh)
        scene.collection.objects.link(carrier);carrier.matrix_world=source.matrix_world.copy()
controls=['torso','chest','head','foot_ik.L','foot_ik.R','hand_ik.L','hand_ik.R','thigh_ik_target.L','thigh_ik_target.R','upper_arm_ik_target.L','upper_arm_ik_target.R']
controls += [part+'_fk.'+side for side in ('L','R') for part in ('thigh','shin','foot')]
rest={n:rig.pose.bones[n].matrix.copy() for n in controls}
neutral={n:rig.pose.bones[n].matrix_basis.copy() for n in controls}
arm_poles={s:rig.pose.bones['upper_arm_parent.'+s].get('pole_vector',0) for s in ('L','R')}
def move(name,point,axis=None,degrees=0):
    m=rest[name].copy()
    if axis:m=Matrix.Rotation(math.radians(degrees),4,axis)@m
    m.translation=Vector(point);rig.pose.bones[name].matrix=m
def key(frame):
    for n in controls:
        b=rig.pose.bones[n];b.keyframe_insert('location',frame=frame,group=n);b.keyframe_insert('rotation_quaternion',frame=frame,group=n)
    for side in ('L','R'):
        b=rig.pose.bones['upper_arm_parent.'+side]
        b.keyframe_insert(data_path='["pole_vector"]',frame=frame,group=b.name)
        b=rig.pose.bones['thigh_parent.'+side]
        b.keyframe_insert(data_path='["IK_FK"]',frame=frame,group=b.name)
def hanging_hand(side):
    prefix='Left' if side=='L' else 'Right'; control=rig.pose.bones['hand_ik.'+side]
    along=Vector((0,0,-1));normal=Vector((0,-1,0));across=along.cross(normal)
    target=Matrix((across,along,normal)).transposed()
    for _ in range(3):
        bpy.context.view_layer.update()
        wrist=native.pose.bones[prefix+'Hand'].head
        current_along=(native.pose.bones[prefix+'HandMiddle1'].head-wrist).normalized()
        current_across=(native.pose.bones[prefix+'HandIndex1'].head-native.pose.bones[prefix+'HandPinky1'].head).normalized()
        if side=='R':current_across=-current_across
        current_normal=current_across.cross(current_along).normalized()
        current_across=current_along.cross(current_normal).normalized()
        current=Matrix((current_across,current_along,current_normal)).transposed()
        m=((target@current.transposed())@control.matrix.to_3x3()).to_4x4();m.translation=control.matrix.translation;control.matrix=m
manifest=[]
for kind in ('Carried','Dragged'):
    rig.animation_data_create();a=bpy.data.actions.new('BCMM_'+kind+'_loop');a.use_fake_user=True;rig.animation_data.action=a
    scene.frame_set(1)
    for n in controls:rig.pose.bones[n].matrix_basis=neutral[n].copy()
    for s in ('L','R'):rig.pose.bones['upper_arm_parent.'+s]['pole_vector']=arm_poles[s]
    for s in ('L','R'):rig.pose.bones['thigh_parent.'+s]['IK_FK']=1.0 if kind=='Carried' else 0.0
    if kind=='Carried':
        # One coherent anatomical frame: local up -> carrier left; local front
        # -> gravity. Preserve left/right ordering through the entire body.
        orientation=Matrix.Rotation(math.radians(90),4,'X')@Matrix.Rotation(math.radians(-90),4,'Y')
        m=orientation@rest['torso'];m.translation=Vector((.25,.25,1.55));rig.pose.bones['torso'].matrix=m
        bpy.context.view_layer.update()
        for name,degrees in [('chest',-25),('head',-12)]:
            b=rig.pose.bones[name];m=b.matrix.copy();p=m.translation.copy();m=Matrix.Rotation(math.radians(degrees),4,'Y')@m;m.translation=p;b.matrix=m
            bpy.context.view_layer.update()
        bpy.context.view_layer.update()
        # Direct joint rotations preserve anatomical roll through hip, knee
        # and ankle. The previous independent foot targets inverted the bend.
        for side,prefix in [('L','Left'),('R','Right')]:
            offset=native.data.bones[prefix+'Leg'].head_local-native.data.bones['Hips'].head_local
            point=native.pose.bones['Hips'].head+orientation.to_3x3()@offset
            for part,angle in [('thigh',-80),('shin',-45),('foot',-45)]:
                b=rig.pose.bones[part+'_fk.'+side]
                m=orientation@Matrix.Rotation(math.radians(angle),4,'X')@b.bone.matrix_local
                m.translation=point;b.matrix=m;bpy.context.view_layer.update();point=b.tail.copy()
        bpy.context.view_layer.update()
        for side,prefix in (('L','Left'),('R','Right')):
            rig.pose.bones['upper_arm_parent.'+side]['pole_vector']=1
            shoulder=native.pose.bones[prefix+'Arm'].head.copy()
            move('hand_ik.'+side,shoulder+Vector((-.015,-.015,-.49)),'Y',-80)
            move('upper_arm_ik_target.'+side,shoulder+Vector((.10 if side=='L' else -.10,-.35,-.30)))
            hanging_hand(side)
        b=rig.pose.bones['hand_ik.R'];m=b.matrix.copy();m.translation.y=.58;b.matrix=m
        b=rig.pose.bones['upper_arm_ik_target.R'];m=b.matrix.copy();m.translation.y=.78;b.matrix=m
    else:
        # Face-up raised torso, low hips and extended trailing legs.
        move('torso',(0,0,.30),'X',-64)
        bpy.context.view_layer.update()
        rig.pose.bones['chest'].rotation_quaternion=Matrix.Rotation(math.radians(-8),4,'X').to_quaternion()
        rig.pose.bones['head'].rotation_quaternion=Matrix.Rotation(math.radians(8),4,'X').to_quaternion()
        bpy.context.view_layer.update()
        for side,sign in (('L',1),('R',-1)):
            move('foot_ik.'+side,(sign*.14,-.77,.12),'X',-65)
            move('thigh_ik_target.'+side,(sign*.22,-.42,.35))
            move('hand_ik.'+side,(sign*.25,.12,.18),'X',-70)
    bpy.context.view_layer.update();key(1);key(31)
    manifest.append({'action':a.name,'stem':'BCMM_'+kind+'_loop','frames':31,'fps':30,'loop':True})
scene.frame_start=1;scene.frame_end=31;scene.render.fps=30
scene['BCMM_Provenance']='Original BCM authored casualty poses on attributed Bohemia sample. ACE is visual reference only.'
rig.animation_data.action=bpy.data.actions['BCMM_Carried_loop'];scene.frame_set(1)
bpy.ops.wm.save_as_mainfile(filepath=str(out/'BCMM_CasualtyPoses.blend'))
(root/'Tools/AnimationAuthoring/casualty-poses.json').write_text(json.dumps(manifest,indent=2))
print('BCMM_ORIGINAL_CASUALTY_BLOCKOUTS_COMPLETE')
