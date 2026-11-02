# BCM Medical player test checklist

## Medical menu and assessment

### MENU01 Open the correct patient
Setup: Two players close together. One is the patient and one is the medic.
Do:
1. Look at the patient and press H. Check the name at the top. Look away and open it again.
2. Use SELF / SWAP. Repeat with another patient and with an unconscious patient.
Pass: The menu selects the person you looked at, or yourself when nobody is selected. Treatment must never go to the previous patient.

### MENU02 Choose the correct body part
Setup: A patient with one confirmed wound.
Do:
1. Click head, chest, abdomen (stomach area), both arms and both legs. Open all five treatment tabs and MORE TREATMENTS.
2. Check the wounded part, then the opposite side. Remember that the patient's left is on your right.
Pass: The wound appears on the correct part. All tabs and buttons work and fit on screen.

### MENU03 Check and refresh the findings
Setup: A patient, a medic and another player viewing the patient.
Do:
1. Open the menu without examining. Then examine, treat and examine again.
2. Change patient while a check is running. Close and reopen the menu. Compare the other player's findings.
Pass: Opening the menu alone does not examine anyone. Old results are not shown as fresh results or attached to the wrong patient.

### MENU04 Check patients inside a vehicle
Setup: Two players in the same vehicle and a third player outside.
Do:
1. Open the patient list. Change seats, leave the vehicle and get back in.
2. Try a treatment that is normally blocked while seated.
Pass: The list shows other occupants of your vehicle. Being able to examine them does not unlock blocked vehicle treatments.

### MENU05 Check controls and screen size
Setup: Keyboard and mouse, plus a controller if you plan to use one.
Do:
1. Rebind Medical interface in BCM Medical controls. Try tap and hold, restart, then restore H.
2. Try body selection, paging, cancel and carry/drag buttons at 720p, 1080p and your usual UI size.
Pass: Your binding stays saved. Buttons remain readable and usable. The small carry/drag pair stays at the bottom right without covering the body.

## Bleeding wounds and tourniquets

### WOUND01 Shoot a leg and bandage it
Setup: Two players and two bandages. Start with a healthy patient.
Do:
1. Fire one controlled shot into the left lower leg. Stop shooting. Press H, confirm the patient's name, select that leg and check bleeding.
2. Bandage through the medical menu and check bleeding again. Reset the patient and repeat using the normal look-at/inventory bandage action. Then test the right leg.
Pass: The correct leg shows the wound. Each successful bandage uses one item and controls that wound's bleeding. Both supported interaction routes work.

### WOUND02 Check all body parts and several wounds
Setup: Separate patients or resets for head, chest, stomach, both arms and both legs.
Do:
1. Create and confirm a wound on each part in turn. Also test two wounded parts together and several wounds on one part.
2. Bandage one part, then check every injured part again.
Pass: Only the treated part's wound group is dressed. Other wounded parts still need treatment. Item use and wound counts stay correct.

### WOUND03 Check bandages wearing out
Setup: Similar bandaged wounds and the current dressing-wear settings.
Do:
1. Keep one patient resting, one walking and one running. Check bleeding regularly and time when each dressing reopens.
2. Bandage a reopened wound again. Use shorter valid wear settings for a separate faster test, then restore them.
Pass: Dressings can reopen. Movement wears them faster, and a fresh bandage controls the reopened wound. Allow for the random starting lifetime.

### WOUND04 Apply and remove a tourniquet
Setup: A bleeding limb, a tourniquet and a bandage.
Do:
1. Use the normal game action to apply the tourniquet. Check that limb's TQ marker and bleeding. Dress the wound.
2. Remove/reapply it where the game allows. Repeat on the opposite limb and try an unsuitable torso target.
Pass: Limb bleeding responds correctly. A tourniquet does not close the wound or replace lost blood. Torso use is refused and no spare item appears.

### WOUND05 Check that bleeding is counted once
Setup: One confirmed external wound with no internal bleeding, medicines or fluids.
Do:
1. Record blood loss over equal periods while resting, after bandaging, and after a confirmed reopening.
2. Repeat during a busier server session. Check for a separate internal injury if loss looks too high.
Pass: Blood loss follows the wound and treatment. There is no unexplained second bleeding effect from the same wound.

## Bullet removal stitching and internal bleeding

### SURG01 Remove a retained bullet
Setup: A wound that examination confirms still contains a bullet, controlled bleeding and a surgery kit.
Do:
1. At a facility if required, examine the injured part and choose Explore / remove projectile. Check it again afterwards.
2. Try repeating the procedure after removal. Also try a wound that contains no bullet.
Pass: The confirmed bullet is removed once. Extraction does not automatically stitch the wound or repair separate internal bleeding. Unneeded repeats must not waste a kit.

### SURG02 Stitch a wound
Setup: A suture kit. Test an actively bleeding wound, a dressed wound containing a bullet, and a controlled wound with no bullet.
Do:
1. Try suturing each case. Complete the allowed case and wait beyond its old bandage-wear time.
2. Cause a fresh wound in the same part and examine it again.
Pass: Bleeding or a retained bullet blocks stitching. The properly closed wound stays closed; a new hit can still create a new wound.

### SURG03 Check where surgery is allowed
Setup: Surgical injuries in the head, chest, stomach and a limb. Record the facility setting.
Do:
1. Try the operation away from a facility, with only one player inside its treatment range, and with both inside.
2. Repeat near the range boundary. In a separate test, change the field-surgery setting and restore it afterwards.
Pass: The current rules are enforced. An ordinary vehicle seat or medical pouch does not count as a treatment facility.

### SURG04 Repair internal bleeding
Setup: Confirmed internal bleeding in the head, chest and stomach, tested separately. Control external bleeding first.
Do:
1. Watch the patient's blood level, then apply an external bandage and check again.
2. Perform Repair internal bleeding on the correct part at the required facility. Check any other internal injuries separately.
Pass: A bandage does not fix internal loss. Surgery repairs its own source, but does not refill blood or clear a separate chest collection.

## Broken legs and the stationary stretcher

### LEG01 Move with a broken leg
Setup: A confirmed broken left leg, then right leg, then both. A splint kit.
Do:
1. Try walking, running, crouching, crawling, jumping and climbing before and after applying a splint to the injured leg.
2. Compare speed, pain and blood loss. Also try treating the wrong leg and moving the patient by carry/drag.
Pass: The splint improves movement but does not heal the break. Jumping/climbing stay blocked. Walking/running can cause fracture-related loss; resting, crawling or being carried avoids that particular loss.

### LEG02 Repair a broken leg fully
Setup: A confirmed broken leg, surgical supplies and a permitted medic.
Do:
1. Try fracture surgery away from a medical facility, even with field surgery enabled.
2. Move both players to a facility, complete the repair, then check the fracture and movement again.
Pass: Full fracture repair requires a facility. A completed repair clears that fracture's restrictions, without fixing unrelated wounds or replacing blood.

### LEG03 Use the stationary stretcher
Setup: A GM and an unconscious living patient with recorded injuries and equipment.
Do:
1. Place BCM stationary treatment stretcher from the GM object browser. Use its Place casualty and Remove casualty actions.
2. Try a conscious patient and an out-of-range patient. Have another player and a late joiner inspect it.
Pass: The same patient is placed and removed without healing or losing gear. Invalid use is refused. Two-player stretcher carrying remains disabled.

### LEG04 Check stretcher treatment support
Setup: Comparable bandaged patients on and off the stationary stretcher, plus a procedure needing a facility.
Do:
1. Compare bandage wear and try the facility treatment with both players in range.
2. During treatment, move out of range or remove the patient/stretcher in separate runs.
Pass: Support applies only while the patient qualifies. Removing support cannot leave a treatment running with invalid facility access.

## Airway and chest treatment

### AIR01 Check an obstructed airway
Setup: An unconscious patient with confirmed soft-tissue airway blockage and an airway adjunct. Use a healthy conscious patient for comparison.
Do:
1. Select Head > Airway > Check airway. Insert the adjunct and check again.
2. Try it on a conscious patient and one without the relevant blockage. If you cannot create the blockage, leave that test untested.
Pass: The adjunct treats the suitable blockage. It does not remove foreign material or make the patient breathe automatically. Refused use keeps the item.

### AIR02 Clear foreign material with suction
Setup: A confirmed airway blockage from foreign material and a suction device.
Do:
1. Check the airway, use suction and check again. Compare this with using an airway adjunct alone.
2. Reuse the same suction device on another suitable patient.
Pass: Suction clears its type of blockage and remains in inventory. An adjunct does not substitute for suction. An open airway alone does not prove good breathing.

### AIR03 Seal the correct side of the chest
Setup: A confirmed open chest leak on one side and chest seals. Test left and right separately.
Do:
1. Select Chest > Airway and check breathing. Try the opposite side first, then seal the injured side.
2. Recheck both sides, item count and the patient's condition.
Pass: Only the correct side's leak is fixed. A seal does not also clear tension, drain collected blood or repair an internal bleeding source.

### AIR04 Relieve chest tension
Setup: A confirmed tension problem on the left or right side and a decompression kit.
Do:
1. Check breathing. Try the wrong side, then decompress the confirmed side. Repeat for both sides.
2. Treat any open leak as needed and keep checking for returning problems.
Pass: Correct-side decompression relieves that side's tension. It does not permanently fix an untreated leak or cure every chest injury.

### AIR05 Drain blood collected in the chest
Setup: A confirmed chest blood collection on one side, a drain kit and any required treatment access.
Do:
1. Check both sides. Compare the effect of a dressing/seal with inserting the drain on the correct side.
2. Watch the collection decrease. Repair the bleeding source and replace lost blood separately.
Pass: The drain removes the matching chest collection over time. It does not refill circulation or drain the head/stomach.

