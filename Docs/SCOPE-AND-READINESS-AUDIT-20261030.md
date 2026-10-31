# BCM Medical scope and readiness audit — 30 October 2026

Current engineering checkpoint: the agreed player-focused changes passed all 51 validation stages with documented diagnostic-only continuity, 12 packed-only phases and a packed dedicated-server/three-client transport test (701/0). The private package is built. See [PLAYER-TEST-CANDIDATE-20261030.md](PLAYER-TEST-CANDIDATE-20261030.md) for exact evidence, package location and remaining human/Internet/art acceptance. The latest carry revision is not marked visually approved by the user. Earlier checkpoint paragraphs below are historical where superseded.

## Current resumed checkpoint — 30 October 2026

Recording has finished; native development and private packing are authorized. Autonomous medical AI is removed. The original inventory sling, compact bottom-right consent controls, sling retention and living transport are implemented. There are 279 GM settings/schema35. Two original casualty poses are explicitly authorized; rescuer locomotion and all 116 earlier animation files remain unchanged. See [RESUMED-DEVELOPMENT-20261030.md](RESUMED-DEVELOPMENT-20261030.md) for current evidence and [CASUALTY-POSE-REFERENCE-20261030.md](CASUALTY-POSE-REFERENCE-20261030.md) for pose provenance. Current full regression/private packed acceptance remains pending. The earlier planning and recording checkpoints below are historical where superseded by this resumed checkpoint.

## Controlling user decisions

This document supersedes earlier transport, AI and animation planning where they conflict. The original audit was documentation-only; the user subsequently authorized implementation, AI removal, native validation/private packing and the narrow exception for two original casualty poses. See the current candidate report above.

- Autonomous medical AI belongs in the user's separate AI mod. Removal was subsequently authorized and completed in active BCM source. Do not continue expanding it. Preserve player treatment of injured AI and ordinary medical simulation on AI casualties when removing autonomous behavior.
- Two-player stretcher carrying remains shelved. If resumed, use the stretcher at the two players' hip height with normal base-game walking. Do not create paired lift, grip or walking animations. Stationary stretcher placement/treatment remains separate.
- Implement carrying of living casualties, including conscious casualties with broken legs. A conscious casualty must explicitly accept the request before transport starts. Unconscious casualties cannot answer a consent prompt; their transport uses eligibility and authority checks. Rejection/timeout must not move the casualty.
- Implement dragging of living casualties. Preserve injury state and original inventory; transport must not heal a fracture or revive a casualty. Define conscious drag consent consistently with conscious carrying before implementation.
- Filled body bags are dragged manually or with the rope kit. Carrying filled bags is outside scope, rather than a deferred requirement.
- Implement BCM-owned weapon retention when becoming unconscious, without depending on or copying the separate keep-weapons addon. Retain the original weapon, attachments and ammunition; do not clone or replace it. Define terminal-death handling separately from unconscious retention.
- For almost all actions, reuse a suitable base-game animation if one exists. Otherwise show an on-screen progress bar. Do not create, modify, retarget, rebuild or commission animations under the current instruction. If animation is truly indispensable, document the exact blocker and return to the user for a focused decision about native reuse, purchase or creation.
- Existing custom animation assets and runtime integration are left untouched by this documentation update. Their presence is historical implementation, not permission for further animation work. Migrating existing animation-dependent actions to a timer/progress fallback needs later code work and validation.

## Audit findings

Inspection covered the project manifest, settings catalogue and generated GM list, current roadmap/handover, controls/training, transport/body-bag, animation/procurement and validation records, plus relevant medical/AI/transport source paths. This is a project-wide scope and readiness audit, not a fresh line-by-line correctness proof or new runtime acceptance. Historical validation remains tied to its exact source snapshot.

