# Player-focused implementation — historical recording checkpoint

Current status: see [RESUMED-DEVELOPMENT-20261030.md](RESUMED-DEVELOPMENT-20261030.md). Recording has finished; the checkpoint below records the earlier recording-safe state.

Historical checkpoint, 30 October 2026. User authorized player-focused development including autonomous medical AI removal while recording Farming Simulator. Keep Arma, Workbench, Blender, native validation and packing closed until recording is finished. This source checkpoint is uncompiled and has no new runtime acceptance. Do not publish or call it player-test ready.

## Changes made

- Removed BCMM_ZMedicalAI.c from active production: no BCM autonomous medic scheduler, fracture-aid AI behavior, weapon-lowering action or added physiology-tick override remains. Original physiology and player treatment of NPC casualties remain in the medical source. Their continued operation still requires the next native regression.
- Retired the four dependent autonomous-AI runtime fixtures and Validate-MedicalAI.ps1. Removed the launcher parameters, runtime dispatch and combined acceptance gate that referenced them. Original files were moved into .checks/player-scope-20261030-recording/retired-autonomous-ai with SHA256 checks; earlier results remain historical evidence, not player-build acceptance. The failed AI threat test was retired with the feature, not fixed or relabelled passed.
- Added fieldProcedureProgressOnly, enabled by default, under Medical interactions. Splint, surgery and suture kit procedures use the existing full server timer, progress notifications, exact-source reservation, permissions and cancellation without starting custom treatment clips. Presentation is selected once at procedure start; GM edits affect later procedures. Turning the control off selects the retained older presentation. Native bandaging, injection and unrelated existing presentation stay separate. No animation source, graph or compiled motion was edited or rebuilt.
- Added a small owner-only progress HUD while a shared procedure is active and the medical menu is closed. It does not capture input or apply treatment. The medical menu retains progress and Cancel treatment. Dedicated/headless actors do not create this widget. Visual/input checks remain pending.
- Appended one setting: 271 settings/schema34, preserving the first 270 keys in order. Schema33 imports require their original complete 270-key set and append the new default; schema34 writes/reads the complete new catalogue. Existing schemas remain supported. New native fixtures cover migration, corrupt/truncated data and resource loading.
- Extended the existing original BCM consent model to bind carry versus drag, reject replacement of a pending offer, permit authenticated requester/patient cancellation and reject replay. Consent is required only for a conscious player. This policy is not yet connected to medical-menu request buttons or live casualty movement.

## Confirmed transport direction

BCM uses ACE only as a behavioral/engineering reference. Do not copy/adapt ACE code, helpers, prefabs, animation assets or other runtime content; add no ACE dependency. Implement our own attachment/ownership/release code against native engine APIs. There is no plan to obtain GPL clearance in order to import ACE material.

Living casualty carry is required, including a conscious player with broken legs after acceptance. Unconscious casualties can be picked up/dragged without any consent prompt. A conscious player's request/accept/decline/cancel UI belongs in the medical interface, with incoming requests taking priority over the selected examination target. Invalid target/range disables the request button with a reason. A closed panel gets a small notification, not forced acceptance. The server must bind each request to the exact actor, patient character/life and carry/drag action and revalidate before pickup. Actor or patient state changes must invalidate stale grants.

Living casualty drag uses ordinary crouch locomotion with reduced speed and free directional movement/turning. No standing, sprinting, jump/vault or new grabbing animation. Casualty pose, collision, replication and safe release require native verification; normal rescuer walking alone does not establish a valid casualty pose. No custom animation work is authorized. Missing suitable native presentation must be raised as a concrete blocker before any focused animation exception.

Two-player stretcher carrying stays shelved. If resumed, use hip-height stretcher placement and normal base-game walking, with no paired/grip animation creation. Stationary stretcher care remains separate. Body bags use existing manual/rope dragging only; filled-bag carrying is outside scope.

## Still to implement

1. Medical-menu transport controls, incoming-request notification/RPC ownership, exact-character consent lifetime, cancellation/timeout and state-change invalidation.
2. Original living carry/drag movement, collision/stance/speed enforcement, original-casualty state/inventory preservation and safe drop/disconnect/deletion/vehicle handling. Never manufacture unconsciousness to enable carrying a conscious player.
3. BCM sling item and original-weapon unconscious retention. Proposed inventory-sling requirement and optional GM normal-drop/sling-required/always-retain policy remain the design direction. Define supported long-gun classes, pistol/launcher policy and one-sling/one-weapon accounting before broadening it. Do not clone a weapon or copy the separate keep-weapons mod. Inspection found native post-drop notifications but has not established a safe pre-drop implementation; runtime ownership/order testing is necessary. No sling/weapon-retention implementation is claimed at this checkpoint.
4. Further suitable-native-or-progress fallback coverage for other procedures as required; existing CPR/tablet assets and animation integration have not been rewritten in this tranche.

## Verification completed without launching native tools

Static PowerShell parsing passes. Resource GUID audit passes with 346 unique owned resources and no duplicate/stale owned references. Catalogue and generated GM list match at 271 unique keys and 15 headings; defaults are within range/step. No retired autonomous-AI symbols remain in active Scripts, Tests/Runtime or Tools. Before/after fingerprints confirm no animation assets changed. Details are in .checks/player-scope-20261030-recording/static-report.json.

These checks do not compile Enforce Script, render the new HUD, prove native treatment completion or establish that players can still treat NPC casualties. Updated native test code has been prepared, not executed.

## Next acceptance after recording

Start with native script/resource validation, schema33-to34 migration/restart, player/NPC clinical regression, and default full-duration progress-only field cancellation/completion. Check exact original-kit retention/consumption and no custom graph playback, owner progress while menu closed, menu progress/cancel, DPI/controller operation, disconnect/character replacement and world restart. Check both new default and retained legacy presentation modes. Only then extend transport/retention integration against a known compiling baseline. Re-run the full player clinical suite and produce a matching packed-only candidate before dedicated-server human testing. Removing the former AI gate does not make the old all49 evidence current.

Native distribution/join, authenticated Limited Lives/reconnect, Internet/load and human testing remain open. Git and publication stay deferred.