### AIR06 Check breathing recovery
Setup: A confirmed chest injury causing poor lung function, without uncontrolled external bleeding.
Do:
1. Record breathing and oxygen readings. Treat the actual leak, tension, collection or bleeding source.
2. Recheck over time. Compare with a similar patient whose cause has not been treated.
Pass: Recovery follows the corrected injuries and server settings. Oxygen alone does not instantly repair a damaged/collapsed lung.

### AIR07 Help breathing with the bag valve mask
Setup: A patient breathing inadequately, a clear airway and a bag valve mask (BVM).
Do:
1. Give assisted ventilation. Watch how long the breathing support lasts, then repeat. Compare with a blocked airway.
2. Cancel one cycle before completion and reuse/transfer the device to another patient.
Pass: Completed cycles provide temporary help, so support may need repeating. The device is reusable. Cancelling must not award a free completed cycle.

## Oxygen and monitoring devices

### OXY01 Connect and use oxygen
Setup: A patient with reduced oxygenation, a clear airway, some functioning lung and a full oxygen kit.
Do:
1. Connect oxygen under Head > Airway. Check Chest > breathing for source, remaining litres and flow. Time the contents falling.
2. Disconnect and reconnect the same kit. Compare with a healthy patient.
Pass: The original supply drains at the set rate and retains its remaining contents. Oxygen helps suitable oxygenation problems; it does not create breaths or replace blood cells.

### OXY02 Check when oxygen stops
Setup: An active oxygen connection with the operator and original kit identified.
Do:
1. In separate runs leave operating range, drop/transfer/delete the kit, make either player unconscious, or kill/reset the patient.
2. After each change, recheck oxygen delivery and source status before adding another kit.
Pass: Invalid source/range/death/reset stops delivery. Unconsciousness alone does not remove an otherwise valid connection. A mask with no working supply gives no oxygen.

### OXY03 Empty and disconnect oxygen
Setup: A nearly empty kit, a full spare and access to the oxygen setting.
Do:
1. Let the original empty, try it again, then disconnect without owning a spare kit.
2. Test oxygen disabled before connection and disabled during use. Check used contents after transfer, reset and save/restart.
Pass: Empty kits stay empty. Disconnecting needs no spare. Disabled oxygen must not keep working invisibly or refill itself.

### MON01 Check vitals with and without equipment
Setup: A patient; first examine without a monitor/oximeter, then with each device.
Do:
1. Check pulse and breathing by hand. Check circulation and perform the instrumented chest check.
2. Change the patient's condition and repeat the checks.
Pass: Hand checks and equipment checks give their intended information. Equipment-only readings need the device. Estimated blood values are labelled; old readings do not update themselves.

### MON02 Use the live pulse oximeter
Setup: A pulse oximeter in the medic's inventory, a patient and another medical viewer.
Do:
1. Start oximetry on an arm. Change the patient's condition and compare both viewers' pulse/oxygen readings. Check the sound setting and range.
2. Move out of device range, transfer/drop it, then end the session. Compare with an older manual reading.
Pass: Live readings/sounds follow the valid device and range. Losing the device stops the feed. The item is not consumed or duplicated.

## No pulse CPR and the AED

### CPR01 Start and stop CPR
Setup: A living patient with a confirmed absent pulse. Use a prepared case if ordinary trauma cannot produce it reliably.
Do:
1. Check pulse, start chest compressions and compare the medic's and observer's views.
2. Stop, cancel and restart. Also let the connection/communication time limit expire and compare the patient's deterioration.
Pass: CPR gives partial blood-flow support, but the spontaneous pulse stays zero until circulation returns. Stopping or losing the session ends unsupported compressions.

### CPR02 Use the AED when a shock is advised
Setup: A living pulseless patient with a confirmed shockable rhythm, electrode pads and a charged AED.
Do:
1. Apply pads and analyse with that AED. Stop CPR and give the advised shock before the analysis expires.
2. Resume needed care and recheck. Test VF and pulseless VT separately when suitable prepared cases are available.
Pass: One completed shock uses one charge on the actual device. Rhythm may change without the patient reviving. Missing kit or active CPR blocks an invalid shock.

### CPR03 Check when a shock is not allowed
Setup: Separate confirmed PEA, asystole, healthy/perfusing and deceased cases. Keep AED equipment available.
Do:
1. Analyse each patient and attempt a discharge when the advice says no shock.
2. Check patient state, pads and remaining charges. Mark a rhythm you cannot create as untested.
Pass: Unshockable rhythms and unsuitable patients are not shocked/revived. A refused shock does not spend a successful discharge.

### CPR04 Check expired or wrong AED advice
Setup: Current shock advice from one AED and a second device.
Do:
1. Try shocking after the advice expires, with the other AED, and after the patient's rhythm changes. Test these separately.
2. Analyse again using the actual device and current patient state.
Pass: Old advice cannot authorise a new shock. Fresh suitable advice can. Swapping/reconnecting devices does not refill them.

### CPR05 Check the AED can run out
Setup: One identified AED with a recorded number of used charges and repeatable shockable cases.
Do:
1. Use it until empty, then attempt another shock.
2. Transfer it, reset/change the patient and save/restart before trying again.
Pass: The same device keeps its used charge count and remains empty when exhausted. Analysis alone is not a free shock.

### CPR06 Restore circulation and check death limits
Setup: Similar living pulseless patients and a team able to provide CPR, breathing support and treatment of the cause.
Do:
1. Treat the actual bleeding, missing blood, airway, chest or drug problem while giving required support. Keep checking pulse/breathing through the recovery period.
2. Compare with an untreated cause. In a separate disposable case, allow prolonged untreated deterioration.
Pass: Proper sustained care can restore spontaneous circulation. CPR, adrenaline or repeated shocks alone do not guarantee revival. A truly dead character stays dead.

## Pain and medicines

### MED01 Check pain and pain relief
Setup: Separate wound, broken-leg and internal-injury cases. Record pain, vitals and aiming before medicine.
Do:
1. Compare pain after dressing, stitching or splinting as appropriate.
2. Give a permitted pain medicine to a stable patient and compare pain, aim, breathing and movement.
Pass: Pain responds to treatment and medicine. Pain relief does not heal fractures/arm damage or replace blood. Breathing/pressure side effects still matter.

### MED02 Give morphine
Setup: A stable patient in pain, one morphine injector and no existing opioid dose.
Do:
1. Select a limb > Medication and give morphine. Check the item count and medication record.
2. Compare pain, breathing and pressure before its effect starts, during it and after it fades. Repeat supported self-use.
Pass: One successful dose uses one injector and produces timed effects. It does not instantly heal the patient or replace blood.

### MED03 Give fentanyl
Setup: A stable patient in pain, a medic, IV access and fentanyl. Record its separate safety settings.
Do:
1. Give fentanyl on a limb. Watch early pain relief and the later breathing effect separately.
2. Repeat without medic status, without IV access and below each safety limit.
Pass: Current role/access/safety rules are enforced without wasting refused doses. Breathing depression can last longer than pain relief.

### MED04 Give paracetamol
Setup: A conscious patient in pain and tablets. Also test unconscious, blocked-airway and poor-breathing cases.
Do:
1. Select Head > Medication and give paracetamol. Watch self-use/handover and wait for pain relief.
2. Try the unsafe cases and different supported stances. Check controls afterwards.
Pass: Tablets give delayed relief and are refused when swallowing is unsafe. A successful dose uses one item and does not leave input stuck.

### MED05 Reverse opioid effects with naloxone
Setup: A patient with an active recorded opioid effect, plus a comparison patient without one.
Do:
1. Give naloxone and watch the change in opioid effects.
2. Keep monitoring after reversal starts wearing off. Try naloxone on the opioid-free patient.
Pass: Reversal is temporary and does not fix traumatic blood loss. Opioid effects can return. The default unnecessary-dose refusal preserves the item.

### MED06 Give epinephrine
Setup: Separate pulseless and pulse-present cases. Record the allow-with-pulse setting and rate/pressure limits.
Do:
1. Give epinephrine when permitted and watch its timed pulse/pressure effect and medication record.
2. Try high pulse, high pressure and pulse-present use with that option disabled.
Pass: The current limits are obeyed. Epinephrine does not replace blood, open the airway or guarantee revival. Refused doses remain available.

### MED07 Give TXA
Setup: An open/dressed wound or unrepaired internal bleed, IV access and TXA. Also test fully repaired injuries and no-IV cases.
Do:
1. Try TXA on a limb. For a successful dose, watch its onset, dressing wear and internal loss.
2. Try repeating it too soon and while its active-dose limit is reached.
Pass: Only eligible use succeeds. TXA supports clotting but does not close wounds, repair the source or add blood cells. Unneeded/early doses are refused.

### MED08 Check medication limits
Setup: Prepared patients close to each current safety limit and counted supplies of all six medicines.
Do:
1. For morphine/fentanyl, test pulse, pressure, breathing and oxygen limits separately: below, at and above. Also test absent pulse. For epinephrine test both upper limits.
2. For every medicine test an early repeat, an allowed repeat and the active-dose limit. Include a patient becoming unsafe during administration.
Pass: The server applies the correct limit at completion and consumes only successful doses. Record each drug/limit separately; leave a condition untested if you cannot create it.

### MED09 Check combined medicines and patient records
Setup: Two patients and controlled, permitted morphine/fentanyl doses with support available.
Do:
1. Compare each opioid alone and both active together. Watch breathing/pressure as well as pain.
2. Switch patient during a request and recheck both medication records. Repeat through unconsciousness and waking.
Pass: Records stay with the correct patient. Effects can overlap. Extra clicks or old replies do not add doses, and cosmetic animation settings cannot bypass safety checks.

