# Whole-mod completion ledger

Current development:270 settings/schema33. Independent drips and optional fluid burden have focused native acceptance; the33-gate combined run214430-980 passed with unchanged source hashes at22:25:26UTC. Whole BCM Medical remains in development. Current work and exact acceptance boundaries are tracked in BACKGROUND-WORK-QUEUE.md and ../VALIDATION.md.

A subsequent native stock-item commit fix passes649 local,643 dedicated owner/network and668 clinical-safety checks. Passenger/drip world restoration passes647/641. The expanded36-gate combined run and the smaller current-source package remain pending; the earlier33-gate pass/package are separate snapshots. See NATIVE-STOCK-CONSUMABLES.md.



All production development belongs to this one standalone addon, with only the base-game dependency. This ledger does not claim the entire medical ecosystem is complete. The original BCM and third-party reference projects remain separate.



## Connected and accepted paths



- Original patient/trauma/physiology and native damage integration: blood reserve/grace, fluid rescue, consciousness, native external single-writer DOT, dressing wear/reopening, internal compartments, fracture movement and replicated oxygen/pulse/arrest/movement.

- H with native personal rebind/reset and tap/hold filter options; self, conscious/unconscious looked-at patient and same-vehicle viewing. Compact supplied-logo/anatomy interface with seven native click handlers, blank initial status, timed owner-private region checks, per-view/region findings and server supply counts.

- Native field-dressing equip and selected-region self/other item animation/consumption. Original 30-second default splinting and 60-second default extraction, suturing, internal repair and facility fracture surgery. Timers are GM-configurable; cancellation before consumption retains supplies. Region requests cannot silently select another region's injury.

- 270 generated native GM controls under one BCM Medical tab with fifteen inline headings, schema 33 with validation/migration of schemas 1–32, replicated values and two-generation profile writes/recovery. Separate-process restart reloads treatment times and medication kinetics.

- Actual corpse -> named searchable bag -> native vehicle cargo equipment flow, rollback and concurrent dropped-weapon pickup protection. Unloading leaves equipment in the vehicle. Four vehicle classes, ordinary-capability discovery and native UI reconciliation.

- Native manual/rope body-bag physics, rope inventory kit, owner RPCs and replicated safety release. No body-bag carrying mode.

- Original life ledger/native spawn-death adapter and BCM-owned focus/suppression integration, with narrower acceptance boundaries described in their documentation.



## Remaining for the full standalone replacement



1. Broader drug formulary, dose/administration-route specificity, IO routes, broader transfusion complications and further monitor/ventilation workflows. Independent finite patient-owned drips passed the previous30-gate combined snapshot. Opt-in redistributed-fluid burden now has focused native flow/save/network and historical-world acceptance; the current33-gate combined regression passed unchanged. Stabilised red-cell/clotting recovery is now available as an opt-in policy with accepted native runtime/restart evidence. TXA injection and handheld continuous pulse oximetry now operate. Current implemented items include morphine, naloxone, epinephrine, TXA, paracetamol, fentanyl, portable oxygen, monitor, AED/pads, BVM, airway adjunct, suction, seal, decompression, drain, IV access and finite typed fluid bags with progressive native delivery and partial-content retention.

2. Complete injury generation and organ/brain/blast/burn/armour calibration, more complete fractures/complications, facilities/qualification policy and medical AI.

3. Broader controller/accessibility and small-screen play testing. The redesigned button layout has actual native screenshot/mouse review and 720p/1080p smoke checks. Pain and Medication now have private checks and original finite-supply effects; broader drug formulary and instrument telemetry remain. Manual findings remain examination snapshots; active handheld pulse oximetry shares continuously refreshed heart rate and oxygen saturation with other viewers.

4. Final placement/material polish for applied devices, surgical/suture/splint hand contact and props, and remaining casualty drag animations. Two-player stretcher carrying is shelved by user request. Original applied splint and tourniquet meshes, separate donor AED pads and applied cannula are imported; eight active device appearances have local runtime checks. Remaining grip and casualty carry animations (original CPR and field-procedure prototypes and native injections now operate); visible tow rope and conscious carry consent/native transport.

5. Live backend-authenticated Limited Lives/reconnect and offline-account administration. Connected targeted native GM grant/remove/reset controls are implemented and locally accepted, including actual owner RPC, Edit-mode revocation and a separate saved-world restart; see LIMITED-LIVES-ADMINISTRATION.md. Native per-account allowance/world persistence is implemented and locally accepted with diagnostic identities; real authentication/reconnect remains open. Body-bag respawn restrictions, spectator and admin menus are removed by user instruction.

