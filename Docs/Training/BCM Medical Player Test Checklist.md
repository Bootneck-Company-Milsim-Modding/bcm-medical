# BCM Medical player test checklist

31 October 2026. Test the complete player-focused build, including treatment, equipment, transport, deceased recovery and server controls. This workbook is for Arma Reforger gameplay. Use it with the BCM Medical Player Training Guide. None of the boxes below is pre-passed by previous automated tests.

## Record the session

Build commit or package hash: __________  Server and mission: __________  Date: __________

Game version/platform: ________  Testers/roles: ________  Ping/player count: ________

Settings profile and changes: __________  Mods loaded: __________  Evidence folder: __________

Mark each test PASS, FAIL, BLOCKED or NOT RUN. Record each named iteration separately. A condition that you could not create is BLOCKED, not PASS. For a failure, record the test ID, exact steps, patient and operator names, inventory before/after, server settings, elapsed time, screenshot/video and client/server logs. Reset the casualty between independent tests; do not let an earlier drug, wound or consumed source invalidate the next setup.

## Prepare the range

Use a private test mission on a dedicated server with BCM Medical and the base game only. Keep a backup of the normal mission save/settings. Use a disposable test profile for destructive life, death and restart cases. Do not test on an active operation. Disable unrelated medical, ACE carrying, retention and AI medical mods for the baseline; compatibility combinations are a separate later run.

Minimum people: patient A and medic B. Recommended: add observer/second medic C and a separate GM. A and B must be real connected players for ownership, consent and reconnect tests. An NPC is useful for injury preparation but cannot substitute for conscious-player acceptance. A headless/local diagnostic client is not proof of Internet account behaviour.

Prepare a flat open range, cover, slope, doorway, stairs and safe water edge; a native qualifying medical facility and a GM-placed BCM stationary treatment stretcher; US, USSR and FIA arsenals; and a jeep, truck, helicopter and ambulance with accessible fixed cargo. Keep one deliberately unsuitable/full cargo container. Record exact vehicle variants.

Stock all 29 entries: bandage, tourniquet, splint, surgery kit, suture kit, folded body bag, drag rope, weapon sling; morphine, naloxone, epinephrine, TXA, paracetamol, fentanyl; chest seal, decompression kit, airway adjunct, suction, BVM, AED, AED pads, chest drain, IV cannula, whole blood, medical monitor, pulse oximeter, saline, plasma and portable oxygen. Also prepare a shovel/entrenching tool, supported rifle with distinctive optic/magazine/ammunition, a pistol, launcher and spare grenades. Keep named spare items separate so a duplicate cannot hide a consumption bug.

Record the active settings before starting. Fresh defaults include 2 m treatment/pickup reach, bandage 10 s, splint 30 s, surgery/suture 60 s, medic reduction 40%, consent timeout 15 s and sling-required retention policy 1. Existing saved profiles can differ. Most advanced medic-only restrictions default off; fentanyl defaults to medic-only plus IV access. Independent fluid lines and oxygen default on; redistributed-fluid burden and stable-care component recovery default off. Two-player stretcher carrying stays off.

## Create and verify casualties

For the first leg test, A stands still with the chosen leg exposed, B fires one controlled low-power shot into the lower leg from a recorded distance, then both stop. Use H, confirm A's name, select that exact leg and run Check bleeding and Examine injury. Record weapon, ammunition, distance and hit side. Armour, projectile chance and damage can change the outcome: a shot is an input, not proof of a fracture, retained bullet, tension or internal injury. Verify the reported condition before using that casualty for a specialised case. GM reset/replace and repeat when the wrong condition was generated.

Use these setup recipes throughout. W means a confirmed external wound in the named region. F means a confirmed left/right leg fracture. P means a confirmed retained projectile. I means a confirmed unrepaired internal source in the named compartment. C means a verified side-specific chest leak, tension or pleural blood collection. O means verified obstruction/foreign material. R means a living pulseless casualty with verified rhythm. V means controlled blood loss followed by external bleeding control, with reserve/pressure recorded. Start healthy when a negative test says so.

For rare conditions, use a prepared, reproducible test mission/casualty supplied by the developer, or repeat controlled trauma and verify the condition. There is no promised GM button to directly set every rhythm, blood component or obstruction. Do not substitute a dead body for arrest, or a low saturation reading for a confirmed chest diagnosis. If a recipe cannot produce a stable isolated condition, mark that case BLOCKED and request a prepared scenario. Developer fixtures under Tests/Runtime are automated evidence, not a player-facing injury menu.

Use a stopwatch from the visible action/progress start to completion. Preparation and completion handling can add variable time; the preparation allowance is not a mandatory extra 15 s. For comparisons, use the same stance, original supply, role and server settings. GM FullHeal is an administrative reset, not a treatment pass, and does not resurrect a destroyed character or refill a used external device. Respawn/re-equip as needed.

## Run order and multiplayer rules

First run INSTALL, MENU, WOUND and TXN as a short smoke session. Then run the remaining clinical, equipment and transport groups. Run LIFE, SAVE and GM on the disposable profile last. Finish with NET and compatibility/performance cases. Reserve enough sessions; do not rush the full checklist into one short evening.

For every successful treatment below, A confirms their own state, B confirms completion and inventory use, and C checks the visible result. Reopen and re-examine: old examination text is a snapshot. For each treatment family repeat once with A/B swapped, once on self where the action is supported, and once with a late joiner observing the completed state. Record unsupported self-use as the expected refusal, not a missing feature.

For each family also run the shared interruption matrix TXN. For left/right procedures, run each side independently and a wrong-side attempt. For seven-region checks, run head, chest, abdomen, left/right arm and left/right leg as seven recorded iterations. Do not tick a parameterised test until all its named iterations have results.

## Installation and supply baseline

### INSTALL01 Clean install and dependency check
Setup: A fresh client profile and dedicated server; record the build identifier. Preserve your normal installation separately.
Do: Load the test mission with BCM Medical plus base game only. Join with A and B, spawn and inspect client/server logs. Repeat on another clean client.
Pass: Join and character spawn succeed, BCM input/menu/supplies load, no missing addon/script/resource errors, and no ACE or external medical/retention dependency is requested.

### INSTALL02 Catalogue and all supplies
Setup: Empty recorded inventories and each US, USSR and FIA arsenal in turn.
Do: Find the BCM Medical heading. Acquire every one of the 29 named entries, inspect its name/icon/model, transfer it, drop/pick it up and reopen inventory. Record each item and faction separately.
Pass: All enabled entries are available and usable by their intended action; counts remain exact through transfer. Temporary shared art is recorded for polish rather than mistaken for a different functional item.

### INSTALL03 Ordinary base game behaviour
Setup: A healthy player, ordinary weapons/backpack, stock bandages and native saline.
Do: Walk, run, crouch, prone, aim, fire, reload, change weapons, enter/exit vehicles and manage inventory. Compare before and after opening medical UI.
Pass: Normal controls return after closing the UI; no stuck camera/input, lost gear or unexpected medical treatment occurs on healthy players.

## Medical menu and assessment

### MENU01 Target identity and self selection
Setup: A and C stand near B; then move one out of examination access.
Do: B looks at A and presses H; repeat looking at C and empty ground. Use SELF / SWAP. Repeat with a conscious and unconscious A.
Pass: Patient name and body belong to the intended eligible target; no eligible looked-at patient opens self. Swapping never applies an action to the previous person.

### MENU02 Seven regions and five tabs
Setup: A has one verified W on a recorded side; other regions are healthy.
Do: Select all seven regions and each of Circulation, Airway, Surgery, Medication and Fluids. Use MORE TREATMENTS. Check bleeding/examine the hit region and its opposite side.
Pass: The correct patient-side injury is reported; patient left appears on the viewer's right. Available actions match region and pagination, with no clipped/unreachable buttons.

### MENU03 Findings are requested and private
Setup: A wounded, B examining, C with a separate medical view.
Do: Open/select without running a check; then examine, treat and re-examine. Switch patient during a pending check, close/reopen, and compare C's findings.
Pass: Opening/clicking alone is not a completed examination. Replies attach to the correct patient/region and do not overwrite a newer selection. Old private snapshots are not presented as newly measured shared findings.

### MENU04 Vehicle patient list
Setup: A and B in one vehicle, C nearby outside or in another vehicle.
Do: Open the occupant list, change seats, exit and re-enter. Attempt an otherwise prohibited treatment while seated.
Pass: The list reflects actual shared-vehicle examination access, not everyone nearby. Leaving removes access as appropriate; examination access does not bypass in-vehicle procedure restrictions.

### MENU05 Bindings and human input
Setup: Save your original input settings; keyboard/mouse and controller if supported by your intended platform.
Do: Rebind Medical interface in personal BCM Medical controls; test tap/hold and reset to H. Reconnect/restart. Repeat body selection, paging, cancel and bottom-right transport controls at 720p, 1080p and your normal UI scale.
Pass: Personal bindings persist and affect only that player. Small transport buttons remain side-by-side at bottom right without shrinking/covering the body or preventing controller focus and selection.

## Bleeding wounds and tourniquets

### WOUND01 First leg shot and both interaction routes
Setup: A healthy with exposed left lower leg; B has at least two counted dressings.
Do: Make one W as described in the setup recipe. Inspect native look-at/inventory bandaging availability without the medical menu, then use H > left leg > Circulation > Check bleeding. Bandage through the menu and recheck. Reset and repeat the available native route, then repeat the right leg.
Pass: Both supported routes treat the intended real wound and consume one dressing only on success. External loss falls/stops for that wound, the region shows dressing state and another unbandaged region is not silently treated.

### WOUND02 Every region and multiple wounds
Setup: Separate verified W cases for all seven regions; then wounds in two regions and two wounds in one region where reproducible.
Do: Record region counts, blood/reserve trend and inventory. Dress one region, recheck every injured region, then finish the rest.
Pass: Counts and dressing state match actual regions. A completed region treatment follows its linked native wound group; it does not close or dress another region or double-charge the same application.

### WOUND03 Dressing wear and movement
Setup: Three comparable dressed W cases; record wear settings and remove further incoming damage.
Do: Keep one resting, one walking and one running. Recheck periodically beyond the active wear budget; record reopening and re-bandage. For a faster dedicated run, use a valid shorter wear setting and restore it afterwards.
Pass: Dressings can reopen, movement accelerates wear according to settings, recurrence produces real renewed loss and fresh dressing controls it. Random lifetime differences are accounted for before comparing rates.

### WOUND04 Native tourniquet
Setup: Verified bleeding limb, one native tourniquet and dressing; record the selected limb.
Do: Apply through its supported native action, inspect TQ state and loss, dress the wound and remove/reapply the tourniquet where native interaction permits. Repeat opposite limb and try an ineligible torso target.
Pass: Appropriate limb flow responds; torso use is refused. TQ state remains separate from dressing, suturing and lost-blood replacement; no duplicate usable tourniquet is created.

### WOUND05 Continued loss without double ticking
Setup: One isolated W, no medicines/fluids/internal injury; note timed circulation estimates.
Do: Compare equal stationary intervals before dressing, immediately after dressing and after a confirmed reopening. Repeat at a different server frame/load condition.
Pass: Native external bleeding remains coherent with the wound and treatment, without an unexplained second BCM damage tick. Unexpected loss is investigated for a hidden internal source before reporting duplication.

