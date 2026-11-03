# Beta Test 01 repair and evidence ledger

Source: [original feedback](../BETA-TEST-01-FEEDBACK-2026-11-02.md), supplied by the user on 2 November 2026. Working branch: `codex/standalone-medical`, starting commit `015e6c49c33d1aad15e945d9f83304857db8894d`. These are local, unpublished changes.

**Broader acceptance remains open.** Native assertions, visible screenshots, real owner/proxy networking, human controls and packed-addon testing are separate evidence categories. Completed packed checks are recorded below. Logs under `.checks` include preserved failed attempts; only explicitly identified completed runs count.

**Resumed work:** see [resumed acceptance](RESUMED-ACCEPTANCE.md). The equip campaign completed with 5,360 recorded cycles (at least 200 for each of 26 handheld entries), and the interrupted equipment/medication/TXA segment passed. Newly reproduced defects A03 (missing bag movement replication) and A04 (first-movement manual-drag cancellation) are described there. Historical checkpoints below must be read with these subsequent results.

## Subsequent acceptance, 2 November

- **User confirmed the prepared hold-F test worked as intended**, in reply to the request to check early-release cancellation, bag preservation and the full 12-second circular hold. Repetition count was not supplied. This is human validation, separate from the 200 server-side cancelled/early-rejected holds.
- Actual local owner menu: **200 A-B-A switches, zero failures**, followed by successful invalidation after airway-device state changed. `.checks/visual-final-snapshot-20261102-094805-082/.checks/beta01-item-review-20261102-095712-348`.
- Dedicated server and two real owners: **200 A-B-A switches per client, zero failures**. Player 1 examined and retained the finding; player 2 never examined and received no finding. Both saw the appropriate absence after a relevant state change. `.checks/knowledge-network-snapshot-20261102-101051-702/.checks/knowledge-network-20261102-101507-723/result.json`.
- Expanded actual casualty inventory: **47 exact original entities / 13 roots**, including two smoke grenades, two M112 demolition blocks, clothing, containers, weapon, grenade, binoculars and watch. Recovery/manual movement/cargo round trip: **865 assertions, zero failures**, `.checks/20261102-095500-780`.
- Native screenshots exposed real sleeping-body floating, rolling and nonuniform rope-render transform defects after assertion passes. Production repairs wake the committed bag, exclude its entire hierarchy from floor/path traces, align towing orientation to the supporting surface, and use uniform rope-segment scale. The loaded manual/rope/cargo visual fixture subsequently passed **189 assertions**, with a visible continuous tether. `.checks/visual-final-snapshot-20261102-094805-082/.checks/beta01-item-review-20261102-100640-792`.
- The narrow doorstep fixture itself introduced an accidental step under the offset bag collider. Preserved first packed failure rejected movement at that obstacle. The corrected flat 12 m deck passes the exact recovery chain.
- Local packed artifact `data.pak` SHA-256 **0FC25731CBD635F47604DC3C799D0E9D874A6B30A4BB5191BF172EF9EF5A6EA3**, created without Workshop publication. Isolated packed mounts, with loose production source excluded: baseline **680**, exact recovery chain **865**, assessment/cache boundaries **1,892**, UI **28** assertions; all zero failures. Results under `.checks/beta01-packed-20261102-101844-508/acceptance-*`.
- Native active-procedure screenshots show the bar and remaining time both in the menu and in the closed-menu HUD; the actual oxygen connection completes and retains its reusable source.

Final packed drag now passes native player movement, all expected owner/peer/late-join phases and real disconnect cleanup (702 assertions); final packing, recovery, eight surface/obstacle cases, living transport and clinical safety also pass. See `RESUMED-ACCEPTANCE.md` for the exact artifact and limits.

Rows below consolidate the recorded checkpoints and current follow-up. Results apply only to their stated scope; they do not establish untested console controls, every terrain/LOD combination or a 200-cycle gameplay matrix for all settings.

