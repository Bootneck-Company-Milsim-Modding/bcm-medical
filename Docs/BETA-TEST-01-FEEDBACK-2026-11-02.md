# BCM Medical — Beta Test 01: Initial Feedback & Mandatory Codex QA Campaign

**Date:** 2 November 2026
**Repository:** `Bootneck-Company-Milsim-Modding/bcm-medical`
**Proposed repository path:** `Docs/BETA-TEST-01-FEEDBACK-2026-11-02.md`
**Status:** OPEN — first-hand player observations, not yet independently reproduced, fixed or validated.

> **DIRECTIVE TO CODEX:** Investigate, reproduce and correct EVERY reported defect below, then rigorously test every other aspect of BCM Medical. Do not claim that code inspection, mocks or synthetic test passes prove actual in-game UI, physics, item sizes, animations or multiplayer behavior. Treat results as VERIFIED only when objective acceptance evidence exists. Preserve errors and blocked tests; never invent passes. This report is not an exhaustive list of defects: actively seek further defects.

## A. Initial findings and acceptance requirements

| ID | Priority | Finding | Required correction and verification |
|---|---|---|---|
| BT01-001 | P1 | Almost all arsenal medical icons are too big, poorly rotated or poorly framed. The body bag and gas mask are the only approximately correct icons. | Review *every* medical icon independently. Ensure sensible scale, center, margins, recognizable front-facing angle, no clipping, consistent presentation. Preserve the two good references. |
| BT01-002 | P1 | Drag rope appears far too large, including inventory and arsenal presentation. | Make its actual carryable object, inventory footprint and rendered icon compact; do not only zoom out the icon camera. |
| BT01-003 | P1 | Splint is too large. | Reduce its model/inventory footprint to little more than a native bandage and make it fit easily into bags. |
| BT01-004 | P1 | Suture kit and survival/surgery kit are too big. | Aim for approximately 2–3 native bandages' worth of packing space each, verified with actual in-game bag contents and visual scale. |
| BT01-005 | P2 | Chest seal is badly oriented. | Rotate item/icon to display the recognizable seal face. |
| BT01-006 | P2 | AED renders as a white, flat-looking object. | Present recognizable front/display; diagnose material/LOD faults as well as rotation. |
| BT01-007 | P1 | Weapon sling is significantly oversized. | Substantially shrink physical and inventory form, enabling easy carriage without breaking retention. |
| BT01-008 | P1 | NCD, epinephrine and other needles/injectors have incorrect hand orientation. | Match native Arma Reforger syringe/injector equip orientation, grip and first/third-person transform for *every* relevant medical item. |
| BT01-009 | P1 | Center player character preview disappears after equipping medical items for a while in arsenal. | Reproduce by repeated equip/swap/unequip, inspect logs, attachment/prefab hierarchy, preview camera, rendering, memory and lifetime; fix disappearing character. |
| BT01-010 | P2 | Blood and plasma bags look wrong. | Blood bag **red**, plasma bag **yellow**, consistently across inventory, world and arsenal representations. |
| BT01-011 | P1 | Saline needs no custom replacement. | Reuse the *native in-game saline bag* where viable, maintaining correct BCM fluid identification/function. |
| BT01-012 | P2 | BVM has one correct bag-attached mask and a second floating mask above it. | Remove only duplicated/floating mask child or mesh, preserving attached mask. Check all views/LODs. |
| BT01-013 | P0 | Rope attaches to body bag but floats; bag does not drag. | Working rope attachment, anchors, rope visualization, bag-follow movement, terrain collision and network replication; test slopes, obstacles, release and disconnect. |
| BT01-014 | P0 | Ordinary manual body-bag dragging does not work. | Fully functional manual drag in addition to rope drag; proper pose, motion, groundedness, player controls and replication. |
| BT01-015 | P1 | Filled/loaded body bag floats too high. | Lower to natural ground contact; avoid clipping and floating through all bag states and terrain surfaces. |
| BT01-016 | P1 | Body bagging is instantaneous. | **Hold F for 10–15 seconds** (default; configurable if appropriate), with native-style visible circular hold progress. Release/interrupt cancels without consuming bag. Only server-validated completion creates bag and transfers equipment. |
| BT01-017 | P0 | Body-bag equipment transfer captured less than 10% of corpse inventory: only magazines and light; grenade, **two smoke grenades, binoculars, watch, two demolition blocks, and all clothing** remained outside. | Preserve **all eligible equipment**—every worn clothing item, containers and nested contents, weapons/attachments, explosives, tools and accessories; reconcile exact before/after manifests, quantities and ownership, no loss or duplication. Document genuine engine exceptions. |
| BT01-018 | P2 | Drag and other body-bag contextual options are poorly placed. | Put relevant interaction menu options centrally and consistently, with keyboard/controller support and no overlap. |
| BT01-019 | P0 | Many medical-menu operations announce started (e.g. **"Oxygen connected started"**) then auto-cancel. | Investigate actor/target validity, range, authority, timing lease, procedure progress, inventory reservations, state update and RPC lifecycle. Treatments must finish normally or provide a *specific truthful cancellation reason*. Test all operations. |
| BT01-020 | P1 | Long procedures provide no useful progress or time-to-finish indicator. | Visible, continuously updated timed **progress bar** and remaining time, including appropriate closed-menu owner HUD; accurate server-authoritative completion, cancellation and result. |
| BT01-021 | P1 | Examination data is forgotten when switching patients. Example: examine patient A airway, switch to patient B, return to A and need to re-examine despite no airway change. | Cache examination knowledge **per patient/life and assessment domain**, retaining results while relevant state remains unchanged; invalidate on relevant changes, death/respawn, incorrect identity or intentional access rules. Verify multiplayer ownership/privacy and repeated switching. |

