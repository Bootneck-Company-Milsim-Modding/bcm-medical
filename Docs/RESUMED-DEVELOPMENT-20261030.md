# Resumed development — 30 October 2026

Current engineering checkpoint: the agreed player-focused changes passed all 51 validation stages with documented diagnostic-only continuity, 12 packed-only phases and a packed dedicated-server/three-client transport test (701/0). The private package is built. See [PLAYER-TEST-CANDIDATE-20261030.md](PLAYER-TEST-CANDIDATE-20261030.md) for exact evidence, package location and remaining human/Internet/art acceptance. The latest carry revision is not marked visually approved by the user. Earlier checkpoint paragraphs below are historical where superseded.

The user approved the black sling and explicitly requested import and continued development until complete or stopped. Recording has finished. Native Workbench/game/server tests and private packing are authorized. The later explicit animation exception permits **two original casualty poses**, with existing rescuer walking/crouching retained and no ACE animation data. Other animation changes, ACE copying/dependency, autonomous medical AI, filled-bag carry and two-player stretcher carry remain outside scope. Git and publication remain deferred.

## Current implementation

- Approved original sling exported to `Assets/Medical/WeaponSling`, seven native PBR materials, 17,999 visual triangles and simple collision. Native Workbench compilation completed in `.checks/sling-native-build-20261030-132155`. Inventory-only `Prefabs/Items/BCMM_WeaponSling.et` inherits the native non-consumable item base. Added to US/USSR/FIA catalogues, now 29 BCM inventory entries each.
- Appended eight GM settings: weapon retention policy, sling supply cost, living carry/drag enables, range, carry/drag speed fractions and movement break distance. Catalogue now 279, saved schema35. Prior schema34 requires its original complete 271 keys and imports defaults for the additions; schema33 still requires 270 keys.
- Original weapon retention captures the exact held rifle/sniper rifle/machine gun at BCM's unconscious transition. A native post-drop callback restores the same entity through inventory after the original transaction. No weapon/attachments/ammo are cloned. Default requires an inventory sling; GM can select native drop or retention without sling. Player-only; pistols, launchers and live grenades retain native handling. Focused native inventory and actual-owner dedicated-server checks pass; complete current-build regression remains required.
- Original living-transport source and medical-menu request/accept/decline/cancel/release buttons compile. Pending offers bind exact character references, players, action and expiry. Unconscious casualties have no consent prompt. Conscious players require acceptance; NPC casualties use eligibility without a human prompt. Native character state is retained. Two original static casualty loops play through base-game compartment animation injection; rescuers retain existing walking/crouch locomotion.
- Carry/drag maintain original patient entities, reduce movement, block jump/vault and leave patient control/gravity/collision handling to the native compartment lifecycle. The transport input restriction ends on release; no stale unconscious control snapshot is restored. Ongoing floor/path, life, vehicle, source-character and movement-break checks release unsafe sessions. Owner/proxy/JIP functional tests pass; visual posture acceptance remains open. Do not call this player-test ready yet.
- Transport buttons are significantly smaller, side by side at bottom right. The central anatomy area has been restored to its larger height. Incoming offers change the pair to ACCEPT / DECLINE; pending/active states provide CANCEL / RELEASE.
- ACE casualty pose comparison is documented in [CASUALTY-POSE-REFERENCE-20261030.md](CASUALTY-POSE-REFERENCE-20261030.md). The user authorized two original casualty poses if needed and accepted the drag draft. A rejected twisted carry draft was rebuilt with a visible static carrier. New original loops were exported through the official EBT profile; no existing animation was changed. Native compartment holders now play the clips on conscious and unconscious original characters. Current visual/multiplayer acceptance remains in progress.

## Native evidence in this run

- `.checks/20261030-131505-994`: production scripts compile.
- `.checks/20261030-131609-536`: diagnostic resources/scripts compile.
- `.checks/20261030-131648-646`: clinical safety 704 assertions, zero failures, after AI removal/progress-only changes and before later feature additions.
- `.checks/20261030-131826-873`: full field procedures 694 assertions, zero failures, progress-only timers/cancellation/consumption; before later feature additions.
- `.checks/20261030-132254-364`: core 670/0, before new settings/retention/transport.
- `.checks/20261030-133746-429`: live transport source and tests compile, before release-owner hardening.
- `.checks/20261030-133815-612`: sling/retention fixture 716/0 across five isolated native cases: no-sling drop, sling retention, always-retain without sling, normal drop with sling, intentional conscious drop. Actual original rifle identity and non-consumed inventory sling exercised.
- `.checks/20261030-134016-907`: living transport 713/0 across actual unconscious carry, crouch drag and conscious broken-leg NPC carry, small movement and release, original collision/control/consciousness preservation. Before subsequent explicit owner-release placement hardening.
- `.checks/20261030-134029-676`: existing native UI smoke 26/0. This is narrower than new transport-interface interaction/accessibility acceptance.

