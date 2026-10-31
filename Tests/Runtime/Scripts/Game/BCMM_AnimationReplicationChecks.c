// Unpublished native owner/proxy/JIP animation acceptance fixture.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMAnimationPhase;
 [RplProp()] protected RplId m_BCMMAnimationActor = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMAnimationPatient = RplId.Invalid();
 void BCMM_PublishAnimationTest(int phase, IEntity actor, IEntity patient)
 {
  m_BCMMAnimationPhase = phase;
  m_BCMMAnimationActor = BCMM_BodyBagComponent.Id(actor);
  m_BCMMAnimationPatient = BCMM_BodyBagComponent.Id(patient);
  Replication.BumpMe();
  PrintFormat("BCMM_ANIM_PHASE %1", phase);
 }
 int BCMM_AnimationPhase() { return m_BCMMAnimationPhase; }
 IEntity BCMM_AnimationActor() { return BCMM_BodyBagComponent.Resolve(m_BCMMAnimationActor); }
 IEntity BCMM_AnimationPatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMAnimationPatient); }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMAnimationRequested;
 protected bool m_BCMMAnimationObserved;
 protected bool m_BCMMAnimationReleased;
 protected float m_BCMMAnimationMin = 1000;
 protected float m_BCMMAnimationMax = -1000;
 protected float m_BCMMAnimationNext;
 protected float m_BCMMAnimationDebugNext;
 protected bool m_BCMMMovementRequested;
 protected float m_BCMMLoopSampleAfter;
 protected bool m_BCMMDisplaced;
 protected bool m_BCMMKeyboardCamera;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string enabled;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmAnimationObserver", enabled) || world.GetWorldTime() < m_BCMMAnimationNext) return;
  m_BCMMAnimationNext = world.GetWorldTime() + 40;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  if (!mode || mode.BCMM_AnimationPhase() < 1) return;
  IEntity actor = mode.BCMM_AnimationActor();
  IEntity patient = mode.BCMM_AnimationPatient();
  if (!actor || !patient) return;
  string keyboardView;
  if (!m_BCMMKeyboardCamera && System.GetCLIParam("bcmmAnimationKeyboard", keyboardView))
  {
   m_BCMMKeyboardCamera = true;
   SCR_PlayerController cameraOwner = SCR_PlayerController.Cast(GetPlayerController());
   CameraBase playerCamera;
   if (cameraOwner) playerCamera = cameraOwner.GetPlayerCamera();
   bool switched;
   if (playerCamera) switched = GetCameraManager().SetCamera(playerCamera);
   PrintFormat("BCMM_ANIM_PLAYER_CAMERA found=%1 switched=%2", playerCamera != null, switched);
  }
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  if (mode.BCMM_AnimationPhase() == 5 && !m_BCMMMovementRequested)
  {
   m_BCMMAnimationRequested = false;
   m_BCMMMovementRequested = true;
  }
  if (mode.BCMM_AnimationPhase() == 6 && player && player.GetPlayerId() == 1)
   controllerMovement(actor);
  if (!m_BCMMAnimationRequested && player && player.GetPlayerId() == 1 && player.GetControlledEntity() == actor && BCMM_TreatmentAccess.CanReach(actor, patient))
  {
   m_BCMMAnimationRequested = true;
   player.BCMM_RequestCPR(patient);
   Print("BCMM_ANIM_OWNER_REQUESTED");
  }
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(actor).GetCharacterController());
  if (player && player.GetPlayerId() == 1 && world.GetWorldTime() >= m_BCMMAnimationDebugNext)
  {
   m_BCMMAnimationDebugNext = world.GetWorldTime() + 2000;
   PrintFormat("BCMM_ANIM_OWNER_STATE lease=%1 loiter=%2 playing=%3 owner=%4 canPlay=%5 reach=%6", player.BCMM_Compressing(patient), controller.IsLoitering(), BCMM_CPRAnimation.IsPlaying(actor), ChimeraCharacter.Cast(actor).GetRplComponent().IsOwner(), controller.CanPlayLoiterAnimation(ELoiteringType.CUSTOM), BCMM_TreatmentAccess.CanReach(actor, patient));
  }
  if (mode.BCMM_AnimationPhase() == 2 && controller.IsLoitering())
  {
   if (m_BCMMLoopSampleAfter == 0) m_BCMMLoopSampleAfter = world.GetWorldTime() + 1500;
   if (world.GetWorldTime() < m_BCMMLoopSampleAfter) return;
   Animation skeleton = actor.GetAnimation();
   vector bone[4];
   if (!skeleton || !skeleton.GetBoneMatrix(skeleton.GetBoneIndex("LeftHand"), bone)) return;
   vector point = bone[3];
   if (point[1] > 0.65) return;
   m_BCMMAnimationMin = Math.Min(m_BCMMAnimationMin, point[1]);
   m_BCMMAnimationMax = Math.Max(m_BCMMAnimationMax, point[1]);
   if (!m_BCMMAnimationObserved && m_BCMMAnimationMin > 0 && m_BCMMAnimationMin < 0.7 && m_BCMMAnimationMax - m_BCMMAnimationMin > 0.005)
   {
    m_BCMMAnimationObserved = true;
    PrintFormat("BCMM_ANIM_SKELETON_OBSERVED min=%1 max=%2", m_BCMMAnimationMin, m_BCMMAnimationMax);
   }
  }
  if (mode.BCMM_AnimationPhase() >= 4 && !controller.IsLoitering() && !m_BCMMAnimationReleased)
  {
   m_BCMMAnimationReleased = true;
   Print("BCMM_ANIM_RELEASE_OBSERVED");
  }
 }
 protected void controllerMovement(IEntity actor)
 {
  if (m_BCMMDisplaced) return;
  m_BCMMDisplaced = true;
  string keyboard;
  if (System.GetCLIParam("bcmmAnimationKeyboard", keyboard))
  {
   InputBinding binding = GetGame().GetInputManager().CreateUserBinding();
   array<string> keys = {};
   binding.GetBindings("CharacterForward", keys, EInputDeviceType.KEYBOARD, "forward", false);
   PrintFormat("BCMM_ANIM_FORWARD_BINDINGS count=%1 keys=%2", keys.Count(), keys);
   Print("BCMM_ANIM_KEYBOARD_READY");
   return;
  }
  actor.SetOrigin(actor.GetOrigin() + "3 0 0");
  Print("BCMM_ANIM_OWNER_DISPLACED");
 }

}
modded class SCR_CharacterControllerComponent
{
 protected bool m_BCMMControlStreamLogged;
 protected float m_BCMMControlLogNext;
 override void OnPrepareControls(IEntity owner, ActionManager am, float dt, bool player)
 {
  super.OnPrepareControls(owner, am, dt, player);
  string keyboard;
  if (!player || !System.GetCLIParam("bcmmAnimationKeyboard", keyboard)) return;
  if (!m_BCMMControlStreamLogged)
  {
   m_BCMMControlStreamLogged = true;
   Print("BCMM_ANIM_NATIVE_CONTROL_STREAM");
  }
  if (GetGame().GetWorld().GetWorldTime() >= m_BCMMControlLogNext)
  {
   m_BCMMControlLogNext = GetGame().GetWorld().GetWorldTime() + 2000;
   PrintFormat("BCMM_ANIM_CONTROL_TICK loiter=%1 forward=%2 right=%3", IsLoitering(), am.GetActionValue("CharacterForward"), am.GetActionValue("CharacterRight"));
  }
  if (Math.AbsFloat(am.GetActionValue("CharacterForward")) > 0.1 || Math.AbsFloat(am.GetActionValue("CharacterRight")) > 0.1) Print("BCMM_ANIM_NATIVE_MOVEMENT_INPUT");
 }
}
