# Bespoke animation source brief

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.




No purchase is needed for the current native animation placeholders. A portable Blender authoring runtime is available; the installed Reforger Tools include EBT-ArmaReforger-Data.zip. The official character rig and working native export setup are available; final contact/pose refinement and the remaining clip sets still need in-game visual review.



Bohemia's [current Enfusion Blender Tools installation guide](https://community.bistudio.com/wiki/Arma_Reforger:Enfusion_Blender_Tools?useskin=vector) specifies Blender LTS 4.5 and the Workbench net API. Its [animation export guide](https://community.bistudio.com/wiki/Arma_Reforger:Enfusion_Blender_Tools:_Import/Export_Animation) describes Blender actions exported to TXA and compiled to ANM, with a matching skeleton export profile. Workbench assembles and compiles the animation graph; it does not generate the missing artist-authored body motion.



Request editable .blend/FBX sources, character-compatible skeleton/rig, textures if props are included, and permission to modify and redistribute compiled assets in a publicly downloadable mod. Do not commission clips tied solely to an ACE/KAT/NIRE runtime or bundle source assets licensed only for private renders.



| Priority | Clip set | Required behavior |

| --- | --- | --- |

| 1 | CPR enter / compress loop / exit | Kneeling by supine casualty; hands stay aligned; cancellable loop; no real medical timing baked into clip |

| 1 | Casualty carry paired lift / hold / locomotion / lower / emergency release | Carrier and casualty alignment, slopes, stairs, stance limits and multiplayer safe release; conscious consent uses the same physical action |

| 1 | Casualty and bag drag enter / loop / release | Drag grip and backward locomotion; bag is never shoulder carried |

| 2 | Splint application | Inspect/position/wrap loop for left/right legs, self/other variants where practical |

| 2 | Surgery and suturing | Kneeling instrument-work loops with separate entry/exit; variable GM duration; avoid dramatic clips with unavoidable long root movement |

| 2 | Chest seal / decompression / airway / BVM / AED pads | Short entry, repeatable working loop, clear completion event and interruption path |

| 3 | Rope attachment / removal | Short hands-to-bag action; towing itself keeps normal locomotion, weapon use and crouch |



For each deliverable include first/third-person needs, hands/prop alignment references, in-place locomotion or documented root motion, loop seam, interruption poses, completion event, and licence provenance. Pairing and native collision/ownership must be tested with the actual game skeleton, not inferred from a rendered video. Native injection and bandage clips can continue covering development while these sources are produced.



## Current portable authoring setup



A SHA-256-verified official portable Blender 4.5.9 LTS build is available outside the addon in the task's animation-tools directory. Bohemia's official Character_AnimationRig_Example.blend has been downloaded and inspected with embedded scripts disabled. Original CPR entry, loop and exit clips now exist in Source/Animations as editable Blender source, TXA sources and compiled ANM resources. The CPR prototype is runtime-integrated through original BCM graph orchestration and the native replicated loiter system.



The official rig is from https://github.com/BohemiaInteractive/Arma-Reforger-Samples/tree/main/SampleMod_NewCharacter/Assets/Characters/SampleCharacter . The repository declares the Arma Public Licence; attribution, noncommercial and Arma-only conditions apply to that source material and any relevant derivatives. Its licence is retained beside the development rig. This is a native Bohemia asset source, not ACE/KAT/NIRE code. The downloaded original rig and Blender binaries remain outside the addon. The derivative source blockout is retained inside Source/Animations with attribution and the sample licence; its compiled motion derivatives are now runtime content.



Original CPR source blockout now exists in Source/Animations/BCMM_CPR_Blockout.blend, generated reproducibly by Tools/AnimationAuthoring/Build-CPR-Blockout.py. The 19-frame/30 fps cycle is original BCM motion on the attributed Bohemia sample rig (APL; full licence alongside the source). Its rendered pose was inspected. Export, native graph integration and multiplayer playback are now accepted. Hand orientation, finger interpenetration, casualty contact and ground alignment remain prototype limitations; this is not final clinical art.





Source update: entry and exit blockouts each have 31 frames at 30 fps. Native-skeleton sampling verified the loop seam (zero matrix error) and both joins (under 0.000002 maximum matrix component error), with finite transforms and actual hand movement. Those measurements validate continuity, not medical accuracy; runtime acceptance is recorded below. Tools/AnimationAuthoring/Stage-CPR-Export.py uses the installed Bohemia EBT sampler; --workbench requires a live local API and the actual native export profile before writing a TXA. The local profile-only staging report is not an exported animation.





