# Beta Test 01 — resumed acceptance

The user explicitly resumed the recording pause on 2 November 2026. Changes remain local and uncommitted; nothing has been published.

## Repairs found after the pause

- **BT01-A03: missing native bag movement replication.** A `NwkPhysicsMovementComponent` was absent from the filled-bag prefab. Authority physics moved the bag while existing peers retained an earlier position. Added the native component; three real clients, including a late joiner, observed rope movement, manual acquisition and release with position checks. Four-phase run: 697 assertions, zero failures. Extended packed testing adds native owner movement and disconnect.
- **BT01-A04: manual drag cancels on first movement.** Native movement could briefly exceed the configured safety speed before the replicated drag gait applied. Manual drag now uses replicated bag ownership to apply a crouched speed cap alongside existing injury limits. The authority still rejects sprinting, raised weapons and sustained excessive speed; a 0.35-second correction tolerance handles brief first-frame spikes. Observed dedicated owner movement and corresponding bag movement are logged separately from scripted actor relocation.
- Diagnostic corrections are retained separately: publish current physical positions; transfer actual character ownership; use native movement actions; prevent reacquisition during a release-observation phase; allow a server update after disconnect. Failed attempts remain in `.checks` and are not counted as passes.

## Completed coverage

- **Equip/stow:** 5,360 recorded native cycles across 26 handheld resources, zero reported failures. Every handheld resource reached at least 200; suction reached 360 across the interrupted and resumed campaigns. Rope, folded bag and sling are inventory-only and excluded from hand-slot cycling. Each completed cycle checks actual hand attachment, detachment, original inventory ownership and character-preview head projection. This does not establish every first-person pose or rendered pixel.
- **Clinical completion:** the resumed unchanged snapshot passed full equipment (738 assertions), native medication (705) and TXA (690), all zero failures. Report: `.checks/equipment-final-snapshot-20261102-102406-316/.checks/clinical-acceptance-20261102-105132-425/report.json`. Earlier accepted segments retain their own snapshots and are not represented as a single unchanged-build campaign.
- **Packing:** the actual native ALICE backpack holds the original bandage, splint, surgery kit, suture kit, rope and sling together. Native volume ratios relative to bandage: **1, 1.25, 3, 2.5, 1.2, 0.8**. Both acceptance of insertion and completed original-item ownership are checked.
- **Surfaces:** real rigid-body manual and rope following on flat and ±10-degree collision decks, with movement, continued reservation, surface alignment and release assertions. These are controlled collision surfaces, not a claim to have covered every map's terrain, vegetation or interior geometry.
- **Recovery:** the requested 47-entity casualty inventory, including both smoke grenades, both demolition blocks, clothing and nested equipment, survives conversion and cargo round trip without cloning the originals.
- **Human hold:** the user's “it worked as intended” confirms the prepared early-release/12-second circular-hold scene. Repetition count is unspecified.

## Acceptance limits

The evidence supports the named fixtures and configurations. It does not establish 200 full gameplay/network/restart cycles for each of 279 settings, 200 completed conversions for varied loadouts, every device procedure repeated 200 times, every LOD/view, controller hardware, console, injected packet loss or a human group playtest. Those remain explicit coverage gaps; state assertions are not relabelled as gameplay tests.

The existing base-game-only dependency is preserved. The diagnostic addon is excluded from local gameplay packages. No release/publication is authorised by these test results alone.

## Focused next playtest

1. Walk and turn while manually dragging, release, then repeat with a rope; include a doorway, slope and vehicle vicinity. Confirm grounded bag and rope anchors on another player's screen.
2. Open arsenal, equip several injectors and devices, switch items repeatedly and check the central character stays visible. Check the revised icon sheet against the live arsenal.
3. Start oxygen and longer invasive procedures; check remaining time with menu open and closed, normal completion and truthful cancellation reasons.
4. Examine A, switch to B, return to unchanged A; then change the examined domain and confirm that finding needs refreshing. A second medic must not inherit the first medic's examination.
5. Bag a fully equipped corpse using hold F, inspect all clothing, nested contents, explosives and weapons, then load/unload the bag through vehicle cargo.

