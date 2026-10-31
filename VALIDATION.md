# BCM Medical validation



26 October 2026, Arma Reforger 1.8.0.13. These results cover the current independent foundation and GM settings adapter, not the completed medical ecosystem.



## Passed



Workbench script validation succeeded after native resource registration. The dedicated diagnostic server passes 126 normal assertions, including loading the actual native GM attribute resource and checking that every one of the 27 settings has a control.



Three separate dedicated-server processes passed 125 checks each with a shared profile:



| Phase | Result | Evidence |

| --- | --- | --- |

| Seed two settings generations | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-23/console.log |

| Restart and load latest into actual game mode | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-26/console.log |

| Damage newest file and recover prior generation | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-29/console.log |



Run tokens differ across processes; stale success/failure logs cannot satisfy a new run. The expected invalid-settings diagnostic in the suite comes from the deliberately invalid-schema fixture. The tests preserve/lock that fixture rather than overwrite it.



Coverage includes patient/wound/chest isolation, AED shock advice, exclusive CPR sessions, depletion rescue/expiry, original life accounting, retained-projectile boundary, dressing reopening and frame-step independence, extraction/suture sequencing, facility-only critical surgery and fracture repair, splinted motion blood loss, pleural drainage versus source repair, consent replay/expiry, rope/manual transport policy, authority rejection without an editor, settings range/step/cross-field validation and JSON persistence/recovery.



Loaded runtime projects: native core, ArmaReforger, the user's BCM Medical GUID 6AB4B3F3FD215919, and the separate diagnostic addon. No ACE/NIRE/KAT/legacy BCM/other Workshop addon was loaded.



## Remaining acceptance



Visual GM menu/controller interaction and save popups have not been tested by a human client. Multiplayer owner/JIP/settings concurrency needs dedicated clients. The resource-load test establishes catalogue presence, not UI usability.



Native injury/consciousness and physiology adapters, treatment durations/inventory/RPC validation, backend life identity, suppression/focus integration, spectator routing, casualty transport physics/animations, body-bag transactions and the original BCM medical interface remain unimplemented or unconnected. No gameplay replacement, Workshop release or live-server deployment is claimed.



The final 126-assertion run also loaded the filled body-bag collision model, empty model and bag material directly from the consolidated addon. Evidence: .checks/20261026-113316-102/logs/logs_2026-10-26_11-33-16/console.log.



## Native body-bag cargo acceptance



The final dedicated server plus three diagnostic clients passed the cargo fixture in `.checks/cargo-network-20261026-122028-620`: 151 server assertions, zero failures, and all four expected cargo phases on each client (12 successful client phase observations). One client joined while the first bag was already stored. The phases cover the named stored manifest and exact unloaded bag/native inventory state for the ambulance and helicopter.



The latest fixture includes direct native-inventory removal from the helicopter and server reconciliation afterward. It also verifies schema-1 settings migration into the expanded 29-control catalogue. No ACE or other Workshop dependency is loaded.



The native ambulance inherits a support-station area-mesh offset diagnostic in client logs; this is separate from the cargo checks and has not been fixed by modifying base resources. The deliberately invalid settings fixture also emits its expected error diagnostic.



This acceptance does not cover a human-operated look-at/search menu, actual player corpse inventory transfer, empty-bag use, sling physics, vehicle destruction, bag deletion while stored, arbitrary vehicle packs, or inventory contention between multiple medics. See Docs/BODY-BAG-CARGO.md.



A final single-server run passed 153 assertions, including actual bag-centred discovery of the compatible ambulance and helicopter: .checks/20261026-122515-307/logs/logs_2026-10-26_12-25-15/console.log. Workbench script validation also passed after the cargo changes.



## Superseding acceptance update — 26 October 15:02



Older counts and unfinished-feature lists above are historical. Current catalogue is 50 controls/schema 7. See Docs/ROADMAP.md and Docs/BODY-BAG-CARGO.md for current boundaries. Armed bagging/cargo: 331 checks; full rollback: 232; concurrent pickup: 234; native medical regression: 207. All passed with zero failures. Tools/Validate.ps1 -ServerRuntime -PickupRuntime reproduces the pickup-after-snapshot scenario. The deliberate invalid-settings error is expected fixture coverage, not a failed check.



## Splint and medical-panel acceptance — 26 October 15:38



- .checks/splint-network-20261026-153723-377: 201 server assertions, zero failures; actual owner splint RPC, exact kit consumption, unsupported -> supported native movement, jump restriction retained, three clients including late join. All three instantiate/close the native medical panel; the treating owner's panel receives real server body-region assessment text.

- .checks/medical-network-20261026-153331-076: 219 server assertions, zero failures; private examination delivery, competing CPR ownership/release and physiological recovery on all three clients. The fixture waits for a newly spawned medic to settle before requesting CPR; production movement restrictions remain unchanged.

- .checks/20261026-153454-397: 198 server assertions, zero failures; interrupted treatment retains kit, reservations release, selected-leg application consumes kit, original native menu presets remain present, schema-7 migration and new GM controls validate.

- .checks/persistence-splints-20261026-153523: two separate server processes successfully seed/reload settings. Verification has 190 assertions, zero failures. The saved splint duration (9 seconds) and movement tolerance (0.7 metres) reload in the actual game mode.



Current schema is 8 with 52 GM controls. The panel is a functional prototype: layout instantiation and live assessment delivery are accepted, but screenshot-based layout review, human button/controller interaction, self-access and final graphics remain outstanding. The splint references a temporary native field-dressing mesh; final splint art and application animation are not accepted. Advanced surgery/item treatment and the full standalone replacement remain unfinished.

## Superseding clinical/anatomy acceptance — 26 October 17:25



Current state and limits are in README.md, Docs/CLINICAL-INTERFACE.md and Docs/ROADMAP.md. Current catalogue is 56 settings/schema 10, with twelve valid GM category resources.



- .checks/splint-network-20261026-172056-705: 198 server checks/zero failures, three clients/JIP, actual client-owned character and actual native bandage animation/consumption. It verifies all seven anatomy handlers, initial/selection-only blank status, owner-private checks and stale-view rejection, native H/J/hold/reset binding API, 30-second splinting and owner cancellation with authority-confirmed kit retention. Fast mode skips only the redundant AI prelude. The later compact sizing/font/wrapping changes are resource-validated separately; they are not screenshot-accepted.

- .checks/20261026-165021-512: 204 checks/zero failures; actual native other/self bandage animation, no instant effect, exact original consumption, selected-leg isolation.

- .checks/20261026-155431-866: 205 checks/zero failures; three full 60-second surgical/closure/fracture procedures.

- .checks/20261026-162923-807: 200 checks/zero failures; facility-loss cancellation/internal repair without restoring lost circulating blood or deleting the collection.

- .checks/medical-network-20261026-172654-521: 227 server checks/zero failures, three clients/JIP; prior CPR/physiology/private-examination paths still pass.

- .checks/persistence-clinical-20261026-172459: separate processes verify saved surgery=75, suture=90, bandage=12 and splint=9; reload has 200 checks/zero failures. New defaults remain 60/60/10/30 for fresh profiles. Existing custom values are intentionally retained.

- .checks/20261026-172505-002: Workbench script/resource indexing validation successful after compact layout/font changes.



Diagnostic fixes matter: assigning a controlled pointer did not transfer native character ownership. Only the native-item fixture now grants actual client ownership and spawns that client naturally within reach. Server transform writes against a predicted owner are not used to make this scenario pass. The original authority/RPC-only medical and drag fixtures retain their separate scope. No production ownership transfer is added by the medical mod.



Physical mouse/keyboard and current-layout screenshot acceptance is still outstanding after a desktop app-access approval timeout. No live visual result is claimed. The native IntroSplashScreen diagnostic can report SCR_WidgetExportRuleRoot before the Game module loads; it is outside the BCM panel. The deliberately invalid-settings fixture emits its expected preservation/lock error.

Final body-bag regression: .checks/drag-network-20261026-172837-275 — 211 server assertions, zero failures; all four rope/manual owner-RPC/release phases observed by all three clients including JIP. Actual position error stayed below 0.09 metres in this run. No gameplay body-bag behavior was changed by the clinical interface work.



## 2026-10-26 pain, medication and native procedure animation development



- .checks/20261026-181727-170: dedicated server, 256 assertions / zero failures. Original pain and finite drug kinetics, repeat/stack limits, antagonist wear-off, no automatic ROSC, native injection into another patient, real native aiming penalty and reduction, mid-animation cancellation with exact-item retention, retry, completion consumption and native FullHeal reset. Native case keeps the real default 10-second medication time.

- .checks/20261026-180849-806: dedicated server, 246 assertions / zero failures. Actual native treatment-loop splint animation, movement interruption, kit preservation, reservation release and full default 30-second application; fracture and jump restriction remain.

- .checks/20261026-182215-876: actual local game UI smoke, 5 assertions / zero failures. Panel instantiates; both Pain and Medication tabs are enabled; opening and anatomy selection reveal no findings. This checks native widget state, not a physical screenshot or mouse-input review.

- .checks/20261026-181528-954: Workbench script/resource validation passed after native injection inheritance and pain handling changes. Subsequent native runtime runs compile newer scripts as well.



Development failures were resolved: native item inheritance initially duplicated a rigid body; a legacy line still disabled Medication after the new handler was bound. The dedicated and local UI tests caught those faults. Multiplayer acceptance for this change and post-animation surgery/persistence regression are tracked separately below once complete. Earlier surgery evidence predates the new animation gate.



## 2026-10-26 subsequent accepted checks



- .checks/20261026-182720-695: 251 assertions / zero failures; real 60-second extraction, suturing and facility fracture repair with native treatment-loop placeholders; midpoint animation assertion.

- .checks/20261026-183126-956: 260 assertions / zero failures; actual other/self native injection, cancellation, finite consumption and native pulse/aim effects.

- .checks/persistence-medication-20261026-183250: separate seed/reload processes; reload 244 assertions / zero failures, including actual game-mode medication onset and pain handling values.

- .checks/splint-network-20261026-182246-345: 244 server assertions / zero failures; all five phases on three clients/JIP, private pain/medication checks and actual owning-client medication animation.

- .checks/20261026-184353-520: 330 assertions / zero failures; all twelve default-time advanced equipment actions.

- .checks/20261026-185427-386: 319 assertions / zero failures; corrected immediate-effect filtering and real native penetrating impact.

- .checks/splint-network-20261026-190352-484: focused equipment fixture, 293 server assertions / zero failures, actual owner chest seal and reusable monitor with private readings, all three clients/JIP.

- .checks/20261026-192607-245: native redesigned UI, nine assertions / zero failures, labelled non-overlapping tabs, aspect-preserved anatomy, blank-before-check and functional paginated treatment bindings. Live Windows review separately verified the actual leg selection and examination mouse clicks.



A full-sequence equipment interaction failure remains under investigation; focused equipment success alone is not reported as complete acceptance of every sequence. UI development caught and fixed unsupported frame Size syntax and a button-content sizing issue before the accepted native smoke run.



## 2026-10-26 preparation, persistence and UI follow-up



The full sequence subsequently passed in .checks/splint-network-20261026-192659-204 (300 assertions, zero failures). After preparation/completion-grace changes, .checks/splint-network-20261026-194953-596 passed 302 server assertions and all seven functional phases on all three clients including JIP, with private medication and instrument results. That run also exposed a repeated native GUI diagnostic in ProcedureFill: the vertical anchors differed while code used FrameSlot.SetSize. Functional assertions alone did not detect this presentation defect. The generator now uses identical point anchors; both native UI and network harnesses reject this specific diagnostic, even if a completion marker exists.



Workbench .checks/20261026-195204-385 compiled the expanded migration/restart checks successfully. Separate dedicated server seed/reload in .checks/persistence-current-20261026-195358-551 passed 295/303 assertions respectively, zero failures. This includes schema 14-to-15 migration, incomplete prior-document rejection, and saved preparation timeout, completion grace, fluid timing and injury probability alongside prior medication/surgery choices. Deliberate invalid-document tests still emit their expected settings-invalid diagnostic.



Post-fix UI runs .checks/20261026-195404-845 (1280x720) and .checks/20261026-195413-628 (1920x1080) each passed nine assertions. Inspection confirms the ProcedureFill sizing diagnostic is absent. Both have the unrelated native diagnostic-world Unknown class SCR_WidgetExportRuleRoot message, so this is not a claim of an entirely warning-free engine log.



## Fluids, anatomy feedback and examined treatment availability



The visible Blood category is now Fluids; its internal widget binding remains stable. Original owner-only bleeding replies carry applicable wound-treatment flags after examination, separately from inventory counts. The client requires both actual supplies and the examined condition. Server procedure checks remain authoritative. An unnecessary bandage request leaves the panel open; the native bandaging path no longer closes it. Native item use also disables overlapping treatment buttons.



Accepted checks:

- .checks/20261026-200627-274: 305 assertions, zero failures, accessible nearby conscious-character discovery without aim, healthy-arm treatment flags, fracture applicability, real same-vehicle viewing and distant outsider rejection. The first assessment run exposed a missing fracture setup in the new test; its fixture was corrected before acceptance.

- .checks/splint-network-20261026-200744-115: 302 server assertions, zero failures, all five medication/bandage phases on three clients including JIP. Native selected-region bandaging succeeds with the menu remaining open; private clinical replies still use patient/view guards. No ProcedureFill resize errors.

- .checks/20261026-211929-092 (720p) and .checks/20261026-211937-956 (1080p): 15 assertions each, zero failures. Real field dressings exist (3 available), yet the healthy examined limb's Bandage button stays disabled and an attempted unnecessary bandage leaves the panel open. Gold buttons, Fluids label and distinct hovered/selected anatomy states are checked. A prior UI assertion incorrectly assumed exactly one dressing; native character loadout already provided additional dressings, so the test now requires a positive actual count instead of an invented count.

- .checks/20261026-212146-887: 15 assertions, zero failures, following stable patient-row refresh and unavailable-patient guards.



Native screenshot/mouse review confirmed the gold controls and arm selection. Source CPR validation in .checks/animation-export/CPR-source-validation.json confirms finite native skeleton transforms, zero loop seam error and entry/exit joins under 0.000002. This is source-only acceptance. Workbench net API was not listening on 5775, and Windows computer-use approval for Workbench timed out. An isolated profile configuration attempt did not enable it; that owned instance was stopped. The user was asked to enable the documented Workbench net API setting. No TXA/ANM export or custom runtime animation is claimed.





## Exported CPR and native replication acceptance — 26 October 2026



Supersedes the earlier source-only/API-timeout status. User enabled Workbench API 5775; official EBT export and native Workbench compilation produced original BCM CPR enter/loop/exit ANMs. Only the native slot schema is referenced from the base game; assigned motions and orchestration are original BCM. The test addon remains unpublished.



- `.checks/20261026-225628-125`: **303 checks, zero failures**, including native bone displacement, stable repeating kneeling compression motion, authored release motion and final loiter exit.

- `.checks/animation-network-20261026-225304-573`: **298 server checks, zero failures**; actual owner and proxy observe skeletal motion, third client joins during compressions and observes the loop, all observe release. A second session is canceled by actual owner displacement out of reach. This does not emulate a physical movement key.

- `.checks/20261026-225639-662`: **324 checks, zero failures**, dedicated native medical regression after the integration.



CPR patient reservation now precedes animation startup; server compression effects wait for native playback. Entry uses the existing saved preparation timeout. Exact-command exit cleanup is bounded and replicated; a delayed callback cannot end a later unrelated command. Expected deliberately-invalid-settings protection diagnostics remain part of the fixtures. Prototype fingers/contact/ground alignment and other bespoke clips remain unfinished.



