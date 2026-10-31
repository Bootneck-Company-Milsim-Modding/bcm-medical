# BCM Medical

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](Docs/PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](Docs/SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.




Standalone Arma Reforger development project. Workbench GUID `6AB4B3F3FD215919`, displayed title BCM Medical, existing folder `MCB Medical`. The base game is the only runtime addon dependency. This remains an unfinished full-mod replacement; no ACE/KAT/NIRE implementation is bundled.



## Current player workflow



**H** opens the medical interface. Rebind or reset it under Settings > Controls > BCM Medical > Medical interface. Its Advanced binding options provide Tap to open and Hold to open. These preferences use the native personal input profile.



The compact panel uses the supplied company logo and anatomy image. H opens the person directly under your aim, including conscious patients; the top Self / Swap control alternates between that patient and you. The five categories are Circulation, Airway, Surgery, Medication and Fluids. Category and body-region selections stay selected while changing the other. Status starts blank until examination. Unnecessary or unavailable treatments remain disabled, and examinations and procedures have subtle server-timed progress bars. Current patient shows the character's name. MORE PATIENTS is a heading over a scrollable list of other occupants of your vehicle/helicopter; it does not search a nearby radius. Morphine, naloxone, epinephrine, TXA, paracetamol and fentanyl have original finite-supply effects with configurable onset, clearance, repeat policy and stacking limits. Saline, plasma and whole blood have distinct composition. Portable oxygen has a retained finite source and an applied mask; see [oxygen and expanded medications](Docs/OXYGEN-AND-EXPANDED-MEDICATIONS.md).



Bandaging equips an actual native field dressing, plays its native self/other/revive item-use animation, and lets native completion consume the original item and treat the selected hit-zone group. Surgery, suturing and splinting use original authority-side timed operations. Other ground/prone/unconscious patients use original BCM enter/loop/exit animation prototypes; self and unsupported standing poses use native item-use presentation. Injectable medication uses the native injection animation. Standing self-use and handover tablets use original BCM entry/loop/exit graphs. Unsupported poses use native handling; final pill/pack props remain dependent on replacement art. A successful native animation and the configured minimum treatment time are both required before the server consumes the reserved kit and applies treatment; cancellation retains the kit. Extraction does not close a wound; retained projectiles and uncontrolled bleeding block suturing. Critical surgery obeys facility restrictions. Definitive fracture repair always requires a medical facility.



Fresh default times: **splint 30 seconds; surgery 60 seconds; sutures 60 seconds; bandage 10 seconds**. All are saved GM controls. Existing saved custom values are retained during migration.



## Implemented foundations



- Original wound/retained-projectile/dressing-wear/suture/internal-compartment, airway, chest and cardiac-rhythm models; deterministic oxygen/perfusion/arrest/CPR behavior. Original CPR entry/loop/exit motion is exported and integrated, with native owner/observer/late-join skeletal playback and cancellation acceptance; movement-key cancellation and the revised overlapping palm-down hands / wider knees have been accepted in game.

- Native character blood depletion/grace, consciousness, measured-fluid rescue, persistent/reopening external wounds, internal blood loss and fracture movement/jump restrictions. Native external DOT remains the single writer for external blood loss.

- Named searchable body bags; authority bagging reservations, real equipment transfer/rollback and folded-bag consumption. Eligible native-dropped recorded weapons can be recovered without sweeping unrelated or newly owned equipment.

- Bag-centred cargo discovery for ambulances, helicopters, trucks and jeeps using native cargo inventory. Equipment moves bag -> vehicle; unloading leaves it in the vehicle. No seat placement.

- Manual and rope-kit body-bag dragging with original server physics and replicated ownership/release. Bags cannot be carried. The stretched rope/grip animation and casualty carry/drag implementation remain unfinished.

- Original life ledger and native spawn/death adapter with native saved-world account persistence accepted locally using diagnostic identities; live backend authentication/reconnect and complete Limited Lives administration acceptance remain unfinished. Body-bag respawn restrictions, requests, spectator and admin menu have been removed.

- BCM-owned suppression/focus integration; live near-miss/optic visual acceptance remains unfinished.



## Game Master and persistence



There are **271 settings in schema 34**, in **one BCM Medical tab with fifteen inline section headings and separator rules**: Bleeding, Airway and breathing, Circulation, Resuscitation, Surgery, Fractures, Medical interactions, Body bags, Transport, Suppression, Lives, Focus, Pain, Medication and Arsenal.



Every setting has its own descriptive hover text. Inline headings now have verified rendered text and dimensions, rather than empty spacing.

The reusable pulse oximeter starts through a timed hand-region equipment action. Live pulse and SpO2 then update in Circulation for other permitted viewers. The operator must retain the exact device and stay near the patient; device loss, invalid access or leaving the configured operating range stops monitoring. Nearby clients play an original pulse-rate beep with configurable sound/range. This is currently a handheld-monitor workflow, not a persistent physical finger clip.

The anatomy diagram uses replicated region markers for fracture (F), splint (S), dressing (B), suturing (ST) and native tourniquet state (TQ). Hover text expands the markers into words. A splint preserves the underlying fracture marker. An independent BCM tourniquet-item procedure is still outstanding.

TXA is a separate injectable inventory item. By default it requires established vascular access, an injury and a completed 15-second administration. Onset, clearance, repeat policy, active-dose cap, medic restriction and clot-wear reduction are GM controls. The current original model stabilises dressed wounds against movement-related reopening; it does not replace blood, remove projectiles, suture wounds or repair internal sources. This is not yet a full coagulation model.

GM right-click character actions **Make medic** and **Remove medic role** assign a replicated character role. Medics take 40% less time by default (15 seconds becomes 9), configurable under Medical interactions. Separate optional restrictions cover surgery, AED use, IV/fluids, advanced airway procedures and injectable medication; defaults preserve unrestricted treatment access. Server checks revalidate the role during advanced procedures. The role belongs to the current character; future loadout code may call the authority-only `BCMM_SetMedic` API. The character medic role also survives native clinical world saves; a separate per-account role/loadout database is not implemented.

Server values are stored in `$profile:BCM_Medical/settings-a.json` and `settings-b.json`. Reuse the same persistent server profile across restarts. Every update is validated and reread before application; a damaged latest generation can fall back to the previous valid generation. Invalid existing files are preserved and edits locked. Current schema33 validates and migrates supported earlier named-key settings through schema32. Native saved-world life balances use a separate serializer; a settings file alone does not restore them.



## Development boundaries



The supplied artwork is imported into native UI textures. Surgery/suture/splint supplies and the stretcher now use the supplied runtime models; see the asset inventory for remaining presentation work. Final applied-item visuals and carrier hand contact, further procedure animation polish, expanded clinical workflows, complete medical AI, supply distribution, burial/disposition and full release QA remain outstanding.



Physical keyboard/mouse and screenshot review of the current panel has now succeeded; the earlier app-access timeout is superseded. Native binding API changes, widget instantiation, all seven region handlers and the actual owner treatment RPC/native animation path have dedicated acceptance coverage.



See [clinical interface](Docs/CLINICAL-INTERFACE.md), [controls and supplies](Docs/PLAYER-CONTROLS-AND-SUPPLIES.md), [validation](VALIDATION.md), [body-bag cargo](Docs/BODY-BAG-CARGO.md) and [completion ledger](Docs/ROADMAP.md). Git publishing remains deferred at the user's request.

Supplied model inventory, provenance and import status are recorded in `Docs/MEDICAL-ASSET-SOURCE-INVENTORY.json` and `Docs/SUPPLIED-MEDICAL-ASSETS.md`. Independent Naloxone/NCD/TXA albedo files can be edited under their `Assets/Medical/<item>/Data` folders. `Tools/AssetImport/Build-InjectorVariants.py` preserves those edits and packs the original roughness channel; the neutral printed body retains its original material while edited colour is applied to the original green end pieces.

Settings survive restart with the same persistent server profile. Native saved-world restart now restores finite item contents, diagnostic-account life balances, patient wounds/medication/equipment, named physical remains, graves, stationary stretcher references and vehicle cargo. Real unload/reload and vehicle destruction survive a further restart without resurrecting disposed gear. These checks use the native persistence-enabled mission and server profile; settings persistence alone does not restore a world. Saved-world owner/proxy/late-join observations and an unavailable-reference timeout/save/restart now pass locally. Live authenticated reconnect and broader contention/corrupt-reference acceptance remain unfinished. See [native persistence validation](Docs/NATIVE-PERSISTENCE-DESIGN.md).


## Shelved two-player stretcher prototype

Two-player carrying is shelved at the user's request and disabled by default; its grip pose remains diagnostic-only. Existing saved settings are preserved. The following describes the retained prototype, not an approved final carrying presentation.

FieldSurgeryStation uses the supplied stretcher model. Look at it to place a nearby unconscious casualty, remove the casualty, take the front or rear handles, or set it down. Two different connected players must join opposite ends. Lower weapons and approach the end handles; walk together. A single player can reserve an end while waiting, but cannot lift it alone.

Transport is server-authoritative, with replicated reservations, transforms and casualty anchoring. Jumping/climbing is blocked and movement is capped while carrying. Loss of a carrier, excessive separation/speed, unsupported terrain, blocked sweeps or a live GM disable causes set-down. Inventory remains on the actual casualty. Full canvas collision is restored when placed down; carried motion uses a small central collision shape plus full-length world sweeps. The casualty retains normal character physics and medical state.

The stationary stretcher remains a medical facility; a raised stretcher does not enable facility-only surgery. Optional stability treats the casualty as resting for movement strain and reduces dressing wear. It does not heal fractures, suture wounds, replace lost blood, stop uncontrolled/internal bleeding or treat hypoxia/arrest.

Transport controls remain under the existing Transport heading in the single BCM Medical GM tab. Current schema 33 migrates earlier named-key settings through schema 32. Final carrying/contact art is shelved; stationary casualty support remains separate.


## Connected clinical safety and training

Pulse now links volume loss, perceived pain, medication and late decompensation to pumping efficiency, pressure and oxygen delivery. Sustained critical pressure or compromised extreme rates can lead to cause-dependent arrest. An ordinary single morphine dose no longer automatically creates severe hypoxia when ventilation remains adequate. Medication and fluid safeguards run on the authority before and throughout treatment. TXA supports clot stability without automatically closing external wounds; fully sutured/repaired injuries do not remain a false indication.

Native passive blood regeneration is disabled by default and can be enabled in GM settings. A successful dressing therefore does not itself refill reserve. Native/legacy saline and morphine effect adapters share the BCM policy; further complete vanilla item UI/consumption and third-party compatibility testing remains on the release ledger.

See [clinical integration audit](Docs/CLINICAL-INTEGRATION-AUDIT.md) for research and deliberate model limits, and [training manual draft](Docs/TRAINING-MANUAL-DRAFT.md) for the current treatment sequence. Typed saline/plasma/whole-blood composition now has model and full default-duration native item acceptance. The dedicated native owner/proxy/late-join gate now passes, including real Fluids-tab requests and exact finite bags; see VALIDATION.md. Additional drugs, continuous delivery/overload and full clinical/world/account persistence remain unfinished.

## Clinical acceptance runner

Run `Tools/Validate-ClinicalAcceptance.ps1` from PowerShell for resource validation, clinical safety, dedicated oxygen/medication, fluid and applied-device owner-proxy-late-join checks, native bleeding, separate-process settings restart and the 720p medical menu smoke test. Add `-FullProcedures` for full default-time oxygen/medications, bags, field procedures, twelve-equipment integration and native medication/TXA acceptance. The runner writes a per-step report and source fingerprints under `.checks/clinical-acceptance-*` and fails if scripts, prefabs, configs or layouts change during the run. It uses isolated diagnostic profiles and the existing local dedicated installation.

This is a reproducible local development gate. It does not replace Internet latency/load testing, visual review, complete world/account persistence or final release acceptance.

## Native faction medical supplies and presentation

The existing US, USSR and FIA inventory catalogues now offer 28 medical inventory items under the native **BCM Medical** heading. Each item price, faction availability and refund fraction is editable in the same GM tab. Live settings refresh native lists/caches; the authority also enforces the medical filter on direct native purchase requests. Weapons and ordinary equipment remain in their native categories. The stationary stretcher is a world object and is excluded from pocket inventory. See [arsenal integration](Docs/ARSENAL-INTEGRATION.md).

The oxygen mask uses head/facial landmarks, with saved position, rotation and scale controls. Original standing tablet self-use and handover clips have native export and local playback checks; final pill/pack props and the separate oxygen cylinder are pending supplied assets. The corrected self-use right palm faces the mouth. See [asset purchase brief](Docs/ASSET-PURCHASE-BRIEF.md). Broader release work remains in the completion ledger; this is not a release-ready claim.



Limited Lives now has native GM character-context grant/remove/reset controls with connected RPC, privilege-revocation and saved-world restart acceptance. See [administration and limits](Docs/LIMITED-LIVES-ADMINISTRATION.md). Backend authentication and offline-account administration remain separate.


Gradual recovery of stabilised red-cell/clotting reserves is available as an opt-in GM policy; it never restores missing circulating volume. See [COMPONENT-RECOVERY.md](Docs/COMPONENT-RECOVERY.md) for controls and accepted native runtime/restart evidence.

Independent finite patient-owned drips are described in [the fluid-line guide](Docs/INDEPENDENT-FLUID-LINES.md). Native inventory/flow, active-world restart and owner/proxy/late-join acceptance are recorded there; expanded edge cases and combined regression remain in progress.


Native stock morphine/saline commit safety now retains the original item when live BCM policy rejects completion. Actual native injector rejection, same-item reuse and owning-client/dedicated behavior are covered separately from human inventory input; see [native stock consumables](Docs/NATIVE-STOCK-CONSUMABLES.md).


Native stock saline preserves its original source and remaining simulation/effect time across saved worlds. Source loss or FullHeal stops tracked delivery; retrieval and reapplication retain the used physical source's existing deadline. Scope and evidence: [native saline continuity](Docs/NATIVE-STOCK-SALINE.md).

Qualified NPC group members can autonomously select in-reach NPC leg fractures and use their own original splint through the shared timed procedure. Qualification, supply ownership, contention, cancellation and actual player takeover have focused native checks; broader care, approach, threat/load and saved-world AI remain in development; see [medical AI](Docs/NATIVE-MEDICAL-AI.md).
