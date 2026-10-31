# Stabilised blood-component recovery

Game Masters can enable gradual recovery of red-cell and clotting reserves under the existing **Circulation** heading. It is disabled by default. It does not refill missing circulating blood volume, treat shock, stop bleeding, repair injuries, remove pooled blood or revive a patient. Native passive blood-volume regeneration remains a separate control and remains disabled by default.

Recovery requires spontaneous circulation and breathing, adequate oxygenation, no tension, enough circulating volume and systolic pressure, and no ongoing external or unrepaired internal bleeding source. A required stationary facility must remain within the configured facility range. Leaving the facility or falling below the safety conditions pauses recovery. Game Masters may explicitly permit field recovery instead.

The six controls are:

| Control | Default |
| --- | --- |
| Allow stabilised blood-component recovery | Off |
| Red-cell recovery timescale | 3600 seconds |
| Clotting recovery timescale | 900 seconds |
| Minimum circulating volume | 80% |
| Minimum systolic pressure | 90 |
| Require stationary medical facility | On |

After one timescale of eligible game time, about63% of the existing component deficit has recovered. Larger values slow recovery. These are configurable gameplay timings. Eligibility does not award time spent outside care or offline; native patient restoration resets the physiology tick clock. Existing native blood-volume infusion limits remain unchanged.

Component recovery was introduced in schema31 with260 controls. Current development is schema33 with270 controls under the same one BCM Medical tab and fifteen headings. Valid older profiles retain their values and receive conservative appended defaults. The obsolete body-bag respawn/menu keys never become active controls.

## Evidence

`.checks/component-recovery-20261029-191124-440/result.json` passes with unchanged sources:615 runtime assertions and600 independent native restart assertions, zero failures. Actual character ticks, real facility removal, explicit field policy, pressure stop and disable controls, unchanged partial native blood volume, unused finite supplies and saved component values/policy are covered. Deterministic checks include tick partitioning, invalid elapsed/state rejection, bleeding/arrest/oxygen/tension boundaries and schema30/31 validation.

The earlier direct runtime190817 passed611 assertions; its first restart correctly failed the settings expectation because the fixture-only setter intentionally skipped disk writes. The accepted rerun explicitly uses the real settings store. That fixture correction did not relax production authority.

The full Clinical Acceptance runner includes this native recovery/restart gate. The current full runner has48 gates; its active snapshot is recorded in [the work queue](BACKGROUND-WORK-QUEUE.md). Whole-mod completion, live authenticated reconnect and the remaining clinical/art/release work are separate requirements.
