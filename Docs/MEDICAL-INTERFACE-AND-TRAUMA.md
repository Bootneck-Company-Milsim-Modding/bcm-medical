# BCM Medical interface and trauma rules

This extends the audit with the user's 26 October requirements. The canonical project is now the new Workbench project in MCB Medical, not the retired compatibility-workspace prototype.

## Interface reference and original design

The supplied NIRE Medical Interface package contains 17 scripts and 11 layouts. Its main menu implementation is about 168 KB and depends on ACE medical/carrying classes. It provides triage, diagnostics, bandages, medication and advanced transport/action pages, with monitoring/history and treatment context. The installed notice identifies GPLv2 code and adapted ACE3 artwork. It is a behavioural reference only; no NIRE scripts, layouts or artwork were copied into BCM Medical.

BCM's intended interface has persistent patient identity, self/other selection, selected body region, measured observations with timestamps, a shared care log and a treatment progress/cancellation area. Keep emergency checks and active treatment visible while changing pages. Separate Stabilise, Examine, Wounds, Airway and Chest, Circulation, Surgery and Evacuate. Display why an action is unavailable, required kit, qualification and facility requirement. A selected patient/body life must stay bound to a treatment even if the medic changes tabs. Do not reveal retained projectiles or exact internal pathology merely because it exists in server state.

This interface is still design work; there is no finished native treatment menu in this commit. Native GM controls are implemented separately from the future patient interface.

## Wounds and surgery

The authority samples retained projectiles once when a gunshot wound is created: default 40% retained and 60% exit. This is configurable game balance, not a claim about real penetration probabilities. Cuts, wire wounds and other non-projectile injuries do not acquire a bullet flag.

Dressings initially control external loss. A dressing's resting wear budget is sampled between 300 and 600 seconds on application; walking and running consume it faster. Default running wear is 3x. Reopening restores reduced external loss (65% of the previous uncontrolled rate). Reapplying a dressing gives a new wear budget. Suture closure survives the old dressing timer but does not heal an unrelated organ or prevent a new injury.

In the current BCM rule set, retained-projectile extraction requires a surgery kit and qualification; suturing separately requires a suture kit, qualification and controlled external bleeding. Head/chest/abdomen operations additionally require a medical facility by default. This is an intentional gameplay treatment sequence, not a medical protocol. Extraction is not automatic wound/organ healing.

## Fractures and internal bleeding

Use the user's final revised rule: without a splint, movement remains possible but is extremely slow; jumping is blocked. A splint improves movement but does not heal the fracture. Weight-bearing movement causes a small configurable internal loss, with running worse than walking. Resting/carried casualties avoid movement-induced loss. Definitive fracture repair always requires a facility, surgical kit and qualification; no passive timer heals it.

Internal bleeding is attached to an injured source and anatomical compartment. Surgery repairs the source. A chest drain can remove a pleural collection; it neither returns blood to circulation nor repairs the source, and cannot drain a head/abdomen injury. Native anatomy mapping, ongoing perfusion/oxygenation and blood-volume coupling are still required.

## Transport

Casualties can be carried or dragged. Conscious carry requires the patient's acceptance, bound to the requesting carrier and current patient life, expiring after a configurable timeout and consumed once. Revocation, death, disconnect, out-of-range and vehicle transitions require authority revalidation/cleanup in the native adapter.

Body bags have manual drag and rope-assisted drag only. A rope/strap must exist in inventory before attachment. Rope mode should permit ordinary movement, crouching and weapon use. The tested policy calculates a bounded trailing target and releases on obstruction, invalid carrier or excessive separation. Actual ground tracing, collision sweeps, rigid-body forces, rope visuals and network ownership are not yet implemented; the policy does not teleport a real bag or demonstrate working shooting-while-dragging.

## Persistent Game Master settings

All current tunables are generated from Tools/SettingsCatalog.json into native GM controls under BCM Medical. The catalogue is the required extension point for future changes. Storage and validation are authority-owned, values replicate to clients, and successful writes are verified before application. Permission is resolved from the editor manager, never a client-supplied actor ID. The same persistent server profile is required across restarts.

KAT reference supplied by the user: https://github.com/KAT-Advanced-Medical/KAM. Its feature documentation informs the roadmap; its implementation and assets are not imported. Trauma priorities also distinguish blood loss, airway/oxygenation and tension causes rather than making the AED a universal revive action.