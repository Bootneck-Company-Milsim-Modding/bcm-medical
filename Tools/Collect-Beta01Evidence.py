"""Collect native evidence without converting assertions into gameplay claims."""
from pathlib import Path
import hashlib,json,re,shutil
from datetime import datetime,timezone
from PIL import Image
root=Path(__file__).resolve().parents[1];out=root/'Docs/Beta01';evidence=out/'Evidence';evidence.mkdir(exist_ok=True)
profiles={
 'clinical-safety-20':'.checks/beta01-packed-20261102-145625-237/acceptance-ClinicalSafety-20261102-160904-054',
 'clinical-safety-cleanup-10':'.checks/beta01-packed-20261102-145625-237/acceptance-ClinicalSafety-20261102-161728-912',
 'bag-action-placement':'.checks/beta01-packed-20261102-145625-237/acceptance-BagActions-20261102-162206-749',
 'ui-4k-clipped':'.checks/beta01-packed-20261102-145625-237/acceptance-UI-20261102-162523-570',
 'ui-4k-fixed':'.checks/beta01-packed-20261102-162550-552/acceptance-UI-20261102-162636-880',
 'ui-720p-fixed':'.checks/beta01-packed-20261102-162550-552/acceptance-UI-20261102-162649-932',
 'ui-ultrawide-fixed':'.checks/beta01-packed-20261102-162550-552/acceptance-UI-20261102-162702-438',
 'packed-drag-server-final':'.checks/packed-drag-clear-corridor-20261102-113004-926/.checks/drag-network-20261102-113008-637/server',
 'packed-drag-owner-final':'.checks/packed-drag-clear-corridor-20261102-113004-926/.checks/drag-network-20261102-113008-637/observer-a',
 'packed-drag-peer-final':'.checks/packed-drag-clear-corridor-20261102-113004-926/.checks/drag-network-20261102-113008-637/observer-b',
 'packed-drag-late-final':'.checks/packed-drag-clear-corridor-20261102-113004-926/.checks/drag-network-20261102-113008-637/late-join',
 'equip-resumed':'.checks/resumed-equip-snapshot-20261102-105207-352/.checks/beta01-item-review-20261102-105228-820',
 'equipment-completed':'.checks/equipment-final-snapshot-20261102-102406-316/.checks/20261102-105133-049',
 'medication-completed':'.checks/equipment-final-snapshot-20261102-102406-316/.checks/20261102-105657-604',
 'txa-completed':'.checks/equipment-final-snapshot-20261102-102406-316/.checks/20261102-105750-302',
 'drag-network-server':'.checks/drag-observers-snapshot-20261102-105525-888/.checks/drag-network-20261102-105619-363/server',
 'drag-network-a':'.checks/drag-observers-snapshot-20261102-105525-888/.checks/drag-network-20261102-105619-363/observer-a',
 'drag-network-b':'.checks/drag-observers-snapshot-20261102-105525-888/.checks/drag-network-20261102-105619-363/observer-b',
 'drag-network-late':'.checks/drag-observers-snapshot-20261102-105525-888/.checks/drag-network-20261102-105619-363/late-join',
 'packed-packing-current':'.checks/beta01-packed-20261102-110632-749/acceptance-Packing-20261102-110936-939',
 'packed-recovery-current':'.checks/beta01-packed-20261102-110632-749/acceptance-Bagging-20261102-110948-399',
 'packed-slopes-current':'.checks/beta01-packed-20261102-110632-749/acceptance-DragMatrix-20261102-111025-512',
 'settings-state':'.checks/20261102-085432-443',
 'hold-cancellation':'.checks/20261102-092331-782',
 'requested-loadout':'.checks/20261102-095500-780',
 'knowledge-boundaries':'.checks/20261102-093329-927',
 'equip-pilot':'.checks/beta01-item-review-20261102-092600-842',
 'equip-campaign':'.checks/beta01-item-review-20261102-093224-436',
 'knowledge-menu':'.checks/visual-final-snapshot-20261102-094805-082/.checks/beta01-item-review-20261102-095712-348',
 'progress-menu-hud':'.checks/visual-final-snapshot-20261102-094805-082/.checks/beta01-item-review-20261102-095247-009',
 'icon-board':'.checks/beta01-item-review-20261102-145406-410',
 'collision-packed-hands':'.checks/beta01-packed-20261102-130428-281/acceptance-Hands-20261102-130527-000',
 'ui-fixed-wall-time-failure':'.checks/beta01-packed-20261102-134409-602/acceptance-UI-20261102-134528-441',
 'ui-acknowledged-deadline':'.checks/beta01-packed-20261102-134409-602/acceptance-UI-20261102-135048-286',
 'human-h-toggle-confirmed':'.checks/toggle-context-review-20261102-144155-774',
 'native-toggle-200':'.checks/beta01-packed-20261102-143953-395/acceptance-Hint-20261102-144316-290',
 'native-toggle-retained-failure':'.checks/beta01-packed-20261102-141928-578/acceptance-Hint-20261102-143850-927',
 'injector-needle-up-hands':'.checks/beta01-packed-20261102-145112-692/acceptance-Hands-20261102-145134-532',
 'menu-context-ui':'.checks/beta01-packed-20261102-145112-692/acceptance-UI-20261102-145201-927',
 'human-h-key-confirmed':'.checks/hint-input-20261102-132443-625',
 'knowledge-network-owner-a':'.checks/knowledge-network-snapshot-20261102-101051-702/.checks/knowledge-network-20261102-101507-723/owner-a',
 'knowledge-network-owner-b':'.checks/knowledge-network-snapshot-20261102-101051-702/.checks/knowledge-network-20261102-101507-723/owner-b',
 'packed-baseline':'.checks/beta01-packed-20261102-101844-508/acceptance-Baseline-20261102-102025-826',
 'packed-recovery':'.checks/beta01-packed-20261102-101844-508/acceptance-Bagging-20261102-102241-251',
 'packed-assessment':'.checks/beta01-packed-20261102-101844-508/acceptance-Assessment-20261102-102320-373',
 'packed-ui':'.checks/beta01-packed-20261102-101844-508/acceptance-UI-20261102-102332-251',
}
pack_marker=json.loads((root/'.checks/beta01-pack-process.json').read_text(encoding='utf-8-sig'))
final_pack=Path(pack_marker['run'])
for result_path in final_pack.glob('acceptance-*/result.json'):
    result=json.loads(result_path.read_text(encoding='utf-8-sig'))
    if result.get('passed'):profiles['final-packed-'+result['case'].lower()]=result_path.parent.relative_to(root).as_posix()
