"""Index retained final-package network logs without turning missing evidence into a pass."""
from pathlib import Path
import hashlib
import json
import re

root = Path(__file__).resolve().parents[1]
marker = json.loads((root / '.checks/network-final-snapshot.json').read_text(encoding='utf-8-sig'))
snapshot = Path(marker['snapshot'])
production = Path(marker['production'])
digest = hashlib.sha256((production / 'data.pak').read_bytes()).hexdigest()
if digest.lower() != marker['sha256'].lower():
    raise SystemExit('Network package differs from the recorded snapshot marker')

records = []
for run in sorted((snapshot / '.checks').glob('*-network-*')):
    if not run.is_dir():
        continue
    logs = []
    for path in sorted(run.rglob('console.log')):
        text = path.read_text(encoding='utf-8-sig', errors='replace')
        normalized = text.replace('\\', '/')
        package_lines = [line for line in normalized.splitlines() if 'name=MCBMedical path=' in line]
        expected_cache = production.as_posix() + '/resourceDatabase.rdb'
        loose = any(('path=' + expected_cache + ')') not in line for line in package_lines)
        packed = ("Adding package '" + production.as_posix() + "/' (pak count: 1)") in normalized
        completions = re.findall(r'BCMM_CHECK COMPLETE checks=(\d+) failures=(\d+) run=(\w+)', text)
        errors = sorted(set(re.findall(r'\(E\): (.*)', text)))
        logs.append({
            'path': str(path.relative_to(root)),
            'sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
            'packedMounted': packed and bool(package_lines) and not loose,
            'looseProductionMounted': loose,
            'completions': [{'checks': int(c), 'failures': int(f), 'token': t} for c, f, t in completions],
            'failureMarkers': re.findall(r'[^\r\n]*(?:BCMM_CHECK FAILED|Virtual Machine Exception|RplLayoutError|RplLoadError|ReplicationError)[^\r\n]*', text),
            'observations': sorted(set(re.findall(r'BCMM_[A-Z_]+(?:OBSERVED|PASSED|COMPLETE|SENT)[^\r\n]*', text))),
            'engineErrors': errors,
        })
    result_path = run / 'result.json'
    driver_result = json.loads(result_path.read_text(encoding='utf-8-sig')) if result_path.exists() else None
    records.append({'run': str(run.relative_to(root)), 'driverResult': driver_result, 'logs': logs,
                    'note': 'Log index only. Missing driver result is not a synthesized pass; consult the retained driver transcript and required observations.'})

control_marker = root / '.checks/native-network-control-latest.txt'
control = None
if control_marker.exists():
    directory = Path(control_marker.read_text(encoding='utf-8-sig').strip())
    control = {'run': str(directory.relative_to(root)), 'errors': sorted(set(
        error for path in directory.rglob('console.log')
        for error in re.findall(r'\(E\): (.*)', path.read_text(encoding='utf-8-sig', errors='replace'))))}

inventory_control = None
inventory_marker = root / '.checks/native-inventory-control-latest.txt'
if inventory_marker.exists():
    directory = Path(inventory_marker.read_text(encoding='utf-8-sig').strip()) / 'native-control'
    control_logs = []
    for path in sorted(directory.rglob('console.log')):
        content = path.read_text(encoding='utf-8-sig', errors='replace')
        control_logs.append({
            'path': str(path.relative_to(root)),
            'sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
            'inventoryOpened': 'NATIVE_INVENTORY_CONTROL_OPEN' in content,
            'productionMounted': 'name=MCBMedical path=' in content,
            'engineErrors': sorted(set(re.findall(r'\(E\): (.*)', content))),
        })
    inventory_control = {'run': str(directory.relative_to(root)), 'logs': control_logs,
                         'scope': 'Base-game inventory with a diagnostic deck/script addon; no BCM production code or UI.'}

output = root / 'Docs/Beta01/Evidence/final-packed-network.json'
output.write_text(json.dumps({'packedSha256': digest, 'snapshot': str(snapshot.relative_to(root)),
                             'nativeControl': control, 'nativeInventoryControl': inventory_control,
                             'runs': records}, indent=2) + '\n', encoding='utf-8')
print(f'Indexed {len(records)} runs and {sum(len(r["logs"]) for r in records)} logs: {output}')
