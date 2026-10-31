"""Original BCM CPR motion blockout on Bohemia's APL animation rig.
Run Blender on the official rig with --disable-autoexec, then this script.
This source is not an accepted in-game animation.
"""
import bpy, math, json
from mathutils import Matrix, Vector, Euler, Quaternion
from pathlib import Path
root=Path(__file__).resolve().parents[2]
out=root/"Source/Animations"
out.mkdir(parents=True,exist_ok=True)
rig=bpy.data.objects.get("rig")
if rig is None: raise RuntimeError("Official character control rig is required")
# Preserve rig drivers/constraints; remove the sample's animation assignments.
for obj in bpy.data.objects:
    if obj.animation_data:
        obj.animation_data.action=None
        for track in list(obj.animation_data.nla_tracks):
            obj.animation_data.nla_tracks.remove(track)
for bone in rig.pose.bones:
    bone.location=(0,0,0); bone.rotation_mode="QUATERNION"
    bone.rotation_quaternion=(1,0,0,0); bone.scale=(1,1,1)
for side in ("L","R"):
    for prefix in ("upper_arm_parent.","thigh_parent."):
        parent=rig.pose.bones.get(prefix+side)
        if parent and "IK_FK" in parent: parent["IK_FK"]=0.0
bpy.context.view_layer.update()
rest={name:rig.pose.bones[name].matrix.copy() for name in ("torso","chest","head","foot_ik.L","foot_ik.R","hand_ik.L","hand_ik.R","thigh_ik_target.L","thigh_ik_target.R")}
action=bpy.data.actions.new("BCMM_CPR_loop_BLOCKOUT")
action.use_fake_user=True
rig.animation_data_create()
rig.animation_data.action=action
controls=list(rest)
def move(name,position,rotation=None):
    bone=rig.pose.bones[name]
    mat=rest[name].copy()
    if rotation is not None:
        mat=Euler(rotation,"XYZ").to_matrix().to_4x4()
    mat.translation=Vector(position)
    bone.matrix=mat
# Use explicit outward knee poles; the sample automatic IK solution crossed
# the knees even when the feet were separated.
for side in ('L','R'): rig.pose.bones['thigh_parent.'+side]['pole_vector']=1
native=bpy.data.objects['Character']
def palm_frame(side):
    prefix='Left' if side=='L' else 'Right'
    wrist=native.pose.bones[prefix+'Hand'].head.copy()
    index=native.pose.bones[prefix+'HandIndex1'].head.copy()
    pinky=native.pose.bones[prefix+'HandPinky1'].head.copy()
    middle=native.pose.bones[prefix+'HandMiddle1'].head.copy()
    along=(middle-wrist).normalized()
    across=index-pinky if side=='L' else pinky-index
    normal=across.cross(along).normalized()
    centre=(wrist+(index+pinky)*.5)*.5
    return centre,along,normal

def flat_hand(side,centre):
    control=rig.pose.bones['hand_ik.'+side]
    for iteration in range(3):
        bpy.context.view_layer.update()
        current,along,normal=palm_frame(side)
        down=Vector((0,0,1))
        tilt=normal.rotation_difference(down)
        forward=tilt@along
        yaw=-math.pi/2-math.atan2(forward.y,forward.x)
        correction=Quaternion((0,0,1),yaw)@tilt
        matrix=(correction.to_matrix()@control.matrix.to_3x3()).to_4x4()
        matrix.translation=control.matrix.translation+Vector(centre)-current
        control.matrix=matrix
    bpy.context.view_layer.update()

for frame in range(1,20):
    bpy.context.scene.frame_set(frame)
    wave=.5-.5*math.cos(2*math.pi*(frame-1)/18)
    move("torso",(0,-.10,.499-.012*wave))
    bpy.context.view_layer.update()
    chest=rig.pose.bones["chest"]
    chest.rotation_mode="XYZ"
    chest.rotation_euler=(math.radians(60+2*wave),0,0)
    head=rig.pose.bones["head"]
    head.rotation_mode="XYZ"
    head.rotation_euler=(math.radians(20),0,0)
    bpy.context.view_layer.update()
    for side,sign in (("L",1),("R",-1)):
        move("foot_ik."+side,(sign*.25,.30,.10),(math.pi,0,0))
        move("thigh_ik_target."+side,(sign*.30,-.48,.12))
    move("hand_ik.L",(0,-.52,.365-.03*wave),(math.pi,0,0))
    move("hand_ik.R",(0,-.52,.392-.03*wave),(math.pi,0,0))
    bpy.context.view_layer.update()
    flat_hand('L',(0,-.52,.365-.03*wave))
    flat_hand('R',(0,-.52,.392-.03*wave))
    bpy.context.view_layer.update()
    for name in controls:
        bone=rig.pose.bones[name]
        bone.keyframe_insert("location",frame=frame,group=name)
        if bone.rotation_mode=="XYZ": bone.keyframe_insert("rotation_euler",frame=frame,group=name)
        else: bone.keyframe_insert("rotation_quaternion",frame=frame,group=name)
