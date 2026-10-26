# BCM Medical validation

26 October 2026, Arma Reforger 1.8.0.13. These results cover the current independent foundation and GM settings adapter, not the completed medical ecosystem.

## Passed

Workbench script validation succeeded after native resource registration. The dedicated diagnostic server passes 126 normal assertions, including loading the actual native GM attribute resource and checking that every one of the 27 settings has a control.

Three separate dedicated-server processes passed 125 checks each with a shared profile:

| Phase | Result | Evidence |
| --- | --- | --- |
| Seed two settings generations | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-23/console.log |
| Restart and load latest into actual game mode | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-26/console.log |
| Damage newest file and recover prior generation | 125 checks, 0 failures | .checks/server-settings-restart-final/logs/logs_2026-10-26_11-28-29/console.log |

Run tokens differ across processes; stale success/failure logs cannot satisfy a new run. The expected invalid-settings diagnostic in the suite comes from the deliberately invalid-schema fixture. The tests preserve/lock that fixture rather than overwrite it.

Coverage includes patient/wound/chest isolation, AED shock advice, exclusive CPR sessions, depletion rescue/expiry, original life accounting, retained-projectile boundary, dressing reopening and frame-step independence, extraction/suture sequencing, facility-only critical surgery and fracture repair, splinted motion blood loss, pleural drainage versus source repair, consent replay/expiry, rope/manual transport policy, authority rejection without an editor, settings range/step/cross-field validation and JSON persistence/recovery.

Loaded runtime projects: native core, ArmaReforger, the user's BCM Medical GUID 6AB4B3F3FD215919, and the separate diagnostic addon. No ACE/NIRE/KAT/legacy BCM/other Workshop addon was loaded.

## Remaining acceptance

Visual GM menu/controller interaction and save popups have not been tested by a human client. Multiplayer owner/JIP/settings concurrency needs dedicated clients. The resource-load test establishes catalogue presence, not UI usability.

Native injury/consciousness and physiology adapters, treatment durations/inventory/RPC validation, backend life identity, suppression/focus integration, spectator routing, casualty transport physics/animations, body-bag transactions and the original BCM medical interface remain unimplemented or unconnected. No gameplay replacement, Workshop release or live-server deployment is claimed.

The final 126-assertion run also loaded the filled body-bag collision model, empty model and bag material directly from the consolidated addon. Evidence: .checks/20261026-113316-102/logs/logs_2026-10-26_11-33-16/console.log.