## IV access blood and fluids

### FLUID01 Insert an IV
Setup: A patient missing blood, an IV kit and a selected limb.
Do:
1. Try treatments that require IV access before inserting it. Insert the IV and retry the permitted treatment.
2. In a supported removal/prepared case, remove access during fluid delivery and check the result.
Pass: Required IV access is enforced. Losing access stops delivery. A visual cannula or selecting another limb does not invent missing access.

### FLUID02 Compare blood saline and plasma
Setup: Three comparable patients with controlled blood loss, no continuing bleed and IV access. Use one named bag type per patient.
Do:
1. Record volume, red-cell and clotting estimates. Give whole blood, saline and plasma separately and check during/after delivery.
2. Compare pressure improvement with the missing blood components.
Pass: Fluid arrives gradually. Saline adds volume; plasma also supports clotting; whole blood also replaces red cells. Better pressure alone does not mean every deficit is fixed.

### FLUID03 Stop and reuse a partly used bag
Setup: One identified full BCM bag and a patient needing enough fluid.
Do:
1. Stop the held infusion about halfway. Check what reached the patient and what remains in that actual bag.
2. Transfer the partial bag and resume it. Separately test dropping/losing it and changing patient.
Pass: Stopping ends further delivery. The same bag supplies only its remainder. An exhausted held bag is consumed once; it never returns full.

### FLUID04 Check fluid safety stops
Setup: Separate full/healthy, high-pressure, small-deficit and blood-loss cases. Keep each fluid type clearly identified.
Do:
1. Attempt infusions in each case and let a patient cross a stop limit during delivery.
2. Remove the requested bag type while leaving a different type available.
Pass: Current pressure/deficit limits stop or refuse fluid and preserve the unused amount. A different bag cannot silently replace the selected type.

### FLUID05 Watch fluid redistribution
Setup: Patients after controlled blood loss and measured saline/plasma/whole-blood treatment. No continuing bleed; natural blood regeneration off.
Do:
1. Record blood-component estimates immediately, then at regular intervals over several minutes. Include an untreated stable comparison.
2. Compare what remains after saline and plasma redistribute.
Pass: Fluid types retain their intended effects. Cell-free fluid does not create red cells, and stopping bleeding alone does not refill blood under default settings.

### FLUID06 Connect and disconnect a drip
Setup: A patient with IV access and spare inventory space. The medic owns one identified bag. Repeat for all three fluid types.
Do:
1. Connect the chosen drip, check the actual bag moves to the patient, and let the medic walk away. Then disconnect without a spare bag.
2. Transfer the partial bag to a different medic before reconnecting. Also test permitted self-connection.
Pass: Valid delivery continues independently. Disconnection keeps the partial original. A new connection needs the usable bag in that operator's inventory; nothing duplicates.

### FLUID07 Check failed and interrupted drip connections
Setup: Patients with room/full inventories, one active drip, a second medic and a second bag.
Do:
1. Try connecting without space and starting a competing line.
2. Separately drop/transfer/delete the active source, remove IV access, kill/reset the patient and disable lines.
Pass: Invalid connections do not lose or copy bags. Losing required source/access/state stops flow. Two requests cannot make the same line deliver twice.

### FLUID08 Empty a drip and move the patient
Setup: A nearly empty drip bag and another patient with a working line.
Do:
1. Let the first bag empty, inspect the remaining item and try reconnecting it.
2. Carry/drag the other patient and use supported vehicle transport. Stop and check delivery again.
Pass: An empty drip leaves an empty physical bag. A valid existing line can continue during transport, without granting blocked new vehicle treatments or extra flow.

### FLUID09 Check base game saline
Setup: An actual base-game saline item, a separate BCM saline bag and a suitable patient.
Do:
1. Use the base-game item's normal route. Interrupt/retrieve it, then transfer and reuse the same source.
2. Repeat after GM FullHeal and a save/restart. Compare it with BCM saline.
Pass: Stock saline keeps its native delivery/lifetime rules. Reusing, transferring, resetting or restarting must not give a used source a fresh lifetime.

### FLUID10 Check optional fluid overload
Setup: A separate test profile with redistributed-fluid burden enabled, plus a default-disabled comparison.
Do:
1. Prepare a patient who has accumulated redistributed cell-free fluid. Observe lung effects and attempts to give more water-loading fluid. Compare a needle/drain attempt.
2. Allow active recovery, then compare with time spent offline after saving.
Pass: Configured overload can affect breathing and block more fluid. A chest drain does not clear it. Offline time gives no free recovery; disabled mode gives no hidden penalty.

### FLUID11 Check optional blood component recovery
Setup: Recovery enabled separately, a stable patient low in red cells/clotting, and a qualifying facility. Keep volume, pressure and oxygen adequate.
Do:
1. Record recovery. Separately introduce bleeding, low pressure/oxygen, leaving the facility and disabling recovery.
2. Compare default-disabled mode and a saved offline interval.
Pass: Only eligible components recover. Disqualifying conditions pause it. It does not add circulating volume, heal a bleed, revive anyone or give offline gains.

## Cancelling treatments and checking progress

### TXN01 Interrupt each treatment
Setup: A suitable patient and counted original supplies. Repeat for every treatment family, including medicines, surgery, devices and fluids.
Do:
1. In separate runs cancel early/late, move away, raise your weapon, become unconscious, lose the item, lose required role/facility access or remove the injury.
2. After each attempt, recheck the patient, item count and remaining contents. Include a normal completed attempt for comparison.
Pass: Cancelled unspent items are kept. Any fluid already delivered stays delivered, with only the real remainder left. No free completed treatment, double use or stuck session occurs.

### TXN02 Check progress and treatment times
Setup: A medic, patient and observer. Record treatment times and medic speed settings.
Do:
1. Time bandage, splint, surgery, suture and selected drugs/devices. Compare ordinary and medic roles, plus supported stances.
2. Check start, cancel, finish and closing/reopening the menu.
Pass: Only the operator gets their progress display. Timing follows active settings plus handling time. Progress-only field procedures work without a special animation or stuck controls.

### TXN03 Try two medics and repeated clicks
Setup: Two medics with separate counted supplies and one patient.
Do:
1. Both try treating the same injury. Double-click start/cancel/finish and change selected body part/patient during requests.
2. Then treat different suitable injuries at the same time.
Pass: Conflicting work does not heal or consume twice. Separate permitted work remains usable. An old request cannot finish on a newly selected patient.

## Carrying and dragging living players

### MOVE01 Ask to carry a conscious player
Setup: A conscious player, then a conscious player with a broken leg. A rescuer stands within pickup range.
Do:
1. Request carry using the small medical-menu buttons. The patient opens their own medical menu and accepts.
2. Repeat with decline, no reply until timeout, rescuer cancel and out-of-range requests.
Pass: No conscious player moves before accepting. Refused/expired/cancelled offers stay invalid. Accepted carrying keeps the same patient, injuries and gear.

### MOVE02 Ask to drag a conscious player
Setup: A conscious player and a rescuer with dragging enabled.
Do:
1. Send a fresh drag request and accept it. Repeat decline, timeout and cancel.
2. Try accepting a cancelled request, an old carry request or another patient's request.
Pass: Only the correct current drag request can be accepted. Old offers and the wrong player cannot authorise movement.

### MOVE03 Pick up an unconscious player
Setup: A living unconscious patient with recorded injuries and equipment.
Do:
1. Carry them without a consent request, release, then repeat by dragging.
2. Restore consciousness through treatment during each mode. Try moving them again with a new conscious-player request.
Pass: Unconscious pickup needs no acceptance. Waking ends unconsented movement; continuing needs fresh consent. Injuries and gear do not reset.

### MOVE04 Check the carried body position
Setup: A carried patient and observers. Repeat with light/heavy loadouts and different clothing.
Do:
1. Look from front, back and both sides while still, walking and turning. Record a short video.
2. Check head, both arms, shoulder contact, hips, knees and feet. Swap who carries whom.
Pass: Movement is slow as configured. No backwards legs, twisted hips/neck, trapped arms or severe hovering/clipping. Leave the latest carry appearance unapproved until this check passes.

### MOVE05 Move while dragging
Setup: A valid living drag on flat ground and an observer.
Do:
1. Move forward, backwards and sideways while turning to look in different directions.
2. Try standing, sprinting, jumping, vaulting and climbing. Compare with ordinary crouch movement.
Pass: Dragging stays crouched and slower, with free direction changes. Blocked moves must not stand up or launch the casualty.

### MOVE06 Use slopes doors and safe release
Setup: Carry and drag cases near slopes, steps, walls, doors and uneven ground.
Do:
1. Approach each slowly, turn in tight spaces and try releasing where placement is obstructed.
2. Have each participant use their release button. Test an excessive movement step in a prepared case.
Pass: No wall teleport, ground fall-through or permanent attachment. Unsafe movement/placement stops or releases safely, and both players can end the allowed session.

### MOVE07 Try competing or disabled transport
Setup: Two rescuers, one patient, a seated/already transported comparison patient and separate carry/drag switches.
Do:
1. Send competing requests, change target and move out of reach. Disable each mode separately and retry.
2. Try an eligible NPC, which has no player acceptance dialogue.
Pass: Only one valid holder/mode owns the patient. Invalid states are refused. NPC pickup still checks life and eligibility.

