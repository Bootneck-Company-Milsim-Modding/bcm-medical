# Pain, medication and animation behavior

BCM owns this implementation. It does not import ACE, KAT or NIRE code or animation files.

## Player behavior

H opens the panel. Select a body region, then Pain > Assess pain or Medication > Check medication record. These checks use the normal examination timer, access checks, owner-only reply and panel-generation guard. Opening the panel or selecting anatomy reveals no findings. Pain is explicitly an overall patient assessment; an unresponsive patient cannot supply a verbal pain report. Recorded administrations describe active BCM game effects, not a laboratory test.

Morphine, naloxone and epinephrine each require their own actual inventory item. The placeholders use the native game's injection model and animation through base-game resource references. The server chooses the exact item and reserves actor, patient and kit. The default administration time is 10 seconds. Actual native animation completion and the minimum configured treatment duration are both required. Death, lost access, raised weapon, movement, cancellation, kit loss or lost eligibility prevents delivery. Duplicate requests cannot administer twice. Cancellation retains the original item and releases native item use.

Wounds, internal injuries and fractures contribute pain. Dressing, closing and splinting reduce the relevant contribution. Morphine gradually lowers pain and the configurable native aiming penalty. Native structural arm injury remains a floor, and analgesia does not repair fractures, unlock jumping, stop bleeding or return blood.

Morphine stacks breathing/pressure penalties as well as analgesia. Naloxone temporarily suppresses active opioid effects; a persisting opioid effect can return as the antagonist wears off. Epinephrine raises pulse/pressure only when there is spontaneous circulation; it does not create a pulse or automatically restore circulation. Drug effects clear with time and native FullHeal creates fresh patient state.

## Game Master / persistence

105 settings, schema 15, fourteen categories. Pain and Medication have their own headings. Every pain contribution, dressing/suture/splint multiplier, maximum pain aiming penalty, medication application time, repeat interval, active-dose limit, individual onset/duration and effect magnitude is a saved GM setting. Existing schema 1–14 settings are validated and migrated with prior values retained. Player key preferences remain in the native personal control profile.

The default model uses normalized game state and abstract game units. It provides no real doses, administration protocol or clinical prediction. Onset and duration are intentionally game-tuned. A more extensive medication formulary, medication allergies, route-specific IV/IO delivery and true instrument monitoring remain separate work.

## Animation boundary

Bandaging uses the real native self/other/revive workflow. Surgery, suturing and splinting now use the same base-game treatment graph as a visible development placeholder, with an original presentation effect that cannot heal or delete supplies itself. The server's procedure alone commits effects and kit consumption. Medication uses the native injection graph, also with that no-heal presentation adapter. These are functional native animations, not bespoke surgical motions.

Original CPR entry/loop/exit now play through the native replicated animation system and have owner/proxy/JIP acceptance. Finger/contact and ground alignment remain unfinished. Still needed: actual splint fitting; extraction/instrument work; needle-holder suturing; bag drag hand grip; paired casualty drag/carry lift, locomotion and safe release; visible rope and applied equipment. No custom clip has been represented as finished. EItemGesture in the installed native generated API contains pickup only, so it does not supply a ready-made CPR or casualty-carry gesture.

## Source rationale

Only the qualitative direction of medication effects is informed by primary labeling: [DailyMed morphine labeling](https://dailymed.nlm.nih.gov/dailymed/drugInfo.cfm?setid=1f53de80-efc8-4930-b3e3-fba0d026af05) describes respiratory and pressure depression; [FDA naloxone explanation](https://www.fda.gov/consumers/consumer-updates/access-naloxone-can-save-life-during-opioid-overdose) explains temporary reversal and possible recurrence. BCM numerical values are original gameplay choices.
