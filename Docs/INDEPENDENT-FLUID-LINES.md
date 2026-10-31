# Independent finite fluid drips

The Fluids tab now offers separate **Connect whole blood drip**, **Connect saline drip** and **Connect plasma drip** actions. Existing held infusions keep their current behavior. Select the limb with the patient's actual vascular access. Use **More treatments** for actions on the second page.

The operator needs the original usable bag in their own inventory. The patient needs enough native inventory capacity. After the timed connection, that same physical bag moves into patient inventory and follows the patient. The operator's hands and procedure lock are released, and flow continues after the operator leaves treatment range. The panel shows the connected bag's remaining volume.

**Disconnect fluid line** needs no spare bag. Stay in treatment reach for the configured removal time. The original partially used bag remains in patient inventory. To reconnect it through another operator, move it using normal native inventory handling into that operator's inventory. A patient with access may also connect their own bag to themselves.

Flow debits only the volume accepted by the native patient adapter. Loss of access, moving/deleting the source, a patient reset/death, disabling lines, pressure/full-reserve safety checks or depletion stop flow. A disconnected or interrupted bag never refills. Connected and transferring sources are reserved against reuse and recursive native arsenal refunds. An exhausted drip bag remains a physical empty bag.

The three saved controls are under Circulation in the existing single BCM Medical tab:

| Control | Default |
| --- | --- |
| Allow independent fluid drips | On |
| Fluid line connection time | 8 seconds |
| Fluid line disconnection time | 3 seconds |

Flow uses the existing **Fluid infusion duration** as the full-bag interval. Medic-duration scaling applies to connection/removal; unattended flow uses the configured full-bag interval. Lines were introduced in schema32 with263 controls; current development is schema33 with270. Valid older profiles retain their existing settings and receive appended defaults; removed body-bag respawn/menu controls stay removed.

The native saved-world extension records the exact source UUID and access region. It restores only from the original bag in the restored patient's inventory and awards no offline delivery. Existing clinical schema1 remains compatible with older worlds through a separate optional line object.

Native evidence: inventory/finite flow630/0 in202212-006; actual active-world save/restart614/613 with unchanged sources in fluid-line-20261029-202308-576; actual owning-client menu/RPC, medic separation, owner/proxy/active-session late joiner and zero-spare-bag disconnection612/0 unchanged in fluid-line-network-20261029-202404-418. Expanded native boundaries637/0, typed/parallel/stale-removal/source-move625/0 and missing-source callback/FullHeal618/612 subsequently passed. The263/schema32 snapshot passed all30 combined gates unchanged in clinical-acceptance-20261029-203856-546. Current270/schema33 development has a separate33-gate regression in progress. This evidence does not certify final contact/line art or whole-mod completion.