Failed compilation/fixture attempts remain in `.checks`. Wake/equip reuse timing was resolved by isolating actors per retention scenario. The first release test incorrectly expected an unconscious patient's original controls to be enabled; it now compares the captured original value. No failed run is counted as acceptance.

## Continue next

### Later native evidence (scope-specific)

- `.checks/living-transport-network-20261030-142953-507/result.json`: Passed=true, SourceUnchanged=true; dedicated authority 693/0, actual rescuer/patient owners and late join. Carry/drag request, decline, accept, timeout, release and exact original rifle retention with non-consumed sling exercised. Private inventory checked on server and actual patient owner, not arbitrary proxies.
- `.checks/20261030-142343-413`: local living-transport fixture 715/0; includes strict schema34 migration assertions. Conscious NPC visual posture remains a separate diagnostic limitation and is not accepted as a pose reference.
- `.checks/20261030-140041-493`: native medical UI 28/0, screenshot verifies compact bottom-right controls and larger anatomy area.
- `.checks/persistence-current-20261030-140719-812`: separate-process settings seed/restart 682/701 assertions, zero failures.
- `.checks/20261030-135656-149`: native faction arsenals 731/0, 29 BCM items per faction.
- Full current clinical acceptance is running with added sling, local living-transport and multiplayer transport gates. A running suite is not acceptance.

Compile/test the latest owner-release/actor-deletion hardening. Add real dedicated owner/patient/proxy/JIP tests for consent, rejection, cancellation, timeout, owner replacement/disconnect, moving/blocked/released casualty and exact weapon inventory/attachments/ammunition. Exercise menu controls and render geometry at supported resolutions; native prone/collapsed posture is not yet visually accepted. Tighten transport sweeps, release placement and any treatment contention discovered. Then refresh schema34 migration/restart and full current clinical/finite inventory/limited-lives regression. Create and verify a matching private packed-only candidate before human dedicated player testing; authenticated backend/Internet/load and final provenance remain external release acceptance, not simulated passes.

## Corrected native pose and movement checkpoint

Dedicated test .checks/living-transport-network-20261030-163629-858 passed 701 assertions, zero failures, with unchanged source during the run. Actual rescuer input walked and turned while carrying and crouch-dragging; all three clients observed original casualty pose holders. Carry moved 1.826m and drag 0.990m during the bounded input window. The movement cap now composes with the clinical/stretcher control hook instead of being overwritten. Wake release, owner control restoration, cancellation replay and original rifle/sling retention also passed. A prior run caught a drag interruption; full regression must establish the final snapshot. Temporary diagnosis logging was removed before the combined run.

Full 51-gate current-source acceptance is running under .checks/clinical-acceptance-20261030-163814-109. No complete pass or current private packed acceptance is claimed yet.


The combined 16:38 run stopped at the network fixture's later unconscious pickup after native movement. The fixture now approaches again before later pickups and prevents its cancel button step from immediately issuing a fresh offer. The first corrected focused rerun exposed that duplicate-request fixture error; neither failed run is accepted. Production clinical/transport source was unchanged by these fixture corrections. Private packing remains gated on complete acceptance.


## Final regression snapshot in progress

The native character-as-ground bug is fixed and independently reproduced by the local regression test (748/0). Corrected dedicated lifecycle passed 701/0. A fresh 51-gate full run is .checks/clinical-acceptance-20261030-165059-895. Private candidate .checks/package-development/candidate-20261030-165119-628 is hash-checked and isolated; the pipeline will reject any source drift, failed gate or native packing failure. Neither packing nor whole-mod completion is claimed while that run is pending.


## Current pose revision and frozen validation

Following the user's lower-body/right-arm rejection, all old acceptance/packing queues were stopped. The carry was rebuilt with a connected joint chain and right arm clear of the carrier; head and left arm are preserved. Local native front/rear inspection .checks/20261030-171454-125 passed 748/0. Visual acceptance by the user remains open. The current full 51-gate snapshot is .checks/clinical-acceptance-20261030-171645-189; private candidate .checks/package-development/candidate-20261030-171720-777 is gated on that exact pass and unchanged source. Earlier full runs are failed/interrupted, not accepted.


## Diagnostic-only continuation

The 17:16 full run passed 23 stages, then stopped because the oxygen-network diagnostic still expected 28 catalogue items and returned before submitting an oxygen request. The actual catalogue correctly contained 29 with the sling. Only that diagnostic and its runner changed to 29; all production files and all earlier-stage fixtures remain unchanged. Focused oxygen networking then passed 698/0 (.checks/oxygen-medication-network-20261030-173634-619).

The continuation report is .checks/clinical-acceptance-20261030-174026-538. It explicitly retains the 23 unaffected passed stages with original evidence links, verifies the exact two diagnostic-only changes and all 1,242 production files against the isolated copy, then runs the failed and remaining stages. This is hash-checked production continuity, not a claim that the earlier failed run passed. The continuation's final result and source hashes govern private packing.

