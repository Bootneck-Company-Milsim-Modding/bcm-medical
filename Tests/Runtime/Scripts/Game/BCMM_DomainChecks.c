// Diagnostic addon only. Never publish as gameplay.
modded class SCR_BaseGameMode
{
 protected ref BCMM_DomainChecks m_BCMMChecks;
 override void OnGameStart()
 {
  super.OnGameStart();
  m_BCMMChecks = new BCMM_DomainChecks();
  GetGame().GetCallqueue().CallLater(m_BCMMChecks.Run, 250);
 }
}

class BCMM_DomainChecks
{
 protected int m_Failures;
 protected int m_Checks;
 protected void Check(bool condition, string label)
 {
  m_Checks++;
  if (!condition)
  {
   m_Failures++;
   Print("BCMM_CHECK FAILED: " + label, LogLevel.ERROR);
  }
 }

 void Run()
 {
  CheckPatientIsolation();
  CheckDeadline();
  CheckLives();
  CheckTrauma();
  CheckTransport();
  CheckSettings();
  CheckGMResource();
  CheckBodyBagResources();
  CheckRestartPhase();
  string runToken;
  System.GetCLIParam("bcmmRunToken", runToken);
  PrintFormat("BCMM_CHECK COMPLETE checks=%1 failures=%2 run=%3", m_Checks, m_Failures, runToken);
  GetGame().RequestClose();
 }

 protected void CheckPatientIsolation()
 {
  BCMM_Patient first = new BCMM_Patient();
  BCMM_Patient second = new BCMM_Patient();
  first.HasSpontaneousCirculation = false;
  first.Rhythm = BCMM_CardiacRhythm.ASYSTOLE;
  Check(!first.IsShockAdvised(), "Asystole is not shockable");
  first.Rhythm = BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;
  Check(!first.IsShockAdvised(), "PEA is not shockable");
  first.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  Check(first.IsShockAdvised(), "Pulseless VF is shockable");
  first.Rhythm = BCMM_CardiacRhythm.PULSELESS_VENTRICULAR_TACHYCARDIA;
  Check(first.IsShockAdvised(), "Pulseless VT is shockable");
  Check(first.StartCompressions(1, 10), "First medic acquires CPR session");
  Check(first.StartCompressions(1, 10), "Repeated identical start is idempotent");
  Check(!first.StartCompressions(2, 11), "Second medic cannot steal CPR");
  Check(!first.StopCompressions(2, 10), "Other medic cannot cancel CPR");
  Check(!second.HasCompressions() && second.HasSpontaneousCirculation, "CPR does not affect bystander");
  Check(first.StopCompressions(1, 10) && first.StartCompressions(2, 11), "CPR handoff after release");
  Check(!first.StopCompressions(1, 10) && first.HasCompressions(), "Stale cancel cannot end new session");
  first.LeftChest.OpenWound = true;
  first.LeftChest.Tension = true;
  first.LeftChest.PleuralBloodMl = 200;
  Check(first.LeftChest.ApplySeal() && first.LeftChest.Tension, "Seal does not cure tension");
  Check(!first.RightChest.Sealed && !second.LeftChest.Sealed, "Chest side and patient isolation");
  Check(first.LeftChest.RelieveTension() && first.LeftChest.PleuralBloodMl == 200, "NCD does not remove haemothorax");
  first.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  Check(!first.CanVentilate() && second.CanVentilate(), "Obstructed airway blocks ventilation");
  int woundId = first.AddWound(1, 20);
  BCMM_Wound wound = first.FindWound(woundId);
  wound.CompressionFraction = 0.75;
  Check(first.TotalExternalFlowMlPerSecond() == 5 && second.TotalExternalFlowMlPerSecond() == 0, "Wounds and treatment belong to patient");
  first.MarkTerminal();
  Check(!first.HasCompressions() && !first.IsShockAdvised() && !first.StartCompressions(3, 12), "Terminal cleanup prevents treatment restart");
 }

