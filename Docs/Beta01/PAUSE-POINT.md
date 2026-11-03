# Beta Test 01 — pause point

**Historical pause checkpoint: 2 November 2026, 10:27:58 UTC. The user subsequently explicitly resumed work. See `ISSUES.md` and `evidence.json` for subsequent results; the unfinished-work list below describes the pause, not current status.**

All five remaining task-owned native game/Workbench processes were stopped after verifying their process IDs and command lines. No user game or unrelated application was stopped. No tests remain intentionally running. The unfinished equipment runner reports failure after its process was stopped; that interruption is not a completed test or a newly discovered assertion failure.

## Workspace and changes

- Project: `C:\Users\ruanb\Documents\My Games\ArmaReforgerWorkbench\addons\MCB Medical`.
- Branch: `codex/standalone-medical`, starting commit `015e6c49c33d1aad15e945d9f83304857db8894d`.
- Source feedback: `Docs/BETA-TEST-01-FEEDBACK-2026-11-02.md`, copied from the user-supplied Downloads file. It was read; work was not inferred from the filename.
- Changes are local and uncommitted. Nothing was pushed or published to Workshop.
- Repairs cover model scale, packing and native preview angles; blood/plasma/saline appearance; duplicate BVM mask; injector orientation; preview recovery; body-bag hold/cancel, exact inventory reconciliation, cargo callback recursion, grounding and manual/rope movement; device timing/cancellation feedback; progress bars; and patient/life/domain examination knowledge.
- Detailed evidence and caveats: `ISSUES.md`, `catalogue.json`, `evidence.json`, and `Evidence/` in this directory. Failed logs remain in `.checks`.

## Completed evidence

| Scope | Completed result |
|---|---|
| Settings state | 279 settings × 200 cycles; 55,800 state cycles; 223,880 assertions including baseline; zero failures. These are not 200 gameplay/persistence/network cycles per setting. |
| Hold authority | 200 actual reservations: 100 cancelled and 100 early-completion rejections, followed by a successful recovery chain; 1,441 assertions, zero failures. |
| Human hold-F | User replied **“it worked as intended”** to the prepared early-release/12-second circular-hold test. Repetition count unspecified. |
| Exact requested recovery | 47 original entities, 13 owning roots, including two smoke grenades and two demolition blocks; 865 assertions, zero failures, through manual movement and cargo round trip. |
| Local examination menu | 200 A-B-A switches, zero failures; relevant airway-device change invalidated the finding. |
| Dedicated examination privacy | Two actual owners, 200 switches each, zero failures; only the examiner received the finding; relevant change invalidated it. |
| Native preview pilot | 26 handheld equip/stow cycles, zero failures; three inventory-only entries excluded. |
| Long equip campaign | **4,360 cycles recorded**, across 22 handheld resources, zero recorded failures. 21 resources reached 200; suction reached a recorded 160. The whole campaign is **incomplete**. |
| Clinical regression segments | 33 steps passed in the first frozen segment; 15 further steps passed in the remainder, including full oxygen/medication, typed fluids and field procedures. Segments use documented different snapshots; do not describe them as one completed unchanged-build campaign. |
| Packed addon | Local `data.pak` built successfully; isolated packed baseline 680, recovery 865, assessment 1,892 and UI 28 assertions, all zero failures. Loose production source excluded. |
| Visual evidence | Labelled native icon board, active progress menu/HUD, examination retention/invalidation, loaded bag ground-contact and continuous rope screenshots. All-view/LOD/terrain acceptance remains broader than these captures. |

Packed artifact: `.checks/beta01-packed-20261102-101844-508/packed/data.pak`, SHA-256 `0FC25731CBD635F47604DC3C799D0E9D874A6B30A4BB5191BF172EF9EF5A6EA3`. Its source and hash manifest are alongside it. This is a local test package, not a published release.

## Resume priorities

1. **Drag network is still failing.** Latest run: `.checks/knowledge-network-snapshot-20261102-101051-702/.checks/drag-network-20261102-102601-799`. Server reports `Authenticated player RPC releases native drag reservation` failed. Inspect every client phase before changing production: the observer requires a frozen published position within 0.35 m before arming its release request, even while the bag is physically converging. Determine whether this is a fixture sequencing problem or a real RPC/lifecycle failure. Do not mark multiplayer dragging verified.
2. Finish the equipment campaign, then native medication/TXA. Frozen source: `.checks/equipment-final-snapshot-20261102-102406-316`; interrupted native run `.checks/20261102-102412-667` inside that snapshot. It reached **Left chest drain** with no recorded assertion failure before the user pause. Earlier fixture assertions incorrectly required a short injection animation to remain active at the midpoint of a longer configurable authority timer. The updated fixture checks retained exact supply and authority lease, with final physiology/consumption checked separately.
3. Finish the remaining repeated equip coverage. Original profile `.checks/beta01-item-review-20261102-093224-436`; all maximum completed counters are in its console log and `catalogue.json`. Suction has 40 unrecorded-to-target cycles remaining; SurgeryKit, SutureKit, TXA and VascularAccess each have 200 remaining. Any resumed campaign must retain previous counts and identify its new source/profile; do not invent continuation within a stopped process. `Tools/Start-Beta01ItemReview.ps1 -Cycles 200` currently starts the entire list again; add a carefully validated resource selection if avoiding repetition.
4. Complete broader sloped/obstacle/disconnect/control checks and final static review. The earlier `-TerrainRuntime` result was a native baseline on the terrain world, **not** a full terrain dragging matrix.
5. Refresh the evidence collector and issue statuses after new results. Do not regenerate the catalogue without recollecting evidence; its generator initializes counts to zero.

## Reproduction notes

- Native Steam initialization fails in the restricted sandbox. Authorized native runs require outside-sandbox execution. Use the installed game/Workbench diagnostics and preserve logs.
- Serialize direct diagnostic dedicated-server worlds: they bind port 2001 despite the existing harnesses' random `-port` arguments.
- Do not change files in a running snapshot. Use `Tools/New-Beta01Snapshot.ps1` when production changes continue during a campaign.
- The packed recovery fixture originally hit an accidental half-metre step made by narrow doorstep strips. It now uses a single 12 m flat test deck above the world floor. The earlier failed packed attempt is retained.
- The first broad segment stopped because the snapshot lacked the historical saved-world profile. The remainder passed that check using the real original profile, without modifying it.
- Drive space was reclaimed by removing this task's downloaded Blender ZIP and redundant asset copies from failed snapshots. Logs and evidence were preserved. Do not delete unrelated files or active snapshots.
- Machine-readable pause and regression state: `.checks/beta01-pause.json`, `.checks/beta01-pause-processes.json`, `.checks/beta01-pause-regression-summary.json`.

Do not publish, commit, or restart native tests merely because this checkpoint exists. Resume when the user asks.
