# Advanced equipment workflows

Original BCM behavior, base-game runtime dependency only. Current catalogue: 254 saved GM settings, schema 30, fifteen inline headings in the single BCM Medical tab. Native treatment/injection graphs and package shapes are temporary presentation; no bespoke BVM, AED-pad or surgical clip is claimed.

## Player actions

Select a body region and complete the relevant check. Airway/head exposes adjunct and suction actions. Breathing/chest exposes left/right seals, decompression, drains and assisted ventilation. Circulation/chest exposes pads, AED analysis/discharge and instrumented readings after pulse assessment. Blood exposes vascular access on limbs and a separate fluid bag action. Supply counts and procedure eligibility are validated on the server.

| Action | Fresh default time | Equipment consumption |
| --- | ---: | --- |
| Chest seal | 15 s | One seal |
| Decompression | 15 s | One needle |
| Airway adjunct | 10 s | One adjunct |
| Suction | 10 s | Device retained |
| Assisted ventilation cycle | 3 s | BVM retained; finite 5 s support |
| AED pads | 20 s | One pad set, installed on patient |
| AED analysis | 5 s | Device retained |
| AED stand-clear/discharge | 5 s | Device retained; one discharge used |
| Chest drain | 30 s | One kit, drain installed |
| Vascular access | 15 s | One kit |
| Fluid bag | 60 s for a full default bag | Contents debit progressively; empty bag consumed, partial contents retained on interruption |
| Instrument check | 5 s | Monitor retained |

Every listed time is a saved GM setting. Ventilation support, drain rate, lung recovery, analysis age, device discharge count and bag volume are also configurable. Fluid delivery now uses finite contents on the physical bag and advances only during the validated native procedure. Interrupting leaves the same partial bag; restarting scales the native duration to its remaining fraction. Only volume actually accepted by the native reserve is debited. Portable oxygen has finite retained-source budgeting, and pulse oximetry has active handheld telemetry; broader ventilation/monitoring, route-specific medicines, overload and device complications remain. Progressive-fluid native and connected acceptance is tracked below.

## Distinct effects

A seal closes an air opening but does not remove tension or pleural blood. Decompression relieves tension; recovery depends on correcting the air leak and the configured recovery rate. Drains remove matching collected pleural blood without repairing its bleeding source or returning blood to circulation.

An adjunct addresses soft-tissue obstruction only on an unresponsive casualty. Suction addresses the separate foreign-material state. Assisted ventilation requires a patent airway and provides only a finite interval; it does not automatically create spontaneous circulation.

AED pads are required before analysis/discharge. Shock authorization belongs to the actual analysing device, expires, and is rechecked against the current rhythm. Ongoing compressions prevent discharge. A discharge uses a finite device charge and does not guarantee ROSC. PEA/asystole are not shockable. Instrument results and advice use owner-only, view-guarded replies; a bare-hands clinical RPC cannot request instrument result key 8.

## Native injury connection

Actual immediate native impacts now create persistent wound records; DOT events are excluded by native effect type rather than rejecting every effect object. Penetrating impacts can create side-correct chest complications, internal compartments and head-associated airway contamination. Chances, severity and flow are original configurable game rules, not clinical incidence estimates. New penetrations can defeat an existing seal. Optional automatic soft-tissue obstruction while unconscious is separately configurable and defaults off.

## Acceptance boundary

- Dedicated native equipment fixture: .checks/20261026-184353-520, 330 assertions, zero failures. Twelve real default-time procedures, including reusable-device retention, AED pads/analysis/one discharge without ROSC, separate chest effects, airway/suction/BVM, drain, access and measured fluid reserve.
- Native penetrating damage fixture: .checks/20261026-185427-386, 319 assertions, zero failures. Real HandleDamage event, side-correct chest and internal source, existing depletion/bleeding/reopening behavior.
- Focused three-client equipment fixture: .checks/splint-network-20261026-190352-484, 293 server assertions, zero failures. Actual owning client uses a chest seal and reusable monitor; instrument result remains private; all three clients include a late joiner.
- Full medication-then-equipment sequence: .checks/splint-network-20261026-192659-204, 300 server assertions, zero failures; seven phases observed on all three clients including the late joiner. Later preparation/grace changes require a fresh regression.

## Progressive finite fluids — 29 October 2026

The first full native saline/plasma/whole-blood run, .checks/20261029-071827-611, passes 598 checks with unchanged captured source. Each full default sixty-second procedure retains a partially depleted physical bag at midpoint while native circulating reserve has already increased. Saline/plasma preserve red-cell reserve; plasma/whole blood supply clotting reserve; whole blood adds red cells. The native renal redistribution model operates throughout the infusion rather than only after a completion bolus. These are existing normalized game reserves, not laboratory measurements or prescribing instructions.

The current interruption/access-loss safety run .checks/20261029-072411-642 passes 575 checks. It retains a genuinely partial physical bag and stops further delivery after access loss. Subsequent accounting/UI additions and the extended cancel-and-resume run are still being validated; connected partial-content replication, live settings, transfer/source-loss and UI acceptance remain required before the feature is marked complete.

Progressive native full/cancel/resume acceptance `.checks/20261029-072521-880` passes 606 assertions, zero failures, with unchanged captured source verified in source-after-audit.json. Saline/plasma/whole blood each deliver during all three full default sixty-second procedures. The fourth saline bag is interrupted after partial delivery, retains 436.515 mL in the exact item, stops in the background, resumes with a proportional remaining duration and is consumed once on exhaustion. Final reserve is 0.591168 after redistribution, with no red-cell creation or refill of the partial bag. Current access-loss/native-medication safety .checks/20261029-072411-642 passes 575 assertions. The current source adds a millilitre-returning native adapter tied to the same clinical settings used by delivery and labels the next bag's remaining contents in the Fluids actions. Connected partial contents and UI acceptance are in progress.

Connected progressive fluids `.checks/fluid-network-20261029-073220-460/result.json` passes 569 authority checks, sourceUnchanged=true. All three clients observe intermediate and final physiology for all three types; the actual owner receives decreasing finite contents (444.510/449.787/449.982 mL during the respective trials). Exact owner RPCs and all five existing final phases still pass. Native partial-bag UI `.checks/20261029-074413-637` passes eight checks and its 720p capture was inspected. A live capacity change creates new 1000-mL bags while preserving the original bag's 500-mL capacity and 375-mL remaining contents. The separate widget-walker fixture timeouts in 073642/073958 are rejected; the corrected bounded traversal produces clean native UI completion in 074151 and 074413.
