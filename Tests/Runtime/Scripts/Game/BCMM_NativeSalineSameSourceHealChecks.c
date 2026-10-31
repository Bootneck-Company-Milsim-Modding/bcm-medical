// Private same-original-source reapplication after FullHeal. Fresh use enters through real native commit.
class BCMM_NativeSalineSameSourceHealChecks : BCMM_NativeSalineRuntimeChecks
{
 protected float m_PreviousLife;
 override protected void CheckFlow()
 {
  BCMM_StockSourceLifeState life = BCMM_StockSourceLifetime.Capture(m_Kit);
  m_Suite.Record(life && life.Remaining > 0, "Same-source FullHeal fixture has an actual already-running original bag lifetime");
  if (life)
  {
   m_PreviousLife = life.Remaining;
  }
  BaseInventoryStorageComponent destination = m_Inventory.FindStorageForItem(m_Kit, EStoragePurpose.PURPOSE_ANY);
  InventoryStorageManagerComponent patientInventory = InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(destination && patientInventory.TryMoveItemToStorage(m_Kit, destination), "Same original used native bag physically leaves its patient slot before FullHeal");
  GetGame().GetCallqueue().CallLater(HealAndPrepare, 700);
 }
 protected void HealAndPrepare()
 {
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth() * 0.6);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion = BCMM_BodyRegion.RIGHT_ARM;
  BCMM_VisualStatusBridge.Publish(m_Medical);
  GetGame().GetCallqueue().CallLater(CommitOriginalAgain, 500);
 }
 protected void CommitOriginalAgain()
 {
  SCR_EConsumableFailReason failReason;
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && m_Consumable.GetConsumableEffect().CanApplyEffect(m_Target, m_Actor, failReason), "Native predicate admits reapplication of the actual same original bag after FullHeal");
  m_BeforeVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_BeforeCells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  m_Consumable.ApplyItemEffect(m_Target, m_Actor, null, m_Kit, false);
  GetGame().GetCallqueue().CallLater(CheckReusedFlow, 3000);
 }
 protected void CheckReusedFlow()
 {
  BCMM_StockSourceLifeState life = BCMM_StockSourceLifetime.Capture(m_Kit);
  m_Suite.Record(BCMM_NativeSalineBinding.Attached(m_Target, m_Kit) && m_Medical.GetBloodHitZone().GetHealthScaled() > m_BeforeVolume + 0.0001 && Math.AbsFloat(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve - m_BeforeCells) < 0.0001, "Same original source resumes typed native delivery under the replacement clinical model");
  m_Suite.Record(life && life.Remaining < m_PreviousLife - 2, "Reapplication of the same original source cannot reset its already-running physical lifetime");
  Finish();
 }
}
