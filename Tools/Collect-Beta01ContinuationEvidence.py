"""Keep faction, distance, impairment and long-running repetition evidence separate."""
from pathlib import Path
import hashlib
import json
import re
from datetime import datetime

root = Path(__file__).resolve().parents[1]
def read(path):
    return json.loads(path.read_text(encoding='utf-8-sig'))

pack = read(root / '.checks/beta01-pack-process.json')
package = Path(pack['output']) / 'data.pak'
result = {'package': str(package.relative_to(root)),
          'sha256': hashlib.sha256(package.read_bytes()).hexdigest(),
          'activeAIFactions': {}, 'worldDistances': [], 'clinicalCampaigns': [], 'toggleCampaigns': [], 'impairment': []}
for path in sorted(Path(pack['run']).glob('acceptance-*/result.json')):
    entry = read(path)
    if entry.get('case') == 'LivingTransport' and entry.get('activeAI') and entry.get('passed') and entry.get('patientFaction'):
        logs = '\n'.join(p.read_text(encoding='utf-8-sig', errors='replace') for p in path.parent.rglob('console.log'))
        manifests = re.findall(r'BCMM_TRANSPORT_INVENTORY_(BEFORE|AFTER) mode=(\d+) conscious=(\d+) count=(\d+)', logs)
        if len(manifests) != 8:
            continue
        result['activeAIFactions'][entry['patientFaction']] = {
            'result': str(path.relative_to(root)), 'completion': entry['completion'],
            'manifests': [{'boundary': b, 'mode': int(m), 'conscious': bool(int(c)), 'count': int(n)} for b,m,c,n in manifests],
            'scope': 'Four native conscious/unconscious carry/drag cases with active idle AI and exact original inventory identity checks; not autonomous medical care or combat navigation.'}
    if entry.get('case') == 'WorldReview':
        result['worldDistances'].append({'result': str(path.relative_to(root)), 'passed': entry['passed'],
                                        'completion': entry['completion'], 'distancesMetres': [0.75,3,12]})
    if entry.get('case') == 'ClinicalSafety':
        log_paths = list(path.parent.rglob('console.log'))
        logs = '\n'.join(p.read_text(encoding='utf-8-sig', errors='replace') for p in log_paths)
        normalized = logs.replace('\\', '/')
        packed_path = package.parent.as_posix()
        mounts = [line for line in normalized.splitlines() if 'name=MCBMedical path=' in line]
        isolated = bool(mounts) and ("Adding package '" + packed_path + "/' (pak count: 1)") in normalized and all('path=' + packed_path + '/resourceDatabase.rdb)' in line for line in mounts)
        completed = re.findall(r'BCMM_NATIVE_ITERATION completed=(\d+) target=(\d+) checks=(\d+) failures=(\d+)', logs)
        samples = entry.get('processSamples', [])
        resources = None
        if samples:
            first = datetime.fromisoformat(samples[0]['utc'].replace('Z', '+00:00'))
            warm = [s for s in samples if (datetime.fromisoformat(s['utc'].replace('Z', '+00:00')) - first).total_seconds() >= 60]
            resources = {'samples': len(samples), 'elapsedSeconds': (datetime.fromisoformat(samples[-1]['utc'].replace('Z', '+00:00'))-first).total_seconds(),
                         'peakPrivateMiB': max(s['privateBytes'] for s in samples)/1048576,
                         'firstAfterWarmup': warm[0] if warm else None, 'last': samples[-1],
                         'scope': 'One native local client and diagnostic repetition; not dedicated multiplayer soak.'}
        result['clinicalCampaigns'].append({'result': str(path.relative_to(root)), 'passed': entry['passed'],
                                           'requested': entry.get('repeatInstances',1), 'completion': entry['completion'],
                                           'lastIteration': completed[-1] if completed else None, 'resources': resources,
                                           'isolatedPackedMount': isolated,
                                           'logs': [{'path': str(p.relative_to(root)), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in log_paths]})
    if entry.get('case') == 'Hint':
        result['toggleCampaigns'].append({'result': str(path.relative_to(root)), 'passed': entry['passed'],
                                         'nativeAction': entry.get('nativeToggleAction'), 'cycles': entry.get('toggleCycles'),
                                         'completion': entry['completion']})
for path in sorted((root / '.checks').glob('impaired-*/result.json')):
    entry = read(path)
    driver = path.parent / 'driver.txt'
    native_run = None
    if driver.exists():
        match = re.search(r'Run directory: ([^\r\n]+)', driver.read_text(encoding='utf-8-sig', errors='replace'))
        if match:
            native_run = str(Path(match[1]).relative_to(root))
    result['impairment'].append({'result': str(path.relative_to(root)), 'nativeRun': native_run, **entry})
result['limits'] = [
    'A completed native assertion suite does not establish visual perfection or all graphics configurations.',
    'Proxy results describe tested synthetic loopback impairment, not public Internet or console acceptance.',
    'Shelved two-player stretcher and grip prototype failures are retained in the network index; production defaults are unchanged.',
    'Failed and incomplete runs are not added to successful instance counts.'
]
output = root / 'Docs/Beta01/Evidence/continuation-coverage.json'
output.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
print(f'Indexed {len(result["activeAIFactions"])} AI factions and {len(result["impairment"])} impairment attempts')
