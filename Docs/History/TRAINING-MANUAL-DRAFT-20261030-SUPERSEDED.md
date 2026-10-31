# BCM Medical training manual — development draft

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


Version: 30 October 2026, clinical settings schema33 (270 controls). This draft describes the implemented game mechanics. It must be checked against the final release before community training. It is an Arma Reforger game guide, not real medical instruction. Server settings can change every numerical limit and timing below.

## Open and select a casualty

Default key: **H**. Look directly at a person and press H to open their medical menu, including a conscious person. Use **Self / Swap** to alternate with your own medical state. The patient list is for people sharing your vehicle; it is not a nearby-player radar. Rebind the medical action and choose tap/hold in the available input settings.

Select a highlighted body part, then a treatment category. The selected part remains selected when changing tabs. Opening the menu does not reveal examination findings. Perform a check first, and distinguish a checked snapshot from a live instrument reading.

| Category | Main use |
|---|---|
| Circulation | External bleeding, bandages/tourniquets, pulse, AED and circulation assessment |
| Airway | Head: obstruction/adjunct/suction. Chest: breathing, seals, decompression, drainage and ventilation |
| Surgery | Injury examination, extraction, sutures, internal repair and definitive fracture repair |
| Medication | Pain, medication history and drugs |
| Fluids | Vascular access, saline, plasma and whole blood |

## A systematic game treatment sequence

1. **Control major external bleeding.** Examine the relevant limbs/regions and use the appropriate dressing or tourniquet. Check the other regions too; one treated wound does not control another wound.
2. **Establish an open airway.** A soft-tissue obstruction and foreign material are different problems. An adjunct does not bypass material that needs suction. An open airway is necessary for effective assisted ventilation.
3. **Assess chest and breathing.** A chest seal closes an open chest wound. NCD relieves tension. A chest drain removes pleural collection. These do not perform one another's job, and drainage does not return blood to circulation or repair a vessel.
4. **Assess pulse, pressure and oxygenation.** A rapid weak pulse with falling pressure suggests poor circulation. Use the monitor and pulse oximeter as available. An active oximeter shares updated pulse/SpO2 with medical viewers while its operator retains the exact device within range.
5. **Restore circulation when indicated.** Establish access and use finite fluid reserve while there is a deficit and pressure is below the server stop limit. Fix ongoing loss; otherwise replacement can be lost again.
6. **Treat pain after assessing stability.** Check medication history and wait for onset rather than immediately repeating doses. Keep reassessing breathing and pressure.
7. **Perform definitive repair and evacuate.** Splint fractures, repair internal bleeding at the required facility, and use sutures/extraction according to the game's wound rules. Recheck after movement or a change in condition.

There is no single heal-all drug. Repeated drugs, shocks or fluid bags cannot replace the missing step in this sequence.

## Understand the vital signs

Blood loss lowers effective reserve and pressure. Pulse initially compensates; pain and stimulants can raise it too. Late severe shock or hypoxia can overcome compensation. Very slow pumping or excessively fast filling reduces efficiency. Persistent poor output/pressure can then produce arrest.

A low resting pulse or a brief high reading does not automatically kill a casualty. The game checks severity, pressure and exposure time. During pulseless arrest, the reported pulse is zero; CPR provides partial circulation without pretending that spontaneous pulse has returned.

Oxygen depends on airway, lung function and adequate ventilation. Morphine can slow breathing without immediately causing severe hypoxia; severe/stacked depression can remove effective ventilation. BVM support works only through a patent airway and cannot by itself replenish blood.

## Troubleshooting during a BCM scenario