## Extraction suturing and internal repair

### SURG01 Retained projectile extraction
Setup: A verified P with bleeding controlled; B owns one surgery kit; suitable facility when required.
Do: Examine the exact region, explore/remove projectile, check progress/inventory and examine again. Attempt the same extraction when no projectile remains; repeat a non-projectile injury.
Pass: The verified projectile is removed by a completed eligible operation. Extraction does not silently suture or repair separate internal injury; repetition without a target cannot consume a spare kit for a free heal.

### SURG02 Suture prerequisites and new damage
Setup: First a bleeding wound, then a dressed P, then a controlled projectile-free wound; one counted suture kit each run.
Do: Attempt suture in all three states. Complete the eligible case, wait beyond the old dressing timer, then inflict and verify a new W in that region.
Pass: Active loss or retained projectile blocks closure. Eligible closure consumes once and stays closed; a new impact can create a new injury without reopening the old closed wound as a timer artefact.

### SURG03 Critical regions and facility radius
Setup: Confirmed head, chest and abdominal surgical indications, plus a limb case; active facility policy recorded.
Do: Try the relevant repair away from a facility, with only one participant inside its radius, and with both inside. Repeat around the boundary; then toggle field critical surgery in the disposable profile.
Pass: Head/torso policy and both-person range checks are enforced throughout. An inventory medical pouch or ordinary seat does not count as a stationary facility.

### SURG04 Internal source and compartments
Setup: I separately in head, chest and abdomen with external loss controlled.
Do: Observe reserve deterioration, apply an external dressing, then perform eligible Repair internal bleeding for the exact compartment. Reassess another unrepaired compartment if present.
Pass: Dressing alone does not repair internal loss. Correct surgery stops its source while leaving other sources, existing pleural collection and missing circulating reserve to their own treatments.

## Leg fractures and stationary care

### LEG01 Unsupported and splinted movement
Setup: Verified F on each leg separately and both legs together; record movement settings and reserve.
Do: Attempt walk/run, crouch/crawl, jump and climb. Apply the correct splint through Surgery, repeat movement and compare pain/weight-bearing loss. Inspect wrong-leg treatment.
Pass: Unsupported movement is very slow; splint improves the configured cap without curing the fracture. Jump/climb remain blocked, and weight-bearing loss follows the fracture rules. Rest/carry/crawl avoid that specific loss, not unrelated bleeding.

### LEG02 Definitive fracture surgery
Setup: A splinted F, surgical supplies; B with permitted role.
Do: Try away from a qualifying facility even if field critical surgery is allowed. Move both participants to a facility, complete fracture repair and re-examine movement/jump restrictions.
Pass: Definitive fracture repair still requires facility access; successful repair clears the actual fracture and corresponding restrictions, without refilling blood or healing unrelated injury.

### LEG03 Stationary stretcher placement
Setup: GM opens the normal object browser; eligible unconscious A with recorded injuries/gear.
Do: Place BCM stationary treatment stretcher on valid ground. Place/remove A using its native actions; try conscious A and invalid range. Inspect from B/C and after late join.
Pass: The placed object/thumbnail and original casualty replicate. Correct placement supplies configured stationary support without healing or replacing A; ineligible placement is rejected. Two-player carry remains unavailable in the baseline.

### LEG04 Facility support and dressing stability
Setup: Comparable dressed casualties on/off stationary support and a procedure requiring a facility.
Do: Confirm both actors meet radius rules, time comparable dressing wear and perform the eligible operation. Remove the casualty/station or move outside range mid-procedure.
Pass: Configured support applies only while eligible; unsupported movement/removal cannot leave a stale facility privilege or complete an invalid procedure.

## Airway and side specific chest treatment

### AIR01 Airway assessment and adjunct
Setup: Healthy conscious negative case and eligible unconscious O with verified soft-tissue obstruction. If necessary enable the configurable obstruction generator for a separate recorded run.
Do: Head > Airway > Check airway, then insert adjunct using the original supply; reassess. Repeat on conscious/ineligible patient and with no obstruction.
Pass: The adjunct affects its eligible soft-tissue obstruction only, not foreign material or spontaneous breath production. Rejected use preserves supplies; applied state is visible and cleans up on reset.

### AIR02 Suction foreign material
Setup: Verified foreign-material O; retained suction device and a separate adjunct-only case.
Do: Check airway, suction, reassess and reuse the same device on a second eligible patient. Compare the result of adjunct alone.
Pass: Suction clears its separate obstruction and retains the exact device. Adjunct alone does not pretend to clear foreign material; a patent airway is not proof of adequate ventilation.

### AIR03 Chest seals and wrong side
Setup: Verified open left chest leak, then right; separate seal supply and assessed side.
Do: Chest > Airway, check breathing and seal the opposite side first, then the confirmed side. Record side state, consumption and deterioration; repeat both sides.
Pass: Wrong-side use cannot repair the affected side. The correct seal closes its leak without silently relieving existing tension, draining blood or repairing an internal source. Record whether an unneeded side is refused according to current state.

### AIR04 Tension decompression
Setup: Confirmed left/right tension cases with leak/source state recorded.
Do: Check breathing, perform wrong-side then correct-side decompression, seal the causative open leak when indicated and recheck over time.
Pass: Correct decompression relieves that side's tension; it is not a permanent all-chest cure while a leak remains. Wrong-side treatment does not clear the affected side; repeat procedure eligibility remains coherent.

### AIR05 Pleural blood drain
Setup: Confirmed pleural collection and internal source on one side; chest drain kit, facility/role if required by the action.
Do: Check both sides, try dressing/seal as a comparison, install the matching drain and watch the collection over time. Repair the source and replace reserve separately.
Pass: Drain removes matching pleural blood at the configured rate. It cannot restore circulating blood, drain an abdominal/head injury or fix the unrepaired bleeding source. Confirm both sides separately.

### AIR06 Lung recovery after cause correction
Setup: Verified chest injury with reduced functioning lung fraction; no uncontrolled external loss.
Do: Record breathing/SpO2, correct the actual leak/tension/collection/source as indicated, then observe recovery at timed intervals. Compare with an intentionally uncorrected cause.
Pass: Recovery follows configured lung capacity and corrected causes over time; oxygen alone does not erase structural collapse. Inspect for stable recovery rather than an unexplained instant full reset.

### AIR07 BVM and patent airway
Setup: Verified inadequate ventilation with a patent airway, then an obstructed comparison; original BVM retained.
Do: Perform assisted ventilation, time the finite support interval, reassess after it lapses and repeat. Stop/cancel one cycle and transfer/reuse the device on another casualty.
Pass: Completed cycles provide the configured temporary ventilation benefit, not indefinite automatic breaths. Airway obstruction still matters, device is retained and cancellation cannot grant a completed cycle for free.

## Oxygen and monitoring devices

### OXY01 Finite oxygen benefit
Setup: A has a patent airway, breathing and reduced oxygenation with remaining functioning lung; B owns a full identified kit.
Do: Connect at Head > Airway, check chest breathing for source status, litres and flow, record a timed litre change and disconnect. Reconnect the same kit; compare a healthy/no-benefit case.
Pass: Actual oxygen drains at the active flow while connected and can support eligible oxygenation. Disconnect stops delivery; the same kit keeps its remaining contents and does not create breaths or restore blood cells.

### OXY02 Range ownership and unconsciousness
Setup: Active oxygen with original kit and operator recorded.
Do: Cross the operating range, transfer/drop/delete the original kit in separate runs, change operator/patient consciousness and kill/reset the patient. Restore a kit only after observing stop behaviour.
Pass: Range/supply/death/reset conditions end invalid delivery. Unconsciousness alone does not remove an otherwise valid connection. A visually present mask without a valid source supplies nothing.

### OXY03 Exhaustion removal and disabled state
Setup: A low-contents kit prepared by prior active use, another full kit and active oxygen GM control.
Do: Let the original run empty, reconnect it, disconnect without a spare kit, then test connection disabled and disabling an active supply. Compare contents before/after reset, transfer and restart under SAVE.
Pass: Empty kits stay empty and cannot supply infinite oxygen. Removal does not require another kit; disabling cannot leave an invisible active oxygen effect or replenish contents.

### MON01 Hands only and instrumented vitals
Setup: Comparable patients; B first has no monitor/oximeter, then obtains each counted device.
Do: Perform hands-only pulse/rate and breathing checks, circulation assessment and instrumented chest vitals. Read labels and repeat after state changes.
Pass: Hands checks can report pulse/rate and qualitative findings. Instrument-only measurements need their device. Game volume/red-cell estimates are labelled as estimates; stale snapshots are not silently live measurements.

### MON02 Live pulse oximeter and sound
Setup: Active arm pulse oximetry, B holding the original retained instrument, C viewing A.
Do: Change A's state, watch updated pulse/SpO2 from B/C, cross operating and audible ranges, toggle sound, transfer/drop the instrument and end the session. Compare with a manual old check.
Pass: Active feed and optional audio follow current valid device/range/settings. Device loss stops updates; a stale manual reading is distinguishable. No monitor item is consumed or duplicated.

## Arrest CPR and AED

### CPR01 Arrest and compressions
Setup: A verified living R created by confirmed severe blood loss or hypoxia, with cause recorded; teammates ready.
Do: Check pulse, start CPR, inspect patient and observer animation, then stop/cancel/restart and let a lease expire. Record pulse and deterioration with/without ongoing compressions.
Pass: CPR provides configured partial perfusion but reported spontaneous pulse remains zero during arrest. Lease/cancellation stops unsupported compressions; no duplicate rescuer session or universal instant revival occurs.

### CPR02 AED pads analysis and advised shock
Setup: Verified shockable VF/pulseless VT R; one pad set and an identified charged AED.
Do: Apply pads, analyse with that AED, stop CPR and complete an advised discharge while analysis is current. Resume necessary resuscitation and reassess. Run VF and pulseless VT separately when prepared cases are available.
Pass: Pads and device are separate requirements; one completed shock spends one charge on the original AED. Rhythm can change without guaranteed ROSC. Missing supplies or active CPR prevent an invalid discharge.

### CPR03 No shock rhythms and healthy patients
Setup: Separate confirmed PEA, asystole, living perfusing and deceased cases; otherwise complete AED kit.
Do: Analyse, follow no-shock advice and attempt discharge. Observe charge/pad counts and patient state.
Pass: PEA/asystole and ineligible patients do not receive a qualifying shock/revive. A refusal does not spend a discharge as though successful. Mark uncreated rhythms BLOCKED rather than substituting another rhythm.

### CPR04 Analysis age device and rhythm changes
Setup: Valid shock advice from AED one and a second AED; note the analysis timestamp.
Do: Wait beyond the active analysis lifetime, switch AED, or let the rhythm change before discharge in separate runs; attempt discharge, then reanalyse with the actual current device.
Pass: Expired/different-device/stale-rhythm advice cannot authorise a shock. A fresh eligible analysis can; transferring/reconnecting does not refill charges.