 protected void CheckDeadline()
 {
  BCMM_DepletionPolicy policy = new BCMM_DepletionPolicy();
  Check(!policy.Observe(0, 100) && !policy.IsActive(), "Positive reserve starts no timer");
  Check(!policy.Observe(10, 0) && policy.DeadlineSeconds() == 70, "Depletion starts sixty second timer");
  policy.Observe(20, 0);
  Check(policy.DeadlineSeconds() == 70, "Repeated injuries do not extend deadline");
  Check(!policy.RecordDeliveredRescueFluid(30, 0, 100), "Merely starting infusion is not rescue");
  Check(!policy.Observe(35, 100) && policy.IsActive(), "Unqualified health setter is not rescue");
  Check(policy.RecordDeliveredRescueFluid(69.9, 1, 1) && !policy.IsActive(), "Actual timely fluid delivery rescues");
  policy.Observe(80, 0);
  Check(policy.DeadlineSeconds() == 140, "Re-depletion receives new deadline");
  Check(!policy.RecordDeliveredRescueFluid(140, 1, 1) && policy.IsExpired(), "Expiry wins exact boundary");
  Check(!policy.RecordDeliveredRescueFluid(141, 100, 100), "Late infusion cannot revive terminal policy");
  policy.ResetByFullHeal();
  Check(!policy.IsActive() && !policy.IsExpired(), "Explicit GM reset clears terminal policy");
  policy.Observe(150, 0);
  policy.Observe(149, 0);
  Check(policy.DeadlineSeconds() == 210 && policy.Observe(210, 0), "Backward clock does not extend deadline");
 }

 protected void CheckLives()
 {
  BCMM_LifeLedger ledger = new BCMM_LifeLedger();
  Check(ledger.RemainingDeployments(1) == 1, "One life includes initial deployment");
  int first = ledger.ReserveDeployment(1);
  Check(first > 0 && ledger.ReserveDeployment(1) == 0, "Concurrent spawn reservation is blocked");
  Check(ledger.CancelDeployment(first) && ledger.RemainingDeployments(1) == 1, "Failed spawn spends no life");
  first = ledger.ReserveDeployment(1);
  Check(ledger.CommitDeployment(first) && !ledger.CommitDeployment(first), "Successful spawn charged exactly once");
  Check(ledger.ReserveDeployment(1, false) == 0, "Disabled limiter cannot create second active body");
  Check(!ledger.ConfirmDeath(first - 1) && ledger.IsAlive(), "Stale death cannot kill active life");
  Check(ledger.ConfirmDeath(first) && !ledger.ConfirmDeath(first), "Confirmed death is idempotent");
  Check(ledger.ReserveDeployment(1) == 0, "One life blocks second deployment");
  Check(ledger.AdjustAllowance(1), "Authority domain permits one life grant");
  ledger.SetDispositionBlocked(true);
  Check(ledger.ReserveDeployment(1) == 0, "Life grant does not bypass body disposition");
  ledger.SetDispositionBlocked(false);
  int next = ledger.ReserveDeployment(1);
  Check(next > first && ledger.CommitDeployment(next), "Released disposition permits granted life");
  Check(!ledger.ConfirmDeath(first) && ledger.IsAlive(), "Old corpse callback cannot affect next life");
  Check(ledger.AdjustAllowance(-1) && ledger.IsAlive(), "Removing allowance does not kill current body");
  ledger.ResetAllowance();
  Check(ledger.IsAlive() && ledger.RemainingDeployments(1) == 0, "Reset retains active first life");
  ledger.ConfirmDeath(next);
  ledger.ResetAllowance();
  Check(ledger.RemainingDeployments(1) == 1, "Explicit dead-account reset restores allowance");
 }

