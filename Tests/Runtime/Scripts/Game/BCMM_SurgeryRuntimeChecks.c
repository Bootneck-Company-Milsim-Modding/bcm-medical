class BCMM_SurgeryRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Station;
 protected IEntity m_Suture;
 protected int m_Wound;
 override protected void Prepare()
 {
  if (!m_Actor || !m_Target) { m_Suite.Record(false, "Surgical characters spawn"); Finish(); return; }
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Surgery actor equips native backpack");
  BCMM_Settings settings = new BCMM_Settings();
  m_Wound = m_Medical.BCMM_GetPatient().RegisterInjury(BCMM_BodyRegion.HEAD, BCMM_InjuryCause.GUNSHOT, 2, 0, settings);
  m_Medical.BCMM_GetPatient().DressWound(m_Wound, 1, settings);
  m_Kit = Spawn("{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et", m_Actor.GetOrigin());
  m_Suture = Spawn("{6AB4B3F3FD216010}Prefabs/Items/BCMM_SutureKit.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY) && m_Suture && m_Inventory.TryInsertItem(m_Suture, EStoragePurpose.PURPOSE_ANY), "Original surgery and suture supplies enter inventory");
  m_Suite.Record(!BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.SUTURE), "Retained projectile blocks suturing even with correct kit");
  m_Suite.Record(!BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.EXTRACT_PROJECTILE), "Critical surgery is rejected outside medical facility");
  m_Station = Spawn("{6AB4B3F3FD216011}Prefabs/Items/BCMM_FieldSurgeryStation.et", m_Actor.GetOrigin() + "0 0 1");
  m_Suite.Record(m_Station && BCMM_MedicalFacilityComponent.Supports(m_Actor, m_Target), "Native field surgery marker establishes shared facility range");
  m_Suite.Record(BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.EXTRACT_PROJECTILE), "Facility surgery starts with retained projectile and exact supply");
  m_Suite.Record(!BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.EXTRACT_PROJECTILE), "Concurrent surgical procedure cannot reserve same patient or item");
  GetGame().GetCallqueue().CallLater(CheckNotInstant, 1000);
  GetGame().GetCallqueue().CallLater(CheckWorkingLoop, 30000);
  GetGame().GetCallqueue().CallLater(CheckExtracted, BCMM_TreatmentAccess.Setting("surgeryDurationSeconds") * 1000 + 8000);
 }
 protected void CheckNotInstant()
 {
  m_Suite.Record(m_Kit && m_Medical.BCMM_GetPatient().FindWound(m_Wound).RetainedProjectile, "Surgery does not instantly consume kit or extract bullet before timer");
 }
 protected void CheckWorkingLoop()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && m_Kit && m_Medical.BCMM_GetPatient().FindWound(m_Wound).RetainedProjectile, "Full-duration surgery remains in native treatment loop at 30 seconds without early extraction");
 }
 protected void CheckExtracted()
 {
  BCMM_Wound wound = m_Medical.BCMM_GetPatient().FindWound(m_Wound);
  m_Suite.Record(!m_Kit && !wound.RetainedProjectile && !wound.Sutured, "Completed extraction consumes surgery kit but leaves closure as separate procedure");
  m_Suite.Record(BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.SUTURE), "Suturing starts only after projectile extraction and bleeding control");
  GetGame().GetCallqueue().CallLater(CheckSutured, BCMM_TreatmentAccess.Setting("sutureDurationSeconds") * 1000 + 8000);
 }
 protected void CheckSutured()
 {
  BCMM_Wound wound = m_Medical.BCMM_GetPatient().FindWound(m_Wound);
  m_Suite.Record(!m_Suture && wound.Sutured && wound.Advance(1000, BCMM_Activity.RUNNING, new BCMM_Settings()) == 0, "Timed suture consumes its own supply and prevents movement reopening");
  m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Medical.BCMM_PublishFractures();
  m_Kit = Spawn("{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Fracture surgery receives separate sterile kit");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Station);
  m_Suite.Record(!BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.REPAIR_LEFT_LEG), "Definitive fracture repair cannot begin outside facility");
  m_Station = Spawn("{6AB4B3F3FD216011}Prefabs/Items/BCMM_FieldSurgeryStation.et", m_Actor.GetOrigin() + "0 0 1");
  m_Suite.Record(BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.REPAIR_LEFT_LEG), "Facility accepts definitive fracture surgery");
  GetGame().GetCallqueue().CallLater(CheckFractureRepaired, BCMM_TreatmentAccess.Setting("surgeryDurationSeconds") * 1000 + 8000);
 }
 protected void CheckFractureRepaired()
 {
  m_Suite.Record(!m_Kit && !m_Medical.BCMM_GetPatient().LeftLeg.Broken && m_Medical.BCMM_LegsAllowJump() && m_Medical.BCMM_MovementCap() == 1, "Completed facility surgery consumes kit and restores native fracture movement and jumping");
  if (m_Station) SCR_EntityHelper.DeleteEntityAndChildren(m_Station);
  Finish();
 }
}