6. Broader vehicle destruction/deletion and recovery, mass contention/disconnect/lifecycle testing and additional arbitrary vehicle packs. Physical burial and Send Body Home are implemented and have native acceptance; they do not affect Limited Lives.

7. Remaining loadout distribution and broader world medical catalogues beyond the accepted stationary stretcher, broader purchase/refund scenarios beyond the accepted native saline path, full asset/licence inventory review, performance/load tests and release packaging. US/USSR/FIA native inventory catalogue overrides now contain 28 medical entries each with live availability/cache filtering and server purchase-policy enforcement. Git publishing remains deferred.



## Current evidence



- .checks/splint-network-20261026-172056-705: 198 server assertions, zero failures. Actual client ownership, real 30-second player splint application, owner cancellation with original-kit retention, all seven anatomy handlers, blank-before-check state, private/stale-safe clinical response, native H -> J / hold / reset, and actual native bandage animation/consumption on the selected leg. Three clients including JIP observed all three phases. Fast mode skips only the separately tested initial AI splint fixture, not the player treatment time.

- .checks/20261026-165021-512: 204 assertions, zero failures; native animated other/self bandaging, delayed consumption, selected leg treated and opposite leg still bleeding.

- .checks/20261026-155431-866: 205 assertions, zero failures; full 60-second extraction, 60-second suture and 60-second facility fracture repair with actual kits. Extraction leaves closure separate.

- .checks/20261026-162923-807: 200 assertions, zero failures; losing facility cancels internal surgery without consuming kit; full-duration repair stops the source without refilling blood or erasing pleural collection.

- .checks/20261026-155758-547: 200 assertions, zero failures; real native seating, self/shared-vehicle examination and outsider rejection.

- .checks/medical-network-20261026-172654-521: 227 assertions, zero failures; CPR ownership, private examination, arrest/recovery and three-client/JIP state observations. This fixture tests authoritative RPC behavior rather than native client item input.

- .checks/persistence-clinical-20261026-172459: separate seed/reload processes passed; reload has 200 assertions, zero failures. Surgery, suture, splint and bandage values are used by the actual game mode after restart.



Older body-bag and physics evidence is recorded in VALIDATION.md and BODY-BAG-CARGO.md. Native screenshot and mouse review subsequently succeeded on the redesigned interface; that supersedes the earlier approval timeout. Current button template, compact layout, pagination and anatomy fit are exercised in the actual game.

- Final drag regression .checks/drag-network-20261026-172837-275: 211 server assertions/zero failures, four phases on all three clients including JIP, observed position error below 0.09 m.



## Latest accepted additions



- Original pain and finite-dose medication physiology, native self/other injection, cancellation and private medication history. Latest native medication fixture: .checks/20261026-193423-664, 315 assertions, zero failures, including independent preparation and completion-grace controls.

- Advanced-equipment native default-time fixture: .checks/20261026-184353-520, 330 assertions, zero failures across twelve procedures. Full three-client medication/equipment sequence: .checks/splint-network-20261026-192659-204, 300 server assertions, zero failures and seven phases observed on each client including JIP. The preparation/grace regression subsequently passed in .checks/splint-network-20261026-194953-596: 302 server assertions, zero failures, all seven phases on three clients/JIP. A progress-bar layout diagnostic found in that run was separately corrected and retested.

- Actual penetrating damage generates configurable side-correct chest and internal injuries: .checks/20261026-185427-386, 319 assertions, zero failures.

- Professional native button template, separate header/tabs/columns, selected/disabled states, treatment pagination, tab-filtered findings, progress/cancellation and screen-dependent anatomy sizing. Native UI smoke: .checks/20261026-193850-097 (720p) and .checks/20261026-193858-998 (1080p), each 9 checks, zero failures. Native mouse selection and examination were visually reviewed.

- Original CPR motion exported and compiled using the attributed Bohemia sample rig. Native owner, observer and late-join playback/release now pass. Hand contact and pose refinement remain.

Post-fix UI smoke runs .checks/20261026-195404-845 (720p) and .checks/20261026-195413-628 (1080p) each passed nine assertions, with no ProcedureFill sizing diagnostic. The diagnostic environment still emits the unrelated native SCR_WidgetExportRuleRoot warning. Separate dedicated-server seed/reload .checks/persistence-current-20261026-195358-551 passed 295/303 assertions, zero failures, including current schema migration and new saved controls.

