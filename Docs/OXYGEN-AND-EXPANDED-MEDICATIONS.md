# Oxygen and expanded medications - 28 October 2026

This describes original BCM game mechanics. Quantities, cutoffs and compressed timing are game settings, not real clinical doses. No ACE/KAT implementation is copied.

## Portable oxygen

Select Airway, then Head, to connect or disconnect. The retained kit uses the supplied mask asset; a separate cylinder and final face alignment remain presentation work. Default connection is 10 seconds and removal 3 seconds, subject to the medic time modifier. The exact kit, reach and configured role must remain valid throughout preparation.

New devices contain 300 litres by default and supply 10 L/min. The operator must retain the exact source within 3 metres. Capacity belongs to that device: reconnection, transfer or changing patient does not refill it. Empty sources, source loss, separation, death, a patient reset or live GM disable stop supply. The fullest usable source is selected when several are present; an empty kit cannot hide a usable spare. Capacity changes affect newly spawned kits, while flow changes affect active supply. A breathing examination reports remaining litres and flow.

Oxygen improves the saturation target through remaining lung function. It does not clear obstruction, provide breaths in apnoea, relieve tension, reinflate a collapsed lung, restore circulation or replace red cells. Connected oxygen can support an actual BVM ventilation action. Gas can be wasted while an obstructed airway is connected. The mask is a mesh-only cosmetic driven by replicated clinical state; it creates no inventory or physical item duplicate.

[RCUK post-resuscitation guidance](https://www.resus.org.uk/professional-library/2025-resuscitation-guidelines/post-resuscitation-care-guidelines) separates oxygenation, ventilation and circulatory support. BCM lung compensation and timing are deliberate game abstractions. Hyperoxia, oxygen toxicity and capnography are not simulated.

## Paracetamol tablets

Select Medication, then Head. The patient must actually be awake in the native game, have spontaneous circulation, an open airway and adequate breathing. Revalidation can interrupt administration and retain the exact supply. Native wake-up transitions must finish before tablets become eligible.

Defaults: application 10 seconds, onset 60 game seconds, clearance 1,200 seconds after the peak, peak relief 20 game units, repeat interval 300 seconds and active cap 2. Relief develops slowly without opioid breathing/pressure modifiers. It does not repair injury or replace missing reserve. The supplied pouch and non-injectable handling category are placeholders; final blister packaging and hand-to-mouth animation remain art work.

[NHS paracetamol information](https://www.nhs.uk/medicines/paracetamol-for-adults/) supports its analgesic role and warns about excessive exposure. BCM uses abstract supplies and repeat/cap limits; liver toxicity, real-day cumulative dosing and actual milligrams are not implemented. This omission does not imply overdose safety.

## Fentanyl

Select Medication, then a limb. This distinct finite supply uses the existing unlabelled injector placeholder. By default it requires a medic and vascular access, with separate pulse, systolic pressure, breathing-rate and saturation safeguards. General medication permissions also apply and are rechecked until completion.

Defaults: administration 10 seconds; analgesic onset 10 seconds and clearance 180 seconds after the peak; peak relief 70 game units. Breathing/pressure effects have separate onset 20 seconds and clearance 300 seconds. Residual exposure remains in history and counts toward the active-dose cap after analgesia fades. Default repeat interval is 120 seconds and active cap 2. These are compressed game values, not a prescribing regimen.

Morphine and fentanyl risks accumulate before shared naloxone reversal. Naloxone recognises fentanyl-only exposure and can remove both opioid analgesia and depression; effects can return as reversal fades. Paracetamol relief is independent of naloxone.

[Manufacturer information via DailyMed](https://dailymed.nlm.nih.gov/dailymed/drugInfo.cfm?setid=38d0c14a-a0c1-44cc-a939-0304eb8037d6) describes dose-related respiratory risk and respiratory effects that can outlast analgesia. Full clinical pharmacokinetics, tolerance and dosing are not simulated.

## Settings and evidence

Schema 24 appends 33 settings while preserving existing named keys: 210 total under the same single GM tab and inline headings. Six medication slots plus fifteen equipment kinds give 25 private supply counts. All new timers, repeat/cap limits, effect magnitudes, vital/route/role safeguards, source capacity/flow/range and mask visibility have GM controls.

Model scenarios passed 491 checks. Final full-time native oxygen/medication acceptance `.checks/20261028-070731-380` passed **517 checks**, including usable-spare selection, exact kit retention/depletion, actual unconscious-wearer continuity, patient reset, role/swallowing guards, fentanyl/naloxone and oral completion.

The final [acceptance report](../.checks/clinical-acceptance-20261028-070956-167/report.json) has status **passed** and **sourceUnchanged: true**. It covers resource validation, clinical safety (512 checks), oxygen/medication actual-menu owner/proxy/active-session JIP (505), typed-fluid multiplayer (507), applied-device multiplayer (497), native bleeding (523), separate-process settings seed/reload (493/509) and 1280x720 menu smoke (26). No gate reported an assertion failure. Native and final acceptance source fingerprints were also compared and unchanged. These are local development fixtures, not Internet load or final art approval. The root VALIDATION.md retains earlier failures and corrected-build evidence.

Two-player stretcher carrying remains shelved. Supplied cap textures remain unchanged. Complete world/account persistence, broader medications, injury calibration and final art remain on the full-mod ledger.


## Presentation and supply update - 28 October

The mask now follows Head/eye/lip landmarks, with GM right/up/forward offsets, yaw/pitch/roll and scale. Native screenshots were inspected after correcting the face-frame direction. Tablet self-use and conscious standing handover now use original compiled entry/loop/exit graphs; the right palm faces inward at the mouth. The self-treatment adapter avoids turning the actor toward its own target bone. Final cylinder, pill/packet props and contact/material polish remain pending.

The single GM tab now has 250 saved schema-25 values and fifteen inline headings. Arsenal includes US/USSR/FIA availability, 28 individual prices and a refund fraction. Native catalogues contain 28 medical inventory entries apiece, preserving ordinary weapons/clothing. Native box cache and server request-policy checks have separate acceptance. Catalogue reflection does not alone prove complete native purchase/refund transactions.

The thirteen-phase owner/proxy/late-join sequence `.checks/oxygen-medication-network-20261028-094909-777` passed 554 server checks with zero failures, including self tablet motion, cancellation without dose/kit loss and all three native faction catalogues. Subsequent frozen-source regression is recorded independently.

29 October 2026, oxygen removal: a connected supply can now be disconnected without the removing medic carrying a spare cylinder. The native medical menu enables the existing removal action from the replicated connection state. Removal retains its saved duration, authority validation, reach and actor/patient locks; cancellation leaves the supply connected. The operation captures the exact oxygen session so reconnecting the same cylinder invalidates an old pending removal. No replacement device is created. Native actual menu/RPC, timer, movement cancellation, same-device reconnection and independent concurrent removals passed608 checks with zero failures in .checks/20261029-194450-572. This focused local runtime does not by itself certify dedicated owner/proxy/JIP removal.