| Finding in the game | Check next | Relevant treatment and reassessment |
|---|---|---|
| Rapid pulse with falling pressure | All bleeding regions, internal injuries and reserve estimate | Control each source, establish access and select appropriate replacement. Recheck after completion; analgesia alone does not replace lost reserve. |
| Pressure improves after saline but casualty remains critically unwell | Oxygen-carrying game estimate, pulse, airway/chest and medication history | Consider remaining red-cell deficit and other unresolved causes. A normal SpO2 alone is not evidence of adequate oxygen transport. |
| Low saturation or ineffective breathing | Head for obstruction; chest for injury/tension/collection; opioid history | Use the intervention that matches the finding. An oxygen reading does not distinguish all causes; a clear airway is required for BVM. |
| Slow breathing after opioid use | Medication history, pulse and pressure | Address active opioid effects and ventilation. Reassess after naloxone because opioid effects can return as reversal clears. |
| No pulse and AED advises no shock | Reserve, airway/ventilation, tension and ongoing drug instability | Continue the game's resuscitation sequence and correct causes. Repeating an unindicated shock cannot replace these steps. |
| Medication or infusion is unavailable | Stated denial, role, supply, access, current vitals and repeat interval | Correct the stated problem and recheck. Do not assume a disabled/rejected action is fixed by changing body parts or repeatedly clicking. |
| A dressed wound reopens during movement | Dressing status, remaining projectile/closure needs and clot support | Control it again, limit movement and arrange definitive repair. TXA can support clot persistence but does not perform suturing or extraction. |

The immediate evacuation goal is sustained spontaneous circulation, effective ventilation, controlled bleeding and supported injuries, with repeat findings that remain stable. This does not require every displayed number to equal a healthy baseline. Definitive surgical/fracture repair and longer-term recovery are separate from initial stabilisation. Full physiological recovery, additional complications and facility care remain subject to the release roadmap; GM FullHeal is an administrative reset, not a substitute training procedure.

## Medication purpose and limits

| Item | Implemented purpose | What it does not fix |
|---|---|---|
| Morphine | Reduces perceived pain and its handling effects; can reduce ventilation and pressure | Bleeding, fractures, missing blood, obstruction |
| Paracetamol | Slower non-opioid pain relief; requires an awake casualty able to swallow | Injury repair, lost reserve or liver-toxicity simulation |
| Fentanyl | Faster stronger opioid relief; distinct breathing/pressure risks, normally medic-only with IV access | Safe unrestricted redosing, bleeding, obstruction or missing reserve |
| Naloxone | Reverses active opioid effects for a shorter period | Traumatic blood loss or chest injury; opioid effects may return after reversal wears off |
| Epinephrine | Raises simulated rate/pressure while circulating; subject to pulse/pressure safeguards | Missing blood, blocked airway or guaranteed ROSC |
| TXA | Supports clot persistence, slows dressing wear and partly reduces ongoing internal loss | Open external wounds, retained projectiles, unclosed wounds or unrepaired internal sources |

Medication quantities are abstract game units, not real-world doses. Default repeat limits prevent immediate redosing, and active effects have finite onset/clearance. GM settings can tighten or relax availability and role requirements.

Default safety checks withhold morphine below pulse **50**, systolic pressure **90**, breathing rate **10** or SpO2 **90%**, and withhold it during pulseless arrest. Epinephrine with a pulse is withheld at pulse **180 or above** or systolic pressure **180 or above**. The server can restrict it to arrest only. Naloxone normally requires a recorded active opioid. TXA normally requires access and an open/dressed bleeding wound or an unrepaired internal source. A fully closed/repaired injury alone does not qualify.

These are BCM game safeguards. A rejected treatment does not mean the corresponding real-world drug has a universal cutoff at that number.

## Oxygen equipment

On **Airway / Head**, connect the portable oxygen kit. New sources default to **300 L** and **10 L/min**. Connecting takes **10 seconds** and removal **3 seconds**, with the normal medic time modifier. The operator retains the exact kit and must remain within **3 metres**. Reconnecting does not refill it. A usable fuller spare is selected ahead of an empty source. On **Airway / Chest**, check breathing to read source status, remaining litres and flow.

A connected source continues through unconsciousness while retained and within range. Source loss, empty gas, separation, death, patient reset or a live GM disable stop it. Oxygen supports remaining lung function; it does not clear an airway, provide breaths in apnoea, fix tension, restore pumping or replace red cells. BVM remains a separate ventilation action. Reassess breathing, circulation and tissue-delivery findings rather than relying on SpO2 alone.

