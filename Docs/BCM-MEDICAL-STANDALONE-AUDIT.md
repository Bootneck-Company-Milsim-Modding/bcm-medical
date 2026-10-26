# BCM Medical standalone audit and asset requirements

26 October 2026. Target: one BCM Medical addon requiring only Arma Reforger. ACE, KAT, RespawnLimiter, Keep Weapons and Improved Blood Effect are not to become dependencies or bundled implementations. Existing BCM functionality is the behavioural specification; new medical and transport implementations must have BCM ownership and provenance.

## Audit status and evidence

The installed packages were unpacked into the task reference directory, separate from gameplay. The five medical packages contain 202 script files and 441 packaged files in total. Core infrastructure, Carrying and Captives add 151 scripts. Improved Blood Effect adds nine. The accompanying installed-resource inventory records every retained file and SHA256. Packaged files do not include all editable artwork sources. The earlier ACE source atlas, exact settings/drug reference and upstream source inventory remain useful companion documents.

This is an architecture, dependency and procurement audit with a complete packaged-file inventory. It is not a claim that every line has received individual semantic review, every mesh has been visually inspected, or every branch has been tested. Source inspection covered the main physiology/death paths through the previous audit, and current UI, AI, transport, animation and BCM integration paths. Per-file indexing is not runtime verification. Full line-level review and asset visual QA remain tracked work before feature parity can be accepted.

Installed Medical Core, Hitzones, Circulation, Breathing, AI and Carrying are 1.5.36; Core infrastructure is 1.5.39. The retained upstream medical reference is commit b0b5d4db507e557d7b94b8b415cee880d8a7486a. Source and packaged hashes must be compared before claiming exact source/binary correspondence. Current online upstream can differ from these packages.

## What the installed systems do

| System | Implementation and interactions | BCM replacement requirement |
| --- | --- | --- |
| Damage and anatomy | Hitzones shifts lethality from aggregate health to vital physical zones and organs. Kinetic/melee trajectories can intersect organ spheres. Heart and femoral injuries have distinct consequences. | Own wound records, anatomy identifiers, penetration mapping, damage adapter and final death policy. Distinguish physical damage, blood reserve, resilience and brain injury. |
| Second chance | Server-side fatal-injury handling groups wounds, records protected zones, incapacitates and can minimally restore destroyed zones. Later fatal hits and brain/blood destruction have additional kill paths. | Centralise terminal decisions. Preserve BCM protection for player-used bodies and CIV AI without accidental wake-ups or recursive Kill calls. |
| Bleeding | Physical wounds feed one aggregate blood-loss effect. Tourniquets scale selected limb groups. Circulation limits effective flow using cardiac output and a gravity floor. | Persistent individual wounds, compression/packing/bandage state, arterial/venous differences, junctional limitations and server integration using elapsed time. |
| Blood and fluids | Advanced blood volume is a reserve plus a 3000 ml baseline. Zero blood-hit-zone health does not mean zero physical blood. Saline delivers volume over time. | Explicit units and baseline. Keep the existing 60-second reserve-depletion grace rule separate from physiology. Only delivered qualifying fluid cancels that rule; starting treatment does not. |
| Consciousness | Resilience damage/regeneration combines with stable circulation, brain condition and airway gates. Pain is separate. | One combined consciousness decision; pulse restoration must not instantly heal wounds or restore consciousness. |
| Circulation | Patient-specific stable, unstable, critical, arrest and resuscitation states. HR and stroke volume produce cardiac output; vascular resistance determines pressure. Threshold tables select transitions. | Explicit patient context, validated default thresholds, shock causes, perfusion and rhythm. No shared mutable casualty state. |
| CPR | Patient-specific performer helper, positioning and cancellation. CPR temporarily supplies reduced circulation; periodic success rolls depend on volume/oxygenation and medication. | Own treatment session and cancellation lifecycle. Separate compression-generated flow, spontaneous circulation and electrical rhythm. |
| Neurology | Hypoxic brain damage and slow recovery can prevent waking or cause death independently of blood. | Own hypoxic injury and recovery model; configurable BCM survival protection remains a deliberate gameplay exception. |
| Airway | Tongue obstruction and vomit occlusion are separate. Recovery position, chin lift, clearing and a laryngeal tube address particular conditions. | Airway patency, obstruction, aspiration risk, suction and placed-device state. BVM cannot bypass obstruction. |
| Breathing | Lung-volume loss, pneumothorax/tension, oxygen/CO2 stores, ventilation and perfusion interact. Chest seals and NCD have distinct effects. | Left/right chest injury state; external wound sealing separate from tension relief, blood in chest and gas exchange. |
| Medication | Patient-specific doses and concentration/effect configurations. Epinephrine, metoprolol, morphine, naloxone and phenylephrine affect circulation/pain; ammonium carbonate has separate wake behaviour. | Own data-driven formulary, dose history, routes, concentration/effect curves and contraindications. Do not copy numeric tables as a substitute for designing BCM balance. |
| Medical kit and facilities | Configurable field repair cap and per-treatment repair. Facilities can have different rules. | Distinguish stabilisation from definitive repair, medic qualifications, equipment and facility requirements. |
| AI | Changed medic behaviour tree, consumable selection/counting and support-station interaction. Basic decisions prioritise bandage, saline, medical kit and basic epinephrine. | New triage/action planner using the same authoritative treatment API as players. Existing AI is not a complete advanced-airway/chest/arrest clinician. |
| Carrying | Character eligibility, stance checks, helper compartments and generic carried-entity infrastructure. Casualty carry/drag rejects dead or active-ragdoll patients. | Own replicated transport session, casualty pose, movement restrictions, obstruction checks, safe placement, disconnect cleanup and vehicle transitions. |
| Captives | Surrender/restraint actions, item use and tied/surrender poses. | Outside medical MVP unless needed for restrained casualties. Do not silently import the captive ecosystem. |
| Blood visuals | Separate mod manages trails, pools, wall splashes, footprints and dynamic decals with cleanup/configuration. | Optional BCM presentation layer. Visual decals must never become the authoritative bleeding simulation. |

