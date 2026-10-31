# Body bags in BCM Medical

## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are required but unfinished. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. Do not create, modify, retarget or rebuild animations. Existing custom assets/integration stay untouched pending separately authorized changes. Any indispensable animation requires a focused user decision first. Autonomous medical AI removal is planned for the separate AI mod, but must wait for the user's go-ahead; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


All production work belongs to **BCM Medical**, project GUID `6AB4B3F3FD215919`, in `MCB Medical`. Arma Reforger is its only runtime dependency. The legacy BCM-BodyBags add-on files have been removed after preservation and integrated validation. Only an empty directory held open by another process remains. Git publishing remains deferred.

## Respawn policy

On 29 October 2026 the user explicitly removed the separate body-bag respawn system and its menu. **Limited Lives is the sole BCM respawn policy.** Recoverable corpses, filled bags, burial, Send Body Home and vehicle loss do not block deployment or charge/refund lives. Native mission spawning and the existing Limited Lives allowance/authority checks remain.

The body-disposition wait/timeout, request/cooldown/review RPCs, Body Bag Admin panel/pause entry, custom deploy countdown/button override and waiting-spectator camera/HUD/actions have been removed. Their tests and pre-removal source are preserved under `.checks/removed-bodybag-respawn-20261029-061118-937` and in the integration history. They are not current features or remaining implementation requirements.

The current catalogue has **254 saved settings, schema 30**. It retains the original 250 Medical controls plus burial enable/duration/range and Send Body Home enable. Schemas 27–29 validate and discard the five retired respawn keys while retaining Limited Lives and all surviving medical/body-bag values. New saves contain no retired keys.

## Physical recovery contracts

- Filled bags support native manual/rope dragging, not carrying. Two-player stretcher carrying remains shelved.
- Bagging transfers original equipment entities, consumes the exact folded bag and deletes the corpse only after successful transfers. Rollback retains the corpse, unused kit and equipment.
- Loading deposits equipment into fixed native vehicle cargo. **Unloading leaves deposited equipment in the vehicle.** A movable backpack is not fixed vehicle cargo.
- Physical remains tokens track corpse-to-bag identity and burial/home/vehicle-loss outcomes. They have no deployment gate or respawn timer.
- Exposed corpses receive at least thirty minutes of native garbage protection; active recovery, filled bags and memorial graves retain their existing protection. Explicit scripted deletion remains possible.
- Burial requires a held native entrenching tool, stationary bag, natural ground and valid performer throughout digging. It creates the authored memorial before disposing of the bag and remaining contained equipment.
- Send Body Home requires an enabled native healing area and disposes of the bag and equipment still inside it. Gear deposited in a vehicle remains there.
- Native search sessions are controller/character-bound and exclude conflicting recovery operations. See BODY-BAG-SEARCH.md.

## Evidence and current validation

Removal build `.checks/20261029-061302-278` compiles successfully with 336 unique resource GUIDs. Native Limited Lives regression `.checks/20261029-061354-705` passes 573 assertions, zero failures: a dead account with allowance can deploy while its corpse exists; exhausted allowance still refuses preparation; unauthenticated grants/resets remain denied; a new life can be committed before old corpse deletion; disposal cannot change the current life or balance. Schema migration/model checks execute in the same suite.

Before this removal, all eight cargo fixtures and five actual piloted aircraft passed on the integrated Medical source; three faction medical tents passed 653 checks each; and the nine-gate clinical suite passed with unchanged source in `.checks/clinical-acceptance-20261029-012118-487`. These are historical evidence for the pre-removal build. Updated persistence, physical recovery and clinical regressions are being run for the current build. Do not treat retired respawn/menu test counts as current acceptance.

All 170 required legacy source/asset/configuration files are preserved and ZIP-content hash-verified in `.checks/bodybags-handoff-20261028-181152/current-source-preservation-20261029.json`. The Medical asset directory and three asset-generation tools are independent of the legacy project. Git history preservation and final legacy deletion remain retirement work.

## Remaining work

Finish current-build migration/restart, physical recovery and clinical regression; then continue the full BCM Medical completion ledger in ROADMAP.md. Backend-authenticated Limited Lives/reconnect, human input/audio/accessibility, remaining Medical physiology/equipment/art, supply/persistence/load and packaging/provenance remain separate requirements. Body-bag respawn gates, requests, admin menus and waiting spectator are intentionally excluded by the user's instruction.
29 October user instruction: removed the independent body-bag respawn restriction and menu. Limited Lives remains the sole BCM respawn policy. Removed wait/timeout coupling, request/review RPCs and cooldown, deploy UI override/countdown, Body Bag Admin/pause entry and keybinding, and optional waiting-spectator camera/HUD/input. Physical recovery remains separate. Current catalogue: 254 settings/schema 30; old schemas validate and discard the five retired keys. Native compilation passes with 336 resources. Limited Lives removal regression 20261029-061354-705 passes 573 checks. Current schema-30 separate-process restart persistence-current-20261029-061721-105 passes 555 seed/574 verify checks. A real copied schema-29 profile with formerly enabled spectator loads in legacy-settings-removal-20261029-061824-427 and passes 575 native Limited Lives/model/migration checks, including native menu-preset and keybinding absence. Retired tests/source are archived in .checks/removed-bodybag-respawn-20261029-061118-937. Pre-removal menu/spectator evidence is historical, not current functionality.

Current physical recovery regression `.checks/20261029-061855-080` passes 627 assertions, zero failures: original equipment is bagged and disposed at the native US medical tent while Limited Lives eligibility is already available before Send Body Home and remains unchanged afterward. The body-bag respawn system is absent. A verified Git bundle also preserves all nine legacy refs and complete history before eventual legacy-folder retirement; its path and hash are recorded in DEVELOPMENT-PAUSE.json. The post-removal unchanged-source clinical regression is running.

Post-removal integrated clinical regression `.checks/clinical-acceptance-20261029-061952-047/report.json` passes all nine gates with sourceUnchanged=true. Legacy add-on retirement then removed its source/assets/configuration/Git contents after verifying the 170-file preservation and complete Git bundle. Windows refused removal of the final directory because another process holds it open; the directory is empty and contains no addon.gproj, so it is no longer a loadable add-on. BCM Medical remains intact. This empty shell is not a runtime dependency or active development project.

