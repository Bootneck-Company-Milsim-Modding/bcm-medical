"""Export only original BCM casualty loops through the official EBT profile."""
import bpy,sys,socket,zipfile,math,json
from pathlib import Path
root=Path(__file__).resolve().parents[2]
stage=root/'Source/Animations/Casualty'
profiles=root/'.checks/casualty-pose-export-profile';profiles.mkdir(parents=True,exist_ok=True)
tools=Path(r'C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger Tools')
sys.path.insert(0,str(tools/'Blender/scripts/addons'))
from EnfusionBlenderTools.txa.operators import AnimationExportChannel,EBT_OT_export_txa
from enfusion import workbench
OriginalSocket=socket.socket
class BoundedSocket(OriginalSocket):
    def setblocking(self,enabled):
        super().setblocking(enabled)
        if enabled:self.settimeout(30)
socket.socket=BoundedSocket
connection=workbench.WorkbenchConnectionInfo('Original BCM casualty poses',5775)
profile='101_FullBodyABS'
result=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile})
channels=result.get('Channels')
if not channels:
    with zipfile.ZipFile(tools/'Blender/EBT-ArmaReforger-Data.zip') as archive:
        (profiles/(profile+'.apr')).write_bytes(archive.read('anims/export_profiles/'+profile+'.apr'))
    workbench.call_function(connection,'AnimExportProfiles',{'ExportProfileAbsPath':str(profiles)})
    channels=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile}).get('Channels')
if not channels:raise RuntimeError('Native animation export profile unavailable')
rig=bpy.data.objects['rig'];character=bpy.data.objects['Character'];reports=[]
kinds=('Carried',) if '--carry-only' in sys.argv else ('Carried','Dragged')
for kind in kinds:
    stem='BCMM_'+kind+'_loop';rig.animation_data.action=bpy.data.actions[stem]
    tracks=EBT_OT_export_txa.create_channels_from_profile(channels,[character,rig])
    if any(not ch.is_valid() for ch in tracks):raise RuntimeError('Missing skeleton channel')
    for frame in range(1,32):
        bpy.context.scene.frame_set(frame);bpy.context.view_layer.update()
        for ch in tracks:ch.sample_key_frame()
    data=AnimationExportChannel.linearize(tracks)
    if not all(math.isfinite(v) for v in data):raise RuntimeError('Non-finite original pose')
    target=stage/(stem+'.txa')
    result=workbench.call_function(connection,'AnimExportTrackExport',{'SourceFile':'BCMM_CasualtyPoses.blend','TargetFile':str(target),'Profile':profile,'NumChannels':len(tracks),'NumKeyframes':31,'Fps':30,'KeyData':data,'DiffData':None})
    if result.get('Result')!='OK' or not target.is_file():raise RuntimeError('Native export rejected '+stem+': '+str(result))
    reports.append({'stem':stem,'channels':len(tracks),'frames':31})
    print('BCMM_CASUALTY_EXPORTED '+stem,flush=True)
(root/'.checks/casualty-pose-export-report.json').write_text(json.dumps(reports,indent=2))
