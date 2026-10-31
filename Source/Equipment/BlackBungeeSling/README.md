# Black two-hook bungee sling — review revision

Original static Blender geometry based on the supplied jag1-23 photograph. This replaces the visual direction of the earlier woodland sling; that earlier work is preserved separately.

- `BCM_Black_TwoHook_BungeeSling.blend`: detailed editable source, packed original 2048 px woven nylon textures, transparent render setup.
- `BCM_Black_TwoHook_BungeeSling_Inventory.fbx`: 18,000 triangle export candidate. Requires engine import/material setup and validation before game use.
- `Black_Bungee_Sling_Preview.png`: transparent overall render.
- `Black_Sling_Hook_and_Stitching_Closeup.png`: geometry and sewing detail.
- `Black_Nylon_Weave_Closeup.png`: original fabric material detail.
- `Black_Sling_Threading_Oblique.png` and `Black_Sling_Threading_Back.png`: angled and rear checks of the folded clip attachment.
- `Black_Sling_Adjuster_Threading.png`: webbing routed through both adjuster slots around its centre bar.
- `Black_Sling_Transparency_Review.png`: checkerboard review composite only; not an asset background.

Inventory-only: no rig, animation, worn attachment, gameplay implementation, or white background geometry. The supplied photograph and its watermark are not used as textures.

Revision: end tabs are now continuous folded strips passing through the clip slots, with a return layer behind the hardware. Three adjusters are aligned with their strap runs and the strap passes through both slots around each centre bar. Revised finer basket weave, yarn normals and textile reflections replace the earlier material.

Subsequent shape revision: removed disconnected lead segments, replaced the pinched inner path with a broad continuous coil, made closed-loop sampling periodic, and extended the clip returns into the elastic junctions. Remaining strap terminations are routed behind the coiled webbing. The full render was compared visually with the supplied photograph; no numerical or exact-match claim is made. The layout remains simpler and more regular than the photograph.

Visual status: review draft, not an exact replica. Fold layout, gathering, hook contours and surface finish still require comparison and refinement to reach the requested photographic resemblance. The reduced export has not been validated in Workbench or on a dedicated server. No claim of game readiness or exact-match accuracy is made.

Generation and review scripts are retained for reproducibility. See `asset-report.json` for geometry counts and structural checks.

Approved shape and material revision: the broad-loop shape was accepted by the user. `roughen_nylon.py` applies the subsequent material-only change: coarser weave, stronger yarn relief and higher roughness with reduced sheen. Run it after the build/export scripts when regenerating. It checks an exact mesh-coordinate hash before/after the edit and retains `BCM_Black_TwoHook_BungeeSling_ApprovedShape.blend` as the previous appearance. The main `.blend`, transparent overall preview and weave closeup contain the revised material; other closeups may show the earlier smoother material.

Fabric junction refinement: `blend_fabric_junctions.py` subsequently refines the accepted layout. The gathered sleeves now gradually flatten over their final section into the sewn tabs. Duplicate floating edge piping on both clip tabs is removed, and the tight centre-adjuster return is eased. This is a targeted geometry change, so the earlier material-only mesh hash is superseded. `Black_Sling_Fabric_Junction_Closeup.png` shows the transition. A pre-refinement source backup is retained.

Current regeneration order from scratch: `build_black_sling.py`, `roughen_nylon.py`, `blend_fabric_junctions.py`, then `finish_black_sling.py`, each loading the main saved `.blend` after the initial build. The finish step recreates the reduced export and refreshes the review images. Native engine validation remains outstanding.
