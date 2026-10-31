# BCM Medical player controls

## Current source checkpoint — 30 October 2026

Player-focused background work is authorized, including AI removal. See [PLAYER-FOCUSED-IMPLEMENTATION.md](PLAYER-FOCUSED-IMPLEMENTATION.md). Autonomous medical AI has been removed from active source; player medical simulation and player treatment of AI casualties are preserved in code, pending native regression. New progress-only field procedures and owner HUD are staged with 271 settings/schema34. Consent policy now distinguishes carry/drag and requires consent only for a conscious player. Live casualty transport, medical-menu transport buttons and sling retention remain unfinished. No ACE code/assets are to be copied or adapted. No animation assets changed. Keep native tools, runtime tests and packing closed during recording; current source is uncompiled/unaccepted. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


Current development:271 settings/schema34. Controls below describe the game; server policies can change eligibility and timing.

## Open and navigate

Press **H** while looking at a person within examination reach to open their medical panel. Conscious and unconscious patients can be selected. If there is no eligible aimed patient, the panel opens on yourself. Press H again, use **Close menu**, or use the normal menu-back action to close it. The medical action is rebindable through native input settings; tap/hold behavior belongs to your personal binding.

Use **Self / Swap** to alternate between yourself and the selected other patient. The patient sidebar discovers occupants sharing your vehicle; it is not a nearby-person radar. Viewing a passenger does not grant permission to begin a new procedure inside a vehicle.

Select a highlighted body region, choose Circulation, Airway, Surgery, Medication or Fluids, then choose an available check or treatment. Use **More treatments** for the next action page. Findings require an examination; opening the menu does not reveal them automatically. Distinguish a completed examination snapshot from a live instrument reading.

**Cancel treatment** interrupts the current supported procedure. Moving away, becoming ineligible, losing the original supply or losing required facility support can also cancel it. A cancelled finite infusion retains its partial contents; already delivered fluid remains in the patient. Availability depends on current server policy, qualification, access, patient condition and actual supplies.

## Equipment and recovery

Stabilised component recovery is an optional Game Master policy under Circulation and is off by default. When enabled, enough circulating volume, pressure, breathing and oxygenation, controlled bleeding and any required stationary facility permit gradual red-cell/clotting recovery. It does not restore missing blood volume or repair injuries. See COMPONENT-RECOVERY.md for settings and accepted native tests.

A medic removing an already-connected oxygen mask needs no spare cylinder. Select Head, then Airway/Breathing and Disconnect oxygen supply. Remain within treatment reach for the saved removal time. The original source remains with its carrier. Movement cancels the removal; a new connection invalidates an older pending removal.

Independent drips: select the access limb, open Fluids, choose the appropriate Connect drip action (use More treatments for the second page). The original bag transfers into patient inventory; after connection the medic can leave with both hands free. Disconnect fluid line needs no spare bag and retains remaining contents in patient inventory. See INDEPENDENT-FLUID-LINES.md for storage, timing, restart and safety behavior. Current development settings are270/schema33, including the optional fluid-burden policy described in FLUID-BURDEN.md.

Use folded body bags for physical recovery of the deceased. Search, dragging, cargo loading, burial and Send Body Home remain recovery actions. They do not gate deployment or refund lives. **Limited Lives** is the sole BCM deployment allowance policy; the separate body-bag respawn flow and Body Bag Admin menu are removed. Filled-bag carrying and two-player stretcher carrying are unavailable. See [physical recovery](BODY-BAG-INTEGRATION.md) and [allowance administration](LIMITED-LIVES-ADMINISTRATION.md).


Qualified AI group members can now support an injured NPC group member's unsplinted leg fracture when already within normal treatment reach and holding their own original splint. The native AI must be active and available, with low threat; player patients and player-controlled medics are excluded. Revoking qualification or losing reach interrupts care without consuming an unused kit. This is bounded NPC fracture support, not full triage, travel/resupply or player consent. See [medical AI](NATIVE-MEDICAL-AI.md).
