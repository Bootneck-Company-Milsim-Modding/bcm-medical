// Original authority-checked hands-only interactions. Instrumented diagnosis,
// equipment procedures and the full medical panel are separate pending work.
class BCMM_TreatmentAccess
{
 static float Setting(string key)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) return mode.BCMM_ReadSetting(key);
  BCMM_Settings defaults = new BCMM_Settings();
  return defaults.Get(key);
 }
 static bool CanReach(IEntity actor, IEntity patient)
 {
  if (!actor || !patient) return false;
  if(BCMM_LivingTransport.ForParticipant(actor) || BCMM_LivingTransport.ForParticipant(patient))return false;
  ChimeraCharacter medic = ChimeraCharacter.Cast(actor);
  ChimeraCharacter casualty = ChimeraCharacter.Cast(patient);
  if (!medic || !casualty) return false;
  CharacterControllerComponent controller = medic.GetCharacterController();
  if (!controller || controller.GetLifeState() != ECharacterLifeState.ALIVE || controller.IsUnconscious() || controller.IsSwimming() || controller.IsFalling() || medic.IsInVehicle()) return false;
  if (casualty.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD) return false;
  if (vector.Distance(actor.GetOrigin(), patient.GetOrigin()) > Setting("treatmentRangeMetres")) return false;
  TraceParam trace = new TraceParam();
  trace.Start = medic.EyePosition();
  trace.End = patient.GetOrigin() + "0 0.7 0";
  trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  trace.LayerMask = EPhysicsLayerPresets.Projectile;
  array<IEntity> exclusions = {actor, patient};
  trace.ExcludeArray = exclusions;
  return actor.GetWorld().TraceMove(trace) >= 0.99;
 }
 static bool CanExamine(IEntity actor, IEntity patient)
 {
  if (!actor || !patient) return false;
  ChimeraCharacter medic = ChimeraCharacter.Cast(actor);
  ChimeraCharacter casualty = ChimeraCharacter.Cast(patient);
  if (!medic || !casualty || medic.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || medic.GetCharacterController().IsUnconscious() || casualty.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD) return false;
  if (actor == patient) return true;
  IEntity vehicle = SCR_CompartmentAccessComponent.GetVehicleIn(actor);
  if (vehicle && vehicle == SCR_CompartmentAccessComponent.GetVehicleIn(patient)) return true;
  return CanReach(actor, patient);
 }
 static string Examine(SCR_CharacterDamageManagerComponent medical)
 {
  BCMM_Patient state = medical.BCMM_GetPatient();
  string pulse = "Present";
  if (!state.HasSpontaneousCirculation) pulse = "Absent";
  else if (state.PerfusionFraction < 0.5) pulse = "Weak";
  else if (state.HeartRate > 110) pulse = "Rapid";
  string breathing = "Present";
  if (!state.HasSpontaneousCirculation || state.Airway != BCMM_AirwayCondition.PATENT) breathing = "Absent/ineffective";
  else if (state.LeftChest.CollapseFraction > 0.25 || state.RightChest.CollapseFraction > 0.25 || state.LeftChest.Tension || state.RightChest.Tension) breathing = "Impaired";
  string bleeding = "No visible active bleeding";
  if (state.TotalExternalFlowMlPerSecond() > 0) bleeding = "Active bleeding";
  string legs = "No recorded leg fracture";
  if (!state.LegsAllowJump()) legs = "Suspected leg fracture";
  string report = "Pulse: " + pulse + "\nBreathing: " + breathing + "\n" + bleeding + "\n" + legs;
  array<string> regions = {"Head", "Chest", "Abdomen", "Left arm", "Right arm", "Left leg", "Right leg"};
  for (int region = 0; region < regions.Count(); region++)
  {
   int active;
   int dressed;
   int closed;
   for (int index = 0; index < state.WoundCount(); index++)
   {
    BCMM_Wound wound = state.WoundAt(index);
    if (!wound || wound.BodyZone != region) continue;
    if (wound.Sutured) closed++;
    else if (wound.Bandaged) dressed++;
    else if (wound.FlowMlPerSecond() > 0) active++;
   }
   string findings = "No recorded external wound";
   if (active + dressed + closed > 0) findings = string.Format("%1 bleeding / %2 dressed / %3 closed", active, dressed, closed);
   if (region == BCMM_BodyRegion.LEFT_LEG && state.LeftLeg.Broken)
   {
    if (state.LeftLeg.Splinted) findings += "; fracture supported";
    else findings += "; fracture unsupported";
   }
   if (region == BCMM_BodyRegion.RIGHT_LEG && state.RightLeg.Broken)
   {
    if (state.RightLeg.Splinted) findings += "; fracture supported";
    else findings += "; fracture unsupported";
   }
   report += "\n" + regions[region] + ": " + findings;
  }
  return report;
 }
}
class BCMM_CPRSession
{
 protected static ref array<ref BCMM_CPRSession> s_Sessions = {};
 protected static int s_NextToken;
 protected static float s_NextUpdate;
 int ActorId;
 bool Preparing = true;
 vector StartPosition;
 static void Reset() { s_Sessions.Clear(); s_NextToken = 0; s_NextUpdate = 0; }
 SCR_PlayerController Player;
 IEntity Actor;
 IEntity Target;
 int Token;
 float Deadline;
 static BCMM_CPRSession Find(SCR_PlayerController player)
 {
  foreach (BCMM_CPRSession session : s_Sessions) if (session.Player == player) return session;
  return null;
 }
 static int Begin(SCR_PlayerController player, IEntity target)
 {
  if (!Replication.IsServer() || !player || !target || Find(player)) return 0;
  IEntity actor = player.GetControlledEntity();
  if (actor == target || !BCMM_TreatmentAccess.CanReach(actor, target)) return 0;
  ChimeraCharacter medic = ChimeraCharacter.Cast(actor);
  if (medic.GetCharacterController().IsUsingItem() || medic.GetCharacterController().IsWeaponRaised() || BCMM_SplintTreatment.IsBusy(actor, target)) return 0;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed() || medical.BCMM_GetPatient().HasCompressions() || medical.BCMM_GetPatient().HasSpontaneousCirculation || medical.BCMM_GetPatient().Terminal) return 0;
  foreach (BCMM_CPRSession reserved : s_Sessions) if (reserved.Target == target) return 0;
  int token = ++s_NextToken;
  // Reserve the casualty first; effective CPR begins only after native playback settles.
  BCMM_CPRSession session = new BCMM_CPRSession();
  session.Player = player;
  session.Actor = actor;
  session.StartPosition = actor.GetOrigin();
  session.ActorId = player.GetPlayerId();
  session.Target = target;
  session.Token = token;
  session.Deadline = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("procedurePreparationSeconds");
  s_Sessions.Insert(session);
  return token;
 }
 static bool Renew(SCR_PlayerController player, int token)
 {
  if (!Replication.IsServer()) return false;
  BCMM_CPRSession session = Find(player);
  if (!session || session.Token != token || !session.Valid()) return false;
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (now >= session.Deadline) return false;
  if (session.Preparing)
  {
   SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(session.Actor).GetCharacterController());
   if (!controller.IsLoitering() || controller.GetMovementInput().Length() > 0.1) return true;
   if (!controller.GetScrInputContext().m_CustomAnimData || controller.GetScrInputContext().m_CustomAnimData.m_sGraphName != BCMM_CPRAnimation.GRAPH) return true;
   SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(session.Target.FindComponent(SCR_CharacterDamageManagerComponent));
   if (!medical.BCMM_GetPatient().StartCompressions(session.ActorId, session.Token)) return false;
   session.Preparing = false;
   session.StartPosition = session.Actor.GetOrigin();
  }
  session.Deadline = now + BCMM_TreatmentAccess.Setting("cprLeaseSeconds");
  return true;
 }
 protected bool Valid()
 {
  if (!Player || Player.GetControlledEntity() != Actor || !BCMM_TreatmentAccess.CanReach(Actor, Target)) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Target.FindComponent(SCR_CharacterDamageManagerComponent));
  ChimeraCharacter medic = ChimeraCharacter.Cast(Actor);
  if (!medic || medic.GetCharacterController().IsWeaponRaised() || (!Preparing && medic.GetCharacterController().GetMovementInput().Length() > 0.1)) return false;
  if (!Preparing && !BCMM_CPRAnimation.IsActive(Actor)) return false;
  float tolerance = BCMM_TreatmentAccess.Setting("treatmentMovementToleranceMetres");
  if (!Preparing && vector.DistanceSqXZ(Actor.GetOrigin(), StartPosition) > tolerance * tolerance) return false;
  return medical && !medical.IsDestroyed() && !medical.BCMM_GetPatient().HasSpontaneousCirculation && (Preparing || medical.BCMM_GetPatient().OwnsCompressions(ActorId, Token));
 }
 static void Stop(SCR_PlayerController player, int token)
 {
  if (!Replication.IsServer()) return;
  BCMM_CPRSession session = Find(player);
  if (!session || session.Token != token) return;
  session.End();
 }
 protected void End()
 {
  SCR_CharacterDamageManagerComponent medical;
  if (Target) medical = SCR_CharacterDamageManagerComponent.Cast(Target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (medical) medical.BCMM_GetPatient().StopCompressions(ActorId, Token);
  if (Player)
  {
   Player.BCMM_BroadcastCPRAnimationStop(BCMM_BodyBagComponent.Id(Actor));
   Player.BCMM_SendCPRLease(RplId.Invalid(), 0);
  }
  BCMM_CPRAnimation.Stop(Actor);
  s_Sessions.RemoveItem(this);
 }
 static void Update()
 {
  if (!Replication.IsServer()) return;
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (now < s_NextUpdate) return;
  s_NextUpdate = now + 0.1;
  for (int i = s_Sessions.Count() - 1; i >= 0; i--)
  {
   BCMM_CPRSession session = s_Sessions[i];
   if (now >= session.Deadline || !session.Valid())
   {
    session.End();
   }
  }
 }
}
modded class SCR_PlayerController
{
 protected bool m_BCMMExamPending;
 protected string m_BCMMLastExamination;
 string BCMM_LastExamination() { return m_BCMMLastExamination; }
 protected IEntity m_BCMMExamActor;
 protected IEntity m_BCMMExamTarget;
 protected RplId m_BCMMCPRTarget = RplId.Invalid();
 protected int m_BCMMCPRToken;
 protected float m_BCMMNextCPRHeartbeat;
 protected IEntity m_BCMMCPRAnimationActor;
 protected float m_BCMMCPRAnimationDeadline;
 protected bool m_BCMMCPRAnimationStarted;
 protected bool m_BCMMCPRStopRequested;
 void BCMM_RequestExamination(IEntity target)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_ExamineRequest, BCMM_BodyBagComponent.Id(target));
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_ExamineRequest(RplId target)
 {
  if (m_BCMMExamPending)
  {
   string busy = "An examination is already in progress. Select Examine again shortly.";
   if (GetGame().GetPlayerController() == this) BCMM_ReceiveExamination(target, busy);
   else Rpc(BCMM_ReceiveExamination, target, busy);
   return;
  }
  IEntity patient = BCMM_BodyBagComponent.Resolve(target);
  if (!BCMM_TreatmentAccess.CanExamine(GetControlledEntity(), patient)) return;
  m_BCMMExamPending = true;
  m_BCMMExamActor = GetControlledEntity();
  m_BCMMExamTarget = patient;
  GetGame().GetCallqueue().CallLater(BCMM_FinishExamination, BCMM_TreatmentAccess.Setting("examinationDurationSeconds") * 1000);
 }
 protected void BCMM_FinishExamination()
 {
  m_BCMMExamPending = false;
  IEntity target = m_BCMMExamTarget;
  IEntity actor = m_BCMMExamActor;
  m_BCMMExamTarget = null;
  m_BCMMExamActor = null;
  if (GetControlledEntity() != actor || !BCMM_TreatmentAccess.CanExamine(actor, target)) return;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical) return;
  string summary = BCMM_TreatmentAccess.Examine(medical);
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveExamination(BCMM_BodyBagComponent.Id(target), summary);
  else Rpc(BCMM_ReceiveExamination, BCMM_BodyBagComponent.Id(target), summary);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveExamination(RplId target, string summary)
 {
  m_BCMMLastExamination = summary;
  if (BCMM_MedicalMenu.ReceiveAssessment(target, summary)) return;
  SCR_PopUpNotification notification = SCR_PopUpNotification.GetInstance();
  if (notification) notification.PopupMsg(summary, duration: 10);
 }
 void BCMM_RequestCPR(IEntity target)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_StartCPRRequest, BCMM_BodyBagComponent.Id(target));
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_StartCPRRequest(RplId target)
 {
  int token = BCMM_CPRSession.Begin(this, BCMM_BodyBagComponent.Resolve(target));
  if (token > 0) BCMM_SendCPRLease(target, token);
 }
 void BCMM_BroadcastCPRAnimationStop(RplId actor)
 {
  if (Replication.IsServer()) Rpc(BCMM_ReceiveCPRAnimationStop, actor);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
 protected void BCMM_ReceiveCPRAnimationStop(RplId actor)
 {
  BCMM_CPRAnimation.Stop(BCMM_BodyBagComponent.Resolve(actor));
 }
 void BCMM_SendCPRLease(RplId target, int token)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveCPRLease(target, token);
  else Rpc(BCMM_ReceiveCPRLease, target, token);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveCPRLease(RplId target, int token)
 {
  if (m_BCMMCPRAnimationActor) BCMM_CPRAnimation.Stop(m_BCMMCPRAnimationActor);
  m_BCMMCPRTarget = target;
  m_BCMMCPRToken = token;
  m_BCMMNextCPRHeartbeat = 0;
  m_BCMMCPRAnimationActor = null;
  m_BCMMCPRAnimationStarted = false;
  m_BCMMCPRStopRequested = false;
  if (token > 0)
  {
   m_BCMMCPRAnimationActor = GetControlledEntity();
   m_BCMMCPRAnimationDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("procedurePreparationSeconds");
   BCMM_CPRAnimation.Start(m_BCMMCPRAnimationActor, BCMM_BodyBagComponent.Resolve(m_BCMMCPRTarget));
  }
 }
 bool BCMM_Compressing(IEntity target) { return m_BCMMCPRToken > 0 && m_BCMMCPRTarget == BCMM_BodyBagComponent.Id(target); }
 void BCMM_StopCPR()
 {
  if (GetGame().GetPlayerController() != this || m_BCMMCPRToken <= 0 || m_BCMMCPRStopRequested) return;
  m_BCMMCPRStopRequested = true;
  BCMM_CPRAnimation.Stop(m_BCMMCPRAnimationActor);
  Rpc(BCMM_CPRLeaseRequest, m_BCMMCPRToken, true);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_CPRLeaseRequest(int token, bool stop)
 {
  if (stop) BCMM_CPRSession.Stop(this, token);
  else if (!BCMM_CPRSession.Renew(this, token)) BCMM_CPRSession.Stop(this, token);
 }
 // Character movement belongs to the character's ActionManager. Reading the
 // global UI manager misses actions consumed during native control preparation.
 void BCMM_PrepareCPRControls(IEntity actor, ActionManager actions)
 {
  if (GetGame().GetPlayerController() != this || actor != m_BCMMCPRAnimationActor || m_BCMMCPRToken <= 0 || m_BCMMCPRStopRequested || !actions) return;
  if (BCMM_MedicalMenu.Current()) return;
  actions.ActivateContext("CharacterMovementContext");
  if (Math.AbsFloat(actions.GetActionValue("CharacterForward")) > 0.1 || Math.AbsFloat(actions.GetActionValue("CharacterRight")) > 0.1) BCMM_StopCPR();
 }
 void BCMM_UpdateCPRHeartbeat()
 {
  if (GetGame().GetPlayerController() != this || m_BCMMCPRToken <= 0 || m_BCMMCPRStopRequested) return;
  BCMM_PrepareCPRControls(GetControlledEntity(), GetActionManager());
  if (m_BCMMCPRStopRequested) return;
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (now < m_BCMMNextCPRHeartbeat) return;
  m_BCMMNextCPRHeartbeat = now + 1;
  if (!m_BCMMCPRAnimationActor || GetControlledEntity() != m_BCMMCPRAnimationActor)
  {
   BCMM_StopCPR();
   return;
  }
  bool playing = BCMM_CPRAnimation.IsPlaying(m_BCMMCPRAnimationActor);
  if (playing) m_BCMMCPRAnimationStarted = true;
  else if (m_BCMMCPRAnimationStarted || now > m_BCMMCPRAnimationDeadline)
  {
   BCMM_StopCPR();
   return;
  }
  else BCMM_CPRAnimation.Start(m_BCMMCPRAnimationActor, BCMM_BodyBagComponent.Resolve(m_BCMMCPRTarget));
  Rpc(BCMM_CPRLeaseRequest, m_BCMMCPRToken, false);
 }
}
class BCMM_ExamineAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user) { return BCMM_TreatmentAccess.CanReach(user, GetOwner()); }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) BCMM_MedicalMenu.Open(pOwnerEntity);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_StartCPRAction : BCMM_ExamineAction
{
 override bool CanBeShownScript(IEntity user)
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
  return medical && medical.BCMM_InCardiacArrest() && user != GetOwner() && super.CanBeShownScript(user);
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestCPR(pOwnerEntity);
 }
}
class BCMM_StopCPRAction : BCMM_ExamineAction
{
 override bool CanBeShownScript(IEntity user)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  return player && player.BCMM_Compressing(GetOwner());
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_StopCPR();
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (world.IsEditMode()) return;
  BCMM_CPRSession.Update();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  if (player)
  {
   player.BCMM_UpdateCPRHeartbeat();
   player.BCMM_UpdateFieldMovement();
  }
 }
}
// Observe the same native action stream used to prepare character movement.
modded class SCR_CharacterControllerComponent
{
 override void OnPrepareControls(IEntity owner, ActionManager am, float dt, bool player)
 {
  super.OnPrepareControls(owner, am, dt, player);
  if (!player) return;
  SCR_PlayerController controller = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (controller) controller.BCMM_PrepareCPRControls(owner, am);
 }
}
