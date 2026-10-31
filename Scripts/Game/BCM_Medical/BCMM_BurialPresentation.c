// Native held-tool animation runs on the original performing controller owner.
// Cancellation carries its death token so delayed input cannot cancel a new burial.
modded class SCR_PlayerController
{
 protected int m_BCMMBurialAnimationToken;
 protected SCR_ChimeraCharacter m_BCMMBurialAnimationActor;

 void BCMM_SetBurialAnimation(int token, bool active, int durationMilliseconds = 0)
 {
  if (!Replication.IsServer() || token <= 0) return;
  if (GetGame().GetPlayerController() == this)
   BCMM_ReceiveBurialAnimation(token, active, durationMilliseconds);
  else
   Rpc(BCMM_ReceiveBurialAnimation, token, active, durationMilliseconds);
 }

 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveBurialAnimation(int token, bool active, int durationMilliseconds)
 {
  if (!active)
  {
   if (m_BCMMBurialAnimationToken == token) BCMM_StopBurialAnimation();
   return;
  }
  BCMM_StopBurialAnimation();
  SCR_ChimeraCharacter actor = SCR_ChimeraCharacter.Cast(GetControlledEntity());
  IEntity tool = BCMM_BurialSite.HeldTool(actor);
  SCR_CharacterControllerComponent controller;
  if (actor) controller = SCR_CharacterControllerComponent.Cast(actor.GetCharacterController());
  CharacterAnimationComponent animation;
  if (controller) animation = controller.GetAnimationComponent();
  if (!tool || !controller || !animation)
  {
   Rpc(BCMM_CancelBurialAnimationServer, token);
   return;
  }
  ItemUseParameters parameters = new ItemUseParameters();
  parameters.SetEntity(tool);
  parameters.SetAllowMovementDuringAction(false);
  parameters.SetKeepInHandAfterSuccess(true);
  parameters.SetCommandID(animation.BindCommand("CMD_Item_Action"));
  parameters.SetCommandIntArg(1); // Native entrenching gadget's digging action.
  m_BCMMBurialAnimationToken = token;
  m_BCMMBurialAnimationActor = actor;
  controller.m_OnItemUseEndedInvoker.Insert(BCMM_BurialAnimationEnded);
  if (!controller.TryUseItemOverrideParams(parameters))
  {
   BCMM_StopBurialAnimation();
   Rpc(BCMM_CancelBurialAnimationServer, token);
   return;
  }
  GetGame().GetCallqueue().CallLater(BCMM_BurialAnimationExpired, Math.Max(1000, durationMilliseconds + 2000), false);
  PrintFormat("BCMM_BURIAL_ANIMATION started token=%1 duration=%2", token, durationMilliseconds);
 }

 protected void BCMM_BurialAnimationEnded(IEntity item, bool successful, ItemUseParameters parameters)
 {
  int token = m_BCMMBurialAnimationToken;
  if (token <= 0) return;
  // Clear and detach before finishing native item use, which can invoke again.
  BCMM_StopBurialAnimation();
  Rpc(BCMM_CancelBurialAnimationServer, token);
 }

 protected void BCMM_BurialAnimationExpired()
 {
  int token = m_BCMMBurialAnimationToken;
  BCMM_StopBurialAnimation();
  if (token > 0) Rpc(BCMM_CancelBurialAnimationServer, token);
 }

 protected void BCMM_StopBurialAnimation()
 {
  GetGame().GetCallqueue().Remove(BCMM_BurialAnimationExpired);
  SCR_CharacterControllerComponent controller;
  if (m_BCMMBurialAnimationActor)
   controller = SCR_CharacterControllerComponent.Cast(m_BCMMBurialAnimationActor.GetCharacterController());
  int token = m_BCMMBurialAnimationToken;
  m_BCMMBurialAnimationToken = 0;
  m_BCMMBurialAnimationActor = null;
  if (controller)
  {
   controller.m_OnItemUseEndedInvoker.Remove(BCMM_BurialAnimationEnded);
   CharacterAnimationComponent animation = controller.GetAnimationComponent();
   CharacterCommandHandlerComponent commands;
   if (animation) commands = animation.GetCommandHandler();
   if (token > 0 && commands) commands.FinishItemUse(true);
  }
  if (token > 0) PrintFormat("BCMM_BURIAL_ANIMATION stopped token=%1", token);
 }

 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_CancelBurialAnimationServer(int token)
 {
  BCMM_BurialOperation.CancelForPerformer(GetControlledEntity(), token);
 }
}