## Native CPR prototype acceptance — 26 October 2026



The user enabled the Workbench net API. The official EBT full-body export profile accepted all three TXAs; Workbench compiled all three ANMs. The original BCM graph and instance assign only these original motions. The graph references the base-game `Officier_Mission01.ast` slot schema as a native data contract, not its speech animations; the only runtime addon dependency remains the base game. The unused failed custom slot-template experiment was removed.



CPR reserves the patient during the existing configurable procedure preparation window. Authoritative compressions begin only after the native animation is playing. Stop/cancel releases the exact command with a finite exit grace; delayed cleanup cannot end an unrelated later interaction. Stationary physics velocity is not used as a false movement signal. Loss of reach, actual horizontal displacement, life/weapon state, lost animation, lease expiry and explicit stop invalidate the session.



- `.checks/20261026-225628-125`: 303 assertions, zero failures; native bones prove entry into kneeling pose, repeating hand travel, authored exit motion and final release.

- `.checks/animation-network-20261026-225304-573`: 298 server assertions, zero failures; actual owning client, observer and a client joining during CPR each observe the native skeletal loop and release. A second CPR session is canceled by actual owning-client displacement beyond reach; authoritative compression and playback both stop.

- `.checks/20261026-225639-662`: 324 assertions, zero failures; dedicated native medical regression after the CPR integration.



The displacement fixture does not simulate physical keyboard input. The dedicated fixture deliberately exercises invalid settings and emits the expected protected-file diagnostic; this is not an assertion failure. Bespoke surgery, suturing, splint, airway/chest and casualty transport clips remain outstanding. Native item animations continue covering those timed treatment operations. No asset purchase is needed to continue this prototype work.



Superseding status, 27 October: the user confirmed live movement cancellation and approved the revised CPR hand/knee pose. Full native default-time surgery/suture/splint clip checks are recorded in VALIDATION.md. Two-player stretcher grip sources were authored and native contact metrics tested, but the user found the carrying presentation awkward and explicitly shelved that work; do not treat it as final approved animation.


Current native item adapter: equipment and injector effects initialise their native consumable categories separately. They only report animation completion; the server procedure still owns eligibility, timer and exact supply consumption. Temporary animation-readiness trace experiments were removed after inspecting the installed native API setup. The dedicated fixture now transfers real character replication ownership and keeps observer bodies out of the treatment line of sight. Full default-duration and multiplayer evidence is in VALIDATION.md.

Final current-build field-loop regression: `.checks/20261027-220219-675`, 462 assertions with zero failures. Full default splint/extraction/suture timers, midpoint graph activity, exact reserved-kit cancellation and final clinical commits pass with unchanged source. This is functional loop acceptance; final instrument/contact art remains separate.


## Tablet self-use and handover - 28 October 2026

Original standing tablet entry/loop/exit clips are authored in `Source/Animations/BCMM_Tablets.blend` using the attributed native sample skeleton. Export uses the official installed Enfusion Blender tools. Runtime graphs contain only BCM-authored motion and native slot contracts. Self-use turns the right palm inward toward the mouth, following the user correction; the left hand presents the packet area. Treating another conscious standing character uses a preparation/handover gesture. The recipient is not forcibly animated. Unsupported poses retain the native treatment adapter.

Native playback was visually inspected after compilation. `.checks/20261028-090405-661` passed 566 checks including actual self and other tablet graphs, exact finite consumption and oxygen/medication procedures. `.checks/oxygen-medication-network-20261028-094909-777` passed 554 server checks and all thirteen phases on owner, observer and active-session late joiner. Each viewer observed both tablet graphs; owner displacement canceled the second self-treatment with its original kit retained and no medication committed. This displacement test does not simulate physical keyboard input. A subsequent unchanged-source regression is tracked separately.

Final visible pill/packet props and contact/clothing review remain open; paracetamol inventory currently uses a supplied pouch placeholder. The current motion is a functional prototype, not a claim of professional motion-capture quality. `Build-Tablets.py` and the tablet manifest support reproducible authoring/export. `BCMM_BuildAnimationsPlugin` registers and rebuilds all owned TXA and workspace resources, rather than a temporary tablet-only subset.

Final current-source regression: `.checks/clinical-acceptance-20261028-095514-670/report.json`, all nine gates passed with source unchanged, including animation fingerprints and explicit replicated nondefault setting prerequisites.
