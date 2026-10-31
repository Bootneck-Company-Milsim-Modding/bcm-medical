# Native persistence implementation boundary

29 October 2026. This is the next implementation design, not acceptance evidence.

BCM Medical should extend the installed game's persistence system for worlds that enable native saves. Avoid a second profile-global account database that accidentally carries allowances between unrelated campaigns. The existing settings profile remains a separate server configuration store.

## Limited Lives

Persist audited backend identities and deployment-ledger counters with native game-mode save state. Persist the sequence, committed deployment count, allowance adjustment, current life and active/dead status. Never save a pending spawn reservation as a committed life. The existing limited-lives policy remains the only BCM deployment restriction; corpse disposition and body-bag menus must stay removed.

An active body's persistent UUID must replace its transient RplId in saved data. Resolve it through the native PersistenceSystem after load. Pending resolution must not manufacture another paid life. A missing/deleted body retires the active pointer without refunding the committed deployment. Old corpse callbacks still cannot alter a newer current life. Saved account data must be fully validated before replacing the live map; unknown versions and corrupt records must be reported and must not silently reset exhausted balances.

Native audit establishes identity before native possession may run. Restoration and reconnect tests must cover that ordering, surviving bodies, dead bodies, exhausted balances, failed spawns and a different account receiving a reused connection ID. Do not accept diagnostic injected identities as authenticated-backend acceptance.

## Finite equipment

Add a BCM component serializer to the native item persistence configuration while preserving ordinary item/storage serializers. Save the item's original fluid, oxygen and AED capacities alongside remaining contents/charges. These values must survive a changed GM capacity default. Validate finite bounds and type before mutating the component; publish restored values to connected peers. Empty and partial supplies must not refill or receive fresh-device refunds after restart.

Native serializer application occurs after entity initialization by default, so restore must deliberately replace initialization defaults once and preserve the newly generated runtime device identity. Patient-to-device references need persistent UUID resolution instead of saving the old per-process device number. Historical saves with no BCM state need an explicit migration boundary; they cannot recover depletion which was never recorded.

## Acceptance

