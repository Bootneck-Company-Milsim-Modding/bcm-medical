// Selected-region native bandaging: the engine owns equip, animation, effect and consumption.
class BCMM_Bandaging
{
 static IEntity FindBandage(IEntity actor)
 {
  if (!actor) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
   if (consumable && SCR_ConsumableBandage.Cast(consumable.GetConsumableEffect())) return item;
  }
  return null;
 }
 static bool CanBegin(IEntity actor, IEntity target, int region)
 {
  if (region < 0 || region > 6 || !BCMM_TreatmentAccess.CanReach(actor, target)) return false;
  ChimeraCharacter user = ChimeraCharacter.Cast(actor);
  if (user.GetCharacterController().IsUsingItem() || BCMM_SplintTreatment.IsBusy(actor, target)) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed()) return false;
  if (!Replication.IsServer()) return true;
  return medical.GetGroupDamageOverTime(BCMM_ClinicalChecks.NativeGroup(region), EDamageType.BLEEDING) > 0;
 }
 static bool Animate(IEntity actor, IEntity target, IEntity item, int region)
 {
  return BCMM_BandageAnimationSession.Begin(actor, target, item, region);
 }
 static bool UseEquipped(IEntity actor, IEntity target, IEntity item, int region)
 {
  if (!CanBegin(actor, target, region) || !item) { Print("BCMM_NATIVE_BANDAGE refused eligibility"); return false; }
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  bool held = ChimeraCharacter.Cast(actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot() == item;
  if (!inventory || (!inventory.Contains(item) && !held)) { Print("BCMM_NATIVE_BANDAGE refused ownership"); return false; }
  SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
  if (!consumable) return false;
  SCR_ConsumableBandage effect = SCR_ConsumableBandage.Cast(consumable.GetConsumableEffect());
  if (!effect) return false;
  consumable.SetTargetCharacter(target);
  ItemUseParameters params = effect.GetAnimationParameters(item, target, BCMM_ClinicalChecks.NativeGroup(region));
  if (!params) { Print("BCMM_NATIVE_BANDAGE missing region animation parameters"); return false; }
  if (actor != target)
  {
   params.SetCommandID(effect.GetApplyToOtherAnimCmnd(actor));
   if (ChimeraCharacter.Cast(target).GetCharacterController().IsUnconscious()) params.SetCommandID(effect.GetReviveAnimCmnd(actor));
  }
  params.SetAllowMovementDuringAction(false);
  params.SetMaxAnimLength(BCMM_MedicRole.Duration(actor, BCMM_TreatmentAccess.Setting("bandageApplicationSeconds")));
  return effect.ActivateEffect(target, actor, item, params);
 }
}
class BCMM_BandageAnimationSession
{
 protected static ref array<ref BCMM_BandageAnimationSession> s_Active = {};
 static void Reset()
 {
  foreach (BCMM_BandageAnimationSession session : s_Active) session.Actor = null;
  s_Active.Clear();
 }
 IEntity Actor;
 IEntity Target;
 IEntity Item;
 int Region;
 float Deadline;
 static bool Begin(IEntity actor, IEntity target, IEntity item, int region)
 {
  if (!item || !BCMM_Bandaging.CanBegin(actor, target, region)) return false;
  foreach (BCMM_BandageAnimationSession pending : s_Active) if (pending.Actor == actor || (pending.Target == target && pending.Region == region)) return false;
  BCMM_BandageAnimationSession session = new BCMM_BandageAnimationSession();
  session.Actor = actor; session.Target = target; session.Item = item; session.Region = region;
  session.Deadline = GetGame().GetWorld().GetWorldTime() * 0.001 + 8;
  s_Active.Insert(session);
  SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if (!manager) { s_Active.RemoveItem(session); return false; }
  manager.SetGadgetMode(item, EGadgetMode.IN_HAND);
  RplComponent actorRpl = RplComponent.Cast(actor.FindComponent(RplComponent));
  PrintFormat("BCMM_NATIVE_BANDAGE actor_owner=%1 proxy=%2", actorRpl && actorRpl.IsOwner(), actorRpl && actorRpl.IsProxy());
  GetGame().GetCallqueue().CallLater(session.TryStart, 100);
  return true;
 }
 protected void TryStart()
 {
  if (!Actor || !Target || !Item || !BCMM_Bandaging.CanBegin(Actor, Target, Region)) { Finish(); return; }
  CharacterControllerComponent controller = ChimeraCharacter.Cast(Actor).GetCharacterController();
  if (controller.GetAttachedGadgetAtLeftHandSlot() == Item && controller.CanUseItem() && !controller.IsChangingItem())
  {
   bool started = BCMM_Bandaging.UseEquipped(Actor, Target, Item, Region);
   if (started)
   {
    PrintFormat("BCMM_NATIVE_BANDAGE animation_started=1 region=%1", Region);
    Finish();
    return;
   }
  }
  if (GetGame().GetWorld().GetWorldTime() * 0.001 >= Deadline) { PrintFormat("BCMM_NATIVE_BANDAGE equip timeout held=%1 ready=%2 changing=%3", controller.GetAttachedGadgetAtLeftHandSlot() == Item, controller.CanUseItem(), controller.IsChangingItem()); Finish(); return; }
  GetGame().GetCallqueue().CallLater(TryStart, 100);
 }
 protected void Finish() { s_Active.RemoveItem(this); }
}
modded class SCR_PlayerController
{
 protected IEntity m_BCMMServerBandageItem;
 protected IEntity m_BCMMServerBandageActor;
 protected float m_BCMMServerBandageDeadline;
 protected bool m_BCMMServerBandageBegan;
 protected void BCMM_MonitorBandage()
 {
  if (!m_BCMMServerBandageItem || GetControlledEntity() != m_BCMMServerBandageActor)
  {
   m_BCMMServerBandageItem = null;
   return;
  }
  CharacterControllerComponent controller = ChimeraCharacter.Cast(m_BCMMServerBandageActor).GetCharacterController();
  bool usingItem = controller.IsUsingItem();
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (usingItem && !m_BCMMServerBandageBegan)
  {
   m_BCMMServerBandageBegan = true;
   m_BCMMServerBandageDeadline = now + BCMM_MedicRole.Duration(m_BCMMServerBandageActor, BCMM_TreatmentAccess.Setting("bandageApplicationSeconds")) + 8;
  }
  if ((m_BCMMServerBandageBegan && !usingItem) || now >= m_BCMMServerBandageDeadline)
  {
   m_BCMMServerBandageItem = null;
   return;
  }
  GetGame().GetCallqueue().CallLater(BCMM_MonitorBandage, 100);
 }
 void BCMM_RequestBandage(IEntity target, int region)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_BandageRequest, BCMM_BodyBagComponent.Id(target), region);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_BandageRequest(RplId targetId, int region)
 {
  if (m_BCMMServerBandageItem) return;
  IEntity actor = GetControlledEntity();
  IEntity target = BCMM_BodyBagComponent.Resolve(targetId);
  IEntity kit = BCMM_Bandaging.FindBandage(actor);
  if (!kit || !BCMM_Bandaging.CanBegin(actor, target, region))
  {
   BCMM_BandageFeedback.Send(this, "Cannot bandage: check selected region, range and a field dressing in your inventory.");
   return;
  }
  m_BCMMServerBandageItem = kit;
  m_BCMMServerBandageActor = actor;
  m_BCMMServerBandageBegan = false;
  m_BCMMServerBandageDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + 8;
  GetGame().GetCallqueue().CallLater(BCMM_MonitorBandage, 100);
  SCR_ConsumableItemComponent serverConsumable = SCR_ConsumableItemComponent.Cast(kit.FindComponent(SCR_ConsumableItemComponent));
  serverConsumable.SetTargetCharacter(target);
  SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if (manager) manager.SetGadgetMode(kit, EGadgetMode.IN_HAND);
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveBandageStart(targetId, BCMM_BodyBagComponent.Id(kit), region);
  else Rpc(BCMM_ReceiveBandageStart, targetId, BCMM_BodyBagComponent.Id(kit), region);
 }
 protected RplId m_BCMMPendingBandageTarget;
 protected RplId m_BCMMPendingBandageKit;
 protected int m_BCMMPendingBandageRegion;
 protected float m_BCMMPendingBandageDeadline;
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveBandageStart(RplId targetId, RplId kitId, int region)
 {
  m_BCMMPendingBandageTarget = targetId;
  m_BCMMPendingBandageKit = kitId;
  m_BCMMPendingBandageRegion = region;
  m_BCMMPendingBandageDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + 8;
  BCMM_RetryBandageStart();
 }
 protected void BCMM_RetryBandageStart()
 {
  IEntity target = BCMM_BodyBagComponent.Resolve(m_BCMMPendingBandageTarget);
  IEntity kit = BCMM_BodyBagComponent.Resolve(m_BCMMPendingBandageKit);
  if ((!kit || !target) && GetGame().GetWorld().GetWorldTime() * 0.001 < m_BCMMPendingBandageDeadline)
  {
   GetGame().GetCallqueue().CallLater(BCMM_RetryBandageStart, 100);
   return;
  }
  bool started = BCMM_Bandaging.Animate(GetControlledEntity(), target, kit, m_BCMMPendingBandageRegion);
  PrintFormat("BCMM_NATIVE_BANDAGE owner_prepared=%1 kit=%2 target=%3", started, kit != null, target != null);
  if (!started)
  {
   SCR_PopUpNotification notification = SCR_PopUpNotification.GetInstance();
   if (notification) notification.PopupMsg("Bandage could not start. Allow inventory to update, then try again.", duration: 5);
  }
 }
}
class BCMM_BandageFeedback
{
 static void Send(SCR_PlayerController player, string text) { player.BCMM_TreatmentFeedback(text); }
}