Medical owner/privacy regression `.checks/medical-network-20261026-225811-146`: **326 server checks, zero failures**, actual client character ownership, CPR preparation/playback, second-medic/stale-token rejection, exact-session release, private examination delivered to one requesting owner, arrest/recovery replicated to three clients including JIP. The old server teleport was replaced by exact-session revocation; real owning-client out-of-reach movement is covered separately by the animation network fixture.



Post-identity-guard regression: `.checks/animation-network-20261026-225938-808` and post-local-cancel regression `.checks/animation-network-20261026-230146-101` each passed **298 checks, zero failures**, all three skeleton/release observers including JIP. `.checks/20261026-230035-683` passed **305 checks, zero failures**, including rejecting a different native animation context. Workbench script validation `.checks/20261026-230047-460` passed, including the animation build plugin GUI Run entry point.





Movement-key interruption remains unaccepted. Optional one-client `-KeyboardCancellation` fixtures `.checks/animation-network-20261026-230249-652` and `230421-407` did not observe cancellation from the automated W tap. The tool provides a tap rather than a held movement input, and the synthetic client has no normal character camera; the cause is not established. Forcing the native movement context did not resolve the result and was removed from production. The normal three-client displacement/lease-release fixture remains accepted. Owner heartbeat now samples native rebound movement actions each frame and suppresses repeated cancel RPCs; this addition has compilation/network regression coverage but does not prove physical-key cancellation. The optional keyboard fixture remains available for direct in-game acceptance.


Current final regression after removing the forced movement context: `.checks/20261026-230547-541` **305 assertions, zero failures**; `.checks/animation-network-20261026-230559-111` **298 assertions, zero failures**, owner/observer/JIP loop and release, second-session displacement cancellation. These runs compile the current production game scripts. Keyboard acceptance remains open as recorded above.


## 27 October 2026 — current acceptance (supersedes earlier open CPR/UI notes)

- Native movement-key CPR cancellation: `.checks/animation-network-20261027-061741-590`, 298 server assertions, zero failures, actual character-camera/native movement stream, lease released while still in reach. User separately confirmed movement stops CPR.
- Revised original CPR pose: `.checks/20261027-062833-034`, 308 assertions, zero failures. Native palm centres overlap with a 2.7 cm vertical gap, palm planes horizontal and native knees approximately 56 cm apart. User accepted the revised pose. All three original CPR clips re-exported/compiled.
- Original field Surgery/Suture/Splint enter/loop/exit clips exported and compiled; standalone playback `.checks/20261027-064210-161`, `064221-666`, `064233-088`, each 306 assertions, zero failures.
- Full clinical animation route `.checks/20261027-074923-411`: 325 assertions, zero failures. Actual native unconscious patient, cancellation retains kit, full 30/60/60-second splint/extraction/suture operations, expected graphs remain active at midpoint, no early consumption/effect, exact supplies consumed only at completion. Earlier failures traced to a ragdoll settling off the narrow diagnostic platform; fixture floor enlarged without weakening reach/clinical checks. These are original motion prototypes, not final instrument/finger/prop art.
- Native UI `.checks/20261027-074719-820`: 22 assertions, zero failures. Six labelled top controls, native BI button inheritance, no overlapping tab bounds, CLOSE MENU, anatomy prompt above image, body/category selection retention, inappropriate arm airway actions absent, blank initial findings, named current patient and vehicle roster layout, progressing examination bar, unnecessary bandaging disabled and panel retained. Includes medic 15→9-second timing and role revocation; expanded restriction tests added subsequently.
- GM/native baseline `.checks/20261027-074715-189`: 301 assertions, zero failures. All 111 settings share one category with 14 display-only headings; Make medic and Remove role merge into native context list without losing native actions; schema-15 migration preserves existing values and adds medic defaults.
- Dedicated restart persistence `.checks/persistence-current-20261027-075300-410`: seed and verification run in separate server processes; verification 311 assertions, zero failures. Current schema 16 settings survive restart. Deliberately invalid fixture settings emit the expected protection diagnostic; that is not a production-profile corruption result.

Remaining acceptance: custom field clips need owner/proxy/JIP clinical coverage and visual instrument/contact polish; casualty carry/drag, rope grip and oral-pill animations remain unfinished. Expanded drugs, pulse oximeter and separate plasma/saline physiology remain outstanding. The mod is not release-complete.


## 27 October 2026 — assets, monitoring and TXA

- Native import completed for eleven models including independent Naloxone/NCD/TXA variants. Stable material names correct the importer-generated empty-material problem. All eleven native model/material/texture dependency checks passed. User-approved cap colours preserve the original neutral printed body and PBR detail.
- `.checks/20261027-095939-272`: current in-game pulse-monitor fixture, 337 assertions and zero failures. Real reusable item, native timed setup, shared live measurements, region markers without cross-region leakage, distance/device-loss shutdown.
- `.checks/20261027-090632-959`: native foreground audio fixture, 320 assertions and zero failures at that revision. Heart-rate beep returns a valid native playback handle. This is local distance-gated audio, not yet a physical attached 3D sensor.
- `.checks/20261027-093515-414`: TXA native runtime, 332 assertions and zero failures. Vascular access policy, real injection, cancellation retains kit, completion consumes exactly one, delayed effect and repeat controls.
- `.checks/20261027-094708-545`: native UI smoke, 26 assertions and zero failures, including visible GM heading text/bounds.
- `.checks/persistence-current-20261027-094719-226`: separate-process schema-18 settings seed/reload; 332 verification assertions, zero failures. All 123 settings covered. This does not claim complete world/patient or per-account life persistence.
- `.checks/splint-network-20261027-093955-250`: dedicated server plus three clients, 331 authority assertions and zero failures. Actual owner splint/bandage/monitor operations, replicated live numeric readings and visual state, device-loss shutdown visible to all three clients. The third client joined before monitoring started; already-active join acceptance is a separate fixture.

The simple TXA model reduces dressing wear after onset. It does not replace blood, close untreated wounds, repair internal sources, or claim to simulate full coagulation. Full mod completion remains tracked in Docs/ROADMAP.md.

- `.checks/splint-network-20261027-100031-934`: already-active monitor join acceptance passed with 328 authority assertions and zero failures. The third client was launched only after phase 9 (monitor active); it received active monitoring, numeric readings and fracture/splint markers, opened the live medical panel, and observed device-loss shutdown alongside both existing clients. Reproduce with `Tools/Validate-SplintNetwork.ps1 -PulseActiveJoin`. This establishes active-session late joining, not monitor persistence across server restart.

## 27 October 2026: resumed assets and two-player stretcher transport

All production changes remain in this standalone addon. Git publishing is deferred. Native test profiles expose only this addon, its unpublished diagnostic addon and the base game.

- Model/material acceptance: `Tools/AssetImport/Verify-MedicalMaterials.py` verifies 22 compiled model entries, native material mappings, packed BCR/NMO texture dependencies and compiled/source freshness. New supplied meshes include the stretcher, splint, surgery kit, suture pouch, separate AED pads, BVM, monitor, cannula, suction and chest seal. Laryngoscope conversion is staged without a new clinical procedure.
- Native field procedures: `.checks/20261027-162421-569`, 365 assertions, zero failures. Actual imported kit inventory, cancellation/retention, full 30-second splint and 60-second surgery/suture loops, and exact consumption passed.
- Equipment regression: `.checks/20261027-164000-415`, 399 assertions, zero failures. All twelve advanced equipment procedures use real inventory, native timed item animation and configured durations. Checks include side-correct seal/decompression, finite AED analysis/discharge, airway/ventilation, monitor, chest drainage with remaining bleeding source, separate IV access and finite fluid delivery. Earlier refreshed runs lost treatment reach when an unconscious casualty rolled off the narrow stepped test floor. The acceptance scene uses the broad flat collision deck; production reach checks were preserved.
- Separate dedicated-server restart: `.checks/persistence-current-20261027-161929-200`, verify phase 353 assertions, zero failures. Schema 19 preserves prior schema 18 values; nondefault transport/stability/speed/handle/span settings survive separate-process reload. There are 136 saved controls, under the same BCM Medical tab with fourteen inline headings.
- Player-casualty multiplayer: `.checks/stretcher-network-20261027-162203-081`, 367 server assertions, zero failures. Two actual client-owned carriers walk using native controls; the third client owns the unconscious casualty. Owner/proxy/late-join acceptance includes lift, movement, set-down, repeated lifts, GM disable/re-enable, stale grip RPC rejection, actual carrier disconnect, retained inventory and adjacent unloading. Native character teleport updates throughout transport fixed old-position restoration on player-owned casualty release. Character physics and medical state remain active.

Final carrier hand-contact animation, applied-item art and transparent-plastic/PBR material polish remain unfinished. Stretcher vehicle transport and conscious casualty consent are not covered by this acceptance. Settings persistence does not imply complete patient/world/account persistence.
### Final acceptance of the resumed implementation

- `.checks/stretcher-network-20261027-172150-059`: 372 server assertions, zero failures; the complete native owner/proxy/late-join gate passed. Every client confirmed nondefault turn rate 150, handle offset 1.7 m and maximum span 3.8 m through both direct and bulk settings readers. Two real player carriers walked, released and rejoined; live GM disable/re-enable, stale-release rejection, a native player disconnect and player-owned casualty unloading passed. Both surviving clients observed the detached casualty beside the stretcher at phase 20. No acceptance distance or inventory check was weakened.
- The final placement path uses native world transforms on authority and casualty owner, an explicit reliable owner confirmation, duplicate/release tokens and a short reconciliation window for unloading. Occupied stretchers continue supporting their casualty after set-down. Normal character physics and clinical state remain active; no persistent physics suspension, healing or inventory substitution is used.
- Settings delivery uses complete validated reliable owner snapshots, with a join request until the first snapshot arrives and revisions rejecting older snapshots. The diagnostic change helper now explicitly publishes edits. Four additional assertions cover atomic changes to related limits and rejection of conflicting, out-of-range or incomplete snapshots.
- `.checks/persistence-current-20261027-172352-292`: separate dedicated seed/reload processes passed; the final reload has 357 assertions, zero failures, including saved nondefault transport/stability controls.
- `.checks/20261027-172358-325`: native 1280x720 interface smoke, 26 assertions, zero failures. Anatomy selection, unavailable-treatment behavior, progress and highlight checks remain accepted.

These are local dedicated-server and diagnostic-client tests. They do not establish Internet latency/load acceptance, final carrier hand-contact animation, applied-instrument attachment art or completion of the entire medical-mod roadmap. The 399-assertion equipment fixture and 365-assertion full field-procedure fixture above remain the corresponding equipment/animation evidence.
## Clinical integration and shelved carrying � 27 October 2026

Two-player carrying was shelved by the user after visual review. Its normal and grip defaults are disabled; the retained grip remains diagnostic-only. Earlier carrying mechanics checks below do not imply final visual approval.

Schema 22/169-control baseline: `.checks/20261027-210128-918` passed 428 clinical safety/domain checks; `.checks/20261027-210349-128` passed 444 native medical/bleeding checks; `.checks/persistence-current-20261027-210359-503` passed separate-process reload with 426 assertions. `.checks/20261027-210407-469` passed 26 native UI smoke assertions. Actual morphine/epinephrine and TXA full timed consumption/cancellation passed 437 and 422 checks in `.checks/20261027-210513-980` and `.checks/20261027-210602-502`. Applied-device and unsafe-owner-RPC acceptance `.checks/applied-equipment-network-20261027-210910-045` passed the full three-client phase gate, including specific authority pressure denial, exact retained supplies, live GM toggles and FullHeal cleanup.

Schema 23 adds eight typed-fluid controls (177 total) and two distinct bag supply types (22 supply slots total). `.checks/20261027-211836-898`: 437 domain checks, zero failures. `.checks/20261027-212117-817`: 479 checks, zero failures, actual inventory saline/plasma/whole-blood bags with full default 60-second treatment animations, midpoint no-early-delivery, exact consumption, native finite reserve, component accounting, saline redistribution and FullHeal reset. Workbench registration `.checks/20261027-212056-133` corrected newly added prefab GUID registration before this acceptance.

`.checks/20261027-212550-525`: 455 clinical safety/domain assertions, zero failures. A cause-corrected red-cell replacement/CPR/ventilation scenario now demonstrates ROSC, while pressure/ventilation alone fail with inadequate reserve. Native consumable adapters now retain BaseContainerProps config reflection; this run resolves the guarded effect from the actual stock morphine prefab. It does not yet prove every stock item's full UI/consumption route. Actual native saline delivers while eligible and terminates when access is lost.

`Tools/AssetImport/Verify-MedicalMaterials.py` verifies 27 compiled model/material entries and packed dependencies. Applied visuals are mesh-only client cosmetics; final close-up alignment, clothing coverage, first-person clipping, transparency and tourniquet placement remain separate art acceptance.

Typed-fluid dedicated acceptance `.checks/fluid-network-20261027-214859-602` passed: 454 server assertions, zero failures, and all five phases observed by owner, proxy and late joiner. The real Fluids-tab requests animate and consume one bag each. Authority checks finite native volume increments while accounting for ongoing water redistribution, exact red-cell/clotting components, stable ventilation and FullHeal reset. Clients confirm pulse, systolic pressure, respiratory rate, saturation and nondefault retention/redistribution/treatment settings. Native ownership is asserted before the owner requests. Earlier failing profiles remain retained under `.checks/fluid-network-*`.

The new fixture initially omitted replication ownership transfer; assigning a controlled entity alone was insufficient for native player item commands. Later observer avatars obstructed the treatment trace when placed in the same location. Both fixture issues were corrected without removing clinical, native readiness, line-of-sight or inventory checks. A final volume expectation was corrected to account for real redistribution during later infusions. Native effect categories are explicitly initialised for equipment versus injectors; hit-zone presentation predicates do not impose unrelated native wound requirements. Installed native source inspection remains diagnostic material under `.checks`, not bundled implementation.

`.checks/20261027-214751-381`: 459 clinical safety assertions, zero failures. Includes real saline animation followed by access loss, cancellation before any delivery, retained exact bag, distinct native item categories, actual stock morphine effect reflection, native saline guard termination and cause-corrected recovery. The protected invalid-settings diagnostic is deliberate corruption-fixture coverage, not a production startup acceptance error.

Settings persistence is separate from full patient/world/account persistence, which remains unfinished. No whole-mod or Internet/load acceptance is claimed.

### Frozen schema 23 regression acceptance

- `.checks/applied-equipment-network-20261027-215223-513`: 444 server assertions, zero failures, and all five phases on each of three clients. The diagnostic body now has real native replication ownership; after position synchronisation the actual owner receives the pressure-specific morphine refusal, retains the exact item and adds no dose. GM visibility toggles and FullHeal cleanup pass on owner/proxies/late join.
- `.checks/20261027-215312-375`: 470 native medical/bleeding assertions, zero failures. Includes the linked model suite and native blood-loss/control/rescue integration.
- `.checks/persistence-current-20261027-215323-505`: separate dedicated seed/reload processes; final verification 454 assertions, zero failures. Nondefault saline/plasma retention, redistribution time, dilution wear, medication safety and existing settings persist, and the actual game mode reads them after restart.
- `.checks/20261027-215332-808`: 1280x720 native medical interface smoke, 26 assertions, zero failures.
- SHA-256 snapshot `.checks/clinical-source-freeze-schema23.json` covers gameplay/test scripts, prefabs, configs and layouts. The pipeline reported `BCMM_SCHEMA23_ACCEPTANCE_SOURCE_UNCHANGED` after the above gates.