### MOVE08 Die disconnect or restart during transport
Setup: Separate carry and drag cases in a disposable saved mission, with original gear recorded. Include a patient with a drip.
Do:
1. Separately kill the patient, kill the rescuer, disconnect each player, reconnect, and save/restart while attached.
2. Check the original patient, equipment, line and controls after each change.
Pass: No ghost attachment, copied/lost patient or stuck controls. Invalid sessions end safely. If restart behaviour is unclear, record it and get it reviewed rather than marking a pass.

## Sling and keeping weapons when unconscious

### SLING01 Keep a rifle using a sling
Setup: Retention policy 1. A rifle with a known optic, magazine and round count; one inventory sling.
Do:
1. Cause controlled BCM unconsciousness while holding the rifle. Inspect hands, inventory and ground from both players, then wake and recheck.
2. Repeat without a sling, then with supported sniper rifles and machine guns.
Pass: With a sling, the exact supported gun, ammo and attachments stay owned. Without it, native dropping applies. No duplicate gun appears; the sling is kept and need not be equipped.

### SLING02 Check all retention policies
Setup: The same identifiable long gun and recorded sling ownership.
Do:
1. Test policy 0 with a sling and policy 2 without one, triggering unconsciousness each time.
2. Under policy 1, drop/transfer the sling before unconsciousness, move it after retention separately, and change the held weapon before the trigger.
Pass: Policy 0 uses native behaviour; policy 2 keeps supported long guns without a sling. The trigger uses the actual current weapon and ownership.

### SLING03 Check other weapons and later death
Setup: A pistol, launcher and active-grenade test on disposable characters. Also use a retained rifle.
Do:
1. Trigger unconsciousness with each unsupported weapon and compare normal game handling.
2. Follow the retained rifle through waking, actual death, bagging and save/restart.
Pass: Unsupported weapons keep native behaviour. Each weapon has one correct location/owner through later events; there is no spare rifle or live grenade duplication.

## Deceased bagging and equipment recovery

### BAG01 Bag a deceased player
Setup: A genuinely dead patient with named gear and one folded bag. Use a living unconscious patient for comparison.
Do:
1. Try bagging the living patient, then bag the dead one within range. Search the result from different players.
2. Cancel or move away during a separate attempt. Check the folded bag and all gear afterwards.
Pass: Only the deceased can be bagged. Success uses one folded bag and creates one named bag with the original gear. Interrupted attempts do not lose or duplicate items.

### BAG02 Recover the correct dropped weapon
Setup: A dead player's recorded dropped weapon nearby, plus an unrelated weapon. Also test the casualty's drop taken by another player or outside recovery range.
Do:
1. Bag the casualty separately in each situation.
2. Search the bag and inspect every weapon's final location and owner.
Pass: Only the eligible recorded casualty drop is recovered. Unrelated, newly owned or out-of-range weapons are not swept into the bag.

### BAG03 Search a bag with several players
Setup: A filled bag with named items and two players searching it.
Do:
1. Move items one at a time. Both players try taking the same item; interrupt a transfer and reopen the bag.
2. Also try bagging the same corpse twice. Reconnect and recheck contents.
Pass: Items end up in one place only. No double loot, second bag or vanished equipment follows a rejected/interrupted action.

### BAG04 Check bodies are not cleaned up too soon
Setup: One unbagged corpse and one filled bag, with timestamps and normal mission cleanup running.
Do:
1. Watch through the current corpse protection period, at least 1800 seconds. Test active bag search/drag/cargo recovery too.
2. Separately test deliberate GM deletion and vehicle deletion on disposable objects.
Pass: Ordinary cleanup respects the protection/recovery rules. Deliberate deletion is recorded separately, and ended recovery sessions do not remain stuck forever.

## Bag dragging rope and cargo

### BAGMOVE01 Drag a filled body bag by hand
Setup: A filled bag on suitable ground and an observer.
Do:
1. Use the normal drag action. Start/stop, turn, crouch/walk and try exceeding the movement limit.
2. Separately raise your weapon, become unconscious, move out of reach and disconnect. Check for a carry option.
Pass: Manual dragging stays within its limits and releases safely when invalid. There is still only one bag and no completed filled-bag carry workflow.

### BAGMOVE02 Drag a bag with a rope
Setup: A filled bag and a rope in the rescuer's inventory. Record the length and release-distance settings.
Do:
1. Attach, walk, turn and crouch; try permitted weapon use. Inspect the bag and tether from another player.
2. Separately exceed separation, remove the rope, die, disconnect and detach manually.
Pass: Valid towing follows the settings. Losing the required rope/state/range releases it safely, without duplicating the bag or leaving an endless tow.

### CARGO01 Load and unload body bags
Setup: A jeep, truck, helicopter and ambulance with suitable fixed cargo space. Test each variant separately.
Do:
1. While stationary and in range, load a named filled bag, inspect deposited gear, move the vehicle, then unload.
2. Search the bag and vehicle. Repeat with observers and a late joiner.
Pass: Supported cargo uses the real bag and gear. Unloading leaves deposited gear in that vehicle. Seats and personal backpacks are not treated as fixed vehicle cargo.

### CARGO02 Try failed loads and two vehicles
Setup: Full/unsuitable cargo, a moving/out-of-range vehicle, and two suitable vehicles. Record named bag contents.
Do:
1. Try invalid and interrupted loads, then simultaneous loading. Load vehicle one, unload and move the bag to vehicle two.
2. Delete/destroy a vehicle separately and inspect where all items end up.
Pass: Invalid transfers lose/copy nothing. Gear left in vehicle one does not magically move to vehicle two. Record deliberate vehicle destruction separately from a transfer bug.

## Burial and sending home

### REMAINS01 Bury a body bag
Setup: A filled bag, natural ground, hard ground and an entrenching tool. Use expendable gear.
Do:
1. Try without the held tool, on unsuitable ground, out of range and while moving. Cancel once, then complete a valid burial.
2. Inspect the memorial from another player, a late joiner and after save/restart.
Pass: Valid burial follows the set time/range, leaves the memorial and disposes remaining bag gear. It does not give back a life. Retrieve wanted items first.

### REMAINS02 Send a body bag home
Setup: A filled bag and the qualifying inactive native healing area, plus unsuitable/active-area comparisons.
Do:
1. Try outside the area and with sending home disabled, then use the permitted action at the correct area.
2. Check the bag, remaining gear and player's life allowance.
Pass: Only the supported location/state succeeds. Disposal removes the bag and remaining gear, without an extra life, free respawn or old Body Bag Admin menu.

## Death protection lives suppression and focus

### DEATH01 Check no instant death and the rescue window
Setup: A fresh patient, a medic and a working fluid source. Record the death/grace settings.
Do:
1. Cause normally lethal damage and check unconscious/living state. Separately let blood reach zero, hit again during the window and give actual fluid before it ends.
2. Run another case untreated until the window expires.
Pass: Protected damage follows BCM unconsciousness rules. Extra hits do not restart the window. Actual fluid can rescue depletion in time, but cannot erase separate terminal oxygen damage or make the player immortal.

### LIFE01 Use and run out of lives
Setup: A disposable test profile with limited lives enabled and a known total, normally four including the first spawn.
Do:
1. Check the allowance after the first spawn, cancelled spawn choices and later actual deployments. Continue until no new deployment is allowed.
2. Reconnect to the same surviving character and try a denied new spawn.
Pass: Actual deployments use the allowance; looking/cancelling does not. The first spawn counts. Reconnecting gives no bonus life and running out does not kill a surviving character.

### LIFE02 Change lives as GM
Setup: A connected injured player, an authorised GM and a normal player.
Do:
1. Use the normal character actions to add/remove/reset allowance. Revoke GM Edit permission and retry; also try as the normal player.
2. Reconnect/save/restart and check the value. Check that unsupported offline targets and retired bag-admin controls are not presented as working features.
Pass: Only authorised supported changes work. Changing allowance does not heal, kill or replace the character. Supported changes persist.

### SUP01 Check incoming suppression
Setup: A healthy player and a shooter using near misses at recorded distances, with/without cover.
Do:
1. Compare close incoming misses, distant shots and the patient's own firing.
2. Fire several near misses, stop, and time the visual effect clearing. Check injuries and blood afterwards.
Pass: Suitable incoming rounds cause local blur/vignette, then release. Own fire does not self-suppress. Suppression alone creates no medical injury.

### SUP02 Change suppression settings
Setup: The same firing test and saved baseline settings.
Do:
1. Test suppression off, blur only, vignette only and stronger/weaker values separately.
2. Disable it during an effect, then restore the original settings.
Pass: The selected visual channels change/clear correctly. Bleeding, health and lives do not change because of these settings.

### FOCUS01 Check focus duration and recovery
Setup: A healthy player with normal optics. Record focus strength, duration and cooldown.
Do:
1. Hold the bound focus control until exhausted. Release/retry before and after cooldown.
2. Test disabled mode and valid zero-duration unlimited mode. Try aiming through optics and opening/closing medical UI.
Pass: Extra focus follows its settings and resets properly after exhaustion. No stuck zoom, camera or input remains through UI use or unconsciousness.

## Applied visuals and presentation

### ART01 Check devices shown on the body
Setup: Patients with applied tourniquet, splint, pads, IV, oxygen mask and supported chest devices.
Do:
1. Inspect the correct body part/side from patient, medic, observer and late joiner. Change clothing/stance and test display range/switches.
2. Remove/reset/kill where supported and look for leftover props.
Pass: Visible devices follow real treatment state and clean up correctly. Hiding/showing them does not provide treatment, a spare item or a refill. Record clipping separately.