### CPR05 Finite AED and source retention
Setup: AED previously used in recorded shocks, with known remaining count; prepared shockable cases.
Do: Continue until empty in a dedicated run, attempt another shock, transfer the exact device, reset the patient and reanalyse. Repeat across SAVE.
Pass: The original finite device stays depleted and no reset, patient change, transfer or supported restart silently refills it. Analysis is not itself a free discharge.

### CPR06 ROSC and terminal limits
Setup: Comparable R cases; one receives CPR, ventilation/airway and correction of actual blood-loss/chest/drug causes; one remains uncorrected.
Do: Recheck pulse and breathing through the configured sustained recovery interval, then monitor after apparent recovery. In a separate disposable case permit prolonged unresolved terminal deterioration.
Pass: Sustained correction permits the simulated ROSC path; CPR/epinephrine/shocks alone are not guaranteed revival. Terminal death stays terminal, and later treatment does not revive a destroyed character.

## Medication pain and safeguards

### MED01 Pain assessment and structural injury
Setup: Verified W, F and I cases with recorded medication-free vitals and aiming behaviour.
Do: Assess pain/medication history, dress/close/splint as appropriate and compare. Give eligible analgesia in a stable case and compare aim, pain, breathing and structural movement limits.
Pass: Injury care and analgesia affect their configured pain contribution. Relief does not repair fractures, arm damage or lost reserve; side effects appear according to onset and active medication records.

### MED02 Morphine
Setup: Stable painful casualty above active safety limits; one named injector; no prior opioid.
Do: Select a limb > Medication, give morphine, inspect use/record, then check pain, pressure and breathing before onset and after about the configured onset/duration. Repeat eligible self-use.
Pass: Exactly one completed dose is recorded/consumed and its timed pain relief/depression is observable. No instant full heal or blood replacement occurs.

### MED03 Fentanyl and its separate effects
Setup: Stable painful casualty, B assigned medic and IV access established; record fentanyl-specific limits.
Do: Give through limb Medication, inspect analgesia and later respiratory effect separately. Repeat without medic role, without IV and with each unsafe vital condition.
Pass: Default medic/access safeguards are enforced without consuming refused doses. Analgesic and respiratory onset/clearance can differ; loss of pain relief does not prove respiratory depression has ended.

### MED04 Paracetamol and oral eligibility
Setup: Conscious stable painful A, tablet supply; then unconscious and obstructed/poor-breathing negative cases.
Do: Head > Medication, administer, watch self/other handling and recheck pain after onset. Try in the unsafe cases and during unsupported posture transitions.
Pass: Tablets are oral and delayed, not an injection or instant repair. Unsafe swallowing/consciousness refuses treatment; successful use consumes one original item, with controls restored after presentation.

### MED05 Naloxone and recurrence
Setup: Confirmed active opioid effect with adequate supporting care; a separate opioid-free casualty.
Do: Give naloxone, observe the finite reversal and continue monitoring long enough for reversal to clear while the original opioid could remain. Try without an active recorded opioid.
Pass: Appropriate opioid effects are temporarily suppressed; trauma/blood loss remain. Recurrence is possible when timings permit. The default opioid-required refusal preserves an unnecessary dose.

### MED06 Epinephrine
Setup: Separate eligible R and perfusing cases; record allow-with-pulse setting and rate/pressure ceilings.
Do: Administer in eligible states, observe timed rate/pressure change and medication record. Try high-rate/high-pressure perfusing states and pulse-present use with that permission disabled.
Pass: Configured admission rules apply. Epinephrine does not refill blood, open an airway or guarantee ROSC; a refused dose is retained.

### MED07 TXA eligibility and persistence of clot support
Setup: Separate open/dressed W, unrepaired I, fully closed/repaired injury and no-IV cases.
Do: Attempt TXA on a limb, record eligible administration, onset, dressing wear/internal flow, then repeat before its dedicated interval or at its active cap.
Pass: Default IV and qualifying-injury requirements hold. Eligible support affects clot persistence/part of internal loss without closure or volume gain; finished injury alone does not justify repeated doses.

### MED08 Every safety threshold and repeat cap
Setup: Prepared cases near each active admission threshold; healthy baseline and original medication counts.
Do: For morphine and fentanyl test pulse, systolic pressure, respiratory rate and saturation separately just below/at/above their limits; test absent circulation. For epinephrine test rate/pressure ceilings. For all six medicines test early repeat, valid repeat and active cap using their own settings.
Pass: Acceptance/refusal matches the actual boundary comparison, supply, route and role. An unstable state arising during a procedure is rechecked before commit. Record each drug/threshold as a separate result; no uncreated boundary is pre-passed.

### MED09 Combining opioids and patient records
Setup: Two distinct patients; controlled permitted doses and adequate support. Use shortened valid timings only in a labelled disposable test profile if needed.
Do: Compare morphine alone, fentanyl alone and eligible combined active effects; switch patients during a request and inspect each history. Repeat after loss/regain of consciousness.
Pass: Separate drug records remain patient-specific, overlapping depression is not accidentally cancelled, and duplicate clicks/replies do not add extra doses. Turning off a visual animation cannot bypass the clinical checks.

## Fluids access and original supplies

### FLUID01 Vascular access
Setup: V casualty and IV cannula kit; record treated limb and access state.
Do: Attempt held fluid/TXA/fentanyl before access where required; establish access on the chosen limb and retry. Remove/lose access mid-delivery through an available supported removal or prepared scenario.
Pass: Required access is checked and its applied state replicates. Access loss stops delivery; a different limb or visual cannula does not silently create missing authorised access.

### FLUID02 Each held bag and composition
Setup: Three comparable V casualties with access, no continuing loss and separately identified whole blood, saline and plasma bags.
Do: Assess reserves, administer each held bag over its active time, recheck delivered volume, pressure, red-cell and clotting estimates. Repeat with a deliberately inadequate reserve type.
Pass: Delivery is progressive and type-correct. Saline supplies volume without red cells/factors, plasma supplies volume/clotting without red cells, and whole blood replaces all modelled components. Pressure improvement alone is not full recovery.

### FLUID03 Partial cancellation and same source
Setup: A single identified full BCM bag in B's inventory and V with enough deficit.
Do: Cancel around halfway, inspect actual delivered amount and remaining source, transfer that exact partial item and resume it. Repeat source drop/loss and patient change in separate runs.
Pass: Already delivered fluid remains; no further delivery occurs while stopped. The same source resumes only its remainder and cannot be swapped invisibly for a full item. Exhausted held supply is consumed once.

### FLUID04 Safety stops and wrong supplies
Setup: Healthy/full reserve, high-pressure, minimal-deficit and genuine V cases; one of each bag type.
Do: Attempt each infusion, cross an active stop threshold during delivery, and remove the required type while leaving another type in inventory.
Pass: Current deficit/pressure safeguards stop or refuse extra delivery and preserve the unused portion. A different bag type cannot silently satisfy the named treatment or be consumed as the original source.

### FLUID05 Redistribution and no passive refill
Setup: Controlled post-loss casualties given measured saline/plasma, no active bleed or further infusion; natural regeneration off.
Do: Record volume/red-cell/clotting estimates immediately and at timed intervals over several redistribution time scales; compare whole blood and an untreated stable case.
Pass: Retention follows the active model, cell-free fluids do not manufacture red cells, and stopped bleeding alone does not automatically refill volume under default settings. Redistribution is not a new external wound.

### FLUID06 Connect independent drips
Setup: V with access and inventory capacity; B has one identified bag. Repeat whole blood, saline and plasma.
Do: Connect the chosen drip, inspect the original item moving into A's inventory, then let B leave. Reassess and disconnect without a spare bag. Transfer the partial source to C before C reconnects; repeat eligible self-connection.
Pass: Connection frees the operator and continues valid delivery independently. Disconnect retains the partial original; C must own the usable source for a new connection. No duplicate bag or free fluid appears.

### FLUID07 Drip capacity duplication and source loss
Setup: Empty/full patient inventory cases, one live line and a second medic/bag.
Do: Attempt connection without capacity and a second simultaneous line; then transfer/drop/delete its source, remove access, kill/reset A or disable lines in separate runs.
Pass: Invalid connection fails without losing/duplicating supplies. Source/access/life/settings loss stops further delivery; one original line cannot deliver twice through competing requests.

### FLUID08 Empty drip and transport
Setup: A low-contents independent bag with valid access and a separate transported patient with an active line.
Do: Run the first bag empty and inspect the physical item; try reconnecting it. Carry, drag and load/seat the second casualty where supported, then stop and recheck.
Pass: Exhausted drip leaves a physical empty bag that cannot refill itself. Valid existing delivery can continue with transport without duplicated flow or granting otherwise prohibited new in-vehicle procedures.

### FLUID09 Native stock saline
Setup: Actual base-game saline, a BCM saline item for comparison, and V with access as required by the native route.
Do: Use the native stock item through supported native actions, interrupt/retrieve it, transfer/reapply the exact source, then repeat FullHeal and SAVE recovery on that used source.
Pass: Native saline retains its native rate/lifetime rather than being labelled a fresh BCM 500 ml bag. The original used lifetime cannot reset through retrieval, reapplication, FullHeal or supported save/restart.

### FLUID10 Optional redistributed fluid burden
Setup: Separate labelled profile with burden enabled; prepare redistributed cell-free fluid accumulation, plus an off-baseline case. Record pressure and functioning lung state.
Do: Observe burden onset/severity, attempt further water-loading infusion, try needle/drain as a comparison, then allow active perfused recovery and a saved offline interval.
Pass: Configured burden can impair lung support and block additional water loading. Pleural procedures do not clear it. Recovery follows eligible active time, not offline elapsed time; default-off produces no hidden burden penalty.

### FLUID11 Optional stable component recovery
Setup: Recovery enabled in a separate profile, stable casualty low in red-cell/clotting reserve at a qualifying facility, adequate volume/pressure/oxygenation.
Do: Record active recovery, then independently add ongoing loss, lower pressure/oxygenation, leave the facility or disable recovery. Compare with default-off and a save/offline interval.
Pass: Only eligible components recover at configured rates. No circulating-volume refill, bleed repair, offline gain or resurrection occurs; each disqualifying condition pauses the benefit.

## Treatment transactions and progress

### TXN01 Shared interruption matrix
Setup: A valid indication, exact named original supply and known inventory. Run for bandage, splint, extraction, suture, internal/fracture repair, airway/chest procedures, IV, each drug, pads, oxygen and held/connected fluids.
Do: In separate attempts cancel early and near completion; move beyond tolerance, leave range/line of sight, raise weapon, become unconscious, lose the original item, change role/facility, or remove the indication mid-action. Include completed controls.
Pass: Each family follows its own commit point. Pre-commit cancellation keeps unspent consumables; held fluids/finite sources retain only what is actually left. No free completed effect, double consumption or stale reservation survives. Record family and interruption separately.

### TXN02 Progress HUD timing and privacy
Setup: Eligible B procedure on A, C nearby; compare medic/non-medic with recorded time reduction.
Do: Time bandage, splint, surgery, suture and selected equipment/drugs; watch owner progress through start, cancel, complete and menu close/reopen. Repeat supported postures.
Pass: Owner sees readable progress and cancel, C does not get B's private HUD, and timing follows current settings plus actual preparation/completion. Default field operations use progress-only presentation; no new animation is required for a missing motion.

