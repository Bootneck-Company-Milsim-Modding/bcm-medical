"""Validate native skeleton sampling and stage original motion for EBT export.
Run using Blender --background --disable-autoexec source.blend --python this_file.
No TXA/ANM is claimed or created without Workbench acceptance.
"""
import sys, os, re, json, math, zipfile
from pathlib import Path
import bpy
root=Path(__file__).resolve().parents[2]
tools=Path(os.environ.get('BCMM_REFORGER_TOOLS',r'C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger Tools'))
sys.path.insert(0,str(tools/'Blender/scripts/addons'))
from EnfusionBlenderTools.txa.operators import AnimationExportChannel, EBT_OT_export_txa
profile='101_FullBodyABS'
with zipfile.ZipFile(tools/'Blender/EBT-ArmaReforger-Data.zip') as archive:
    content=archive.read('anims/export_profiles/'+profile+'.apr').decode('utf-8')
channels={}
for line in content.splitlines():
    match=re.match(r'\s*"([^"]+)"\s+"([^"]*)"\s+"([^"]+)"',line)
    if match:
        props={}
        generated=re.search(r'\$genFn\s*\{\s*"([^"]+)"',line)
        if generated: props['GenFn']=generated.group(1)
        channels[match.group(1)]=props
# Workbench is the authority on the actual export profile; local parsing is
# sufficient only for a source check and must not authorize a TXA export.
live_export='--workbench' in sys.argv
connection=None
if live_export:
    import socket
    # EBT explicitly enables blocking mode. Bound that mode locally so an
    # unresponsive editor cannot leave the build process waiting indefinitely.
    OriginalSocket=socket.socket
    class BoundedSocket(OriginalSocket):
        def setblocking(self, enabled):
            super().setblocking(enabled)
            if enabled: self.settimeout(20)
    socket.socket=BoundedSocket
    from enfusion import workbench
    connection=workbench.WorkbenchConnectionInfo('BCM Medical animation build',5775)
    print('BCMM_EXPORT_QUERY_CHANNELS',flush=True)
    native=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile})
    if not native.get('Channels'):
        profiles=root/'.checks/animation-export/current-profile'
        profiles.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(tools/'Blender/EBT-ArmaReforger-Data.zip') as archive:
            (profiles/(profile+'.apr')).write_bytes(archive.read('anims/export_profiles/'+profile+'.apr'))
        listed=workbench.call_function(connection,'AnimExportProfiles',{'ExportProfileAbsPath':str(profiles)})
        if profile not in listed.get('ProfileNames',[]): raise RuntimeError('Workbench did not accept full-body export profile: '+str(listed))
        native=workbench.call_function(connection,'AnimExportProfileChannels',{'ProfileName':profile})
    channels=native.get('Channels')
    if not channels: raise RuntimeError('Workbench returned no export channels: '+str(native))
character=bpy.data.objects.get('Character')
rig=bpy.data.objects.get('rig')
if not character or not rig: raise RuntimeError('Official native and control armatures required')
tracks=EBT_OT_export_txa.create_channels_from_profile(channels,[character,rig])
missing=[ch.name for ch in tracks if not ch.is_valid()]
if missing: raise RuntimeError('Profile channels missing: '+', '.join(missing))
frames=list(range(1,20))
positions=[]
for frame in frames:
    bpy.context.scene.frame_set(frame)
    bpy.context.view_layer.update()
    for ch in tracks: ch.sample_key_frame()
    positions.append({name:list(character.pose.bones[name].matrix.translation) for name in ['LeftHand','RightHand','Hips']})
