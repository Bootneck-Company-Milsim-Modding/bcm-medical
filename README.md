# BCM Medical

Standalone BCM Medical development project for Arma Reforger. Workbench project GUID `6AB4B3F3FD215919`; folder currently `MCB Medical`, displayed title `BCM Medical`. Only the base game is a runtime dependency.

## Implemented

- Original wound, airway, chest, cardiac-rhythm and CPR session domain state.
- Retained projectile sampling (40% default), bandage wear (5–10 minutes resting), faster reopening with movement, lower recurrent bleeding and separate extraction/suture rules.
- Internal bleeding sources and compartment-specific pleural collection/drainage rules.
- Left/right leg fractures, splints, movement penalties, movement-related blood loss, no jumping and facility-only definitive fracture repair.
- Consent bound to carrier, conscious patient and patient life; single-use, expiring acceptance.
- Manual and rope-assisted body-bag drag policy. There is no body-bag carrying mode. Rope drag permits weapons, running and crouching at the policy level; native physics/animation is not implemented yet.
- Original deployment ledger and BCM reserve-depletion policy.
- 27 native Game Master controls under BCM Medical, server validation, replicated values and server-profile persistence with verified writes and two-generation recovery. GM save success/failure feedback is owner-targeted.
- Owned BCM body-bag models, editable FBX, materials and texture sources consolidated under Assets/BodyBags.

## Current gameplay boundary

The GM settings and persistence adapter runs in the native game mode. Most medical and transport functionality above is tested domain logic, not yet connected to native injury events, item consumption, treatment RPCs, locomotion, carrying/dragging physics or a medical screen. Suppression, focus, life UI/camera and full body-bag gameplay migration remain incomplete. This is not a playable replacement for the old mods yet. No ACE, KAT, NIRE, RespawnLimiter, 420 or Improved Blood Effect implementation is bundled.

The 27 controls include reserved suppression/focus/lives values for the upcoming migrations; those values persist but do not activate those unported features. Every future adjustable gameplay value must be added to the same catalogue and a generated GM control before its feature is accepted.

## Server settings

Saved inside the server's persistent profile:

- `$profile:BCM_Medical/settings-a.json`
- `$profile:BCM_Medical/settings-b.json`

Use the same persistent `-profile` directory after restart. Changing or deleting the profile changes/removes the saved settings. Mount this directory on persistent storage when using a container. These files are server configuration, not mission-header defaults. Newest valid generation wins; a damaged newest file falls back to the previous generation. If neither existing file is valid, both are preserved and edits are locked instead of silently overwriting them.

Schema version 1 requires the current complete catalogue. Catalogue additions need an explicit migration and tests before release; they must not silently invalidate old server configuration. Per-account life balances are a separate future persistence concern; settings persistence alone does not implement a player database.

## Development and verification

Open addon.gproj in Workbench to register/import resources after cloning. Do not hand-edit resourceDatabase.rdb. The generated index is excluded from Git.

- `Tools/Generate-Settings.ps1` regenerates the native GM attributes and catalogue from Tools/SettingsCatalog.json.
- `Tools/Validate.ps1` runs Workbench script validation.
- `Tools/Validate.ps1 -ServerRuntime -ServerDirectory '<dedicated server installation>'` runs the separate engine test addon.

The test runner can also discover the existing sibling BCM NID AceCompat dedicated-server location pointer on this workstation. That is a development-tool convenience, not a mod dependency.

See VALIDATION.md, Docs/BCM-MEDICAL-STANDALONE-AUDIT.md and Docs/MEDICAL-INTERFACE-AND-TRAUMA.md. This repository is source development; no Workshop publication or live-server deployment has been performed.