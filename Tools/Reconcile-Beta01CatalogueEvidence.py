"""Link reviewed artifacts without promoting partial workflow coverage to full acceptance."""
from pathlib import Path
import json

root = Path(__file__).resolve().parents[1]
path = root / 'Docs/Beta01/catalogue.json'
catalogue = json.loads(path.read_text(encoding='utf-8-sig'))
continuation = 'Evidence/continuation-coverage.json'
network = 'Evidence/final-packed-network.json'
clinical = 'Evidence/clinical-campaign-reconciliation.json'
resumed = 'RESUMED-ACCEPTANCE.md'
inventory_only = {'DragRope', 'EmptyBodyBag', 'WeaponSling'}
for index, item in enumerate(catalogue['items']):
    item.setdefault('checks', {})['handPose'] = (
        'INVENTORY_ONLY_NOT_HANDHELD' if item['stem'] in inventory_only else 'REVIEWED_NATIVE_THREE_VIEWS')
    item['worldReview'] = {'record': continuation, 'catalogueIndex': index,
                           'distancesMetres': [0.75, 3, 12],
                           'scope': 'Native captures reviewed; small far-distance items can be subpixel. Not all LODs or graphics presets.'}
    item['handReview'] = {'record': resumed,
                         'scope': 'Native front, side and first-person review for 26 handheld entries; six catalogue injectors reviewed injection-end-up. No claim of perfect hand contact.'}

families = {
    'BCMM_Bagging.c': ('Hold, cancellation, exact recovery and contextual action evidence', [resumed, 'ISSUES.md']),
    'BCMM_Burial.c': ('Dedicated burial/search workflow', [network]),
    'BCMM_Cargo.c': ('Dedicated cargo/search and saved-world workflows', [network]),
    'BCMM_NativeBagDrag.c': ('Native movement, owner/proxy/JIP, collision and impaired-network attempts', [continuation, network]),
    'BCMM_Stretcher.c': ('Supported stationary placement/removal with a real patient owner', [network, resumed]),
    'BCMM_MedicalMenu.c': ('Native UI, H action, owner privacy and procedure progress', [continuation, resumed, network]),
    'BCMM_Settings.c': ('State cycles and dedicated persistence; no 200-cycle per-setting gameplay matrix', ['ISSUES.md', network]),
    'BCMM_LifeContextActions.c': ('Dedicated lives/admin workflow; not every physical editor gesture', [network]),
    'BCMM_MedicRole.c': ('Dedicated role/permission workflow; not every physical editor gesture', [network]),
    'BCMM_ZFluidLineTreatment.c': ('Actual owner line connection/removal with independent finite flow and JIP', [network, continuation]),
    'BCMM_ZOxygenRemovalTreatment.c': ('Dedicated oxygen removal workflow', [network]),
    'BCMM_ProcedureAnimation.c': ('Native treatment animation and repeated cancellation safety', [continuation, network]),
    'BCMM_EquipmentTreatment.c': ('Equipment/oxygen workflows and native cancellation safety', [network, continuation, clinical]),
    'BCMM_MedicationTreatment.c': ('Medication/private-owner workflows and native cancellation safety', [network, continuation, clinical]),
    'BCMM_SplintTreatment.c': ('Repeated native splint treatment and dedicated owner workflow', [resumed, network, clinical]),
    'BCMM_Surgery.c': ('Frozen clinical campaign, including facility rules; preserve its build identity', [clinical]),
    'BCMM_Treatments.c': ('Assessment privacy and frozen clinical workflow evidence', [resumed, clinical]),
    'BCMM_SendHome.c': ('Recorded native send-home/lifecycle cases; preserve each build identity', [resumed, clinical]),
}
for surface in catalogue['surfaces']:
    source = Path(surface['file']).name
    if source == 'BCMM_StretcherCarry.c':
        surface.update(status='SHELVED_NOT_ACCEPTED', relatedEvidence=[resumed],
                       evidenceScope='Two-person stretcher/grip prototype remains shelved; experimental failures retained.')
    elif source == 'BCMM_SettingsGenerated.c':
        surface.update(status='NATIVE_STATE_ONLY', relatedEvidence=['ISSUES.md'],
                       evidenceScope='Canonical settings have 200 state cycles; this does not verify every editor control or gameplay effect.')
    elif source in families:
        scope, records = families[source]
        surface.update(status='RELATED_NATIVE_EVIDENCE_PARTIAL', relatedEvidence=records,
                       evidenceScope=scope + '. Related workflow evidence does not certify every method, UI gesture or configuration.')
    else:
        raise SystemExit('Unmapped surface: ' + surface['name'])
catalogue['coverageNote'] = ('Discovered surfaces with related evidence, not a completeness proof. '
    'Per-setting gameplay/multiplayer/restart counts are not inferred from broad suites. '
    'Records retain build identities and failed attempts; partial workflow evidence is not full acceptance.')
path.write_text(json.dumps(catalogue, indent=2) + '\n', encoding='utf-8')
print(f'Reconciled {len(catalogue["items"])} item reviews and {len(catalogue["surfaces"])} surface references; settings counts unchanged.')