values=AnimationExportChannel.linearize(tracks)
if not all(math.isfinite(value) for value in values): raise RuntimeError('Non-finite exported transform')
seam=max(abs(a-b) for ch in tracks for row_a,row_b in zip(ch.keyframes[0],ch.keyframes[-1]) for a,b in zip(row_a,row_b))
travel=max(p['LeftHand'][2] for p in positions)-min(p['LeftHand'][2] for p in positions)
if seam>0.0001: raise RuntimeError('Loop seam discontinuity '+str(seam))
if travel<0.005: raise RuntimeError('Native skeleton does not follow the authored control animation')
output=root/'.checks/animation-export'
output.mkdir(parents=True,exist_ok=True)
payload={'SourceFile':Path(bpy.data.filepath).name,'TargetFile':str(root/'Source/Animations/BCMM_CPR_loop.txa'),'Profile':profile,'NumChannels':len(tracks),'NumKeyframes':len(frames),'Fps':30,'KeyData':values,'DiffData':None}
(output/'CPR-export-payload.json').write_text(json.dumps(payload))
if live_export:
    accepted=workbench.call_function(connection,'AnimExportTrackExport',payload)
    if accepted.get('Result')!='OK': raise RuntimeError('Workbench rejected TXA export: '+str(accepted))
    target=Path(payload['TargetFile'])
    if not target.is_file() or target.stat().st_size==0: raise RuntimeError('No exported TXA was written')
    print('BCMM_WORKBENCH_TXA_EXPORTED '+str(target),flush=True)
report={'source':Path(bpy.data.filepath).name,'profile':profile,'channels':len(tracks),'frames':len(frames),'finite_values':len(values),'loop_seam_max_error':seam,'native_hand_vertical_motion_metres':travel,'status':'Validated source sampling only; Workbench export and game integration pending'}
loop_pose={ch.name:ch.keyframes[0] for ch in tracks}
transition_report={}
for name in ('BCMM_CPR_enter_BLOCKOUT','BCMM_CPR_exit_BLOCKOUT'):
    action=bpy.data.actions.get(name)
    if not action: raise RuntimeError('Missing transition '+name)
    rig.animation_data.action=action
    sampled=[]
    for frame in (1,31):
        bpy.context.scene.frame_set(frame)
        bpy.context.view_layer.update()
        sample=EBT_OT_export_txa.create_channels_from_profile(channels,[character,rig])
        for ch in sample: ch.sample_key_frame()
        sampled.append({ch.name:ch.keyframes[0] for ch in sample})
    working_end=sampled[1] if 'enter' in name else sampled[0]
    error=max(abs(a-b) for channel in loop_pose for row_a,row_b in zip(loop_pose[channel],working_end[channel]) for a,b in zip(row_a,row_b))
    if error>0.0001: raise RuntimeError('Transition does not join loop '+name+': '+str(error))
    transition_report[name]={'frames':31,'loop_join_max_error':error}
    if live_export:
        transition_tracks=EBT_OT_export_txa.create_channels_from_profile(channels,[character,rig])
        for frame in range(1,32):
            bpy.context.scene.frame_set(frame)
            bpy.context.view_layer.update()
            for channel in transition_tracks: channel.sample_key_frame()
        transition_payload=dict(payload)
        suffix='enter' if 'enter' in name else 'exit'
        transition_payload['TargetFile']=str(root/'Source/Animations'/('BCMM_CPR_'+suffix+'.txa'))
        transition_payload['NumKeyframes']=31
        transition_payload['KeyData']=AnimationExportChannel.linearize(transition_tracks)
        accepted=workbench.call_function(connection,'AnimExportTrackExport',transition_payload)
        if accepted.get('Result')!='OK': raise RuntimeError('Workbench rejected transition export: '+str(accepted))
        if not Path(transition_payload['TargetFile']).is_file(): raise RuntimeError('Missing transition TXA')
        print('BCMM_WORKBENCH_TXA_EXPORTED '+transition_payload['TargetFile'],flush=True)
report['transitions']=transition_report
if live_export: report['status']='Workbench accepted all three TXA clips; ANM compilation and in-game review pending'
(output/'CPR-source-validation.json').write_text(json.dumps(report,indent=2))
print('BCMM_ANIMATION_SOURCE_VALIDATED '+json.dumps(report))