Latest UI/patient work: Fluids label, brighter gold controls with dark labels, anatomy hover/selected feedback, examination-based treatment flags and no menu-close on bandaging. Native UI acceptance now has 15 checks at each of 720p/1080p with real bandage supplies; nearby/same-vehicle discovery has a 305-assertion server fixture. The patient list refreshes once a second with stable existing rows, real accessible characters and six-entry pagination. Five-phase native owner medication/bandaging replication passed 302 assertions on three clients/JIP. CPR entry/loop/exit source blockouts have verified skeleton continuity; the enabled Workbench API has now exported all three clips and native runtime integration is accepted.



Latest CPR acceptance: `.checks/animation-network-20261026-225304-573` passed 298 assertions with owner/observer/JIP skeletal playback, release and second-session out-of-reach cancellation. Native animation regression `.checks/20261026-225628-125` passed 303 assertions; dedicated medical regression `.checks/20261026-225639-662` passed 324. See ANIMATION-SOURCE-BRIEF.md for prototype limitations and source provenance.


Current production CPR regressions: `.checks/20261026-230547-541` passed 305 native assertions; `.checks/animation-network-20261026-230559-111` passed 298 server assertions with three skeletal/release observers. Medical privacy/ownership regression `.checks/medical-network-20261026-225811-146` passed 326 assertions. The user subsequently confirmed live movement cancellation and approved the revised CPR hand/knee pose. Final instrument/contact art and remaining carry/drag clips remain on the ledger.


### Stretcher and equipment update

Two connected-player stretcher reservations, native character locomotion, server collision sweeps, replicated patient anchoring, live GM stop/re-enable and carrier-disconnect recovery now have dedicated-server acceptance. Stationary placement/removal and equipment preservation remain supported. Player-owned casualty anchoring, set-down and adjacent unloading now pass on the actual casualty owner as well as both carrier clients. Final carrier grip animations and remaining equipment presentation are separate unfinished items. Model import checks cover 27 entries; staged drainage donor parts and the high-detail blood bag are not final runtime replacements.

Final resumed acceptance: `.checks/stretcher-network-20261027-172150-059` passed all 372 assertions and the complete three-client gate, including nondefault direct/bulk GM settings and native player-casualty unloading. `.checks/persistence-current-20261027-172352-292` passed separate-process settings reload with 357 assertions; `.checks/20261027-172358-325` passed 26 native UI smoke checks. Final equipment and field-procedure fixtures passed 399 and 365 assertions respectively. The stretcher transport mechanics are accepted for these local tests; carrier grip/contact art, Internet/load testing and the broader roadmap above remain unfinished.

## Clinical integration pass, 27 October

The original physiology now connects compensatory pulse, pain/stimulants, severe decompensation, rate-dependent pumping, pressure, ventilation and arrest/recovery conditions. Medication and fluid safety thresholds, TXA clot support and passive blood regeneration policy are GM-configurable. The default prevents passive native blood refilling. The native finite-item safety fixture and complete domain scenarios are tracked in VALIDATION.md.

The applied-device presentation uses replicated clinical flags and per-client mesh-only cosmetics, with no usable item or physical collision duplication. Exact IV limb, local range culling, live GM filtering and healing cleanup are covered independently of transport. Additional visual/LOD and varied-clothing calibration remain necessary.

The clinical research and current training sequence are in CLINICAL-INTEGRATION-AUDIT.md and TRAINING-MANUAL-DRAFT.md. The latter is a development draft and will need a final release preset and scenario evidence before publication as community training.

## Typed-fluid integration, 27 October

Saline, plasma and whole blood now have distinct finite inventory kinds, volume/red-cell/clotting accounting, water redistribution and linked tissue oxygen delivery. Domain checks include severe anaemia with normal saturation and cause-corrected recovery. Full native 60-second procedures passed all three bags with exact consumption and component accounting. The dedicated native owner/proxy/late-join gate passed all five phases, including actual Fluids-tab item use, replicated vital signs and nondefault GM fluid controls. No continuous drip, overload syndrome or blood-type matching is claimed.

## Oxygen and expanded medication update — 28 October 2026

Finite retained oxygen, active replicated mask, six-drug supply indexing, oral swallowing guards, stronger-opioid role/route safeguards, additive opioid risk and naloxone reversal are implemented. See [implementation and limits](OXYGEN-AND-EXPANDED-MEDICATIONS.md) and the current evidence in `../VALIDATION.md`. Settings now total 250 in fifteen inline sections, including Arsenal. The face mask follows native facial landmarks with saved alignment controls. Standing tablet self/handover motions are compiled and replicated, with the right palm corrected inward toward the mouth. Remaining oxygen/tablet art includes the separate cylinder, final mask materials/contact and visible pill packaging. Full formulary, continuous infusions, organ calibration and world/account persistence remain open. Two-player carrying remains shelved.

## Active body-bag integration