The retained ACE-MEDICAL-AUDIT.md contains the detailed equations, thresholds and interactions for this installed release. Its old CPR incident section was superseded by CPR-CONFLICT-AUDIT.md and DEATH-RESTART-INVESTIGATION.md; do not treat its early unresolved incident narrative as the latest diagnosis.

## Menus, settings and networking

ACE extends the native inventory/body-zone and inspect-casualty UI; it does not provide the complete KAT-style treatment dashboard imagined for BCM. Core adds fracture information, extensible tooltip flags and pain presentation. Circulation adds blood/vitals notifications and medication history; Breathing adds pneumothorax indicators and state-change listeners. GM tooltip details expose health, resilience and oxygen information. Treatments are additional user actions and consumables, not just buttons in one screen.

Settings are typed mod settings loaded through ACE infrastructure, with mission-header overrides and prefab inheritance. Medical kit limits, bleeding and regeneration multipliers, second-chance rules, organ enablement, physiology thresholds, airway/chest probabilities and pharmacology configurations must be inventoried separately from menu labels. The earlier ACE-SETTINGS-AND-DRUGS.md retains the declarations.

BCM needs a native treatment interface with patient selection, body-zone selection, examinations, wounds, airway/breathing, circulation, medications, treatment progress, triage and a shared event log. Observed measurements need timestamps and device requirements; exact underlying pathology should not automatically be revealed to everyone. Controller navigation and keyboard/mouse interaction both require testing. A GM category must retain every existing BCM lives, suppression and focus control.

Simulation belongs on the server. Client requests must resolve patient and actor identities, recheck range, life state, inventory, qualification and current treatment lock, then consume inventory only when the authoritative treatment commits. Completion/cancellation must be idempotent. JIP receives current state, not replayed treatment side effects. Medical records, animation state and inventory state require separate cleanup on death, deletion, disconnect and mission transition.