**Visual evidence:** User-supplied arsenal screenshot from 2 November 2026. Add it to the repository test artifacts when available. Screenshot illustrates framing/orientation issues; it does not alone prove physical world scale.

## B. Source and repair protocol for Codex

1. Audit the active branch and latest code, README, `Docs/SOURCE-SNAPSHOT-20261031.md`, runtime prefabs/assets, UI, inventory and settings; historical notes are not necessarily current. Trace every symptom from player action through local UI, server authority, inventory, physics and replication.
2. Reproduce BT01-001–021, recording exact steps, environment, expected/observed result, logs, root cause and relevant resource/file. A failure to reproduce means **UNVERIFIED**, not fixed.
3. Repair foundational P0s first: equipment recovery, body bag manual/rope dragging and procedure auto-cancellation. Then fix the arsenal preview, asset physical sizes, UI/progress and visual assets. Retest each subsystem before proceeding.
4. Check every current medical inventory item, not just those named: all icon rotations/camera frames, world object dimensions, collision, inventory capacity costs, hand pose/attachment, stored/equipped presentation and LOD/material.
5. Check every user-visible action from request to server completion, including cancellation, range, roles, invalid states and simultaneous client requests. Make failures explain themselves.
6. Preserve the standalone base-game-only production dependency. Do not copy ACE/KAT code, animation assets or addon content; do not reintroduce removed autonomous medical AI or shelved two-person stretcher carry.
7. Never change a numeric item dimension without checking native bandage reference and functional carrying. Keep world geometry, packing volume and icon zoom separate.

## C. Mandatory deep regression test design

### C1 — Inventory of every testable surface

Generate a machine-readable, itemized test catalogue of **every** production medical inventory entry and variant, every GM setting (the 31 October README says **279 saved controls, 15 sections, settings schema 35**; confirm actual current count from `Tools/SettingsCatalog.json`), every injury/treatment, medication, fluid, airway intervention, device, medical facility, body bag and transport action, interaction menu, UI control, lifecycle transition, save/load and role/lives control. Reconcile against code and manifests so no implemented system is omitted.

### C2 — Repetition targets and assertions

- **Every GM setting:** target **200 meaningful set/apply/read-back cycles per setting**, including minimum, maximum, default, typical intermediate values, allowed step increments, toggles/enums, boundary rejection, live behavior, role/authority, reload/reconnect, persistence and schema migration. Assert real *gameplay effects*, not just serialized values. If 200 full native cycles cannot be run, report exactly how many native cycles ran, perform 200 automated state assertions, and label remaining live coverage as a gap.
- **Splint treatment:** at least **200 distinct application scenarios**, covering success, already-treated, missing supply, cancellation, injury changes, role, movement, interrupted procedure, facility rules, multiplayer ownership and repeat application. Confirm exact equipment consumption, correct fracture outcome and retained state. Apply comparable 200-case targets to each other medication, medical procedure, fluid and equipment workflow where practical.
- **Body bag creation, equipment capture, manual drag, rope attach/drag/detach, cargo load/unload, search, burial and send-home:** **at least 200 completed test instances per workflow**; before/after complete inventory manifests for at least 200 varying corpse loadouts, including nested bags, worn clothing, all named missing items and weapons/attachments. Assert zero unaccounted item loss/duplication.
- **Every arsenal medical item:** **200 equip/unequip/swap cycles per item** and periodic view/preview screenshots; actively monitor for disappearing central player model, render errors, crash, resource leaks and incorrect pose.
- **Every timed medical procedure:** at least **200 meaningful test iterations** where feasible, mixing successes, interruptions and expected failures. Assert actual duration within justified tolerances, progress UI, exact supply reservation/consumption, authority and final physiological state. Explicitly test oxygen start/connect/disconnect and no unexplained cancel.
- **Body-bag hold-F action:** repeatedly verify 10–15 second default, accurate fill circle, release-to-cancel, no instant completion, concurrent actor rejection, bag preservation, interruption and full contents transfer.
- **Assessment persistence:** repeated A→B→A switching; unchanged vs changed domain; unchanged airway but changed unrelated condition; respawn/death, JIP, different viewer roles and life identity, server updates and save/load when supported.

