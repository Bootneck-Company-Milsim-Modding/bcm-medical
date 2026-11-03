# BCM Medical player test checklist — 4 November 2026

Test the Workshop update published on 3 November. Record each check as PASS, FAIL or BLOCKED. Leave untested boxes empty. Stop and record evidence immediately if equipment disappears, duplicates, a player becomes stuck, or a treatment gives an unexplained cancellation.

## Session record

- Date/time and tester names:
- Workshop version shown on server and clients:
- Server mission, medical settings preset and active addon list:
- Vehicle configuration:
- Client resolution, UI scale, input device and approximate ping:
- Results and issue log location:

## 1. Update and session setup

- [ ] Update the server and every client from Workshop. Confirm matching versions before connecting.
- [ ] Start with BCM Medical and required dependencies. Save the complete addon list.
- [ ] Use a dedicated server with at least three people, rotating medic, patient and observer roles.
- [ ] Prepare conscious injured, unconscious and dead casualties, including AI.
- [ ] Give the equipment-test casualty clothing, a backpack with nested contents, weapon/attachments, magazines, grenade, two smoke grenades, binoculars, watch and two demolition blocks. Capture the full inventory.
- [ ] Take baseline screenshots of medical settings and the casualty inventory.

## 2. Interface and assessment

- [ ] Press H to open, close and reopen the medical menu. Repeat while viewing yourself and another casualty.
- [ ] Test the rebound medical key and the configured tap/hold preference. Reset it afterward.
- [ ] Check every tab, body region, patient entry and scrolling area for clipped text or overlapping controls.
- [ ] Check procedure progress and remaining time with the menu open and closed.
- [ ] Examine patient A, switch to B and return to A. Unchanged findings should remain available.
- [ ] Change A's assessed condition. Confirm the relevant finding updates or needs reassessment.
- [ ] Have another medic view A. Confirm they receive only findings available to that viewer.
- [ ] Check readability at the resolutions and UI scales used by the group, including ultrawide/4K if available.
- [ ] If a controller is available, test menu navigation, treatment selection, hold actions and leaving the menu. Record the controller model.

## 3. Equipment appearance and inventory

- [ ] Inspect every catalogue item in arsenal/inventory. Look for oversized, clipped, blank or backwards icons.
- [ ] Equip and stow the handheld items repeatedly. Confirm the central character preview remains visible.
- [ ] Inspect epinephrine, fentanyl, morphine, naloxone, NCD and TXA in first and third person. The injection end should face upward.
- [ ] Check compact rope, splint, surgery kit, suture kit and sling sizes. Pack them together in a normal backpack.
- [ ] Check the chest-seal face, AED controls, red blood bag, yellow plasma bag and native saline model.
- [ ] Check the BVM from several angles for an extra floating mask.
- [ ] Drop representative small and large items. Inspect them nearby and at distance for disappearing geometry or material errors.

Catalogue coverage: tourniquet, bandage, AED, AED pads, airway adjunct, BVM, chest drain, chest seal, drag rope, folded body bag, epinephrine, fentanyl, blood/fluid bag, monitor, morphine, naloxone, NCD, oxygen, paracetamol, plasma, pulse oximeter, saline, splint, suction, surgery kit, suture kit, TXA, vascular access and weapon sling.

## 4. Medical workflows

Use the training guide and configured role/facility rules to prepare each indication. Record the actual cancellation reason for deliberate interruptions.

- [ ] Bandage/bleeding control and tourniquet application/removal.
- [ ] Left and right leg splints, including an already-treated limb.
- [ ] Airway assessment, adjunct, suction and BVM.
- [ ] Oxygen connection, continued use and disconnection.
- [ ] Pulse oximeter and monitor connection/readings/removal.
- [ ] Chest-seal, NCD and chest-drain workflows with appropriately prepared injuries.
- [ ] Vascular access, blood, plasma and saline connection. Confirm finite contents decrease.
- [ ] Walk the medic out of range after establishing an independent fluid line. Check that the intended line continues.
- [ ] Remove a fluid line with no spare fluid bag in the medic's inventory. Confirm the original partial bag is preserved.
- [ ] Test each medication with an appropriate casualty. Check completion, supply use and resulting patient state.
- [ ] CPR and AED with appropriately prepared casualties. Check role restrictions and displayed results.
- [ ] Surgery and suturing in the configured facility conditions.
- [ ] Test representative procedures with missing supplies, early cancellation and patient movement.
- [ ] Have two medics attempt overlapping work. Check that reservations, consumption and feedback stay consistent.
- [ ] Treat yourself, another player and an AI casualty where supported.
- [ ] Confirm inventory reservations prevent refunding a supply currently in use.