Tablets are on **Medication / Head**; injectable drugs are on a limb. Paracetamol defaults to a **60-second** onset with slower relief and no opioid pressure/breathing modifier. Fentanyl defaults to a **10-second** analgesic onset; respiratory effects can outlast pain relief. Withhold it by default below pulse **50**, systolic **100**, breathing **12** or saturation **92%**. Its default repeat interval is **120 seconds**, active cap **2**, with medic and IV requirements. Both opioids contribute to depression and both can be reversed by naloxone; repeat assessment is essential after reversal. These are configurable compressed game mechanics, not prescribing rules.

The oxygen cylinder, final mask material/contact polish and tablet packaging remain presentation work. The original standing hand-to-mouth/handover motions are integrated; unsupported poses use native handling. The current pouch and unlabelled injector are identifiable inventory placeholders.

## Fluids

Passive native blood regeneration is **disabled by default**. Stopping bleeding preserves remaining reserve; it does not automatically refill what was lost. GM can enable the base-game regeneration behaviour explicitly.

Each new default BCM bag contains **500 ml** and delivers progressively during a validated **60-second** held procedure. A partial bag takes a proportional remaining interval. Separate independent drip actions are also available. Access must already be established. The default pressure stop is systolic **140**, and the default minimum deficit is **50 ml**. The authority rechecks during treatment and cannot increase reserve above capacity.

| Bag | Volume | Clotting support | Oxygen-carrying reserve |
|---|---|---|---|
| Saline | Increases, then part gradually redistributes | Does not replace lost factors; can dilute remaining support | Does not replace red cells |
| Plasma | Increases, with less redistribution by default | Replenishes the simplified clotting reserve | Does not replace red cells |
| Whole blood | Increases | Replenishes clotting reserve | Replenishes red-cell reserve |

Volume, clotting reserve and red-cell reserve are separate game quantities. Loss of blood removes all proportionally. Restoring pressure with cell-free fluid does not restore red cells. The pulse oximeter reports saturation, not how much haemoglobin is available: a normal SpO2 can coexist with dangerously poor tissue oxygen delivery. Continue circulation assessment and control blood loss; do not treat an improved saturation or pressure alone as recovery.

Diluted clotting support increases remaining internal bleeding and dressing wear; plasma/whole blood can improve that deficit. Neither seals an open vessel or sutures a wound. TXA helps existing clot persistence and does not supply the missing reserve.

By default, saline retains 25% and plasma 80% of added volume after the simplified redistribution phase, with an exponential 300-second time scale. These are tunable game abstractions, not clinical measurements. Water redistribution does not create a wound or remove red cells. The circulation blood-loss check labels numerical reserves as **game estimates**; these are not laboratory measurements or pulse-oximeter findings.

Infusion delivers during the validated native procedure. The Fluids action shows the next bag's remaining millilitres. A full default bag takes sixty seconds; a partial bag takes a proportional remaining interval. Cancelling, moving away or losing access stops further delivery and retains that same partially used bag. Previously delivered fluid remains in the patient, subject to loss and redistribution. Restarting cannot refill the bag. Empty bags are consumed. Live capacity changes affect newly created bags; existing contents remain finite.
**Connect whole blood drip**, **Connect saline drip** and **Connect plasma drip** use the selected access limb. Connection moves the original bag into the patient's inventory and releases the operator. Flow can continue after the operator leaves. **Disconnect fluid line** needs no spare bag and retains the same partially used source in patient inventory. Moving/deleting the source, loss of access, FullHeal/death, safety limits or exhaustion stops delivery; restart awards no offline fluid. An already connected line can remain with a transported patient. This does not authorize new procedures inside a vehicle. See [independent lines](INDEPENDENT-FLUID-LINES.md).

The optional redistributed-fluid burden model defaults **off**. When enabled, redistributed saline/plasma water can impair lung function and trigger the configured water-loading stop. It is separate from pleural bleeding and clears gradually during active perfused game time. Oxygen/ventilation support does not instantly remove it. See [fluid burden](FLUID-BURDEN.md).

Native stock saline bags retain native rate/lifetime behavior rather than the BCM finite-millilitre model above. Their used-source deadline remains attached to the original physical item across supported retrieval/restart paths; FullHeal or reapplication does not provide a fresh lifetime. See [native stock saline](NATIVE-STOCK-SALINE.md).

