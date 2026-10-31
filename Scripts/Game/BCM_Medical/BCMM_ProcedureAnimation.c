// Native game treatment animation, with an original no-heal/no-delete effect.
// Only the server's reserved procedure may commit the real treatment.
[BaseContainerProps()]
class BCMM_ProcedurePresentationEffect : SCR_ConsumableEffectHealthItems
{
 void BCMM_ProcedurePresentationEffect() { m_eConsumableType=SCR_EConsumableType.BANDAGE; }
 // This effect only presents a reserved BCM procedure; native wound predicates
 // must not require an unrelated injured limb before equipment can animate.
 override bool CanApplyEffectToHZ(notnull IEntity target,notnull IEntity user,ECharacterHitZoneGroup group,out SCR_EConsumableFailReason failReason=SCR_EConsumableFailReason.NONE)
 {
  return CanApplyEffect(target,user,failReason);
 }
 override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
 {
  if (Replication.IsServer()) BCMM_SplintTreatment.AnimationCompleted(user, target, item);
 }
 override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
 {
  return BCMM_TreatmentAccess.CanReach(user, target);
 }
}
[BaseContainerProps()]
class BCMM_InjectionPresentationEffect : BCMM_ProcedurePresentationEffect
{
 void BCMM_InjectionPresentationEffect() { m_eConsumableType=SCR_EConsumableType.MORPHINE; }
}
class BCMM_ProcedureAnimation
{
 protected static ref array<ref BCMM_ProcedureAnimation> s_Pending = {};
 IEntity Actor;
 IEntity Target;
 IEntity Item;
 float Seconds;
 float Deadline;
 int Token;
 int Region;
 string FieldClip;
 bool MovementCancelled;
 static void Stop(IEntity actor, IEntity item)
 {
  for (int i = s_Pending.Count() - 1; i >= 0; i--)
   if (s_Pending[i].Actor == actor && (!item || s_Pending[i].Item == item))
   {
    if (!s_Pending[i].FieldClip.IsEmpty()) BCMM_FieldClips.Stop(s_Pending[i].Actor);
    s_Pending[i].Actor = null;
    s_Pending.Remove(i);
   }
  BCMM_FieldClips.Stop(actor);
  if (!actor || !item) return;
  SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if (manager) manager.SetGadgetMode(item, EGadgetMode.IN_STORAGE);
 }
 static void StopToken(int token)
 {
  for (int i = s_Pending.Count() - 1; i >= 0; i--)
   if (s_Pending[i].Token == token) Stop(s_Pending[i].Actor, s_Pending[i].Item);
 }
 static void RequestCancel(SCR_PlayerController player)
 {
  if (player) player.BCMM_RequestCancelProcedure();
 }
 static bool CancelMovement(IEntity actor, ActionManager actions)
 {
  if (!actor || !actions || BCMM_MedicalMenu.Current()) return false;
  foreach (BCMM_ProcedureAnimation session : s_Pending)
  {
   if (session.Actor != actor || session.FieldClip.IsEmpty() || session.MovementCancelled) continue;
   actions.ActivateContext("CharacterMovementContext");
   if (Math.AbsFloat(actions.GetActionValue("CharacterForward")) <= 0.1 && Math.AbsFloat(actions.GetActionValue("CharacterRight")) <= 0.1) return false;
   session.MovementCancelled = true;
   BCMM_FieldClips.Stop(actor);
   return true;
  }
  return false;
 }
 static void Reset()
 {
  foreach (BCMM_ProcedureAnimation pending : s_Pending) pending.Actor = null;
  s_Pending.Clear();
 }
 static void End(SCR_PlayerController player, IEntity actor, IEntity item, int token)
 {
  Stop(actor, item);
  if (player)
  {
   player.BCMM_BroadcastFieldAnimationStop(BCMM_BodyBagComponent.Id(actor));
   player.BCMM_SendProcedureAnimationStop(token);
  }
 }
 static void Start(IEntity actor, IEntity target, IEntity item, float seconds, int token, int region, string fieldClip = "")
 {
  foreach (BCMM_ProcedureAnimation pending : s_Pending) if (pending.Actor == actor) return;
  BCMM_ProcedureAnimation session = new BCMM_ProcedureAnimation();
  session.Actor = actor; session.Target = target; session.Item = item; session.Seconds = seconds; session.Token = token; session.Region = region;
  session.FieldClip = fieldClip;
  session.Deadline = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("procedurePreparationSeconds");
  s_Pending.Insert(session);
  if (!fieldClip.IsEmpty())
  {
   GetGame().GetCallqueue().CallLater(session.Update, 100);
   return;
  }
  SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
  if (!consumable) { s_Pending.RemoveItem(session); return; }
  consumable.SetTargetCharacter(target);
  SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if (manager) manager.SetGadgetMode(item, EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(session.Update, 100);
 }
 protected void Update()
 {
  if (!Actor || !Target || !Item || !BCMM_TreatmentAccess.CanReach(Actor, Target)) { s_Pending.RemoveItem(this); return; }
  if (!FieldClip.IsEmpty())
  {
   if (!BCMM_FieldClips.Supported(FieldClip,Actor,Target)) { Stop(Actor, Item); return; }
   ResourceName graph = BCMM_FieldClips.Graph(FieldClip);
   if (BCMM_CPRAnimation.IsPlaying(Actor, graph)) return;
   if (GetGame().GetWorld().GetWorldTime() * 0.001 >= Deadline) { Stop(Actor, Item); return; }
   bool started = BCMM_CPRAnimation.Start(Actor, Target, graph, BCMM_FieldClips.Instance(FieldClip));
   string diagnostic;
   if (System.GetCLIParam("bcmmFieldProceduresRuntime", diagnostic)) PrintFormat("BCMM_FIELD_PREPARE kind=%1 started=%2 reachable=%3 playing=%4", FieldClip, started, BCMM_TreatmentAccess.CanReach(Actor, Target), BCMM_CPRAnimation.IsPlaying(Actor, graph));
   GetGame().GetCallqueue().CallLater(Update, 100);
   return;
  }
  CharacterControllerComponent controller = ChimeraCharacter.Cast(Actor).GetCharacterController();
  if (controller.GetAttachedGadgetAtLeftHandSlot() == Item && controller.CanUseItem() && !controller.IsChangingItem())
  {
   SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(Item.FindComponent(SCR_ConsumableItemComponent));
   BCMM_ProcedurePresentationEffect effect = BCMM_ProcedurePresentationEffect.Cast(consumable.GetConsumableEffect());
   if (effect)
   {
    ItemUseParameters params = effect.GetAnimationParameters(Item, Target);
    if (Region >= 0 && Region <= 6)
    {
     SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Target.FindComponent(SCR_CharacterDamageManagerComponent));
     params.SetIntParam(medical.FindAssociatedBandagingBodyPart(BCMM_ClinicalChecks.NativeGroup(Region)));
    }
    params.SetCommandID(effect.GetApplyToSelfAnimCmnd(Actor));
    if (Actor != Target)
    {
     params.SetCommandID(effect.GetApplyToOtherAnimCmnd(Actor));
     if (ChimeraCharacter.Cast(Target).GetCharacterController().IsUnconscious()) params.SetCommandID(effect.GetReviveAnimCmnd(Actor));
    }
    params.SetMaxAnimLength(Seconds);
    params.SetAllowMovementDuringAction(false);
    if (effect.ActivateEffect(Target, Actor, Item, params))
    {
     PrintFormat("BCMM_PROCEDURE_ANIMATION started token=%1 seconds=%2", Token, Seconds);
     s_Pending.RemoveItem(this);
     return;
    }
   }
  }
  if (GetGame().GetWorld().GetWorldTime() * 0.001 >= Deadline) { s_Pending.RemoveItem(this); return; }
  GetGame().GetCallqueue().CallLater(Update, 100);
 }
 static void Dispatch(SCR_PlayerController player, IEntity actor, IEntity target, IEntity item, float seconds, int token, int region, string fieldClip = "")
 {
  if (!fieldClip.IsEmpty())
  {
   if (player) player.BCMM_SendProcedureAnimation(target, item, seconds, token, region, fieldClip);
   else Start(actor, target, item, seconds, token, region, fieldClip);
   return;
  }
  SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
  if (consumable) consumable.SetTargetCharacter(target);
  SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if (manager) manager.SetGadgetMode(item, EGadgetMode.IN_HAND);
  if (player) player.BCMM_SendProcedureAnimation(target, item, seconds, token, region, fieldClip);
  else Start(actor, target, item, seconds, token, region, fieldClip);
 }
}
modded class SCR_PlayerController
{
 protected RplId m_BCMMAnimationActor;
 protected RplId m_BCMMAnimationTarget;
 protected RplId m_BCMMAnimationItem;
 protected float m_BCMMAnimationSeconds;
 protected float m_BCMMAnimationDeadline;
 protected int m_BCMMAnimationToken;
 protected int m_BCMMAnimationRegion;
 protected int m_BCMMAnimationCancelled;
 protected string m_BCMMFieldClip;
 void BCMM_SendProcedureAnimation(IEntity target, IEntity item, float seconds, int token, int region, string fieldClip = "")
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveProcedureAnimation(BCMM_BodyBagComponent.Id(target), BCMM_BodyBagComponent.Id(item), seconds, token, region, fieldClip, BCMM_BodyBagComponent.Id(GetControlledEntity()));
  else Rpc(BCMM_ReceiveProcedureAnimation, BCMM_BodyBagComponent.Id(target), BCMM_BodyBagComponent.Id(item), seconds, token, region, fieldClip, BCMM_BodyBagComponent.Id(GetControlledEntity()));
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveProcedureAnimation(RplId target, RplId item, float seconds, int token, int region, string fieldClip, RplId actor)
 {
  if (token <= m_BCMMAnimationToken || token <= m_BCMMAnimationCancelled) return;
  string diagnostic; if(System.GetCLIParam("bcmmEquipmentNetwork",diagnostic)) { PrintFormat("BCMM_ANIMATION_RECEIVED actor=%1 item=%2 target=%3 token=%4",actor,item,target,token); }
  m_BCMMAnimationActor = actor;
  m_BCMMFieldClip = fieldClip;
  m_BCMMAnimationToken = token;
  m_BCMMAnimationTarget = target; m_BCMMAnimationItem = item; m_BCMMAnimationSeconds = seconds; m_BCMMAnimationRegion = region;
  m_BCMMAnimationDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("procedurePreparationSeconds");
  BCMM_RetryProcedureAnimation();
 }
 protected void BCMM_RetryProcedureAnimation()
 {
  if (m_BCMMAnimationToken <= m_BCMMAnimationCancelled) return;
  IEntity actor = BCMM_BodyBagComponent.Resolve(m_BCMMAnimationActor);
  if (!actor || actor != GetControlledEntity()) return;
  IEntity target = BCMM_BodyBagComponent.Resolve(m_BCMMAnimationTarget);
  IEntity item = BCMM_BodyBagComponent.Resolve(m_BCMMAnimationItem);
  if ((!target || !item) && GetGame().GetWorld().GetWorldTime() * 0.001 < m_BCMMAnimationDeadline)
  { GetGame().GetCallqueue().CallLater(BCMM_RetryProcedureAnimation, 100); return; }
  string diagnostic; if(System.GetCLIParam("bcmmEquipmentNetwork",diagnostic)) { PrintFormat("BCMM_ANIMATION_RESOLVED actor=%1 target=%2 item=%3 token=%4",actor!=null,target!=null,item!=null,m_BCMMAnimationToken); }
  if (target && item) BCMM_ProcedureAnimation.Start(actor, target, item, m_BCMMAnimationSeconds, m_BCMMAnimationToken, m_BCMMAnimationRegion, m_BCMMFieldClip);
 }
 void BCMM_UpdateFieldMovement()
 {
  if (GetGame().GetPlayerController() == this && BCMM_ProcedureAnimation.CancelMovement(GetControlledEntity(), GetActionManager())) BCMM_ProcedureAnimation.RequestCancel(this);
 }
 void BCMM_BroadcastFieldAnimationStop(RplId actor)
 {
  if (Replication.IsServer()) Rpc(BCMM_ReceiveFieldAnimationStop, actor);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
 protected void BCMM_ReceiveFieldAnimationStop(RplId actor)
 {
  BCMM_FieldClips.Stop(BCMM_BodyBagComponent.Resolve(actor));
 }
 void BCMM_SendProcedureAnimationStop(int token)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveProcedureAnimationStop(token);
  else Rpc(BCMM_ReceiveProcedureAnimationStop, token);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveProcedureAnimationStop(int token)
 {
  if (token < m_BCMMAnimationCancelled) return;
  m_BCMMAnimationCancelled = token;
  if (token != m_BCMMAnimationToken) return;
  BCMM_ProcedureAnimation.StopToken(token);
  BCMM_ProcedureAnimation.Stop(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(m_BCMMAnimationItem));
 }
}
