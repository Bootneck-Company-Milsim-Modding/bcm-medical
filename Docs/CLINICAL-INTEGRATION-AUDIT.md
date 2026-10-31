# BCM clinical integration audit — 27 October 2026

This audit describes the original BCM implementation and its game abstractions. It is not a real clinical prediction model. No KAT/ACE implementation is incorporated by this work.

## Findings corrected in this pass

- Medication eligibility previously checked supply and repeat intervals without enforcing pulse, pressure, respiratory rate or oxygenation. Production authority now checks configurable vital limits before starting and during treatment. Rejections identify the relevant reason.
- Pulse previously rose as blood fell, without a late failure phase or a pumping-efficiency cost at extreme rates. Compensation now includes perceived pain and stimulant effects; profound hypovolaemia/hypoxia can reduce rate. Inadequate filling/output changes pressure and tissue oxygen delivery.
- Persistent critical pressure or a compromised rate extreme can lead to arrest. High pulse alone is not an automatic kill. Haemorrhagic/obstructive failure is nonshockable; sustained compromised extreme tachycardia can become pulseless VT.
- A single ordinary morphine dose previously scaled oxygen saturation directly by its respiratory modifier, causing excessive hypoxia. Oxygen now depends on whether ventilation remains adequate. Stacked depression can still produce hypoxic arrest, and naloxone can wear off before morphine.
- Fluid procedures now enforce the pressure ceiling and actual reserve deficit on the authority. The finite delivery adapter repeats its safeguard and caps delivery at the missing reserve.
- Native passive blood regeneration is now a GM option, disabled by default. Native saline is terminated before further delivery if its eligibility is lost.
- TXA still does not close external wounds. It slows dressing wear and provides a bounded reduction in internal loss while active. Surgery remains necessary for an unrepaired source; collected pleural blood remains separate.

## KAT comparison

Research snapshot: KAM `dev-Tomcat`, commit `e164ae2e6d146897f93364b4c5083e8e3b17655d`, read through the GitHub connector on 27 October 2026.

KAT connects rate to volume, perceived pain and medication adjustments, derives pressure from output/resistance, and distinguishes arrest rhythms. Its advanced TXA function can periodically bandage wounds under configuration-dependent conditions. BCM uses its own smaller deterministic model, with explicit external control and clot support instead of automatic wound closure. The BCM thresholds below are tunable design choices, not copied KAT numbers or real prescribing thresholds.