### TXN03 Two rescuers and repeated clicks
Setup: B and C target the same injury with distinct counted supplies, then different injuries.
Do: Start competing same-source procedures, spam/double click start/finish/cancel, switch region/patient, then coordinate separate permitted care.
Pass: Authority serialises conflicting commits without duplicate consumption/heal. Independent permitted care is coherent; rejected/stale requests cannot complete on a new patient or injury.

## Living carry drag and consent

### MOVE01 Conscious carry request
Setup: Conscious A, first healthy then verified broken leg; B within pickup range, C observing.
Do: B selects A and requests carry using the small medical controls. A opens their own medical interface and accepts; then repeat decline, timeout, requester cancel and out-of-range attempts.
Pass: No conscious player is moved before explicit acceptance. Correct named offer/mode is shown; stale/declined/expired offers cannot lift A. Accepted broken-leg carry preserves the original casualty and injuries.

### MOVE02 Conscious drag request
Setup: Conscious A and eligible B with drag enabled; use a fresh request rather than an old carry offer.
Do: Request drag, accept, decline, cancel and let it time out in separate runs. Try accepting a cancelled offer and accepting as a different player.
Pass: The conscious-player consent path authorises only the actual offer, patient and mode. No stale carry token or another person's button grants drag consent.

### MOVE03 Unconscious pickup and waking
Setup: A verified unconscious but alive, with named gear and current medical state.
Do: Carry without requesting consent, release, then drag without requesting consent. Wake A through genuine stabilisation during each mode; try continuing/restarting with a fresh conscious request.
Pass: Unconscious pickup needs no acceptance prompt. Waking ends the unconsented attachment; further conscious transport requires fresh acceptance. No injury, inventory or blood reset occurs.

### MOVE04 Carry locomotion and casualty shape
Setup: Accepted or unconscious carry, normal light/heavy loadouts and different character clothing.
Do: A/B/C inspect front, rear and both sides while idle, walking and turning. Film shoulder contact, head, right/left arm, hips, knees and feet. Repeat both player owners.
Pass: Carrier uses permitted normal locomotion and configured speed cap; casualty remains anatomically coherent without reversed legs, twisted hips/neck, trapped arm or large hovering/intersection. Record latest carry as visual review pending until this human check passes.

### MOVE05 Drag crouch and direction
Setup: Eligible drag on flat ground with A/B/C views.
Do: Walk forward/back/sideways, turn freely, try standing, sprint, jump, vault and climb. Compare against normal crouch speed and active drag movement fraction.
Pass: Rescuer remains crouched, can change facing/direction and moves at the configured reduced cap. Forbidden movements do not stand/launch the casualty; drag posture remains believable with the original injuries.

### MOVE06 Terrain obstruction and release
Setup: Carry and drag cases near slope, steps, doorway, wall and uneven ground.
Do: Approach each slowly, turn in tight spaces, attempt an obstructed placement and release through both participants' menus. Repeat moving across the safe-step boundary in a prepared controlled case.
Pass: No wall teleport, ground fall-through, explosive correction or permanent attachment. Invalid floor/obstruction/excessive step yields a safe stop/release; each participant can end their permitted session.

### MOVE07 Competing requests and permissions
Setup: B/C request A, then attempt on a patient already transported or seated/ineligible; toggle carry and drag separately.
Do: Accept/reject competing offers, switch target, exceed range, disable a mode and test NPC pickup without a player dialogue.
Pass: One authorised holder/mode owns the actual casualty. Disallowed states/settings refuse cleanly, requests do not cross-authorise, and NPC absence of consent does not bypass life/eligibility checks.

### MOVE08 Death disconnect and active restart
Setup: Separate accepted carry and drag sessions on a disposable save; both participants logged in normally.
Do: In independent runs kill A, kill B, disconnect A, disconnect B, reconnect each and save/restart during attachment. Repeat with active drip and distinctive gear.
Pass: No ghost holder, stuck input, duplicated/lost casualty or unreachable gear survives. Invalid sessions terminate safely. Restored medical/supply state stays coherent; record actual attachment restoration/release policy and require a developer ruling before accepting an ambiguous restart result.

## Sling and unconscious weapon retention

### SLING01 Default sling required policy
Setup: Policy 1; A holds an identified rifle with recorded optic, magazine and exact round count. First carry one sling, then none.
Do: Cause verified BCM unconsciousness through a controlled injury, inspect hands/inventory/ground from A/B/C, recover consciousness and inspect the exact weapon. Repeat rifle, sniper rifle and machine gun where supported.
Pass: With sling, the same supported held long gun and its ammunition/attachments remain owned. Without sling, native dropping policy applies. No duplicate/prefab replacement appears, and the sling remains an inventory-only non-consumable item.

### SLING02 Other policies and item movement
Setup: The same distinctive weapon; record policy 0 then 2 in separate runs; carried sling state known.
Do: Repeat unconsciousness for policy 0 with sling and policy 2 without sling. Under policy 1 transfer/drop the sling before the trigger, and move it after retention separately. Change weapon before trigger.
Pass: Policy 0 leaves native behaviour; policy 2 protects supported held long guns without a sling. Trigger uses current item/weapon ownership without conjuring protection from a transferred sling or retaining the wrong weapon.

### SLING03 Unsupported weapons and final death
Setup: Pistol, launcher and active grenade cases on a disposable character; sling present.
Do: Trigger unconsciousness and compare native handling; separately follow a retained rifle through waking, actual death, body bag recovery and restart.
Pass: Unsupported classes follow native behaviour without an unsafe blanket retention claim. Supported gear has one coherent location/owner through later transitions; no extra rifle or live grenade duplication occurs.

## Deceased bagging and equipment recovery

### BAG01 Actual death and bag identity
Setup: Verified deceased A with distinctive named equipment, one folded bag; separately prepare a living unconscious A.
Do: Attempt bagging alive, then bag the actual deceased within range; search/display the result and inspect from all clients. Move out of range/cancel one attempt.
Pass: Living casualties cannot be bagged. Successful bagging consumes one folded bag, creates one named searchable bag and transfers the original remains/equipment without duplication; interrupted attempts retain valid gear/supply.

### BAG02 Dropped weapon ownership
Setup: A's recorded native-dropped weapon within recovery radius; another unrelated weapon nearby. In a second run C takes ownership of A's drop; in a third it is outside radius.
Do: Bag A and inventory/search all sources after each run.
Pass: Only eligible exact recorded casualty drops are recovered. Nearby unrelated, newly owned or out-of-range items are not swept into the bag. Record all gear locations rather than just total counts.

### BAG03 Search and simultaneous recovery
Setup: Filled bag with named contents, B/C opening search and a third attempted bagging of the same body.
Do: Move one item at a time, compete for the same item, close/open search and interrupt a transfer; attempt duplicate bagging.
Pass: Search/authority locks preserve single ownership. Each item has one final location; no double loot, second bag or vanished inventory appears after rejection/reconnect.

### BAG04 Remains lifetime
Setup: One unbagged corpse and one filled bag with evidence timestamps on a disposable mission; ordinary cleanup active.
Do: Observe through the protected corpse interval and active bag drag/search/cargo use, then through ordinary allowed cleanup. Separately test explicit GM deletion/vehicle deletion and record the policy.
Pass: Ordinary garbage collection respects at least the current 1800 s corpse protection and active recovery retention. Explicit deletion is not confused with ordinary GC protection; no immortal leaked session remains after intentional removal.

## Bag dragging rope and cargo

### BAGMOVE01 Manual filled bag drag
Setup: Filled bag on valid ground; B uses its normal native drag action, C observing.
Do: Start/stop, turn, walk/crouch, exceed manual movement limit, raise weapon, become unconscious, lose reach and disconnect in independent runs. Try to carry the filled bag.
Pass: Manual dragging follows limits and releases invalid control safely. Inventory stays with the one bag; no filled-body-bag carry feature is offered as a completed workflow.

### BAGMOVE02 Rope drag
Setup: Filled bag and original rope in B's inventory; record rope length/release distance.
Do: Attach and walk/turn/crouch/use permitted weapons, increase separation, remove the rope item, die/disconnect and detach manually in separate runs. Inspect visual tether and bag collisions from C.
Pass: Valid rope dragging permits its intended ordinary movement, tracks the bag within configured bounds and releases when eligibility/separation fails. Rope absence cannot leave an infinite tow; no duplicate bag appears.

### CARGO01 Four vehicle families
Setup: Jeep, truck, helicopter and ambulance variants with suitable fixed inventory capacity, stationary and within range.
Do: For each variant load a filled named bag, inspect gear deposition, move the vehicle, unload and search both bag and vehicle. Repeat with all clients and a late joiner.
Pass: Supported fixed cargo handles the actual bag and original gear. Unload leaves deposited gear in that vehicle inventory; a passenger seat/personal backpack is not substituted for fixed cargo.

### CARGO02 Rejections rollback and vehicle changes
Setup: Full/unsuitable cargo, moving/out-of-range vehicle and two suitable vehicles A/B; named bag contents.
Do: Attempt invalid loads, interrupt a valid one, then load into A, unload and load into B. Try to load one bag simultaneously, delete/destroy a vehicle in a disposable run and inspect results.
Pass: Invalid operations do not lose/duplicate gear. Gear deposited in A is not magically recovered into B by unloading its bag. Explicit vehicle destruction is recorded separately from a transfer failure.

## Burial and sending home

### REMAINS01 Burial requirements and memorial
Setup: Filled bag, natural ground and hard/invalid surfaces; B first without then with held entrenching tool.
Do: Try invalid ground/tool/range/movement, then hold still for valid burial. Cancel once, complete once, inspect the memorial from B/C and after late join/SAVE.
Pass: Default 30 s/4 m policy is enforced. Completed burial leaves inspectable memorial state and disposes remaining bag gear; no life refund or copied equipment. Retrieve wanted gear before this destructive test.

### REMAINS02 Send home
Setup: Filled bag with expendable contents; qualifying native healing area/facility inactive and active variants as applicable.
Do: Try sending home away from the qualifying area and with feature disabled; then complete the permitted action at the configured inactive native healing area. Inspect bag, gear and life allowance.
Pass: Only the qualifying supported path disposes the actual bag/remaining gear. There is no automatic respawn permission, extra life or old separate Body Bag Admin/countdown workflow.

## Death protection lives suppression and focus

### DEATH01 No instant death and depletion grace
Setup: Fresh patient and active death/grace settings recorded; keep a medic and verified fluid source ready.
Do: Apply controlled normally lethal trauma and inspect unconscious/living state. In separate runs let blood reach zero, hit again during depletion, supply actual fluid before expiry, and allow grace to expire untreated.
Pass: Ordinary protected lethal damage follows BCM unconsciousness policy. Repeat hits do not restart a free grace window. Real positive fluid delivery can clear depletion before expiry, but does not erase independent terminal hypoxic debt or make the character immortal.

