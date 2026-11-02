# BCM Medical

BCM Medical is a standalone Arma Reforger medical and casualty-recovery addon. Its production project depends on the base game only. This repository contains the current source, runtime resources, editable assets, development tools, automated fixtures and player documentation.

## Current checkpoint 31 October 2026

The player-focused implementation has 29 inventory entries and 279 saved GM controls under fifteen BCM Medical headings, with settings schema 35. Autonomous medical AI has been removed; players can still treat NPC casualties.

Implemented systems include external and internal bleeding, dressing wear and closure, projectile extraction, leg fractures and facility repair, airway and side-specific chest treatment, oxygenation and circulation, CPR and AED workflows, pain and six medications, typed finite fluids, independent drips, finite oxygen and retained monitoring devices. Optional redistributed-fluid burden and stable-care blood-component recovery are disabled by default.

Living carry and crouch-drag preserve the actual casualty and injuries. Conscious players explicitly accept through the medical menu; unconscious patients can be picked up directly. Default unconscious retention requires the inventory-only sling and protects the exact supported held long gun, ammunition and attachments. The current carry pose still needs final human visual acceptance.

Named body bags support exact equipment recovery, searching, manual/rope dragging, fixed vehicle cargo, burial and sending home. Limited Lives is the deployment allowance system. Suppression, additional focus, applied-device presentation, configurable medic roles and supported native persistence are included.

Two-player stretcher carrying remains shelved and disabled. The GM stationary treatment stretcher is a separate implemented facility/support object. Filled body bags are dragged rather than carried. No ACE code, animation data or addon dependency is imported.

## Player documents

- [Medical training guide](Docs/Training/BCM%20Medical%20Player%20Training%20Guide.docx): treatment workflows, equipment, recovery, instructor drills and the complete settings reference.
- [Player test workbook](Docs/Training/BCM%20Medical%20Player%20Test%20Checklist.docx): simplified checklist starting at medical menu and assessment, with 101 checks, numbered steps, expected results and all 279 settings. The earlier introductory and installation sections have been removed at the user's request.
- [Test checklist source and worksheet](Docs/Training/BCM%20Medical%20Player%20Test%20Checklist.md): repository-readable version with exact setting keys.
- [Current source snapshot and validation limits](Docs/SOURCE-SNAPSHOT-20261031.md).
- [Private test candidate](Docs/PLAYER-TEST-CANDIDATE-20261030.md): package identifier and the previous automated acceptance evidence.

Older development notes preserve their original checkpoints. Their earlier counts, AI plans, incomplete-status statements and animation restrictions can be superseded by the current documents. The previous README is preserved in Docs/History.

## Development and validation

Open addon.gproj in Arma Reforger Workbench. The project identity remains MCBMedical with GUID 6AB4B3F3FD215919; its displayed title is BCM Medical. The base-game dependency is 58D0FB3206B6F859.

Tools/SettingsCatalog.json is the canonical settings catalogue. Generate settings with Tools/Generate-Settings.ps1. Audit owned GUIDs with Tools/Test-ResourceGuids.ps1. Tools/Validate.ps1 and the focused Tools/Validate-*.ps1 scripts run the native compile/runtime/network fixtures; read their parameters before running. Native validation needs the installed game/tools and available Steam platform services. The runtime diagnostic addon under Tests is development tooling, not a production dependency.

Tools/Build-PlayerTestChecklist.py regenerates the test DOCX and its settings worksheet from the reviewed Markdown and current catalogue, using Python with python-docx. A native DOCX render and page review are separate quality checks.

The source checkpoint retains the prior 51-stage acceptance, twelve packed smoke phases and packed dedicated-server/three-client transport result. Human Internet/account, input, platform, load, visual and boundary acceptance remain separate. The new human workbook is deliberately unpassed. The 31 October native recheck did not finish because Steam platform initialisation was unavailable; it is not recorded as a new validation pass.

## Source and package boundaries

Assets, Prefabs, Configs, Language, Scripts, UI, Sounds and addon.gproj are runtime/source resources. Source retains editable Blender projects and intentionally named revisions; Tests and Tools support development. Automatic Blender recovery copies, resource databases, local profiles, generated packages and logs stay outside Git. The full private package remains in local .checks output; cloning this source repository is not a Workshop release/install.

See [supplied asset provenance](Docs/SUPPLIED-MEDICAL-ASSETS.md), [pose provenance](Docs/CASUALTY-POSE-REFERENCE-20261030.md), [release packaging](Docs/RELEASE-PACKAGING.md) and [roadmap](Docs/ROADMAP.md). Final cylinder/tablet artwork, applied-device polish and outstanding donor licence/receipt review are not made complete by committing this snapshot.