 protected void CheckTrauma()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient patient = new BCMM_Patient();
  int retained = patient.RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 2, 0.39, settings);
  int exited = patient.RegisterInjury(BCMM_BodyRegion.RIGHT_ARM, BCMM_InjuryCause.GUNSHOT, 2, 0.4, settings);
  Check(patient.FindWound(retained).RetainedProjectile && !patient.FindWound(exited).RetainedProjectile, "Retained chance has deterministic forty percent boundary");
  Check(patient.DressWound(retained, 0, settings), "Retained wound can be temporarily bandaged");
  BCMM_Wound wound = patient.FindWound(retained);
  Check(wound.Advance(99, BCMM_Activity.RUNNING, settings) == 0 && wound.Bandaged, "Running wears dressing before reopening");
  Check(wound.Advance(2, BCMM_Activity.RUNNING, settings) > 0 && !wound.Bandaged, "Running reopens minimum dressing at one hundred seconds");
  Check(Math.AbsFloat(wound.FlowMlPerSecond() - 1.3) < 0.001, "Reopened wound bleeds less");
  BCMM_ProcedureCapabilities capability = new BCMM_ProcedureCapabilities();
  capability.SutureKit = true; capability.QualifiedSurgeon = true;
  patient.DressWound(retained, 0.5, settings);
  Check(!patient.SutureWound(retained, capability), "Retained projectile blocks definitive closure in BCM rules");
  Check(!patient.ExtractProjectile(retained, capability, settings), "Extraction needs surgery kit");
  capability.SurgeryKit = true;
  Check(patient.ExtractProjectile(retained, capability, settings) && patient.SutureWound(retained, capability), "Extraction and suture are distinct kit-gated procedures");
  Check(wound.Advance(10000, BCMM_Activity.RUNNING, settings) == 0 && wound.Sutured, "Sutured wound does not reopen from old dressing timer");
  Check(!patient.SutureWound(retained, capability), "Duplicate suture cannot commit twice");
  int head = patient.RegisterInjury(BCMM_BodyRegion.HEAD, BCMM_InjuryCause.GUNSHOT, 1, 0, settings);
  Check(!patient.ExtractProjectile(head, capability, settings), "Head surgery requires facility");
  capability.AtMedicalFacility = true;
  Check(patient.ExtractProjectile(head, capability, settings), "Qualified facility head extraction");
  int cut = patient.RegisterInjury(BCMM_BodyRegion.LEFT_LEG, BCMM_InjuryCause.LACERATION, 1, 0, settings);
  Check(!patient.FindWound(cut).RetainedProjectile, "Cuts never receive retained bullet flag");
  Check(!patient.SutureWound(cut, capability), "Uncontrolled wound must be stabilised before suture");
  patient.DressWound(cut, 1, settings);
  Check(patient.FindWound(cut).Advance(599, BCMM_Activity.RESTING, settings) == 0, "Maximum resting dressing lasts up to ten minutes");
  patient.FindWound(cut).Advance(2, BCMM_Activity.RESTING, settings);
  Check(patient.FindWound(cut).Reopened, "Non-bullet wounds also reopen");
  patient.LeftLeg.Broken = true;
  Check(!patient.LegsAllowJump() && patient.LegMovementScale(settings) < 0.1, "Unsplinted fracture is extremely slow and cannot jump");
  Check(patient.LeftLeg.ApplySplint() && patient.LegMovementScale(settings) > 0.5, "Splint improves movement without healing fracture");
  Check(patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.RESTING, settings) == 0, "Resting fracture has no movement-induced loss");
  Check(patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.RUNNING, settings) > patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.WALKING, settings), "Running loads fractured limb more than walking");
  Check(!patient.LeftLeg.DefinitiveRepair(false, true, true) && patient.LeftLeg.Broken, "Field repair cannot heal fracture");
  Check(patient.LeftLeg.DefinitiveRepair(true, true, true) && patient.LegsAllowJump(), "Facility treatment repairs fracture");
  int chestBleed = patient.AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 2);
  BCMM_InternalBleeding source = patient.FindInternalBleed(chestBleed);
  Check(source.Advance(10) == 20 && source.PooledMl == 20, "Internal bleeding loses circulating blood and creates local collection");
  Check(source.DrainPleural(10) == 10 && !source.Repaired && source.Advance(1) == 2, "Drain removes collection without repairing active source");
  capability.AtMedicalFacility = false;
  Check(!patient.RepairInternalBleeding(chestBleed, capability, settings), "Critical internal surgery respects facility restriction");
  capability.AtMedicalFacility = true;
  Check(patient.RepairInternalBleeding(chestBleed, capability, settings) && source.Advance(10) == 0, "Repair stops internal source");
  int abdomen = patient.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN, BCMM_BleedCompartment.ABDOMEN, 1);
  Check(patient.FindInternalBleed(abdomen).DrainPleural(100) == 0, "Chest drain cannot treat abdominal bleeding");
  // Exact event split must not depend on server frame length.
  BCMM_Wound coarse = new BCMM_Wound(); BCMM_Wound fine = new BCMM_Wound();
  coarse.UncontrolledFlowMlPerSecond = 2; fine.UncontrolledFlowMlPerSecond = 2;
  coarse.Dress(settings, 0); fine.Dress(settings, 0);
  float coarseLoss = coarse.Advance(110, BCMM_Activity.RUNNING, settings);
  float fineLoss;
  for (int i = 0; i < 110; i++) fineLoss += fine.Advance(1, BCMM_Activity.RUNNING, settings);
  Check(Math.AbsFloat(coarseLoss - fineLoss) < 0.01, "Reopening blood loss is invariant to coarse/fine frame steps");
 }

 protected void CheckTransport()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_CarryConsent consent = new BCMM_CarryConsent();
  Check(consent.Request(1, 10, 20, 5, 100, settings), "Conscious carry request created");
  Check(!consent.Respond(1, 10, true, 101), "Carrier cannot consent for patient");
  Check(consent.Respond(1, 20, true, 101), "Patient accepts carry request");
  Check(!consent.Consume(1, 11, 20, 5, 102) && !consent.Consume(1, 10, 20, 6, 102), "Consent bound to carrier and patient life");
  Check(consent.Consume(1, 10, 20, 5, 102) && !consent.Consume(1, 10, 20, 5, 102), "Consent consumed exactly once");
  Check(!consent.Respond(1, 20, true, 103), "Consumed consent rejects replayed acceptance");
  consent.Request(2, 10, 20, 5, 200, settings);
  Check(!consent.Respond(2, 20, true, 215), "Consent expires at configured boundary");
  BCMM_BodyBagDragSession session = new BCMM_BodyBagDragSession();
  Check(!session.Begin(10, 1, true, false), "Rope drag requires rope item capability");
  Check(session.Begin(10, 1, true, true) && session.AllowsWeapon() && session.AllowsRunning() && session.AllowsCrouching(), "Rope drag policy permits shooting running and crouching");
  Check(!session.Begin(11, 2, false, false) && !session.Release(11, 1), "Second actor cannot steal bag");
  vector desired;
  Check(session.DesiredBagPosition("0 0 0", "3 0 0", true, true, settings, desired) && vector.Distance(desired, "2 0 0") < 0.001, "Rope target respects configured length");
  Check(!session.DesiredBagPosition("0 0 0", "3 0 0", true, false, settings, desired) && session.Mode() == BCMM_TransportMode.NONE, "Obstacle releases instead of teleporting bag through wall");
  session.Begin(10, 2, false, false);
  Check(!session.AllowsWeapon() && !session.AllowsRunning(), "Manual dragging has occupied hands and limited locomotion");
  Check(!session.DesiredBagPosition("0 0 0", "20 0 0", true, true, settings, desired), "Large separation triggers safe release");
 }

 protected void CheckSettings()
 {
  BCMM_Settings settings = new BCMM_Settings();
  for (int i = 0; i < settings.Count(); i++) Check(settings.Definition(i).Accepts(settings.ValueAt(i)), "Catalogue default in range: " + settings.Definition(i).Key);
  Check(!settings.Set("retainedBulletChance", 2), "Out of range rejected");
  Check(!settings.Set("totalLives", 1.5), "Integer lives rejects fraction");
  Check(!settings.Set("bandageReopenMinSeconds", 700), "Inverted min max rejected");
  Check(!settings.Set("ropeLengthMetres", 5) || settings.Get("ropeBreakDistanceMetres") > 5, "Rope length stays below release distance");
  Check(!SCR_BaseGameMode.BCMM_IsSettingsEditor(null), "No editor manager means no settings authority");
  BCMM_SettingsStore store = new BCMM_SettingsStore("$profile:BCMM_SettingsChecks");
  store.Load(settings);
  Check(settings.Set("retainedBulletChance", 0.52) && store.Save(settings), "Settings JSON write and verify");
  BCMM_Settings loaded = new BCMM_Settings();
  BCMM_SettingsStore reload = new BCMM_SettingsStore("$profile:BCMM_SettingsChecks");
  Check(reload.Load(loaded) && Math.AbsFloat(loaded.Get("retainedBulletChance") - 0.52) < 0.001, "Settings JSON round trip");
  Check(loaded.Set("retainedBulletChance", 0.63) && reload.Save(loaded), "Alternate settings generation saved");
  BCMM_SettingsDocument invalid = new BCMM_SettingsDocument();
  invalid.Schema = 999; invalid.Generation = 999;
  FileIO.MakeDirectory("$profile:BCMM_InvalidSettings");
  invalid.PackToFile("$profile:BCMM_InvalidSettings/settings-a.json");
  BCMM_SettingsStore brokenStore = new BCMM_SettingsStore("$profile:BCMM_InvalidSettings");
  Check(!brokenStore.Load(loaded) && !brokenStore.IsWritable() && !brokenStore.Save(loaded), "Unknown/corrupt settings preserved and edits locked");
 }

 protected void CheckRestartPhase()
 {
  string phase;
  if (!System.GetCLIParam("bcmmPersistencePhase", phase)) return;
  BCMM_Settings state = new BCMM_Settings();
  BCMM_SettingsStore store = new BCMM_SettingsStore();
  if (phase == "seed")
  {
   store.Load(state);
   state.Set("retainedBulletChance", 0.52);
   Check(store.Save(state), "Restart seed generation one");
   state.Set("retainedBulletChance", 0.63);
   Check(store.Save(state), "Restart seed generation two");
  }
  if (phase == "verify") { Check(store.Load(state) && Math.AbsFloat(state.Get("retainedBulletChance") - 0.63) < 0.001, "Separate server process reloads latest settings"); Check(Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("retainedBulletChance") - 0.63) < 0.001, "Actual game mode loads persisted value before gameplay"); }
  if (phase == "recover") { Check(store.Load(state) && Math.AbsFloat(state.Get("retainedBulletChance") - 0.52) < 0.001, "Corrupted latest generation falls back to previous settings"); Check(Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("retainedBulletChance") - 0.52) < 0.001, "Game mode uses recovered generation"); }
 }

 protected void CheckGMResource()
 {
  Resource resource = Resource.Load("{F3D6C6D25642352C}Configs/Editor/AttributeLists/Edit.conf");
  Check(resource && resource.IsValid(), "Native GM attribute override resource loads");
  if (!resource || !resource.IsValid()) return;
  BaseContainer container = resource.GetResource().ToBaseContainer();
  BaseContainerList attributes = container.GetObjectArray("m_aAttributes");
  int medicalAttributes;
  if (attributes)
  {
   for (int i = 0; i < attributes.Count(); i++)
    if (attributes.Get(i).GetClassName().StartsWith("BCMM_Setting_")) medicalAttributes++;
  }
  BCMM_Settings defaults = new BCMM_Settings();
  Check(medicalAttributes == defaults.Count(), "Every registered setting has a loaded native GM control");
 } protected void CheckBodyBagResources()
 {
  array<ResourceName> bagResources = {
   "{98555BDCD0BE490A}Assets/BodyBags/BCM_FilledBodyBag/BCM_FilledBodyBag_Collision.xob",
   "{977722F41091B866}Assets/BodyBags/BCM_EmptyBodyBag/BCM_EmptyBodyBag.xob",
   "{60F57EB7414F43B2}Assets/BodyBags/BCM_FilledBodyBag/Data/BCM_BodyBag.emat"
  };
  foreach (ResourceName path : bagResources)
  {
   Resource resource = Resource.Load(path);
   Check(resource && resource.IsValid(), "Owned body bag resource loads from standalone mod: " + path);
  }
 }}