### LIFE01 Initial spawn and exhaustion
Setup: New disposable identity/profile with limited lives on and known total, default four including initial deployment.
Do: Record allowance after first spawn, cancelled spawn choices, confirmed later deployments and eventual exhaustion. Reconnect to an existing surviving character and try a denied new deployment.
Pass: Allowance is committed by deployment, not a mere query/cancel. Initial deployment counts, reconnect does not award another allowance, and exhaustion prevents an unauthorised new body without killing a surviving one.

### LIFE02 GM administration and authority
Setup: Connected A with recorded allowance/current injury; authorised GM and ordinary non-GM C.
Do: Use normal character context actions to add/remove/reset allowance, revoke Edit authority and retry, then SAVE/reconnect. Try on an unsupported offline target without assuming an admin UI exists.
Pass: Only authorised connected-target operations change the ledger; they do not heal/kill/replace A. New value persists where supported. Removed body-bag respawn controls are absent; offline administration is not claimed implemented.

### SUP01 Suppression source and cover
Setup: Healthy stationary A, B firing controlled near misses at recorded distances with/without intervening cover; C observing.
Do: Test qualifying near rounds, clearly distant rounds, A's own fire, successive near misses and stopping fire. Measure release and compare UI/vitals before/after.
Pass: Qualifying incoming near misses produce configured local blur/vignette and release; own fire does not self-suppress. Cover/distance affect eligibility; suppression alone creates no wound/blood loss or shared clinical injury.

### SUP02 Suppression settings
Setup: Saved baseline and separate disabled, blur-only, vignette-only and scaled settings runs.
Do: Repeat the same firing recipe for each, including disabling during an active effect.
Pass: Visual channels follow their separate scales and clear safely; changing these controls does not alter bleeding, health or life allowance. Record photos/video at the same resolution.

### FOCUS01 Focus budget and cooldown
Setup: Healthy A with normal optics and active focus strength/duration/cooldown recorded.
Do: Use the normal bound focus control, hold through its budget, release/repress before and after cooldown, then test disabled and valid zero-duration unlimited configurations. Repeat ADS/optics and after medical UI close.
Pass: Additional focus follows configured strength/budget and requires proper release/recovery after exhaustion. Native aiming/optic behaviour stays coherent; no stuck focus, camera or input persists through UI/unconsciousness.

## Applied visuals and presentation

### ART01 Every applied device
Setup: Eligible treated casualties for tourniquet, bandage markers, splint, pads, IV, mask and supported chest devices.
Do: Inspect correct region/side from A/B/C and late joiner, change clothing/stance, remove/reset/kill where supported, and toggle master/per-device visibility and distance.
Pass: Appearance matches authoritative application and cleans up when invalid. Cosmetic visibility does not grant treatment, duplicate a usable item or refill a finite supply. Record clipping/temporary art as distinct visual defects.

### ART02 Existing treatment and casualty motions
Setup: CPR and current drug/item handling plus default progress-only field procedures; supported self/other/prone/standing cases.
Do: Watch entry, loop, exit, cancellation and control return. Inspect sling weave/buckles in inventory and carry/drag body alignment during movement.
Pass: No stuck motion, contorted limb or missing control follows an action. Default splint/surgery/suture show progress-only policy; the two approved original casualty poses are assessed without expecting new custom rescuer motions or shelved stretcher animations.

## Save restart and persistence

### SAVE01 Settings profile versus world save
Setup: Mission with actual native persistence enabled and a known server profile/save ID; baseline and backup recorded.
Do: Change one harmless setting, create a wounded named casualty with gear, save the world, exit cleanly and restart using that same profile and save. Repeat once without loading the save as a comparison.
Pass: Settings and actual world persistence are distinguishable. Valid loaded save restores supported state; merely saving settings does not recreate a casualty/world. Record profile path/save ID without uploading credentials.

### SAVE02 Clinical state and old findings
Setup: Saved wounded/splinted casualty with projectile/internal/chest/airway state and active drug records documented; each condition tested separately where necessary.
Do: Save/restart and reconnect with the original identity, check original character/gear, then freshly examine every relevant region. Reassess CPR/rhythm and AED analysis instead of assuming old authority remains valid.
Pass: Supported injuries, timed state and inventory restore coherently without a free heal or extra dose. Old private snapshots/expired analysis are not used as current measurements; no time-offline fluid or component gain occurs.

### SAVE03 Finite physical equipment
Setup: Distinct partly used oxygen kit, partly spent AED, partial held/drip BCM bag and used native saline source. Photograph/name/record each original before saving.
Do: Save/restart/reconnect, retrieve or transfer each original, resume only an eligible use and compare remaining contents/charges/native lifetime. Include unused controls and exhausted sources.
Pass: Exact original sources retain supported used state. Nothing refills from FullHeal, new owner, reapplication or offline duration; spent/empty sources remain spent. A missing physical source cannot be replaced by a phantom persisted delivery.

### SAVE04 World remains cargo and stretcher
Setup: Separate named searchable bag, memorial, loaded vehicle cargo and placed stationary stretcher with casualty association; record gear locations and entity identities.
Do: Save/restart, inspect with owner/observer/late joiner, search/remove/unload and compare original contents. Include a deliberately deleted source/vehicle in a separate run.
Pass: Supported world records restore once with correct associations; cargo gear remains at the recorded vehicle. Deleted/missing objects do not produce cloned equipment or stuck recovery sessions.

## All GM controls and role boundaries

### GM01 Catalogue and permissions
Setup: Authorised GM and normal player, fresh profile plus a backup of a complete older supported profile.
Do: Open BCM Medical settings, count all 279 saved controls under the fifteen headings, change/save/reopen one per group and try as the normal player. Load the older profile and compare retained custom values.
Pass: One coherent tab with fifteen groups, all catalogue entries and current schema 35; permissions enforced. Supported older complete profiles migrate without replacing valid custom values with fresh defaults.

### GM02 Every setting worksheet
Setup: Use Appendix A for each of the 279 entries; save active baseline and change only one independent setting at a time.
Do: Read its label/description/allowed range, choose a valid contrasting value, save/reopen/reconnect, run its linked family case, restore baseline and rerun. For paired min/max settings keep the pair valid. Exercise UI bounds without editing production profile files.
Pass: Saved value and observable intended effect agree across authority/clients, unrelated behaviour stays stable and baseline is restored. For retained shelved stretcher controls, verify presence/save and disabled workflow only; do not claim unimplemented carrying accepted.

### GM03 Role assignment revocation and time reduction
Setup: B initially ordinary, GM assigns/revokes the native BCM medic role; restrictions and reduction recorded.
Do: For each configurable surgery, AED, fluids, advanced-airway and injectable family test allowed ordinary use, medic-only enabled, role grant and mid-action revocation. Separately test fentanyl's own role/access controls and time comparisons.
Pass: Current authority role/policy is checked at commit and affects time as configured. UI/client flags cannot grant a forbidden treatment; default-off restrictions are not mistakenly taught as always medic-only.

### GM04 Arsenal prices availability and refunds
Setup: Native supply economy mission, each faction and recorded item costs/refund value.
Do: For each of 29 entries change its price, acquire/return through supported arsenal actions and compare supply totals; toggle faction availability, reopen and reconnect. Restore values.
Pass: Prices/refunds and US/USSR/FIA visibility match controls without duplicating stock. Hiding entries does not silently delete already carried supplies; no real-money system is implied.

### GM05 Disabled and removed scope
Setup: Baseline player-focused build with no external AI medical mod loaded.
Do: Inspect GM controls/actions and observe untreated NPC casualties near other NPCs. Treat NPCs manually, and inspect retired body-bag admin/AI and shelved stretcher options.
Pass: Autonomous medical AI and old body-bag respawn administration are absent; manual care of NPCs still works. Two-player stretcher carrying remains disabled and no ACE dependency or borrowed ACE content is required.

## Real multiplayer compatibility and performance

### NET01 Ownership observers and late join
Setup: Dedicated server, three real accounts, then a fourth late joiner; record ordinary Internet ping.
Do: Repeat one successful and interrupted procedure from each family, carry/drag consent/release, sling retention, bagging/search/cargo and GM allowance change. Rotate patient/operator/observer, then join after applied states exist.
Pass: Only the proper player accepts conscious transport; authority commits once, all relevant state/visuals converge and private HUD/findings remain appropriately private. Late joiner sees current state rather than replaying consumptions or requests.

### NET02 Disconnect latency and session recovery
Setup: Disposable dedicated mission; baseline low ping and an ordinary higher-latency remote connection, never an active operation.
Do: Disconnect each participant during treatment preparation, near commit, drip/oxygen, CPR, bag transfer and transport; reconnect as the original account. Repeat rapid repeated requests under latency and a clean server restart.
Pass: No double treatment, double charge, stolen identity/allowance, ghost session or endless progress. Original items and committed partial effects reconcile. Record account authentication and actual ping; local diagnostics alone do not pass this case.

### NET03 Load and long session
Setup: Expected player count or staged increasing counts with controlled casualties; record server FPS, client FPS, memory and initial log size.
Do: Run concurrent examinations/treatments, devices, drips, bag inventories and transports; rotate for an extended session and repeat joins/leaves. Sample metrics at regular intervals and after cleanup.
Pass: Choose acceptable latency/FPS/memory thresholds before the run. No sustained unbounded memory/entity/log growth, repeated script error or worsening interaction delay; totals and ownership remain correct under expected load.

### NET04 Intended platforms and content combinations
Setup: Separate baseline and one additional intended mod/platform/input combination at a time; identify exact versions.
Do: Repeat INSTALL, MENU, WOUND01, TXN, MOVE, SLING and SAVE smoke cases for each intended Windows/Linux server, client/controller/console target and content pack that is actually available.
Pass: Each tested combination has its own result/evidence. Unavailable Linux/console builds and untested third-party weapons remain NOT RUN; a Windows local pass does not certify them. Investigate a failed combination against the clean baseline before blaming either mod.

## Acceptance and handover

Treat lost/duplicated equipment, wrong-patient actions, consent bypass, infinite supplies, life bypass, persistent input lock and reproducible authority/script errors as blocking defects. Clinical wrong-side repair, uncontrolled loss after a falsely reported success, or false revival/death is also blocking. Record art/contact issues separately, with the latest carry needing explicit human visual acceptance.

The first human session is ready only when the short smoke groups pass and open blockers are reviewed. Wider player testing needs every intended feature group and relevant multiplayer/restart matrix completed. Public release additionally needs intended platform/compatibility/load acceptance and outstanding artwork/provenance review. A checklist document or an automated log is not itself a human pass.

Tester signoff: __________  Build: __________  PASS/FAIL/BLOCKED/NOT RUN totals: __________

Open blocking IDs and evidence: __________  Retest build/date: __________  Reviewer: __________

## Appendix A Complete settings worksheet

The build tool appends all current canonical setting labels, defaults and linked test groups here. Each row is a separate required GM02 iteration. For each, record baseline, changed value, effect evidence and restored value; use a separate sheet keyed by the exact setting key if you need more space. A blank result is NOT RUN. Binary defaults use On/Off; other values retain their stated units or fractional scale.

<!-- GENERATED SETTINGS WORKSHEET -->

Retained shelved stretcher controls are marked with *. Keep carrying/grip disabled; test presence and saving only. All other rows require a functional comparison using GM02 and their linked test family.