Sources: [heart-rate calculation](https://github.com/KAT-Advanced-Medical/KAM/blob/e164ae2e6d146897f93364b4c5083e8e3b17655d/addons/vitals/functions/fnc_handleCardiacFunction.sqf), [pressure](https://github.com/KAT-Advanced-Medical/KAM/blob/e164ae2e6d146897f93364b4c5083e8e3b17655d/addons/circulation/functions/fnc_getBloodPressure.sqf), [rhythms](https://github.com/KAT-Advanced-Medical/KAM/blob/e164ae2e6d146897f93364b4c5083e8e3b17655d/addons/circulation/functions/fnc_handleCardiacArrest.sqf), [TXA behaviour](https://github.com/KAT-Advanced-Medical/KAM/blob/e164ae2e6d146897f93364b4c5083e8e3b17655d/addons/pharma/functions/fnc_treatmentAdvanced_TXALocal.sqf).

## Medical basis and deliberate simplifications

Resuscitation guidance prioritises compressions, effective ventilation, indicated defibrillation and treatment of reversible causes. It distinguishes an unstable arrhythmia from an isolated heart-rate number. BCM therefore requires sustained compromise and keeps shock eligibility separate from pulse rate. [Resuscitation Council UK ALS 2025](https://www.resus.org.uk/professional-library/2025-resuscitation-guidelines/adult-advanced-life-support-guidelines).

Opioid respiratory depression and hypotension, especially with reduced volume, are supported adverse effects; opioid antagonism treats opioid effects rather than the injury itself. BCM models these links but omits actual milligram doses and clinical variability. [Morphine prescribing information](https://dailymed.nlm.nih.gov/dailymed/fda/fdaDrugXsl.cfm?setid=6023606c-150c-40c5-9175-598efe2a56c3&type=display).

TXA supports clot persistence. Rapid IV administration can cause hypotension, but an accurate administration-rate complication model needs dose/concentration/rate tracking. BCM does not currently model that or add an arbitrary pressure penalty to every dose. [Manufacturer information](https://labeling.pfizer.com/showlabeling.aspx?id=556), [NHS overview](https://www.nhs.uk/medicines/tranexamic-acid/about-tranexamic-acid/).

Trauma guidance calls for reassessment and resuscitation endpoints. The configurable BCM pressure stop is a game safeguard, not a clinical transfusion rule. [Joint Trauma System prolonged casualty care](https://jts.health.mil/assets/docs/cpgs/Prolonged_Casualty_Care_Guidelines_21_Dec_2021_ID91.pdf).

## Typed fluids and tissue delivery

BCM tracks normalised volume, red-cell reserve and clotting support independently. Saline increases volume without red cells or clotting factors; plasma also supports clotting; universal game whole blood supplies all three. Haemorrhage removes components proportionally. Non-blood water redistribution reduces native circulating volume without adding a wound or discarding red cells. Dilution affects remaining internal flow and dressing wear.

Tissue oxygen delivery combines perfusion, saturation and available red-cell concentration. This permits inadequate tissue delivery despite a normal SpO2. Compensation and arrest/ROSC checks use these linked quantities. The blood-loss examination explicitly labels reserve percentages as game estimates, not measured haemoglobin.

The underlying distinction between red-cell oxygen transport and plasma clotting proteins follows blood-component descriptions: [NHS Blood and Transplant](https://www.blood.co.uk/why-give-blood/how-blood-is-used/blood-components/), [Canadian Blood Services](https://professionaleducation.blood.ca/en/transfusion/publications/blood-basics). The retained fractions, redistribution scale, clotting multipliers and critical/recovery delivery cutoffs are **BCM tunable game abstractions**, not clinical predictions or prescribing rules. There is no simulated full blood-count or detailed coagulation chemistry.

## Release gaps that remain open

- Original typed-fluid composition now separates circulating volume, red-cell reserve and clotting support. Model acceptance passes; full native and multiplayer acceptance is tracked in `VALIDATION.md`. Fluid overload, continuous administration, laboratory chemistry and blood-type matching remain unimplemented.
- Native/legacy saline and morphine adapters now share BCM guards; the native effect test exercises allowed saline delivery and termination after access loss. Complete vanilla item animation/consumption, all third-party routes and client prediction still require broader acceptance. Native adapter classes now retain BaseContainerProps config reflection; the fixture resolves its guarded effect from the actual stock morphine prefab. Earlier missing-effect fixtures remain historical failures, not current acceptance.
- Blood pressure currently exposes systolic pressure, not a separate validated diastolic/MAP model. SpO2 and respiratory-rate models remain simplified.
- Further medication types beyond the implemented six, continuous oxygen/source presentation polish, explicit treatment indication/history UI, additional native and multiplayer deterioration/recovery scenarios, and full patient/world/account persistence remain release work.
- Numerical cutoffs are server-configurable game defaults. They must not be presented as real clinical instructions.
- Two-player stretcher carrying and its grip art are shelved at the user's request. Defaults are disabled; existing saved settings are preserved.

## Native treatment presentation

BCM effects retain native consumable type setup for equipment versus injectors. The presentation effect cannot heal or delete supplies itself; the authoritative reserved procedure owns clinical revalidation, elapsed time and final consumption. Loss of access during a real saline animation cancels before delivery and retains the exact bag. Native stock-effect config reflection is tested separately from complete stock inventory/UI consumption.

The multiplayer fixture must transfer native character replication ownership as well as assigning a controlled entity. Observer characters must remain outside the medic's treatment trace. These fixture issues were corrected rather than removing native readiness or obstruction checks.

## Validation

The current production domain suite includes complete multiple-bleed deterioration/control, internal repair, restoration of reserve, ordinary opioid ventilation, unsafe medication boundaries, TXA support, stimulant instability, shock/ROSC limits and schema migration. Native inventory/animation and multiplayer acceptance are tracked separately in the project-root `VALIDATION.md`; domain assertions alone are not proof of all network paths.
## Supplemental oxygen and expanded analgesia

The [oxygen/medication implementation note](OXYGEN-AND-EXPANDED-MEDICATIONS.md) records the primary research and game abstractions. Supplemental oxygen raises the target supported by remaining lung capacity; it cannot bypass obstruction, apnoea or absent circulation and never replaces red cells. Paracetamol has independent non-opioid relief and a conscious swallowing guard. Fentanyl has separate analgesic and longer respiratory effects; residual exposure remains recorded, mixed opioid risk accumulates and naloxone recognises either opioid. These use abstract finite supplies, not real drug doses. Liver toxicity, tolerance and full pharmacokinetics are unimplemented.