Use actual native PersistenceSystem configuration/tracking and JSON/binary serializer paths. Require separate-process restart with the same saved world, original finite item identities/state, current GM defaults, owner/proxy/late-join state and refund values. A manual JSON round trip alone does not prove native world persistence. Patient physiology, wounds, medication timers, stationary transport, body bags and vehicle cargo need their own validated state adapters and must remain on the roadmap until implemented and checked.
The native diagnostic executable supports -worldSystemsConfig for directly launched worlds, documented in [Bohemia's startup parameters](https://community.bistudio.com/wiki/Arma_Reforger%3AStartup_Parameters#worldSystemsConfig). The finite persistence fixture uses the installed MissionSystems.conf; the ordinary plain-world fixture otherwise has no PersistenceSystem.

## Accepted finite inventory adapter

BCMM_EquipmentItemSerializer is registered in the existing native item persistence configuration, retaining its normal inventory-storage serializer. A versioned four-value vector stores schema, kind, original capacity and remaining contents. Restoration is server-only, validates the full vector before mutation and rejects restoration into an active procedure's source. Native load failure uses ERROR handling so corrupt state cannot silently yield a fresh device.

Real native saved-world restart .checks/finite-persistence-20261029-084753-382 passes595 seed and559 verify assertions, both zero failures, SourceUnchanged=true. It preserves the original persistent item UUIDs and partial saline/oxygen/AED state. The diagnostic Save/RequestLoad and Save/RequestSpawn callbacks are accepted; earlier direct client DeserializeSpawn/DeserializeLoad crashes are rejected diagnostic paths. Whole patient and diagnostic-account Limited Lives persistence subsequently passed the focused checks below; live authenticated reconnect remains open.


## Current character adapter validation

29 October: real native binary saved-world restart `.checks/finite-persistence-20261029-141138-357` passes 604 seed /586 verify assertions, zero failures and unchanged sources. Character clinical data uses versioned codecs, exact native hit-zone names, remaining timer durations and persistent source UUIDs. Native health data is isolated in a `base` object; DEFAULT healthy health omits that object and must retain prefab health rather than invoke an empty native load. The original healthy-body deletion regression was reproduced and corrected before this pass. A separate dead character and an alive depleted character also restore.

Persisted drugs retain dose ages and repeat cooldowns; CPR sessions and process-local AED analysis are intentionally cleared. Actual body identity, native partial blood, dressing wear, internal bleed, splinted movement, chest seal, access/pads and depletion grace pass. Connected oxygen and invalid patient payloads subsequently passed611/588 at141545-028; peer/JIP acceptance remains open. Limited Lives local saved-world accounting is accepted with diagnostic identities; live backend audit/reconnect is not yet accepted.

## Physical-world adapters now under validation

Native Items group registration adds a specific Medical world prefab rule and preserves existing item/radio entries. The world adapter retains immutable physical remains records, including disposed records and relative death age; native UUID availability binds physical body/bag, original corpse and stored vehicle with expected-record guards. This has no Limited Lives calls. Named bag metadata and stable revision survive, while searching, dragging and half-completed cargo states do not resume. Native storage deserialization has a synchronous recovery insertion scope and immediately restores the existing insertion restriction.

Physical bag restart615/593 and grave/stationary restart617/596 are accepted locally at143820 and144020 respectively. Stationary reattachment validates the original UUID, unconscious living controller, physical proximity and absence of conflicting placement/vehicle; it does not teleport or heal. The fixture explicitly seeds stationary association and memorial metadata. Real native vehicle cargo restart and destruction lifecycle subsequently passed the checks below; owner/proxy/JIP and missing-reference/corrupt-registry acceptance remain open.

## Native vehicle inventory scope fix and automatic registration

The vehicle inventory save payload contained the original bag and partial AED, yet native deserialization returned false before insertion. The failure reproduced with a native dressing, on both jeep and truck, with both binary and JSON databases, and on active-world RequestLoad. Slot-identity flags, an extra payload object and early deserialization did not fix it. Matching the native nested slot/entity type-discriminator scope on read did: BCMM_NativeVehicleStorageSerializer temporarily calls EnableTypeDiscriminator(false), delegates to the existing native reader, then restores the previous setting. The native vehicle configuration retains every other serializer and its original failure policy. Production uses the ordinary binary backend; diagnostic backend/configuration overrides were archived outside the test addon.

Production binary restart `.checks/finite-persistence-20261029-151708-485` passes624/601. Native Persistence registration is now authored on filled bags, graves and stationary field stretchers, matching the [official registration mechanism](https://community.bistudio.com/wiki/Arma_Reforger%3APersistence_System). `.checks/finite-persistence-20261029-152208-407` passes623/601 after removing their diagnostic tracking fallbacks. Jeep152340, helicopter152445, ambulance152506 and truck152526 all pass623/601 with unchanged sources.

Post-restart jeep unload/reload/native destruction/deletion `.checks/finite-persistence-20261029-152830-883` passes623/609. Equipment remains in the vehicle after unloading; reloading publishes one exact bag; destruction and deletion retain VEHICLE_LOST and preserve unrelated remains and the surviving account balance. The third-process post-wreck save boundary subsequently passed623/612/563 with unchanged sources at153104-289; original disposed cargo UUIDs do not resurrect. Live backend authentication, owner/proxy/JIP restoration and the remaining full Medical roadmap are still open.

## Current review and queued fault cases

Owner/proxy/late-join saved-world checks are staged outside the runtime addon while regression155841-589 runs. They must observe the original restored patient, partial connected oxygen, named world remains and native streamed cargo. Diagnostic local ownership is separate from backend authentication.

Code review identified an unaccepted missing-reference case: if an awaiting-disposition physical UUID never becomes available, its record currently retains a null runtime pointer, and the next capture rejects an awaiting record without a physical UUID. Reproduce this with a real missing native record before selecting recovery behavior. Missing physical recovery must never refund or gate a paid life, recreate deleted equipment, or silently corrupt the next native save. Also cover duplicate record IDs, malformed/future versions and stale reference callbacks without mutating a previously valid registry. These cases remain open; no production change was made during the running regression.

## Connected saved-world and unresolved-reference corrections — 29 October

Saved-world owner/proxy/late-join acceptance now passes at `.checks/world-persistence-network-20261029-163505-666/result.json`:623seed/558server, zero failures and unchanged sources. Original patient/equipment, finite connected oxygen and native streamed vehicle cargo are observed. A restored depleted patient and independent terminal patient reach native DEAD. The first162648 run reproduced a real recursive Kill -> OnDamage -> UpdateConsciousness -> Kill stack overflow at depletion expiry. All native death paths now preserve the consciousness re-entry guard through synchronous native callbacks; already-dead controllers return early.

The direct missing-reference baseline failed566/1 at native registry capture in163200 logs16-33-14. A validated unavailable physical UUID now remains attached to its runtime record until matching native resolution, so capture can retain it without spawning an entity. Existing expected-record guards prevent stale callbacks replacing later restored state. Corrected direct regression passes566/0 at16-34-29.

`.checks/missing-physical-restart-20261029-163758-859/result.json` passes623/572/558, zero failures, unchanged sources. It waits beyond the native availability timeout, commits an actual new world save with the unresolved reference, then loads that save in another process. The UUID/disposition survive; missing gear remains absent; the independent account body UUID and every paid-life ledger value remain unchanged. This covers the previously open save-poisoning edge case. Further contended/malformed associations, authenticated saved-body reconnect and full release work remain open.

Current Jeep unload/reload/destruction/post-wreck restart also passes623/612/563 unchanged at163606-770 after both production fixes. The earlier18-gate full clinical pass155841-589 predates these fixes; the expanded20-gate163909 regression stopped at the existing oxygen phase8 range cancellation and is not a combined pass.