**A test loop is not a pass simply because it iterated 200 times.** Each iteration must establish meaningful state, trigger the real code path and assert its actual effect. Count test attempts, successful completions, expected rejections, unexpected failures and blocked runs separately. Include failing seeds and exact reproduction steps.

### C3 — Environment and state matrix

- Native Workbench runtime; actual packaged mod; clean installation; dedicated server with multiple real clients; reconnect and join-in-progress; mixed latency and packet loss when feasible.
- Self and other players; player treating AI; conscious, unconscious and dead casualties; multiple responders; role assignment/revocation, GM controls and reduced permissions.
- Flat/uneven ground, roads, slopes, interiors, rocks/vegetation, vehicle proximity, obstacles and terrain boundaries; crouched movement, changes of stance and moving actors.
- Empty, minimally loaded, standard, heavily equipped and deeply nested inventory; gear from US, USSR and FIA as applicable; bags with clothing, weapons, explosives, optics, tools and unusual containers.
- UI 16:9 and ultrawide resolutions, supported DPI settings, tap/hold bindings, relevant first/third-person presentation and controller interaction.
- Live GM edits while procedures run, supply depletion, simultaneous treatments, cancellation races, disconnect mid-treatment/dragging, death, deleted entity, stale patient refs, server restart and supported persistence.
- Extended stress/soak for hours of gameplay, repeated corpse conversions, reequipping, death and respawn, monitoring FPS, memory, replication, event subscriptions, errors and stuck actions.

### C4 — Validation tiers (all must be separately reported)

1. **Static audit:** compilation, code review, GUIDs, asset links, lint, schema generation and config sanity.
2. **Automated state tests:** clinical state machines, inventory conservation, treatment lease, timing, settings round-trip and concurrency.
3. **Actual native gameplay:** in-engine movements, menu, treatment, icons, equip/hand alignments, body-bag ground behavior and physical dragging.
4. **Packed dedicated multiplayer:** actual clients, authority, inventory transfer, patient state, reconnect/JIP, race conditions and persistence.
5. **Human visual acceptance:** icon readability, model scale, body/rope pose, AED/chest seal/injector orientation, progress feedback, player preview and animation/contact. **Codex cannot declare visual perfection without inspection.**

Never mark tests PASSED because a scripted fixture ran in isolation while Steam platform initialization, packed multiplayer or human visual checks were unavailable. Label separately as BLOCKED, PARTIAL or UNVERIFIED.

## D. Test evidence and deliverables

Codex must supply:

1. **Issue ledger BT01-001 to BT01-021**: severity, reproducibility, root cause, files/commit, fix, proof, final status (OPEN / FIXED-PENDING-TEST / VERIFIED / BLOCKED).
2. **Additional issues** discovered during whole-mod inspection, with new stable IDs and equivalent tests.
3. **Every-item matrix**: icon, orientation, scale, packing volume, hand/pose, equip state, visual screenshot and result.
4. **Every-setting ledger**: real canonical setting key, expected effect, default/range, number of cycles, boundary/cross-client/restart tests, failures and concrete supporting logs.
5. **Treatment matrix**: all interventions, repetition counts, success/cancel/rejection, progress/timing, kit consumption, output vitals and replica consistency.
6. **Full inventory reconciliation artifacts** for body bags, including the named missing grenades, smokes, binoculars, watch, demolition blocks and **all clothing**.
7. **Multiplayer/soak report**: game versions, branch and commit SHA, run environment, player count, tests, timings, resource usage, crashes, client/server logs, JIP and unresolved limitations.
8. **Human acceptance checklist**: actual visual examples and game footage/screenshots; do not fabricate or replace human sign-off with static assertions.
9. **Release gate summary** with factual PASS/FAIL/BLOCKED and a no-go recommendation while blockers remain.

## E. Work phases and stop-ship rules

**Phase 1:** Baseline build, test tooling, full source audit, reproduce all issues and write root-cause hypotheses.
**Phase 2:** Fix critical bag equipment preservation, ordinary bag dragging and rope dragging.
**Phase 3:** Fix medical-menu auto-cancel, oxygen workflow, timed procedure feedback and progress.
**Phase 4:** Correct all equipment sizes, packing, icon framing, injector poses, duplicate BVM mask, bag colors and arsenal preview disappearance.
**Phase 5:** Correct hold-F body bagging, central actions, ground placement and per-patient assessment caching/invalidation.
**Phase 6:** Execute full automated, in-engine, packed-dedicated, settings, repetition, stress and persistence regression campaign; investigate every failure.
**Phase 7:** Run human group playtest and capture final go/no-go sign-off.

**STOP SHIP:** Any loss/duplication of corpse equipment; broken manual/rope body-bag drag; unexplained treatment cancellation; disappearing arsenal character; client/server state divergence; critical crashes; nonfunctional settings; or missing reliable packed/server and human verification. Do not equate a high count of synthetic passes with real-world readiness.

**Quality bar:** Make BCM Medical reliable and polished in *actual gameplay*. Every reported and newly discovered flaw remains open until evidence demonstrates its fix. If a verification action is impossible in the available environment, state that fact explicitly and leave it pending.