### ART02 Check treatment and transport movement
Setup: Supported self/other, standing/prone treatments, CPR, medicine use and progress-only field procedures.
Do:
1. Watch each action start, run, stop and cancel. Check normal controls return.
2. Inspect the sling's fabric/buckles and carried/dragged body position during movement.
Pass: No stuck animation or twisted limb remains. Default splint/surgery/suture work through progress bars. Assess the two original casualty poses; do not expect shelved stretcher carrying.

## Save restart and persistence

### SAVE01 Save the actual mission
Setup: A disposable mission with native persistence enabled, a known profile/save and a backup.
Do:
1. Change one harmless setting, create a wounded player with named gear, save the world and restart using that same profile/save.
2. Compare with restarting without loading the world save.
Pass: Settings and actual world saving are separate. The correct loaded save restores supported state; saving settings alone does not recreate the mission's casualties.

### SAVE02 Restore patient injuries and medicines
Setup: A saved patient with recorded wounds, fracture/splint, bullet, internal/chest/airway injuries and medicine history. Use separate cases as needed.
Do:
1. Save/restart and reconnect as the same account. Confirm the character and gear, then freshly examine each affected part.
2. Recheck pulse/rhythm and redo AED analysis rather than trusting old results.
Pass: Supported medical state restores once, without a free heal or extra dose. Old findings/advice are not fresh measurements and offline time gives no free fluid/recovery.

### SAVE03 Restore partly used equipment
Setup: Identified partly used oxygen, AED, held/drip BCM bags and native saline. Include unused and empty controls.
Do:
1. Record remaining contents/charges, save/restart and reconnect.
2. Retrieve/transfer each original and resume suitable use. Compare after FullHeal and reapplication too.
Pass: Each source keeps its supported used state. No refill or phantom delivery appears after transfer, reset, restart or loss of the real item.

### SAVE04 Restore bags vehicles and stretchers
Setup: Named filled bag, memorial, loaded vehicle and stationary stretcher with a patient. Record gear locations.
Do:
1. Save/restart. Inspect from the owner, observer and late joiner, then search/unload/remove as appropriate.
2. Separately repeat with a deliberately deleted source/vehicle.
Pass: Supported objects restore once with the right patient/gear links. Vehicle gear stays in its recorded cargo. Missing objects do not create copied gear or stuck sessions.

## GM settings and medic permissions

### GM01 Find and save the medical settings
Setup: An authorised GM, an ordinary player, a fresh profile and a backed-up older supported complete profile.
Do:
1. Open BCM Medical settings. Check the fifteen headings and all 279 controls. Change/save/reopen one per group; try the same as the ordinary player.
2. Load the older profile and compare its saved custom values.
Pass: Permissions work and supported old settings migrate to schema 35 without replacing valid choices with fresh defaults.

### GM02 Test each setting in the worksheet
Setup: A saved baseline. Use every row in the settings worksheet as a separate check.
Do:
1. Change one setting to another valid value. Save/reopen/reconnect and repeat the linked test to see the difference. Keep paired minimum/maximum values valid.
2. Restore the baseline and check again. Record the changed value and result beside the setting.
Pass: The saved value produces the intended effect. Test menu limits too. For shelved stretcher controls marked *, check presence/save only and keep carrying/grip disabled.

### GM03 Assign and remove medic permission
Setup: An ordinary player who can be assigned medic by the GM. Record role limits and medic speed reduction.
Do:
1. For surgery, AED, fluids, advanced airway and injections, compare ordinary use, medic-only enabled, medic role granted and role revoked mid-treatment.
2. Test fentanyl's separate role/IV options and compare treatment times.
Pass: Current permission is checked at completion and times follow settings. Client buttons alone cannot grant access. Default-off limits are not treated as permanently medic-only.

### GM04 Check every supply and arsenal setting
Setup: US, USSR and FIA arsenals, a mission using native supply costs, and all 29 BCM items.
Do:
1. Take, inspect, transfer, drop/pick up and use each item. Change its price/refund setting and compare supply totals when taking/returning it.
2. Toggle each faction's availability, reopen/reconnect and restore baseline values.
Pass: All enabled items work under the BCM heading, with correct counts/costs/refunds. Hiding an entry does not delete carried items. Shared temporary models are recorded as art issues.

### GM05 Check removed and shelved features
Setup: The player-focused mod without another AI medical mod.
Do:
1. Inspect GM controls and watch untreated NPC casualties near other NPCs. Treat an NPC manually.
2. Check the old body-bag respawn admin and two-player stretcher workflow.
Pass: Autonomous medical AI and old bag-respawn administration are absent. Players can still treat NPCs. Stretcher carrying stays disabled and no ACE addon is required.

## Multiplayer other mods and performance

### NET01 Compare real players and late joiners
Setup: A dedicated server with three real accounts and a fourth joining later. Record ping.
Do:
1. Repeat successful/interrupted treatment from each family, carry/drag consent, sling retention, bag search/cargo and GM life changes. Swap patient/medic/observer roles.
2. Join another player after treatments and devices are already applied.
Pass: The correct player accepts transport, treatment is applied once, relevant state agrees and private information stays private. Joining does not repeat item use or old requests.

### NET02 Disconnect during actions
Setup: A disposable dedicated session, tested with ordinary low and higher Internet latency.
Do:
1. Disconnect each participant before/near completion of treatment, drip/oxygen, CPR, bag transfer and transport. Reconnect using the same account.
2. Repeat rapid requests under latency and a clean server restart.
Pass: No double use, wrong identity/life allowance, ghost session or endless progress. Original items and already delivered effects remain correct. A local automated client does not replace this check.

### NET03 Run a busy long session
Setup: The expected player count, or numbers increased in stages. Agree acceptable performance limits before testing.
Do:
1. Run simultaneous treatment, live devices/drips, bag search and transport for an extended session. Include players joining/leaving.
2. Record server/client FPS, memory, logs and interaction delay regularly and after cleanup.
Pass: No repeated script errors, steadily growing memory/object counts or worsening delays. Gear, patient state and ownership stay correct under the intended load.

### NET04 Try intended platforms and other mods
Setup: A clean baseline, then one intended extra mod, platform or input combination at a time. Record versions.
Do:
1. Repeat menu, leg bandage, interruption, carry/drag, sling, supply and save tests for each available combination.
2. Compare any failure with the clean baseline before deciding its cause.
Pass: Give every combination its own result. Leave unavailable/untested Linux, console and third-party weapon combinations untested; one Windows pass does not cover them all.

## Record the result

For each test, tick Pass, Fail or Not tested. Use Not tested if you cannot create the required condition. Write down which side, item, medicine or interruption you tried; repeated versions need separate results. Record failures with the test ID, what happened and a screenshot or video.

Stop and report wrong-patient treatment, consent bypass, lost/copied gear, infinite supplies, incorrect life use, stuck controls, repeated script errors or clearly wrong treatment/death results. Record appearance issues separately. Recheck fixed problems before approving the build.

Tester: __________  Build: __________  Date: __________

Failed or untested checks: __________  Evidence or notes: __________

## Appendix A Settings worksheet

Use GM02 above for each row. Change one setting, check its effect, then restore it. Defaults below are references; your server may use different values. Write the test result and changed/restored value in your notes.

<!-- GENERATED SETTINGS WORKSHEET -->

Controls marked * belong to shelved stretcher carrying. Keep those features off; check that the controls exist and save only. Use GM02 and the listed checks for other rows.

### Bleeding

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Bandage minimum wear time (seconds) (`bandageReopenMinSeconds`) | 300 | WOUND | Not tested |
| Bandage maximum wear time (seconds) (`bandageReopenMaxSeconds`) | 600 | WOUND | Not tested |
| Walking dressing wear multiplier (`walkingWoundWearMultiplier`) | 1.5 | WOUND | Not tested |
| Running dressing wear multiplier (`runningWoundWearMultiplier`) | 3 | WOUND | Not tested |
| Reopened wound bleeding fraction (`reopenedBleedingMultiplier`) | 0.65 | WOUND | Not tested |
| Bandage application time (seconds) (`bandageApplicationSeconds`) | 10 | WOUND | Not tested |
| Show applied tourniquets (`equipmentVisualTourniquetsEnabled`) | On | WOUND | Not tested |
| Maximum dilution bleeding increase (`fluidDilutionBleedMultiplier`) | 0.5 | WOUND | Not tested |
| Maximum dilution dressing wear increase (`fluidDilutionWearMultiplier`) | 1 | WOUND | Not tested |