The checklist supplements automated evidence; it does not block independent automated work or imply human sign-off has occurred.

## Historical packed checkpoint, 11:22 UTC (superseded build)

Package: `.checks/beta01-packed-20261102-112244-087/packed/data.pak`.

SHA-256: `444399AAEC5125D714129001061BE2CA7434BCB355E11CBA2B1EA4C9338D96AF`.

All 1,179 files in its source manifest matched the current production files at verification; diagnostic and documentation changes are separate.

| Completed final-package test | Assertions | Failures |
|---|---:|---:|
| Three-client dedicated drag, native owner movement, late join and disconnect | 702 | 0 |
| Manual/rope flat and ±10° surfaces; wall evidence invalidated below | 728 | 0 |
| Exact casualty recovery and cargo round trip | 865 | 0 |
| Native backpack packing and bandage-relative costs | 698 | 0 |
| Living casualty transport regression | 748 | 0 |
| Clinical cancellation safety | 714 | 0 |
| Assessment/cache boundary regression | 1,892 | 0 |
| Medical UI at 1280 × 720 | 28 | 0 |

The packed network owner walked **2.06 m** and the bag followed **2.08848 m**, retaining the manual-drag reservation. Both surviving clients observed the released state after the real owner process disconnected; the server cleared the session. All three clients loaded the packed addon without mounting loose production source. Evidence: `Evidence/final-packed-network-result.json` and the corresponding four console transcripts.

This checkpoint's wall fixture is invalid: moving a static deck after spawn did not rotate its native collision. The corrected spawn-transform tests below replace that obstacle evidence.

**Gate:** the listed automated repair checks pass. Wider controller/console/LOD, variable-latency and human group acceptance remains pending. This is a tested local beta candidate, not an assertion that every matrix in the feedback document is complete or that it is ready for public release.

## Continued safeguard review, 2 November

The user reaffirmed independent BCM implementation: review comparable behaviour and safeguards, but do not import third-party code requiring credits or disguise copied code through renaming. No ACE implementation has been copied into this beta work. Existing provenance records remain intact.

**A05 — manual drag allowed native jump/vault:** the existing speed/crouch restriction did not override native jump eligibility. A new native regression failed on the preceding packed build at `.checks/beta01-packed-20261102-112244-087/acceptance-Bagging-20261102-115135-221`. BCM now checks its own replicated manual-drag reservation inside its existing `CanJumpClimb` override. This applies to the owning client as well as authority and automatically ends with the reservation.

The full recovery/cargo fixture, including new assertions for blocked jumping and restored eligibility after release, passes **867 checks, zero failures** in loose source (`.checks/20261102-115226-387`) and the rebuilt packed addon (`.checks/beta01-packed-20261102-115343-720/acceptance-Bagging-20261102-115516-145`). The dedicated observer fixture now checks the owning client's same policy in acquired/released phases; that updated network test remains pending.

The 200-instance bagging and splint campaigns and the complete clinical acceptance campaign continue against their frozen preceding build. Their results must retain that build identity; they do not prove the subsequent A05 change or its new assertions.

## Collision follow-up and corrected test geometry, 12:46 UTC

The previous static-wall evidence is superseded: its test moved a mass-zero deck after spawning, leaving native collision bounds horizontal. The apparent pass did not establish wall rejection. The corrected fixture spawns the wall with its transform and asserts its vertical bounds before dragging; both manual and rope cases then release on the real obstruction. Native evidence: `.checks/20261102-124410-162` (726 assertions, zero failures). Stronger fixture assertions and repeated/network validation are in progress; this is not a completed acceptance gate.

Per-entity transport collision state now preserves every original geometry mask, including separate native character geometry, and restores it on release. Four local living-transport modes pass in `.checks/20261102-123114-588`. An independent unconscious-character control showed the native aggregate mask is stale until individual geometry setters recompute it; acceptance checks each original geometry and their effective combined mask.

The frozen clinical campaign's resumed final 14 steps passed (`.checks/full-beta01-snapshot-20261102-113742-957/.checks/clinical-acceptance-20261102-122106-239/report.json`). Its original run stopped after 37 completed step outputs when the disk filled. `report-recovered.json` preserves those outputs and hashes; the interrupted original report remains intact. These runs predate the latest collision changes and must not be presented as validation of them.

