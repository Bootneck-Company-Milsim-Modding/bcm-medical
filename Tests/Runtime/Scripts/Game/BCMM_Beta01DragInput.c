// Diagnostic native input on the actual owning client. Production gait limits
// remain in charge; do not force prepared movement past OverrideMaxSpeed.
modded class ArmaReforgerScripted
{
 protected float m_BCMMBagWalkAt;
 protected float m_BCMMBagTraceAt;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  string enabled;
  SCR_BaseGameMode traceMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (traceMode && traceMode.BCMM_DragTestPhase() == 6 && world.GetWorldTime() >= m_BCMMBagTraceAt)
  {
   m_BCMMBagTraceAt = world.GetWorldTime() + 100;
   ChimeraCharacter tracedActor = ChimeraCharacter.Cast(BCMM_BodyBagComponent.Resolve(traceMode.BCMM_DragTestActor()));
   if (tracedActor) PrintFormat("BCMM_DRAG_SPEED_TRACE authority=%1 position=%2 velocity=%3 policy=%4", Replication.IsServer(), tracedActor.GetOrigin(), tracedActor.GetCharacterController().GetVelocity(), BCMM_NativeBagDrag.ManualDragging(tracedActor));
  }
  if (!Replication.IsServer() && System.GetCLIParam("bcmmDragObserver", enabled))
  {
   PlayerController local = GetGame().GetPlayerController();
   SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   if (local && local.GetPlayerId() == 1 && local.GetControlledEntity() && mode)
   {
    InputManager actions = GetGame().GetInputManager();
    if (mode.BCMM_DragTestPhase() == 6)
    {
     if (m_BCMMBagWalkAt == 0) { m_BCMMBagWalkAt = world.GetWorldTime(); Print("BCMM_DRAG_OWNER_INPUT_PREPARED"); }
     float speed = 0; if (world.GetWorldTime()-m_BCMMBagWalkAt < 1800) speed = 1;
     actions.ActivateContext("CharacterMovementContext");
     actions.SetActionValue("CharacterForward", speed); actions.SetActionValue("CharacterSprint", 0);
    }
    else if (m_BCMMBagWalkAt > 0) actions.SetActionValue("CharacterForward", 0);
   }
  }
  super.OnUpdate(world, timeslice);
 }
}