| ID | Repair | Current evidence and remaining acceptance |
|---|---|---|
| BT01-001 | Independent model-aware preview distance, orientation and centering across medical catalogue | All 29 entries rendered on a labelled native board; tablet pouch framing corrected in the latest review. Native assets remain subject to the wider view/LOD limits below. |
| BT01-002 | Rope physical scale, packing dimensions and camera reduced | Native backpack accepts rope together with the original bandage, splint, surgery, suture and sling; rope volume is 1.2 times the bandage reference. |
| BT01-003 | Splint model reduced to 105 mm, packing volume 125 versus bandage 100 | Native combined backpack packing passed; splint volume 1.25 times native bandage. The 200-instance native splint campaign passed. |
| BT01-004 | Surgery/suture packing volumes 300/250; models 160/120 mm | Native combined backpack packing passed; surgery/suture volumes 3 and 2.5 times native bandage. |
| BT01-005 | Chest seal face rotation corrected | Seal face and label visible in native icon board. Wider LOD combinations remain unverified. |
| BT01-006 | AED front shown instead of pale back | Front controls and pad diagram visible in native icon and hand captures. Wider LOD combinations remain unverified. |
| BT01-007 | Sling source baked to 140 mm, compact packing | Native combined backpack packing and exact weapon-retention regression passed; compact sling volume 0.8 times native bandage. |
| BT01-008 | Injector long axis and origin aligned against native morphine; NCD native injection presentation | All 26 handheld resources have front, side and first-person native captures. User-reported reversed injector ends corrected. All six catalogue injectors reviewed injection-end-up in front, side and first-person native captures; seven source model definitions corrected. H toggle is physically confirmed and passed 200 native input-action cycles. |
| BT01-009 | Guarded recovery of missing native character-preview binding after swaps | 5,360 actual native equip/stow cycles, at least 200 per handheld resource, with ownership, attachment and head-projection checks. This does not certify every rendered pixel. |
| BT01-010 | Separate red blood/yellow plasma materials and plasma mesh | Red blood and yellow plasma visible in native icon and hand captures; distinct materials and models retained. |
| BT01-011 | Saline uses native US saline mesh | Native saline mesh displayed in the icon board. Clinical source movement, access loss, full heal, expiry, retrieval and restart cases passed on the reconciled frozen campaign. |
| BT01-012 | Excluded detached duplicate BVM mask mesh, retained attached mask | Detached source Mask object excluded; attached mask retained. Current native board and three-view hand captures are available; full LOD acceptance remains open. |
| BT01-013 | Replicated-state-driven local rope visual; drag grounding transition fixes | Final collision build passed controlled slopes, real vertical obstacles, six native terrain cases and packed owner/peer/late-join/disconnect checks. 202 completed recovered-bag chains are reconciled; the reproduced setup failure was an airborne actor correctly rejected by production. |
| BT01-014 | Manual follow distance shortened; brief falling transitions tolerated; vertical landing speed ignored | Native owner walking moves the packed bag and preserves reservation; jump/vault is blocked while occupied and restored on release. The intermittent fixture start failure was reproduced as falling after teleport; bounded grounding and explicit airborne rejection now pass. |
| BT01-015 | Filled bag spawned upright instead of inheriting corpse ragdoll roll/pitch | Exact loaded recovery chain and current controlled-surface/native-terrain fixtures passed. Earlier wall evidence was invalidated and replaced; see resumed acceptance. |
| BT01-016 | Native 12-second hold; server start/confirm/cancel lease; early confirm rejected | User confirmed the prepared early-release and 12-second circular hold worked as intended. Separately, 200 authority reservations exercised cancellation and early-confirm rejection. |
| BT01-017 | Enumerate exact hierarchy inventory entities; transfer owning roots intact; reconcile all entities before consumption; corrected rollback manager | Requested 47 original entities / 13 roots, including both smoke grenades, both demolition blocks, clothing and nested equipment, survive recovery and cargo round trip. 202 completed varied-loadout recovery/transport chains are recorded; these use repeated variants, not 202 unique random loadouts. |
| BT01-018 | Action anchor moved to bag model center | Native recovered-bag context contains all eight actions, with its anchor centred in rendered bounds. Player-camera capture verifies centrally placed keyboard prompts (760 assertions, zero failures). Physical controller interaction remains open. |
| BT01-019 | Timed device procedures no longer await unrelated bandage animation; specific cancellation reasons | 51-step frozen clinical campaign reconciled. Current packed build also completed 200 defined native safety instances (142,800 assertions including repeated baseline, zero failures) and dedicated treatment workflows. These are named fixtures, not 200 cycles for every operation. |
| BT01-020 | Increased in-menu and closed-menu progress-bar height; retained server-driven time/status | Native active-procedure screenshots verify remaining time and the enlarged progress bar in both the open menu and closed-menu HUD. |
| BT01-021 | Viewer-private exact actor/patient/life/domain knowledge; revalidate before restoring completed findings | 200 local A-B-A switches and 200 switches on each of two real owners passed; exact viewer privacy and domain invalidation checked. Packed boundary regression passed 1,892 assertions. |
| BT01-A01 | Fixed synchronous cargo callback recursion causing stack overflow on loaded inventory | Reproduced before fix; loaded bag recovery/drag/cargo chain passed after queued callback continuation. |
| BT01-A02 | Prevent same actor or folded bag being reserved concurrently for different corpses | Authority hold/cancel campaign covers duplicate reservation rejection. |
| BT01-A03 | Filled bag lacked native movement replication | Packed dedicated owner, peer and late-join tests observe physical bag motion; exact runs are indexed in `Evidence/final-packed-network.json`. |
| BT01-A04 | First movement could cancel manual drag during a brief native falling/speed transition | Grounding/airborne and ordinary packed owner movement passed. Seven corrected owner-placement runs passed at 250 ms one-way delay and 5% loss. The original speed-release attempt remains retained; incomplete velocity logs prevent conclusively attributing that particular failure to the demonstrated fixture position-correction race. |
| BT01-A05 | Manual dragging allowed native jump/vault | Reservation-aware native jump guard; dedicated owning client verifies blocked while dragging and restored after release. |
| BT01-A06 | Patient-list guidance clipped at 4K because wrapping was disabled | Reproduced with DPI-scaled native text bounds, then repaired; 720p, 3440x1440 and 3840x2160 UI checks pass 30 assertions each. Native 4K screenshot shows the complete guidance. |

