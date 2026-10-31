# Stationary treatment stretcher in Game Master

## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is planned for the separate AI mod, but must wait for the user's go-ahead; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


Search the normal Game Master object browser for **BCM stationary treatment stretcher** and place it on valid terrain. It appears as a medical object with a thumbnail of the supplied stretcher model. The existing native placement, editor permissions and world rules apply.

Use the stretcher's **Place casualty on stretcher** and **Remove casualty from stretcher** actions during play. The casualty must meet the existing unconscious-patient, distance and interaction checks. Placement preserves the character's inventory, injuries and native life state; it does not heal or replace them. The stretcher acts as a medical facility under the configured BCM treatment/facility policy.

Two-player stretcher carrying remains shelved. This entry adds a stationary world object; it does not add folded inventory deployment. Body-bag respawn controls and the old Body Bag Admin menu remain removed. Limited Lives remains the sole BCM deployment allowance system.

## Persistence and validation

The editable variant has a stable resource ID6AB4B3F3FD21B100 and explicitly participates in Medical's native world persistence. Its stationary patient association uses the existing validated stretcher serializer. Native editable-component metadata uses the engine serializer.

`.checks/gm-catalogue-network-20261029-183154-225/result.json` passes with unchanged sources:589 connected assertions and565 separate-process restart assertions, zero failures. The actual owning client opened the native editor, resolved the catalogue index, loaded the thumbnail through ImageWidget and requested placement through the public native placement/RPC path. The server created one station; owner, proxy and late joiner resolved its actual replicated entity/components. A separate native process restored the same placed station UUID. Existing GM allowance actions, real Edit-mode revocation, the target's paid body and partial blood remained covered.

`.checks/world-persistence-network-20261029-183426-508/result.json` also passes unchanged after the saved clinical fixture switched to the editable variant. This covers a real saved casualty/stretcher association and connected owner/proxy/late-join restoration alongside existing finite equipment and world remains.

The catalogue-specific checks preserve all41 native placement registry entries and add exactly one Medical registry. The authoring helper is excluded development tooling; it is not installed in production. The owned-resource audit now rejects stale GUID references as well as duplicate GUID definitions.

Human controller/accessibility review, final package/clean-install validation, additional content-pack combinations and live backend account/reconnect acceptance remain separate release work. Local diagnostic identities are not evidence of backend authentication.
