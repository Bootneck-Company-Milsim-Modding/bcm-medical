// Test-only observers verify actual bag position, ownership and release on peers.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMDragTestPhase;
 [RplProp()] protected RplId m_BCMMDragTestActor = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMDragTestBag = RplId.Invalid();
 [RplProp()] protected vector m_BCMMDragTestPosition;
 void BCMM_PublishDragTest(int phase, IEntity actor, IEntity bag)
 {
  if (!Replication.IsServer()) return;
  m_BCMMDragTestActor = BCMM_BodyBagComponent.Id(actor);
  m_BCMMDragTestBag = BCMM_BodyBagComponent.Id(bag);
  m_BCMMDragTestPosition = bag.GetOrigin();
  m_BCMMDragTestPhase = phase;
  Replication.BumpMe();
  PrintFormat("BCMM_DRAG_NET_PHASE %1", phase);
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
   if (mode.BCMM_DragTestPhase() == 2 && now >= m_BCMMNextOwnerDragRequest && player.GetControlledEntity() && vector.Distance(player.GetControlledEntity().GetOrigin(), entity.GetOrigin()) <= 2 && bag.DraggingActor() == RplId.Invalid())
   {
    m_BCMMNextOwnerDragRequest = now + 1;
    player.BCMM_RequestBagDrag(entity, false, false);
   }
   if (mode.BCMM_DragTestPhase() == 3 && m_BCMMOwnerReleaseAt > 0 && now >= m_BCMMOwnerReleaseAt && !m_BCMMOwnerReleased)
   {
    m_BCMMOwnerReleased = true;
    player.BCMM_RequestBagDrag(entity, false, true);
   }
  }
  if (mode.BCMM_DragTestPhase() <= m_BCMMObservedDragPhase) return;
  if (mode.BCMM_DragTestPhase() == 1 || mode.BCMM_DragTestPhase() == 3)
  {
   BCMM_TransportMode expected = BCMM_TransportMode.BODY_BAG_ROPE_DRAG;
   if (mode.BCMM_DragTestPhase() == 3) expected = BCMM_TransportMode.BODY_BAG_MANUAL_DRAG;
   if (bag.DragMode() != expected || bag.DraggingActor() != mode.BCMM_DragTestActor()) return;
  }
  else if (bag.DragMode() != BCMM_TransportMode.NONE || bag.DraggingActor() != RplId.Invalid()) return;
  float error = vector.Distance(entity.GetOrigin(), mode.BCMM_DragTestPosition());
  if (error > 0.35) return;
  m_BCMMObservedDragPhase = mode.BCMM_DragTestPhase();
  if (m_BCMMObservedDragPhase == 3) m_BCMMOwnerReleaseAt = now + 3;
  PrintFormat("BCMM_DRAG_NET_OBSERVED phase=%1 mode=%2 error=%3 position=%4", m_BCMMObservedDragPhase, bag.DragMode(), error, entity.GetOrigin());
 }
}