## Current collision build, 13:04 UTC

Packed artifact: `.checks/beta01-packed-20261102-130428-281/packed/data.pak`, SHA-256 `4245FAF49B5143A1BC0892E3A7777EF0E0C244A16968071534FDF9CAF05388A8`.

The collision helper retains entity and geometry identities, never a native Physics handle across frames. Release restores each saved geometry mask only while that geometry still has BCM's applied mask. A later native collision-policy change is preserved, including changes observed during reapplication. Bag dragging suppresses character collision; living carry/drag uses the native FireGeometry mask. Obstruction sweeps still prevent pulling bags through solid walls.

| Current packed checks | Assertions | Failures |
|---|---:|---:|
| Living transport, four modes and native collision-policy changes | 840 | 0 |
| Manual/rope flat, slopes and correctly spawned vertical obstacles | 730 | 0 |
| Six manual/rope terrain cases on native Arland surfaces | 722 | 0 |
| Hand attachment and 78 rendered captures across 26 items | 734 | 0 |
| Dedicated bag drag: owner movement, peer, late join, disconnect | 702 | 0 |

Packed offline results are under the artifact directory's `acceptance-*` folders. The packed network run is `.checks/drag-start-diagnostic-snapshot-20261102-132108-655/.checks/drag-network-20261102-132115-759`. The same frozen production source also passed living-transport networking: 701 assertions, zero failures, owner/patient/late-join collision observations, consent, wake-up and control restoration; result at `living-transport-network-20261102-132224-756` in the same snapshot.

The new medical-item hint displays `H Medical menu` instead of the native morphine label on procedural items. Native first-person rendering confirms the injector grip and label; physical H-key interaction is still under investigation, not counted as passed.

Three 50-instance recovered-bag batches completed on the preceding collision package. The fourth stopped after 32 complete instances when manual drag acquisition failed; its failed attempt and two consequent collision/jump assertions remain preserved at `.checks/collision-acceptance-snapshot-20261102-124847-903/.checks/repeated-Bagging-20261102-125014-057`. These are 182 completed instances plus one unresolved attempt, not a successful 200-instance campaign. New final-package runs log native actor, bag and reservation state at acquisition without weakening production safeguards.

The complete clinical campaign is reconciled in `Evidence/clinical-campaign-reconciliation.json`: 37 preserved original step outputs and 14 resumed steps, 51 expected labels covered, matching frozen source identity. This predates collision-only changes.

Vehicle dependency preflight now distinguishes unavailable addons from product failures. Vanilla Mi-8 passed 11 authority checks plus owner/late-join phases. Seven third-party entries are blocked by unavailable dependencies: MRZR `64900A5A31F5DCB5`, JLTV `5C721177A220B42F`, Cougar `5D5A20A8AE33C21E`, Medical Little Bird transitive dependency `62CCD69DD17E4F2F`, and all three BCM helicopter entries `6992B994D1870DD9`. Exact records: `.checks/drag-start-diagnostic-snapshot-20261102-132108-655/.checks/bodybag-compatibility-20261102-133334-306/results.json`. None is labelled compatible from a zero-check run.

## Completed repetition and input follow-up, 14:20 UTC

Recovery reconciliation: **202 completed native recovery/manual-drag/rope/cargo chains** across recorded batches, including the resumed six cases and two explicit airborne-guard cases. `Evidence/recovery-repetition-reconciliation.json` preserves each build, completed count and original failure. The reproduced intermittent acquisition failure logged `falling=1` immediately after a test teleport. Production correctly rejected the airborne actor. The fixture now waits up to three seconds for supporting ground; it does not disable that safeguard. Two deliberately airborne cases verified rejection followed by successful grounded recovery. These results do not claim 200 separate search, burial or send-home instances, nor 200 unique random loadouts.

Splint repetition completed **200 native fixture instances, 138,400 assertions, zero failures**, including successful treatment, missing supply, wrong leg, concurrency and movement cancellation. `Evidence/splint-200-result.json` identifies the original frozen pre-collision build. These repeat a defined scenario set; they are not 200 different clinical scenarios.

