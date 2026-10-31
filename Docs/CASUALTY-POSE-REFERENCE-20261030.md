# Casualty pose reference — 30 October 2026

User requirement: the players being carried and dragged must have poses similar to ACE Carrying. BCM remains independent of ACE. The user explicitly approved a narrow exception: **two original BCM casualty poses if needed**, while retaining existing rescuer walk/crouch animations and using no ACE animation data. Other animation creation/modification remains outside scope.

## Reference inspection

Primary source: https://github.com/acemod/ACE-Anvil/tree/master/addons/carrying/Assets/Helpers/Workspace

ACE supplies dedicated `p_carried_*` and `p_dragged_*` clips, including in/loop/out, and separate carried/dragged animation instances. Its carried helper attaches around the rescuer's upper spine; its dragged helper attaches around the root. These are dedicated casualty poses, not simply ordinary prone stance raised above the floor.

The public Blender source was downloaded into the excluded `.checks/ace-pose-reference-20261030` directory for visual inspection only, with embedded scripts disabled. Reference-only stills were rendered without saving the source or exporting animation assets. No reference geometry, animation curves, rig settings, clip data, prefabs or source code were imported into production BCM.

- Carried reference: casualty torso folded over the rescuer's shoulders, head and arms hanging, legs bent. Original BCM acceptance must check support/contact, torso curvature and hanging limbs from both sides while moving and turning.
- Dragged reference: casualty facing upward, torso raised, hips lower and legs extending along the ground. Original BCM acceptance must check head/shoulder support, foot clearance, turning and slopes.
- Rescuer stays on existing base-game walk/crouch locomotion, per user instruction; dragged movement remains crouched and allows changing direction as requested, regardless of ACE's movement restrictions.

## Superseded placeholder gap

The earlier ordinary prone/collapsed poses were functional placeholders. The conscious prone carry is too straight and does not reproduce the folded shoulder shape. The conscious prone drag is face-down rather than the raised face-up reference. They are **not visually accepted**.

The installed base-game resource database contains ordinary injury/unconscious poses and ambulance/vehicle patient clips. A name inventory found no `carried` or `dragged` clips. Names alone do not prove an appropriate reusable pose is absent; any selected native alternative needs visual inspection. Do not force a conscious patient medically unconscious to obtain a pose.

## Functional baseline

`.checks/living-transport-network-20261030-142953-507`: dedicated server and three clients, 693 assertions, zero failures. Actual owners exercised request, rejection, acceptance, timeout and release; late join observed carry/drag/release. Server and patient owner confirmed the original rifle remained in inventory with a sling following native unconsciousness. This is functional evidence only, not pose acceptance.

Original pose authorization does not authorize copying ACE assets or modifying rescuer locomotion. New poses require renewed owner/proxy/JIP, release, wake-up, disconnect, collision and full regression checks before player-test readiness. The drag draft was visually accepted by the user; the carry draft was rejected for twisted anatomy and an arm behind the neck and is being rebuilt with a static carrier visible. Do not mark carry visually accepted yet.

## Clinical regression found during this work

The first combined run stopped on the clinical safety injection-start assertion. A focused rerun reproduced it. Isolating the new falling event override restored native injection playback; the correct inactive fallback is `HandleFallingDefault`, rather than the empty inherited event. `.checks/20261030-145809-339` then passed 714/0 with that correction. Temporary diagnosis logging was removed afterward. The interrupted combined run is failed, not accepted; a complete current-build run remains required.


## Original native implementation

The corrected carry has both arms hanging beside the head and torso, with separate knee and elbow targets. The original drag draft is retained. Both 31-frame static loops use the official Bohemia sample skeleton and EBT FullBodyABS export. Native Cargo_Base compartment holders inject the respective original graph/instance into the original patient entity, including conscious patients. No medical unconsciousness is manufactured for appearance. The holders have explicit stable root IDs and reuse the base-game replication component.

Focused local native transport: .checks/20261030-161514-179, 740 assertions and zero failures. Dedicated server plus three clients: .checks/living-transport-network-20261030-161446-036, 697 assertions and zero failures, including wake release, actual-owner control restoration, cancelled-token replay and exact sling/rifle retention. These preceded the new real-input walking test; complete current-build acceptance is still pending. The corrected carry is not yet marked visually accepted by the user.

Rebuild tools are in Tools/AnimationAuthoring. Build-CasualtyPoses.py requires a fresh attributed official sample; Export-CasualtyPoses.py requires running Workbench NetAPI on port 5775 and exports only the two original BCM actions. Render-CasualtyPoses.py renders the BCM source for visual review. Existing animation resources remain unchanged. See Source/Animations/BOHEMIA-SAMPLES-LICENSE.txt for sample terms.

## Character-as-ground regression fixed

The actual-input dedicated fixture exposed an intermittent termination: the floor probe hit the late-joining character at 2.48m instead of the 1m deck, then correctly rejected that false support height. The ground probe now retries past characters and attached equipment, using a separate exclusion list so horizontal obstruction tests still detect people. The diagnostic observer is also parked clear of the walking lane. This preserves real wall/height/path cancellation.

.checks/20261030-165012-910 passed 748/0: all four native cases explicitly confirm a raw trace intersects a character and the corrected probe reaches the solid deck. .checks/living-transport-network-20261030-164852-824 passed 701/0, with original-entity poses, actual-owner movement/turning, late join, wake release, cancellation replay and sling retention. The full 51-gate snapshot is .checks/clinical-acceptance-20261030-165059-895; its final result governs packing.


## Carry revision after lower-body/right-arm rejection

The user rejected the earlier lower-body orientation and then the right arm. The 16:50 full run was deliberately interrupted before editing; its partial passes are not a complete acceptance. Its packing queue was stopped.

The current original carry uses direct connected thigh/shin/foot rotations (80 degrees hip flexion, 35 degrees knee flexion), preserving roll through the complete chain. The right elbow and hand hang outside the carrier instead of being drawn across the neck. The head, pelvis and left arm/hand transforms remain unchanged. Drag's 11 runtime resources remain byte-identical. No rescuer animation is authored or modified.

Native .checks/20261030-171454-125 passed 748/0 and captures front and rear views for conscious and unconscious carry. The fresh attributed-sample rebuild differs by at most 0.0000244 in native bone matrices for carry and 0.00000126 for drag, consistent with floating-point evaluation. It reads no ACE assets. Visual user acceptance remains open; current full regression/private packing must be rerun after this revision.