## Completed native runs

| Evidence | Result | Location |
|---|---|---|
| Settings state campaign | 279 settings × 200 cycles = 55,800 state cycles; 223,880 assertions including baseline; zero failures. **Not** gameplay/persistence/multiplayer cycles. | `.checks/20261102-085432-443`, run `683c00e070c74791984b6cd3daf7041d` |
| Loaded bag chain | 841 assertions, zero failures; exact manifest, recovery, actual manual movement, release and cargo round trip | `.checks/20261102-083755-897`, run `f580d89afa0441be88e600765ba106ec` |
| Hold cancellation campaign | 1,441 assertions, zero failures; 200 rejected/cancelled holds and subsequent completed bag chain | `.checks/20261102-092331-782`, run `f57bff874e9d4b2088da69fda3abbbe3` |
| Clinical cancellation safety | 714 assertions, zero failures | `.checks/20261102-091215-625`, run `8127001d9c06478785ee0587001a2cd3` |
| UI smoke, 720p | 28 assertions, zero failures | `.checks/20261102-090314-086`, run `1ef43e50248740be8d53059977f0cbf2` |
| Native equip pilot | 26 actual handheld equip/stow cycles, zero failures; three inventory-only items explicitly excluded | `.checks/beta01-item-review-20261102-092600-842` |

The full clinical acceptance campaign runs from `.checks/clinical-fixed-snapshot-20261102-091615-282`, an immutable source/resource/animation snapshot. Its `report.json` is the authority for individual step results. Later preview/cache changes require their own validation; a snapshot result must not silently be attributed to newer code.

## Test limitations and retained failures

- Sandbox launches cannot initialize Steam; native tests run with approved outside-sandbox execution.
- Direct diagnostic `-server world.ent` fixtures bind default port 2001 despite `-port`; serialize server runs. One earlier parallel run failed from collision.
- An early snapshot omitted animation sources and invalidated its tablet test. Discarded as acceptance evidence.
- Earlier equip pilots exposed fixture transfer/preview timing problems. Accepted insertion requests do not prove completed physical insertion. The current fixture verifies actual inventory ownership and actual hand-slot attach/detach for completed equip cycles.
- Earlier loaded bag tests exposed real callback stack overflow and immediate falling-state cancellation; failed logs remain preserved.
- No claim of 200 successful gameplay cycles for every setting or every medical procedure is made.
