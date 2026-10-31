// Full-duration progress-only clinical-commit and cancellation fixture; engine execution required.
class BCMM_FieldProcedureRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Step;
 protected IEntity m_ExtraFloorA;
 protected IEntity m_ExtraFloorB;
 protected int m_Wound;
 protected IEntity m_Station;
 protected ResourceName m_ExpectedGraph;
 protected float m_Duration;
 override protected void Prepare()
 {
  m_ExtraFloorA = Spawn("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et", "100 0 3");
  m_ExtraFloorB = Spawn("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et", "100 0 -3");
  vector floorMin;
  vector floorMax;
  m_Floor.GetWorldBounds(floorMin, floorMax);
  vector centre = (floorMin + floorMax) * 0.5;
  centre[1] = floorMax[1] + 0.2;
  centre[2] = centre[2] - 0.7;
  m_Actor.SetOrigin(centre + "-0.8 0 0");
  m_Target.SetOrigin(centre + "0.3 0 0");
  m_Target.SetAngles("90 0 0");
  PrintFormat("BCMM_FIELD_FLOOR min=%1 max=%2 actor=%3 target=%4", floorMin, floorMax, m_Actor.GetOrigin(), m_Target.GetOrigin());
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  AIControlComponent actorAI = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));
  AIControlComponent targetAI = AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
  if (actorAI) actorAI.DeactivateAI();
  if (targetAI) targetAI.DeactivateAI();
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Field procedure actor has native kit storage");
  // Hold the real native unconscious animation without manufacturing hypoxic
  // terminal physiology during this isolated presentation test.
  m_Medical.BCMM_FieldFixtureUnconscious(true);
  m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Medical.BCMM_PublishFractures();
  BCMM_Settings settings = new BCMM_Settings();
  m_Wound = m_Medical.BCMM_GetPatient().RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 2, 0, settings);
  m_Medical.BCMM_GetPatient().DressWound(m_Wound, 1, settings);
  m_Station = Spawn("{6AB4B3F3FD21B100}PrefabsEditable/Auto/Items/E_BCMM_FieldSurgeryStation.et", m_Actor.GetOrigin() + "0 0 1");
  GetGame().GetCallqueue().CallLater(PrepareCancellation, 1500);
 }
 protected void PrepareCancellation()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious(), "Ground fixture is actually in native unconscious state");
  m_Kit = Spawn("{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Progress-only splint treatment reserves a real kit");
  PrintFormat("BCMM_FIELD_BEFORE_START actor=%1 target=%2 reach=%3 eligible=%4 falling=%5", m_Actor.GetOrigin(), m_Target.GetOrigin(), BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), BCMM_SplintTreatment.Eligible(m_Target, true), ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsFalling());
  m_Suite.Record(BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Progress-only splint procedure starts through production policy");
  GetGame().GetCallqueue().CallLater(CancelFirstAttempt, 4000);
 }
 protected void CancelFirstAttempt()
 {
  SCR_CharacterControllerComponent control = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  string graphName;
  if (control.GetScrInputContext().m_CustomAnimData) graphName = control.GetScrInputContext().m_CustomAnimData.m_sGraphName;
  PrintFormat("BCMM_FIELD_SETUP graph=%1 loiter=%2 canPlay=%3 reach=%4 selected=%5 busy=%6 target=%7 actor=%8", graphName, control.IsLoitering(), control.CanPlayLoiterAnimation(ELoiteringType.CUSTOM), BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), BCMM_FieldClips.Select(m_Actor, m_Target, m_Kit), BCMM_SplintTreatment.IsBusy(m_Actor, m_Target), m_Target.GetOrigin(), m_Actor.GetOrigin());
  m_Suite.Record(BCMM_TreatmentAccess.Setting("fieldProcedureProgressOnly") > 0.5 && BCMM_SplintTreatment.IsBusy(m_Actor, m_Target) && !BCMM_CPRAnimation.IsActive(m_Actor, BCMM_FieldClips.SPLINT_GRAPH), "Progress-only treatment reserves the procedure without starting custom splint playback");
  BCMM_SplintTreatment.CancelActor(m_Actor);
  GetGame().GetCallqueue().CallLater(CheckFieldCancelled, 2000);
 }
 protected void CheckFieldCancelled()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !m_Medical.BCMM_GetPatient().LeftLeg.Splinted, "Interrupted timed fitting retains the original kit without treating the leg");
  m_Suite.Record(!BCMM_CPRAnimation.IsActive(m_Actor, BCMM_FieldClips.SPLINT_GRAPH), "Canceled timed procedure leaves custom playback inactive");
  BeginStep();
 }
 protected void BeginStep()
 {
  PrintFormat("BCMM_FIELD_STEP step=%1 reach=%2 eligible=%3 using=%4 busy=%5 actor=%6 target=%7", m_Step, BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), BCMM_SplintTreatment.Eligible(m_Target, true), ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), BCMM_SplintTreatment.IsBusy(m_Actor, m_Target), m_Actor.GetOrigin(), m_Target.GetOrigin());
  bool started;
  if (m_Step == 0)
  {
   m_ExpectedGraph = BCMM_FieldClips.SPLINT_GRAPH;
   m_Duration = BCMM_TreatmentAccess.Setting("splintApplicationSeconds");
   started = BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true);
  }
  else
  {
   ResourceName supply = "{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et";
   BCMM_SurgicalProcedure procedure = BCMM_SurgicalProcedure.EXTRACT_PROJECTILE;
   m_ExpectedGraph = BCMM_FieldClips.SURGERY_GRAPH;
   m_Duration = BCMM_TreatmentAccess.Setting("surgeryDurationSeconds");
   if (m_Step == 2)
   {
    supply = "{6AB4B3F3FD216010}Prefabs/Items/BCMM_SutureKit.et";
    procedure = BCMM_SurgicalProcedure.SUTURE;
    m_ExpectedGraph = BCMM_FieldClips.SUTURE_GRAPH;
    m_Duration = BCMM_TreatmentAccess.Setting("sutureDurationSeconds");
   }
   m_Kit = Spawn(supply, m_Actor.GetOrigin());
   m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Next timed procedure receives its exact original kit");
   started = BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, procedure, null, BCMM_BodyRegion.LEFT_ARM);
  }
  m_Suite.Record(started, "Full-duration progress-only field procedure begins");
  GetGame().GetCallqueue().CallLater(CheckWorking, m_Duration * 500);
  GetGame().GetCallqueue().CallLater(CheckCommitted, m_Duration * 1000 + 8000);
 }
 protected void CheckWorking()
 {
  m_Suite.Record(BCMM_SplintTreatment.IsBusy(m_Actor, m_Target) && !BCMM_CPRAnimation.IsActive(m_Actor, m_ExpectedGraph) && m_Kit && m_Inventory.Contains(m_Kit), "Progress-only procedure remains reserved at midpoint with original kit unconsumed and no custom playback");
  if (m_Step == 0) m_Suite.Record(!m_Medical.BCMM_GetPatient().LeftLeg.Splinted, "Timed fitting cannot repair the leg before the configured duration");
  if (m_Step == 1) m_Suite.Record(m_Medical.BCMM_GetPatient().FindWound(m_Wound).RetainedProjectile, "Timed surgery cannot extract before its configured duration");
  if (m_Step == 2) m_Suite.Record(!m_Medical.BCMM_GetPatient().FindWound(m_Wound).Sutured, "Timed suturing cannot close the wound before its configured duration");
 }
 protected void CheckCommitted()
 {
  m_Suite.Record(!m_Kit, "Successful timed procedure consumes its reserved kit only after the server timer");
  if (m_Step == 0) m_Suite.Record(m_Medical.BCMM_GetPatient().LeftLeg.Splinted, "Timed fitting commits the splint to the selected leg");
  if (m_Step == 1) m_Suite.Record(!m_Medical.BCMM_GetPatient().FindWound(m_Wound).RetainedProjectile && !m_Medical.BCMM_GetPatient().FindWound(m_Wound).Sutured, "Original extraction leaves closure as a separate operation");
  if (m_Step == 2) m_Suite.Record(m_Medical.BCMM_GetPatient().FindWound(m_Wound).Sutured, "Timed suturing commits durable wound closure");
  m_Step++;
  if (m_Step < 3) BeginStep();
  else
  {
   if (m_Station) SCR_EntityHelper.DeleteEntityAndChildren(m_Station);
   if (m_ExtraFloorA) SCR_EntityHelper.DeleteEntityAndChildren(m_ExtraFloorA);
   if (m_ExtraFloorB) SCR_EntityHelper.DeleteEntityAndChildren(m_ExtraFloorB);
   Finish();
  }
 }
}
modded class SCR_CharacterDamageManagerComponent
{
 [RplProp()] protected bool m_BCMMFieldFixtureUnconscious;
 void BCMM_FieldFixtureUnconscious(bool enabled) { m_BCMMFieldFixtureUnconscious = enabled; UpdateConsciousness(); Replication.BumpMe(); }
 override protected bool ShouldBeUnconscious()
 {
  return m_BCMMFieldFixtureUnconscious || super.ShouldBeUnconscious();
 }
}
