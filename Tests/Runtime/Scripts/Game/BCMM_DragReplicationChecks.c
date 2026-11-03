// Test-only observers verify actual bag position, ownership and release on peers.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMDragTestPhase;
 [RplProp()] protected RplId m_BCMMDragTestActor = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMDragTestBag = RplId.Invalid();
 [RplProp()] protected vector m_BCMMDragTestPosition;
 [RplProp()] protected bool m_BCMMDragAcquireAllowed;
 bool BCMM_DragAcquireAllowed() { return m_BCMMDragAcquireAllowed; }
 void BCMM_AllowDragAcquire() { m_BCMMDragAcquireAllowed = true; Replication.BumpMe(); }
 void BCMM_PublishDragTest(int phase, IEntity actor, IEntity bag)
 {
  if (!Replication.IsServer()) return;
  m_BCMMDragTestActor = BCMM_BodyBagComponent.Id(actor);
  m_BCMMDragTestBag = BCMM_BodyBagComponent.Id(bag);
  m_BCMMDragTestPosition = bag.GetOrigin();
  m_BCMMDragTestPhase = phase;
  m_BCMMDragAcquireAllowed = false;
  Replication.BumpMe();
  PrintFormat("BCMM_DRAG_NET_PHASE %1", phase);
  GetGame().GetCallqueue().Remove(BCMM_RefreshDragPosition);
  GetGame().GetCallqueue().CallLater(BCMM_RefreshDragPosition, 250);
 }
 protected void BCMM_RefreshDragPosition()
 {
  if (m_BCMMDragTestPhase <= 0) return;
  IEntity bag = BCMM_BodyBagComponent.Resolve(m_BCMMDragTestBag);
  if (!bag) return;
  m_BCMMDragTestPosition = bag.GetOrigin();
  Replication.BumpMe();
  GetGame().GetCallqueue().CallLater(BCMM_RefreshDragPosition, 250);
 }
 int BCMM_DragTestPhase() { return m_BCMMDragTestPhase; }
 RplId BCMM_DragTestActor() { return m_BCMMDragTestActor; }
 RplId BCMM_DragTestBag() { return m_BCMMDragTestBag; }
 vector BCMM_DragTestPosition() { return m_BCMMDragTestPosition; }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMObservedDragPhase;
 protected float m_BCMMNextDragObservation;
 protected float m_BCMMNextOwnerDragRequest;
 protected float m_BCMMOwnerReleaseAt;
 protected bool m_BCMMOwnerReleased;
 protected float m_BCMMNextAcquireTrace;
 protected int m_BCMMPreparedAcquirePhase;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string enabled;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmDragObserver", enabled) || world.GetWorldTime() < m_BCMMNextDragObservation) return;
  m_BCMMNextDragObservation = world.GetWorldTime() + 250;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  if (!mode || mode.BCMM_DragTestPhase() <= 0) return;
  IEntity entity = BCMM_BodyBagComponent.Resolve(mode.BCMM_DragTestBag());
  if (!entity) return;
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(entity.FindComponent(BCMM_BodyBagComponent));
  if (!bag) return;
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  float now = world.GetWorldTime() * 0.001;
  if (player && player.GetPlayerId() == 1)
  {
   // Character movement belongs to this client. A server-only SetOrigin can
   // be overwritten by its next movement update under delayed replication.
   if ((mode.BCMM_DragTestPhase() == 2 || mode.BCMM_DragTestPhase() == 5) && mode.BCMM_DragAcquireAllowed() && m_BCMMPreparedAcquirePhase != mode.BCMM_DragTestPhase() && player.GetControlledEntity())
   {
    m_BCMMPreparedAcquirePhase = mode.BCMM_DragTestPhase();
    vector offset = "1.2 0.2 0";
    if (m_BCMMPreparedAcquirePhase == 5) offset = "0 0.2 1.2";
    player.GetControlledEntity().SetOrigin(entity.GetOrigin() + offset);
    m_BCMMNextOwnerDragRequest = now + 1;
    PrintFormat("BCMM_DRAG_OWNER_POSITIONED phase=%1", m_BCMMPreparedAcquirePhase);
   }
   if ((mode.BCMM_DragTestPhase() == 2 || mode.BCMM_DragTestPhase() == 5) && now >= m_BCMMNextAcquireTrace && player.GetControlledEntity())
   {
    m_BCMMNextAcquireTrace = now + 1;
    PrintFormat("BCMM_DRAG_ACQUIRE_TRACE phase=%1 allowed=%2 distance=%3 mode=%4 actor=%5", mode.BCMM_DragTestPhase(), mode.BCMM_DragAcquireAllowed(), vector.Distance(player.GetControlledEntity().GetOrigin(), entity.GetOrigin()), bag.DragMode(), player.GetControlledEntity().GetOrigin());
   }
   if (mode.BCMM_DragTestPhase() == 3 && m_BCMMOwnerReleaseAt == 0 && bag.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG && bag.DraggingActor() == BCMM_BodyBagComponent.Id(player.GetControlledEntity()))
    m_BCMMOwnerReleaseAt = now + 3;
   if ((mode.BCMM_DragTestPhase() == 2 || mode.BCMM_DragTestPhase() == 5) && mode.BCMM_DragAcquireAllowed() && now >= m_BCMMNextOwnerDragRequest && player.GetControlledEntity() && vector.Distance(player.GetControlledEntity().GetOrigin(), entity.GetOrigin()) <= 2 && bag.DraggingActor() == RplId.Invalid())
   {
    m_BCMMNextOwnerDragRequest = now + 1;
    player.BCMM_RequestBagDrag(entity, false, false);
   }
   if (mode.BCMM_DragTestPhase() == 3 && m_BCMMOwnerReleaseAt > 0 && now >= m_BCMMOwnerReleaseAt && !m_BCMMOwnerReleased)
   {
    m_BCMMOwnerReleased = true;
    Print("BCMM_DRAG_NET_OWNER_RELEASE_REQUEST");
    player.BCMM_RequestBagDrag(entity, false, true);
   }
  }
  if (mode.BCMM_DragTestPhase() <= m_BCMMObservedDragPhase) return;
  if (mode.BCMM_DragTestPhase() == 1 || mode.BCMM_DragTestPhase() == 3 || mode.BCMM_DragTestPhase() == 6 || mode.BCMM_DragTestPhase() == 7)
  {
   BCMM_TransportMode expected = BCMM_TransportMode.BODY_BAG_ROPE_DRAG;
   if (mode.BCMM_DragTestPhase() != 1) expected = BCMM_TransportMode.BODY_BAG_MANUAL_DRAG;
   if (bag.DragMode() != expected || bag.DraggingActor() != mode.BCMM_DragTestActor()) return;
  }
  else if (bag.DragMode() != BCMM_TransportMode.NONE || bag.DraggingActor() != RplId.Invalid())
  {
   PrintFormat("BCMM_DRAG_NET_STATE_WAIT phase=%1 mode=%2 actor=%3", mode.BCMM_DragTestPhase(), bag.DragMode(), bag.DraggingActor());
   return;
  }
  float error = vector.Distance(entity.GetOrigin(), mode.BCMM_DragTestPosition());
  if (error > 0.35)
  {
   PrintFormat("BCMM_DRAG_NET_POSITION_WAIT phase=%1 error=%2 actual=%3 authority=%4", mode.BCMM_DragTestPhase(), error, entity.GetOrigin(), mode.BCMM_DragTestPosition());
   return;
  }
  Physics body=entity.GetPhysics();
  int expectedLayer=-1;if(bag.DragMode()!=BCMM_TransportMode.NONE)expectedLayer=EPhysicsLayerPresets.ItemFireView;
  if(!body || body.GetInteractionLayer()!=expectedLayer){Print("BCMM_CHECK FAILED: Replicated bag collision suppression or restoration");return;}
  PrintFormat("BCMM_DRAG_NET_COLLISION phase=%1 mask=%2",mode.BCMM_DragTestPhase(),body.GetInteractionLayer());
  m_BCMMObservedDragPhase = mode.BCMM_DragTestPhase();
  if (player && player.GetPlayerId() == 1 && (m_BCMMObservedDragPhase == 3 || m_BCMMObservedDragPhase == 4))
  {
   ChimeraCharacter carrier = ChimeraCharacter.Cast(player.GetControlledEntity());
   bool jumpAllowed = carrier && carrier.GetCharacterController().CanJumpClimb();
   bool shouldAllow = m_BCMMObservedDragPhase == 4;
   if (jumpAllowed != shouldAllow) Print("BCMM_CHECK FAILED: Replicated manual drag jump/vault policy or release restoration");
   else PrintFormat("BCMM_DRAG_NET_JUMP_POLICY phase=%1 allowed=%2", m_BCMMObservedDragPhase, jumpAllowed);
  }
  PrintFormat("BCMM_DRAG_NET_OBSERVED phase=%1 mode=%2 error=%3 position=%4", m_BCMMObservedDragPhase, bag.DragMode(), error, entity.GetOrigin());
  if (m_BCMMObservedDragPhase == 7 && player && player.GetPlayerId() == 1) Print("BCMM_DRAG_NET_DISCONNECT_OWNER");
 }
}