### Bleeding

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Bandage minimum wear time (seconds) (`bandageReopenMinSeconds`) | 300 | WOUND | NOT RUN |
| Bandage maximum wear time (seconds) (`bandageReopenMaxSeconds`) | 600 | WOUND | NOT RUN |
| Walking dressing wear multiplier (`walkingWoundWearMultiplier`) | 1.5 | WOUND | NOT RUN |
| Running dressing wear multiplier (`runningWoundWearMultiplier`) | 3 | WOUND | NOT RUN |
| Reopened wound bleeding fraction (`reopenedBleedingMultiplier`) | 0.65 | WOUND | NOT RUN |
| Bandage application time (seconds) (`bandageApplicationSeconds`) | 10 | WOUND | NOT RUN |
| Show applied tourniquets (`equipmentVisualTourniquetsEnabled`) | On | WOUND | NOT RUN |
| Maximum dilution bleeding increase (`fluidDilutionBleedMultiplier`) | 0.5 | WOUND | NOT RUN |
| Maximum dilution dressing wear increase (`fluidDilutionWearMultiplier`) | 1 | WOUND | NOT RUN |

### Airway and breathing

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Resting respiratory rate (`normalRespiratoryRate`) | 16 | AIR OXY | NOT RUN |
| Oxygen response per second (`oxygenChangePerSecond`) | 0.015 | AIR OXY | NOT RUN |
| Hypoxic arrest oxygen fraction (`arrestOxygenFraction`) | 0.3 | AIR OXY | NOT RUN |
| Unconscious oxygen fraction (`unconsciousOxygenFraction`) | 0.65 | AIR OXY | NOT RUN |
| Chest seal application time (seconds) (`chestSealApplicationSeconds`) | 15 | AIR OXY | NOT RUN |
| Decompression procedure time (seconds) (`decompressionSeconds`) | 15 | AIR OXY | NOT RUN |
| Airway adjunct insertion time (seconds) (`airwayInsertionSeconds`) | 10 | AIR OXY | NOT RUN |
| Airway suction time (seconds) (`suctionSeconds`) | 10 | AIR OXY | NOT RUN |
| Assisted ventilation action time (seconds) (`ventilationCycleSeconds`) | 3 | AIR OXY | NOT RUN |
| Ventilation support per completed cycle (seconds) (`ventilationSupportSeconds`) | 5 | AIR OXY | NOT RUN |
| Chest drain insertion time (seconds) (`chestDrainInsertionSeconds`) | 30 | AIR OXY | NOT RUN |
| Installed drain removal rate (game ml per second) (`chestDrainMlPerSecond`) | 10 | AIR OXY | NOT RUN |
| Lung recovery after corrected chest cause (fraction per second) (`lungReinflationPerSecond`) | 0.01 | AIR OXY | NOT RUN |
| Chest penetration injury chance at full impact (`penetratingChestInjuryChance`) | 0.35 | AIR OXY | NOT RUN |
| Tension chance within a generated chest injury (`initialTensionChance`) | 0.2 | AIR OXY | NOT RUN |
| Maximum initial traumatic lung collapse fraction (`initialLungCollapseFraction`) | 0.35 | AIR OXY | NOT RUN |
| Airway contamination chance at full head impact (`headAirwayOcclusionChance`) | 0.15 | AIR OXY | NOT RUN |
| Generate soft-tissue airway obstruction while unconscious (`automaticUnconsciousAirwayObstruction`) | Off | AIR OXY | NOT RUN |
| Minimum adequate ventilation rate (`adequateRespiratoryRate`) | 8 | AIR OXY | NOT RUN |
| Portable oxygen enabled (`oxygenEnabled`) | On | AIR OXY | NOT RUN |
| Connect oxygen equipment (seconds) (`oxygenApplicationSeconds`) | 10 | AIR OXY | NOT RUN |
| Disconnect oxygen equipment (seconds) (`oxygenRemovalSeconds`) | 3 | AIR OXY | NOT RUN |
| New portable oxygen capacity (litres) (`oxygenCapacityLitres`) | 300 | AIR OXY | NOT RUN |
| Portable oxygen flow (litres/minute) (`oxygenFlowLitresPerMinute`) | 10 | AIR OXY | NOT RUN |
| Oxygen operator maximum range (metres) (`oxygenRangeMetres`) | 3 | AIR OXY | NOT RUN |
| Oxygen support of remaining lung capacity (`oxygenLungCompensation`) | 0.5 | AIR OXY | NOT RUN |
| Minimum lung fraction for oxygen benefit (`oxygenMinimumLungFraction`) | 0.1 | AIR OXY | NOT RUN |
| Oxygen connection requires medic (`medicOnlyOxygen`) | Off | AIR OXY | NOT RUN |
| Show applied oxygen mask (`equipmentVisualOxygenEnabled`) | On | AIR OXY | NOT RUN |
| Mask right offset (m) (`maskOffsetRight`) | 0 | AIR OXY | NOT RUN |
| Mask up offset (m) (`maskOffsetUp`) | -0.08 | AIR OXY | NOT RUN |
| Mask forward offset (m) (`maskOffsetForward`) | 0.09 | AIR OXY | NOT RUN |
| Mask scale (`maskScale`) | 1.45 | AIR OXY | NOT RUN |
| Mask yaw adjustment (`maskYawDegrees`) | 0 | AIR OXY | NOT RUN |
| Mask pitch adjustment (`maskPitchDegrees`) | 90 | AIR OXY | NOT RUN |
| Mask roll adjustment (`maskRollDegrees`) | 0 | AIR OXY | NOT RUN |

### Circulation

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Nominal blood volume (mL) (`normalBloodVolumeMl`) | 5000 | FLUID MON | NOT RUN |
| Resting heart rate (`normalHeartRate`) | 75 | FLUID MON | NOT RUN |
| Instrumented vital check time (seconds) (`monitorCheckSeconds`) | 5 | FLUID MON | NOT RUN |
| Vascular access insertion time (seconds) (`ivAccessSeconds`) | 15 | FLUID MON | NOT RUN |
| Fluid bag delivery time (seconds) (`fluidInfusionSeconds`) | 60 | FLUID MON | NOT RUN |
| Fluid reserve delivered by one bag (game ml) (`fluidBagVolumeMl`) | 500 | FLUID MON | NOT RUN |
| Pulse oximeter setup time (seconds) (`pulseOximeterApplicationSeconds`) | 5 | FLUID MON | NOT RUN |
| Pulse oximeter operating range (metres) (`pulseOximeterRangeMetres`) | 3 | FLUID MON | NOT RUN |
| Pulse oximeter audible range (metres) (`pulseOximeterAudibleMetres`) | 8 | FLUID MON | NOT RUN |
| Pulse oximeter sound enabled (`pulseOximeterSoundEnabled`) | On | FLUID MON | NOT RUN |
| Show applied chest devices (`equipmentVisualChestDevicesEnabled`) | On | FLUID MON | NOT RUN |
| Blood-loss compensation pulse boost (`compensatoryHeartRateBoost`) | 80 | FLUID MON | NOT RUN |
| Late shock decompensation fraction (`shockDecompensationBloodFraction`) | 0.35 | FLUID MON | NOT RUN |
| Lower efficient pumping rate (`minimumEfficientHeartRate`) | 50 | FLUID MON | NOT RUN |
| Upper efficient pumping rate (`maximumEfficientHeartRate`) | 180 | FLUID MON | NOT RUN |
| Allow native passive blood regeneration (`naturalBloodRegenerationEnabled`) | Off | FLUID MON | NOT RUN |
| Allow stabilised blood-component recovery (`componentRecoveryEnabled`) | Off | FLUID MON | NOT RUN |
| Red-cell recovery timescale (seconds) (`redCellRecoverySeconds`) | 3600 | FLUID MON | NOT RUN |
| Clotting recovery timescale (seconds) (`clottingRecoverySeconds`) | 900 | FLUID MON | NOT RUN |
| Minimum volume for component recovery (`componentRecoveryMinimumBloodFraction`) | 0.8 | FLUID MON | NOT RUN |
| Require facility for component recovery (`componentRecoveryRequiresFacility`) | On | FLUID MON | NOT RUN |
| Minimum pressure for component recovery (`componentRecoveryMinimumSystolicPressure`) | 90 | FLUID MON | NOT RUN |
| Allow independent fluid drips (`continuousFluidLinesEnabled`) | On | FLUID MON | NOT RUN |
| Fluid line connection time (seconds) (`fluidLineConnectionSeconds`) | 8 | FLUID MON | NOT RUN |
| Fluid line disconnection time (seconds) (`fluidLineRemovalSeconds`) | 3 | FLUID MON | NOT RUN |
| Model redistributed fluid overload (`fluidOverloadEnabled`) | Off | FLUID MON | NOT RUN |
| Fluid burden onset fraction (`fluidOverloadThresholdFraction`) | 0.15 | FLUID MON | NOT RUN |
| Severe fluid burden fraction (`fluidOverloadSevereFraction`) | 0.4 | FLUID MON | NOT RUN |
| Fluid burden clearance timescale (`fluidOverloadClearanceSeconds`) | 1800 | FLUID MON | NOT RUN |
| Lung fraction at severe fluid burden (`fluidOverloadMinimumLungFraction`) | 0.65 | FLUID MON | NOT RUN |
| Withhold additional water-loading fluids (`fluidOverloadStopAdditionalWater`) | On | FLUID MON | NOT RUN |
| Water-loading stop severity (`fluidOverloadStopSeverity`) | 0.5 | FLUID MON | NOT RUN |

### Resuscitation

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| BCM depletion grace (seconds) (`bloodDepletionGraceSeconds`) | 60 | CPR DEATH | NOT RUN |
| Resting systolic pressure (`normalSystolicPressure`) | 120 | CPR DEATH | NOT RUN |
| Circulatory arrest blood fraction (`arrestBloodFraction`) | 0.25 | CPR DEATH | NOT RUN |
| Critical-state time before arrest (seconds) (`arrestOnsetSeconds`) | 10 | CPR DEATH | NOT RUN |
| CPR perfusion fraction (`cprPerfusionFraction`) | 0.25 | CPR DEATH | NOT RUN |
| Corrected reversible causes before ROSC (seconds) (`roscStabilitySeconds`) | 10 | CPR DEATH | NOT RUN |
| Maximum hypoxic injury debt (seconds) (`hypoxicDebtLimitSeconds`) | 180 | CPR DEATH | NOT RUN |
| AED pad application time (seconds) (`aedPadApplicationSeconds`) | 20 | CPR DEATH | NOT RUN |
| AED analysis time (seconds) (`aedAnalysisSeconds`) | 5 | CPR DEATH | NOT RUN |
| AED stand-clear and discharge time (seconds) (`aedShockSeconds`) | 5 | CPR DEATH | NOT RUN |
| Maximum shock analysis age (seconds) (`aedAnalysisValiditySeconds`) | 30 | CPR DEATH | NOT RUN |
| Available discharges per AED device (`aedShocksPerDevice`) | 20 | CPR DEATH | NOT RUN |
| Critical slow pulse threshold (`criticalBradycardiaRate`) | 25 | CPR DEATH | NOT RUN |
| Critical fast pulse threshold (`criticalTachycardiaRate`) | 220 | CPR DEATH | NOT RUN |
| Critical perfusion pressure (`criticalSystolicPressure`) | 45 | CPR DEATH | NOT RUN |
| Rate instability pressure ceiling (`criticalRatePressureCeiling`) | 90 | CPR DEATH | NOT RUN |
| Unstable vital exposure time (seconds) (`criticalVitalExposureSeconds`) | 15 | CPR DEATH | NOT RUN |
| Critical oxygen-delivery fraction (`criticalOxygenDeliveryFraction`) | 0.15 | CPR DEATH | NOT RUN |
| Recovery oxygen-delivery fraction (`recoveryOxygenDeliveryFraction`) | 0.3 | CPR DEATH | NOT RUN |

