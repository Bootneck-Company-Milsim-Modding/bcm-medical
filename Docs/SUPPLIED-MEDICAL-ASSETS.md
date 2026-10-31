# Supplied medical assets

The source inventory records archive hashes and individual files from the supplied Mod Assets.zip. Original downloads remain in ignored `.checks/vendor-medical`; the runtime uses converted models and packed native textures.

## Current selection

- Better Epinephrine: original top LOD selected from the multi-LOD display file.
- Better Morphine: textured morphine-labelled injector.
- Better Pulse Oximiter: preferred 4,124-triangle model rather than the much heavier alternatives.
- GenericInjector: 476-triangle green injector, plus separate Naloxone, NCD and TXA variants. User-approved edited albedos affect only the originally green cap/grip geometry; the printed body retains its original material. The original roughness and normal maps are retained. User confirmed the supplied colours on 27 October 2026: brown/gold Naloxone, blue NCD and orange TXA. The green original now provides the unlabelled fentanyl inventory placeholder.
- AED: supplied model converted to a native resource and assigned to the AED item.
- FluidBag: supplied FBX has no accompanying textures. A temporary basic material is assigned; final bag/label artwork remains outstanding.
- Oxygen mask: used by the finite portable oxygen kit and replicated mesh-only applied mask. Final facial contact, transparent plastic and a separate cylinder remain art work. Its embedded licence is noncommercial; see source records below.
- Better Injector: retained alternate with morphine printing, not used as the generic NCD kit.
- CS2-derived MediShot: not selected for runtime use.

## Source credits and embedded terms

The following are embedded source records, including alternatives. Presence in this inventory does not mean an asset is used in the runtime. Keep purchase receipts and product licence terms for the paid FBX packs: the archive does not contain a complete EULA for every model.