The examination-progress failure was a test timing defect: a diagnostic rerun observed only **1.7 seconds** of the authoritative **2-second** examination when the old fixed wall-time assertion ran. At **2.1 seconds**, the result existed and progress was hidden. The fixture now waits relative to acknowledged world time with a finite one-second grace bound and verifies the actual finding. Ten fresh packed UI instances passed 29 assertions each. Production examination timing was not relaxed.

The requested H toggle uses the existing rebindable medical action, keeps that action enabled while the medical panel owns input, and defers closing the exact current panel until outside input dispatch. The labels now describe a toggle. A first custom-context attempt failed native activation checks; direct action activation passed. The user then reported that the visible panel still did not close, so closure was deferred and explicit post-event logging added. The deferred implementation passes native action activation and open/close/reopen handler assertions; its final physical-key check is still pending and must not be represented as human-approved.

User confirmation that H opened the menu in all earlier test windows is recorded separately from the pending close-toggle check. Completed test windows are closed; one interactive toggle fixture remains.

User follow-up: epinephrine injector appears rear-end-up in the physical H review. After the H toggle is resolved, recheck every injector in actual hand views and correct orientation so the needle end points up. Existing attachment/equip checks do not establish correct orientation.

## H toggle correction and injector follow-up, 14:49

The direct-handler test was insufficient: `SetActionValue` through native input reproduced the second-press failure on package 141928. The menu was using `MenuWithEditorContext`, whose higher-priority input routing excluded the gameplay medical action. The fix gives the medical panel its own context inheriting the native menu context and adding `BCMM_OpenMedical`; the per-frame `ActivateAction` workaround is removed.

- Failed native-action regression retained: `.checks/beta01-packed-20261102-141928-578/acceptance-Hint-20261102-143850-927`.
- Corrected native-action test: `.checks/beta01-packed-20261102-143953-395/acceptance-Hint-20261102-144012-282`, 688 assertions, zero failures.
- Bounded 200 native open/close cycles: `.checks/beta01-packed-20261102-143953-395/acceptance-Hint-20261102-144316-290`, 1,085 assertions, zero failures, explicit `BCMM_TOGGLE_CYCLES_COMPLETE cycles=200`.
- An intervening repetition fixture omitted its termination counter; stopped that owned client and corrected the fixture. It is not counted as a completed campaign.
- User confirmed: "perfect it works now, opens and closes by pressing H". Physical logging in `.checks/toggle-context-review-20261102-144155-774` records both open and close, twice, with the native action value 1 on close.

Injector follow-up remains in progress: reviewed existing seven hand-view rows and four normalized source-model views. Epinephrine, morphine and injector variants have the rear/plunger uppermost. All seven injector model definitions now reverse their long-axis direction about the original grip offset, after assigning coloured end materials. Native model import and packed visual verification must complete before accepting the orientation repair.

## Injector orientation accepted, final local package

All six catalogue injectors (epinephrine, fentanyl, morphine, naloxone, NCD, TXA) now present the injection end upward in native front, side, and first-person views. Capped model ends remain capped; no needle geometry was invented. Seven related source model definitions were corrected, including unused/shared model variants. Evidence: `Evidence/injector-needle-up-native.png` and all 78 original hand captures under `.checks/beta01-packed-20261102-145112-692/acceptance-Hands-20261102-145134-532`. That full 26-handheld run completed 734 assertions with zero failures. Menu UI completed 29 assertions with zero failures.

A fresh native icon board exposed the rope coil's importer-default missing `material/metal.gamemat` reference. Replaced it with the installed native fabric surface and corrected the rope generator so regeneration retains that reference. The final package differs from the reviewed injector package only in rope model/metadata and the resource database; injector assets and menu code are unchanged.

Final package: `.checks/beta01-packed-20261102-145625-237/packed/data.pak`, SHA-256 `8f535cba61d5c7b0b81be468c610cee3a837530d039f2095c5f9636b74971882`. Final native packing: 698 assertions, zero failures. Final native toggle: 10 cycles, 705 assertions, zero failures. Resource GUID audit: 391 unique resources, passed. Source/package manifest comparison: no mismatches. `git diff --check` passed. Local only, not published.