### Airway and breathing

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Resting respiratory rate (`normalRespiratoryRate`) | 16 | AIR | Not tested |
| Oxygen response per second (`oxygenChangePerSecond`) | 0.015 | AIR | Not tested |
| Hypoxic arrest oxygen fraction (`arrestOxygenFraction`) | 0.3 | AIR | Not tested |
| Unconscious oxygen fraction (`unconsciousOxygenFraction`) | 0.65 | AIR | Not tested |
| Chest seal application time (seconds) (`chestSealApplicationSeconds`) | 15 | AIR | Not tested |
| Decompression procedure time (seconds) (`decompressionSeconds`) | 15 | AIR | Not tested |
| Airway adjunct insertion time (seconds) (`airwayInsertionSeconds`) | 10 | AIR | Not tested |
| Airway suction time (seconds) (`suctionSeconds`) | 10 | AIR | Not tested |
| Assisted ventilation action time (seconds) (`ventilationCycleSeconds`) | 3 | AIR | Not tested |
| Ventilation support per completed cycle (seconds) (`ventilationSupportSeconds`) | 5 | AIR | Not tested |
| Chest drain insertion time (seconds) (`chestDrainInsertionSeconds`) | 30 | AIR | Not tested |
| Installed drain removal rate (game ml per second) (`chestDrainMlPerSecond`) | 10 | AIR | Not tested |
| Lung recovery after corrected chest cause (fraction per second) (`lungReinflationPerSecond`) | 0.01 | AIR | Not tested |
| Chest penetration injury chance at full impact (`penetratingChestInjuryChance`) | 0.35 | AIR | Not tested |
| Tension chance within a generated chest injury (`initialTensionChance`) | 0.2 | AIR | Not tested |
| Maximum initial traumatic lung collapse fraction (`initialLungCollapseFraction`) | 0.35 | AIR | Not tested |
| Airway contamination chance at full head impact (`headAirwayOcclusionChance`) | 0.15 | AIR | Not tested |
| Generate soft-tissue airway obstruction while unconscious (`automaticUnconsciousAirwayObstruction`) | Off | AIR | Not tested |
| Minimum adequate ventilation rate (`adequateRespiratoryRate`) | 8 | AIR | Not tested |
| Portable oxygen enabled (`oxygenEnabled`) | On | OXY | Not tested |
| Connect oxygen equipment (seconds) (`oxygenApplicationSeconds`) | 10 | OXY | Not tested |
| Disconnect oxygen equipment (seconds) (`oxygenRemovalSeconds`) | 3 | OXY | Not tested |
| New portable oxygen capacity (litres) (`oxygenCapacityLitres`) | 300 | OXY | Not tested |
| Portable oxygen flow (litres/minute) (`oxygenFlowLitresPerMinute`) | 10 | OXY | Not tested |
| Oxygen operator maximum range (metres) (`oxygenRangeMetres`) | 3 | OXY | Not tested |
| Oxygen support of remaining lung capacity (`oxygenLungCompensation`) | 0.5 | AIR | Not tested |
| Minimum lung fraction for oxygen benefit (`oxygenMinimumLungFraction`) | 0.1 | OXY | Not tested |
| Oxygen connection requires medic (`medicOnlyOxygen`) | Off | OXY | Not tested |
| Show applied oxygen mask (`equipmentVisualOxygenEnabled`) | On | OXY | Not tested |
| Mask right offset (m) (`maskOffsetRight`) | 0 | OXY | Not tested |
| Mask up offset (m) (`maskOffsetUp`) | -0.08 | OXY | Not tested |
| Mask forward offset (m) (`maskOffsetForward`) | 0.09 | OXY | Not tested |
| Mask scale (`maskScale`) | 1.45 | OXY | Not tested |
| Mask yaw adjustment (`maskYawDegrees`) | 0 | OXY | Not tested |
| Mask pitch adjustment (`maskPitchDegrees`) | 90 | OXY | Not tested |
| Mask roll adjustment (`maskRollDegrees`) | 0 | OXY | Not tested |

### Circulation

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Nominal blood volume (mL) (`normalBloodVolumeMl`) | 5000 | FLUID | Not tested |
| Resting heart rate (`normalHeartRate`) | 75 | FLUID | Not tested |
| Instrumented vital check time (seconds) (`monitorCheckSeconds`) | 5 | FLUID | Not tested |
| Vascular access insertion time (seconds) (`ivAccessSeconds`) | 15 | FLUID | Not tested |
| Fluid bag delivery time (seconds) (`fluidInfusionSeconds`) | 60 | FLUID | Not tested |
| Fluid reserve delivered by one bag (game ml) (`fluidBagVolumeMl`) | 500 | FLUID | Not tested |
| Pulse oximeter setup time (seconds) (`pulseOximeterApplicationSeconds`) | 5 | MON | Not tested |
| Pulse oximeter operating range (metres) (`pulseOximeterRangeMetres`) | 3 | MON | Not tested |
| Pulse oximeter audible range (metres) (`pulseOximeterAudibleMetres`) | 8 | MON | Not tested |
| Pulse oximeter sound enabled (`pulseOximeterSoundEnabled`) | On | MON | Not tested |
| Show applied chest devices (`equipmentVisualChestDevicesEnabled`) | On | FLUID | Not tested |
| Blood-loss compensation pulse boost (`compensatoryHeartRateBoost`) | 80 | FLUID | Not tested |
| Late shock decompensation fraction (`shockDecompensationBloodFraction`) | 0.35 | FLUID | Not tested |
| Lower efficient pumping rate (`minimumEfficientHeartRate`) | 50 | FLUID | Not tested |
| Upper efficient pumping rate (`maximumEfficientHeartRate`) | 180 | FLUID | Not tested |
| Allow native passive blood regeneration (`naturalBloodRegenerationEnabled`) | Off | FLUID | Not tested |
| Allow stabilised blood-component recovery (`componentRecoveryEnabled`) | Off | FLUID | Not tested |
| Red-cell recovery timescale (seconds) (`redCellRecoverySeconds`) | 3600 | FLUID | Not tested |
| Clotting recovery timescale (seconds) (`clottingRecoverySeconds`) | 900 | FLUID | Not tested |
| Minimum volume for component recovery (`componentRecoveryMinimumBloodFraction`) | 0.8 | FLUID | Not tested |
| Require facility for component recovery (`componentRecoveryRequiresFacility`) | On | FLUID | Not tested |
| Minimum pressure for component recovery (`componentRecoveryMinimumSystolicPressure`) | 90 | FLUID | Not tested |
| Allow independent fluid drips (`continuousFluidLinesEnabled`) | On | FLUID | Not tested |
| Fluid line connection time (seconds) (`fluidLineConnectionSeconds`) | 8 | FLUID | Not tested |
| Fluid line disconnection time (seconds) (`fluidLineRemovalSeconds`) | 3 | FLUID | Not tested |
| Model redistributed fluid overload (`fluidOverloadEnabled`) | Off | FLUID | Not tested |
| Fluid burden onset fraction (`fluidOverloadThresholdFraction`) | 0.15 | FLUID | Not tested |
| Severe fluid burden fraction (`fluidOverloadSevereFraction`) | 0.4 | FLUID | Not tested |
| Fluid burden clearance timescale (`fluidOverloadClearanceSeconds`) | 1800 | FLUID | Not tested |
| Lung fraction at severe fluid burden (`fluidOverloadMinimumLungFraction`) | 0.65 | FLUID | Not tested |
| Withhold additional water-loading fluids (`fluidOverloadStopAdditionalWater`) | On | FLUID | Not tested |
| Water-loading stop severity (`fluidOverloadStopSeverity`) | 0.5 | FLUID | Not tested |

### Resuscitation

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| BCM depletion grace (seconds) (`bloodDepletionGraceSeconds`) | 60 | CPR | Not tested |
| Resting systolic pressure (`normalSystolicPressure`) | 120 | CPR | Not tested |
| Circulatory arrest blood fraction (`arrestBloodFraction`) | 0.25 | CPR | Not tested |
| Critical-state time before arrest (seconds) (`arrestOnsetSeconds`) | 10 | CPR | Not tested |
| CPR perfusion fraction (`cprPerfusionFraction`) | 0.25 | CPR | Not tested |
| Corrected reversible causes before ROSC (seconds) (`roscStabilitySeconds`) | 10 | CPR | Not tested |
| Maximum hypoxic injury debt (seconds) (`hypoxicDebtLimitSeconds`) | 180 | CPR | Not tested |
| AED pad application time (seconds) (`aedPadApplicationSeconds`) | 20 | CPR | Not tested |
| AED analysis time (seconds) (`aedAnalysisSeconds`) | 5 | CPR | Not tested |
| AED stand-clear and discharge time (seconds) (`aedShockSeconds`) | 5 | CPR | Not tested |
| Maximum shock analysis age (seconds) (`aedAnalysisValiditySeconds`) | 30 | CPR | Not tested |
| Available discharges per AED device (`aedShocksPerDevice`) | 20 | CPR | Not tested |
| Critical slow pulse threshold (`criticalBradycardiaRate`) | 25 | CPR | Not tested |
| Critical fast pulse threshold (`criticalTachycardiaRate`) | 220 | CPR | Not tested |
| Critical perfusion pressure (`criticalSystolicPressure`) | 45 | CPR | Not tested |
| Rate instability pressure ceiling (`criticalRatePressureCeiling`) | 90 | CPR | Not tested |
| Unstable vital exposure time (seconds) (`criticalVitalExposureSeconds`) | 15 | CPR | Not tested |
| Critical oxygen-delivery fraction (`criticalOxygenDeliveryFraction`) | 0.15 | CPR | Not tested |
| Recovery oxygen-delivery fraction (`recoveryOxygenDeliveryFraction`) | 0.3 | CPR | Not tested |

### Surgery

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Retained projectile chance (`retainedBulletChance`) | 0.4 | SURG | Not tested |
| Head and torso surgery requires facility (`criticalSurgeryFacilityOnly`) | On | SURG | Not tested |
| Surgery procedure time (seconds) (`surgeryDurationSeconds`) | 60 | SURG | Not tested |
| Suture procedure time (seconds) (`sutureDurationSeconds`) | 60 | SURG | Not tested |
| Medical facility treatment radius (metres) (`medicalFacilityRangeMetres`) | 5 | SURG | Not tested |
| Internal injury chance at full penetrating impact (`penetratingInternalBleedChance`) | 0.4 | SURG | Not tested |
| Maximum initial internal source flow (game ml per second) (`internalBleedMaxMlPerSecond`) | 6 | SURG | Not tested |

### Fractures

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Unsplinted leg movement fraction (`fractureUnsplitSpeedScale`) | 0.08 | LEG | Not tested |
| Splinted leg movement fraction (`fractureSplintedSpeedScale`) | 0.75 | LEG | Not tested |
| Splinted walking internal loss (ml/s) (`fractureWalkingBleedMlPerSecond`) | 0.15 | LEG | Not tested |
| Splinted running internal loss (ml/s) (`fractureRunningBleedMlPerSecond`) | 0.45 | LEG | Not tested |
| Splint application time (seconds) (`splintApplicationSeconds`) | 30 | LEG | Not tested |
| Show applied leg splints (`equipmentVisualSplintsEnabled`) | On | LEG | Not tested |

