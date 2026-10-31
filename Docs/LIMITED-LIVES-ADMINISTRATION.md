# Limited Lives GM controls

Limited Lives remains the sole BCM respawn policy. Use the existing Game Master character context menu on a connected player's controlled character:

- **Add one deployment life** grants one future deployment.
- **Remove one deployment life** removes one future deployment without killing the current character.
- **Reset deployment allowance** restores the configured total, counting the current life if already alive. It cancels an uncommitted preparation; an old finalization cannot spend that cancelled reservation.

These actions require the executing native editor's unlimited Edit authority. The server derives the target from the selected controlled character and rechecks its live controller/account mapping. AI, disconnected/stale selections, invalid deltas and missing authority are rejected. Revoking the native Edit mode invalidates held editor authority. No account identifier or acting player ID is accepted from an action parameter.

Resetting an active account preserves its exact character, current life identity and clinical state. It does not heal, spawn a new character or dispose of remains. Body-bag recovery, cargo, burial and Send Body Home never spend or refund the allowance. The removed body-bag respawn/admin menu is not restored; these are additions to the ordinary native GM context list, alongside the medic role actions. No new settings were added:254 controls/schema30 remain.

Native saved-world persistence retains granted allowance and ledger state when using the persistence-enabled mission and same server profile. Local native acceptance uses diagnostic account IDs; backend-authenticated reconnect remains unaccepted. These controls target connected controlled characters; an offline-account administration interface is not implemented.

## Evidence and limits

Native reset core171139-848 passes582/0, covering pending field cleanup, stale finalization rejection, fresh reserve/commit, active body/partial blood preservation and independent remains. This uses a direct authorized-core diagnostic seam and does not alone prove GM UI authority.

Actual owning-client native context/RPC172042-308 passes566/0 with private target status observations for grant/remove/reset. Extended172512-561 passes578 connected/557 separate-process restart, SourceUnchanged=true. It also grants again, revokes the actual native Edit mode, rejects held authority after revocation, commits a world save and restores the exact granted ledger, paid-body UUID and partial blood. Native controls remain present in the merged context configuration. Human mouse/controller/accessibility review remains separate.

The context action container does not supply trustworthy executing-editor identity. The native actions component provides that context on the server; direct unscoped action-container execution has no allowance side effect. Source registration attributes are retained on the diagnostic extension so config reflection can load all three actions. The first failed171929 run is preserved; it found that missing test-container attributes prevented config loading.