Broader acceptance limits remain as recorded: absent third-party vehicle dependencies, controller/LOD/group-session coverage, and gameplay combinations not represented by the identified native fixtures. These limitations are not converted into passes by the H or injector fixes.

## Continued native acceptance, 16:28 UTC

**BT01-A06, verified:** patient-list guidance was clipped at 4K. Existing control-bounds checks passed while the text exceeded its widget. The native `GetTextSize` result is in logical units; converting it through `Workspace.DPIScale` reproduces the failure on the preceding package. Production now wraps the guidance inside its existing panel. Native rendered UI passed **30 assertions each** at 1280x720, 3440x1440 and 3840x2160; the 4K result shows both lines completely. Source/package manifest comparison is exact; only `BCMM_MedicalMenu.c` differs from the preceding package. Current package SHA-256: `bedda0f29e13af1009581140adaafef8a4c543940c75e35ab41badcd0d078edf`.

**BT01-018, PC native placement verified:** an actually recovered fully equipped casualty exposes all eight actions in one native context, centred within its rendered footprint. The native character input path aims the real player camera downward, and the screenshot shows the central F/Drag body bag prompt with separate contextual rows. Completed fixture: **760 assertions, zero failures**. Initial captures did not aim correctly and were not accepted as visual proof; an exploratory four-direction capture identified the native aiming axes. Physical controller operation remains unverified.

**Clinical safety repetition:** twenty completed native instances on package 145625 passed **14,280 assertions**. A test-only standalone refund arsenal was missing from cleanup; that run cannot establish production memory retention. The fixture now deletes its own arsenal before base cleanup. Ten subsequent instances passed **7,140 assertions**. Total gameplay count: 30 instances, **21,420 assertions, zero failures**. Cases include actual injection animation, unsafe initial vitals, mid-injection deterioration, progressive fluid delivery and access loss, reserved-supply/containing-backpack refund rejection, and casualty-only range loss with unchanged oxygen supply. These are repeated defined cases, not 30 unique clinical scenarios or all possible interventions.

The corrected ten-instance run recorded 26 process samples. After the first minute, private memory changed from 2305.7 MiB to 2297.4 MiB and handle count from 1254 to 1237. This short local run does not establish hours-long dedicated multiplayer soak stability.

Exact results and artifact identities: `Evidence/resumed-native-coverage.json`. Visual proof: `Evidence/bag-action-placement.png`, `Evidence/ui-4k-fixed.png`, `Evidence/ui-ultrawide-fixed.png`. Tooling now verifies requested resolution against the native BMP header, supports bounded repeated packed cases, samples process resources, and refuses to pass when requested iterations are incomplete. No release/publication was performed. Controller, group-session, full LOD/distance coverage and absent third-party vehicle dependencies remain open.

## Continued packed multiplayer and native review, 17:20 UTC

Package remains `bedda0f29e13af1009581140adaafef8a4c543940c75e35ab41badcd0d078edf`; no production change in this continuation. Dedicated runs use actual game clients and exclude loose production mounts. Evidence is indexed in `Evidence/final-packed-network.json`, including retained failed attempts; an index entry alone is not acceptance.

- Body-bag drag: 702 assertions, owner movement, peer/late-join state and disconnect cleanup.
- Living transport: 701 assertions with carrier, patient and late-joining clients. Separate native active-AI carry/drag: 846 assertions. This is an active idle AI casualty, not autonomous medical AI or combat navigation.
- Splint/medication/equipment: 704 authority assertions plus all required client observations, private panel checks, role changes and binding rebind/hold/reset.
- Fluid lines: 684 assertions, owner connection, moving out of range, zero-bag removal and late-join observations.
- Cargo: 747 assertions and all eight phases observed by each client. Burial/search: 857 assertions and native owner/observer workflow.
- Native saved-world restart: 750 seed assertions, 685 restarted-server assertions and owner/peer/late-join observations. The saved UUID is retained with its result.
- Stationary stretcher: 687 assertions in the original multiplayer fixture. Additional real patient-owner stationary placement/unloading also completed; consult the `171626-348` run and its ownership marker.