**Development acceptance:** held fluids, independent lines, optional burden and original-source saved-world cases have scoped native checks in [VALIDATION.md](../VALIDATION.md). The bags currently share the approved bag model as placeholders. Blood-type matching, real transfusion ratios and laboratory chemistry remain outside the implemented model.

## Arrest and recovery

- Start CPR for a pulseless casualty and support ventilation through a clear airway.
- Apply AED pads and analyse. Shock only when advised, stop compressions and stand clear for discharge.
- VF/pulseless VT can be shockable. PEA/asystole are not. Repeating a shock after the rhythm changes is not an alternative to reassessment.
- Correct reversible causes: missing volume/red-cell reserve, obstruction, inadequate ventilation, tension and ongoing drug-related instability.
- A shock changes rhythm; it does not guarantee circulation. ROSC needs corrected causes, CPR and a sustained recovery interval.
- Prolonged inadequate tissue oxygen delivery can make the casualty terminal. A later drug or shock does not undo that state.

Gradual red-cell/clotting recovery is a separate GM option, **disabled by default**. Eligible stabilised care can rebuild those components during active game time; it does not refill circulating blood volume, stop bleeding or revive a casualty. Stationary facility care is required by default. See [component recovery](COMPONENT-RECOVERY.md).

## Wounds, surgery and fractures

A dressing stops the corresponding external flow, but the game's dressing-wear timer can reopen it. Movement, particularly running, accelerates wear; TXA slows it. Reopening normally produces a smaller bleed rather than a brand-new unrelated injury.

The retained-projectile default is **40%**. Extraction and closure are separate procedures. Suturing requires controlled bleeding and removal of a retained projectile. Critical-zone surgery defaults to a medical facility. Definitive fracture repair is facility treatment; a splint only supports the fracture.

Default timings: bandage **10 s**, splint **30 s**, surgery **60 s**, sutures **60 s**, chest seal/NCD **15 s**, access **15 s**, pads **20 s**, AED analysis/discharge **5 s** each. The preparation period is separate. The progress bar follows the active procedure. Moving, losing reach, raising a weapon, losing the facility or becoming ineligible can interrupt treatment.

A medic role normally reduces procedure time by **40%**. GM can assign/revoke the role and restrict advanced treatments to medics. Server configuration overrides these defaults.

## Equipment and transport

Applied models indicate treatment state; their cosmetic toggles do not change clinical effects or return consumed supplies. A pulse clip represents an active retained instrument, not a second usable item. Some art/attachment alignment still needs visual acceptance.

Two-person stretcher carrying is shelved and disabled by default. A stationary stretcher remains a separate support/facility feature. Body bags contain the deceased's equipment; loading into supported vehicle cargo transfers that equipment to the vehicle. Unloading the bag leaves transferred gear in the vehicle.

## Instructor practice scenarios

1. Single external bleed: measure before/after dressing, then check movement-related reopening.
2. Multiple bleeding sites: demonstrate that TXA alone does not close them; control each and reassess reserve.
3. Chest trauma: compare seal, decompression and drainage; explain the remaining internal source.
4. Pain with low pressure: demonstrate morphine rejection and retained supplies, then correct the cause and reassess.
5. Opioid respiratory depression: assess airway, ventilation and reversal; watch for return of effects.
6. Fluids: compare equal-volume saline, plasma and whole blood. Demonstrate improved pressure without restored red cells, and explain why a normal SpO2 is insufficient.
7. Oxygen: compare a ventilating injured lung, obstruction, apnoea and depleted supply; demonstrate that the mask alone does not correct every cause of low saturation.
8. Tablets versus stronger opioid: compare onset, role/access denials and breathing/pressure consequences; reassess after naloxone.
9. Arrest: distinguish shockable/nonshockable rhythms and demonstrate failure of CPR alone while the reversible cause persists.

Record server presets, observations and treatment times for each session. A successful drill should show stable findings after reassessment, not merely an animation or a disappearing kit.

## Before final publication

This draft needs updates for further medication types, final oxygen/medication art and native-route compatibility, complete multiplayer/latency acceptance, authenticated reconnect and the remaining saved-world fault cases. The release manual must cite the final settings preset and passing scenario evidence, and remove any claim that the release does not implement.
## Obtaining supplies

