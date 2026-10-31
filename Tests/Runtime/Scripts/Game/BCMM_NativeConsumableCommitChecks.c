// Original native stock item callback, rejected commit and subsequent-use regression.
class BCMM_NativeConsumableCommitChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_BaseGameMode m_Mode;
 protected SCR_ConsumableItemComponent m_StockConsumable;
 protected SCR_CharacterControllerComponent m_StockController;
 protected bool m_StockBegan;
 protected bool m_StockCompleted;
 protected int m_StockAttempts;
 protected int m_StockPhase;
 override protected void Prepare()
 {
  m_Mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Mode.BCMM_TestRoleSetting("medicOnlyMedication", 0);
  SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(false);
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  array<HitZone> physical = {};
  m_Medical.GetPhysicalHitZones(physical);
  m_Suite.Record(physical.Count() > 0, "Native morphine fixture has a physical hit zone");
  if (physical.Count() > 0)
  {
   physical[0].SetHealth(physical[0].GetMaxHealth() * 0.8);
  }
  m_Medical.BCMM_GetPatient().Medication.Pain = 70;
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Kit = Spawn("{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Commit probe owns the exact native stock morphine injector");
  if (!m_Kit)
  {
   Finish();
   return;
  }
  m_StockConsumable = SCR_ConsumableItemComponent.Cast(m_Kit.FindComponent(SCR_ConsumableItemComponent));
  m_StockController = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Kit, EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(BeginStock, 1000);
 }
 protected void BeginStock()
 {
  if (m_StockController.GetAttachedGadgetAtLeftHandSlot() != m_Kit || !m_StockController.CanUseItem())
  {
   if (m_StockAttempts++ < 24)
   {
    GetGame().GetCallqueue().CallLater(BeginStock, 250);
    return;
   }
   m_Suite.Record(false, "Native stock injector becomes usable in the original actor's hand");
   Finish();
   return;
  }
  SCR_ConsumableEffectBase effect = m_StockConsumable.GetConsumableEffect();
  m_Suite.Record(SCR_ConsumableMorphine.Cast(effect) && effect.GetDeleteOnUse() && effect.CanApplyEffect(m_Target, m_Actor), "Native stock morphine is initially clinically eligible and ordinarily consumed on success");
  m_StockConsumable.SetTargetCharacter(m_Target);
  ItemUseParameters parameters = effect.GetAnimationParameters(m_Kit, m_Target);
  parameters.SetCommandID(effect.GetApplyToOtherAnimCmnd(m_Actor));
  parameters.SetMaxAnimLength(effect.GetApplyToOtherDuraction());
  m_StockController.m_OnItemUseBeganInvoker.Insert(OnStockBegan);
  m_StockController.m_OnItemUseEndedInvoker.Insert(OnStockEnded);
  m_Suite.Record(effect.ActivateEffect(m_Target, m_Actor, m_Kit, parameters), "Actual native other-patient stock-injector animation starts");
  GetGame().GetCallqueue().CallLater(CheckStockBegin, 700);
 }
 protected void OnStockBegan(IEntity item, ItemUseParameters parameters)
 {
  if (item == m_Kit)
  {
   m_StockBegan = true;
  }
 }
 protected void OnStockEnded(IEntity item, bool successful, ItemUseParameters parameters)
 {
  if (m_StockBegan && successful)
  {
   m_StockCompleted = true;
  }
 }
 protected void CheckStockBegin()
 {
  m_Suite.Record(m_StockBegan && m_StockController.IsUsingItem(), "Original native controller actually uses the stock injector before revocation");
  if (m_StockPhase == 0)
  {
   m_Mode.BCMM_TestRoleSetting("medicOnlyMedication", 1);
   m_Suite.Record(!BCMM_NativeTreatmentSafety.MorphineAllowed(m_Target, m_Actor), "Live GM role policy revokes eligibility during the actual native animation");
   PrintFormat("BCMM_STOCK_REVOKED began=%1 using=%2 allowed=%3", m_StockBegan, m_StockController.IsUsingItem(), BCMM_NativeTreatmentSafety.MorphineAllowed(m_Target, m_Actor));
  }
  else
  {
   m_Suite.Record(BCMM_NativeTreatmentSafety.MorphineAllowed(m_Target, m_Actor), "The same retained stock injector is eligible for an ordinary subsequent native use");
  }
  m_StockAttempts = 0;
  GetGame().GetCallqueue().CallLater(CheckStockCommit, 250);
 }
 protected void CheckStockCommit()
 {
  if (m_StockController.IsUsingItem() && m_StockAttempts++ < 100)
  {
   GetGame().GetCallqueue().CallLater(CheckStockCommit, 250);
   return;
  }
  m_Suite.Record(m_StockCompleted, "Native successful-completion callback is observed on the tested stock use");
  PrintFormat("BCMM_STOCK_COMMIT phase=%1 began=%2 completed=%3 source=%4 owned=%5 doses=%6", m_StockPhase, m_StockBegan, m_StockCompleted, m_Kit != null, m_Kit && m_Inventory.Contains(m_Kit), m_Medical.BCMM_GetPatient().Medication.ActiveDoses());
  if (m_StockPhase == 0)
  {
   m_Suite.Record(m_Kit && !m_Kit.IsDeleted() && m_Inventory.Contains(m_Kit), "Rejected stock morphine commit retains the exact original injector");
   m_Suite.Record(m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "Rejected native commit adds no clinical dose");
  }
  else
  {
   m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 1, "Ordinary subsequent native success consumes the same original injector and commits exactly one dose");
  }
  m_StockController.m_OnItemUseBeganInvoker.Remove(OnStockBegan);
  m_StockController.m_OnItemUseEndedInvoker.Remove(OnStockEnded);
  m_Mode.BCMM_TestRoleSetting("medicOnlyMedication", 0);
  if (m_StockPhase == 0 && m_Kit)
  {
   m_StockPhase = 1;
   m_StockBegan = false;
   m_StockCompleted = false;
   m_StockAttempts = 0;
   m_Medical.FullHeal();
   array<HitZone> physical = {};
   m_Medical.GetPhysicalHitZones(physical);
   if (physical.Count() > 0)
   {
    physical[0].SetHealth(physical[0].GetMaxHealth() * 0.8);
   }
   m_Medical.BCMM_GetPatient().Medication.Pain = 70;
   SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Kit, EGadgetMode.IN_HAND);
   GetGame().GetCallqueue().CallLater(BeginStock, 1000);
   return;
  }
  Finish();
 }
}