The full-duration twelve-equipment and native medication/TXA regressions subsequently passed against that same frozen build; evidence follows.

`.checks/20261027-215425-808`: 496 native equipment assertions, zero failures. All twelve real-inventory procedures ran at their configured default durations against the final linked physiology/native item category build. Assertions cover midpoint native animations and reservations, side-specific chest care, finite AED discharge and stand-clear/reanalysis, obstruction types, BVM without artificial ROSC, drainage without source repair, separate access and measured whole-blood completion.

`.checks/20261027-215946-676`: 463 native medication assertions, zero failures. Real default-time other-patient morphine and self epinephrine, exact supply consumption, cancellation/animation release, gradual pain handling and actual pulse change pass.

`Tools/Validate-ClinicalAcceptance.ps1` is a reusable orchestration entry point for these existing gates, with optional full procedures, per-step records and before/after complete source manifests. Its PowerShell parser check passed; the individual gates above supply the current execution evidence.

`.checks/20261027-220035-819`: 448 native TXA assertions, zero failures. Real injectable item, access prerequisite, cancellation and exact retained item, completed single administration, gradual clot-support onset and absence of opioid/pressure modifiers pass. `.checks/20261027-220141-874`: final native Workbench script/resource validation succeeds. The regression pipeline reported `BCMM_SCHEMA23_FULL_PROCEDURES_SOURCE_UNCHANGED`.

`.checks/20261027-220219-675`: 462 full native field-procedure assertions, zero failures. Original 30-second splint, 60-second extraction and 60-second suturing loops remain active at midpoint, preserve the exact kit on interruption, consume only at authority completion and apply only the selected treatment. Extraction leaves suturing separate. This fixture deliberately holds native unconscious presentation without inducing terminal physiology; the native medical and linked clinical scenarios supply the independent physiological acceptance. Source fingerprints remain unchanged (`BCMM_SCHEMA23_FIELD_PROCEDURES_SOURCE_UNCHANGED`).

All gates recorded in this clinical pass are complete. The entire medical-mod roadmap is not complete: supplemental oxygen workflows, broader medications, long-term component recovery/overload, actual vehicle treatment, further injury/organ calibration, full patient/world/account persistence, distribution, final equipment/contact art, asset provenance and Internet/load acceptance remain. Two-player stretcher carrying remains shelved.

## Oxygen and expanded medication integration - 28 October 2026

Schema 24 appends 33 controls, giving 210 saved GM settings in the existing one-tab/fourteen-heading layout. The private supply vector now contains six medications and fifteen equipment kinds (25 slots). Prior named-key settings migrate without replacing existing choices.

- `.checks/20261028-063610-197`: 491 model/domain assertions, zero failures. Oxygen limits, oral swallowing conditions, independent tablet kinetics, opioid accumulation/reversal, fentanyl residual exposure and schema 23 migration are included.
- `.checks/20261028-064439-037`: 514 native runtime assertions, zero failures. Real default-time oxygen connection/removal, retained finite capacity, reconnection/depletion, default fentanyl role/access, actual injection/reversal and unresponsive/awake oral use passed. The positive oral test waits for the native wake-up transition, preserving the real consciousness safeguard.
- `.checks/oxygen-medication-network-20261028-064834-645`: 502 server assertions, zero failures, with all required owner/proxy/active-session late-join phases. Actual medical-menu requests drive oxygen, fentanyl, naloxone and tablets.
- `.checks/oxygen-medication-network-20261028-065420-246`: expanded ten-phase gate, 505 server assertions, zero failures, all required phases on every client. Genuine native possession, finite retained source, live GM disable, stronger-opioid breathing/pressure effects, reversal, tablet pain relief, fuller-spare selection, actual source deletion and reset cleanup pass. The late join occurs while oxygen is active and confirms the cosmetic mask without usable/physical duplicate items.

The initial compile naming conflict, missing oxygen presentation bit and the too-early positive oral fixture are retained as failed development profiles. Native swallowing refusal was preserved; no clinical eligibility check was bypassed to make the test pass.

Remaining visuals: cylinder/source presentation, final mask alignment/transparency, tablet/blister model and hand-to-mouth animation, and a dedicated fentanyl label. Existing supplied cap colours are unchanged. Detailed gas exchange, hyperoxia/toxicity, hepatic toxicity, real dosing and tolerance are not claimed. See `Docs/OXYGEN-AND-EXPANDED-MEDICATIONS.md` for sources, defaults and limitations. Further regression evidence follows.

### Oxygen continuity and final native recheck, 28 October 2026

`.checks/20261028-070731-380` passed **517 checks, zero failures** on the current schema-24 build. This includes a usable spare selected ahead of an empty retained source, actual native unconsciousness while self oxygen remains connected, and patient-reset mask/session cleanup. The preceding `.checks/20261028-070408-541` exposed the native incapacitated life-state guard; continuous supply now rejects death rather than requiring the conscious ALIVE state. Initial timed connection still enforces normal treatment eligibility. The earlier UI profile `.checks/20261028-065933-877` failed a stale three-action Head/Airway assertion after two oxygen controls were added; the test now checks the actual five actions and both oxygen labels.

Material validation still passes all **27** native model/material/packed-texture entries. Settings catalog validation passes **210 unique keys**, every help description and default bounds. Full final regression report is `.checks/clinical-acceptance-20261028-070956-167/report.json`; a running report is not acceptance until its final status and source hashes pass.

### Final unchanged-source clinical acceptance, 28 October 2026

Report: `.checks/clinical-acceptance-20261028-070956-167/report.json` -- **passed**, **sourceUnchanged: true**. Every listed gate passed on the same frozen build:

| Gate | Profile | Checks / failures |
| --- | --- | --- |
| Native resource/script validation | `20261028-070956-247` | successful |
| Clinical safety | `20261028-071006-359` | 512 / 0 |
| Oxygen/medication owner/proxy/active-session JIP, ten phases | `oxygen-medication-network-20261028-071029-699` | 505 / 0 |
| Typed fluids, actual menu and three clients | `fluid-network-20261028-071306-305` | 507 / 0 |
| Applied-device owner/proxy/JIP | `applied-equipment-network-20261028-071456-698` | 497 / 0 |
| Native bleeding integration | `20261028-071545-935` | 523 / 0 |
| Separate-process settings seed / reload | `persistence-current-20261028-071557-856` | 493 / 0; 509 / 0 |
| Native medical UI, 1280x720 | `20261028-071609-004` | 26 / 0 |

The full default-time native oxygen/medication fixture above passed 517 / 0 immediately before this suite. `.checks/oxygen-clinical-source-freeze-schema24-final.json` contains 224 source fingerprints; a second comparison confirmed no changes through both runs. Validation runner syntax, 210-key catalog help/default bounds and all 27 native material dependencies also passed. Isolated test processes were cleaned up. This acceptance does not approve final mask alignment/materials, a separate cylinder, tablet packaging/hand-to-mouth motion, Internet latency/load or full clinical/world/account persistence. Two-player stretcher carrying remains shelved and disabled by default; Git publishing remains deferred.


## 28 October 2026 - corrected tablet hand, native arsenals and schema 25

Right tablet palm now faces inward toward the mouth. Original self/handover entry-loop-exit resources were rebuilt and native playback reviewed. Mask facial frame and saved alignment controls are integrated. Native US/USSR/FIA catalogues each retain ordinary supplies and add 28 BCM Medical inventory entries, with live native cache filtering and authoritative request policy. Full purchase/refund transactions remain separate acceptance.

Final unchanged-source clinical acceptance: .checks/clinical-acceptance-20261028-095514-670/report.json. All nine gates passed; sourceUnchanged true, including animation source/compiled resource fingerprints.