The standard US, USSR and FIA arsenals use the **BCM Medical** category. They contain the same standalone selection, including separate fluids, oxygen, all six medications, folded body bags and towing straps. GM controls set each medical item price, faction availability and refund fraction. Disable/re-enable refreshes arsenal content; carried items are retained. Refund defaults to zero. The stationary stretcher is a world object available in the native GM placement catalogue; folded inventory deployment remains separate work.

Mask fit controls adjust appearance only. They do not change oxygen flow or source capacity. Tablet animation can be disabled for native handling; either presentation still requires safe swallowing, a valid finite supply and the server timer.

## Recovering the deceased and waiting to deploy

Use a folded body bag on a deceased character. Successful bagging consumes that kit, transfers the original equipment into the filled bag and removes the physical corpse. Search the bag through normal inventory. Close its inventory before another player drags, loads, buries or sends it home. Filled bags support manual or rope dragging; carrying them is unavailable.

Loading a bag deposits its equipment into the vehicle's fixed cargo. Unloading brings back the bag while the deposited equipment stays in that vehicle. Check the vehicle inventory before leaving. A backpack placed in a vehicle remains a personal container rather than fixed vehicle cargo.

To bury, hold an entrenching tool and place the bag stationary on natural ground. Remain close and eligible through the digging action; the default duration is thirty seconds. Burial creates an inspectable memorial and disposes of equipment still inside the bag. Send Body Home instead requires a bag within an enabled native medical facility's healing area. It also disposes of equipment still inside the bag. Neither operation refunds a consumed deployment life.

Limited Lives is the sole BCM respawn policy. Recovering, burying or sending home a bag does not block deployment, spend another life or refund allowance. There is no separate body-bag respawn countdown, request, spectator option or Body Bag Admin menu. Burial and Send Body Home remain physical recovery actions configured in the single BCM Medical GM tab.

## Saved-world restart during development

On a persistence-enabled mission using the same server profile, native saved-world tests retain original partial fluid, oxygen and AED contents; patient wounds, applied equipment and medication history; physical recovery bags, graves, stationary stretcher references and native vehicle cargo. Restarting does not provide fresh supplies or a fresh medication repeat interval. CPR and the previous AED analysis do not resume; reassess before a new shock decision.

Unloading a recovery bag still leaves transferred equipment in the vehicle after a restart. Local three-process checks also confirm destroyed/deleted vehicle cargo remains disposed on the following saved-world restart. Physical disposition never refunds or gates a Limited Lives deployment.

These are development acceptance results. Restored-world owner/observer/late-join coverage now passes local native tests. Backend-authenticated reconnect and broader fault/load acceptance remain open; a settings file alone is not a world save. Use the persistence-enabled mission/profile setup and current validation ledger when preparing scenarios.


GMs can use Add one deployment life, Remove one deployment life and Reset deployment allowance in the existing native character context menu for a connected player. These require unlimited native Edit authority and preserve the current character/clinical state. Reset cancels pending preparation. They do not restore the removed body-bag menu or affect physical recovery. See [Limited Lives administration](LIMITED-LIVES-ADMINISTRATION.md).


### Game Master stationary treatment stretcher

The normal GM browser now exposes **BCM stationary treatment stretcher** as a medical world object. Place it on valid terrain, then use the normal casualty placement/removal actions during play. It inherits the configured field-treatment facility policy and native world persistence. Two-player carrying remains shelved; folded inventory deployment is not part of this entry. See [GM-STATIONARY-STRETCHER.md](GM-STATIONARY-STRETCHER.md) for controls and acceptance limits.

## Qualified NPC fracture aid

An active, available AI group member with explicit BCM medic qualification and its own usable splint can support a fractured NPC group member already in treatment reach. Normal patient ticks select care; the usual preparation, animation, timer, reservation and source consumption apply. Qualification loss, range loss, patient loss or native behavior interruption cancels the owned reservation. Player patients and player-controlled medics are excluded, including patient takeover during care. This does not provide distant casualty navigation, broad triage, advanced AI drug/device care or player consent. See [medical AI scope](NATIVE-MEDICAL-AI.md).