| Area | Current evidence | Remaining work / decision |
| --- | --- | --- |
| Dependency | addon.gproj lists only base game GUID 58D0FB3206B6F859 | No ACE, NIRE, carrying or keep-weapons dependency is declared. Feature gaps are not supplied by that manifest. |
| GM settings | 270 catalogue entries and 270 unique generated GM keys; no missing/unknown keys; 15 headings; documented schema 33 | Some reserved/shelved settings have no active gameplay effect. Personal keybinds remain native player preferences; not GM settings. New features need their own deliberate GM policy and matching persistence checks. |
| Suppression | BCM near-miss visual implementation and GM controls | Live near-miss/optic visual acceptance remains open. |
| No instant death | BCM character death permission and blood-depletion rescue policy | Not immortality or proof against every environmental/native kill path. Keep clinical death/recovery regression in the player build. |
| Limited Lives | BCM ledger, native death/spawn adapter and GM context controls; local persistence/diagnostic identity evidence | Real backend identity, reconnect, offline administration and Internet acceptance remain separate. Physical body disposition does not gate lives. |
| Body bags | Bagging, original equipment recovery, search, cargo, manual/rope dragging, burial and Send Body Home | No filled-bag carry requirement. Keep ownership, rollback and persistence checks. |
| Living casualty carrying | Not a completed available feature; conscious consent timeout setting explicitly reserved | Required, including accepted conscious broken-leg transport and unconscious casualty transport. |
| Living casualty dragging | Not completed; body-bag drag code is not living-casualty drag | Required. Do not advertise body-bag dragging as proof of this feature. |
| Stretcher | Stationary casualty support implemented; two-carrier prototype retained/disabled by default | Carrying shelved; future hip-height placement with ordinary native walking, no custom animation work. |
| Unconscious weapon retention | Equipment tracking remembers nearby dropped items for later body recovery | No confirmed drop-prevention implementation. Required BCM feature; recovery tracking does not satisfy it. |
| Procedures/presentation | On-screen server-driven progress already exists; several operations also require animation readiness/completion | Missing native clips must not block future actions: implement timed progress fallback without bypassing duration, permissions, original-source accounting or cancellation. Existing custom loops are frozen. |
| Autonomous AI | Integrated NPC fracture-aid behavior remains in source | Planned removal awaits user instruction; preserve player medical and treatment of NPC casualties. |

## Exact development checkpoint

- Historical full regression clinical-acceptance-20261030-053005-482 passed all 49 gates with unchanged source at 06:29:17 UTC.
- Its private PC candidate053017-458 contains 424 entries, 65,529,138 bytes. Nine packed-only phases passed; later packed AI064227-646 passed twelve scenarios. These do not accept subsequent source edits.
- Later private weapon tests075626-957 and075700-215 each passed 671 assertions, zero failures, unchanged source for those runs.
- Current-source AI regression medical-ai-20261030-075859-350 is FAILED with SourceUnchanged=true. ThreatAdmission reported: normal patient ticks could queue autonomous care while the qualified supplied medic was threatened. Weapon-lowering logic is now in production BCMM_ZMedicalAI.c; older notes saying no production lowering change are superseded.
- No passing full regression/package has been established for that later source snapshot. Do not distribute the old packet as a validated representation of current source. Do not mark the AI failure fixed through a documentation update.

## Acceptance requirements for the requested work

Living transport must validate actor/patient identity, consent where answerable, reach, obstruction, state and exclusive ownership on the authority. Cover simultaneous requests, release, injury/death changes, vehicle boundaries, controller takeover, disconnect/deletion and safe placement. A broken leg stays broken; inventory and active medical state stay on the original casualty. Observe owner, proxy and late joiner on a dedicated server.

Weapon retention must cover the originally held rifle/pistol, attachments/ammunition, repeated unconscious/wake cycles, inventory contention, disconnect/reconnect and death transition with owner/proxy/late-join checks. No duplication, silent replacement, inventory loss or forced weapon use while unconscious.

Progress-only procedures must show start/progress/cancel/completion, retain the authoritative duration and exact reservation, consume original supplies once at successful completion, and leave supplies/effects unchanged on cancellation. Missing animation callbacks must not deadlock the fallback. Do not weaken medical reach, role, life-state or movement checks to make presentation succeed.

After authorized changes: run appropriate focused checks, a full current-source regression and matching packed-only checks. Establish actual dedicated-server distribution/join and backend-authenticated accounts, then a small human session for medical care, transport, retention, GM settings, reconnect and persistence. Internet/load, clean-install/platform and final provenance remain release work. No invented completion percentage or fixed readiness date is supported.

## Documentation corrections

Older controls text counted 260 settings/schema31; current catalogue/generated GM list are 270/schema33. Earlier animation shopping/authoring plans are superseded by the progress-bar policy. Earlier full-standalone AI requirements are removed from desired BCM scope, while actual code removal remains pending. Historical animation and validation records are retained as evidence, not instructions to resume that work.