## 5. Living casualty transport

- [ ] Carry and drag an unconscious player. Have the observer inspect pose, collision and movement.
- [ ] Request carry and drag of a conscious player. Test accept, decline, requester cancellation and unanswered expiry.
- [ ] Check that a cancelled/expired request cannot later move the patient.
- [ ] Release from the carrier's menu and from the patient's menu. Confirm both can move and use controls afterward.
- [ ] Walk, turn, crouch and stop while transporting. Check the casualty follows on each client.
- [ ] Test slopes, doorways, walls and proximity to other players.
- [ ] Wake a transported unconscious patient. Confirm release and restored controls.
- [ ] Disconnect the carrier during transport. Check the patient and remaining clients.
- [ ] Repeat conscious/unconscious carry and drag with US, USSR and FIA AI.
- [ ] Check the casualty's clothing, equipment and sling-retained weapon after release.
- [ ] Test stationary stretcher placement, treatment and removal with a player patient.

Two-person stretcher carrying and custom stretcher grip animation remain shelved.

## 6. Body bags and complete equipment preservation

- [ ] Start bagging, then release F early. Confirm cancellation and retention of the folded bag.
- [ ] Complete the default 12-second hold. Check circular progress and one completed bag.
- [ ] Have two players attempt the same corpse/bag simultaneously. Confirm a single consistent result.
- [ ] Search the filled bag. Compare the complete original inventory against the baseline, including all named explosives, accessories, clothing and nested contents.
- [ ] Manually drag the loaded bag. Check actual movement on the owner and observer.
- [ ] Check jump/vault restrictions during manual drag and normal movement after release.
- [ ] Attach a rope, tow, detach and reattach. Check the visible tether and bag ground contact.
- [ ] Test road, grass, slope, doorstep, doorway and a solid obstruction. Look for floating, rolling or passage through walls.
- [ ] Disconnect the dragging player. Confirm release and later reuse by another player.
- [ ] Check the centre interaction prompts and all available bag actions.

## 7. Vehicles and lifecycle

- [ ] Load and unload a fully equipped bag in a ground vehicle. Reconcile the original equipment again.
- [ ] Repeat with MRZR, JLTV and Cougar when available.
- [ ] Repeat with Medical Little Bird, UH-60, HH-60G and ACM MH-47G SOAR Chinook.
- [ ] For aircraft, load, take off, turn, land and unload. Observe from another client.
- [ ] Join the server while the bag is stored. Check the late joiner's inventory/state view.
- [ ] Transfer a bag between vehicles and unload it. Check bag identity and complete contents.
- [ ] Test occupied-vehicle, moving-vehicle and out-of-range rejection.
- [ ] Exercise burial/grave inspection and send-home on separate test casualties.
- [ ] Disconnect during bag search or a pending operation. Confirm reservations clear.
- [ ] If the mission supports persistence, restart the server with stored remains/equipment and inspect restored state.

Use the recorded tested helicopter configuration. HH-60G passed with UH-60 Tweaks and BCM Vehicles. The additional MH60 Variants configuration produced electrical/startup errors and should be logged separately if tested.

## 8. Roles, settings and longer session

- [ ] Confirm ordinary players cannot change restricted medical/GM controls.
- [ ] Grant and revoke medic/admin permissions. Check the affected actions and menus.
- [ ] Change representative treatment durations, consent timeout, transport enable controls and facility restrictions. Check their actual gameplay effects.
- [ ] Verify lives/respawn controls and an actual death/respawn cycle.
- [ ] Keep playing for at least 60 minutes, repeating treatment, equipment swaps, bagging and transport.
- [ ] Record errors, falling FPS, increasing memory, stuck controls, stale menus and inconsistent casualty states.
- [ ] Reconnect and perform a final treatment and cargo cycle.

## Issue record — copy for each failure

- Check number/section:
- PASS / FAIL / BLOCKED:
- Time and tester role:
- Exact reproduction steps:
- Expected behavior:
- Actual behavior and exact message:
- Frequency, for example 2 failures in 5 attempts:
- Workshop version, mission, active addons and relevant settings:
- Patient state and inventory before/after:
- Screenshot/video and server/client log references:

## End-of-session decision

- [ ] Reconcile all inventory tests and list any missing/duplicated entities.
- [ ] Collect server and client logs with timestamps matching the issues.
- [ ] List failed, blocked and untested checks.
- [ ] Decide whether another repair build is required before wider testing.