- resources: {"checks": [], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\20261028-095515-853\\logs\\logs_2026-10-28_09-55-16\\console.log"]}
- native-faction-arsenals: {"checks": [["558", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\20261028-095527-104\\logs\\logs_2026-10-28_09-55-27\\console.log"]}
- clinical-safety: {"checks": [["557", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\20261028-095541-122\\logs\\logs_2026-10-28_09-55-41\\console.log"]}
- oxygen-medication-network: {"checks": [["554", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\oxygen-medication-network-20261028-095605-182"]}
- typed-fluid-network: {"checks": [["552", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\fluid-network-20261028-095928-724"]}
- applied-equipment-network: {"checks": [["542", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\applied-equipment-network-20261028-100120-280"]}
- native-bleeding: {"checks": [["568", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\20261028-100210-619\\logs\\logs_2026-10-28_10-02-10\\console.log"]}
- server-restart: {"checks": [["538", "0"], ["555", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\persistence-current-20261028-100223-369\\logs\\logs_2026-10-28_10-02-23\\console.log", "C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\persistence-current-20261028-100223-369\\logs\\logs_2026-10-28_10-02-29\\console.log", "C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\persistence-current-20261028-100223-369"]}
- medical-ui: {"checks": [["26", "0"]], "paths": ["C:\\Users\\ruanb\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\MCB Medical\\.checks\\20261028-100236-079\\logs\\logs_2026-10-28_10-02-36\\console.log"]}

Thirteen oxygen/medication phases included native self and other tablet playback on owner/proxy/JIP, client nondefault settings, exact finite consumption and owner displacement cancellation with kit retention. The displacement fixture is not a physical keyboard-input test. Settings restart covers schema 25 and saved mask/tablet/arsenal nondefaults. Twenty-seven compiled models/material dependencies also verified. Final props/material/contact review, cylinder, full patient/world/account persistence, Internet/load acceptance and remaining whole-mod ledger are unfinished. Two-player stretcher carrying stays shelved.

## Body-bag integration destination confirmed, 2026-10-28

All body-bag production work now belongs in BCM Medical, as reconfirmed by the
user. The standalone BCM-BodyBags addon is being retired; its ACE-dependent
results are reference evidence and do not certify this native implementation.
See Docs/BODY-BAG-INTEGRATION.md for the preserved reference archive and scope.

Native Medical cargo baseline `.checks/20261028-180957-769` passes 600 assertions.
A new real backpack-inside-vehicle fixture reproduces wrong cargo classification
in `.checks/20261028-181233-232`. The policy now stops at movable inventory parent
slots while accepting fixed authored child storage; cargo discovery/insertion
share valid native-capacity storage search. Post-fix `.checks/20261028-181316-440`
passes 603 assertions, zero failures, across all four native cargo fixtures.
The existing intentional invalid-settings fault-injection diagnostic remains in
these domain runs; it is not described as a clean error-free log.

The current native cargo network regression
`.checks/cargo-network-20261028-181612-199` passes 603 authority assertions and all
eight phases on each of three clients including late join. Native stored entities
may stream out; its manifest/state evidence retains that documented scope.

The new Medical-only ordinary-vehicle test
`.checks/bodybag-vehicle-20261028-182756-719` passes 11 authority assertions and all
three phases on both owner and late joiner. It uses an ordinary native M923 with
no BCM vehicle override, explicit real ownership, and native inventory sessions
and cached resource counts. Loading transfers the exact equipment into the
vehicle; unloading exposes the original bag while deliberately leaving that
exact equipment in cargo; deleting the unloaded bag preserves the equipment.
Native inventory sessions make stored contents relevant without treating a
legitimate streamed-out entity as lost. The diagnostic platform is replicated
and physically supports both the vehicle and owned characters. Earlier attempts
fell off the plain world's narrow support, then measured authored prefab scale
being replaced by the spawn transform; those fixture failures are superseded by
the accepted scaled-platform run. No corresponding production cargo defect was
inferred from those invalid fixtures.

Medical-only piloted Mi-8 acceptance
`.checks/bodybag-vehicle-20261028-183926-158` passes all 13 authority assertions
and six owner/late-join phase checks, with actual pilot and character ownership
confirmed. Native collective/cyclic control under gravity performs takeoff,
horizontal movement and landing. The pilot parks/exits, the test waits for
Medical's configured stationary-vehicle limit, and unloading leaves the exact
original equipment in the vehicle. Deleting the unloaded bag preserves it.

The preceding generated-floor fixture had an invalid prefab replication GUID
and is not accepted flight evidence. Canonical native static platform geometry
is now created identically on every peer, with measured bounds; the runner fails
on all RPL errors. A following attempt passed landing but correctly failed
unloading while the helicopter still exceeded Medical's speed limit. Waiting for
natural settling and using the native parking brake corrected that test without
changing the production speed policy. No standalone Body Bags or ACE dependency
is loaded for this vanilla Medical test.

The initial eight-vehicle Medical matrix (`bodybag-compatibility-20261028-184314-107`)
was interrupted after client visibility failures and is **not an accepted pass**.
Server cargo assertions passed, but ownership transfer could race client setup,
leaving the client without the actor/vehicle entities. The test now requires a
real owned-controller ready RPC before assigning the performer, and applies the
same readiness gate to the late-join body. Focused native truck regression
`bodybag-vehicle-20261028-184821-361` passes all eleven assertions and all six
owner/late-join checks with that handshake. The matrix is being rerun. No
production ownership defect is inferred from that premature test setup.

Medical eight-vehicle cargo compatibility matrix
`.checks/bodybag-compatibility-20261028-184944-161` passes all eight fixtures:
vanilla Mi-8, MRZR D4, JLTV, Cougar 4x4, Medical Little Bird, BCM UH-60, BCM HH-60G
and BCM Chinook. Each passes eleven authority assertions and three phases on both
owner and late joiner, with actual character ownership confirmed. Exact original
equipment moves into the vehicle, remains there after unloading and survives
unloaded-bag deletion. Ordinary vehicles do not require a BCM cargo component.
The HH-60G run uses the narrow upstream supplies-diagnostic policy; that subsystem
is not certified by cargo acceptance. These are Medical's own results, superseding
the interrupted initial matrix; they do not claim piloted acceptance for all five
aircraft. Native piloted Mi-8 acceptance is recorded separately above.

Production Workbench validation `.checks/20261028-185444-111` passes with only
Medical and the base game loaded. The standalone Body Bags/ACE dependencies are
not required by Medical. Native runtime test artifacts and source manifests stay
under `.checks/`.

External-transfer regression `.checks/bodybag-vehicle-20261028-185658-074`
reproduces a production association bug: native A-to-B transfer succeeds but the
bag retains vehicle A's ID. Medical now resolves the actual eligible storage
owner, removes A's optional manifest entry and reconciles B through the existing
cargo/equipment coordinator. Equipment already transferred into A is not moved
or duplicated. This also supports fixed authored storage children without a
blanket vehicle prefab override.

Post-fix authority-only `.checks/bodybag-vehicle-20261028-185752-965` passes all
16 assertions: exact native A-to-B move, updated B association, original equipment
remaining in A, bag survival after A is deleted, native disposal of equipment
belonging to deleted A, ordinary unloading from B, unlocked world state and
preserved deceased name/readiness. This is privileged native API coverage, not
human UI or arbitrary custom cargo acceptance. The previous all-eight cargo
matrix predates this focused fix; the full piloted aircraft matrix now exercises
the updated stored-state reconciliation path.

Medical's complete native piloted aircraft coverage is now accepted across two
record sets: Mi-8 and Medical Little Bird in
`bodybag-compatibility-20261028-185944-500`, plus UH-60, HH-60G and Chinook in
`bodybag-compatibility-20261028-190729-300`. Each passes thirteen authority checks,
all six owner/late-join checks and explicit native pilot/character ownership.
Collective/cyclic controls perform real takeoff, horizontal movement and landing;
parking/settling precedes Medical unloading, with original equipment deliberately
retained in vehicle storage. The heavy-aircraft test starts its flight program at
90% of native target rotor RPM; UH-60 took about 82 seconds to reach that threshold.
The earlier UH-60 timeout in the first set is superseded by its later full pass.
These are automated native control tests, not physical keyboard/gamepad sign-off.

The initial Medical garbage test `remains-lifetime-20261028-192008-340` reproduced
three failures: native collection accepted both corpse and filled bag immediately
and physically removed them alongside the ordinary control. A native lifetime
broker now observes actual DEAD life-state transitions through Medical's damage
manager, protects exposed corpses for at least 1,800 seconds, retains active or
attached protection afterward, and protects filled bags for their lifetime.
Bagging release no longer removes a still-required minimum blacklist. The native
character controller has no OnPostInit override, so the initial compile attempt
was corrected to Medical's supported damage-manager initialization/invoker path.
Quick regression `remains-lifetime-20261028-192302-511` passes all seven checks:
both protected entities survive while the same native collector removes the
ordinary control. The real thirty-minute and broader bagging regression remain
pending; no shortened minimum is used.

Strengthened Medical burial `20261028-201353-559` passes all 628 assertions with zero failures, including exact original equipment disposal for the first bag, two full native thirty-second faction-shovel completions, cancellation and memorial lifetime checks. Corrected full-duration lifetime run `remains-lifetime-20261028-201531-888` is now live; its initial seven native garbage checks pass. Its complete thirty-minute result remains pending.

Medical corrected full-duration lifetime `remains-lifetime-20261028-201531-888` passes all fifteen native assertions, zero failures. The original corpse rejects collection at actual 1798.43 seconds, becomes eligible at 1802.93 seconds, is physically collected, and the original filled bag remains protected. The lifetime-broker source hash is unchanged from the captured manifest. Later other-module and filled-bag editor/presentation changes still need final combined-source regression.

Current native deployment/disposition adapter `.checks/20261028-204545-072` passes 586 assertions, zero failures, with test-only trusted identities and real native entities. It covers current exact-token blocking/release, allowance reset preserving the independent wait, stale release rejection, fresh rejected-body cleanup, registered-life preservation, migration and model timeout/live-setting cases. Actual native heartbeat timeout, real backend reconnect and GM/owner UI remain separate acceptance.

Updated rollback `.checks/20261028-204553-100` passes 624 assertions, zero failures. Actual owning-client disconnect `.checks/bodybag-vehicle-20261028-204616-331` passes eighteen assertions, zero failures: native PC/connection removed, original corpse/unused kit survive, reservation releases and garbage protection remains. Only the exact test client PID recorded by the runner was stopped.

Strict Medical terrain/tool/placement acceptance `.checks/20261028-211946-588` passes all 671 native assertions, zero failures. Both native faction entrenching tools are actually held and remain unconsumed; stored tools are rejected. Actual grass is accepted, a native concrete overlay and moving bag are rejected. Both full thirty-second burials preserve original equipment disposal, memorial identity/lifetime and the approved 0.59 scale, terrain-aligned long axis, X/Z mesh-bounds centring and -0.1 m pivot offset. Runtime scale is read from the owned prefab's native source and applied to spawn axes; no duplicated runtime scale constant replaces the prefab definition. The earlier unit-transform scale failure is superseded by this full pass.

Connected return-home diagnostic: the first network fixture failed compilation because its additional modded layer loaded before the base diagnostic layer; its generated filename now enforces ordering. Two subsequent attempts stopped on the native M997 replicated area-mesh offset diagnostic (approximately 0.0007 m in X/Y). Fixed-coordinate spawning does not remove that upstream client rounding difference. The runner now bounds that specific HEAL area-mesh message to less than 0.001 m per axis and only in ReturnHome mode; all other errors remain fatal. These attempts are not accepted passes. Actual native station boundary and server range checks remain required independently.

Connected return-home `bodybag-vehicle-20261028-214633-092` passes eighteen authority assertions and all six owner/late-join cargo phase checks. The actual original owning client invokes the production send-home request RPC, receives its exact death token and SENT_HOME kind, and acknowledges while still owning/controlling the original performer. Native hint API reports `hintShown=1`; the observer receives no private notice. Original equipment previously left in the first vehicle remains there. The test-only station traversal was expanded to explicit loop statements after the compact form stalled the preceding attempt; the corrected run completes. Its narrow native M997 sub-millimetre mesh diagnostic allowance is documented above. Hint API success does not replace screenshot/human UI acceptance.

Burial now starts/stops the installed native entrenching-tool item-use animation on the original performing controller owner. Early item-use end or start failure requests authority cancellation with the exact death token; stale tokens cannot cancel a different operation. Release stops only the matching local animation. Compile `20261028-214808-137` passes. Native held-tool/terrain regression is running; real owner animation/cancellation and visual acceptance remain pending.

Burial authority regression `20261028-214845-489` passes 673 assertions, zero failures. Local native owner `20261028-215104-331` passes 680 assertions, zero failures: the actual local controller owns the performer, both faction tools run native IsUsingItem during their full thirty-second burials, exact animation tokens remain active, release clears the animation, and range/stowing cancellation preserves remains. Both suites retain natural terrain, exact grave transform, disposal and lifetime checks. Expected invalid-settings diagnostic is deliberately exercised. This is actual local native item-use acceptance, not dedicated owner/observer animation replication or human visual signoff.

Dedicated burial/memorial `burial-network-20261028-215611-965` passes 646 authority assertions, actual owner production burial RPC, actual native item-use observed by owner and proxy, owner-only completion token, and exact replicated name/token/scale on the owner, observer and post-burial joiner. Native hint API succeeds for the owner confirmation. The diagnostic client role labels are launch slots; the first ready client's controller is the actual performer and can differ from the slot named owner.

Strengthened native vehicle-loss `bodybag-vehicle-20261028-215732-957` passes 33 checks, zero failures. In addition to wreck and direct hierarchy loss, actual explicit deletion of a reconciled stored bag becomes ADMIN_RELEASED after the child-first grace interval, preserves the live parent and exact unrelated native cargo, and cannot be rewritten by later parent deletion. This uses the actual deletion API; native GM UI selection remains a separate gate.

Grave inspection previously exposed only an action label. It now presents the replicated deceased-name snapshot locally, formatted from an independent Medical localization key, without requiring the deleted corpse. English source/runtime string tables are registered for native project configurations; no legacy/ACE dependency is introduced. Native `burial-network-20261028-220047-744` passes 646 assertions and all three clients open the localized memorial through the production local inspection method with hintShown=1 and the exact formatted text. The post-burial joiner confirms the same behavior. This run's source manifest includes Language sources and metadata. A follow-up diagnostic now dispatches the actual registered grave action instead of calling its presentation method directly; that stronger dispatch check remains pending.

A native reservation-removal guard is being validated at ScriptedBaseInventoryStorageComponent. It follows the exact containing bag, including nested kit, and denies removal while Ready is false unless Medical is performing its original internal transfer. The first attempted BaseInventoryStorageComponent hook is an engine class and cannot be modded; the native compiler rejected it. The supported scripted extension point compiles and its real removal/refusal/cancellation suite is running. Full native searching-session exclusion is still unfinished; this guard alone is not evidence of that requirement.

Native removal guard `20261028-220607-021` passes 750 assertions, zero failures, including native CanRemove and actual TryRemove requests on original/nested equipment during burial reservation, both full burials and cancellation. The guard is implemented on the supported ScriptedBaseInventoryStorageComponent, with the Medical internal transfer exception. Expanded native cancellation-restoration assertions were added afterward. Native item IsLocked preconditions now also refuse burial/home before reservation and cancel an active burial when a native inventory lock intervenes. Authority-only attempts to create that user lock are not accepted evidence: RequestUserLock returned false with both an unowned actor and the documented optional null caller (`20261028-220858-287`, `20261028-220958-133`). Those inappropriate setup assertions have been moved out of the baseline suites. The actual owning-client open-inventory/lock diagnostic is now running; server-observed lock and disposal refusal remain required. No search-exclusion acceptance is claimed from the failed authority setups.

Current registered-action multiplayer `burial-network-20261028-221309-201` passes 754 assertions, zero failures, SourceUnchanged=true. All three clients enumerate the native grave's registered ActionsManager actions, resolve BCMM_InspectGraveAction, invoke its production PerformAction, and obtain the exact localized memorial with hintShown=1. The late joiner arrives only after BURIED and physical bag deletion. Native owner/proxy item-use, exact private token, scale, original equipment disposal, removal refusal during reservation and actual native cancellation restoration are included. This supersedes the earlier presentation-method-only inspection evidence for action dispatch.

The connected native user-lock experiment also returned false (`bodybag-vehicle-20261028-221127-755`), so it is not lock acceptance. The experimental runner switch/code has been removed from active baselines and preserved in `.checks/native-lock-investigation-20261028-2214/`; failure logs and exact generated addon source remain in the original run. Native IsLocked refusal is retained as an eligibility safeguard, but an open searching UI is not proven to expose such a lock. Explicit search-session coordination and its real UI/authority contention test remain required. Do not treat the 754 removal guard checks as proof that an already-open search prevents operation start.

Current-source regressions after the scripted storage guard pass: `20261028-221428-859` completes 722 full original-equipment corpse-to-bag-to-native-cargo/world assertions, zero failures; `20261028-221527-377` completes 637 actual M997 boundary/disposal/account assertions, zero failures. Both retain the expected intentionally invalid-settings diagnostic. These are targeted body-bag regressions; complete clinical and final aircraft/modded-vehicle source-freeze gates remain necessary.

Search-session integration: BCMM_BagSearch coordinates real native bag views with expiring authority sessions keyed to the original controller, controlled character, target, client request and unique server generation. Native inventory owns item movement. Both the Search action and native loot/window navigation are tracked. Original controller/actor/range/life/readiness checks repeat on renewal; close, range loss, control change and disconnect release only matching sessions. The replicated busy flag and transport revision prevent conflicting disposal/drag/cargo actions and stale transport requests. Native direct cargo insertion also rechecks readiness/search state. No new saved controls or runtime addon dependencies are introduced.

Native two-view search `bodybag-vehicle-20261028-223544-646` passes 31 authority checks and all six cargo phases. Strengthened native direct-inventory/stale-reply/disconnect `bodybag-vehicle-20261028-223902-291` passes 32 checks, SourceUnchanged=true, three owner phases and both observer phases before the exact recorded observer process is stopped; native server disconnection is observed. It does not claim a stopped observer's final phase/privacy check.

Actual held-tool/natural-ground burial search exclusion `burial-network-20261028-224315-807` passes 755 assertions, zero failures, SourceUnchanged=true. A real owning-client production burial request is rejected while its actual native bag inventory is open; after close, full thirty-second digging, original-equipment disposal, private notice and registered localized grave inspection pass on owner, proxy and post-burial joiner.

Possession-return expansion exposed a native menu lifecycle race: a delayed close could remove the inventory after the open attempt. Both controller ownership and native inventory permission were valid. Pending navigation now retries the normal native OpenInventory call when no menu exists, within its existing short grace period. No permission override is applied. The corrected native run `bodybag-vehicle-20261028-225839-063` passes 36 checks, zero failures, SourceUnchanged=true: proper native possession/return, fresh search generation, stale transport revision, direct native reopen, cross-controller/stale reply protection, actual observer disconnect, range cleanup and successful home with original gear left in the truck. A follow-up normal-close run retains the connected observer and independently checks otherwise-valid cargo prerequisites for the stale-revision assertion.

Final normal-close search run `bodybag-vehicle-20261028-230350-290` passes 36 authority checks and all six owner/observer cargo phases, SourceUnchanged=true. It retains the connected observer's private-notice check and explicitly confirms otherwise valid cargo prerequisites before rejecting the old pre-search revision. Search protocol/evidence is documented in Docs/BODY-BAG-SEARCH.md. Current body-bag search implementation is accepted at these recorded sources; human UI/controller, Internet/load and final combined-source clinical/vehicle gates remain separate.

## Respawn requests and review integration — 28 October 2026

BCM Medical owns the account request backend, owner-only status/submission RPC, death-relative countdown and role-checked paginated review RPCs. No caller-supplied account identity is accepted by the network adapter. Each page contains at most 32 matching current deaths; requests-only and all-waits views remain distinct. Native administrator/session-administrator or unrestricted full Game Master authorization is repeated on every snapshot and review. Approval/dismissal also require the current submission revision; all mutations require the exact death token, deployment life and immutable original corpse replication ID. Cooldown survives dismissal and account rebinding. Authority timeout and current physical disposal clear obsolete requests; old physical remains cannot clear a later death.

The native Respawn and Body Bag Admin panel provides player submission/status, Requests/All Waits, pagination, selection, approve, dismiss and explicit wait release. A Body Bag Admin entry is inserted in the pause menu's Game Master section. The rebindable `BCMM_OpenRespawn` action is listed under BCM Medical and starts unbound. Server authorization applies regardless of panel access. Physical disposal and the independent deployment allowance are unchanged by request approval.

Accepted backend runtime: `.checks/20261028-233025-172`, 631 checks, zero failures, including schema-28 migration, saved request settings, timeout/deletion cleanup and countdown boundaries. Native menu: `.checks/20261028-233511-351`, 24 checks, zero failures, including actual native host authorization, production roster/review RPCs, real row creation/selection/approval and preservation of corpse/consumed allowance. Target account binding is diagnostic, not backend audit acceptance. The engine intentionally supplies the local host's native administrator role; this is not a remote ordinary-client or role-revocation test. An earlier 16-check UI run passed basic bindings/close lifecycle; a subsequent fixture exposed an invalid scroll-slot class and a nested dispatch null exception, both corrected before the 24-check run.

Still required: remote owner submission and privacy, actual ordinary-client/limited-GM/revoked-role denial, all-waits release/dismiss UI flows, pagination under populated load, saved-setting process restart/live GM, persistent spawn-menu countdown/spectator integration, native pause placement and human keyboard/gamepad/visual acceptance. Final combined-source transport/clinical/release regressions remain outstanding. The legacy BCM-BodyBags project remains preserved until release migration is validated, then it will be deleted.

Native request/review follow-up: `20261028-234042-999` passes 28 checks, zero failures, including actual native pause-menu placement before the Game Master separator and current-role enable state. `20261028-233810-282` passes 23 checks with actual host administrator/session/GM roles cleared: previously valid exact review rejected, pending state preserved, and denied snapshot removes cached rows. Role regrant is not assumed; the isolated local engine did not restore the host role using GivePlayerRole, so positive and revoked acceptance run in separate disposable processes. The readable full-width roster is asserted and visually inspected from `20261028-233932-047/profile/respawn-admin.png.bmp`; the earlier tiny centred row was corrected using the installed native AlignableSlot definition. Native API automation and this capture do not certify human keyboard/gamepad acceptance or remote peer privacy. The local client logs an upstream `SCR_WidgetExportRuleRoot` diagnostic while loading the base-game IntroSplashScreen before Game compilation; the custom panel itself no longer logs the invalid scroll-slot diagnostic.

Current-role pause follow-up: .checks/20261028-234105-105 passes 26 checks, zero failures, including actual native role removal, rejected stale review, cleared denied roster and disabled Body Bag Admin pause entry. The native pause capture from the 28-check positive run was inspected and confirms the entry in the Game Master group. Remote peer privacy/identity acceptance remains open.


## Connected requests and native deploy screen — 29 October 2026

Production now supplies owner-only disposition/request state, authoritative burial policy and a death-relative countdown to the native SCR_DeployMenuMain. The persistent status refreshes every five seconds, rounds displayed time upward to five-second boundaries, and waits for server release at zero. It is removed when the current wait clears or the menu closes. The native deploy control remains disabled under Medical's independent gate/allowance and shows a readable Medical reason. The actual native request button uses the same account-bound production submission RPC. Dismissal now immediately publishes updated private owner status while retaining the existing submission cooldown and physical wait.

Accepted connected evidence: `.checks/respawn-network-20261028-234804-641` passed 29 authority checks with SourceUnchanged=true on two actual peers: owner submission/default cooldown, private owner status, ordinary roster denial, Game Master-role-only denial without unrestricted EDIT, native session-administrator and full Game Master snapshots, role revoke/regrant, stale cached selection rejection, dismissal, All Waits explicit release, preserved native corpse and unchanged consumed allowance. These are diagnostic account bindings to actual controller IDs, not backend account audit/reconnect acceptance.

Accepted native deploy evidence: `.checks/respawn-network-20261029-000351-926` passed 34 authority checks with SourceUnchanged=true in a mapped Arland native editor mission. The actual deploy menu and disabled deploy control, live countdown, enabled request button, real button callback submission (no fallback allowed), authoritative burial-off waiting text, role-checked review and owner status removal after release all passed. Engine captures of the waiting and burial-disabled states were inspected. Backend/model/schema regression `.checks/20261029-000448-713` passed 634 assertions, zero failures, including countdown boundaries and saved schema-28 settings. Its invalid-settings preservation diagnostic is intentionally injected.

The mapped fixture supplies a real native map, respawn logic and US spawn point; initializes the native US faction as playable on every peer; assigns player faction through SetFaction_S; registers missing native scoring entries in the no-backend clients; and closes the initial editor/welcome flow through normal native APIs. These satisfy the native deploy screen's own prerequisites rather than overriding its menu handler or spawn permissions. Early failed fixtures identified missing scoring/faction initialization, a non-playable faction and a test death helper that did not bump its replicated permitted-death flag. None are accepted deploy gates. Direct session-admin grant in the plain fixture also exposed an upstream recursive editor role-notification exception; native role calls are now ordered with the Game Master flag initialized before full-mode addition and cleared before removal. Production authorization is unchanged, and the runner still fails on script/RPL/VM errors.

The runner is `Tools/Validate-RespawnNetwork.ps1`, optionally `-DeployMenu`. It creates an isolated diagnostic addon, starts exactly its owned native dedicated/client processes, records source/runner hashes, checks peer stages and stops those exact processes. Final combined-source transport/clinical gates, backend audit/reconnect, spectator integration, populated pagination, saved-setting restart/live GM and human keyboard/gamepad/visual acceptance remain open. BCM Medical remains the only release target; legacy BCM-BodyBags retirement follows integrated release validation and preservation.

Latest plain-world connected regression on the current owner-status/deploy source: .checks/respawn-network-20261029-000538-102, 29 authority checks, zero failures, SourceUnchanged=true. It retains the Game Master-role-only denial case omitted by the authored full-editor deploy fixture. Combined with the mapped 34-check run, this covers the current seven-field private status payload, both native authorization paths and revoked stale review. This does not establish backend-audited identity or reconnect.


## Optional spectator integration — 29 October 2026

The saved `spectatorWhileWaitingEnabled` control is disabled by default. The catalogue is now 259 controls in schema 29; schema 28 retains exactly its original 258 keys. Server records capture the deceased native character's faction once, alongside immutable original replication ID/deployment life/death token. Query/entry authorization derives the caller from the exact owning controller and requires the current blocked dead deployment, valid original faction and enabled policy. Stale tokens and living controllers are denied. Separate query/session nonces prevent a delayed grant from reopening an exited session. Revalidation runs every five seconds with a client expiry deadline.

The native deploy button offers Spectate only after an owner-specific authorization reply. The client-only camera derives from native ManualCameraSpectate with explicit native movement, speed, rotation and terrain-collision components. Attachment, orbit, teleport, save/load and editor-only components are disabled. It starts above the controlled corpse with an upright horizon. Native movement/rotation/speed controls are retained; ManualCameraToggleUI toggles the bottom-centre countdown/input hints, while the separate persistent deploy status is unaffected. The request action defaults to keyboard R/gamepad Y and is listed under BCM Medical for rebinding. The native automatic deploy handler is suspended only while the local camera session is active, then resumes on exit.

Opponent character hierarchies are filtered locally using the original death faction; friendly/allied characters remain visible. Player discovery uses PlayerManager and AI discovery uses a camera-centred dynamic query. Visibility snapshots track individual nodes changed by this session, preserving originally hidden nodes and guarding Medical/native vehicle containment. Native SocialComponent mutes added by the session are tracked separately from pre-existing mutes and restored on exit. No editor role or access is granted.

Accepted first camera/lease/menu lifecycle: `.checks/respawn-network-20261029-001746-457`, 44 authority checks, zero failures, SourceUnchanged=true. Strengthened `.checks/respawn-network-20261029-003107-295` passes 46 authority checks, zero failures, SourceUnchanged=true, covering actual native Spectate button entry, retained camera across renewal, HUD toggle handler, no role/gate changes, local opposing player/AI exclusion, friendly visibility, unchanged authority visibility, native SocialComponent muted readback, native movement inputs (60 enabled frames and over five metres of displacement), local exit/visibility/mute restoration, resumed deploy menu and rejected stale grant. The existing owner request/admin review/disposal/allowance regression also completes in this run.

These tests use diagnostic account bindings on two real peers. They do not establish backend authentication, live microphone audibility or human keyboard/gamepad acceptance. Pre-existing mute/hidden-node corner cases, active revocation/disposal/lease-loss exits, rotation/collision behavior and final combined-source regressions remain separate acceptance gates. Early movement fixtures lost BaseContainerProps metadata when overriding the native movement classes, causing native resource factories to omit them; those failed runs are not movement acceptance. The runner now treats world-resource errors as fatal, except exact unchanged native M151A2/BRDM2/BTR70 editor-catalog property diagnostics bounded by their native resources and byte offsets. All unknown classes and Medical resource errors remain fatal.

Spectator restoration follow-up: `.checks/respawn-network-20261029-003605-706` passed 46 authority checks, zero failures, SourceUnchanged=true, with an actual pre-existing native mute (`voiceMuted=1 priorMute=1`) preserved through exit. The ordinary mute case records `priorMute=0` and restores that state. The runner now explicitly requires the prior-mute marker when `-SpectatorPreMuted` is selected. `.checks/20261029-003742-393` passes 642 native assertions, zero failures, adding real entity hierarchy snapshots: an originally hidden intermediate node remains hidden after root/descendant restoration, and an actually transferred folded kit remains hidden inside native Medical bag inventory. Schema-28 migration, schema-29 opt-in reload and existing request/disposition/life assertions also pass. The native invalid-settings preservation diagnostic remains intentional.

Current source therefore has an implemented and partially accepted optional spectator path. Active revocation/authorization-loss/disposal exits, original-faction mutation cases, full rotation/terrain-collision behavior, populated review pagination, backend audit/reconnect, saved-control process restart/live GM, human input/audio/UI, final cargo/flight/clinical regressions and release packaging/legacy removal remain unfinished. No complete-mod or full spectator acceptance claim is made.

Spectator interruption/control evidence, 29 October: `.checks/respawn-network-20261029-004245-899` passes 48 checks for active authority policy revocation; `004347-465` passes 47 for deliberately withheld renewal replies and actual native-clock lease expiry; `004614-329` passes 23 for actual current corpse deletion and matching wait release. All report zero failures and SourceUnchanged=true. Visibility, native mute state and deploy presentation restore; allowance and native roles remain unchanged. The disposed case intentionally ends after disposal because that death can no longer submit another waiting request.

`.checks/respawn-network-20261029-005303-727` passes 48 checks, zero failures, SourceUnchanged=true. Native movement and rotate components process normal-range diagnostic inputs; measured yaw/pitch change is 54.9189/18.3064 degrees. A subsequent downward input reaches native terrain collision with twelve sampled contacts and minimum clearance 0.199997 metres. These are spectator camera control tests, not an additional aircraft flight gate or physical keyboard/gamepad acceptance. The fixture measures orientation using the same matrix-to-angle convention as the native controller. Original-faction mutation and camera replacement still require explicit acceptance, followed by the remaining release gates above.

Original-faction acceptance: `.checks/respawn-network-20261029-005647-714` passes 49 checks with zero failures and SourceUnchanged=true. The fixture changes the actual dead native character affiliation from US to USSR, calls production registry re-observation and verifies the same death token/record retains US. Subsequent native spectator entry, US friendly visibility, USSR player/AI exclusion, lease renewal, rotation, terrain collision, restoration and owner/admin request chain pass. This proves corpse-affiliation mutation cannot change the saved death faction; backend identity/reconnect remains separate.

Native camera ownership interruption: `.checks/respawn-network-20261029-005809-877` passes 48 checks, zero failures, SourceUnchanged=true. The fixture creates/selects a separate native manual camera, invokes the production session tick and confirms the old spectator camera is deleted while the replacement remains selected. The diagnostic then disposes its replacement and opens deploy normally. Opponent visibility/native mute restoration, stale grant rejection, preserved physical wait and complete request/admin chain pass. This specifically validates the production ownership-loss branch; it does not simulate physical UI input or claim arbitrary external camera integrations have been tested.

Integrated-source cargo regression `.checks/bodybag-compatibility-20261029-005905-960/results.json` passes all eight fixtures (88 authority assertions total), with owner and late-join cargo phases plus carrier ownership confirmed for every vehicle. All 1,664 recorded production-source comparisons still match the current files. The HH-60G uses its existing explicitly bounded upstream supplies diagnostic policy. This is cargo acceptance; the separate five-aircraft piloted regression is running.

Legacy deletion preparation: `.checks/bodybags-handoff-20261028-181152/current-source-preservation-20261029.json` records 170 current source/asset/configuration files verified byte-for-byte against preserved ZIP entries. The supplemental Assets/Configs archive preserves 56 files (132,535,049 uncompressed bytes), including editable meshes and compiled resources omitted by the initial implementation archive. A small delta archive preserves the remaining current `.gitignore`. Only native resource databases and the legacy `.workbench-validation` diagnostic/cache tree are excluded. The legacy folder has not been deleted; integrated release validation remains a prerequisite.

Integrated-source piloted aircraft regression `.checks/bodybag-compatibility-20261029-010501-080/results.json` passes all five aircraft: Mi-8, Medical Little Bird, BCM UH-60, BCM HH-60G and BCM Chinook. All 65 authority assertions pass; exact native pilot ownership is confirmed for every aircraft, with owner/late-join cargo phases and landing/unload/equipment retention checks. The 1,040 recorded production-source comparisons match the current files. The HH-60G retains its documented upstream supplies diagnostic policy. This complements the eight-fixture cargo matrix above; it does not establish arbitrary third-party aircraft compatibility or Internet latency acceptance.

Separate-process schema-29 acceptance: `.checks/persistence-current-20261029-011604-932` passes 581 assertions in the seed dedicated process and 602 in the restarted process, zero failures. Nondefault burial/home/disposition policy, timeout 1234, disabled respawn requests, cooldown 777 and enabled spectator policy survive JSON storage and reach the actual restarted game mode. Earlier clinical/mask/arsenal settings remain covered by the same restart suite. The invalid-settings preservation diagnostic is deliberately injected by the domain tests; this is not a zero-global-log-errors claim. Native GM widget interaction remains separate.

Native US medical tent acceptance: `.checks/20261029-011807-287` passes 653 assertions, zero failures. The exact native prefab `{F8427C3EFC52644E}Prefabs/Structures/Military/Camps/TentUS_01/TentUSMedical_01.et` supplies a five-metre healing area at its authored one-metre Z offset. The actual original-equipment bagging chain, outside/inside range, disabled station, live return-home policy, distant performer refusal, exact bag/item disposal, SENT_HOME token, matching account release and unchanged allowance pass. Normal replicated fixture cleanup also completes. The known plain-test-world null loading-layout resource diagnostic and deliberately injected invalid-settings preservation diagnostic are unchanged; no zero-global-error claim is made.

The native USSR tent run `.checks/20261029-011857-566` and FIA tent run `.checks/20261029-011937-492` each pass 653 assertions, zero failures, completing the same original-equipment bagging/area/policy/disposal/account checks and normal fixture cleanup. Their native ranges are 100 metres (USSR) and four metres (FIA); production code retains each station's authored geometry rather than imposing the M997 or US tent radius. Together with the five-metre US tent acceptance and existing M997 boundary tests, these cover three stationary faction facilities and the ambulance. This does not claim every modded medical facility is tested.

Populated connected roster/native UI acceptance: `.checks/respawn-network-20261029-012022-229` passes 47 authority checks, zero failures, SourceUnchanged=true. Sixty-five separately committed/killed native casualties produce distinct diagnostic-account requests. Reliable owner-only pages contain 32, 32 and one row, with 65 unique death tokens, no duplication and zero rows on the overflow page. All Waits returns the matching population. The actual native panel renders 32/32/1 rows; registered Next, Previous and All Waits button callbacks navigate/reset pages with the expected enabled states. Native screenshots of the first and last pages were inspected: the populated list scrolls within its panel and the final single-row page disables Next. Existing owner privacy, role revocation, stale selection, physical preservation and independent allowance regression also passes. This is local native diagnostic population/load coverage, not real backend identity or Internet player-load acceptance.

29 October user instruction: removed the independent body-bag respawn restriction and menu. Limited Lives remains the sole BCM respawn policy. Removed wait/timeout coupling, request/review RPCs and cooldown, deploy UI override/countdown, Body Bag Admin/pause entry and keybinding, and optional waiting-spectator camera/HUD/input. Physical recovery remains separate. Current catalogue: 254 settings/schema 30; old schemas validate and discard the five retired keys. Native compilation passes with 336 resources. Limited Lives removal regression 20261029-061354-705 passes 573 checks. Current schema-30 separate-process restart persistence-current-20261029-061721-105 passes 555 seed/574 verify checks. A real copied schema-29 profile with formerly enabled spectator loads in legacy-settings-removal-20261029-061824-427 and passes 575 native Limited Lives/model/migration checks, including native menu-preset and keybinding absence. Retired tests/source are archived in .checks/removed-bodybag-respawn-20261029-061118-937. Pre-removal menu/spectator evidence is historical, not current functionality.

Current physical recovery regression `.checks/20261029-061855-080` passes 627 assertions, zero failures: original equipment is bagged and disposed at the native US medical tent while Limited Lives eligibility is already available before Send Body Home and remains unchanged afterward. The body-bag respawn system is absent. A verified Git bundle also preserves all nine legacy refs and complete history before eventual legacy-folder retirement; its path and hash are recorded in DEVELOPMENT-PAUSE.json. The post-removal unchanged-source clinical regression is running.

Post-removal integrated clinical regression `.checks/clinical-acceptance-20261029-061952-047/report.json` passes all nine gates with sourceUnchanged=true. Legacy add-on retirement then removed its source/assets/configuration/Git contents after verifying the 170-file preservation and complete Git bundle. Windows refused removal of the final directory because another process holds it open; the directory is empty and contains no addon.gproj, so it is no longer a loadable add-on. BCM Medical remains intact. This empty shell is not a runtime dependency or active development project.

Progressive native full/cancel/resume acceptance `.checks/20261029-072521-880` passes 606 assertions, zero failures, with unchanged captured source verified in source-after-audit.json. Saline/plasma/whole blood each deliver during all three full default sixty-second procedures. The fourth saline bag is interrupted after partial delivery, retains 436.515 mL in the exact item, stops in the background, resumes with a proportional remaining duration and is consumed once on exhaustion. Final reserve is 0.591168 after redistribution, with no red-cell creation or refill of the partial bag. Current access-loss/native-medication safety .checks/20261029-072411-642 passes 575 assertions. The current source adds a millilitre-returning native adapter tied to the same clinical settings used by delivery and labels the next bag's remaining contents in the Fluids actions. Connected partial contents and UI acceptance are in progress.

Connected progressive fluids `.checks/fluid-network-20261029-073220-460/result.json` passes 569 authority checks, sourceUnchanged=true. All three clients observe intermediate and final physiology for all three types; the actual owner receives decreasing finite contents (444.510/449.787/449.982 mL during the respective trials). Exact owner RPCs and all five existing final phases still pass. Native partial-bag UI `.checks/20261029-074413-637` passes eight checks and its 720p capture was inspected. A live capacity change creates new 1000-mL bags while preserving the original bag's 500-mL capacity and 375-mL remaining contents. The separate widget-walker fixture timeouts in 073642/073958 are rejected; the corrected bounded traversal produces clean native UI completion in 074151 and 074413.

2026-10-29 finite arsenal refunds: .checks/20261029-080048-505, 598 assertions / 0 failures. Native supply and separate personal allocation calculators agree for partially depleted saline, oxygen and AED; live capacity changes preserve existing item value. Owning-client financial transaction acceptance remains open.

Native active-treatment refund reservation: .checks/20261029-080904-846 passed 579/0, including actual saline, enclosing native storage, unrelated item and release after cancellation. Parent links alone were insufficient; guard now traverses native storage with a visited set.

Native actual arsenal transactions: .checks/20261029-081611-314, 606/0. Real public purchase/refund RPC paths, finite supplies 1000 -> 880 -> 925, exact inventory creation/removal, disabled purchase and duplicate stale refund rejection. .checks/20261029-081457-429 registered the new test-only prefab resource and passed script validation. Prior failed transaction runs had an unregistered diagnostic prefab GUID and are not acceptance evidence.

Dedicated native transaction diagnostic .checks/arsenal-network-20261029-081823-385: server 606/0 and all four genuine owning-client requests processed; whole gate failed because observer read the unreplicated raw container. Subsequent observer work uses the native consumer total/subscription path; this diagnostic is not overall connected acceptance.

Dedicated arsenal purchase/refund: .checks/arsenal-network-20261029-082231-072/result.json Passed=true, SourceUnchanged=true, authority 606/0. Actual native owner purchase/partial refund/disabled purchase/stale duplicate requests; native subscription totals observed on owner, proxy and late joiner.

Real native active-treatment refund RPC guard: .checks/20261029-082344-613, 581/0. Both direct saline and enclosing native backpack refund RPCs log BCMM_ARSENAL_REFUND_DENIED, preserve the active procedure and exact items, and leave arsenal supplies at1000. Cancellation releases reservations and preserves the partial bag.

Combined current clinical acceptance: .checks/clinical-acceptance-20261029-082439-133/report.json, all ten gates passed, sourceUnchanged=true. Includes actual owning-client arsenal transactions plus resources, local arsenals, clinical safety, oxygen/medication, progressive fluids, applied equipment, native bleeding, settings restart and medical UI. Capture now includes all diagnostic prefabs and validation tools. Later finite-item persistence changes require new focused evidence.

Finite-supply native Save/RequestLoad acceptance .checks/20261029-084119-225 passes569/0 for depleted saline, oxygen and AED. Native JSON includes original capacities/remaining state and callback reload restores it after further depletion. Earlier local direct DeserializeSpawn/DeserializeLoad experiments crashed the client before BCM's restore callback; these are rejected diagnostics. Real server stored-record spawn and separate-process restart remain under development.

Native finite-equipment saved-world restart: .checks/finite-persistence-20261029-084753-382/result.json Passed=true, SourceUnchanged=true; seed595/0 and separate-process verify559/0. Real SaveGameManager save UUID3e258fa5-c964-4b16-a75b-7a8a3f75459f loaded by -loadSessionSave. Original native persistent item UUIDs survive: saline500/375mL, oxygen300/299.833L, AED20/19charges. Before saving, native stored-record spawn already preserved each original capacity after doubling GM defaults. Invalid versions/kinds/negative/over-capacity/fractional AED states rejected without mutation. Corrupt component-load policy reports native ERROR and removes the invalid finite item. Earlier084654 runner had a reporting-line parse defect;595/0+559/0 ran, but its result flags are null and are superseded by084753.

2026-10-29 resumed Limited Lives persistence: .checks/finite-persistence-20261029-132424-457/result.json Passed=true, SourceUnchanged=true,600 seed/574 verify,0failures. Native world save preserves surviving/exhausted/pending accounts; reused/new connection IDs retain account ownership; native NOT_FOUND body recovery preserves paid usage, delayed callbacks cannot retire newer lives, duplicate identities reject atomically, and unauthorized reset cannot bypass rejected-state lock. Identities are explicitly injected diagnostic UUIDs, not proof of live backend authentication. Standalone authority and active-source safety regressions also passed in the same sequential run.

Limited Lives offline native death handling added through OnControllableDestroyed, deferred until actual native DEAD state. .checks/finite-persistence-20261029-133940-633 passes600/574 native restart checks and .checks/20261029-134000-131 passes575 authority checks. Deaths no longer depend on an online connection mapping or physical corpse deletion; body disposition remains independent. The earlier combined132623 run failed one oxygen reconnection cancellation; instrumented dedicated133439 rerun passed571/0 and all owner/proxy/JIP phases. Intermittent cancellation remains under observation in the next combined run.

Focused native oxygen source stress .checks/oxygen-source-stress-20261029-135056-560/result.json passes619/0 with source unchanged: ten genuine owner reconnections select/deplete each fuller cylinder while preserving all previous disconnected contents. Full-sequence134241 still rejected because reach became false mid-second connection despite zero actor drift, correct held source and valid clinical eligibility; additional trace/collision/state diagnostics are now installed. No production reach safeguard has been weakened.

29 October patient persistence: `.checks/finite-persistence-20261029-141138-357/result.json` passes604 seed/586 separate-process world-restart checks, zero failures, SourceUnchanged=true. Native blood and clinical injury/drug/equipment/timer state restore together. Earlier140447/140542/140715/140859/140932 attempts are failed diagnostic runs and are superseded for the healthy DEFAULT-health case. Offline native death confirmation now waits100ms for controller finalization; prior zero-delay event handling intermittently preceded native DEAD state. Full clinical suite and connected patient/source persistence remain unaccepted pending their own checks.

Current focused native world restart `.checks/finite-persistence-20261029-141545-028/result.json`:611 seed/588 verify assertions, both0failures, SourceUnchanged=true. Adds actual saved oxygen source in native patient inventory, actor/device persistent UUID reattachment and continued partial-gas consumption; malformed nested patient wound/pooled-bleed/fracture data rejected by real native JSON load. `.checks/finite-persistence-20261029-141439-448`607/588 proves binding lifetime correction;141252/141352 are failed diagnostic runs. Full18-gate unchanged-build regression started at `.checks/clinical-acceptance-20261029-141632-030`; result still pending when this note was written.

Eighteen-gate attempt `.checks/clinical-acceptance-20261029-141632-030/report.json`: first12 core gates passed, but the combined run failed at full-oxygen-medication. `.checks/20261029-142535-796` reports awake oral target eligible but CanReach=false before the request. This run is not full-suite acceptance. Test-only distance/trace/controller-state instrumentation was added at that start point; focused reproduction is running. No production treatment reach/range rules were relaxed.

Physical-world persistence `.checks/finite-persistence-20261029-143820-339/result.json` passes615 seed/593 verify,0failures, unchanged: named physical bag, immutable remains metadata, original partial350mL saline item and native recovery-only inventory insertion lock after load. The test first reproduced metadata/gear loss at143517, then gear-only loss at143726; a synchronous native recovery-storage serializer scope fixes insertion without leaving gameplay storage open.

Expanded grave/stationary-world restart `.checks/finite-persistence-20261029-144020-016/result.json` passes617/596,0failures, unchanged. Native physical grave and disposed remains metadata survive. Original unconscious patient UUID rebinds to a stationary stretcher; no carrying state resumes, and deleting the restored stretcher leaves the patient alive. Stationary association and memorial fixtures are explicit diagnostic seeding, separate from previously tested interactive placement/burial actions.

The intermittent full oral-treatment reach failure is still open. Instrumented full oxygen/medication repeats `.checks/20261029-142903-247` and `.checks/20261029-143204-498` each pass583/0, with clear1.17m/1.25m traces at the tablet start. They do not explain the earlier failed start or intermittent network phase-eight cancellation.

Vehicle cargo persistence root cause resolved: type-discriminator scope around native vehicle storage deserialization. Normal production binary restart151708 passes624/601; after native prefab automatic-registration changes152208 passes623/601. No diagnostic backend/configuration override remains under Tests/Runtime. Ordinary native inventory flags/rules and all other vehicle serializers retain their native behavior.

Production native vehicle saved-world variants, each623seed/601restart,0failures, SourceUnchanged=true: Jeep `.checks/finite-persistence-20261029-152340-598`, Helicopter152445-584, Ambulance152506-171, Truck152526-203. Post-restart native unload/reload/destruction/deletion on Jeep `.checks/finite-persistence-20261029-152830-883` passes623/609 and preserves exact gear lifetime, dispositions, independent remains and paid-life balance. A further saved-world restart after disposition is running; do not count it accepted until its own result exists.

Three-process Jeep saved-world lifecycle `.checks/finite-persistence-20261029-153104-289/result.json`:623seed/612restored-lifecycle/563post-disposition-restart,0failures, SourceUnchanged=true. Original wreck, physical cargo bag and deposited AED remain deleted on the new save; VEHICLE_LOST, unrelated remains, independent grave/deleted stretcher state and surviving paid-life balance persist. This is production binary storage with no test backend or native serializer override.

Full18 clinical regression .checks/clinical-acceptance-20261029-155841-589/report.json passed all18 gates with sourceUnchanged=true at16:26 UTC29October. Full oxygen/medication583/0, typed fluids606/0 and field procedures577/0 are included. Earlier intermittent reach failures remain preserved and unexplained; this pass does not prove their cause fixed. Diagnostic saved-world network/fault additions were applied only after this accepted source freeze ended.


Saved-world connected run `.checks/world-persistence-network-20261029-162648-998/result.json` FAILED with SourceUnchanged=true. Native seed623/0 passed; original restored clinical flags107, partial oxygen and native cargo were observed by both initial clients before phase2. A restored depleted patient then triggered recursive native Kill -> OnDamage -> UpdateConsciousness -> Kill at grace expiry (stack overflow), so JIP acceptance was not completed. This is a production defect, not a passed network case. Native consciousness guard now remains held across synchronous native death callbacks and already-dead controllers return early. TestResources compile162919-094 passes341resources; connected native expiry regression is running. The earlier full18 passing report predates this production correction.

Post-fix connected restart `.checks/world-persistence-network-20261029-163505-666/result.json` passes623seed/558server, zero failures, SourceUnchanged=true. Actual owner and proxy observe phases1/2; late join observes phase2. Restored finite oxygen falls299.042->296.376 in the earlier accepted163200 owner trace; current run separately passes the same finite-content gate. Restored depletion expiry and independently terminal native physiology reach DEAD without recursive death callbacks. The diagnostic cargo floor and actual owning-character teleport provide normal native cargo access in the empty test world; no production access rule changes.

Current three-process Jeep lifecycle `.checks/finite-persistence-20261029-163606-770/result.json` passes623/612/563 with SourceUnchanged=true after both production fixes. Direct malformed/unavailable-reference native regression in world-persistence-network163200 logs16-34-29 passes566/0; the preserved16-33-14 baseline fails566/1 at registry capture before the missing-reference correction. Actual save after availability timeout plus another restart is now running separately. The combined full regression runner now includes saved-world connected and missing-reference restart gates; with FullProcedures it has20 gates.

Expanded20-gate attempt `.checks/clinical-acceptance-20261029-163909-556/report.json` FAILED in oxygen-medication-network phase8. `.checks/oxygen-medication-network-20261029-164101-396` captured exact cancellation evidence:actor drift0, native item using/held, eligible1, clear tracefraction1, no falling/swimming/unconscious actor, but actor-to-casualty distance2.04861m exceeded configured2m as casualty became unconscious. Authority correctly cancelled without consuming the source. The success fixture now approaches each other-patient procedure before requesting, then leaves movement/range checks active throughout. No production range or safety change. Separate native casualty-only displacement beyond2m now cancels oxygen, preserves the exact full source and applies no oxygen; clinical safety164611-962 passes587/0. Full oral-readiness historical failure remains separately unproven; do not close it by analogy alone.

Corrected connected oxygen/medication `.checks/oxygen-medication-network-20261029-164641-044` passes571/0 with owner/proxy/JIP required phases and unchanged production access rules. New native negative casualty-only range loss is included in safety164611-962,587/0. Expanded20 rerun `.checks/clinical-acceptance-20261029-165632-741` is active after those focused passes.

Raw-backend diagnostic `.checks/authenticated-reconnect-development/addon` compiles165047-610 and165407-895. Runtime165106 rejects -config combined with -addons;165132/165228/165441 lose local addon context before mission header load;165516 direct-world start bypasses backend/configured bind and was stopped. Its original requested loopback metadata is explicitly corrected by ObservedLoopbackOnly=false and BackendHostingVerified=false. All are failed/unaccepted attempts, not proof of authenticated reconnect. Configuration/metadata were corrected locally; regular matching1.8.0.13 binaries remain another investigation path. No publication performed.

Full oral-readiness failure is now diagnosed independently: expanded20 run165632-741 passed all14 core gates then failed full-oxygen-medication in170748-592. Trace shows awake/otherwise-eligible actors, clear fraction1 and distance2.07984m beyond2m. Success fixture now approaches once after native recovery before starting the new oral procedure. Native full171149-714 passes583/0: captured pre-approach distance2.03621, then valid1.00105m and completion. No production range/trace/eligibility change. Both previous intermittent range failures now have captured causes, separate from their corrected success-case setup.

Native Limited Lives administration implemented in the existing GM context list.171139-848 passes582/0 for pending-reset cleanup/stale finalization/fresh commit and active body/native partial blood/remains preservation. Connected172042-308 passes566/0 for actual owning GM context/RPC grant/remove/reset and private target status. Extended `.checks/life-admin-network-20261029-172512-561/result.json` passes578connected/557native restart, SourceUnchanged=true: grants/removal/reset/regrant, actual Edit-mode revocation, held-authority rejection and exact saved ledger/paid-body UUID/partial blood restoration. Uses local diagnostic account IDs, not backend authentication. No separate menu or settings/schema addition. First171929 failure preserved: diagnostic modded action needed BaseContainerProps attributes for configuration reflection. Native context merge retains both role actions, all native controls and three allowance actions.

Current all-platform TestResources172430-255 passes341resources. Combined FullProcedures runner now has21 gates including native GM allowance/restart. `.checks/clinical-acceptance-20261029-172748-736` is active. Regular-binary raw-backend probe172706-262 also fails before local mission-header resolution; no authentication/reconnect pass is claimed.

29 October 2026, oxygen removal: a connected supply can now be disconnected without the removing medic carrying a spare cylinder. The native medical menu enables the existing removal action from the replicated connection state. Removal retains its saved duration, authority validation, reach and actor/patient locks; cancellation leaves the supply connected. The operation captures the exact oxygen session so reconnecting the same cylinder invalidates an old pending removal. No replacement device is created. Native actual menu/RPC, timer, movement cancellation, same-device reconnection and independent concurrent removals passed608 checks with zero failures in .checks/20261029-194450-572. This focused local runtime does not by itself certify dedicated owner/proxy/JIP removal.

The schema-31 component-recovery build passed all17 core gates unchanged in .checks/clinical-acceptance-20261029-192149-567/report.json. Subsequent oxygen-removal changes are undergoing a new18-core/24-full acceptance run; earlier evidence remains specific to its recorded source snapshot.

29 October 2026,20:18 UTC: all24 Clinical Acceptance gates passed with sourceUnchanged=true in .checks/clinical-acceptance-20261029-194537-299/report.json. This schema31/260-control production build includes inventory-free timed oxygen removal, component recovery and the native stationary-stretcher catalogue. The run covers18 core gates plus full-duration oxygen/medication, typed fluids (including cancellation/resume), field procedures, equipment, native medication and TXA. Later multiplayer-removal fixture additions and independent-line candidates have separate acceptance boundaries.

Dedicated cylinder-free oxygen removal is accepted in .checks/oxygen-removal-network-20261029-201924-695/result.json:Passed=true,SourceUnchanged=true,599 authority assertions and zero failures. Actual native character possession, owning-client enabled menu/RPC with zero cylinders, intermediate saved removal timer and original carrier device retention are verified. Owner/proxy/active-session JIP each observe the real oxygen mask present then removed. The earlier201845 fixture compile failure was corrected with explicit declaration layout; it is not a production regression claim.

Independent drips are now integrated as a development feature with263 settings/schema32. Native base/schema608/0 and actual inventory/flow630/0 pass202118-785 and202212-006. The first native connection fixture correctly saw native backpack storage registration lag immediately after equipping; the accepted fixture waits for registration before starting. Save/restart, owner/proxy/JIP and broad unchanged-build regression remain pending. The .checks/continuous-line-development preparation candidates predate live fixes and must not be reapplied.

Native line boundaries now pass637/0 in202831-504 (valid live pressure stop, full reserve and genuinely exhausted source) and625/0 in203346-656 (simultaneous typed saline/plasma, distinct actor/patient removals, stale same-source reconnect protection and actual cross-character source movement). The first pressure fixture used80 below the saved minimum90; it correctly failed validation and was corrected to90 with a real higher native pressure. No production settings bound was relaxed.

Diagnostic unresolved-source pending-save/restart/timeout passed615/612 unchanged in missing-fluid-line-20261029-203046-183. A subsequent available-source callback/FullHeal extension and whitespace-only cleanup are undergoing fresh native acceptance. The unresolved UUID is diagnostic; this does not certify arbitrary missing third-party prefab packages.

29 October2026,21:16 UTC: all30 Clinical Acceptance gates passed with sourceUnchanged=true in .checks/clinical-acceptance-20261029-203856-546/report.json. This is the263-control/schema32 independent-drip build, including real native flow/boundaries, owner/proxy/JIP, active-line save/restart, diagnostic missing-source save/restart/timeout, available-source callback cancellation, GM catalogue/admin, physical recovery/lives persistence and full-duration clinical procedures.

Subsequent current development adds an optional redistributed-fluid burden model:270 controls/schema33, disabled by default, separate saved state, linked lung effects, active clearance and kind-specific water-loading guards. Base/schema/deterministic634/0 passed211726-007. Native generation/save651/0 passed with tracing; restart accounting is being investigated. Do not treat the new overload work as accepted or label the prior30-gate snapshot as its validation. Historical staging files must not overwrite live fixes.

29 October2026, focused270/schema33 acceptance: actual fluid-burden native world save/restart passed651/642 unchanged in .checks/fluid-burden-20261029-212421-106/result.json, including original finite whole-blood/oxygen sources and no offline clearance. Owner/proxy/active-session JIP passed640/0 unchanged in .checks/fluid-burden-network-20261029-213559-673/result.json; actual menu water-loading gates and counts update while open without manual refresh. Actual pre-burden native-world compatibility passed639/0 unchanged in .checks/fluid-burden-legacy-world-20261029-214022-749/result.json, with the original historical profile unchanged. This older profile had no settings file. These results supersede the preceding in-progress restart statement; retained failures document earlier fixtures, not an unresolved failure in these accepted focused runs.

Current full33-gate unchanged-source regression is .checks/clinical-acceptance-20261029-214430-980/report.json, started21:44. All14 completed gates passed through the burden world-restart check at21:58:20. The owner-network check is active. Whole-mod completion, raw backend reconnect, clean engine package, final art/licences and broader requirements remain open. No combined pass is claimed until the report completes with sourceUnchanged=true.

Historical full-duration lifetime evidence recovered from remains-lifetime-20261028-201531-888/result.json and its native console:15 checks, zero failures, fullDuration=1. Original corpse and bag survive the actual1798.43-second pre-expiry observation; corpse becomes collection-eligible at1802.93seconds and is physically removed, while the exact filled bag remains protected. This closes the historical pending-result statement above for that snapshot; it does not certify subsequent source changes without their own acceptance.

29 October2026,22:25:26 UTC: current270-setting/schema33 Clinical Acceptance passes all33 gates with sourceUnchanged=true in .checks/clinical-acceptance-20261029-214430-980/report.json. This includes all27 core gates, independent-line boundaries/network/restart/missing-source, opt-in burden world/network/old-world compatibility, GM administration/catalogue, component recovery, actual inventory/lives/physical-world persistence, UI and all six full-duration procedure groups. Full typed fluids687/0, field procedures658/0 and equipment692/0 passed. Negative invalid-settings fixtures intentionally exercise diagnostic errors; zero failed assertions does not mean every native log line is error-free.

Private isolated PC native packing then completed at22:25:33 with explicit Packaging project successful and process exit0. Candidate20261029-214847-473 produced one83,236,673-byte data.pak, addon.gproj and resourceDatabase.rdb. Read-only PAC1 inspection found422 entries and no flagged unsafe paths, private development directories or authoring archives. It does include the retained unused corpse texture/material/model chain, about16.9MiB of potential separate-copy exclusions. No source exclusion has been applied. Packed authority/renderer smoke is running; no native package runtime pass is claimed yet. No upload/publication occurred and licences/final art remain unresolved.

Private packed-output native smoke passes in candidate-20261029-214847-473/smoke-20261029-222616-070/result.json: authority634/0 and720p renderer/UI26/0. Native Available/Loaded addons identify Medical only under the isolated packed/addon.gproj, including the packed marker. Package hashes remain unchanged. This is a private PC/headless compatibility smoke with the separately copied diagnostic addon, not full clean-install, all-platform, multiplayer, licence or publication acceptance. Production code still matches the accepted270/schema33 snapshot.

After the33-gate source snapshot, a real stock-consumable commit defect was reproduced and fixed; see Docs/NATIVE-STOCK-CONSUMABLES.md. Native real-animation rejection/same-item reuse649/0, promoted native owner/proxy/post-revocation joiner643/0 unchanged225032-517, and supported local clinical-safety668/0 passed. Native passenger/finite-drip separate-world restoration647/641 passed unchanged in transport-fluid-line-20261029-223748-938/result.json. The full runner now has30 core/36 full gates; do not relabel the pre-fix33-gate result or old packed output as this source's combined/package acceptance.

29 October23:05 UTC, current commit-guard source: all five piloted aircraft passed13 checks each in bodybag-compatibility-20261029-225220-683/results.json (Mi-8, Medical Little Bird, UH-60, HH-60G, Chinook). HH-60G retains passed-with-upstream-diagnostic-policy. Actual carrier/pilot ownership and owner/late-join inspection pass. MRZR/JLTV/Cougar each pass11 in bodybag-compatibility-20261029-230304-641/results.json. These certify the exact installed fixtures/source snapshots, not arbitrary packs or human controller play.

Curated current-source PC packet candidate225341-700 packed successfully at23:05:18, explicit native success/exit0.419 entries;65,521,683-byte PAK versus pre-fix baseline83,236,673 bytes, about21% smaller. Four unused retained resources/eight source-copy files omitted; no production references removed. No omitted resource was recreated, no flagged development/authoring/unsafe entry found. Extended packed-only smoke230519-328 passes634 authority,26 UI,649 actual stock commit/reuse and701 actual four-family cargo renderer-runtime assertions; unchanged PAK hashes and loaded Medical origins exclusively inside packed output. Still private PC evidence, not licences/publication/all-platform/Internet/load/final art acceptance.

29 October23:49:23UTC: current270/schema33 full36-gate Clinical Acceptance passes with sourceUnchanged=true in clinical-acceptance-20261029-230712-443/report.json. All30 core and six full-duration groups pass, including current stock commit/owner networking and passenger/drip world restart. Full typed fluids687/0, field procedures658/0 and equipment692/0.

Subsequent isolated stock-saline probe run234924-977 is NOT accepted. Actual native role rejection retains the original item; policy-restored same source transfers to the patient's native slot and delivers volume without red cells. Native access-loss check then reaches cleanup, where DeleteEntityAndChildren(patient) causes a native access violation (crash.log points to SplintRuntimeChecks.Finish line247). Resource load also used an unqualified zero-GUID name; correcting canonical resource lookup before repeating/classifying. No production change based on this probe, and no crash-free saline result claimed. The separate36-gate source snapshot remains recorded as passed; its coverage did not include this exact original-stock/parent-deletion sequence.

30 October native original-stock saline: canonical resource lookup resolved {00E36F41CA310E2A}SalineBag_US_01.et. Both235440-184 and fixed-GUID235637-761 pass648 checks with normal patient cleanup, real rejection/retained source, same-source second use/native slot, volume gain without red cells and access-loss stop. Earlier zero-GUID run crashed during cleanup; no exact causality beyond the observed lookup correction/clean subsequent runs is asserted.

Actual native world run235913-507 saved650/0, restored original patient/source/slot but failed637/1 because the native delivery effect was absent. New optional bcmmNativeSaline state and original/restored binding code preserve source UUID, remaining native lifespan/effect time and DPS. Initial000634-151 passes650 seed/640 restart unchanged. Extended000902-302 fails at expiry: effect stopped but original source remained. Callqueue/world-deadline boundary correction and expiry trace are now undergoing fresh native acceptance. This later production code is outside the preceding36-gate source snapshot and curated package; no final expiry or new combined/package pass is claimed yet. Settings remain270/schema33; existing clinical schema1 is extended by an optional strict versioned object.

30 October00:17UTC: Native stock-saline saved-world expiry now passes650 seed/642 restart, SourceUnchanged=true, private world-run-20261030-001222-247. Preceding000902-302 failed physical-source expiry; early callqueue deadline rearm corrects it. A separate real source-movement probe001648-164 then reproduced continued delivery after physical removal (649/1); source-presence correction is under native test, not accepted yet. The36-gate and curated packet snapshots precede these latest production changes.

30 October00:20UTC: Combined native saline corrections passed unchanged: source movement649/0 in001723-037; access-loss648/0 in001801-887; world seed650/0 and restart642/0 including physical expiry in001823-294. These remain focused private-addon checks; a FullHeal/new-source boundary is now being exercised.

30 October00:35UTC: Promoted native stock saline active world658/650 unchanged (.checks/native-stock-saline-20261030-002924-035), inactive world659/650 unchanged (003156-355), and deferred-reference650/0 (003436-296) pass. Four runtime/codecs files plus world runner are promoted. Full42-gate unchanged-source regression003550-595 now active; no combined result yet. Private third fresh-bag animation staging remains unaccepted; FullHeal fresh-source treatment uses the real native commit path.

30 October01:24UTC: Full42-gate clinical-acceptance-20261030-003550-595 PASSED with sourceUnchanged=true, completed01:24:07.661UTC. Native curated candidate20261030-003816-344 packAddon also completed explicitly, exit0:421 entries,65,524,547-byte PAK, SHA25637344FA7D88DF42D67024AFC67AF0EF40769FADDFA6F863AF555A8952258A601. Inventory findings0. Seven packed-only phases now running; no final packed runtime result yet. Retrieved-source expiry and actual optional-scalar corruption remain prepared follow-ups, not accepted behavior.

30 October01:28UTC: Candidate003816-344 packed-only smoke012416-602 PASSES all seven phases:642/26/657/656/657/661/709 assertions, zero failures; packagesUnchanged=true and strict packed origin throughout. The next independent retrieved-used-bag world012616-959 reproduces a remaining defect:seed662/0, restart646/1 at retained expiry, SourceUnchanged=true. Original source survives world restart beyond its original native deletion deadline. This narrower failure does not invalidate the42 covered gates, and prevents claiming complete stock lifetime coverage.

30 October01:39UTC: Present scalar optional values are reproduced through actual registered world save/load:baselines013240-997(saline),013343-313(line),013359-651(burden) each647 seed/644 restart with one restart failure, unchanged sources. Four production guards now use DoesKeyExist for bcmmClinical and the three optional states, preserving absent legacy data. Corrected worlds013630-547/013647-364/013704-082 each647/644 zero failures, unchanged. Earlier direct DeserializeLoad experiment012852-309 crashed at the direct API call; it is a rejected harness approach, not evidence of scalar-field causation. Native normal world loader replaced it.

01:41UTC: Isolated source-timer candidate014004-650 saves662/0 but fails original source/actor restoration before expiry (644/1). Do not promote. Added lifetime save/load/expiry traces and explicit restored actor/patient/source diagnostics;74960 runs the next private reproduction. Whole Medical remains incomplete.

30 October01:54UTC CORRECTION: Initial013630/013647/013704 scalar-case passes were insufficient: they lacked a healthy control. Native traces014610-635 establish .bin, CanSeekMembers=0, DoesKeyExist(absent)=1, DoesObjectExist(absent)=0. The blanket key check deleted healthy characters too. It has been replaced with BCMM_PatientSaveValidation.HasState: key detection only for seekable contexts, named-object detection otherwise. An unlabelled binary scalar is not a named-field corruption case. New genuinely named invalid-schema objects plus healthy controls pass647 seed/645 restart unchanged for saline015043-526, line015100-377, burden015117-110. Earlier direct DeserializeLoad crash and malformed test scope remain historical rejected approaches. Do not cite the initial scalar cases as accepted strict binary-field behavior.

30 October02:46UTC: Production timer/config promotion022929-456 passes666/648 with FullHeal and unused-bag controls; same-source native commit022903-777 passes. Source clinical binding now adopts older attached-source lifetimes into an independent source timer, retaining an earlier deadline, and delivery checks the independent expiry. Standard same-source024519-131 passes661/0 after safe JSON primitive guards. JSON scalar object/field-reader probes023314-975 and023856-874 crashed in native ReadValue; later024225-787 establishes object/key entry alone is not structural validation. Native primitive probes returnfalse on real objects and preserve context validity. Temporary discovery code removed; valid-object and scalar-rejection contracts are in the main codec checks. The current promoted lifecycle batch is running; full48/current packed pass remains pending.

30 October03:10UTC: promoted seven-scope batch stock-lifecycle-promoted-20261030-024806-279 passes unchanged. Retrieved expiry and FullHeal/unused control each668 seed/650 restart; three invalid-schema named-state worlds649/647 each with healthy controls; active/inactive attached worlds661/652 each. Historical pre-item-timer saved world legacy-run030302-331 passes647/0 unchanged, original attached source adopts remaining clinical lifetime, native retrieval stops flow and original physical source expires. Initial legacy025902-787 compilation failure was private dispatcher64-local limit, corrected by reusing an existing CLI temporary.

Private settings allocation run030457-495 passes645/0 unchanged:200 reads28ms fresh/8ms reused, exact policy sums and immediate actual GM toggle/restore. Production is unchanged; no multiplayer performance acceptance claimed. AI discovery030511-555 and030559-668 failed canonical resource enumeration; recursive search plus native $ArmaReforger: root produces030625-630,643/0 unchanged. Actual SCR_AIMedicHealBehavior execution030746-904 passes646/0: genuine bleeding and original native dressing, no fixture healing call. Extended030903-592 passes with no-supply persistence, subsequent supply and linked BCM wound Bandaged assertion. These are explicit native behavior requests, not autonomous casualty detection/triage or broader clinical AI. Full48 combined clinical-acceptance-20261030-030939-332 is now running, shell44377; source frozen, no latest combined/package claim yet.

30 October04:03:42.396UTC: current source-lifetime/format/reapplication snapshot passes all48 full Clinical Acceptance gates with sourceUnchanged=true in clinical-acceptance-20261030-030939-332/report.json. All42 core gates and six full default-duration groups pass. No source/test/config/tool change occurred during that run. This does not close the separately identified own-item JSON primitive guard, null/deleted-source or same-process world teardown follow-ups.

Fresh private curated candidate031340-185 native pack succeeds exit0 with explicit completion:422 entries,65,526,748-byte data.pak, SHA256659D481544080E0F5033C7FF76B35C4DC89F221E71CF8887E57EB31FA3C5EC36, zero PAC1 inventory findings.1045 original source-copy files/1037 retained after the same eight reviewed copy-only exclusions; production reference assets retained. Nine packed-only phases are running in smoke040350-678, including original retrieved-source FullHeal/unused-bag saved-world expiry. No packed runtime completion claimed yet. Publication/Git remain deferred; whole Medical remains incomplete.

First packed smoke040350-678 is NOT accepted: candidate had no PackageSmokeChecks/addon.gproj; engine reported project/addon not found before any assertion. The harness reached its deadline. This was private diagnostic setup, not a PAK gameplay result. Copied and SHA-verified97 diagnostic files from the unchanged accepted Tests/Runtime source into the candidate, outside the PAK. Invoke-PackedSmoke now prepares/verifies missing diagnostic copies and rejects missing-project/addon errors promptly. Same unchanged PAK is rerunning with all nine phases; shell71690. Original pipeline99558 has ended with that recorded harness failure.

Packed smoke040734-781: seven standard packed-only phases each passed with strict packed Medical origins and unchanged PAK hashes:644 authority/26 UI/659 stock commit/658 saline access/659 source move/663 FullHeal/711 cargo. Retrieved FullHeal/unused-control seed also completed668 assertions and actual native save, but the harness's broad stock-saline-* marker condition incorrectly demanded the ordinary animation-scenario markers and rejected it. Overall result remains false; do not relabel it. Narrowed that condition to the three intended phases and added source-life-specific seed/restart markers. Only the two persistence phases are rerunning via -SourceLifetimeOnly, shell18719, same PAK/diagnostic copy. Combine scoped standard-phase evidence only after the two-phase run passes, with explicit hashes and references; do not repeat or relabel the rejected overall result.

30 October04:17UTC: candidate031340-185/packed-acceptance.json verifies all nine packed-only phases across preserved scoped records:644 authority,26 UI,659 commit,658 access,659 source move,663 FullHeal,711 cargo,668 retrieved FullHeal/unused seed and650 resumed expiry. All phase logs identify only packed Medical origins; all before/after PAK hashes match659D481544080E0F5033C7FF76B35C4DC89F221E71CF8887E57EB31FA3C5EC36, and97 diagnostic files were reverified. Overall failed harness results remain false. Seed041128-301 was valid but restart setup used the wrong nested-profile manifest path; resumed041444-353 passes650/0 against that actual original saved profile. This is private PC phase acceptance, not whole Medical, release or authentication.

Post-full48/packet changes: private manual stock-life codec041735-095 passes653/0 unchanged (eleven actual native JSON cases). Deleted-source041748-761 reproduces647/1: null Capture matched a weak-null deleted item's timer. Added early null Find guard and promoted BCMM_ZStockSalineSourceCodec.c, preserving manual named fields and native base/read order; own primitive shape guard runs before base mutation. Main codec tests are promoted and source-move now checks deletion of the genuinely used original source. Production-linked codec041902-538 passes653/0; null repro041938-744 passes647/0; main source-move20261030-041947-306 passes672/0. These changes are AFTER accepted full48/422-entry PAK. FullHeal/unused, active source and historical migration cohort71137 is now running; source frozen during it. No latest combined or refreshed packet is claimed for this later hardening batch.

Hardened protocol/null batch: retrieved FullHeal/unused042059-800 passes679 seed/661 restart, source unchanged; active attached original source042304-410 passes672/663 unchanged; historical pre-item-timer legacy-run042512-778 passes647/0 unchanged. All three cohort71137 checks finished successfully. Native AI execution cohort82195 now runs prepared shared splint positive/cancel/range, native bandage abort and group casualty response sequentially with no source/private-addon edits during snapshots. No AI branch result is claimed before its native outcome.

30 October04:57UTC: hardened-source compatibility refresh completes all eight installed fixtures, each native detail Passed=true/SourceUnchanged=true. Aircraft bodybag-compatibility-20261030-044500-410: Mi-8, Medical Little Bird, BCM UH-60, BCM HH-60G and BCM Chinook,13 checks each with actual carrier/pilot and owner/late-join inspection. HH-60G retains passed-with-upstream-diagnostic-policy. Ground045540-167: MRZR D4, JLTV and Cougar4x4,11 each, carrier owner/late join. Exact installed fixtures, no arbitrary-pack/Internet/human-controller claim. Current private AI ownership/autonomous cohort80343 is running after vehicles; no production AI change or latest AI pass is inferred yet.

Revised private AI cohort80343: contention045946-067 passed (original independent reservation survives denied AI); positive050036-821647/0, explicit cancel050106-380649/0, range050140-042649/0, qualification revocation050214-170649/0 and actual patient deletion050247-805647/0, all unchanged. Native procedure token now scopes cancellation; task that never acquired a reservation cannot cancel another. Initial autonomous050257-303 failed fixture reach immediately after setup teleport, before care. Added a pre-care500ms physics settle, preserved two-metre policy. Autonomous050504-877 now passes unchanged: normal physiology tick queues native utility task and shared timed procedure; no fixture AddAction/BeginForActor. Current three-case cohort32169 continues qualification/supply admission; do not edit its source/addon while hashes are frozen.

30 October production NPC fracture aid: main medical-ai-20261030-051823-967 passes all nine scenarios with SourceUnchanged=true. It covers shared care, contention, explicit cancellation, actual range loss, qualification revocation, patient deletion, autonomous selection, role admission and original-supply admission. A subsequent actual local-player boundary run medical-ai-20261030-052735-822 passes666/0 unchanged after adding continued player-patient exclusion: admission is refused, NPC release permits natural selection, and takeover during care cancels the exact reservation while retaining the original kit. These are local native checks, not backend authentication or full battlefield AI acceptance. Full49 clinical-acceptance-20261030-053005-482 is running on the current frozen source; staged candidate-20261030-053017-458 waits for that acceptance before native packing and nine-phase smoke checks. No combined or packet pass is claimed yet.

Current combined regression AI sub-gate `.checks/medical-ai-20261030-055931-814/result.json` passes all ten scenarios together with SourceUnchanged=true. Totals are contention661, shared660, cancel662, range662, role662, patient loss660, autonomous663, role admission664, supply admission667 and actual local-player boundary666; all zero failures. This accepts the AI sub-gate on the current source snapshot. The surrounding full49 clinical regression053005-482 is still running, and its package pipeline remains deferred until complete unchanged-source acceptance.

30 October06:29:17UTC: `.checks/clinical-acceptance-20261030-053005-482/report.json` passes all49 full-procedure gates with SourceUnchanged=true, including the integrated ten-scenario AI sub-gate and stock-source codec/null hardening. Candidate053017-458 verifies the exact accepted source and copy hashes, then native packAddon exits0 with explicit completion. One65,529,138-byte data.pak contains424 entries, zero inventory findings, SHA256 C6806AA35C871FFB907F75F662FD062FCFCC9F822A4954EE3375625747FE4E80. Packed runtime smoke062926-881 is now running; no packed phase acceptance is inferred from successful packing. Production remains frozen through that pipeline, followed by the separately staged native AI threat probes.

Current private packet smoke062926-881 PASSED, PackagesUnchanged=true, on candidate053017-458. Nine structured phases:655 authority,26 UI,670 stock commit,669 saline access,672 source move/deletion,674 FullHeal,722 cargo,679 retrieved FullHeal/unused seed,661 original-source expiry restart; all zero failures and strict packed-only Medical origins. The original Observations array also contains nine human-readable progress strings; phase accounting must filter structured Phase records. Follow-up threat waiter46868 incorrectly counted all18 entries and stopped before native execution. Corrected that private guard; restarted shell68572 now runs Shared/threat interruption and ThreatAdmission sequentially. Original packed result remains unmodified and valid. Whole Medical is still incomplete.

Additional native threat-event probes pass unchanged with no production-code changes: private run063419-340 interruption662/0 and run063453-382 autonomous threat admission665/0. They invoke the native projectile-flyby threat API rather than writing a fake threat flag, retain original supplies, assert no healing/reservation while interrupted or denied, then complete normal timed care after fixture threat clears. They do not prove live ballistic battlefield/load behavior. Both fixtures are promoted into Tests/Runtime and the AI validator now defaults to twelve cases. Main twelve-case medical-ai063604-603 is running. These diagnostic/tool changes follow the accepted full49 snapshot; production Medical source and its accepted424-entry package remain unchanged. Packed-only twelve-case AI staging awaits this exact source-test result.

Promoted twelve-case `.checks/medical-ai-20261030-063604-603/result.json` PASSED, SourceUnchanged=true. The ten prior scenarios and both native threat cases pass together. No production Medical code changed after the accepted full49/424-entry packet; only diagnostic fixtures and validator scenario lists were expanded. Packed-only twelve-case AI shell39922 now runs after verifying every production source-copy manifest entry against the accepted package snapshot.

Packed-only twelve-case AI `.checks/packed-medical-ai-development/run-20261030-064227-646/result.json` PASSED all12, PackagesUnchanged=true and DiagnosticsUnchanged=true, with strict packed Medical origins. It uses the accepted424-entry PAK and newer isolated diagnostic copy; production code is unchanged. Additional private approach work remains unaccepted: run064850-150 failed on an empty-navmesh fixture; corrected native Arland navigation065308-440 achieved12.55m actual travel and0.8m arrival, but normal queued care repeatedly interrupted and its completion failed. Failures are preserved unchanged. A logging-only follow-up now investigates actual field abort/deselection, rather than weakening treatment reach or moving actors during care.

Private terrain approach070128-955 PASSES674/0 unchanged. Native Arland soldier tiles are valid; actor travels about12.5m from twelve-metre separation, reaches about0.8m, and normal production ticks queue the shared full-duration splint using the original kit. Prior logging065844-993 identified raised weapon state as the actual Valid rejection despite reach/supply/eligibility/zero drift. A private OnActionSelected extension asks only a still-qualified active NPC controller to lower its weapon once; normal weapon/movement/timing guards remain intact. Actual care then completes. No production approach or weapon-transition change is promoted yet; qualification/threat/patient/source interruption and further navigation/player boundaries remain required.

Native weapon diagnostics: unarmed admission070855-557 failed setup; armed admission071149-222 completed treatment but crashed in recursive fixture deletion and is not accepted. Admission071426-555 passed666/0 unchanged with native Rpl cleanup. Interruption071458-352 and raise-only071754-913 completed retry but crashed at actor deletion; both remain failed. A later cleanup edit072107-411 failed compilation (GetParentSlot belongs to InventoryItemComponent, not IEntity); corrected before any further runtime. Native holster/storage-removal cleanup is now under test.

Automatic approval review rejected rerunning the crashing weapon fixture because native crash reporting had automatically uploaded logs/dumps. The rejected action was not executed. Official Bohemia startup/crash-report documentation provides -disableCrashReporter to disable appearance/sending of automatic crash reports. Current offline main launchers and active private weapon/approach/packed-AI/packing/auth launchers now include it; the private weapon launcher refuses execution without the flag or with silentCrashReport. All main PowerShell launchers parse successfully. A modified run with this supported suppression was approved and launched; process command-line verification confirms the flag. This documented option was intended to address report egress, but subsequent native evidence below shows it did not suppress reporting in this installed direct-world server mode; suppression is NOT accepted. No firewall/system-policy workaround was used. References: https://community.bistudio.com/wiki/Arma_Reforger%3AStartup_Parameters and https://community.bistudio.com/wiki/Arma_Reforger%3ACrash_Report.


Correction to crash-report suppression: run072338-018 logged -disableCrashReporter in actual CLI and still launched/sent its automatic report. Presence of the flag does NOT verify disabled egress. Crash-prone reruns stopped, and the user explicitly answered "Approve diagnostic uploads for these tests" to the approval question. Further current diagnostic runs therefore have that explicit scoped authorization; the documented flag remains best effort. No claim of silent/local-only crash reporting is valid. Callback/phased cleanup074316-578 still crashed at detached-rifle deletion. A corrected fixture now requires PURPOSE_WEAPON_PROXY and actual native weapon-manager registration before equipping the original rifle; that cohort is running. All failures remain failed.

## Documentation scope audit — 30 October 2026

Documentation-only update: see Docs/SCOPE-AND-READINESS-AUDIT-20261030.md. No gameplay, settings, prefabs, animation sources or compiled resources changed in this update; no native tests launched. Static settings comparison found 270 catalogue entries, 270 unique generated GM keys, zero missing/unknown keys and 15 section headings. Conscious broken-leg carry with acceptance, living casualty drag and original-weapon unconscious retention are required but unimplemented. Body-bag carry is outside scope; stretcher carry remains shelved with future hip-height/native-walk presentation. Animation authoring/modification is frozen; suitable native animation or timed progress is the policy. Autonomous AI removal awaits the user's go-ahead.

Latest inspected current-source AI medical-ai-20261030-075859-350/result.json is Passed=false, SourceUnchanged=true, with ThreatAdmission failing normal-tick denial while threatened. Private weapon075626-957/075700-215 pass671/0 each. Historical full49 and packed424 acceptance are retained for their earlier snapshots; this audit does not establish current-source runtime acceptance.

## Player-focused recording-safe source checkpoint — 30 October 2026

User authorized background player-focused changes including autonomous AI removal. See Docs/PLAYER-FOCUSED-IMPLEMENTATION.md. Retired active autonomous AI behavior/scheduler, dependent four fixtures, validator and acceptance gate to hash-checked .checks archive. Clinical simulation/player treatment code remains; native preservation acceptance is pending. Added default progress-only field timer policy and owner HUD; 271 settings/schema34 with strict schema33 migration. Extended consent policy for carry/drag identity, cancellation, active-offer exclusion and conscious-player-only prompting. Living transport/menu integration and sling retention remain unfinished.

Static checks only: all active PowerShell launchers parse, 346 unique resource GUIDs with no stale owned references, 271 matching unique GM keys/15 headings and valid defaults, no dangling retired AI symbols, unchanged animation assets. Runtime fixtures updated/prepared but not run. No Arma/Workbench/Blender/native test/packing launch during recording. This source has NO native compile/full-regression/package acceptance; prior all49/424-entry results remain historical.
