# Optional redistributed-fluid burden

Overload is **disabled by default**. This normalized gameplay model records water redistributed out of circulating typed fluids as a separate burden. It does not create tension or pleural blood, and chest needles/drains do not remove it.

With the model and typed composition enabled, burden above the onset fraction progressively reduces the remaining structural lung capacity. The existing oxygenation, respiratory response, oxygen-delivery and deterioration model handles the consequences. Portable oxygen and ventilation support can help breathing while the burden remains.

Stored burden clears gradually during active simulation, scaled by spontaneous perfusion. Poor perfusion slows clearance; arrest and offline time award none. Clearance removes burden only: it does not refill native circulating blood, add red cells or clotting, or repair injuries. Stopping the model suppresses new accumulation and effects; existing burden continues to clear during active time. Re-enabling can therefore reveal burden that has not yet cleared.

With fluid safety and the water-stop policy enabled, saline/plasma are withheld at the configured severity. This applies to the native saline gate, held procedures, independent-line admission and actual native volume adapter. Blocked finite bags retain their contents. Whole blood continues to follow the existing access, volume and pressure checks. The medical panel updates the availability of water-loading actions; a private fluid assessment can report elevated game burden.

Seven new controls are under Circulation in the same single BCM Medical tab:

| Control | Default |
| --- | --- |
| Model redistributed fluid overload | Off |
| Burden onset fraction | 0.15 |
| Severe burden fraction | 0.40 |
| Clearance timescale | 1800 seconds |
| Lung fraction at severe burden | 0.65 |
| Withhold additional water-loading fluids | On |
| Water-loading stop severity | 0.50 |

Onset must remain below the severe fraction. A zero stop severity starts withholding only after burden is above onset; it never blocks a burden-free patient or a disabled model. At full spontaneous perfusion, about63% of existing burden clears over one timescale. Fractions and timings are configurable game values.

Current development has270 controls/schema33. Valid profiles through schema32 preserve their values and receive conservative defaults. A separate optional schema1 saved-world object records nonzero burden alongside existing clinical, oxygen and line state. Older worlds without the object start with zero burden. Native FullHeal resets burden and its water gate without refilling finite supplies.

Native base/schema/deterministic acceptance passes634/0 in211726-007. Actual finite saline generation, held/line water rejection, original whole-blood flow, retained oxygen and native world save/restart pass651/642 unchanged in fluid-burden-20261029-212421-106. This includes current-process bounds against offline clearance, continued active perfusion clearance and original finite-source accounting. Exhaustion uses the pre-existing0.05mL usability threshold; earlier strict-zero/composite failures remain in historical logs. Multiplayer/UI policy subsequently passed as recorded below; combined33-gate regression subsequently passed unchanged as recorded below. This feature is not yet a release certificate.

Subsequent native owner/proxy/active-session JIP acceptance passed640/0 unchanged in fluid-burden-network-20261029-213559-673. Live policy toggles and supply counts update without manually rebuilding the menu. A copied actual pre-burden world passed639/0 unchanged in fluid-burden-legacy-world-20261029-214022-749, with the original historical profile unchanged. That profile contained no settings file. Combined33-gate regression214430-980 passed all33 gates with sourceUnchanged=true at22:25:26 UTC on29 October2026.



