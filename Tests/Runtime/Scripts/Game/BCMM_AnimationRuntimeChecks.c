// Diagnostic addon only. Proves actual skeletal playback, not just resource loading.
class BCMM_AnimationRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected float m_StandingHand;
 protected ResourceName m_Graph = BCMM_CPRAnimation.GRAPH;
 protected ResourceName m_Instance = BCMM_CPRAnimation.INSTANCE;
 protected string m_FieldName;
 protected float m_MinHand = 1000;
 protected float m_MaxHand = -1000;
 protected int m_Samples;
 protected vector m_WorkingMin = "1000 1000 1000";
 protected vector m_WorkingMax = "-1000 -1000 -1000";
 protected float HandHeight()
 {
  Animation animation = m_Actor.GetAnimation();
  vector matrix[4];
  if (!animation || !animation.GetBoneMatrix(animation.GetBoneIndex("LeftHand"), matrix)) return -1000;
  return matrix[3][1];
 }
 protected vector JointPosition(string name)
 {
  Animation bones = m_Actor.GetAnimation();
  vector matrix[4];
  if (!bones || !bones.GetBoneMatrix(bones.GetBoneIndex(name), matrix)) return vector.Zero;
  return matrix[3];
 }
 protected vector PalmCentre(string side)
 {
  return (JointPosition(side + "Hand") + (JointPosition(side + "HandIndex1") + JointPosition(side + "HandPinky1")) * 0.5) * 0.5;
 }
 protected void CheckContactPose()
 {
  vector left = PalmCentre("Left");
  vector right = PalmCentre("Right");
  float heightGap = Math.AbsFloat(left[1] - right[1]);
  m_Suite.Record(vector.DistanceSqXZ(left, right) < 0.0004 && heightGap > 0.015 && heightGap < 0.045, "Native CPR palms overlap with separate hand thickness instead of intersecting");
  vector leftIndex = JointPosition("LeftHandIndex1");
  vector leftPinky = JointPosition("LeftHandPinky1");
  vector rightIndex = JointPosition("RightHandIndex1");
  vector rightPinky = JointPosition("RightHandPinky1");
  m_Suite.Record(Math.AbsFloat(leftIndex[1] - leftPinky[1]) < 0.01 && Math.AbsFloat(rightIndex[1] - rightPinky[1]) < 0.01, "Native CPR knuckles lie in a horizontal palm plane");
  float kneeSpacing = vector.DistanceXZ(JointPosition("LeftKnee"), JointPosition("RightKnee"));
  PrintFormat("BCMM_CPR_CONTACT palmGap=%1 kneeSpacing=%2 left=%3 right=%4", heightGap, kneeSpacing, left, right);
  m_Suite.Record(kneeSpacing > 0.4 && kneeSpacing < 0.65, "Native CPR knees form a wider stable stance without crossing");
 }
 override protected void Prepare()
 {
  string network;
  if (System.GetCLIParam("bcmmAnimationNetwork", network)) { BeginNetwork(); return; }
  if (System.GetCLIParam("bcmmFieldAnimation", m_FieldName))
  {
   m_Graph = BCMM_FieldClips.Graph(m_FieldName);
   m_Instance = BCMM_FieldClips.Instance(m_FieldName);
   m_Suite.Record(!m_Graph.IsEmpty() && !m_Instance.IsEmpty(), "Requested original field animation has an owned resource mapping");
  }
  AIControlComponent control = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));
  if (control) control.DeactivateAI();
  BCMM_AnimationReview.Start(m_Actor);
  m_Start = m_Actor.GetOrigin();
  m_StandingHand = HandHeight();
  m_Suite.Record(Resource.Load(m_Graph).IsValid() && Resource.Load(m_Instance).IsValid(), "Original CPR graph and instance load in native game");
  m_Suite.Record(m_StandingHand > 0.5, "Native standing hand bone can be measured");
  m_Suite.Record(BCMM_CPRAnimation.Start(m_Actor, null, m_Graph, m_Instance), "Original CPR graph starts through native custom animation command");
  GetGame().GetCallqueue().CallLater(CheckLoop, 1800);
 }
 protected int m_NetworkWait;
 protected int m_KeyboardStopWait;
 protected SCR_PlayerController m_Medic;
 protected void BeginNetwork()
 {
  m_Medic = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!m_Medic || !m_Medic.GetControlledEntity())
  {
   if (m_NetworkWait++ < 100) { GetGame().GetCallqueue().CallLater(BeginNetwork, 250); return; }
   m_Suite.Record(false, "Owning CPR player connects"); Finish(); return;
  }
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.BCMM_GetPatient().HasSpontaneousCirculation = false;
  m_Medical.BCMM_GetPatient().Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(1, m_Medic.GetControlledEntity(), m_Target);
  m_NetworkWait = 0;
  GetGame().GetCallqueue().CallLater(CheckNetworkStarted, 250);
 }
 protected void CheckNetworkStarted()
 {
  ChimeraCharacter medic = ChimeraCharacter.Cast(m_Medic.GetControlledEntity());
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(medic.GetCharacterController());
  if ((!controller.IsLoitering() || !m_Medical.BCMM_GetPatient().HasCompressions()) && m_NetworkWait++ < 80)
  { GetGame().GetCallqueue().CallLater(CheckNetworkStarted, 250); return; }
  PrintFormat("BCMM_ANIM_SERVER_STATE loiter=%1 compressions=%2 session=%3 reach=%4 pulse=%5", controller.IsLoitering(), m_Medical.BCMM_GetPatient().HasCompressions(), BCMM_CPRSession.Find(m_Medic) != null, BCMM_TreatmentAccess.CanReach(m_Medic.GetControlledEntity(), m_Target), m_Medical.BCMM_GetPatient().HasSpontaneousCirculation);
  m_Suite.Record(controller.IsLoitering() && m_Medical.BCMM_GetPatient().HasCompressions(), "Actual owning player starts CPR physiology and replicated native animation together");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(2, medic, m_Target);
  GetGame().GetCallqueue().CallLater(StopNetwork, 16000);
 }
 protected void StopNetwork()
 {
  BCMM_CPRSession session = BCMM_CPRSession.Find(m_Medic);
  m_Suite.Record(session != null, "CPR owner lease survives the native animation and JIP observation interval");
  if (session) BCMM_CPRSession.Stop(m_Medic, session.Token);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(3, m_Medic.GetControlledEntity(), m_Target);
  GetGame().GetCallqueue().CallLater(CheckNetworkStopped, 3000);
 }
 protected void CheckNetworkStopped()
 {
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Medic.GetControlledEntity()).GetCharacterController());
  m_Suite.Record(!m_Medical.BCMM_GetPatient().HasCompressions() && !controller.IsLoitering(), "Revoked CPR lease stops physiology and native animation on the server");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(4, m_Medic.GetControlledEntity(), m_Target);
  GetGame().GetCallqueue().CallLater(BeginMovementRound, 2000);
 }
 protected void BeginMovementRound()
 {
  m_NetworkWait = 0;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(5, m_Medic.GetControlledEntity(), m_Target);
  GetGame().GetCallqueue().CallLater(CheckMovementRoundStarted, 250);
 }
 protected void CheckMovementRoundStarted()
 {
  if (!m_Medical.BCMM_GetPatient().HasCompressions() && m_NetworkWait++ < 60)
  { GetGame().GetCallqueue().CallLater(CheckMovementRoundStarted, 250); return; }
  m_Suite.Record(m_Medical.BCMM_GetPatient().HasCompressions(), "CPR can restart after a completed native release");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(6, m_Medic.GetControlledEntity(), m_Target);
  string keyboard;
  int cancellationWait = 5000;
  if (System.GetCLIParam("bcmmAnimationKeyboard", keyboard)) cancellationWait = 250;
  GetGame().GetCallqueue().CallLater(CheckMovementCancelled, cancellationWait);
 }
 protected void CheckMovementCancelled()
 {
  string reason = "Actual owning-client displacement cancels authoritative CPR";
  string keyboard;
  if (System.GetCLIParam("bcmmAnimationKeyboard", keyboard))
  {
   if (BCMM_CPRSession.Find(m_Medic) && m_KeyboardStopWait++ < 2400)
   { GetGame().GetCallqueue().CallLater(CheckMovementCancelled, 250); return; }
   reason = "Real owning-client movement key cancels authoritative CPR";
  }
  m_Suite.Record(!m_Medical.BCMM_GetPatient().HasCompressions() && !BCMM_CPRSession.Find(m_Medic), reason);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAnimationTest(7, m_Medic.GetControlledEntity(), m_Target);
  GetGame().GetCallqueue().CallLater(Finish, 3000);
 }
 protected void CheckLoop()
 {
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  m_Suite.Record(controller.IsLoitering(), "CPR graph remains in native animation command after entry");
  float height = HandHeight();
  PrintFormat("BCMM_CPR_BONE standing=%1 working=%2", m_StandingHand, height);
  m_Suite.Record(height < m_StandingHand - 0.25 && height > 0, "Compiled CPR pose actually lowers native hand bones");
  if (m_FieldName.IsEmpty()) CheckContactPose();
  SampleLoop();
 }
 protected void SampleLoop()
 {
  float height = HandHeight();
  m_MinHand = Math.Min(m_MinHand, height);
  m_MaxHand = Math.Max(m_MaxHand, height);
  vector working = JointPosition("RightHand");
  for (int axis = 0; axis < 3; axis++)
  {
   m_WorkingMin[axis] = Math.Min(m_WorkingMin[axis], working[axis]);
   m_WorkingMax[axis] = Math.Max(m_WorkingMax[axis], working[axis]);
  }
  m_Samples++;
  if (m_Samples < 30) { GetGame().GetCallqueue().CallLater(SampleLoop, 40); return; }
  PrintFormat("BCMM_CPR_MOTION min=%1 max=%2", m_MinHand, m_MaxHand);
  if (m_FieldName.IsEmpty()) m_Suite.Record(m_MaxHand - m_MinHand > 0.005, "Compiled CPR loop repeatedly moves the actual native skeleton");
  else
  {
   float workingTravel = vector.Distance(m_WorkingMin, m_WorkingMax);
   PrintFormat("BCMM_FIELD_MOTION kind=%1 workingHandTravel=%2", m_FieldName, workingTravel);
   m_Suite.Record(workingTravel > 0.005, "Compiled field procedure repeatedly moves the actual working hand in three dimensions");
  }
  m_Suite.Record(vector.Distance(m_Start, m_Actor.GetOrigin()) < 0.2, "In-place CPR does not translate the actor away");
  string review;
  if (System.GetCLIParam("bcmmAnimationReview", review)) { Print("BCMM_CPR_REVIEW_READY"); return; }
  SCR_CharacterControllerComponent nativeController = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  SCR_LoiterCustomAnimData original = nativeController.GetScrInputContext().m_CustomAnimData;
  m_Suite.Record(BCMM_CPRAnimation.IsActive(m_Actor, m_Graph), "Native CPR identity is recognized without relying on a local ownership map");
  nativeController.GetScrInputContext().m_CustomAnimData = SCR_LoiterCustomAnimData.Default;
  m_Suite.Record(!BCMM_CPRAnimation.IsActive(m_Actor, m_Graph), "Another native animation context cannot masquerade as active CPR");
  nativeController.GetScrInputContext().m_CustomAnimData = original;
  BCMM_CPRAnimation.Stop(m_Actor, m_Graph);
  GetGame().GetCallqueue().CallLater(CheckReleasing, 300);
  GetGame().GetCallqueue().CallLater(CheckReleased, 2000);
 }
 protected void CheckReleasing()
 {
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  m_Suite.Record(controller.IsLoitering(), "CPR stop plays the authored exit instead of immediately cutting the graph");
  PrintFormat("BCMM_CPR_RELEASE height=%1 low=%2", HandHeight(), m_MinHand);
  m_Suite.Record(HandHeight() > m_MinHand + 0.025, "Authored CPR exit actually lifts the native hands before control is released");
 }
 protected void CheckReleased()
 {
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  m_Suite.Record(!controller.IsLoitering(), "CPR release returns native controller from the custom animation");
  Finish();
 }
}
class BCMM_AnimationReview
{
 static SCR_ManualCamera Camera;
 static void Start(IEntity actor)
 {
  string review;
  if (!System.GetCLIParam("bcmmAnimationReview", review) && !System.GetCLIParam("bcmmAnimationKeyboard", review)) return;
  vector target = actor.GetOrigin() + "0 0.7 0";
  vector origin = actor.GetOrigin() + "2 1.4 2";
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  Math3D.AnglesToMatrix((target - origin).VectorToAngles(), spawn.Transform);
  spawn.Transform[3] = origin;
  Camera = SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera, GetGame().GetWorld(), spawn));
  GetGame().GetCallqueue().CallLater(Activate, 200);
 }
 protected static void Activate()
 {
  if (!Camera) return;
  Camera.SetInputEnabled(false);
  Camera.SetVerticalFOV(55);
  PrintFormat("BCMM_CPR_REVIEW_CAMERA active=%1", GetGame().GetCameraManager().SetCamera(Camera));
  Camera.ApplyTransform(0);
 }
}