### Surgery

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Retained projectile chance (`retainedBulletChance`) | 0.4 | SURG | NOT RUN |
| Head and torso surgery requires facility (`criticalSurgeryFacilityOnly`) | On | SURG | NOT RUN |
| Surgery procedure time (seconds) (`surgeryDurationSeconds`) | 60 | SURG | NOT RUN |
| Suture procedure time (seconds) (`sutureDurationSeconds`) | 60 | SURG | NOT RUN |
| Medical facility treatment radius (metres) (`medicalFacilityRangeMetres`) | 5 | SURG | NOT RUN |
| Internal injury chance at full penetrating impact (`penetratingInternalBleedChance`) | 0.4 | SURG | NOT RUN |
| Maximum initial internal source flow (game ml per second) (`internalBleedMaxMlPerSecond`) | 6 | SURG | NOT RUN |

### Fractures

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Unsplinted leg movement fraction (`fractureUnsplitSpeedScale`) | 0.08 | LEG | NOT RUN |
| Splinted leg movement fraction (`fractureSplintedSpeedScale`) | 0.75 | LEG | NOT RUN |
| Splinted walking internal loss (ml/s) (`fractureWalkingBleedMlPerSecond`) | 0.15 | LEG | NOT RUN |
| Splinted running internal loss (ml/s) (`fractureRunningBleedMlPerSecond`) | 0.45 | LEG | NOT RUN |
| Splint application time (seconds) (`splintApplicationSeconds`) | 30 | LEG | NOT RUN |
| Show applied leg splints (`equipmentVisualSplintsEnabled`) | On | LEG | NOT RUN |

### Medical interactions

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Treatment interaction range (metres) (`treatmentRangeMetres`) | 2 | TXN GM03 SLING | NOT RUN |
| Examination duration (seconds) (`examinationDurationSeconds`) | 2 | TXN GM03 SLING | NOT RUN |
| CPR communication lease (seconds) (`cprLeaseSeconds`) | 5 | TXN GM03 SLING | NOT RUN |
| Treatment movement tolerance (metres) (`treatmentMovementToleranceMetres`) | 0.5 | TXN GM03 SLING | NOT RUN |
| Maximum procedure equipment preparation time (seconds) (`procedurePreparationSeconds`) | 15 | TXN GM03 SLING | NOT RUN |
| Procedure animation completion grace (seconds) (`procedureCompletionGraceSeconds`) | 5 | TXN GM03 SLING | NOT RUN |
| Medic time reduction (%) (`medicTimeReductionPercent`) | 40 | TXN GM03 SLING | NOT RUN |
| Only medics may perform surgery (`medicOnlySurgery`) | Off | TXN GM03 SLING | NOT RUN |
| Only medics may operate AEDs (`medicOnlyAED`) | Off | TXN GM03 SLING | NOT RUN |
| Only medics may insert IVs and administer fluids (`medicOnlyFluids`) | Off | TXN GM03 SLING | NOT RUN |
| Only medics may perform advanced airway procedures (`medicOnlyAdvancedAirway`) | Off | TXN GM03 SLING | NOT RUN |
| Only medics may administer injectable medication (`medicOnlyMedication`) | Off | TXN GM03 SLING | NOT RUN |
| Applied medical equipment visuals (`equipmentVisualsEnabled`) | On | TXN GM03 SLING | NOT RUN |
| Applied equipment display range (metres) (`equipmentVisualRangeMetres`) | 40 | TXN GM03 SLING | NOT RUN |
| Progress-only splint, surgery and suturing (`fieldProcedureProgressOnly`) | On | TXN GM03 SLING | NOT RUN |
| Unconscious weapon retention (`weaponRetentionPolicy`) | 1 | TXN GM03 SLING | NOT RUN |

### Body bags

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Bagging duration (seconds) (`baggingDurationSeconds`) | 3 | BAG REMAINS | NOT RUN |
| Bagging interaction range (metres) (`baggingRangeMetres`) | 2 | BAG REMAINS | NOT RUN |
| Dropped casualty equipment recovery radius (metres) (`deathEquipmentRecoveryRangeMetres`) | 5 | BAG REMAINS | NOT RUN |
| Allow body-bag burial (`burialEnabled`) | On | BAG REMAINS | NOT RUN |
| Body-bag burial duration (seconds) (`burialDurationSeconds`) | 30 | BAG REMAINS | NOT RUN |
| Body-bag burial range (metres) (`burialRangeMetres`) | 4 | BAG REMAINS | NOT RUN |
| Allow sending body bags home (`sendHomeEnabled`) | On | BAG REMAINS | NOT RUN |

### Transport

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Body bag rope length (metres) (`ropeLengthMetres`) | 2 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Rope safety release distance (metres) (`ropeBreakDistanceMetres`) | 6 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Conscious carry consent timeout (seconds) (`carryConsentTimeoutSeconds`) | 15 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Vehicle cargo interaction range (metres) (`cargoInteractionRangeMetres`) | 5 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Maximum loading vehicle speed (m/s) (`cargoMaxVehicleSpeedMetresPerSecond`) | 0.5 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Bag follow speed limit (metres/second) (`bagDragMaxSpeedMetresPerSecond`) | 8 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Bag follow responsiveness (`bagDragResponsiveness`) | 6 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Manual drag movement limit (metres/second) (`manualBagDragMaxSpeedMetresPerSecond`) | 2.5 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Two-player stretcher carrying * (`stretcherCarryEnabled`) | Off | Retained only | NOT RUN |
| Stretcher handle reach (metres) * (`stretcherJoinRangeMetres`) | 1 | Retained only | NOT RUN |
| Wait for second stretcher carrier (seconds) * (`stretcherPairTimeoutSeconds`) | 20 | Retained only | NOT RUN |
| Minimum carrier separation (metres) * (`stretcherMinimumSpanMetres`) | 1.7 | Retained only | NOT RUN |
| Maximum carrier separation (metres) * (`stretcherMaximumSpanMetres`) | 3.4 | Retained only | NOT RUN |
| Stretcher carry safety speed (metres/second) * (`stretcherMaximumSpeedMetresPerSecond`) | 2.2 | Retained only | NOT RUN |
| Stretcher carrier movement fraction * (`stretcherCarrySpeedFraction`) | 0.3 | Retained only | NOT RUN |
| Stretcher lift height (metres) * (`stretcherCarryHeightMetres`) | 0.85 | Retained only | NOT RUN |
| Maximum carrier height difference (metres) * (`stretcherMaximumHeightDifferenceMetres`) | 0.6 | Retained only | NOT RUN |
| Stretcher movement protection (`stretcherStabilityEnabled`) | On | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Stretcher dressing wear reduction (`stretcherDressingWearReduction`) | 0.25 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Stretcher carrier end offset (metres) * (`stretcherHandleOffsetMetres`) | 1.5 | Retained only | NOT RUN |
| Stretcher maximum turn rate (degrees/second) * (`stretcherMaximumTurnDegreesPerSecond`) | 120 | Retained only | NOT RUN |
| Stretcher hand-grip animation * (`stretcherGripAnimationEnabled`) | Off | Retained only | NOT RUN |
| Stretcher grip blend time (seconds) * (`stretcherGripBlendSeconds`) | 0.25 | Retained only | NOT RUN |
| Living casualty carrying (`casualtyCarryEnabled`) | On | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Living casualty dragging (`casualtyDragEnabled`) | On | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Casualty pickup and request range (`casualtyTransportRangeMetres`) | 2 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Casualty carry movement fraction (`casualtyCarrySpeedFraction`) | 0.35 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Casualty crouch-drag movement fraction (`casualtyDragSpeedFraction`) | 0.25 | MOVE BAGMOVE CARGO LEG | NOT RUN |
| Casualty transport movement break distance (`casualtyTransportMaxStepMetres`) | 1.5 | MOVE BAGMOVE CARGO LEG | NOT RUN |

### Suppression

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Suppression enabled (`suppressionEnabled`) | On | SUP | NOT RUN |
| Suppression release (seconds) (`suppressionReleaseSeconds`) | 3 | SUP | NOT RUN |
| Suppression blur (%) (`suppressionBlurPercent`) | 12 | SUP | NOT RUN |
| Suppression vignette (%) (`suppressionVignettePercent`) | 18 | SUP | NOT RUN |
| Suppression blur multiplier (`suppressionBlurMultiplier`) | 1 | SUP | NOT RUN |
| Suppression vignette multiplier (`suppressionVignetteMultiplier`) | 1 | SUP | NOT RUN |

### Lives

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Limit player lives (`livesEnabled`) | On | LIFE | NOT RUN |
| Total lives including initial deployment (`totalLives`) | 4 | LIFE | NOT RUN |

### Focus

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Focus budget enabled (`focusEnabled`) | On | FOCUS | NOT RUN |
| Extra focus strength (%) (`focusStrengthPercent`) | 50 | FOCUS | NOT RUN |
| Focus maximum duration (0 unlimited) (`focusDurationSeconds`) | 10 | FOCUS | NOT RUN |
| Focus cooldown (seconds) (`focusCooldownSeconds`) | 30 | FOCUS | NOT RUN |

### Pain

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Pain per open wound (game units) (`painWoundUnits`) | 18 | MED01 | NOT RUN |
| Dressed wound pain multiplier (`painDressedMultiplier`) | 0.6 | MED01 | NOT RUN |
| Closed wound pain multiplier (`painSuturedMultiplier`) | 0.2 | MED01 | NOT RUN |
| Pain per unsupported fracture (game units) (`painFractureUnits`) | 45 | MED01 | NOT RUN |
| Splinted fracture pain multiplier (`painSplintMultiplier`) | 0.5 | MED01 | NOT RUN |
| Pain per active internal injury (game units) (`painInternalUnits`) | 25 | MED01 | NOT RUN |
| Maximum pain contribution to native aiming damage (`painAimingPenalty`) | 0.4 | MED01 | NOT RUN |
| Maximum pain pulse increase (`painHeartRateBoost`) | 15 | MED01 | NOT RUN |