The vanilla dedicated/client control reproduced the splash-layout unknown-class error, null loading-layout/zero GUID errors and offline authorization warning without BCM loaded. They are not evidence of a BCM-specific regression. Native inventory `Frame0` slot warnings remain separately recorded rather than silently removed from logs.

A broader optional stretcher run exposed old fixture assumptions: carrying and grip presentation now default off, but the fixture did not explicitly enable them. The fixture now checks disabled carrying then enables the requested optional settings through authority. The experimental hand-pose test still fails at a nondefault 1.7 m handle offset (approximately 0.14 m contact error); a post-carry server-position tolerance check also failed while owner/proxy clients reported the expected adjacent unload. These are retained failures, not passes. The controlling scope record explicitly shelves two-player stretcher carrying and custom grip animation; production defaults and CLI gates remain unchanged. The supported stationary workflow is tested independently with a real patient owner.

Native world-distance review captured all 29 catalogue items at 0.75, 3 and 12 metres: 87 screenshots, 739 assertions, zero failures, run `acceptance-WorldReview-20261102-165832-262`. All five labelled sheets were inspected. No new detached BVM mask or gross geometry defect was found. Very small items are subpixel or poorly distinguishable at the far distance; this is not proof of every graphics preset or LOD transition. Original images and fixed-scale crop sheets remain in that run.

The 200-instance clinical campaign is still running and is not yet counted as complete. Initial impairment attempt bypassed the UDP proxy because the client ignored the separate port argument; zero proxy traffic correctly invalidated its impairment claim despite successful ordinary fluid assertions. A corrected explicit endpoint is being tested.

## Extended verification, 18:03 UTC

The current packed production artifact is unchanged. Final-package H input-action testing completed 200 open/close cycles (1,085 assertions, zero failures, `acceptance-Hint-20261102-174448-141`); the earlier physical-key user confirmation remains separate evidence.

Active idle US, USSR and FIA AI casualties each completed conscious/unconscious carry and drag, with exact original inventory identity and count comparisons immediately before transport and after release. Final results: US 946, USSR 942, FIA 930 assertions, all zero failures. The first faction fixture crashed during immediate cleanup; native GUIDs were corrected and deletion deferred until native Physics locals had left scope. A separate inventory-count failure came from taking the manifest before incapacitation setup finished; the corrected boundary is immediately before transport. Those failed runs remain retained. These checks establish neither combat navigation nor autonomous medical care.

Packed dedicated tests also passed for arsenal, applied equipment, lives/admin, medical operations, oxygen medication/removal, stock commits, fluid burden, animation and progressive fluids. The progressive-fluid launch initially failed because the diagnostic snapshot omitted Assets; after supplying that diagnostic input, the native run passed. See `Evidence/final-packed-network.json` for logs and explicit results rather than treating every indexed attempt as successful.

Synthetic loopback impairment passed body-bag drag and living transport at 150 ms one-way delay, +/-40 ms jitter and 3% packet loss. At 250 ms one-way delay, +/-75 ms jitter and 5% loss, body-bag drag, living transport and fluid lines passed with three actual client endpoints. Final living transport forwarded 29,671 datagrams and dropped 1,609; fluid lines forwarded 16,674 and dropped 921. Exact proxy seeds, hashes and attempts are retained in `Evidence/continuation-coverage.json`. Shutdown connection resets are recorded separately from proxy errors.

The heavier living-transport attempts exposed two diagnostic timing assumptions. The patient now waits for a fresh consent token rather than answering an already-declined offer. Cleanup is timed from authority-observed release, not from publishing a client phase before the delayed release arrives; it waits within the production retry bound and still fails on an uncleared holder. No production safeguard was weakened. Both failed attempts remain evidence; the corrected native fixture passed. Additional independent loss seeds are running.

The native inventory control reproduced the same `Frame0` GridWidgetSlot/LayoutSlot warning with only the base game and a diagnostic deck/script addon, without BCM production code or UI. The native US character and Ural inventory opened successfully and its screenshot was inspected. Control: `.checks/native-inventory-control-20261102-175339-927/native-control`; earlier failed control compilations are retained. This warning is not a demonstrated BCM-specific defect.

