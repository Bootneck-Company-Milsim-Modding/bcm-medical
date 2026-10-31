# BCM Medical asset purchase brief - 28 October 2026

Current engineering checkpoint: the agreed player-focused changes passed all 51 validation stages with documented diagnostic-only continuity, 12 packed-only phases and a packed dedicated-server/three-client transport test (701/0). The private package is built. See [PLAYER-TEST-CANDIDATE-20261030.md](PLAYER-TEST-CANDIDATE-20261030.md) for exact evidence, package location and remaining human/Internet/art acceptance. The latest carry revision is not marked visually approved by the user. Earlier checkpoint paragraphs below are historical where superseded.

## Current source checkpoint — 30 October 2026

Recording has finished and native development has resumed. See [RESUMED-DEVELOPMENT-20261030.md](RESUMED-DEVELOPMENT-20261030.md) for current evidence. Autonomous medical AI is removed; the approved inventory sling is imported. The catalogue is 279 settings/schema35. Compact medical-menu carry/drag controls, conscious-player consent and sling retention have native functional tests, including dedicated owner/proxy/late-join coverage. Casualty poses remain visually unfinished; see [CASUALTY-POSE-REFERENCE-20261030.md](CASUALTY-POSE-REFERENCE-20261030.md). Full regression/private packed acceptance is pending. The user subsequently authorized two original casualty poses; existing rescuer locomotion and earlier animations remain unchanged. No ACE code/assets/dependency are imported. Earlier snapshots and queue entries below are historical.


## Current user scope — 30 October 2026

The controlling decisions and audit are in [SCOPE-AND-READINESS-AUDIT-20261030.md](SCOPE-AND-READINESS-AUDIT-20261030.md). Living casualty carrying (including conscious broken-leg casualties after explicit acceptance), living casualty dragging and BCM-owned unconscious weapon retention are implemented and have current native/private-package validation; visual review and human server acceptance remain open. Body bags use manual/rope dragging only; filled-bag carrying is outside scope. Two-player stretcher carrying remains shelved; if resumed, place the stretcher at hip height and use normal base-game walking.

Reuse suitable base-game animations; otherwise use an on-screen progress bar. The subsequent explicit exception permits two original BCM casualty poses only. Existing rescuer locomotion and earlier custom assets remain unchanged; other animation creation/modification requires a focused user decision. Autonomous medical AI removal is authorized and now performed in active source; preserve player medical care and player treatment of AI casualties. Earlier conflicting animation procurement and AI expansion tasks are superseded.


This list follows the supplied archive and folder inventory. It distinguishes finished inventory models from the additional parts needed for believable treatment presentation. No purchase has been made by the assistant. Keep two-player stretcher carrying shelved.

| Priority | Asset | What to look for | Existing coverage / reason |
| --- | --- | --- | --- |
| Now | Portable oxygen cylinder | Cylinder, valve, regulator/flowmeter and separate hose; carry bag optional. Separate moving valve/knob meshes are useful. | The finite gas/source logic and supplied face mask work. There is no supplied standalone cylinder. The user is sourcing this. |
| Now | Tablet packaging | Blister pack or pill bottle with lid, separate tablet, clean editable labels. A generic unbranded product is sufficient. | Paracetamol currently uses the supplied pouch as an inventory placeholder. The original self/handover motion does not require buying a duplicate character. |
| Next | Complete chest-drain assembly | Chest tube, connections, clamp and collection/water-seal bottle or chamber; sterile kit packaging optional. | The supplied injection-instrument donor has assorted parts, duplicated layouts and some missing UVs. It is not a finished chest-drain assembly. |
| Next | Surgical and suture hand tools | Scalpel, forceps, needle holder, scissors, curved needle and thread, ideally as separate meshes. | Supplied surgery/suture pouches are already present. Their existence does not supply convincing working instruments. Inspect the donor set before buying overlapping individual tools. |
| Next | Game-ready airway adjunct | Oropharyngeal airway or laryngeal tube with appropriate connector and cuff geometry for the chosen type. | The supplied oral-airway STL needs source/licence confirmation and runtime UV/material work. The laryngoscope is already imported; do not buy it again. |
| Optional visual upgrade | Fitted tourniquet | Open/applied strap and windlass, separate strap parts, suitable for arm and leg fitting. | Current applied tourniquet is an original technical placeholder. Inventory tourniquets use native supplies. |
| Optional visual upgrade | Deployed medical splint | Fitted straight/supporting form with separate straps, preferably multiple leg sizes or a deformable mesh. | The supplied packaged splint is imported. The current deployed visual is an original technical approximation. |
| Later, only if inventory deployment is wanted | Folded stretcher | Packed/folded carry form matching the supplied deployed stretcher. | The stationary stretcher already exists. It is a world object, not an ordinary pocket item, and two-player carrying is shelved. |

## Do not repurchase yet

AED main unit and separate pads, pulse oximeter, BVM, suction unit, monitor, cannula, chest seal, packaged splint, surgical/suture pouches, body bags, existing injectors and the deployed stretcher are already available. The newer blood-bag source needs reduction/material work before replacing the simpler bag; another blood-bag purchase is not the first step. Transparent plastic, PBR packing and attachment alignment are finishing work on supplied assets, not evidence that every model needs replacing.

## Delivery checklist for models

- Supply the original download archive, product licence/receipt, FBX or glTF/GLB (BLEND is also useful), and all texture files. These are project delivery requirements, not a guarantee that a particular marketplace licence permits every distribution.
- Prefer game-ready topology with UVs, normals and usable lower-detail versions. A dense render-only sculpt or untextured printing STL requires additional work.
- Provide base colour, normal, roughness and metalness/packed-map definitions. Include opacity/transmission information for clear plastic and glass. Editable label textures help identify medication accurately.
- Separate valves, caps, pills, hoses, instruments and straps are preferable to one fused mesh. Keep the realistic proportions and source scale documented.
- Verify the product terms cover use in a distributed game mod. Do not buy an asset whose delivery is restricted to rendered images. Preserve attribution and do not publish raw purchased source files with the mod.

## Animation policy

Only the two original casualty poses are authorized for new animation authoring; other animation procurement or authoring remains deferred. Use suitable base-game animation; otherwise show timed progress. Existing custom clips remain frozen. Only a concrete indispensable blocker justifies returning to the user to discuss focused native reuse, purchase or creation. Hand-contact polish and bespoke clips are not prerequisites for progress-only treatment.

## Current release blockers

Final oxygen-cylinder integration and pill/pack contact depend on the supplied replacement assets. Current masks, pouches and other working placeholders do not mean all high-quality release art is finished. The broader completion ledger remains in ROADMAP.md; local fixture success does not replace live Internet/load acceptance or complete casualty/world/account persistence.
