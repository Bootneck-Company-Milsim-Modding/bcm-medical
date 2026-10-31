# Native stock consumable commit

BCM's native stock morphine and saline effect adapters recheck clinical policy when their effect commits. Native1.8.0.13 SCR_ConsumableItemComponent subsequently deletes a delete-on-use item even if that effect refused. Actual stock-morphine animation reproduced this after a live GM medication-role revocation: native begin and successful completion occurred, no dose was allowed, but the original injector disappeared.

BCMM_NativeConsumableCommit.c now guards that item-commit boundary for the two actual stock effect classes. Rejection clears native gadget/invoker state, unlocks and retains the original item, restores its packaged model when applicable and clears the old target. Ordinary allowed use and unrelated consumables retain the native path. BCM presentation items can share native animation categories, so the guard uses effect classes rather than animation-category labels.

Evidence on29 October2026:

- Original reproduction native-consumable-development/run-20261029-222848-840:642 checks, one failure; began=1, completed=1, source=0, owned=0 after revocation.
- Initial fix223009-318:642/0 with retained original item.
- Expanded real subsequent-use223205-101 and promoted20261029-223731-487:649/0. Denied commit retains the exact injector with zero doses; permission restored, the same injector completes another native use, disappears once and applies exactly one dose.
- Private native-stock-network-20261029-224538-504 and promoted225032-517:643/0, source unchanged. Actual possessing owner plays both stock animations and reports genuine native callbacks. Authority observes using-item state, revokes policy during use, retains original source/no dose, then allows the same source to commit one dose. Proxy and post-revocation joiner observe phase/settings changes. This does not claim remote inventory inspection or backend authentication.
- Clinical safety20261029-223857-753:668/0 in its supported local-client mode. The preceding223823-525 server launch failed in fixture setup because that existing fixture needs a local player controller; it is not a passing runtime or evidence of a new gameplay defect.

The36-gate combined run230712-443 passed unchanged at23:49UTC on29 October, including native stock commit, dedicated stock owner networking and passenger/drip restoration. Subsequent full42 run003550-595 also passed unchanged. Later independent stock-source lifetime, format and reapplication changes expand the full runner to48 gates; combined030939-332 passes unchanged at04:03:42UTC on30 October. Earlier packets do not certify those later changes; refreshed422-entry packet031340-185 is undergoing packed-only runtime acceptance. Settings remain270/schema33; the commit fix adds no setting or separate menu. Current evidence is in [native saline](NATIVE-STOCK-SALINE.md) and [the work queue](BACKGROUND-WORK-QUEUE.md).

Broader stock-item UI interaction, native saline finite-source/persistence coverage, latency/load, other clinical compatibility and final release work remain on the roadmap. Actual scripted native activation/callback tests are distinct from human inventory-button play testing.

See [native stock saline continuity](NATIVE-STOCK-SALINE.md) for original-source world restoration, source-loss cancellation, FullHeal and strict state validation.