The 200-instance clinical safety run remains in progress; partial iterations are not reported as a completed campaign. The seven missing third-party vehicle dependencies, physical controller operation, public/group-session acceptance and exhaustive graphics/LOD coverage remain unverified. Shelved two-person stretcher/grip prototypes remain outside the supported stationary workflow.

## High-loss follow-up, 18:12 UTC

The first additional seed 303 drag run failed during owner movement with `sustained movement exceeds manual drag speed limit`; it is not erased by later passes. Diagnostic velocity/policy traces are now present. A traced rerun and another seed 303 run passed, so the initial failure remains an intermittent case under investigation.

Seed 404 then exposed a separate fixture setup failure: the server-only teleport of a client-owned character was overwritten by native client movement. The owner stayed 2.03948 m from the bag, beyond the diagnostic's 2 m request threshold, and correctly never sent the request. Diagnostic repositioning now occurs on the owning client, waits one second before requesting, and retains all production range/revision/authority checks. Seed 404 passed with this correction; additional seeds are still running.

The retained local ACE reference was checked again for movement and collision lifecycle methods (`ACE_CarriableEntityComponent`, `ACE_CarriableAnimationHelperComponent`, carrying character controller). It restricts owner input/stance and restores collision on detach. BCM uses its independent movement-cap/reservation implementation and per-entity/per-geometry restoration; no reference implementation was imported. This review is a design comparison, not evidence that every ACE safeguard or feature is identical.

## Clinical repetition completed, 18:19 UTC

The unchanged packed package completed all **200 defined native clinical safety instances**, **142,800 assertions and zero failures**, token `40284d391dc74ea49fdded7b1de3f3c9`. This total includes repeated baseline assertions; it is not a count of distinct clinical scenarios. The final iteration marker, completion, actual isolated packed mount and log hash are recorded in `Evidence/continuation-coverage.json`.

The run collected 501 process samples over 5,089 seconds (about 85 minutes). Private memory after the first minute was 2,329.78 MiB and ended at 2,341.23 MiB, a difference of 11.46 MiB; peak private memory was 2,344.59 MiB. Handles decreased from 1,247 to 1,233. This bounded local native repetition completed without crash or assertion failure; it does not establish an hours-long multiplayer soak or prove the absence of every leak.

The corrected owner-positioning drag fixture passed seeds 404, 505, 606 and 707, then three separate replays of seed 303 at 250 ms one-way delay, +/-75 ms jitter and 5% loss. Production drag code and safety limits were unchanged. The early server-only repositioning failures remain retained, including the initial speed-guard release; the available evidence is consistent with the diagnostic ownership/position-correction race, but the original run did not log every velocity and cannot establish that cause conclusively. The corrected flow is separately validated rather than rewriting that failed attempt as a pass.

## Network observation ordering, 18:33 UTC

Additional high-loss attempts exposed diagnostic observation races beyond the earlier source corrections. A post-wake consent request was sent while the rescuer client still held the preceding carried state (trace: phase 17, mode 1, eligible 0). The final positive consent-cancel case now waits for replicated release, wakefulness and eligibility. The corrected case completed 701 authority assertions; that run still lacked the late-join phase-4 observation because the late client finished loading after the timed carry stage had ended. It remains a failed overall run.

The living-transport fixture now waits for authenticated test-only acknowledgements from all three clients at the required carry/drag/released observation stages, plus the actual patient's retained-weapon, restored-controls and cancelled-token replay observations. Existing motion, pose, collision and inventory assertions remain required; each stage still has a 25-second bound. A four-seed high-loss campaign is validating the corrected fixture.

Fluid-line seed 303 passed. Seed 404 completed its authority assertions but the fixed eight-second diagnostic teardown closed the server before the late client logged removal. Replaced the timer with actual three-client acknowledgements and a 25-second observation limit. The corrected seed-404 run completed **685 assertions, zero failures**, with all three removal observations before shutdown (`impaired-FluidLine-20261102-182746-970`). This is stronger observation sequencing, not a change to fluid behavior or an acceptance of missing client evidence.