### Medical interactions

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Treatment interaction range (metres) (`treatmentRangeMetres`) | 2 | TXN | Not tested |
| Examination duration (seconds) (`examinationDurationSeconds`) | 2 | MENU | Not tested |
| CPR communication lease (seconds) (`cprLeaseSeconds`) | 5 | TXN | Not tested |
| Treatment movement tolerance (metres) (`treatmentMovementToleranceMetres`) | 0.5 | TXN | Not tested |
| Maximum procedure equipment preparation time (seconds) (`procedurePreparationSeconds`) | 15 | TXN | Not tested |
| Procedure animation completion grace (seconds) (`procedureCompletionGraceSeconds`) | 5 | TXN | Not tested |
| Medic time reduction (%) (`medicTimeReductionPercent`) | 40 | GM03 | Not tested |
| Only medics may perform surgery (`medicOnlySurgery`) | Off | GM03 | Not tested |
| Only medics may operate AEDs (`medicOnlyAED`) | Off | GM03 | Not tested |
| Only medics may insert IVs and administer fluids (`medicOnlyFluids`) | Off | GM03 | Not tested |
| Only medics may perform advanced airway procedures (`medicOnlyAdvancedAirway`) | Off | GM03 | Not tested |
| Only medics may administer injectable medication (`medicOnlyMedication`) | Off | GM03 | Not tested |
| Applied medical equipment visuals (`equipmentVisualsEnabled`) | On | GM03 | Not tested |
| Applied equipment display range (metres) (`equipmentVisualRangeMetres`) | 40 | ART01 | Not tested |
| Progress-only splint, surgery and suturing (`fieldProcedureProgressOnly`) | On | TXN | Not tested |
| Unconscious weapon retention (`weaponRetentionPolicy`) | 1 | SLING | Not tested |

### Body bags

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Bagging duration (seconds) (`baggingDurationSeconds`) | 3 | BAG | Not tested |
| Bagging interaction range (metres) (`baggingRangeMetres`) | 2 | BAG | Not tested |
| Dropped casualty equipment recovery radius (metres) (`deathEquipmentRecoveryRangeMetres`) | 5 | BAG | Not tested |
| Allow body-bag burial (`burialEnabled`) | On | REMAINS | Not tested |
| Body-bag burial duration (seconds) (`burialDurationSeconds`) | 30 | REMAINS | Not tested |
| Body-bag burial range (metres) (`burialRangeMetres`) | 4 | REMAINS | Not tested |
| Allow sending body bags home (`sendHomeEnabled`) | On | REMAINS | Not tested |

### Transport

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Body bag rope length (metres) (`ropeLengthMetres`) | 2 | BAGMOVE | Not tested |
| Rope safety release distance (metres) (`ropeBreakDistanceMetres`) | 6 | BAGMOVE | Not tested |
| Conscious carry consent timeout (seconds) (`carryConsentTimeoutSeconds`) | 15 | MOVE | Not tested |
| Vehicle cargo interaction range (metres) (`cargoInteractionRangeMetres`) | 5 | CARGO | Not tested |
| Maximum loading vehicle speed (m/s) (`cargoMaxVehicleSpeedMetresPerSecond`) | 0.5 | CARGO | Not tested |
| Bag follow speed limit (metres/second) (`bagDragMaxSpeedMetresPerSecond`) | 8 | BAGMOVE | Not tested |
| Bag follow responsiveness (`bagDragResponsiveness`) | 6 | BAGMOVE | Not tested |
| Manual drag movement limit (metres/second) (`manualBagDragMaxSpeedMetresPerSecond`) | 2.5 | BAGMOVE | Not tested |
| Two-player stretcher carrying * (`stretcherCarryEnabled`) | Off | Shelved | Not tested |
| Stretcher handle reach (metres) * (`stretcherJoinRangeMetres`) | 1 | Shelved | Not tested |
| Wait for second stretcher carrier (seconds) * (`stretcherPairTimeoutSeconds`) | 20 | Shelved | Not tested |
| Minimum carrier separation (metres) * (`stretcherMinimumSpanMetres`) | 1.7 | Shelved | Not tested |
| Maximum carrier separation (metres) * (`stretcherMaximumSpanMetres`) | 3.4 | Shelved | Not tested |
| Stretcher carry safety speed (metres/second) * (`stretcherMaximumSpeedMetresPerSecond`) | 2.2 | Shelved | Not tested |
| Stretcher carrier movement fraction * (`stretcherCarrySpeedFraction`) | 0.3 | Shelved | Not tested |
| Stretcher lift height (metres) * (`stretcherCarryHeightMetres`) | 0.85 | Shelved | Not tested |
| Maximum carrier height difference (metres) * (`stretcherMaximumHeightDifferenceMetres`) | 0.6 | Shelved | Not tested |
| Stretcher movement protection (`stretcherStabilityEnabled`) | On | LEG | Not tested |
| Stretcher dressing wear reduction (`stretcherDressingWearReduction`) | 0.25 | LEG | Not tested |
| Stretcher carrier end offset (metres) * (`stretcherHandleOffsetMetres`) | 1.5 | Shelved | Not tested |
| Stretcher maximum turn rate (degrees/second) * (`stretcherMaximumTurnDegreesPerSecond`) | 120 | Shelved | Not tested |
| Stretcher hand-grip animation * (`stretcherGripAnimationEnabled`) | Off | Shelved | Not tested |
| Stretcher grip blend time (seconds) * (`stretcherGripBlendSeconds`) | 0.25 | Shelved | Not tested |
| Living casualty carrying (`casualtyCarryEnabled`) | On | MOVE | Not tested |
| Living casualty dragging (`casualtyDragEnabled`) | On | MOVE | Not tested |
| Casualty pickup and request range (`casualtyTransportRangeMetres`) | 2 | MOVE | Not tested |
| Casualty carry movement fraction (`casualtyCarrySpeedFraction`) | 0.35 | MOVE | Not tested |
| Casualty crouch-drag movement fraction (`casualtyDragSpeedFraction`) | 0.25 | MOVE | Not tested |
| Casualty transport movement break distance (`casualtyTransportMaxStepMetres`) | 1.5 | MOVE | Not tested |

### Suppression

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Suppression enabled (`suppressionEnabled`) | On | SUP | Not tested |
| Suppression release (seconds) (`suppressionReleaseSeconds`) | 3 | SUP | Not tested |
| Suppression blur (%) (`suppressionBlurPercent`) | 12 | SUP | Not tested |
| Suppression vignette (%) (`suppressionVignettePercent`) | 18 | SUP | Not tested |
| Suppression blur multiplier (`suppressionBlurMultiplier`) | 1 | SUP | Not tested |
| Suppression vignette multiplier (`suppressionVignetteMultiplier`) | 1 | SUP | Not tested |

### Lives

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Limit player lives (`livesEnabled`) | On | LIFE | Not tested |
| Total lives including initial deployment (`totalLives`) | 4 | LIFE | Not tested |

### Focus

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Focus budget enabled (`focusEnabled`) | On | FOCUS | Not tested |
| Extra focus strength (%) (`focusStrengthPercent`) | 50 | FOCUS | Not tested |
| Focus maximum duration (0 unlimited) (`focusDurationSeconds`) | 10 | FOCUS | Not tested |
| Focus cooldown (seconds) (`focusCooldownSeconds`) | 30 | FOCUS | Not tested |

### Pain

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Pain per open wound (game units) (`painWoundUnits`) | 18 | MED01 | Not tested |
| Dressed wound pain multiplier (`painDressedMultiplier`) | 0.6 | MED01 | Not tested |
| Closed wound pain multiplier (`painSuturedMultiplier`) | 0.2 | MED01 | Not tested |
| Pain per unsupported fracture (game units) (`painFractureUnits`) | 45 | MED01 | Not tested |
| Splinted fracture pain multiplier (`painSplintMultiplier`) | 0.5 | MED01 | Not tested |
| Pain per active internal injury (game units) (`painInternalUnits`) | 25 | MED01 | Not tested |
| Maximum pain contribution to native aiming damage (`painAimingPenalty`) | 0.4 | MED01 | Not tested |
| Maximum pain pulse increase (`painHeartRateBoost`) | 15 | MED01 | Not tested |

