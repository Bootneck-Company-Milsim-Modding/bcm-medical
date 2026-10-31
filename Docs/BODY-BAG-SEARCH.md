# Body-bag search sessions

BCM Medical keeps native inventory responsible for every equipment transaction.
Its search sessions coordinate that inventory with burial, return-home, dragging
and cargo operations. Searching does not consume a bag, move its contents or
change the associated death record.

The registered Search action requests authority before opening inventory. Bags
opened through ordinary native vicinity or nested-container navigation are also
tracked. Several players may search the same bag; closing one view does not
release another player's session.

Authority requires the original connected controller, its current living,
conscious character, a ready world bag, no dragging/attachment and a distance of
at most three metres. It repeats those checks on renewal. A session records the
exact controller, character and bag, a unique server generation and its client
request number. Reopening creates a fresh generation. Old closes, renewals,
acquisition replies and renewal rejections cannot affect that newer view or a
different controller's session.

The client renews only while native inventory actually shows the bag, including
the loot panel's navigation and separate or nested container windows. Sessions
have a 3,500 ms deadline and authority checks them every 500 ms. Closing inventory,
leaving reach, changing controlled character, becoming ineligible or losing the
client releases the reservation. Only bag views are closed when eligibility is
lost. A short pending-open grace period permits native inventory initialization;
failure to open releases the reservation without changing equipment.

An active session prevents burial, return-home, drag start and cargo loading.
Native direct cargo insertion checks the same restriction. The replicated busy
flag drives client eligibility; authority checks its own sessions. Busy-state
changes advance the bag's transport revision, invalidating requests captured
before a newer search. The independent native removal guard still prevents
equipment extraction if an old inventory view outlives a disposal reservation.

## Native acceptance

- `bodybag-vehicle-20261028-223544-646`: 31 authority checks and all six owner/peer
  cargo-phase checks pass. Two actual native inventory views, independent close,
  reopen, stale/cross-controller rejection, range release and return-home.
- `bodybag-vehicle-20261028-223902-291`: 32 authority checks, SourceUnchanged=true.
  Includes ordinary native inventory reopening, stale reply rejection and an
  abrupt stop of the exact recorded observer client. The native server observes
  disconnection and the other search remains active. Three owner and two
  pre-disconnect peer cargo phases pass; the stopped peer cannot certify phase 3.
- `burial-network-20261028-224315-807`: 755 assertions, SourceUnchanged=true. The
  actual owner sends the production burial RPC while native inventory is open;
  authority rejects it despite an otherwise valid held tool, range and natural
  ground. After inventory closes, full digging, exact disposal, private owner
  notice and owner/proxy/post-burial-joiner memorial inspection pass.

- `bodybag-vehicle-20261028-225839-063`: 36 authority checks,
  SourceUnchanged=true. Adds proper native possession/return, fresh search after
  restoration and stale transport-revision rejection to the disconnect variant.
- `bodybag-vehicle-20261028-230350-290`: 36 authority checks and all six cargo
  phases, SourceUnchanged=true. Normal close retains the observer's final privacy
  check and independently establishes valid cargo prerequisites before rejecting
  the old revision. Native inventory permissions remain enabled throughout.

These native API checks do not replace human
keyboard/gamepad acceptance, Internet/load testing or final combined-source
clinical and aircraft/vehicle regressions.

Run the local dedicated diagnostics with matching game/server versions:

```powershell
& .\Tools\Validate-BodyBagVehicles.ps1 -ServerDirectory $server -ReturnHome -SearchSessions
& .\Tools\Validate-BodyBagVehicles.ps1 -ServerDirectory $server -ReturnHome -SearchSessions -SearchDisconnect
& .\Tools\Validate-BurialNetwork.ps1 -ServerDirectory $server -SearchSessions
```

The earlier RequestUserLock experiments are not acceptance: that API refused the
test setups and an open inventory did not supply the required authority lock.
Current sessions track real native views explicitly instead. Historical failed
fixtures remain under `.checks/` for diagnosis.
