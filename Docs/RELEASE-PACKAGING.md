# Release packaging preparation

Current engineering checkpoint: the agreed player-focused changes passed all 51 validation stages with documented diagnostic-only continuity, 12 packed-only phases and a packed dedicated-server/three-client transport test (701/0). The private package is built. See [PLAYER-TEST-CANDIDATE-20261030.md](PLAYER-TEST-CANDIDATE-20261030.md) for exact evidence, package location and remaining human/Internet/art acceptance. The latest carry revision is not marked visually approved by the user. Earlier checkpoint paragraphs below are historical where superseded.

## Current source checkpoint — 30 October 2026

Recording has finished and native development has resumed. See [RESUMED-DEVELOPMENT-20261030.md](RESUMED-DEVELOPMENT-20261030.md) for current evidence. Autonomous medical AI is removed; the approved inventory sling is imported. The catalogue is 279 settings/schema35. Compact medical-menu carry/drag controls, conscious-player consent and sling retention have native functional tests, including dedicated owner/proxy/late-join coverage. Casualty poses remain visually unfinished; see [CASUALTY-POSE-REFERENCE-20261030.md](CASUALTY-POSE-REFERENCE-20261030.md). Full regression/private packed acceptance is pending. The user subsequently authorized two original casualty poses; existing rescuer locomotion and earlier animations remain unchanged. No ACE code/assets/dependency are imported. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are implemented and have current native/private-package validation; visual review and human server acceptance remain open. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. The subsequent explicit exception permits two original BCM casualty poses only. Existing rescuer locomotion and earlier custom assets remain unchanged; other animation creation/modification requires a focused user decision. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


The local project contains source art beside compiled runtime assets. Build a clean engine package; do not distribute a copy or ZIP of the entire development directory.

The current AI-integrated planning hash audit is `.checks/release-development/medical-ai-20261030/release-candidate-audit.json`:791 candidate files and251 excluded/unclassified files. The earlier source-life audit has789 candidates and predates BCMM_ZMedicalAI.c and BCMM_ZStockSalineSourceCodec.c. This is an inventory, not a final engine package or licence clearance. The audit script is staged separately while clinical regression holds production code unchanged.

Keep Blender/FBX source, backup files, raw source maps, vendor archives, tests, diagnostic profiles, authoring tools and paid receipts outside the distributed game package. Review compiled resource dependencies through the engine build; the allowlist is only a planning aid. Maintain the existing source-author attributions and modification notes in SUPPLIED-MEDICAL-ASSETS.md and the Bohemia samples licence.

Release remains pending final source-licence/receipt verification, the replacement cylinder/pill props, current clinical/world/vehicle acceptance, clean-install platform builds and live network/load validation. Git publishing remains deferred. None of these planning artifacts is a published release.


Private local PC engine packing succeeded on29 October22:25:33 in candidate-20261029-214847-473: one83,236,673-byte PAK with422 entries, no flagged development/source archives. Packed-output-only native authority634/0 and720p UI26/0 passed with unchanged PAK hashes in smoke222616-070. This is a private smoke, not full platform/clean-install/network or licence acceptance. The existing retained corpse reference assets are included; a separate reviewed build copy may reduce approximately16.9MiB without deleting production references. No publication occurred.


The later stock-consumable commit fix is outside that baseline package. Curated candidate225341-700 includes the fix, retains1034 source-copy files after eight reviewed reference exclusions, and awaits its native build/runtime. No production reference assets were deleted.


The curated current-source build subsequently succeeded:419 entries/65,521,683-byte PAK (about21% smaller), no re-created omitted resource and no flagged private/authoring/unsafe entry. Extended packed-only native checks pass634 core,26 UI,649 stock commit/reuse and701 four-family cargo renderer assertions in smoke230519-328; PAK hashes unchanged and no source fallback. This remains private PC validation; production reference assets are retained and release/licence/platform/network requirements remain open.

The subsequent pre-item-timer checkpoint candidate20261030-003816-344 packed421 entries into65,524,547 bytes, SHA25637344FA7D88DF42D67024AFC67AF0EF40769FADDFA6F863AF555A8952258A601. Smoke012416-602 passes seven packed-only phases642/26/657/656/657/661/709 with unchanged package hashes and exclusively packed Medical origin. This packet predates the independent physical stock-source timer, format guards and same-source/model correction.

Full48 snapshot030939-332 passes unchanged at04:03:42UTC on30 October. Candidate031340-185 then packs422 entries into65,526,748 bytes, SHA256659D481544080E0F5033C7FF76B35C4DC89F221E71CF8887E57EB31FA3C5EC36, with zero inventory findings. Its packed-acceptance.json verifies nine actual phases:644 authority/26 UI/659 commit/658 access/659 source move/663 FullHeal/711 cargo/668 retrieved FullHeal+unused seed/650 original-source expiry restart. All phase origins are packed-only, all PAK hashes match and97 diagnostic hashes were reverified. Preserved failed overall harness attempts are explained in that aggregate; their valid phase records are not relabelled as overall passes.

The later manual-codec/null-source hardening and integrated qualified NPC fracture aid are outside this422-entry snapshot. Their focused checks pass, and combined full49 regression053005-482 passes unchanged. Candidate053017-458 subsequently passes native packing, inventory review and all nine packed-only phases. No previous packet certifies a later code change. All packages remain private PC evidence; publication, complete platform/input/network acceptance and final supplied art/provenance remain open.





Current full49 snapshot053005-482 passes unchanged at06:29:17UTC. Candidate053017-458 verifies the accepted source and private-copy hashes, then native packing completes with424 entries/65,529,138-byte data.pak, SHA256 C6806AA35C871FFB907F75F662FD062FCFCC9F822A4954EE3375625747FE4E80 and zero inventory findings. Nine-phase packed runtime smoke062926-881 PASSES:655 authority/26 UI/670 stock commit/669 access/672 source move/674 FullHeal/722 cargo/679 retrieved seed/661 restart, zero failures. PackagesUnchanged=true and every Medical origin is under the packed output; no source fallback. Original source assets remain retained; the eight reviewed omissions affect only this private build copy. No publication or release clearance is implied.