## KAT research and useful design targets

[KAT's official repository](https://github.com/KAT-Advanced-Medical/KAM) describes airway devices and suction, oxygen monitoring and chest pathology, cardiac rhythms/AEDs, blood grouping, surgery/fractures and configurable medical expertise. Its [official Workshop description](https://steamcommunity.com/sharedfiles/filedetails/?id=2020940806) also describes IV/IO flow, additional medications, coagulation, surgery tools and evacuation equipment. These are design references, not evidence that Reforger ACE already supports them. Arma 3 SQF, CBA callbacks, P3D assets and animation systems cannot simply run in Enfusion.

BCM should initially target haemorrhage control, airway, chest injury, perfusion, oxygenation, rhythm-aware resuscitation and evacuation. Advanced surgery, blood matching, hypothermia, burns, ocular injuries and chemical exposure should be later independently testable features. A long item list without interacting physiology would not meet the intended realism.

An AED should analyse rhythm and advise shock only for a shockable rhythm; asystole and PEA should not become shockable merely because a casualty is unconscious. CPR supports circulation while causes are treated; the AED is not a CPR machine or universal revive item. This design distinction follows the [Resuscitation Council UK adult ALS algorithm](https://www.resus.org.uk/sites/default/files/2025-10/Adult%20ALS%20algorithm%202025.pdf). It is a game-model design reference, not a treatment guide. Haemorrhagic/obstructive arrests need their causes represented, and monitor rhythm is not proof of perfusion.

## Licence and ownership findings

| Material | Evidence | Decision for BCM |
| --- | --- | --- |
| ACE scripts/infrastructure | Upstream LICENSE and module notices specify GPL v2 or later. | Study behaviour; write new BCM implementations. No ACE code bundled. Reuse would require distribution/source/notice compliance, not a purchase. Renaming copied code does not make it original. |
| ACE models, textures, audio and animations | Project licence is present; packaged resources do not by themselves prove every contribution's provenance or supply editable sources. | Inventory as existing reference assets, not cleared BCM assets. Use original/base-game substitutes; inspect exact asset notices and upstream source before any later reuse decision. |
| KAT | Repository GPLv3; Workshop description also states APL-SA. Per-file/source-era notices can differ. | No blanket clearance of KAT artwork/audio. No direct code/asset import into BCM. |
| RespawnLimiter-derived BCM lives | NOTICE-RespawnLimiter.md records APL adaptation. | Rewrite the limiter and spectator lifecycle against the existing BCM behaviour contract. Preserve notices on the legacy files; do not simply remove them or claim renamed code is new. |
| 420 Keep Weapons | Installed metadata supplies a restrictive custom licence including extraction/repackaging restrictions. | Excluded from the new project. The task-local unpacked copy was removed after checking the metadata; the installed original is unchanged. Implement weapon retention from native engine behaviour and BCM requirements. |
| Improved Blood Effect | Installed metadata declares APL. | No code/assets copied for the original BCM ecosystem. Create optional BCM effects. |
| BCM Body Bags | Editable FBX, imported models, collision model, materials and source texture maps are present. | Retain BCM asset and functionality. No missing mesh blocks migration. Purchase receipt/author grant is still needed for a complete asset provenance record; possession alone is not a new licence grant. |
| Native Reforger resources | Referenced through base-game resource paths. | Use through the game dependency. Do not export/relicense game assets as BCM-owned content. |

Primary licence references: [ACE licence](https://github.com/acemod/ACE-Anvil/blob/master/LICENSE), [KAT licence](https://github.com/KAT-Advanced-Medical/KAM/blob/master/LICENSE), [APL](https://www.bohemia.net/en/licenses/arma-public-license). No purchase is authorised or necessary to start programming. Asset-store standard licences must be checked for Workshop redistribution; royalty-free does not necessarily permit raw/editable-source redistribution.

## Assets to obtain or create

Do not buy needles or chest seals yet. Simple generic equipment can be modelled in Blender; icons, diagnostic screens, labels and tones can be authored for BCM. Spend first on high-quality device models or specialist character animation if original prototypes are inadequate. No individual store listing or price has been vetted.

| Priority | Asset family | What is needed | Existing/base-game position | Recommended action |
| --- | --- | --- | --- | --- |
| First | AED or monitor-defibrillator | Closed/carry and deployed device, opening lid, pads, leads, connector, screen area; generic unbranded design | No AED model identified in the supplied medical package set | Create prototype; commission/buy a game-ready model only if higher detail is wanted |
| First | BVM and oxygen | Bag, mask, valve, reservoir, tubing; oxygen cylinder/regulator for finite supply | ACE oxygen-mask asset exists; this does not establish an owned BVM kit | Create generic BCM equipment; animate bag compression |
| First | Airway devices | OPA/NPA, supraglottic airway, packaging; suction unit and catheter | ACE King LT and oxygen mask exist as reference | Simple tubes in Blender; suction device optional commissioned asset |
| First | Chest treatment | Seal package/applied dressing; NCD casing, catheter and hose | All these ACE model forms are present | Build original low-poly BCM models and original labels |
| First | Bleeding control | Dressing, packing gauze, pressure bandage, tourniquet in packed/applied forms | Native dressing/tourniquet resources may support initial references | Verify base-game resource closure; create custom variants where needed |
| First | IV/IO and fluids | Cannula, port, tubing/flow regulator, IV pouch; optional IO driver/needle | Native saline can support a first prototype; full kit is additional | Create IV kit; defer detailed IO driver until route simulation exists |
| First | Diagnostics | Pulse oximeter, BP cuff/manometer, stethoscope, thermometer | No complete original BCM set found | Simple prototypes; purchase only for desired close-up fidelity |
| First | Medication | Generic injector, syringe, vial/ampoule with original labels | Native morphine injector can be referenced; drug-specific ACE assets are not required | Share original mesh families with distinct data/labels; do not buy one mesh per drug |
| First | Splint and triage | Packed/applied limb splints, triage card/tags, thermal blanket | Requires native-resource verification or original assets | Create in Blender; original icons and readable UI |
| Already present | Body bags | Empty and filled bag; collision; textures; corpse source | Full local BCM sources and runtime resources present | Retain; record original purchase/author permission |
| Later | Evacuation | Folded/deployed stretcher, handles, straps, optional litter mounts | No complete BCM stretcher system identified | Commission if required; two-carrier animation is the larger integration task |
| Later | Thoracic surgery | Chest drain set, tube, collection bag; instruments | Beyond initial chest seal/NCD | Build only when advanced treatment rules exist |
| Later | Surgery/burns | Surgical pouch, tools, sutures, irrigation, burn dressings | No need to block initial gameplay | Share tool meshes; defer specialist artwork |
| Later | Blood logistics | Collection/transfusion bags and labels, storage/cooler | Adds logistics and compatibility rules | Create original variants after blood-product modelling |

For any purchased/commissioned asset request: editable .blend/FBX, metre scale, clean UVs, PBR maps, appropriate LODs and collision, separate moving pieces/pivots, no unwanted trademarks, documented authorship, and explicit permission to redistribute it inside a Reforger Workshop addon. Source redistribution rights matter if any chosen distribution licence requires source. A rendered product image is not a usable game model. Device cables need an explicit rig or procedural geometry plan.

## Animation requirements

| Action | Current evidence | Standalone BCM approach |
| --- | --- | --- |
| CPR | ACE's CPR instance references native p_cro_revive_CPR_in/loop/out.anm | Reference native clips through a new BCM animation/session integration. No need to buy compression clips initially. |
| Basic bandage/injection | Native treatment machinery exists | Reuse native capabilities where the exact item/pose fits; verify hand alignment and cancellation. |
| Casualty carry/drag | ACE packages contain custom carried/dragged in/loop/out clips and helper instances | Author BCM clips and controller/session integration. Need pickup, sustained pose, locomotion interaction, release and interruption variants. |
| Head tilt/recovery | ACE has custom head-tilt source/clip; recovery poses require resource inspection | Author missing poses/transitions against native unconscious states. |
| Chest seal/NCD | ACE includes custom treatment and finger/item clips | New BCM apply/remove animations; NCD includes a coordinated prop movement. |
| Airway insertion/suction | ACE has King LT-specific clips; full planned device set is broader | New hand/device alignment and insertion/removal motions. |
| BVM ventilation | Oxygen-mask clips are not proof of a complete BVM squeeze cycle | New loop, entry/exit and prop deformation. |
| AED | No supplied full AED animation set identified | Kneel, open device, place pads, stand-clear/analysis interaction and removal. Shock effects must not replace rhythm logic. |
| IV/IO/splint | Generic treatment can prototype timed actions | Add specific character/prop clips once mechanics are stable. |
| Body-bag handling | BCM has carry/drag presentation code but relies on generic ACE carrying | Rebuild transport ownership/attachment safely; add hand poses and bag handling clips as needed. |
| Stretcher | No accepted BCM paired-carrier implementation | Coordinated pickup/putdown and two-person locomotion; commission or capture if original animation quality is insufficient. |

Blender can author these clips with the Reforger rig; Enfusion Blender Tools exports TXA and Workbench compiles ANM. Workbench's Animation Editor connects graphs, instances, events and blends; it is not an automatic generator of realistic human movement. I can write model-generation/animation scripts and integrate resources, but convincing multi-person motion requires visual iteration and may benefit from a specialist animator. No claim is made that those clips have already been produced. See [official animation export](https://community.bistudio.com/wiki/Arma_Reforger:Enfusion_Blender_Tools:_Import/Export_Animation) and [export profiles](https://community.bistudio.com/wiki/Arma_Reforger:Animation_Export_Profiles).

## Existing BCM migration contract

Retain all lives settings: enable/disable, total lives including first deployment, targeted add/remove/reset, reset all, confirmed-death accounting, backend identity across reconnect, GM-authorised changes, and last-life spectator behaviour. Rewrite the derived life/spectator core. Suppression retains enable, release, blur/vignette values and both multipliers; focus retains enable, strength, duration and cooldown. Their existing BCM implementation can be retained where provenance is BCM and no external classes/resources are required.

Preserve body-bag bagging transactions, inventory transfer/search, naming, burial/home disposition, cargo loading/removal, vehicle recovery, disconnect retention and admin tools. Combine its disposition respawn gate with the life gate: spawn requires both applicable rules. One death record should feed life accounting and disposition, with explicit mission/session persistence. One camera router must resolve remaining lives, disposition waiting and admin release. Copying both old camera overrides unchanged would create competing behaviour.

Do not publish or load the new project alongside the old BCM mods when they override the same native resources/classes. New custom resources require unique GUIDs. Native override resources keep their native GUIDs. Asset paths and localisation must be consolidated deliberately; a generated resource database is not a substitute for validating references.

## Build order and acceptance

1. Independent project and provenance inventory. Original pure patient state, treatment and life policy with automated invariants. Preserve existing projects.
2. Native damage/consciousness adapter and server scheduler; native-item treatments and patient examinations. Confirm no ACE is loaded during compilation/runtime.
3. Original lives/GM integration, retained BCM suppression/focus, consolidated camera and body-bag transport/respawn integration.
4. Medical interface and physical items, airway/chest/circulation/CPR/AED. Add each mechanic with server-side inventory and cancellation validation.
5. Dedicated server plus two clients: two unrelated patients, concurrency, stale requests, JIP, disconnect, possession, vehicles, body bags, rescue/depletion recurrence and last-life behaviour.
6. Asset visual QA, controller UI, load/performance testing, source/licence manifest and release packaging.

An addon with only the dependency list removed is not accepted. A compile pass is not a multiplayer gameplay pass. Foundation work is allowed to use placeholders but must state which features are still absent. The intended final product is the full BCM-owned ecosystem; this audit is the first deliverable, followed by implementation in verified stages.
