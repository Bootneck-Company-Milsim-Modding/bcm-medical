import bpy,json
from pathlib import Path
rig=bpy.data.objects['rig'];native=bpy.data.objects['Character']
rig.animation_data.action=bpy.data.actions['BCMM_CPR_loop_BLOCKOUT'];bpy.context.scene.frame_set(1);bpy.context.view_layer.update()
result={'controls':[b.name for b in rig.pose.bones if any(t in b.name for t in ['f_index','f_middle','f_pinky','thigh_ik','shin','thigh_parent'])]}
for side,word in [('L','Left'),('R','Right')]:
 names=[word+'Hand',word+'HandIndex1',word+'HandPinky1',word+'HandMiddle1',word+'Knee']
 result[side]={n:list(native.pose.bones[n].head) for n in names if n in native.pose.bones}
 result[side]['control_matrix']=[list(r) for r in rig.pose.bones['hand_ik.'+side].matrix]
 result[side]['parent_props']={k:rig.pose.bones['thigh_parent.'+side][k] for k in rig.pose.bones['thigh_parent.'+side].keys()}
result['leg_ik']={b.name:{'head':list(b.head),'tail':list(b.tail),'constraints':[{'type':c.type,'pole_subtarget':getattr(c,'pole_subtarget',''),'subtarget':getattr(c,'subtarget','')} for c in b.constraints]} for b in rig.pose.bones if any(t in b.name for t in ['thigh_ik.','thigh_ik_target','MCH-shin_ik'])}
Path('.checks/CPR-pose-inspection.json').write_text(json.dumps(result,indent=2))
