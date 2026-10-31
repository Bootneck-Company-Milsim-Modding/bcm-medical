# BCM Medical controls and current supplies

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


Default keyboard binding: **H**. Settings > Controls > BCM Medical > Medical interface lets each player bind another key or reset to H. In that binding's Advanced options choose **Tap to open** or **Hold to open**. These personal bindings are saved by the native input system; a Game Master does not set another player's keyboard preference.

H opens the directly looked-at living character, including conscious characters, within examination range; otherwise it opens self. Self / Swap alternates self and the looked-at patient; the patient list contains actual occupants of the same vehicle. The title identifies the selected player by name. The server independently authorizes self/shared-vehicle examination. Viewing a vehicle occupant does not grant remote or in-vehicle surgical treatment.

All configurable gameplay controls are now in one Game Master tab named BCM Medical, with inline section headings: Bleeding, Airway and breathing, Circulation, Resuscitation, Surgery, Fractures, Medical interactions, Body bags, Transport, Suppression, Lives, Focus, Pain, Medication and Arsenal. There are 271 saved controls in schema 34. Limited Lives is the sole BCM respawn policy; the separate body-bag respawn controls and menu have been removed. Existing valid profile values are preserved during migration, including previously chosen splint times. Fresh default times are splint 30 seconds, extraction/internal/fracture surgery 60 seconds, suturing 60 seconds. Bandage application defaults to 10 seconds with its own saved GM control. Examination, body bagging and CPR leases retain their separately adjustable controls.

The menu has a subtle progress bar driven by an owner-only server procedure notification, plus Cancel your treatment. Cancellation before consumption retains the reserved kit. Treatments revalidate range, actor movement/control, life state, lowered weapon, original supply ownership and the exact injury every 100 ms. Kit consumption and state changes happen only at server completion. Extraction and suturing are separate consumable procedures. A reopened or actively bleeding wound cannot be sutured, and a retained projectile must be removed first. Head/chest/abdominal surgery obeys the critical-surgery facility setting; definitive fracture repair always requires a facility. GM can assign or revoke the medic role on a character, set the medic time reduction (default 40%), and restrict advanced procedures by role. The authority rechecks role permissions during treatment.

## Placeholder resources

| Resource | Current visual | Final replacement needed |
| --- | --- | --- |
| BCMM_Oxygen.et | Supplied mask representing the portable kit | Separate cylinder and final facial contact/materials |
| BCMM_Paracetamol.et | Supplied pouch with non-injectable handling | Tablet packaging and final prop/finger contact |
| BCMM_Fentanyl.et | Unlabelled supplied injector | Final identification and contact |
| BCMM_Splint.et | Supplied packaged splint | Final attachment/contact polish |
| BCMM_SurgeryKit.et | Supplied medical survival kit | Instrument contact and final treatment presentation |
| BCMM_SutureKit.et | Supplied military medical pouch | Needle/holder contact and applied closure presentation |
| BCMM_FieldSurgeryStation.et | Supplied stretcher; stationary casualty support; two-player transport shelved | Stationary support; carrying shelved, future hip-height placement with native walking |

No purchase is required for the current implementation. The supplied AED/pads, monitor, BVM, cannula, suction, seal, splint and treatment pouches are imported; final donor provenance, transparent plastic/PBR maps and treatment contact still need release review. Original applied splint and tourniquet models and selected donor pad/cannula parts are available. Applied device visibility follows authoritative treatment state; it does not create duplicate usable supplies.

Original compiled treatment loops animate CPR, splinting, suturing and surgery. Native injection/item graphs provide other procedure presentation. Final manual bag-drag grip, casualty carry/drag, visible tow rope and broader instrument contact remain unfinished. Native resources are referenced through the base game; no ACE/KAT implementation or animation assets are bundled. The owned BCM body-bag model remains in use.

Morphine, naloxone, epinephrine, TXA, paracetamol and fentanyl are separate finite inventory items. Standing tablet use has original self/handover clips, with native handling for unsupported poses. On Medication, select Head for tablets and a limb for injections. The stronger opioid defaults to medic-only with IV access and its own vital safeguards. On Airway/Head, connect or disconnect the retained finite oxygen kit. On Airway/Chest, check breathing for the source status, remaining litres and flow. Ongoing oxygen requires the exact kit and configured range; unconsciousness alone does not remove an already connected source. See [oxygen and expanded medications](OXYGEN-AND-EXPANDED-MEDICATIONS.md). Supplied injector cap colours are retained. Saline (`BCMM_Saline.et`), plasma (`BCMM_Plasma.et`) and whole blood (`BCMM_FluidBag.et`) are distinct supplies using the approved shared bag model as a temporary visual. They use separate equipment kinds and cannot silently substitute for one another. See [training draft](TRAINING-MANUAL-DRAFT.md) for effects and safety limits. Native US/USSR/FIA arsenals contain 28 medical inventory entries, and stationary stretcher GM placement is implemented; broader distribution remains unfinished.

## Stretcher status

Two-player carrying is shelved at the user's request and disabled by default. Existing saved values are preserved; the experimental grip presentation remains diagnostic-only. Stationary placement/removal of unconscious casualties and optional resting/dressing support remain separate features. A raised stretcher is not a surgery facility. Conscious casualty consent and stretcher vehicle carriage remain unfinished.

The native US, USSR and FIA arsenals now use the BCM Medical heading and 28 inventory entries. Item prices, availability per faction and refund fraction are under the Arsenal inline heading. The separate oxygen cylinder and final pill/pack contact await purchased assets; see [purchase brief](ASSET-PURCHASE-BRIEF.md).

## Recovering the deceased and waiting to deploy

Use a folded body bag on a deceased character. Successful bagging consumes that kit, transfers the original equipment into the filled bag and removes the physical corpse. Search the bag through normal inventory. Close its inventory before another player drags, loads, buries or sends it home. Filled bags support manual or rope dragging; carrying them is unavailable.

Loading a bag deposits its equipment into the vehicle's fixed cargo. Unloading brings back the bag while the deposited equipment stays in that vehicle. Check the vehicle inventory before leaving. A backpack placed in a vehicle remains a personal container rather than fixed vehicle cargo.

To bury, hold an entrenching tool and place the bag stationary on natural ground. Remain close and eligible through the digging action; the default duration is thirty seconds. Burial creates an inspectable memorial and disposes of equipment still inside the bag. Send Body Home instead requires a bag within an enabled native medical facility's healing area. It also disposes of equipment still inside the bag. Neither operation refunds a consumed deployment life.

Limited Lives is the sole BCM respawn policy. Recovering, burying or sending home a bag does not block deployment, spend another life or refund allowance. There is no separate body-bag respawn countdown, request, spectator option or Body Bag Admin menu. Burial and Send Body Home remain physical recovery actions configured in the single BCM Medical GM tab.

GMs can use Add one deployment life, Remove one deployment life and Reset deployment allowance in the existing native character context menu for a connected player. These require unlimited native Edit authority and preserve the current character/clinical state. Reset cancels pending preparation. They do not restore the removed body-bag menu or affect physical recovery. See [Limited Lives administration](LIMITED-LIVES-ADMINISTRATION.md).


### Game Master stationary treatment stretcher

The normal GM browser now exposes **BCM stationary treatment stretcher** as a medical world object. Place it on valid terrain, then use the normal casualty placement/removal actions during play. It inherits the configured field-treatment facility policy and native world persistence. Two-player carrying remains shelved; folded inventory deployment is not part of this entry. See [GM-STATIONARY-STRETCHER.md](GM-STATIONARY-STRETCHER.md) for controls and acceptance limits.
