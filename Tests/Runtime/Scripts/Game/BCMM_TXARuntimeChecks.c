// Real native injection acceptance. Diagnostic addon only.
class BCMM_TXARuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected int m_WoundId;
 protected int m_ReleaseWait;
 override protected void Prepare()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  AIControlComponent ai = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  ai = AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "TXA fixture equips real inventory storage");
  m_Kit = Spawn("{6AB4B3F3FD216032}Prefabs/Items/BCMM_TXA.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Kit.GetVObject() && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "TXA uses its distinct real injectable item and model");
  BCMM_Settings settings = new BCMM_Settings();
  m_WoundId = m_Medical.BCMM_GetPatient().RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 1, 0, settings);
  m_Medical.BCMM_GetPatient().DressWound(m_WoundId, 0, settings);
  GetGame().GetCallqueue().CallLater(BeginInjection, 1200);
 }
 protected void BeginInjection()
 {
  m_Suite.Record(!BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, BCMM_MedicationKind.TXA), "TXA refuses injection without the configured vascular access even when its item exists");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = true;
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, BCMM_MedicationKind.TXA), "An injured patient with established access permits TXA injection");
  GetGame().GetCallqueue().CallLater(CancelInjection, 2500);
 }
 protected void CancelInjection()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "TXA plays native injection animation without a premature dose");
  BCMM_SplintTreatment.CancelActor(m_Actor);
  GetGame().GetCallqueue().CallLater(RestartInjection, 500);
 }
 protected void RestartInjection()
 {
  if (ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && m_ReleaseWait++ < 32) { GetGame().GetCallqueue().CallLater(RestartInjection, 250); return; }
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "Interrupted TXA retains its exact item and delivers no medication");
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, BCMM_MedicationKind.TXA), "The same retained TXA item can restart after native animation releases");
  GetGame().GetCallqueue().CallLater(CheckDelivered, 23000);
 }
 protected void CheckDelivered()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 1, "Completed TXA consumes exactly one original item and records one administration");
  GetGame().GetCallqueue().CallLater(CheckOnset, 31000);
 }
 protected void CheckOnset()
 {
  BCMM_Patient patient = m_Medical.BCMM_GetPatient();
  m_Suite.Record(patient.Medication.TXAEffect > 0.8 && patient.Medication.BreathingScale == 1 && patient.Medication.PressureScale == 1, "Native physiological ticks produce clot stability without opioid or pressure effects");
  m_Suite.Record(patient.FindWound(m_WoundId).RetainedProjectile && !patient.FindWound(m_WoundId).Sutured, "TXA injection does not substitute for extraction or suturing");
  Finish();
 }
}