### Medication

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| Medication application time (seconds) (`medicationApplicationSeconds`) | 10 | MED | Not tested |
| Minimum repeat interval per medication (seconds) (`medicationRepeatSeconds`) | 60 | MED | Not tested |
| Maximum concurrent doses per medication (`medicationMaxActiveDoses`) | 3 | MED | Not tested |
| Morphine onset time (seconds) (`morphineOnsetSeconds`) | 30 | MED | Not tested |
| Morphine effect duration (seconds) (`morphineDurationSeconds`) | 600 | MED | Not tested |
| Morphine pain relief per active dose (game units) (`morphinePainReliefUnits`) | 45 | MED | Not tested |
| Morphine breathing suppression per active dose (`morphineRespiratoryDepression`) | 0.35 | MED | Not tested |
| Morphine pressure suppression per active dose (`morphinePressureDepression`) | 0.15 | MED | Not tested |
| Naloxone onset time (seconds) (`naloxoneOnsetSeconds`) | 5 | MED | Not tested |
| Naloxone effect duration (seconds) (`naloxoneDurationSeconds`) | 180 | MED | Not tested |
| Naloxone opioid suppression per active dose (`naloxoneReversalFraction`) | 1 | MED | Not tested |
| Epinephrine onset time (seconds) (`epinephrineOnsetSeconds`) | 10 | MED | Not tested |
| Epinephrine effect duration (seconds) (`epinephrineDurationSeconds`) | 180 | MED | Not tested |
| Epinephrine pulse increase per active dose (`epinephrineHeartRateBoost`) | 25 | MED | Not tested |
| Epinephrine pressure increase per active dose (`epinephrinePressureBoost`) | 15 | MED | Not tested |
| TXA administration time (seconds) (`txaApplicationSeconds`) | 15 | MED | Not tested |
| TXA onset time (seconds) (`txaOnsetSeconds`) | 30 | MED | Not tested |
| TXA effect duration (seconds) (`txaDurationSeconds`) | 600 | MED | Not tested |
| TXA clot breakdown reduction (`txaClotWearReduction`) | 0.5 | MED | Not tested |
| TXA minimum repeat interval (seconds) (`txaRepeatSeconds`) | 600 | MED | Not tested |
| TXA maximum active administrations (`txaMaxActiveDoses`) | 1 | MED | Not tested |
| TXA requires vascular access (`txaRequiresVascularAccess`) | On | MED | Not tested |
| TXA restricted to medics (`txaMedicOnly`) | Off | MED | Not tested |
| Show applied IV access (`equipmentVisualIVEnabled`) | On | MED | Not tested |
| Medication vital-sign safeguards (`medicationVitalChecksEnabled`) | On | MED | Not tested |
| Morphine minimum pulse (bpm) (`morphineMinimumHeartRate`) | 50 | MED | Not tested |
| Morphine minimum systolic pressure (`morphineMinimumSystolicPressure`) | 90 | MED | Not tested |
| Morphine minimum breathing rate (`morphineMinimumRespiratoryRate`) | 10 | MED | Not tested |
| Morphine minimum oxygen saturation (`morphineMinimumOxygenFraction`) | 0.9 | MED | Not tested |
| Epinephrine maximum pulse (bpm) (`epinephrineMaximumHeartRate`) | 180 | MED | Not tested |
| Epinephrine maximum systolic pressure (`epinephrineMaximumSystolicPressure`) | 180 | MED | Not tested |
| Allow epinephrine with a pulse (`epinephrineWithPulseEnabled`) | On | MED | Not tested |
| Naloxone requires active opioid (`naloxoneRequiresOpioid`) | On | MED | Not tested |
| Fluid vital-sign safeguards (`fluidSafetyChecksEnabled`) | On | MED | Not tested |
| Fluid stop systolic pressure (`fluidMaximumSystolicPressure`) | 140 | MED | Not tested |
| Minimum fluid deficit (ml) (`fluidMinimumDeficitMl`) | 50 | MED | Not tested |
| TXA internal clot support fraction (`txaInternalBleedReduction`) | 0.2 | MED | Not tested |
| Simulate fluid composition (`fluidCompositionEnabled`) | On | MED | Not tested |
| Saline retained volume fraction (`salineRetainedFraction`) | 0.25 | MED | Not tested |
| Plasma retained volume fraction (`plasmaRetainedFraction`) | 0.8 | MED | Not tested |
| Fluid redistribution time scale (seconds) (`fluidRedistributionSeconds`) | 300 | MED | Not tested |
| Tablet administration time (seconds) (`paracetamolApplicationSeconds`) | 10 | MED | Not tested |
| Paracetamol onset (game seconds) (`paracetamolOnsetSeconds`) | 60 | MED | Not tested |
| Paracetamol clearance (game seconds) (`paracetamolDurationSeconds`) | 1200 | MED | Not tested |
| Tablet minimum repeat interval (seconds) (`paracetamolRepeatSeconds`) | 300 | MED | Not tested |
| Maximum active tablet administrations (`paracetamolMaxActiveDoses`) | 2 | MED | Not tested |
| Tablet pain relief (game units) (`paracetamolPainReliefUnits`) | 20 | MED | Not tested |
| Minimum breathing rate for oral medicine (`oralMinimumRespiratoryRate`) | 10 | MED | Not tested |
| Fentanyl administration (seconds) (`fentanylApplicationSeconds`) | 10 | MED | Not tested |
| Fentanyl analgesic onset (game seconds) (`fentanylOnsetSeconds`) | 10 | MED | Not tested |
| Fentanyl analgesic clearance (seconds) (`fentanylDurationSeconds`) | 180 | MED | Not tested |
| Fentanyl breathing-effect onset (seconds) (`fentanylRespiratoryOnsetSeconds`) | 20 | MED | Not tested |
| Fentanyl breathing-effect clearance (seconds) (`fentanylRespiratoryDurationSeconds`) | 300 | MED | Not tested |
| Fentanyl minimum repeat interval (seconds) (`fentanylRepeatSeconds`) | 120 | MED | Not tested |
| Maximum active fentanyl administrations (`fentanylMaxActiveDoses`) | 2 | MED | Not tested |
| Fentanyl pain relief (game units) (`fentanylPainReliefUnits`) | 70 | MED | Not tested |
| Fentanyl breathing depression fraction (`fentanylRespiratoryDepression`) | 0.5 | MED | Not tested |
| Fentanyl pressure depression fraction (`fentanylPressureDepression`) | 0.2 | MED | Not tested |
| Fentanyl minimum systolic pressure (`fentanylMinimumSystolicPressure`) | 100 | MED | Not tested |
| Fentanyl minimum pulse (bpm) (`fentanylMinimumHeartRate`) | 50 | MED | Not tested |
| Fentanyl minimum breathing rate (`fentanylMinimumRespiratoryRate`) | 12 | MED | Not tested |
| Fentanyl minimum oxygen fraction (`fentanylMinimumOxygenFraction`) | 0.92 | MED | Not tested |
| Fentanyl requires vascular access (`fentanylRequiresVascularAccess`) | On | MED | Not tested |
| Fentanyl requires medic (`fentanylMedicOnly`) | On | MED | Not tested |
| Tablet hand-to-mouth animation (`tabletAnimationEnabled`) | On | MED | Not tested |

### Arsenal

| Control and exact key | Fresh default | Run with GM02 | Result and evidence |
| --- | --- | --- | --- |
| US medical supplies (`arsenalUSEnabled`) | On | GM04 | Not tested |
| USSR medical supplies (`arsenalUSSREnabled`) | On | GM04 | Not tested |
| FIA medical supplies (`arsenalFIAEnabled`) | On | GM04 | Not tested |
| Medical supply refund fraction (`arsenalRefundFraction`) | 0 | GM04 | Not tested |
| Tourniquet supply cost (`arsenalCostTourniquet`) | 0 | GM04 | Not tested |
| Bandage supply cost (`arsenalCostBandage`) | 0 | GM04 | Not tested |
| AED supply cost (`arsenalCostAED`) | 30 | GM04 | Not tested |
| AED electrode pads supply cost (`arsenalCostAEDPads`) | 3 | GM04 | Not tested |
| AirwayAdjunct (placeholder) supply cost (`arsenalCostAirwayAdjunct`) | 3 | GM04 | Not tested |
| Bag valve mask supply cost (`arsenalCostBVM`) | 8 | GM04 | Not tested |
| ChestDrain (placeholder) supply cost (`arsenalCostChestDrain`) | 3 | GM04 | Not tested |
| Vented chest seal supply cost (`arsenalCostChestSeal`) | 3 | GM04 | Not tested |
| Body-bag drag rope supply cost (`arsenalCostDragRope`) | 2 | GM04 | Not tested |
| Body bag (empty) supply cost (`arsenalCostEmptyBodyBag`) | 3 | GM04 | Not tested |
| Epinephrine supply cost (`arsenalCostEpinephrine`) | 3 | GM04 | Not tested |
| Fentanyl supply cost (`arsenalCostFentanyl`) | 3 | GM04 | Not tested |
| Whole blood bag supply cost (`arsenalCostFluidBag`) | 3 | GM04 | Not tested |
| Medical monitor supply cost (`arsenalCostMonitor`) | 15 | GM04 | Not tested |
| Morphine supply cost (`arsenalCostMorphine`) | 3 | GM04 | Not tested |
| Naloxone supply cost (`arsenalCostNaloxone`) | 3 | GM04 | Not tested |
| NCD kit supply cost (`arsenalCostNCD`) | 3 | GM04 | Not tested |
| Portable oxygen kit supply cost (`arsenalCostOxygen`) | 15 | GM04 | Not tested |
| Paracetamol tablets supply cost (`arsenalCostParacetamol`) | 3 | GM04 | Not tested |
| Plasma bag supply cost (`arsenalCostPlasma`) | 3 | GM04 | Not tested |
| Pulse oximeter supply cost (`arsenalCostPulseOximeter`) | 3 | GM04 | Not tested |
| Saline bag supply cost (`arsenalCostSaline`) | 3 | GM04 | Not tested |
| Packaged leg splint supply cost (`arsenalCostSplint`) | 5 | GM04 | Not tested |
| Portable medical suction supply cost (`arsenalCostSuction`) | 10 | GM04 | Not tested |
| Surgery kit supply cost (`arsenalCostSurgeryKit`) | 10 | GM04 | Not tested |
| Suture kit supply cost (`arsenalCostSutureKit`) | 5 | GM04 | Not tested |
| TXA supply cost (`arsenalCostTXA`) | 3 | GM04 | Not tested |
| IV cannula kit supply cost (`arsenalCostVascularAccess`) | 3 | GM04 | Not tested |
| Weapon sling supply cost (`arsenalCostWeaponSling`) | 3 | GM04 | Not tested |