current_snapshot=Path((root/'.checks/drag-start-diagnostic-path.txt').read_text(encoding='utf-8-sig').strip())
for family,roles in {'drag-network':('server','observer-a','observer-b','late-join'),'living-transport-network':('server','owner','patient','late-join')}.items():
    for result_path in sorted((current_snapshot/'.checks').glob(family+'-*/result.json')):
        result=json.loads(result_path.read_text(encoding='utf-8-sig'))
        if result.get('passed',result.get('Passed',False)):
            for role in roles:profiles['current-'+family+'-'+role]=(result_path.parent/role).relative_to(root).as_posix()
report={'collectedUtc':datetime.now(timezone.utc).isoformat(),'results':{},'screenshots':[]}
logs={}
for label,relative in profiles.items():
    directory=root/relative;found=sorted(directory.glob('**/console.log'))
    if not found:continue
    log=found[-1];text=log.read_text(errors='replace');logs[label]=text
    destination=evidence/(label+'.txt');shutil.copyfile(log,destination)
    completed=re.findall(r'BCMM_CHECK COMPLETE checks=(\d+) failures=(\d+) run=(\w+)',text)
    superseded=label=='packed-slopes-current'
    report['results'][label]={'profile':relative,'consoleSha256':hashlib.sha256(log.read_bytes()).hexdigest(),'completion':completed[-1] if completed else None,'evidence':destination.relative_to(out).as_posix(),'status':'SUPERSEDED_FIXTURE' if superseded else 'RECORDED','limitation':'Static wall was moved after spawn; native collision remained horizontal. This run does not prove obstacle rejection.' if superseded else ''}