# Original entry/exit blockouts. Blend the control rig from its neutral stance
# into/out of the working pose; these still require native visual acceptance.
scene=bpy.context.scene
scene.frame_set(1)
bpy.context.view_layer.update()
working={name:rig.pose.bones[name].matrix_basis.copy() for name in controls}
loop_action=action
from mathutils import Quaternion
for clip,reverse in (("BCMM_CPR_enter_BLOCKOUT",False),("BCMM_CPR_exit_BLOCKOUT",True)):
    transition=bpy.data.actions.new(clip)
    transition.use_fake_user=True
    rig.animation_data.action=transition
    for frame in range(1,32):
        scene.frame_set(frame)
        t=(frame-1)/30
        t=t*t*(3-2*t)
        if reverse: t=1-t
        for name in controls:
            bone=rig.pose.bones[name]
            position,rotation,scale=working[name].decompose()
            bone.matrix_basis=Matrix.LocRotScale(position*t,Quaternion((1,0,0,0)).slerp(rotation,t),Vector((1,1,1)).lerp(scale,t))
            bone.keyframe_insert("location",frame=frame,group=name)
            if bone.rotation_mode=="XYZ": bone.keyframe_insert("rotation_euler",frame=frame,group=name)
            else: bone.keyframe_insert("rotation_quaternion",frame=frame,group=name)
    transition["BCMM_Status"]="Original transition blockout; export/runtime visual review pending"
rig.animation_data.action=loop_action
scene.frame_set(10)
# Do not carry embedded rig UI Python into the derived source.
for script in list(bpy.data.texts):
    if script.name.endswith(".py"): bpy.data.texts.remove(script)
scene=bpy.context.scene
scene.frame_start=1;scene.frame_end=19;scene.render.fps=30
scene["BCMM_Status"]="Original CPR prototype; native export/runtime accepted; updated hand and knee contact under review"
scene["BCMM_OriginalMotion"]="BCM medical CPR motion, 2026-10-26"
scene["BCMM_RigAttribution"]="Bohemia Interactive Arma-Reforger-Samples official Character_AnimationRig_Example.blend; Arma Public Licence"
def material(name,color):
    mat=bpy.data.materials.new(name);mat.diffuse_color=(*color,1)
    mat.use_nodes=True
    bsdf=mat.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value=(*color,1)
    bsdf.inputs["Roughness"].default_value=.75
    return mat
cloth=material("BCMM_preview_uniform",(.18,.23,.19))
skin=material("BCMM_preview_skin",(.55,.37,.26))
dark=material("BCMM_preview_dark",(.04,.055,.055))
hidden=("WGT-","Reference","Canteen","BreadBag","FAK_","Helmet","Magazine","AK74","M16","Grenade","Pouch","Radio","Shovel","Bayonet")
for obj in bpy.data.objects:
    if obj.type!="MESH": continue
    if any(token.lower() in obj.name.lower() for token in hidden):
        obj.hide_render=True;continue
    # Sample preview materials may reference textures not shipped with the rig.
    obj.data.materials.clear()
    mat=skin if any(t in obj.name.lower() for t in ("head_lod","hand","body")) else cloth
    if "eye" in obj.name.lower(): mat=dark
    obj.data.materials.append(mat)
def primitive(name,location,scale,mat):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=12,location=location)
    obj=bpy.context.object;obj.name=name;obj.scale=scale;obj.data.materials.append(mat)
    return obj
dummy=material("BCMM_original_dummy",(.25,.32,.36))
primitive("BCMM_Casualty_torso",(0,-.55,.22),(.37,.22,.15),dummy)
primitive("BCMM_Casualty_head",(-.57,-.55,.19),(.14,.13,.15),dummy)
for side in (-1,1):
    primitive("BCMM_Casualty_leg",(side*.0+.62,-.55+side*.11,.11),(.32,.09,.09),dummy)
floor=material("BCMM_preview_floor",(.12,.14,.15))
bpy.ops.mesh.primitive_plane_add(size=20,location=(0,0,-.02))
bpy.context.object.data.materials.append(floor)
for obj in list(bpy.data.objects):
    if obj.type in {"CAMERA","LIGHT"}: bpy.data.objects.remove(obj,do_unlink=True)
bpy.ops.object.camera_add(location=(2.9,-3.6,2.1))
camera=bpy.context.object
camera.rotation_euler=(Vector((0,-.12,.65))-camera.location).to_track_quat("-Z","Y").to_euler()
camera.data.type="ORTHO";camera.data.ortho_scale=2.7
scene.camera=camera
for location,power,size in (((2,-3,5),1300,5),((-3,1,3),1000,4)):
    bpy.ops.object.light_add(type="AREA",location=location)
    light=bpy.context.object;light.data.energy=power;light.data.shape="DISK";light.data.size=size
    light.rotation_euler=(Vector((0,0,.7))-light.location).to_track_quat("-Z","Y").to_euler()
scene.render.engine="BLENDER_EEVEE_NEXT"
scene.render.resolution_x=1200;scene.render.resolution_y=900;scene.render.resolution_percentage=100
scene.world.color=(.16,.16,.16)
scene.frame_set(1);bpy.context.view_layer.update()
metrics={}
for side in ('L','R'):
    centre,along,normal=palm_frame(side)
    metrics[side]={'palm_centre':list(centre),'finger_direction':list(along),'palm_normal':list(normal)}
metrics['knees']={word:list(native.pose.bones[word+'Knee'].head) for word in ('Left','Right')}
metrics['knee_spacing_metres']=abs(native.pose.bones['LeftKnee'].head.x-native.pose.bones['RightKnee'].head.x)
(root/'.checks/CPR-pose-refinement.json').write_text(json.dumps(metrics,indent=2),encoding='utf-8')
scene.frame_set(10)
scene.render.filepath=str(out/"BCMM_CPR_blockout_preview.png")
bpy.ops.wm.save_as_mainfile(filepath=str(out/"BCMM_CPR_Blockout.blend"))
bpy.ops.render.render(write_still=True)
print("BCMM_CPR_SOURCE_BLOCKOUT_COMPLETE")