Body bags are integrated only in BCM Medical. The separate body-bag respawn gate, requests, countdown, spectator and Body Bag Admin menu were removed on 29 October at the user's instruction. Limited Lives is the sole BCM respawn policy. Bagging, cargo, search, dragging, burial and Send Body Home remain. Current settings: 254 saved controls, schema 30; old respawn keys are discarded during validated migration.

The removal compiles and its native Limited Lives/model/migration run passes 573 assertions, zero failures. Current-build persistence/recovery/clinical checks are in progress. Earlier eight-vehicle/five-aircraft and clinical evidence remains historical until the relevant current build is checked. See BODY-BAG-INTEGRATION.md for exact evidence and retirement steps. Continue the full Medical requirements above; carrying remains shelved and Git publishing deferred.

Progressive finite held-fluid delivery is implemented and accepted in the native full-duration, interruption/resume, safety, connected replication and UI gates listed below. Independent line/drip workflows remain open. Legacy add-on files are removed; Windows retains only an empty locked directory. Its source/assets and complete Git history are preserved under Medical's .checks directory.

Progressive held infusions now pass 606 full/cancel/resume native checks, 575 access-loss/clinical-safety checks, 569 connected checks with owner partial-content replication and intermediate physiology on owner/proxy/JIP, and eight native UI/live-capacity checks. The 720p partial-bag display was inspected. Independent line/drip workflows, overload, broader route/formulary and world/account persistence remain on the full Medical ledger. Next work continues native arsenal purchase/refund acceptance, including depleted finite supplies.

Finite arsenal refunds now account for remaining fluid, oxygen and AED capacity, including the separate native personal-allocation path. Native calculator acceptance passed 598/0; active-treatment/enclosing-storage reservation acceptance passed 579/0. Actual native financial transaction acceptance is underway; owning-client transaction delivery remains open.

Combined ten-gate clinical acceptance .checks/clinical-acceptance-20261029-082439-133/report.json passes with sourceUnchanged=true, including dedicated native arsenal transactions. Active-source and enclosing-container refund RPC guards pass581/0. Native finite-item persistence is the next active implementation; full Medical remains in development.

Finite Medical inventory state now has native world-save restart acceptance: .checks/finite-persistence-20261029-084753-382/result.json Passed=true, SourceUnchanged=true,595seed/559verify, both0failures. Original saline/oxygen/AED persistent UUIDs, capacities and depleted contents survive. Patient/wound/medication, body-bag/cargo world state and Limited Lives account persistence remain separate work. That recording hold has ended; native testing resumed on 29 October.

## Current native persistence work — 29 October, recording finished

The recording hold has ended. `.checks/finite-persistence-20261029-141545-028/result.json` passes611 seed/588 real separate-process world-restart assertions, zero failures, unchanged sources. Implemented and locally accepted: original finite supply capacities/contents, diagnostic-account Limited Lives balances/body identity and missing-record recovery, native circulating blood plus clinical wounds/native links/dressing wear/internal bleeding, fracture and applied equipment state, medication doses/cooldowns, remaining depletion grace, and connected oxygen source UUID reattachment without refill. Native DEFAULT healthy health and delayed oxygen binding lifetime failures were reproduced and fixed. Nested invalid wound, fracture and internal-bleed payload rejection passes.

Live backend-authenticated reconnect and peer/JIP persistence remain open; injected identities do not prove authentication. Body-bag, grave, stationary stretcher and vehicle cargo world persistence subsequently passed the focused native checks below. The eighteen-gate clinical regression is running; the intermittent full-sequence oxygen reconnection reach failure remains open until diagnosed/closed. Continue the full remaining requirements; these focused persistence passes do not mean the mod is complete.

Current production world persistence: four native vehicle families pass623/601 binary saved-world checks, including automatic custom-prefab registration. Jeep153104-289 passes a third process623/612/563 after actual unload/reload/destruction/deletion; destroyed recovery cargo stays disposed and never changes the surviving paid-life balance. Native storage nested type scope is fixed; no diagnostic backend override ships. All eight final connected vehicle cases now pass individually: aircraft matrix153431-946 plus corrected Mi-8 rerun155033-495 and ground matrix155244-615. The original aircraft report retains the initial Mi-8 failure; HH-60G retains its explicit upstream diagnostic policy. Fresh full18 clinical regression155841-589 is running with its first12 gates passed. Owner/proxy/JIP saved-world and live-authenticated reconnect remain separate acceptance work.