captures={
 'icons-native':('icon-board','labelled-native-icon-board'),
 'procedure-menu':('progress-menu-hud','active-procedure-menu'),
 'procedure-hud':('progress-menu-hud','active-procedure-closed-menu-hud'),
 'airway-200-switches':('knowledge-menu','airway-after-200-patient-switches'),
 'airway-invalidated':('knowledge-menu','airway-invalidated-after-device-change'),
}
captures['epinephrine-native-grip-and-menu-hint']=('injector-needle-up-hands','hand-10-view-2')
if 'final-packed-hint' in profiles:
    captures['held-injector-medical-menu']=('final-packed-hint','held-injector-medical-menu')
for label,(profile,name) in captures.items():
    source=root/profiles[profile]/'profile/screenshots'/f'{name}.bmp'
    if not source.exists():
        matches=list((root/profiles[profile]).glob('**/'+name+'.bmp'))
        if not matches:continue
        source=matches[0]
    target=evidence/(label+'.png');Image.open(source).save(target)
    report['screenshots'].append({'file':target.relative_to(out).as_posix(),'nativeOriginal':source.relative_to(root).as_posix(),'sha256':hashlib.sha256(target.read_bytes()).hexdigest()})
catalogue=out/'catalogue.json'
if catalogue.exists():
    data=json.loads(catalogue.read_text())
    settings=dict(re.findall(r'BCMM_BETA_SETTING key=(\w+) attempts=(\d+) stateAssertions=800 gameplayCycles=0',logs.get('settings-state','')))
    for setting in data['settings']:
        if setting['key'] in settings:setting.update(stateCycles=int(settings[setting['key']]),status='NATIVE_STATE_ONLY')
    equips={}
    for label in ('equip-campaign','equip-resumed'):
        run_counts={}
        for resource,cycles,failures in re.findall(r'BCMM_BETA_EQUIP_CYCLES resource=(\S+) completed=(\d+) failures=(\d+)',logs.get(label,'')):
            run_counts[resource]=max(run_counts.get(resource,0),int(cycles))
        for resource,cycles in run_counts.items():equips[resource]=equips.get(resource,0)+cycles
    for item in data['items']:
        item.setdefault('checks',{})
        item['nativeEquipCycles']=equips.get(item['resource'],0)
        if item['nativeEquipCycles']:item['checks']['equipUnequip']='NATIVE_CYCLES_RECORDED'
        item['checks']['icon']='REVIEWED_NATIVE_BOARD'
        item['status']='PARTIAL_ACCEPTANCE'
    data['evidenceNote']='Counters are from identified native runs. Equipment cycling is not hand-pose, packing, multiplayer or packed-addon acceptance. Later asset revisions require separate render evidence.'
    catalogue.write_text(json.dumps(data,indent=2)+'\n')
    report['nativeEquipCyclesRecorded']=sum(equips.values())
    report['nativeEquipResourcesRecorded']=len(equips)
report['packedArtifact']={'path':str(final_pack/'packed/data.pak'),'sha256':hashlib.sha256((final_pack/'packed/data.pak').read_bytes()).hexdigest()}
report['releaseAcceptance']='Each result identifies its own build and scope. Historical wall evidence is superseded by corrected spawn-transform fixtures. Recovery reached 202 completed chains; the retained acquisition failure was reproduced as a still-falling fixture actor and resolved by bounded setup waiting without weakening production rejection. Splint reached 200 native instances. H toggling is physically user-confirmed and passed 200 native input-action cycles. Six catalogue injectors were reviewed needle-end-up in three native hand views each; capped models retain their caps. Missing third-party vehicle dependencies, controller/LOD and human group acceptance, and unexecuted gameplay matrices remain explicit gaps.'
pause=root/'.checks/beta01-pause.json'
if pause.exists():report['pause']=json.loads(pause.read_text())
(out/'evidence.json').write_text(json.dumps(report,indent=2)+'\n')
print('Collected',len(report['results']),'native logs and',len(report['screenshots']),'native screenshots;',report.get('nativeEquipCyclesRecorded',0),'equip cycles recorded so far.')
