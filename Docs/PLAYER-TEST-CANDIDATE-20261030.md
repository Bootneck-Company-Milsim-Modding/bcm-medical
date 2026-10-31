# BCM Medical private player-test candidate — 30 October 2026

The agreed player-focused implementation has a validated private Windows PC package. The current carry is a revised visual candidate, not marked approved by the user. Authenticated Internet and human multiplayer acceptance are still pending. Nothing has been published or pushed to Git.

## Delivered scope

- Autonomous medical AI removed; player clinical care and player treatment of NPC casualties preserved.
- Approved original black two-hook sling imported as a non-consumable inventory item. Default retention requires a sling and preserves the exact held supported long gun, ammunition and attachments through BCM unconsciousness. GM policies also permit native dropping or retention without a sling.
- Living carry and crouch-drag use original patients and preserve injuries/inventory. Conscious players explicitly accept; unconscious patients have no consent prompt. Offers support decline, timeout, cancellation and replay rejection.
- Small side-by-side controls at the bottom right of the medical menu preserve the larger body selector. The pair changes between request, accept/decline, cancel and release states.
- Two original casualty loops use the attributed Bohemia sample skeleton. The latest carry rebuild uses a connected hip/knee/ankle chain and moves the right arm clear of the carrier. Head and left arm remain unchanged. The user-accepted drag runtime resources are unchanged. Rescuer locomotion and all 116 earlier animation files remain unchanged. No ACE assets, code or dependency are imported.
- Timed progress defaults for field procedures and owner progress HUD. Two-player stretcher carry remains shelved; filled bags use manual/rope dragging.
- 279 GM settings, schema35, fifteen section headings; strict migration from complete older settings profiles. Only the base-game dependency remains in the production project.

## Acceptance evidence

| Scope | Evidence | Result |
|---|---|---|
| Full current production validation | `.checks/clinical-acceptance-20261030-174026-538/report.json` | 51/51 stages passed; final source snapshot unchanged |
| Production continuity | Same report, `continuity` | 23 prior passes retained with original logs; only the two oxygen diagnostics changed from 28 to 29 catalogue items; all 1,242 production files verified unchanged |
| Native front/rear pose review and four transport cases | `.checks/20261030-171454-125` | 748 assertions, zero failures |
| Native private packing | `.checks/package-development/candidate-20261030-171720-777/pack-result.json` | Native success marker, exit0, one package |
| Package inventory | Candidate `package-inventory.json` | 472 entries, zero flagged private/authoring/unsafe entries |
| Packed-only smoke acceptance | Candidate `smoke-20261030-181912-536/result.json` | All 12 phases passed; package hashes unchanged; Medical origin exclusively packed output |
| Packed dedicated server + 3 clients | Candidate `packed-transport-network-20261030-182501-467/packed-result.json` | 701 assertions, zero failures; packed origins verified; package hashes unchanged |

The original interrupted/failed full reports remain recorded. They are not relabelled as passes. The oxygen-network failure was a diagnostic early return on the stale 28-item catalogue expectation; a focused 698/0 rerun and the resumed full gate both passed after correcting it to 29.

Packed phases: authority680, sling718, living transport748, clinical safety714, UI28, stock commit695, saline access694, source movement697, full heal699, cargo747, retrieved-source seed704 and retrieved-source restart686. The separate packed multiplayer test exercises actual-owner requests, consent, movement/turning, late join, release, waking, cancelled-token replay and exact rifle/sling retention.

## Package location

`.checks/package-development/candidate-20261030-171720-777/packed`

Keep its three files together: `addon.gproj`, `data.pak`, `resourceDatabase.rdb`. The package is 72,601,203 bytes. SHA256 of `data.pak`:

`9BFFA029E04A23D5221E2B6D67258F521194B27C36B20F5E6A0434C5ADDFC499`

Project GUID: `6AB4B3F3FD215919`. Dependency: base game `58D0FB3206B6F859`. The diagnostic project, source checkout, `.checks`, Tools and authoring files are not part of the runtime package.

## Human testing and remaining limits

1. Review the latest carry from front/rear and during movement with normal loadouts; verify the right arm, knee direction, shoulder contact and clothing intersections. The user has not approved this revision yet.
2. On a private server, exercise conscious broken-leg consent, unconscious pickup, rejection/timeout/cancel, patient/carrier release, waking and nearby players. Try standing, sprinting, jumping, slopes, obstacles and narrow spaces while dragging.
3. Exercise death, participant disconnect/reconnect and saving/restarting during active living transport. These new-holder boundary scenarios are not separately proven by the 18-phase carry network fixture. Existing world/persistence gates cover their stated medical/fluid/remains scopes.
4. Check sling/no-sling policies with real inventory/loadouts, and the compact menu/progress HUD with human keyboard/controller input and small screens. Automated native input and screenshots are narrower evidence.
5. Authenticated backend identities, Internet latency/load, clean installs and Linux/console acceptance remain separate. Current dedicated evidence is local diagnostic networking with three clients.
6. Missing oxygen-cylinder/tablet-pack artwork, final applied-device polish and source-asset licence/receipt review remain on the broader roadmap. This local private package is not a published or licence-cleared public release.

See [resumed work](RESUMED-DEVELOPMENT-20261030.md), [pose provenance](CASUALTY-POSE-REFERENCE-20261030.md) and [asset credits](SUPPLIED-MEDICAL-ASSETS.md).
