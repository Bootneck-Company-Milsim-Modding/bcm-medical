# BCM sling — inventory-only Blender source

## Current primary files

- BCM_Sling_ReferenceMatched.blend — editable original PBR source with flat padded upper webbing, volumetric rounded/gathered sleeves, threaded webbing and manufacturer-informed mechanical hardware.
- BCM_Sling_ReferenceMatched_Inventory.fbx — 12,000-triangle reduced export candidate.
- Sling_ReferenceMatched_Preview.png — transparent 600x1000 view aligned to the supplied photograph.
- Sling_ReferenceMatched_HighQuality.png — transparent 1200x2000 original-PBR render.
- Sling_Hardware_Closeup.png — oblique view of actual hardware geometry.
- Sling_Reference_Overlay.png — cyan model silhouette over the supplied photograph. White here belongs to the diagnostic reference, not the model.
- reference-comparison.json — exact image-space measurements and limitations.
- match_reference.py — primary reproducible build. Uses original helper code in build_sling.py and hardware-pose-fit.json. Set the REFERENCE path if rebuilding elsewhere.
- Sling_Multiterrain_BaseColor.png / Sling_Webbing_Roughness.png / Sling_Webbing_Normal.png — original 2K fabric maps, also packed into the primary blend.

Earlier BCM_SinglePoint_WeaponSling.blend, BCM_Sling_Inventory.fbx and Sling_Product_Preview.png are initial shape studies, superseded by the ReferenceMatched files above.

## Construction and scope

Static inventory art only. No armature, wearer, equipping, character attachment or animation actions. No weapon-retention gameplay is implemented by this asset. The final model has no backdrop mesh and its PNG previews have alpha transparency.

Upper adjustment webbing is flat; the wider upper section is shallow foam-padded webbing. The lower fabric sleeves have nearly round cross-sections and irregular gathers around an implied elastic core. Manufacturer detail images informed the separate male/female side-release parts, lower ladder-lock, forged QD wire bail, knurled cylindrical collar/stud, push button and detents. Hardware placement/rotation/uniform estimated size were fitted to the photograph without deforming mechanical meshes into a photo card.

Manufacturer references:
https://wildecustomgear.com/products/padded-single-point-sling
https://grovtec.com/products/standard-push-button-swivel
The manufacturer specifies a 2-inch padded section, closed-cell foam/1000D Cordura, industrial bungee, side-release buckle and Grovtec QD option. Actual dimensions and hidden surfaces are not established by the supplied photograph.

## Separate photograph-projection study

BCM_Sling_ReferenceTextureStudy.blend and Sling_ReferenceTexture_Study.png are explicitly photo-derived comparison studies. They project the supplied photograph onto the real cloth geometry, retaining its baked illumination. They are not evidence of an independently authored exact camouflage reproduction, calibrated PBR albedo or hidden-surface accuracy. The main ReferenceMatched blend uses original procedural PBR textures, not a photograph card or reference-photo alpha cutout.

The study's reference-derived texture is Sling_ReferenceFabric_Study.png. Public-distribution provenance for the supplied product photograph has not been established; do not silently promote this study into a cleared game package. The original PBR version is kept separate.

## Verification and remaining differences

The current image-space measurements are in reference-comparison.json. They are silhouette measurements of the detailed opaque 3D mesh, not a whole-asset quality percentage, not confirmation of perfect hardware, and not acceptance of the reduced export. Max RGB below 0.94 defines the reference silhouette; model alpha above 0.5 defines the rendered silhouette.

An earlier 96.8% silhouette result predates the proper mechanical hardware reconstruction and is superseded. The current original-PBR version is NOT confirmed perfect or near-exact overall. Camouflage placement, photographed folds, hardware pose, unseen geometry and exact dimensions remain approximations. No false perfect/near-exact acceptance is recorded.

Packed source textures and zero armature/animation-action counts were checked. The GPU used is an NVIDIA GeForce RTX 5070 Ti, explicitly authorized by the user after recording temporarily stopped.

Enfusion import/material conversion, collision, reduced-LOD appearance, normal/texture transfer, inventory icon readability and in-game validation remain pending. No mod publication or gameplay integration occurred.

Earlier build_sling/finish_sling/finalize_sling scripts reproduce initial studies; match_reference.py is the current primary model. fit_hardware_pose.py is an authoring fit utility and is not a gameplay component.
Current measured silhouette overlap: 94.55%. Boundary 95th-percentile distance: model-to-reference 8.06 pixels; reference-to-model 5 pixels at 600x1000.