- **IPAD SP1 Automated External Defibrillator – AED** — R3D Reality (https://sketchfab.com/redonemart). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/ipad-sp1-automated-external-defibrillator-aed-faff5cbba8734373bed35e00291bf9a1
- **Autoinjector** — Milch Frische (https://sketchfab.com/MilchFrische). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/autoinjector-e426b178833847919a1ba24381d0066b
- **HEALTHSHOT | Injector Model (CS2)** — 6lucius (https://sketchfab.com/6lucius). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/healthshot-injector-model-cs2-c4829f084346419888184552f5d2cb62
- **Morphine Auto Injector** — doodyk2019 (https://sketchfab.com/doodyk2019). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/morphine-auto-injector-338fcd5ea9de466ab71641ff9f528724
- **Oxygen Mask** — elouisetrewartha (https://sketchfab.com/etrewartha). CC-BY-NC-4.0 (http://creativecommons.org/licenses/by-nc/4.0/). Source: https://sketchfab.com/3d-models/oxygen-mask-1614f4e9974c4789a0b09865d3b4a7f3
- **Pulse oximeter Ⅱ** — 星医療酸器 (https://sketchfab.com/7634hosi). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/pulse-oximeter-ii-045c7ea17bb9441da84f6c3dc2283062
- **Pulse oximeter** — 星医療酸器 (https://sketchfab.com/7634hosi). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/pulse-oximeter-2f65771d9823465faf83a24e27ec14c6
- **Medic stim** — Cailjones (https://sketchfab.com/Cailjones). CC-BY-4.0 (http://creativecommons.org/licenses/by/4.0/). Source: https://sketchfab.com/3d-models/medic-stim-efe7dec45d21435e822403bd79223e9b

Conversions include unit/pivot adjustments, simple collision, source LOD selection and texture-channel packing. The generic injector variants additionally use the user's cap recolours and a material split preserving the source body. These are modifications; source authors do not endorse BCM Medical.

The pulse-monitor beep is an original generated PCM sample. The current implementation is local playback with a distance gate; dedicated spatial sound presentation remains outstanding; the mesh-only finger attachment has functional owner/proxy/late-join acceptance, with final contact still requiring art review.

## Rebuild and checks

Prepare base materials only when intentionally rebuilding the source import, then run Build-InjectorVariants.py to preserve/repack existing *_Albedo_EDIT.png files. Build-MedicalModels.py exports native FBX/material assignments; Generate-BuildPlugin.py generates the Workbench rebuild plugin. Verify-MedicalMaterials.py rejects missing native material mappings and missing packed texture dependencies. Editing the user source PNG does not alone update compiled textures: repack and rebuild are required.


## Additional supplied assets, 27 October 2026

Source folder: `C:/Users/ruanb/Downloads/Mod Assets`. The second inventory and mesh inspection are NEW-MEDICAL-ASSET-INVENTORY.json and NEW-MEDICAL-MODEL-INSPECTION.json. Original files remain unchanged. Twenty-two source files were staged and all archives extracted successfully.

The native material/compiled-model checks now cover 22 imported model entries. Added runtime meshes include the supplied stretcher, splint, surgery kit, suture pouch, separate AED electrode pads, BVM, monitor, cannula/vascular-access item, suction device and supplied chest seal. The existing AED main unit is retained. The laryngoscope is converted as a staged model, with no invented laryngoscopy procedure or item workflow implied.

- **Chest seal:** referenced `chest-seal-valve-2.jpg` was absent. A new BCM albedo follows the supplied UV layout. Editable generated sources: `Source/MedicalTextures/ChestSeal_Albedo_v1.png` and `_v2.png`; version 2 is active. Pale adhesive and red valve are separate atlas regions. It uses a flat normal map pending finer surface work.
- **Stretcher:** supplied albedo is used; referenced metalness and roughness images were absent. Neutral native roughness/metal values are temporary. The high-detail source remains preserved; final LODs and carrier hand contact need work.
- **Cannula:** supplied materials/UVs but no bitmap textures. Rebuilt plastic/metal colours replace missing surface inputs, and the mesh is reduced for runtime. Clear-plastic transparency still needs polishing.
- **Suction:** supplied screen/pump/switch/LED colour maps are used where they match. Other materials are reconstructed. The high-detail mesh is reduced; final translucent parts and the airway suction attachment remain presentation work.
- **BVM:** supplied colour/normal/roughness/metal/AO are packed. Transparent parts currently use the opaque native material path; final clear-plastic treatment remains outstanding.
- **Surgery/suture pouches:** supplied colour/normal textures work. Their packed MAORS convention needs confirmation before using all packed channels; neutral roughness/metal values are used meanwhile.
- **Drainage donor set:** multiple instrument parts, duplicated layouts and some parts without UVs. Retained in staging for selection/assembly; not falsely represented as a finished chest-drain prop.
- **New blood bag:** 50,000-triangle source needs reduction/material work before replacing the existing simpler fluid-bag item.
- **Oral airway:** STL intended for 3D printing, with an attribution/share-alike notice. Kept staged while confirming the exact source licence/version and preparing a suitable runtime mesh/UVs.

No additional purchase is needed for the current functional prototypes. Missing original PBR maps, receipts/product licence terms and final art/animations should be retained or supplied when available. Purchase alone is not used to infer redistribution terms absent from a package.


## Applied variants, 27 October

The manifest now contains 27 compiled model/material entries. Five added variants are separate upper/lower donor AED pads, an applied cannula with the sharp stylet and spare caps omitted, and original BCM applied splint/tourniquet meshes. The splint is a shaped board with lining and straps rather than the rolled inventory package. Their editable sources are under Source/MedicalModels/AppliedSplint and AppliedTourniquet; technical flat-colour PBR maps are under the matching Assets/Medical folders. Purchased originals and the user's injector edits remain unchanged.

Mesh-only cosmetic prefabs are in Prefabs/MedicalVisuals. They follow replicated treatment state and do not contain inventory, replication or physics components. Native local checks cover eight simultaneous pads/seals/splints/cannula/oximeter appearances, GM toggles, range culling and healing cleanup. This is functional presentation evidence; varied uniforms, first-person clipping, close-up alignment, transparency and final tourniquet placement still need visual acceptance.

## Original BCM additions, 30 October 2026

The inventory-only black two-hook bungee sling is original Blender geometry/material work made from the supplied photographic reference. Its photograph background and watermark are not model geometry. Source is retained under Source/Equipment/BlackBungeeSling.

The two original casualty loops in Source/Animations/Casualty use Bohemia's official sample rig/skeleton, with attribution and terms retained in Source/Animations/BOHEMIA-SAMPLES-LICENSE.txt. ACE was inspected as visual/method reference only; no ACE animation, model, prefab or code is imported into this standalone mod. Rescuer locomotion uses existing base-game animations.

