# BCM Medical development roadmap

Current engineering checkpoint: the agreed player-focused changes passed all 51 validation stages with documented diagnostic-only continuity, 12 packed-only phases and a packed dedicated-server/three-client transport test (701/0). The private package is built. See [PLAYER-TEST-CANDIDATE-20261030.md](PLAYER-TEST-CANDIDATE-20261030.md) for exact evidence, package location and remaining human/Internet/art acceptance. The latest carry revision is not marked visually approved by the user. Earlier checkpoint paragraphs below are historical where superseded.

## Current source checkpoint — 30 October 2026

Recording has finished and native development has resumed. See [RESUMED-DEVELOPMENT-20261030.md](RESUMED-DEVELOPMENT-20261030.md) for current evidence. Autonomous medical AI is removed; the approved inventory sling is imported. The catalogue is 279 settings/schema35. Compact medical-menu carry/drag controls, conscious-player consent and sling retention have native functional tests, including dedicated owner/proxy/late-join coverage. Casualty poses remain visually unfinished; see [CASUALTY-POSE-REFERENCE-20261030.md](CASUALTY-POSE-REFERENCE-20261030.md). Full regression/private packed acceptance is pending. The user subsequently authorized two original casualty poses; existing rescuer locomotion and earlier animations remain unchanged. No ACE code/assets/dependency are imported. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are implemented and have current native/private-package validation; visual review and human server acceptance remain open. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. The subsequent explicit exception permits two original BCM casualty poses only. Existing rescuer locomotion and earlier custom assets remain unchanged; other animation creation/modification requires a focused user decision. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


Updated 30 October 2026. BCM Medical remains in development. Production is the single MCB Medical project, GUID 6AB4B3F3FD215919, with only the base-game dependency. There are 279 settings (schema 35) in one BCM Medical GM tab with fifteen section headings.

Body-bag respawn restrictions, waiting/spectator logic and the Body Bag Admin menu are removed. Limited Lives is the sole BCM deployment allowance policy. Physical recovery is independent. Two-player stretcher carrying remains shelved. Git and publication remain deferred.

## Implemented and exercised in the native engine

- Linked trauma/physiology: native blood loss, pressure, pulse, ventilation, consciousness, cause-based arrest/recovery, external bleeding, internal/chest injuries, fracture effects, pain and configurable medication kinetics.
- Selected-region treatment: native dressings, splinting, extraction, suturing, internal repair and facility fracture surgery; timed procedures, cancellation and original-supply accounting.
- Medical panel: native personal keybind/reset, self and aimed conscious/unconscious patient selection, same-vehicle viewing, private timed examinations, anatomy/tabs/pagination, native progress and live supply availability. See [player controls](PLAYER-CONTROLS.md).
- Finite oxygen and six medication kinds, retained applied equipment, original tablet self/handover clips, native AED/BVM/airway/suction/seal/decompression/drain/access/monitor workflows. See [oxygen and medications](OXYGEN-AND-EXPANDED-MEDICATIONS.md).
- Typed finite fluid bags with progressive held delivery, partial-content retention and independent patient-owned drips. Original source UUIDs restore across native saves; active transport can retain an already connected drip. See [independent lines](INDEPENDENT-FLUID-LINES.md).
- Optional redistributed-fluid burden and gradual red-cell/clotting recovery. Both policies default disabled and have separate saved state/eligibility. See [fluid burden](FLUID-BURDEN.md) and [component recovery](COMPONENT-RECOVERY.md).
- Native stock-consumable commit protection: revoked permission retains the original injector without a dose; ordinary subsequent use consumes it once and applies one dose. See [stock consumables](NATIVE-STOCK-CONSUMABLES.md).
- Native US/USSR/FIA catalogues with 28 medical items, live prices/availability/refunds, server purchase enforcement and actual native saline transactions. Stationary treatment stretcher has native GM catalogue placement/restart. See [arsenals](ARSENAL-INTEGRATION.md) and [stationary stretcher](GM-STATIONARY-STRETCHER.md).
- Corpse-to-searchable-bag equipment recovery, native cargo transfer, rollback, manual/rope physics, burial, Send Body Home and disposal records. Unloading leaves deposited equipment in the vehicle. See [physical recovery](BODY-BAG-INTEGRATION.md), [cargo](BODY-BAG-CARGO.md) and [search](BODY-BAG-SEARCH.md).
- Native saved-world clinical/finite-source/remains/vehicle restoration and diagnostic-account life accounting. Connected GM allowance controls use the existing native context list. See [persistence boundaries](NATIVE-PERSISTENCE-DESIGN.md) and [allowance administration](LIMITED-LIVES-ADMINISTRATION.md).

These are implemented paths with recorded scope-specific checks. They do not establish every item interaction, arbitrary addon or release requirement.

## Latest evidence and current work

