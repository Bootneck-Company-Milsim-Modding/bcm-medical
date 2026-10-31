// Private native owner animation and authority commit acceptance.
class BCMM_StockOwnerEvents
{
 int Phase;
 int Begins;
 int Ends;
 void Began(IEntity item, ItemUseParameters parameters)
 {
  Begins++;
  PrintFormat("BCMM_STOCK_OWNER_BEGAN phase=%1", Phase);
 }
 void Ended(IEntity item, bool successful, ItemUseParameters parameters)
 {
  if (successful)
  {
   Ends++;
   PrintFormat("BCMM_STOCK_OWNER_ENDED phase=%1", Phase);
  }
 }
}
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMStockPhase;
 [RplProp()] protected RplId m_BCMMStockActor = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMStockPatient = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMStockSource = RplId.Invalid();
 protected ref array<int> m_BCMMStockRetainedObservers = {};
 protected ref array<int> m_BCMMStockFinalObservers = {};
 protected bool m_BCMMStockOwnerRetained;
 void BCMM_PublishStock(int phase, IEntity actor, IEntity patient, IEntity source)
 {
  m_BCMMStockPhase = phase;
  m_BCMMStockActor = BCMM_BodyBagComponent.Id(actor);
  m_BCMMStockPatient = BCMM_BodyBagComponent.Id(patient);
  if (source)
  {
   m_BCMMStockSource = BCMM_BodyBagComponent.Id(source);
  }
  Replication.BumpMe();
  PrintFormat("BCMM_STOCK_NETWORK_PHASE %1", phase);
 }
 int BCMM_StockPhase() { return m_BCMMStockPhase; }
 IEntity BCMM_StockActor() { return BCMM_BodyBagComponent.Resolve(m_BCMMStockActor); }
 IEntity BCMM_StockPatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMStockPatient); }
 IEntity BCMM_StockSource() { return BCMM_BodyBagComponent.Resolve(m_BCMMStockSource); }
 bool BCMM_StockObserversReady(int phase)
 {
  if (phase == 3)
  {
   return m_BCMMStockOwnerRetained && m_BCMMStockRetainedObservers.Count() >= 3;
  }
  return m_BCMMStockFinalObservers.Count() >= 3;
 }
 void BCMM_RecordStockObservation(SCR_PlayerController player, int phase, bool ownerRetained)
 {
  if (!Replication.IsServer() || !player || !player.GetControlledEntity() || phase != m_BCMMStockPhase)
  {
   return;
  }
  int playerId = player.GetPlayerId();
  if (phase == 3)
  {
   if (!m_BCMMStockRetainedObservers.Contains(playerId))
   {
    m_BCMMStockRetainedObservers.Insert(playerId);
   }
   IEntity actor = BCMM_StockActor();
   IEntity source = BCMM_StockSource();
   InventoryStorageManagerComponent inventory;
   if (actor)
   {
    inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
   }
   if (playerId == 1 && player.GetControlledEntity() == actor && ownerRetained && source && inventory && inventory.Contains(source))
   {
    m_BCMMStockOwnerRetained = true;
   }
  }
  if (phase == 5 && !m_BCMMStockFinalObservers.Contains(playerId))
  {
   m_BCMMStockFinalObservers.Insert(playerId);
  }
 }
}
modded class SCR_PlayerController
{
 void BCMM_ReportStockObservation(int phase, bool ownerRetained)
 {
  if (GetGame().GetPlayerController() == this)
  {
   Rpc(BCMM_StockObservationRequest, phase, ownerRetained);
  }
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_StockObservationRequest(int phase, bool ownerRetained)
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RecordStockObservation(this, phase, ownerRetained);
 }
}
class BCMM_StockCommitNetworkChecks : BCMM_NativeConsumableCommitChecks
{
 protected int m_NetworkWait;
 protected bool m_SuccessUse;
 override protected void Prepare()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!player || !player.GetControlledEntity())
  {
   if (m_OwnerWait++ < 120)
   {
    GetGame().GetCallqueue().CallLater(Prepare, 250);
    return;
   }
   m_Suite.Record(false, "Native stock network fixture receives its actual owner");
   Finish();
   return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Actor = player.GetControlledEntity();
  m_Actor.SetOrigin("100 1.2 0");
  super.Prepare();
 }
 override protected void BeginStock()
 {
  m_StockConsumable.SetTargetCharacter(m_Target);
  m_Mode.BCMM_PublishStock(1, m_Actor, m_Target, m_Kit);
  m_NetworkWait = 0;
  WaitNativeUse();
 }
 protected void WaitNativeUse()
 {
  if (m_StockController.IsUsingItem())
  {
   m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit), "Authority sees actual owner using the original stock injector");
   if (!m_SuccessUse)
   {
    m_Mode.BCMM_TestRoleSetting("medicOnlyMedication", 1);
    m_Suite.Record(!BCMM_NativeTreatmentSafety.MorphineAllowed(m_Target, m_Actor), "Authority revokes native stock dose eligibility during real owner item use");
    m_Mode.BCMM_PublishStock(2, m_Actor, m_Target, m_Kit);
   }
   m_NetworkWait = 0;
   GetGame().GetCallqueue().CallLater(WaitNativeEnd, 100);
   return;
  }
  if (m_NetworkWait++ < 200)
  {
   GetGame().GetCallqueue().CallLater(WaitNativeUse, 100);
   return;
  }
  m_Suite.Record(false, "Authority observes the actual owning client's native using-item state");
  Finish();
 }
 protected void WaitNativeEnd()
 {
  if (m_StockController.IsUsingItem() && m_NetworkWait++ < 200)
  {
   GetGame().GetCallqueue().CallLater(WaitNativeEnd, 100);
   return;
  }
  GetGame().GetCallqueue().CallLater(CheckNetworkCommit, 500);
 }
 protected void CheckNetworkCommit()
 {
  int doses = m_Medical.BCMM_GetPatient().Medication.ActiveDoses();
  PrintFormat("BCMM_STOCK_NETWORK_COMMIT successUse=%1 source=%2 owned=%3 doses=%4", m_SuccessUse, m_Kit != null, m_Kit && m_Inventory.Contains(m_Kit), doses);
  if (!m_SuccessUse)
  {
   m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && doses == 0, "Dedicated rejected stock commit retains original owner item and applies no dose");
   m_Mode.BCMM_PublishStock(3, m_Actor, m_Target, m_Kit);
  }
  else
  {
   m_Suite.Record(!m_Kit && doses == 1, "Dedicated ordinary native success consumes the same original injector and applies exactly one dose");
   m_Mode.BCMM_PublishStock(5, m_Actor, m_Target, null);
  }
  m_NetworkWait = 0;
  WaitNetworkObservers();
 }
 protected void WaitNetworkObservers()
 {
  int phase = 3;
  if (m_SuccessUse)
  {
   phase = 5;
  }
  if (m_Mode.BCMM_StockObserversReady(phase))
  {
   if (m_SuccessUse)
   {
    m_Suite.Record(true, "Actual owner, proxy and post-revocation joiner acknowledge final stock-use policy/phase state");
    Finish();
    return;
   }
   m_Suite.Record(true, "Owner confirms native callback/item retention and all three clients observe revoked policy/phase");
   m_Mode.BCMM_TestRoleSetting("medicOnlyMedication", 0);
   m_Medical.FullHeal();
   array<HitZone> physical = {};
   m_Medical.GetPhysicalHitZones(physical);
   if (physical.Count() > 0)
   {
    physical[0].SetHealth(physical[0].GetMaxHealth() * 0.8);
   }
   m_Medical.BCMM_GetPatient().Medication.Pain = 70;
   m_SuccessUse = true;
   m_NetworkWait = 0;
   m_StockConsumable.SetTargetCharacter(m_Target);
   m_Mode.BCMM_PublishStock(4, m_Actor, m_Target, m_Kit);
   WaitNativeUse();
   return;
  }
  if (m_NetworkWait++ < 240)
  {
   GetGame().GetCallqueue().CallLater(WaitNetworkObservers, 100);
   return;
  }
  m_Suite.Record(false, "All three clients acknowledge the required stock-use phase");
  Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMStockEquippedPhase;
 protected int m_BCMMStockStartedPhase;
 protected int m_BCMMStockObservedPhase;
 protected ref BCMM_StockOwnerEvents m_BCMMStockEvents = new BCMM_StockOwnerEvents();
 protected float m_BCMMStockReadyAt;
 protected SCR_CharacterControllerComponent m_BCMMStockOwnerController;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string observer;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmStockObserver", observer))
  {
   return;
  }
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  if (!mode || !player || !player.GetControlledEntity() || !mode.BCMM_HasSettingsSnapshot())
  {
   return;
  }
  int phase = mode.BCMM_StockPhase();
  IEntity actor = mode.BCMM_StockActor();
  IEntity patient = mode.BCMM_StockPatient();
  IEntity source = mode.BCMM_StockSource();
  if (!actor || !patient || phase < 1)
  {
   return;
  }
  if (player.GetPlayerId() == 1 && (phase == 1 || phase == 4) && source && player.GetControlledEntity() == actor && mode.BCMM_ReadSetting("medicOnlyMedication") == 0)
  {
   RplComponent ownership = RplComponent.Cast(actor.FindComponent(RplComponent));
   if (!ownership || !ownership.IsOwner())
   {
    return;
   }
   SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(actor).GetCharacterController());
   if (m_BCMMStockEquippedPhase != phase && !controller.IsUsingItem())
   {
    vector pose[4];
    actor.GetWorldTransform(pose);
    pose[3] = patient.GetOrigin() + "-1 0.1 0";
    ChimeraCharacter.Cast(actor).Teleport(pose);
    SCR_GadgetManagerComponent.GetGadgetManager(actor).SetGadgetMode(source, EGadgetMode.IN_HAND);
    m_BCMMStockEquippedPhase = phase;
    m_BCMMStockReadyAt = world.GetWorldTime() + 1000;
    if (!m_BCMMStockOwnerController)
    {
     m_BCMMStockOwnerController = controller;
     controller.m_OnItemUseBeganInvoker.Insert(m_BCMMStockEvents.Began);
     controller.m_OnItemUseEndedInvoker.Insert(m_BCMMStockEvents.Ended);
    }
   }
   if (m_BCMMStockStartedPhase != phase && world.GetWorldTime() > m_BCMMStockReadyAt && controller.GetAttachedGadgetAtLeftHandSlot() == source && controller.CanUseItem())
   {
    SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(source.FindComponent(SCR_ConsumableItemComponent));
    SCR_ConsumableEffectBase effect = consumable.GetConsumableEffect();
    if (!effect.CanApplyEffect(patient, actor))
    {
     return;
    }
    consumable.SetTargetCharacter(patient);
    ItemUseParameters parameters = effect.GetAnimationParameters(source, patient);
    parameters.SetCommandID(effect.GetApplyToOtherAnimCmnd(actor));
    parameters.SetMaxAnimLength(effect.GetApplyToOtherDuraction());
    m_BCMMStockStartedPhase = phase;
    m_BCMMStockEvents.Phase = phase;
    if (!effect.ActivateEffect(patient, actor, source, parameters))
    {
     Print("BCMM_STOCK_NETWORK_FAILED owner native activation", LogLevel.ERROR);
    }
   }
  }
  if ((phase == 3 || phase == 5) && m_BCMMStockObservedPhase != phase)
  {
   bool policyReady = mode.BCMM_ReadSetting("medicOnlyMedication") == 0;
   if (phase == 3)
   {
    policyReady = mode.BCMM_ReadSetting("medicOnlyMedication") == 1;
   }
   if (!policyReady || ChimeraCharacter.Cast(actor).GetCharacterController().IsUsingItem())
   {
    return;
   }
   bool ownerRetained;
   if (player.GetPlayerId() == 1)
   {
    InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
    if (phase == 3)
    {
     ownerRetained = source && inventory.Contains(source) && m_BCMMStockEvents.Begins >= 1 && m_BCMMStockEvents.Ends >= 1;
     if (!ownerRetained)
     {
      return;
     }
     Print("BCMM_STOCK_OWNER_RETAINED original=1 dosesBlocked=1");
    }
    else if (source || m_BCMMStockEvents.Begins < 2 || m_BCMMStockEvents.Ends < 2)
    {
     return;
    }
   }
   m_BCMMStockObservedPhase = phase;
   PrintFormat("BCMM_STOCK_NETWORK_OBSERVED %1 player=%2", phase, player.GetPlayerId());
   player.BCMM_ReportStockObservation(phase, ownerRetained);
  }
 }
}
