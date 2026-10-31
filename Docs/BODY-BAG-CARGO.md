# Body bags and vehicle cargo

The only runtime addon dependency is Arma Reforger. Filled bags use native inventory cargo, never passenger seats. All world bag actions belong to the bag's native look-at action context: deceased name, search, load nearby cargo, manual drag, attach rope and release.

Loading searches for native vehicle inventory storage that accepts the exact bag. This applies to helicopters, ambulances, trucks and jeeps without a blanket Vehicle_Base override. Default interaction range is 5 metres, editable and saved through Game Master. Native capacity rules still apply. Loading requires a stationary-enough vehicle; it is not an airborne winching mechanic.

## Equipment flow

Bagging reserves the exact dead casualty, checks the acting player's real folded-bag item, waits the configured duration and revalidates. Original item entities move into a recovery container; weapon attachments remain with their original weapon. The empty bag is consumed and the corpse removed only after transfers succeed. Failure attempts to restore original slots; incomplete rollback preserves real items and a recovery bag rather than deleting equipment.

The authority records the casualty's equipment before unconsciousness/death. Native death may drop the held rifle. Only exact recorded, unparented items still within the configured recovery radius are eligible. A final ownership check immediately before movement prevents taking a rifle that another player picked up after the snapshot. Unrelated nearby equipment is never swept up.

Loading moves the bag's remaining equipment into the vehicle's own cargo, then inserts the bag. On failure, the coordinator restores equipment to the bag where possible. **Unloading leaves all transferred equipment in the vehicle**, as explicitly requested. No automatic return-to-bag operation runs. Native inventory UI insertions/removals also reconcile bag state and equipment handoff.

## Transport and replication

Server-reserved WORLD/LOADING/STORED/UNLOADING states, revisions, vehicle identity and deceased name replicate. Native inventory owns the actual hierarchy and visibility. Optional BCM vehicle variants supply a named manifest; ordinary native vehicles can use their own cargo inventory without that component. Stored entities can legitimately stream out of relevance.

Manual dragging and rope towing use original server physics, collision traces, speed limits and release rules. The inventory rope kit is required continuously for towing. The owner RPC binds to the player's current controlled character; stale release requests cannot release a newer session. Rope towing permits running, crouching and weapons. Manual dragging releases on raised weapon or excessive speed. Body bags have no carry action. Visible stretched rope and grip animations remain unfinished.

## Accepted evidence and limits

- .checks/20261026-150139-689: 331 assertions, zero failures; armed corpse -> named searchable bag -> truck -> unloaded bag, exact original equipment left in cargo.
- .checks/20261026-145959-046: 232 assertions, zero failures; failure after all transfers restores original equipment hierarchy and preserves corpse/empty kit.
- .checks/20261026-150225-311: 234 assertions, zero failures; pickup after snapshot respects the current weapon holder.
- .checks/cargo-network-20261026-132605-990: 234 server assertions, zero failures, three clients including late join; eight ambulance/helicopter/truck/jeep phases, equipment handoff and direct native UI reconciliation.
- .checks/drag-network-20261026-141647-420: 193 server assertions, zero failures; all three clients observe physical follow, release and actual owner-RPC manual drag/release.

Current Medical acceptance includes the 603-assertion cargo regression on three clients, all eight native/modded cargo fixtures, and five piloted aircraft with actual character ownership and owner/late-join inventory inspection. External native A-to-B moves reconcile the new vehicle; previously deposited equipment stays in A. See [current integration evidence](BODY-BAG-INTEGRATION.md) for exact runs and limits. Human menu/controller play tests, broader mass inventory contention, arbitrary vehicle packs and complete visual/animation acceptance remain outstanding. Burial/disposition and focused actual vehicle destruction/deletion subsequently gained native acceptance; original vehicle/bag/equipment persistence and post-restart unload/reload/wreck disposal passed in the29 October focused runs. These remain source-specific checks, not universal vehicle compatibility. The compact legacy 2.5 L bag footprint and 85 kg mass follow native inventory mechanics, not a claim of real luggage-space fit.

The Medical-only lifetime broker protects exposed corpses for at least 1,800 seconds and filled bags for their lifetime. Active bagging and attached remains retain protection. Historical full-duration native garbage acceptance in remains-lifetime-20261028-201531-888 passed15 checks with zero failures: the original corpse survived1798.43 seconds, became eligible after1802.93 seconds and was physically collected; the original filled bag remained protected. This evidence belongs to that historical source snapshot. This protection does not prevent explicit scripted deletion of a vehicle and its children.

The current whole-addon catalog has270 configurable values under GM > BCM Medical. Schema33 validates and migrates supported named-key schemas1–32. Reuse the server's persistent profile directory across restarts.


Current29 October23:05 compatibility: Mi-8, Medical Little Bird, UH-60, HH-60G and Chinook each pass13 checks in bodybag-compatibility-20261029-225220-683; HH-60G is explicitly passed with the narrow upstream diagnostic policy. MRZR/JLTV/Cougar each pass11 in230304-641. Actual carrier/pilot ownership and owner/late-join inventory inspection pass. Current private curated packet also passes701 native four-family cargo renderer-runtime checks with Medical loaded only from packed output. This does not establish arbitrary-pack, human controller, latency/load or final art acceptance.