### Medication

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Medication application time (seconds) (`medicationApplicationSeconds`) | 10 | MED | NOT RUN |
| Minimum repeat interval per medication (seconds) (`medicationRepeatSeconds`) | 60 | MED | NOT RUN |
| Maximum concurrent doses per medication (`medicationMaxActiveDoses`) | 3 | MED | NOT RUN |
| Morphine onset time (seconds) (`morphineOnsetSeconds`) | 30 | MED | NOT RUN |
| Morphine effect duration (seconds) (`morphineDurationSeconds`) | 600 | MED | NOT RUN |
| Morphine pain relief per active dose (game units) (`morphinePainReliefUnits`) | 45 | MED | NOT RUN |
| Morphine breathing suppression per active dose (`morphineRespiratoryDepression`) | 0.35 | MED | NOT RUN |
| Morphine pressure suppression per active dose (`morphinePressureDepression`) | 0.15 | MED | NOT RUN |
| Naloxone onset time (seconds) (`naloxoneOnsetSeconds`) | 5 | MED | NOT RUN |
| Naloxone effect duration (seconds) (`naloxoneDurationSeconds`) | 180 | MED | NOT RUN |
| Naloxone opioid suppression per active dose (`naloxoneReversalFraction`) | 1 | MED | NOT RUN |
| Epinephrine onset time (seconds) (`epinephrineOnsetSeconds`) | 10 | MED | NOT RUN |
| Epinephrine effect duration (seconds) (`epinephrineDurationSeconds`) | 180 | MED | NOT RUN |
| Epinephrine pulse increase per active dose (`epinephrineHeartRateBoost`) | 25 | MED | NOT RUN |
| Epinephrine pressure increase per active dose (`epinephrinePressureBoost`) | 15 | MED | NOT RUN |
| TXA administration time (seconds) (`txaApplicationSeconds`) | 15 | MED | NOT RUN |
| TXA onset time (seconds) (`txaOnsetSeconds`) | 30 | MED | NOT RUN |
| TXA effect duration (seconds) (`txaDurationSeconds`) | 600 | MED | NOT RUN |
| TXA clot breakdown reduction (`txaClotWearReduction`) | 0.5 | MED | NOT RUN |
| TXA minimum repeat interval (seconds) (`txaRepeatSeconds`) | 600 | MED | NOT RUN |
| TXA maximum active administrations (`txaMaxActiveDoses`) | 1 | MED | NOT RUN |
| TXA requires vascular access (`txaRequiresVascularAccess`) | On | MED | NOT RUN |
| TXA restricted to medics (`txaMedicOnly`) | Off | MED | NOT RUN |
| Show applied IV access (`equipmentVisualIVEnabled`) | On | MED | NOT RUN |
| Medication vital-sign safeguards (`medicationVitalChecksEnabled`) | On | MED | NOT RUN |
| Morphine minimum pulse (bpm) (`morphineMinimumHeartRate`) | 50 | MED | NOT RUN |
| Morphine minimum systolic pressure (`morphineMinimumSystolicPressure`) | 90 | MED | NOT RUN |
| Morphine minimum breathing rate (`morphineMinimumRespiratoryRate`) | 10 | MED | NOT RUN |
| Morphine minimum oxygen saturation (`morphineMinimumOxygenFraction`) | 0.9 | MED | NOT RUN |
| Epinephrine maximum pulse (bpm) (`epinephrineMaximumHeartRate`) | 180 | MED | NOT RUN |
| Epinephrine maximum systolic pressure (`epinephrineMaximumSystolicPressure`) | 180 | MED | NOT RUN |
| Allow epinephrine with a pulse (`epinephrineWithPulseEnabled`) | On | MED | NOT RUN |
| Naloxone requires active opioid (`naloxoneRequiresOpioid`) | On | MED | NOT RUN |
| Fluid vital-sign safeguards (`fluidSafetyChecksEnabled`) | On | MED | NOT RUN |
| Fluid stop systolic pressure (`fluidMaximumSystolicPressure`) | 140 | MED | NOT RUN |
| Minimum fluid deficit (ml) (`fluidMinimumDeficitMl`) | 50 | MED | NOT RUN |
| TXA internal clot support fraction (`txaInternalBleedReduction`) | 0.2 | MED | NOT RUN |
| Simulate fluid composition (`fluidCompositionEnabled`) | On | MED | NOT RUN |
| Saline retained volume fraction (`salineRetainedFraction`) | 0.25 | MED | NOT RUN |
| Plasma retained volume fraction (`plasmaRetainedFraction`) | 0.8 | MED | NOT RUN |
| Fluid redistribution time scale (seconds) (`fluidRedistributionSeconds`) | 300 | MED | NOT RUN |
| Tablet administration time (seconds) (`paracetamolApplicationSeconds`) | 10 | MED | NOT RUN |
| Paracetamol onset (game seconds) (`paracetamolOnsetSeconds`) | 60 | MED | NOT RUN |
| Paracetamol clearance (game seconds) (`paracetamolDurationSeconds`) | 1200 | MED | NOT RUN |
| Tablet minimum repeat interval (seconds) (`paracetamolRepeatSeconds`) | 300 | MED | NOT RUN |
| Maximum active tablet administrations (`paracetamolMaxActiveDoses`) | 2 | MED | NOT RUN |
| Tablet pain relief (game units) (`paracetamolPainReliefUnits`) | 20 | MED | NOT RUN |
| Minimum breathing rate for oral medicine (`oralMinimumRespiratoryRate`) | 10 | MED | NOT RUN |
| Fentanyl administration (seconds) (`fentanylApplicationSeconds`) | 10 | MED | NOT RUN |
| Fentanyl analgesic onset (game seconds) (`fentanylOnsetSeconds`) | 10 | MED | NOT RUN |
| Fentanyl analgesic clearance (seconds) (`fentanylDurationSeconds`) | 180 | MED | NOT RUN |
| Fentanyl breathing-effect onset (seconds) (`fentanylRespiratoryOnsetSeconds`) | 20 | MED | NOT RUN |
| Fentanyl breathing-effect clearance (seconds) (`fentanylRespiratoryDurationSeconds`) | 300 | MED | NOT RUN |
| Fentanyl minimum repeat interval (seconds) (`fentanylRepeatSeconds`) | 120 | MED | NOT RUN |
| Maximum active fentanyl administrations (`fentanylMaxActiveDoses`) | 2 | MED | NOT RUN |
| Fentanyl pain relief (game units) (`fentanylPainReliefUnits`) | 70 | MED | NOT RUN |
| Fentanyl breathing depression fraction (`fentanylRespiratoryDepression`) | 0.5 | MED | NOT RUN |
| Fentanyl pressure depression fraction (`fentanylPressureDepression`) | 0.2 | MED | NOT RUN |
| Fentanyl minimum systolic pressure (`fentanylMinimumSystolicPressure`) | 100 | MED | NOT RUN |
| Fentanyl minimum pulse (bpm) (`fentanylMinimumHeartRate`) | 50 | MED | NOT RUN |
| Fentanyl minimum breathing rate (`fentanylMinimumRespiratoryRate`) | 12 | MED | NOT RUN |
| Fentanyl minimum oxygen fraction (`fentanylMinimumOxygenFraction`) | 0.92 | MED | NOT RUN |
| Fentanyl requires vascular access (`fentanylRequiresVascularAccess`) | On | MED | NOT RUN |
| Fentanyl requires medic (`fentanylMedicOnly`) | On | MED | NOT RUN |
| Tablet hand-to-mouth animation (`tabletAnimationEnabled`) | On | MED | NOT RUN |

### Arsenal

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| US medical supplies (`arsenalUSEnabled`) | On | INSTALL02 GM04 | NOT RUN |
| USSR medical supplies (`arsenalUSSREnabled`) | On | INSTALL02 GM04 | NOT RUN |
| FIA medical supplies (`arsenalFIAEnabled`) | On | INSTALL02 GM04 | NOT RUN |
| Medical supply refund fraction (`arsenalRefundFraction`) | 0 | INSTALL02 GM04 | NOT RUN |
| Tourniquet supply cost (`arsenalCostTourniquet`) | 0 | INSTALL02 GM04 | NOT RUN |
| Bandage supply cost (`arsenalCostBandage`) | 0 | INSTALL02 GM04 | NOT RUN |
| AED supply cost (`arsenalCostAED`) | 30 | INSTALL02 GM04 | NOT RUN |
| AED electrode pads supply cost (`arsenalCostAEDPads`) | 3 | INSTALL02 GM04 | NOT RUN |
| AirwayAdjunct (placeholder) supply cost (`arsenalCostAirwayAdjunct`) | 3 | INSTALL02 GM04 | NOT RUN |
| Bag valve mask supply cost (`arsenalCostBVM`) | 8 | INSTALL02 GM04 | NOT RUN |
| ChestDrain (placeholder) supply cost (`arsenalCostChestDrain`) | 3 | INSTALL02 GM04 | NOT RUN |
| Vented chest seal supply cost (`arsenalCostChestSeal`) | 3 | INSTALL02 GM04 | NOT RUN |
| Body-bag drag rope supply cost (`arsenalCostDragRope`) | 2 | INSTALL02 GM04 | NOT RUN |
| Body bag (empty) supply cost (`arsenalCostEmptyBodyBag`) | 3 | INSTALL02 GM04 | NOT RUN |
| Epinephrine supply cost (`arsenalCostEpinephrine`) | 3 | INSTALL02 GM04 | NOT RUN |
| Fentanyl supply cost (`arsenalCostFentanyl`) | 3 | INSTALL02 GM04 | NOT RUN |
| Whole blood bag supply cost (`arsenalCostFluidBag`) | 3 | INSTALL02 GM04 | NOT RUN |
| Medical monitor supply cost (`arsenalCostMonitor`) | 15 | INSTALL02 GM04 | NOT RUN |
| Morphine supply cost (`arsenalCostMorphine`) | 3 | INSTALL02 GM04 | NOT RUN |
| Naloxone supply cost (`arsenalCostNaloxone`) | 3 | INSTALL02 GM04 | NOT RUN |
| NCD kit supply cost (`arsenalCostNCD`) | 3 | INSTALL02 GM04 | NOT RUN |
| Portable oxygen kit supply cost (`arsenalCostOxygen`) | 15 | INSTALL02 GM04 | NOT RUN |
| Paracetamol tablets supply cost (`arsenalCostParacetamol`) | 3 | INSTALL02 GM04 | NOT RUN |
| Plasma bag supply cost (`arsenalCostPlasma`) | 3 | INSTALL02 GM04 | NOT RUN |
| Pulse oximeter supply cost (`arsenalCostPulseOximeter`) | 3 | INSTALL02 GM04 | NOT RUN |
| Saline bag supply cost (`arsenalCostSaline`) | 3 | INSTALL02 GM04 | NOT RUN |
| Packaged leg splint supply cost (`arsenalCostSplint`) | 5 | INSTALL02 GM04 | NOT RUN |
| Portable medical suction supply cost (`arsenalCostSuction`) | 10 | INSTALL02 GM04 | NOT RUN |
| Surgery kit supply cost (`arsenalCostSurgeryKit`) | 10 | INSTALL02 GM04 | NOT RUN |
| Suture kit supply cost (`arsenalCostSutureKit`) | 5 | INSTALL02 GM04 | NOT RUN |
| TXA supply cost (`arsenalCostTXA`) | 3 | INSTALL02 GM04 | NOT RUN |
| IV cannula kit supply cost (`arsenalCostVascularAccess`) | 3 | INSTALL02 GM04 | NOT RUN |
| Weapon sling supply cost (`arsenalCostWeaponSling`) | 3 | INSTALL02 GM04 | NOT RUN |
