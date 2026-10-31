// Actual stock native saline animation, transfer and linked-flow regression.
class BCMM_NativeSalineRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_BaseGameMode m_Mode;
 protected SCR_ConsumableItemComponent m_Consumable;
 protected SCR_CharacterControllerComponent m_Controller;
 protected int m_Phase;
 protected int m_Waits;
 protected bool m_Began;
 protected bool m_Ended;
 protected float m_BeforeVolume;
 protected float m_BeforeCells;
 protected float m_StoppedVolume;
 override protected void Prepare()
 {
  m_Mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Mode.BCMM_TestRoleSetting("medicOnlyFluids", 0);
  SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(false);
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth() * 0.6);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion = BCMM_BodyRegion.RIGHT_ARM;
  BCMM_VisualStatusBridge.Publish(m_Medical);
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Kit = Spawn("{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Original native US saline source enters actual actor inventory");
  if (!m_Kit)
  {
   Finish();
   return;
  }
  m_Consumable = SCR_ConsumableItemComponent.Cast(m_Kit.FindComponent(SCR_ConsumableItemComponent));
  m_Controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  EquipSource();
 }
 protected void EquipSource()
 {
  m_Began = false;
  m_Ended = false;
  m_Waits = 0;
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Kit, EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(BeginUse, 1000);
 }
 protected void BeginUse()
 {
  if (m_Controller.GetAttachedGadgetAtLeftHandSlot() != m_Kit || !m_Controller.CanUseItem())
  {
   if (m_Waits++ < 24)
   {
    GetGame().GetCallqueue().CallLater(BeginUse, 250);
    return;
   }
   m_Suite.Record(false, "Original native saline becomes usable in actor hand");
   Finish();
   return;
  }
  SCR_ConsumableEffectBase effect = m_Consumable.GetConsumableEffect();
  m_Suite.Record(SCR_ConsumableSalineBag.Cast(effect) && effect.CanApplyEffect(m_Target, m_Actor), "Actual native saline predicate initially admits the blood-deficient accessed patient");
  PrintFormat("BCMM_STOCK_SALINE_START phase=%1 delete=%2 allowed=%3", m_Phase, effect.GetDeleteOnUse(), BCMM_NativeTreatmentSafety.FluidAllowed(m_Target, m_Actor));
  m_BeforeVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_BeforeCells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  m_Consumable.SetTargetCharacter(m_Target);
  ItemUseParameters parameters = effect.GetAnimationParameters(m_Kit, m_Target);
  parameters.SetCommandID(effect.GetApplyToOtherAnimCmnd(m_Actor));
  parameters.SetMaxAnimLength(effect.GetApplyToOtherDuraction());
  m_Controller.m_OnItemUseBeganInvoker.Insert(OnBegan);
  m_Controller.m_OnItemUseEndedInvoker.Insert(OnEnded);
  m_Suite.Record(effect.ActivateEffect(m_Target, m_Actor, m_Kit, parameters), "Actual native other-patient saline animation activates");
  GetGame().GetCallqueue().CallLater(CheckBegan, 700);
 }
 protected void OnBegan(IEntity item, ItemUseParameters parameters)
 {
  if (item == m_Kit)
  {
   m_Began = true;
  }
 }
 protected void OnEnded(IEntity item, bool successful, ItemUseParameters parameters)
 {
  if (m_Began && successful)
  {
   m_Ended = true;
  }
 }
 protected void CheckBegan()
 {
  m_Suite.Record(m_Began && m_Controller.IsUsingItem(), "Original native controller actually uses the stock saline source");
  if (m_Phase == 0)
  {
   m_Mode.BCMM_TestRoleSetting("medicOnlyFluids", 1);
   m_Suite.Record(!BCMM_NativeTreatmentSafety.FluidAllowed(m_Target, m_Actor), "Live GM role revocation rejects the native saline commit");
  }
  m_Waits = 0;
  GetGame().GetCallqueue().CallLater(WaitEnded, 250);
 }
 protected void WaitEnded()
 {
  if (m_Controller.IsUsingItem() && m_Waits++ < 160)
  {
   GetGame().GetCallqueue().CallLater(WaitEnded, 250);
   return;
  }
  m_Suite.Record(m_Ended, "Genuine successful native saline completion callback is observed");
  m_Controller.m_OnItemUseBeganInvoker.Remove(OnBegan);
  m_Controller.m_OnItemUseEndedInvoker.Remove(OnEnded);
  GetGame().GetCallqueue().CallLater(CheckCommit, 500);
 }
 protected void CheckCommit()
 {
  SCR_SalineStorageComponent storage = SCR_SalineStorageComponent.Cast(m_Target.FindComponent(SCR_SalineStorageComponent));
  array<IEntity> attached = {};
  if (storage)
  {
   storage.GetAll(attached);
  }
  PrintFormat("BCMM_STOCK_SALINE_COMMIT phase=%1 source=%2 actorOwned=%3 patientAttached=%4 volume=%5 cells=%6 nativeFlow=%7", m_Phase, m_Kit != null, m_Kit && m_Inventory.Contains(m_Kit), attached.Contains(m_Kit), m_Medical.GetBloodHitZone().GetHealthScaled(), m_Medical.BCMM_GetPatient().Fluids.RedCellReserve, m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect));
  if (m_Phase == 0)
  {
   m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && attached.IsEmpty() && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && Math.AbsFloat(m_Medical.GetBloodHitZone().GetHealthScaled() - m_BeforeVolume) < 0.0001, "Rejected native saline preserves the exact source and starts no transfer or blood delivery");
   if (!m_Kit)
   {
    Finish();
    return;
   }
   m_Mode.BCMM_TestRoleSetting("medicOnlyFluids", 0);
   m_Phase = 1;
   EquipSource();
   return;
  }
  m_Suite.Record(m_Kit && storage && attached.Contains(m_Kit) && !m_Inventory.Contains(m_Kit), "Ordinary second native use transfers the same original saline source into the patient's actual native slot");
  GetGame().GetCallqueue().CallLater(CheckFlow, 3000);
 }
 protected void CheckFlow()
 {
  float volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  float cells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  PrintFormat("BCMM_STOCK_SALINE_FLOW volume=%1 before=%2 cells=%3 beforeCells=%4 active=%5", volume, m_BeforeVolume, cells, m_BeforeCells, m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect));
  m_Suite.Record(volume > m_BeforeVolume && Math.AbsFloat(cells - m_BeforeCells) < 0.0001, "Actual native saline raises circulating volume without inventing red cells");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = false;
  m_StoppedVolume = volume;
  GetGame().GetCallqueue().CallLater(CheckStopped, 700);
 }
 protected void CheckStopped()
 {
  PrintFormat("BCMM_STOCK_SALINE_STOP active=%1 volume=%2 heldVolume=%3 source=%4", m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), m_Medical.GetBloodHitZone().GetHealthScaled(), m_StoppedVolume, m_Kit != null);
  m_Suite.Record(!m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && m_Medical.GetBloodHitZone().GetHealthScaled() <= m_StoppedVolume + 0.0001, "Actual stock native saline flow stops before further delivery after its source or vascular access is lost");
  Print("BCMM_STOCK_SALINE_CLEANUP_BEGIN");
  AfterStopped();
 }
 protected void AfterStopped()
 {
  Finish();
 }
}


