"""Export original field-procedure prototypes through installed official EBT.
Run Blender with BCMM_FieldProcedures_Blockout.blend; requires Workbench API.
"""
import bpy,sys,os,json,math,socket,zipfile
from pathlib import Path
root=Path(__file__).resolve().parents[2]
tools=Path(os.environ.get('BCMM_REFORGER_TOOLS',r'C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger Tools'))
sys.path.insert(0,str(tools/'Blender/scripts/addons'))
from EnfusionBlenderTools.txa.operators import AnimationExportChannel,EBT_OT_export_txa
from enfusion import workbench
OriginalSocket=socket.socket
class BoundedSocket(OriginalSocket):
 def setblocking(self,enabled):
  super().setblocking(enabled)
  if enabled:self.settimeout(60)
socket.socket=BoundedSocket
connection=workbench.WorkbenchConnectionInfo('BCM field procedure export',5775)
profile='101_FullBodyABS'
print('BCMM_FIELD_QUERY_PROFILE',flush=True)
result=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile})
channels=result.get('Channels')
if not channels:
 profiles=root/'.checks/animation-export/field-profile';profiles.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(tools/'Blender/EBT-ArmaReforger-Data.zip') as archive:
  (profiles/(profile+'.apr')).write_bytes(archive.read('anims/export_profiles/'+profile+'.apr'))
 loaded=workbench.call_function(connection,'AnimExportProfiles',{'ExportProfileAbsPath':str(profiles)})
 if profile not in loaded.get('ProfileNames',[]):raise RuntimeError('Native profile registration failed: '+str(loaded))
 result=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile});channels=result.get('Channels')
if not channels:raise RuntimeError('Native export profile unavailable: '+str(result))
rig=bpy.data.objects['rig'];character=bpy.data.objects['Character']
manifest_name=next((a.split('=',1)[1] for a in sys.argv if a.startswith('--bcmm-manifest=')),'field-procedures.json')
manifest=json.loads((root/'Tools/AnimationAuthoring'/manifest_name).read_text())
report=[]
endpoints={}
for clip in manifest:
 rig.animation_data.action=bpy.data.actions[clip['action']]
 tracks=EBT_OT_export_txa.create_channels_from_profile(channels,[character,rig])
 if any(not ch.is_valid() for ch in tracks):raise RuntimeError('Missing native skeleton channel')
 for frame in range(1,clip['frames']+1):
  bpy.context.scene.frame_set(frame);bpy.context.view_layer.update()
  for ch in tracks:ch.sample_key_frame()
 data=AnimationExportChannel.linearize(tracks)
 if not all(math.isfinite(v) for v in data):raise RuntimeError('Nonfinite transform in '+clip['stem'])
 first=[v for ch in tracks for row in ch.keyframes[0] for v in row]
 last=[v for ch in tracks for row in ch.keyframes[-1] for v in row]
 seam=max(abs(a-b) for a,b in zip(first,last))
 if clip['loop'] and seam>0.0001:raise RuntimeError('Loop seam in '+clip['stem']+': '+str(seam))
 endpoints[clip['stem']]=(first,last)
 payload={'SourceFile':Path(bpy.data.filepath).name,'TargetFile':str(root/'Source/Animations'/(clip['stem']+'.txa')),'Profile':profile,'NumChannels':len(tracks),'NumKeyframes':clip['frames'],'Fps':clip['fps'],'KeyData':data,'DiffData':None}
 print('BCMM_FIELD_EXPORTING '+clip['stem'],flush=True)
 accepted=workbench.call_function(connection,'AnimExportTrackExport',payload)
 if accepted.get('Result')!='OK':raise RuntimeError('Native export rejected '+clip['stem']+': '+str(accepted))
 if not Path(payload['TargetFile']).is_file():raise RuntimeError('Missing TXA '+clip['stem'])
 report.append({'stem':clip['stem'],'frames':clip['frames'],'channels':len(tracks),'seam_max_error':seam if clip['loop'] else None})
 print('BCMM_FIELD_EXPORTED '+clip['stem'],flush=True)
for kind in sorted({c['stem'].replace('BCMM_','').rsplit('_',1)[0] for c in manifest}):
 base='BCMM_'+kind
 loop=endpoints[base+'_loop'][0]
 enter=endpoints[base+'_enter'][1];exit=endpoints[base+'_exit'][0]
 error=max(abs(a-b) for point in (enter,exit) for a,b in zip(loop,point))
 if error>0.0001:raise RuntimeError('Transition discontinuity '+kind+': '+str(error))
(root/('.checks/'+manifest_name.replace('.json','')+'-export.json')).write_text(json.dumps({'clips':report,'status':'Native TXA export accepted; compiled playback acceptance separate'},indent=2),encoding='utf-8')
print('BCMM_FIELD_EXPORT_COMPLETE clips='+str(len(report)),flush=True)