Future impaired-network attempts now retain their exact diagnostic source tree, driver source and diagnostic manifest alongside result and proxy statistics. Source changes during a run invalidate acceptance. Earlier failures remain in the evidence index; their available logs/manifests are not represented as complete immutable diagnostic archives.

## Final local checkpoint, 18:39 UTC

The final acknowledged living-transport runs passed seeds **303, 404, 505 and 606**, each with 250 ms one-way delay, +/-75 ms jitter, 5% packet loss, a dedicated server and three actual client endpoints. Each completed 701 authority assertions and all required owner/patient/late-join observations. Forwarded/dropped datagrams respectively: 29,331/1,567; 30,001/1,545; 33,694/1,819; 29,546/1,548. Each retains its diagnostic sources and unchanged-source manifest. No production safety checks were relaxed.

The impairment index contains **29 retained attempts: 21 successful, 8 unsuccessful**. Failed attempts are not added to pass counts. The network log index contains 56 retained runs and 219 console logs, including failures and controls; those totals are archival counts, not successful-test counts. The 200-instance clinical campaign and its 501 resource samples are also retained in `Evidence/clinical-safety-200-result.json`.

The 29-item catalogue now records actual three-view handheld review (with three inventory-only exceptions) and the 0.75/3/12 m world captures. All 40 discovered implementation surfaces link to related workflow evidence or are explicitly marked state-only/shelved. Related evidence is partial coverage, not a claim that every method or every possible setting combination was exercised. The 279 per-setting gameplay/multiplayer/restart counts have not been inflated from broad suites.

Current production remains the unchanged local package with SHA-256 `bedda0f29e13af1009581140adaafef8a4c543940c75e35ab41badcd0d078edf`. The production manifest recheck covered 1,181 entries with zero mismatches. The resource GUID audit passed 391 unique resources; final diff whitespace checks passed. Owned native test clients and servers closed after completion. Nothing was committed, pushed or published.

**Acceptance boundary:** the identified local/native/packed dedicated workflows pass within their recorded scope. Seven absent third-party vehicle dependencies, physical controller/console operation, a human/public group session, all graphics/LOD combinations and a 200-cycle live gameplay matrix for every setting/procedure remain unverified. The optional two-person stretcher/grip prototype remains shelved with its experimental failures preserved. The original high-loss speed-release attempt has incomplete diagnostic evidence, although seven corrected owner-setup runs passed; no claim of universal network reliability or visual perfection is made.

## Vehicle retest on 3 November 2026

All seven third-party vehicle entries completed packed dedicated multiplayer cargo testing. The owner and late joiner observed each required stage. MRZR D4, JLTV and Cougar 4x4 each passed 11 server checks. Medical Little Bird, BCM UH-60, BCM HH-60G and the installed ACM MH-47G SOAR Chinook each passed 13 piloted checks, including flight, landing and unload. Each of the seven independently passed 21 server-side transfer checks covering the original bag, equipment and casualty record. Each accepted log loaded the medical resource database from the expected packed directory.

The current BCM Vehicles release depends on the ACM Chinook. Its installed SOAR prefab replaces the obsolete H47/CH47F_MY2 test reference. The UH-60 test explicitly loads MH60VariantsAwenzj and its dependencies alongside BCM Vehicles. The HH-60G passed with UH60TweaksAwenzj and BCM Vehicles, under the existing narrow support-station diagnostic policy. Adding the optional MH60VariantsAwenzj layer to the HH-60G produced electrical-system and startup-sequence errors during initialization. That combination failed its check. The installed variant explicitly disables both components.

Steam authentication recovered before the successful client runs. The official Workshop downloader supplied the base JLTV, Cougar and AKI Core in a private test directory. The vehicle runner now accepts additional addon directories and a packed production directory. The diagnostic stops after a rejected cargo start, preserving the original rejection without a follow-on null inventory exception.

The [vehicle evidence](Evidence/vehicle-download-retest-20261103.json) records the package digest, exact prefabs and addons, native completion markers, client observations, log hashes and failed attempts. This supersedes the seven vehicle dependency blocks reported on 2 November. The wider controller, human group-play, settings and graphics coverage gaps are tracked separately.
