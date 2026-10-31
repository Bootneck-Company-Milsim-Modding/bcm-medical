# Native medical AI development

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


BCM Medical preserves native bandage behavior and applies its clinical state changes through the existing native treatment callback. A validated private in-reach NPC fracture-aid path is now integrated into production source and has passed its main nine-scenario native regression, followed by a local player-control boundary case. Full autonomous medical AI remains incomplete.

The integrated path considers only NPC patients and active NPC members of the same native group. It requires explicit BCM medic qualification, the original usable splint, ordinary procedure reach, available native utility state and low threat. It preserves player/GM priority orders. Normal patient ticks queue a native utility behavior; the same timed authority procedure handles animation, reservation, consumption and applied support. Qualification loss, deselection and failure cancel only the exact acquired procedure token. The procedure supports the fracture rather than repairing it. No new setting, menu, player-consent behavior or long-distance navigation is introduced.

## Main runtime evidence

Main nine-scenario run `.checks/medical-ai-20261030-051823-967/result.json` passed with unchanged source. All listed assertion totals have zero failures:

| Scenario | Assertions |
| --- | ---: |
| Competing reservation |661 |
| Shared timed care |660 |
| Explicit cancellation |662 |
| Actual range loss |662 |
| Qualification revocation |662 |
| Actual patient deletion |660 |
| Autonomous same-group selection |663 |
| Qualification admission |664 |
| Original-supply admission |667 |

Later `.checks/medical-ai-20261030-052735-822/result.json` passes actual local-player admission/takeover666/0 unchanged after the continued player-patient guard. The full49 sub-gate medical-ai-20261030-055931-814 subsequently passes all ten scenarios together with SourceUnchanged=true. The surrounding full49 clinical regression053005-482 subsequently passes all49 gates unchanged at06:29:17UTC. These focused runs are local native evidence, not live battlefield, backend authentication or Internet/load acceptance.

## Accepted private execution evidence

Installed native version1.8.0.13; production Medical GUID6AB4B3F3FD215919 and base game only. Diagnostic code is private and excluded from distribution.

| Run under `.checks/native-ai-development` | Observed result |
| --- | --- |
| `run-20261030-030625-630` |643 assertions, zero failures, source unchanged. Actual AI world/control/agent and canonical group resources discovered. Resource search requires the native `$ArmaReforger:` root. |
| `run-20261030-030746-904` |646 assertions, zero failures, source unchanged. Explicit native medic behavior-tree request completes actual bleeding treatment and consumes the original physical field dressing. The fixture does not call the healing function. |
| `run-20261030-030903-592` |648 assertions, zero failures, source unchanged. Bleeding persists without supply. After receiving the original dressing, the native medic completes treatment, consumes that dressing and marks the linked BCM wound Bandaged. |
| `run-20261030-042834-755` |647/0 unchanged. Original private native-utility behavior enters the shared timed BCM splint procedure, reserves the original kit, consumes it on completion and leaves the underlying fracture broken. |
| `run-20261030-042904-436` |649/0 unchanged. Behavior failure releases the live shared reservation without healing/consumption; a later attempt uses that original kit successfully. |
| `run-20261030-042938-118` |649/0 unchanged. Actual loss of BCM reach cancels shared AI care and retains the source; retry begins only after returning within reach. |
| `run-20261030-043012-236` |650/0 unchanged. Native bandage behavior is interrupted during actual item animation, retains the original dressing and later completes with it. |
| `run-20261030-043942-637` |650/0 unchanged. Exactly the original two characters form a native group. An actual casualty life-state event produces native group response, original dressing consumption and linked wound update, with no fixture injection of an individual medic behavior. |

The first bandaging cases explicitly request an individual behavior. The later group case exercises native event response, while shared-splint cases assign the target explicitly. They do not establish broad autonomous triage, advanced medical decisions, battlefield navigation or qualification policy. Native animation, movement and item callbacks execute; no shortened treatment or phantom supply is introduced. These earlier native-bandaging experiments remain private prototypes. Qualified NPC fracture aid was subsequently integrated and has the separate main-runtime evidence below.

The explicit behavior log also shows the actor about3.3 metres from the patient near completion. BCM's configured procedure reach is a separate check and was not asserted by that fixture. Consequently this is native compatibility evidence, not acceptance of common BCM reach/reservation enforcement for AI. Add explicit range/obstruction/movement boundaries before promoting broader AI care; do not widen the player limit to make the fixture pass.

## Next acceptance cases

Exact-token cancellation, qualification revocation, contention and patient deletion subsequently passed in the private branch and in main runtime051823-967. A later actual local-player admission/takeover case052735-822 passes666/0 after adding continued player-patient exclusion. The current full49 AI sub-gate055931-814 passes all ten scenarios unchanged; the surrounding clinical regression053005-482 subsequently passes all49 gates unchanged at06:29:17UTC.

The integrated in-reach NPC fracture scheduler observes normal patient physiology ticks and queues native utility care. Autonomous positive, qualification and supply admission have actual native passing evidence; the fixture does not assign care directly. Player consent, approaching distant casualties and broader treatment decisions remain separate requirements.

Earlier group setup attempts failed because the chosen prefab spawned additional TL/AR members. The accepted fixture clears only its own spawn slots and removes only its own auto-spawned nonplayer members before the casualty event; the exact-two-original-character check remains. The failed attempts are preserved in native logs.

The private token-hardened branch subsequently passed contention, positive care, explicit abort, range loss, qualification revocation and actual patient deletion. Autonomous positive/role-admission/supply-ownership cases passed650/651/654 with unchanged source. Supply admission uses normal inventory transfer of the same original kit, not a spawned replacement. Those private results justify the production integration; they do not substitute for the current main runtime gate.

Further requirements include actual battlefield threat/load, broader simultaneous-medics cases, devices/fluids/advanced care, saved-world AI roles and owner/proxy visibility. Approaching distant casualties and player consent remain open. Passing bandaging or in-reach fracture aid alone does not close those requirements.

## Integration constraints

Native `EUnitRole.MEDIC` selection is supply-derived; BCM's GM-assigned medic qualification is separate. Preserve explicit BCM role revocation and saved state. Do not grant qualification merely because a native inventory contains a health item.

Keep native AI movement and threat arbitration. Future BCM procedures must use the same server authority, actor/patient reservation, role, reach, duration, cancellation and original-source accounting as human treatment. Behavior failure must cancel active item use and release reservations without healing or consuming an unused source. No native reference source is copied into the standalone mod.

Current run state is in [BACKGROUND-WORK-QUEUE.md](BACKGROUND-WORK-QUEUE.md); accepted snapshots and failures are in [VALIDATION.md](../VALIDATION.md).



## Native threat-event checks

A fresh private copy under `.checks/medical-ai-threat-development` stages native projectile-flyby threat interruption during an actual shared fracture-aid reservation. It must observe normal utility deselection, original unused kit retention, no healing and a later full-duration retry. The native threat API is used only by the fixture; production code remains unchanged. Private interruption063419-340 passes662/0 and autonomous admission063453-382 passes665/0, both unchanged. The tests are promoted into the default twelve-case main AI validator; main run063604-603 passes all twelve cases unchanged. Packed-only AI064227-646 subsequently passes all twelve cases, with unchanged package/diagnostic hashes and no development-source fallback. These checks establish native threat-event integration, not live ballistic battlefield or load acceptance.
