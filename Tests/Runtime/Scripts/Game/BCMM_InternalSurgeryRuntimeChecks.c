class BCMM_InternalSurgeryRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Station;
 protected int m_SourceId;
 protected float m_Collection;
 override protected void Prepare()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Internal surgery equips native backpack");
  m_Kit = Spawn("{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Internal surgery reserves real surgical supply");
  m_SourceId = m_Medical.BCMM_GetPatient().AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 10);
  m_Station = Spawn("{6AB4B3F3FD216011}Prefabs/Items/BCMM_FieldSurgeryStation.et", m_Actor.GetOrigin() + "0 0 1");
  m_Suite.Record(BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.REPAIR_INTERNAL), "Facility begins internal bleeding surgery");
  m_Station.SetOrigin(m_Actor.GetOrigin() + "20 0 0");
  GetGame().GetCallqueue().CallLater(CheckFacilityCancellation, 500);
 }
 protected void CheckFacilityCancellation()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !m_Medical.BCMM_GetPatient().FindInternalBleed(m_SourceId).Repaired, "Losing facility eligibility cancels surgery and preserves original kit");
  m_Station.SetOrigin(m_Actor.GetOrigin() + "0 0 1");
  m_Suite.Record(BCMM_SurgicalTreatment.BeginSurgery(m_Actor, m_Target, BCMM_SurgicalProcedure.REPAIR_INTERNAL), "Cancelled internal surgery releases locks for a new attempt");
  GetGame().GetCallqueue().CallLater(CheckRepaired, BCMM_TreatmentAccess.Setting("surgeryDurationSeconds") * 1000 + 8000);
 }
 protected void CheckRepaired()
 {
  BCMM_InternalBleeding source = m_Medical.BCMM_GetPatient().FindInternalBleed(m_SourceId);
  m_Collection = source.PooledMl;
  m_Suite.Record(!m_Kit && source.Repaired && m_Collection > 0, "Completed internal surgery consumes kit and repairs source without erasing collected blood");
  m_Suite.Record(m_Medical.GetBloodHitZone().GetHealthScaled() < 0.99, "Internal surgery does not refill native circulating blood");
  GetGame().GetCallqueue().CallLater(CheckStopped, 2500);
 }
 protected void CheckStopped()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().FindInternalBleed(m_SourceId).PooledMl == m_Collection, "Repaired internal source no longer adds pleural collection");
  if (m_Station) SCR_EntityHelper.DeleteEntityAndChildren(m_Station);
  Finish();
 }
}