Latest persistence/death corrections pass actual saved-world connected623/558, unavailable-reference timeout/save/third-process623/572/558 and Jeep post-wreck restart623/612/563, all unchanged and zero failures. Native death re-entry was reproduced/fixed; unavailable physical UUIDs no longer poison later saves or recreate gear. Expanded20 regression163909 failed oxygen phase8; trace2.04861m proved valid casualty range loss beyond2m. Native negative range test now passes587/0 and corrected connected success-case staging passes571/0. Production reach remains unchanged. Expanded20 rerun165632-741 is active; separate earlier full oral-readiness failure is still unproven.

A standalone raw-backend audit/reconnect diagnostic compiles at165047/165407 but has not passed runtime. Config-mode launches lose the local addon context and cannot read the diagnostic mission; direct-world launch bypasses backend/bind settings and is rejected as proof. Regular1.8.0.13 server/client binaries are available for a later correctly configured attempt. No test addon was published and no authenticated-life acceptance is claimed.


### Current clinical regression accepted, 29 October 17:58 UTC

Combined21-gate report `.checks/clinical-acceptance-20261029-172748-736/report.json` is passed with unchanged source hashes. Native GM allowance administration, saved-world connected restoration, missing-reference timeout/save/restart and all full-duration procedures are included. Development continues with stationary treatment-stretcher native GM catalogue distribution. Live backend reconnect and the remaining full replacement ledger above remain open.

### Stationary stretcher GM distribution accepted, 29 October18:35 UTC

The native GM object browser now contains one localized **BCM stationary treatment stretcher** with the supplied model thumbnail, inherited treatment/facility actions and explicit native persistence for the editable variant. All41 native placement registries remain configured. Actual owner/native placement RPC, owner/proxy/JIP entity observation and independent saved-world restart pass in gm-catalogue-network183154-225 (589/565, unchanged/zero failures). Saved casualty/stretcher connected restoration also passes in world-persistence-network183426-508. Native authoring helpers remain excluded. Folded inventory deployment, broader distribution/loadouts, controller/accessibility and clean-package review remain open; carrying stays shelved.

Clinical acceptance now includes a dedicated native GM catalogue/late-join/restart gate, bringing the suite to16 core gates or22 with full-duration procedures. Production/test source guards include PrefabsEditable, and owned-resource auditing rejects stale GUID references. The previous21-gate pass remains the accepted earlier clinical snapshot; it is not relabeled as a22-gate result.

### Stabilised component recovery accepted, 29 October19:12 UTC

Gradual red-cell/clotting recovery is implemented as an opt-in policy with separate timescales, minimum volume/pressure and stationary-facility requirements. Active bleeding, arrest, inadequate oxygenation and tension prevent it. It never replenishes missing native blood volume or removes the need for fluid/surgical care. Six new controls bring the existing single tab to260settings/schema31; valid historical profiles retain values, and retired body-bag controls stay removed. Native character/facility/policy and separate-process saved-state acceptance191124-440 passes615/600 unchanged. Independent lines/drips, IO/routes, overload, broader formulary/monitoring and the other completion ledger remain open.

The prior catalogue16-gate build184611-984 passed unchanged at18:59:50. Clinical Acceptance now has17 core/23 full gates; the newly expanded combined run is not yet accepted.

29 October 2026, oxygen removal: a connected supply can now be disconnected without the removing medic carrying a spare cylinder. The native medical menu enables the existing removal action from the replicated connection state. Removal retains its saved duration, authority validation, reach and actor/patient locks; cancellation leaves the supply connected. The operation captures the exact oxygen session so reconnecting the same cylinder invalidates an old pending removal. No replacement device is created. Native actual menu/RPC, timer, movement cancellation, same-device reconnection and independent concurrent removals passed608 checks with zero failures in .checks/20261029-194450-572. This focused local runtime does not by itself certify dedicated owner/proxy/JIP removal.
29 October2026,20:18: the inventory-free oxygen-removal build passed all24 gates unchanged in clinical-acceptance-20261029-194537-299. Dedicated zero-cylinder removal acceptance is next; independent drips remain staged and unaccepted.

Independent patient-owned drips are implemented in current development263/schema32, with native finite flow630/0, active-world save/restart614/613 and owner/proxy/active-session JIP612/0 accepted in their own unchanged snapshots. See INDEPENDENT-FLUID-LINES.md. Expanded pressure/full-reserve/depletion and missing-source boundaries, followed by combined regression, remain in progress. Existing held infusions remain available.

Current optional fluid-burden development270/schema33 is disabled by default. Base634/0 and real finite-source/breathing/oxygen/active-line world save/restart651/642 pass unchanged212421-106. See FLUID-BURDEN.md. Multiplayer live-policy/UI validation and combined regression remain in progress. The263/schema32 independent-drip build separately passed all30 gates unchanged203856-546.




