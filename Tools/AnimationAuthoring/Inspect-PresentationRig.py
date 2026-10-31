import bpy,json
from pathlib import Path
root=Path(__file__).resolve().parents[2]
rig=bpy.data.objects['rig'];native=bpy.data.objects['Character']
rig.animation_data.action=bpy.data.actions['BCMM_CPR_enter_BLOCKOUT'];bpy.context.scene.frame_set(1);bpy.context.view_layer.update()
names=[b.name for b in rig.pose.bones if any(x in b.name.lower() for x in ('thumb','f_index','f_middle','f_ring','f_pinky','hand','forearm','upper_arm'))]
report={'controls':names,'native':{b.name:{'matrix':[list(row) for row in b.matrix],'head':list(b.head),'tail':list(b.tail)} for b in native.pose.bones if any(x in b.name for x in ('Hand','Arm','Shoulder'))}}
(root/'.checks/presentation-rig.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('BCMM_PRESENTATION_CONTROLS',names)