class BCMM_NativeSalineSourceMoveChecks : BCMM_NativeSalineRuntimeChecks
{
 override protected void AfterStopped()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && BCMM_StockSourceLifetime.Capture(m_Kit), "Retrieved original native source retains its independent deadline before actual deletion");
  RplComponent.DeleteRplEntity(m_Kit, false);
  GetGame().GetCallqueue().CallLater(CheckDeletedSource, 200);
 }
 protected void CheckDeletedSource()
 {
  m_Suite.Record(!m_Kit && !BCMM_StockSourceLifetime.Capture(null) && !BCMM_StockSourceLifetime.Expired(null), "Deleting the genuine used stock source cannot transfer its deadline to a null source lookup");
  Finish();
 }
 override protected void CheckFlow()
 {
  float volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  float cells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  PrintFormat("BCMM_STOCK_SALINE_FLOW volume=%1 before=%2 cells=%3 beforeCells=%4 active=%5", volume, m_BeforeVolume, cells, m_BeforeCells, m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect));
  m_Suite.Record(volume > m_BeforeVolume && Math.AbsFloat(cells - m_BeforeCells) < 0.0001, "Actual native saline raises circulating volume without inventing red cells");
  BaseInventoryStorageComponent destination = m_Inventory.FindStorageForItem(m_Kit, EStoragePurpose.PURPOSE_ANY);
  InventoryStorageManagerComponent patientInventory = InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(destination && patientInventory.TryMoveItemToStorage(m_Kit, destination), "Original native attached saline is physically moved back into actor inventory");
  m_StoppedVolume = volume;
  GetGame().GetCallqueue().CallLater(CheckStopped, 700);
 }
}

class BCMM_NativeSalineFullHealChecks : BCMM_NativeSalineRuntimeChecks
{
 override protected void CheckFlow()
 {
  float volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  float cells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  PrintFormat("BCMM_STOCK_SALINE_FLOW volume=%1 before=%2 cells=%3 beforeCells=%4 active=%5", volume, m_BeforeVolume, cells, m_BeforeCells, m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect));
  m_Suite.Record(volume > m_BeforeVolume && Math.AbsFloat(cells - m_BeforeCells) < 0.0001, "Actual native saline raises circulating volume without inventing red cells");
  if (m_Phase == 1)
  {
   BaseInventoryStorageComponent destination = m_Inventory.FindStorageForItem(m_Kit, EStoragePurpose.PURPOSE_ANY);
   InventoryStorageManagerComponent patientInventory = InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
   m_Suite.Record(destination && patientInventory.TryMoveItemToStorage(m_Kit, destination), "Original used native source leaves its occupied slot before FullHeal fresh-source treatment");
   m_Medical.FullHeal();
   m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth() * 0.6);
   m_Medical.BCMM_GetPatient().Equipment.VascularAccess = true;
   m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion = BCMM_BodyRegion.RIGHT_ARM;
   BCMM_VisualStatusBridge.Publish(m_Medical);
   m_Kit = Spawn("{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et", m_Actor.GetOrigin());
   m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "FullHeal boundary equips a genuinely different original native saline bag");
   m_Consumable = SCR_ConsumableItemComponent.Cast(m_Kit.FindComponent(SCR_ConsumableItemComponent));
   m_Phase = 2;
   GetGame().GetCallqueue().CallLater(CheckFreshAfterHeal, 500);
   return;
  }
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = false;
  m_StoppedVolume = volume;
  GetGame().GetCallqueue().CallLater(CheckStopped, 700);
 }
 protected void CheckFreshAfterHeal()
 {
  m_Suite.Record(!m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && m_Medical.GetBloodHitZone().GetHealthScaled() <= 0.6001, "FullHeal terminates the tracked stock effect before old-model delivery can enter the replacement model");
  m_BeforeVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_BeforeCells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  m_Consumable.ApplyItemEffect(m_Target, m_Actor, null, m_Kit, false);
  GetGame().GetCallqueue().CallLater(CheckCommit, 500);
 }
}