| Snapshot or scope | Result |
| --- | --- |
| Before the stock fix, 270 settings/schema33 combined regression214430-980 | All 33 gates passed; source unchanged, 22:25:26UTC |
| Current stock commit/reuse223731-487 | 649 native assertions, zero failures |
| Current stock owner/proxy/post-revocation joiner225032-517 | 643 authority assertions, zero failures; source unchanged |
| Current supported clinical-safety223857-753 | 668 assertions, zero failures |
| Passenger/drip world restart223748-938 | 647 seed/641 restart assertions, zero failures; source unchanged |
| Pre-saline-binding Mi-8, Medical Little Bird, UH-60, HH-60G, Chinook225220-683 | 13 checks each, actual carrier/pilot ownership and owner/late-join inspection; HH-60G retains its upstream diagnostic policy annotation |
| Pre-saline-binding MRZR, JLTV, Cougar230304-641 | 11 checks each with owner/late-join inspection |
| Curated private pre-saline-binding PC packet225341-700 | 419 entries, 65,521,683-byte PAK, about 21% smaller than the earlier baseline; production reference assets retained |
| Packed-output-only smoke230519-328 | 634 core/26 UI/649 stock commit/701 cargo assertions; zero failures, unchanged PAK hashes and no development-source fallback |
| Earlier stock-commit snapshot230712-443 | All 36 gates passed; source unchanged, 23:49:23 UTC; before the subsequent native-saline persistence work |
| Native stock saline continuity | Promoted access656/source loss657/FullHeal661/deferred650 checks pass; active world658/650 and inactive659/650 pass unchanged. Full42 combined run003550-595 passes unchanged at01:24:07UTC. See [native saline](NATIVE-STOCK-SALINE.md). |

Latest combined snapshot030939-332 passes all48 gates unchanged at04:03:42UTC on30 October. Its private PC packet031340-185 has422 entries/65,526,748 bytes and zero inventory findings; packed-acceptance.json verifies nine phases against the same unchanged PAK, including actual retrieved-source FullHeal/unused-control save/restart expiry. Failed private harness attempts remain recorded separately.

Subsequent hardening promotes the manual item-timer codec/primitive guard and fixes a reproduced null lookup after native source deletion. The linked codec passes653/0, the null reproduction now647/0, and standard original-source movement/deletion672/0. This later code is outside the preceding full48/packet snapshot; its focused restart and compatibility results follow. Same-process teardown, broader AI and the remaining whole Medical ledger remain open.

The later hardening restart cohort passes unchanged: retrieved FullHeal/unused679/661, active-source672/663 and historical migration647/0. Current-source compatibility refresh044500-410/045540-167 also passes all eight installed fixtures: five aircraft13 each, three modded ground vehicles11 each, actual carrier/pilot and owner/late-join checks. HH-60G retains its upstream-diagnostic policy annotation. Qualified NPC fracture aid is integrated: main runtime051823-967 passes nine scenarios unchanged; actual local-player admission/takeover052735-822 passes666/0 after continued player-patient exclusion. Full49 combined regression053005-482 passes all49 gates with SourceUnchanged=true at06:29:17UTC, including all ten AI scenarios. Candidate053017-458 then packs424 entries/65,529,138 bytes with zero inventory findings; all nine packed-only phases then pass in smoke062926-881 with unchanged PAK hashes and no development-source fallback. Later twelve-case packed AI passed; current source subsequently changed and its threat-admission regression failed. See the dated audit for the exact checkpoint.

Full native logs, source fingerprints, historical failures and limits are recorded in [VALIDATION.md](../VALIDATION.md). Current execution details are in [BACKGROUND-WORK-QUEUE.md](BACKGROUND-WORK-QUEUE.md). Do not relabel earlier snapshots as validation of later source changes.

## Remaining for the full standalone replacement

1. **Clinical breadth:** additional routes/IO and dose/formulary detail, broader transfusion complications, monitoring/ventilation and fuller native-stock item/finite-content/persistence coverage.
2. **Injury and care systems:** organ/brain/blast/burn/armour/fracture calibration, further complications, facilities/qualification policy and player-facing care calibration. Autonomous medical AI has been removed following explicit authorization; player clinical care and player treatment of AI casualties are preserved.
3. **Interaction and transport:** current original living carry/drag, conscious-player consent and sling-based weapon retention require complete current-build regression and human acceptance. Further work includes new native in-vehicle procedures, broader lifecycle contention/disconnect/corrupt-reference cases and additional arbitrary vehicle packs. Existing connected drips during transport do not imply permission for new in-vehicle procedures.
4. **Authenticated accounts and networking:** real backend-authenticated Limited Lives/paid deployment/reconnect, offline-account administration, Internet/latency/load. Current diagnostic account fixtures are not authentication acceptance. Config-only raw-backend probes reject the two unpublished IDs during Workshop lookup; direct-world mode bypasses the required loopback hosting configuration. Publication remains deferred.
5. **Final supplied art:** oxygen cylinder, tablet/pack props, final applied-device placement/materials, optional working surgical/drain/airway tool presentation and visible rope. Missing action animations use timed progress rather than custom clip work. Keep two-player stretcher carrying shelved. See [asset brief](ASSET-PURCHASE-BRIEF.md).
6. **Input and presentation:** human keyboard/controller/accessibility and small-screen play testing, broader UI review, spatial audio and progress-bar feedback review. Earlier animation assets remain frozen; only two original casualty poses are newly authorized. Automated native input/renderer checks are narrower evidence.
7. **Distribution and release:** broader catalogues/purchase/refund scenarios, complete source-asset/licence/receipt review, performance, clean-install/platform packages and final release regression. The private PC packet is not a published or licence-cleared release. See [packaging](RELEASE-PACKAGING.md).

The earlier chronological roadmap is preserved in [ROADMAP-HISTORY-20261029.md](ROADMAP-HISTORY-20261029.md). Continue this ledger after each accepted milestone; whole-mod completion requires the remaining applicable work and its real acceptance evidence.
