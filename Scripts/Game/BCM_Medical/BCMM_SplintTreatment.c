// Original BCM authority-side splint treatment. No native bandage effect is used.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_SplintItemComponentClass : ScriptComponentClass {}
class BCMM_SplintItemComponent : ScriptComponent {}
class BCMM_SplintTreatment : ScriptedInventoryOperationCallback
{
 protected static ref array<ref BCMM_SplintTreatment> s_Active = {};
 protected IEntity m_Actor;
 protected IEntity m_Target;
 protected IEntity m_Kit;
 protected SCR_PlayerController m_Player;
 protected InventoryStorageManagerComponent m_Inventory;
 protected bool m_Left;
 protected bool m_Consuming;
 protected bool m_Finished;
 protected bool m_UsesAnimation;
 protected bool m_AnimationComplete;
 protected bool m_AnimationObserved;
 protected static int s_NextProcedureToken;
 protected int m_ProcedureToken;
 static void Reset()
 {
  foreach (BCMM_SplintTreatment operation : s_Active) operation.m_Finished = true;
  s_Active.Clear();
 }
 protected vector m_StartPosition;
 protected float m_Deadline;
 protected float m_PreparationDeadline;
 protected float m_AnimationEndedAt;
 protected float m_Duration;
 protected string m_FieldClip;
 static IEntity FindKit(IEntity actor)
 {
  if (!actor) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items) if (item && item.FindComponent(BCMM_SplintItemComponent)) return item;
  return null;
 }
 static bool Eligible(IEntity target, bool left)
 {
  if (!target) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed() || medical.BCMM_GetPatient().Terminal) return false;
  BCMM_Fracture leg = medical.BCMM_GetPatient().RightLeg;
  if (left) leg = medical.BCMM_GetPatient().LeftLeg;
  return leg.Broken && !leg.Splinted;
 }
 static bool Begin(SCR_PlayerController player, IEntity target, bool left)
 {
  if (!Replication.IsServer() || !player) return false;
  return BeginForActor(player.GetControlledEntity(), target, left, player);
 }
 // Shared authority entry point; player treatment and isolated clinical fixtures use this path.
 static bool BeginForActor(IEntity actor, IEntity target, bool left, SCR_PlayerController player = null)
 {
  if (!Replication.IsServer()) return false;
  if (player && player.GetControlledEntity() != actor) return false;
  if (!BCMM_TreatmentAccess.CanReach(actor, target) || !Eligible(target, left)) return false;
  IEntity kit = FindKit(actor);
  if (!kit) return false;
  BCMM_SplintTreatment treatment = new BCMM_SplintTreatment();
  treatment.m_Left = left;
  return StartOperation(treatment, actor, target, kit, BCMM_TreatmentAccess.Setting("splintApplicationSeconds"), player);
 }
 static bool IsReservedSupply(IEntity item)
 {
  if(!item)return false;
  if(BCMM_FluidLineSession.UsesSource(item) || BCMM_FluidLineTransfer.SourcePending(item))return true;
  foreach(BCMM_SplintTreatment operation:s_Active)
  {
   if(!operation.m_Finished && operation.m_Kit==item)return true;
  }
  return false;
 }
 // Refunding an enclosing bag must not delete a procedure's reserved source.
 static bool ContainsReservedSupply(IEntity item)
 {
  if(!item)return false;
  array<IEntity> pending={item};
  set<IEntity> visited=new set<IEntity>();
  while(!pending.IsEmpty())
  {
   IEntity current=pending[pending.Count()-1];
   pending.Remove(pending.Count()-1);
   if(!current || visited.Contains(current))continue;
   visited.Insert(current);
   if(IsReservedSupply(current))return true;
   BaseInventoryStorageComponent storage=BaseInventoryStorageComponent.Cast(current.FindComponent(BaseInventoryStorageComponent));
   if(storage)
   {
    array<IEntity> contents={};
    storage.GetAll(contents);
    foreach(IEntity content:contents)pending.Insert(content);
   }
  }
  return false;
 }
 static bool IsBusy(IEntity actor, IEntity patient)
 {
  foreach (BCMM_SplintTreatment operation : s_Active)
   if (operation.m_Actor == actor || operation.m_Target == patient) return true;
  return false;
 }
 protected static bool StartOperation(BCMM_SplintTreatment treatment, IEntity actor, IEntity target, IEntity kit, float seconds, SCR_PlayerController player)
 {
  if(BCMM_LivingTransport.ForParticipant(actor) || BCMM_LivingTransport.ForParticipant(target))return false;
  if (ChimeraCharacter.Cast(actor).GetCharacterController().IsUsingItem()) return false;
  foreach (BCMM_SplintTreatment operation : s_Active)
   if (operation.m_Actor == actor || operation.m_Target == target || (kit && operation.m_Kit == kit)) return false;
  seconds = BCMM_MedicRole.Duration(actor, seconds);
  treatment.m_Actor = actor;
  treatment.m_Target = target;
  treatment.m_Player = player;
  treatment.m_Kit = kit;
  treatment.m_Inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  treatment.m_StartPosition = actor.GetOrigin();
  treatment.m_Deadline = GetGame().GetWorld().GetWorldTime() * 0.001 + seconds;
  treatment.m_Duration = seconds;
  // Snapshot presentation once. Live GM edits must not alter an active procedure.
  treatment.m_UsesAnimation = treatment.RequiresNativeAnimation();
  if (kit && BCMM_TreatmentAccess.Setting("fieldProcedureProgressOnly") > 0.5)
  {
   if (kit.FindComponent(BCMM_SplintItemComponent) || kit.FindComponent(BCMM_SurgeryKitComponent) || kit.FindComponent(BCMM_SutureKitComponent)) treatment.m_UsesAnimation = false;
  }
  if (treatment.m_UsesAnimation) treatment.m_FieldClip = BCMM_FieldClips.Select(actor, target, kit);
  treatment.m_PreparationDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("procedurePreparationSeconds");
  treatment.m_ProcedureToken = ++s_NextProcedureToken;
  s_Active.Insert(treatment);
  if (player) player.BCMM_SendProcedureProgress(treatment.m_ProcedureToken, true, BCMM_TreatmentAccess.Setting("procedurePreparationSeconds"), "Preparing " + treatment.ProcedureName());
  if (player) player.BCMM_TreatmentFeedback(treatment.ProcedureName() + " started. Stay still with your weapon lowered.");
  if(treatment.m_UsesAnimation)BCMM_ProcedureAnimation.Dispatch(player,actor,target,kit,seconds,treatment.m_ProcedureToken,treatment.ProcedureRegion(),treatment.m_FieldClip);
  else
  {
   treatment.m_AnimationObserved=true;treatment.m_AnimationComplete=true;
   if(player)player.BCMM_SendProcedureProgress(treatment.m_ProcedureToken,true,seconds,treatment.ProcedureName());
  }
  GetGame().GetCallqueue().CallLater(treatment.CheckProgress, 100);
  return true;
 }
 protected int ProcedureRegion()
 {
  if (m_Left) return BCMM_BodyRegion.LEFT_LEG;
  return BCMM_BodyRegion.RIGHT_LEG;
 }
 protected string ProcedureName() { return "Splint application"; }
 protected bool StillEligible() { return Eligible(m_Target, m_Left); }
 protected bool RequiresInventorySupply(){return true;}
 protected bool RequiresNativeAnimation(){return true;}
 protected bool Valid()
 {
  if (m_Player && m_Player.GetControlledEntity() != m_Actor) return false;
  if(RequiresInventorySupply() && (!m_Inventory || !m_Kit))return false;
  bool held=m_Kit && ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot()==m_Kit;
  if(RequiresInventorySupply() && !m_Inventory.Contains(m_Kit) && !held)return false;
  if (!BCMM_TreatmentAccess.CanReach(m_Actor, m_Target) || !StillEligible()) return false;
  ChimeraCharacter actor = ChimeraCharacter.Cast(m_Actor);
  if (actor.GetCharacterController().IsWeaponRaised()) return false;
  if (!m_FieldClip.IsEmpty() && !BCMM_FieldClips.Supported(m_FieldClip,m_Actor,m_Target)) return false;
  if (actor.GetCharacterController().IsUsingItem() && !held) return false;
  return vector.Distance(m_Actor.GetOrigin(), m_StartPosition) <= BCMM_TreatmentAccess.Setting("treatmentMovementToleranceMetres");
 }
 protected void CheckProgress()
 {
  if (m_Finished || m_Consuming) return;
  if (!Valid())
  {
   string diagnostic;
   if (System.GetCLIParam("bcmmFieldProceduresRuntime", diagnostic)) PrintFormat("BCMM_FIELD_ABORT kind=%1 kit=%2 inventory=%3 reach=%4 ground=%5 drift=%6 eligible=%7", m_FieldClip, m_Kit != null, m_Inventory && m_Inventory.Contains(m_Kit), BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), BCMM_FieldClips.IsGroundPatient(m_Target), vector.Distance(m_Actor.GetOrigin(), m_StartPosition), StillEligible());
   if (m_Player) m_Player.BCMM_TreatmentFeedback(ProcedureName() + " cancelled; kit retained.");
   Finish();
   return;
  }
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  bool usingItem = ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem();
  if (!m_FieldClip.IsEmpty())
  {
   usingItem = BCMM_CPRAnimation.IsActive(m_Actor, BCMM_FieldClips.Graph(m_FieldClip));
   if (m_AnimationObserved && !usingItem)
   {
    if (m_Player) m_Player.BCMM_TreatmentFeedback(ProcedureName() + " interrupted; kit retained.");
    Finish();
    return;
   }
   // Only authoritative uninterrupted native playback and the saved GM timer
   // can complete this hands-only graph. No client completion event is trusted.
   if (m_AnimationObserved && usingItem && now >= m_Deadline) m_AnimationComplete = true;
  }
  if (usingItem && !m_AnimationObserved)
  {
   m_AnimationObserved = true;
   m_Deadline = now + m_Duration;
   if (m_Player) m_Player.BCMM_SendProcedureProgress(m_ProcedureToken, true, m_Duration, ProcedureName());
  }
  if (usingItem) m_AnimationEndedAt = 0;
  else if (m_AnimationObserved && !m_AnimationComplete && m_AnimationEndedAt <= 0) m_AnimationEndedAt = now;
  float grace = BCMM_TreatmentAccess.Setting("procedureCompletionGraceSeconds");
  if (m_UsesAnimation && ((!m_AnimationObserved && now > m_PreparationDeadline) || (m_AnimationEndedAt > 0 && !m_AnimationComplete && now > m_AnimationEndedAt + grace) || (m_AnimationObserved && now > m_Deadline + grace)))
  {
   if (m_Player) m_Player.BCMM_TreatmentFeedback(ProcedureName() + " animation interrupted; kit retained.");
   Finish();
   return;
  }
  if(m_AnimationObserved && (usingItem || m_AnimationComplete))AdvanceProcedure(Math.Min(now,m_Deadline));
  if (now < m_Deadline || !m_AnimationComplete)
  {
   GetGame().GetCallqueue().CallLater(CheckProgress, 100);
   return;
  }
  m_Consuming = true;
  if (!ConsumesKit()) { ApplyProcedure(); Finish(); return; }
  if (!m_Inventory.TryDeleteItem(m_Kit, this) && !m_Finished) Finish();
 }
 override protected void OnComplete()
 {
  if (m_Finished || !m_Consuming) return;
  bool controlled = !m_Player || m_Player.GetControlledEntity() == m_Actor;
  if (controlled && BCMM_TreatmentAccess.CanReach(m_Actor, m_Target) && StillEligible())
  {
   ChimeraCharacter actor = ChimeraCharacter.Cast(m_Actor);
   if (!actor.GetCharacterController().IsWeaponRaised() && vector.Distance(m_Actor.GetOrigin(), m_StartPosition) <= BCMM_TreatmentAccess.Setting("treatmentMovementToleranceMetres")) ApplyProcedure();
  }
  Finish();
 }
 protected void AdvanceProcedure(float now) {} // Authority-only progress; ordinary procedures remain atomic.
 protected bool ConsumesKit() { return true; }
 protected void ApplyProcedure()
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (m_Left) medical.BCMM_GetPatient().LeftLeg.ApplySplint();
  else medical.BCMM_GetPatient().RightLeg.ApplySplint();
  medical.BCMM_PublishFractures();
  if (m_Player) m_Player.BCMM_TreatmentFeedback("Splint applied. The fracture still requires hospital treatment.");
 }
 override protected void OnFailed() { Finish(); }
 static void CancelFor(SCR_PlayerController player)
 {
  if (!Replication.IsServer() || !player) return;
  for (int i = s_Active.Count() - 1; i >= 0; i--)
  {
   BCMM_SplintTreatment operation = s_Active[i];
   if (operation.m_Player != player || operation.m_Consuming) continue;
   player.BCMM_TreatmentFeedback(operation.ProcedureName() + " cancelled; kit retained.");
   operation.Finish();
  }
 }
 // Authority medical AI may interrupt its own reserved procedure.
 static void CancelActor(IEntity actor)
 {
  if (!Replication.IsServer() || !actor) return;
  for (int i = s_Active.Count() - 1; i >= 0; i--)
   if (s_Active[i].m_Actor == actor && !s_Active[i].m_Consuming) s_Active[i].Finish();
 }
 static void AnimationCompleted(IEntity actor, IEntity target, IEntity item)
 {
  if (!Replication.IsServer()) return;
  foreach (BCMM_SplintTreatment operation : s_Active)
  {
   if (operation.m_FieldClip.IsEmpty() && !operation.m_Finished && operation.m_Actor == actor && operation.m_Target == target && operation.m_Kit == item && operation.Valid())
   {
    operation.m_AnimationComplete = true;
    // A genuine native success may arrive before its using-item snapshot.
    if (!operation.m_AnimationObserved)
    {
     operation.m_AnimationObserved = true;
     operation.m_Deadline = Math.Max(operation.m_Deadline, GetGame().GetWorld().GetWorldTime() * 0.001);
    }
   }
  }
 }
 protected void Finish()
 {
  if (m_Finished) return;
  m_Finished = true;
  if(m_UsesAnimation)BCMM_ProcedureAnimation.End(m_Player,m_Actor,m_Kit,m_ProcedureToken);
  if (m_Player) m_Player.BCMM_SendProcedureProgress(m_ProcedureToken, false, 0, "No active procedure");
  s_Active.RemoveItem(this);
 }
}
modded class SCR_PlayerController
{
 protected int m_BCMMProcedureToken;
 protected bool m_BCMMProcedureActive;
 protected float m_BCMMProcedureDeadline;
 protected float m_BCMMProcedureDuration;
 protected string m_BCMMProcedureName;
 void BCMM_SendProcedureProgress(int token, bool active, float seconds, string name)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveProcedureProgress(token, active, seconds, name);
  else Rpc(BCMM_ReceiveProcedureProgress, token, active, seconds, name);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveProcedureProgress(int token, bool active, float seconds, string name)
 {
  if (token < m_BCMMProcedureToken) return;
  m_BCMMProcedureToken = token;
  m_BCMMProcedureActive = active;
  m_BCMMProcedureDuration = seconds;
  if (!active) BCMM_MedicalMenu.RefreshSupplies();
  m_BCMMProcedureName = name;
  m_BCMMProcedureDeadline = GetGame().GetWorld().GetWorldTime() * 0.001 + seconds;
 }
 bool BCMM_HasProcedure() { return m_BCMMProcedureActive; }
 float BCMM_ProcedureProgress()
 {
  if (!m_BCMMProcedureActive || m_BCMMProcedureDuration <= 0) return 0;
  float remaining = m_BCMMProcedureDeadline - GetGame().GetWorld().GetWorldTime() * 0.001;
  return Math.Clamp(1 - remaining / m_BCMMProcedureDuration, 0, 1);
 }
 string BCMM_ProcedureStatus()
 {
  if (!m_BCMMProcedureActive) return "No active procedure";
  float remaining = Math.Max(0, m_BCMMProcedureDeadline - GetGame().GetWorld().GetWorldTime() * 0.001);
  return "Your treatment: " + m_BCMMProcedureName + " / " + Math.Ceil(remaining).ToString() + " seconds";
 }
 void BCMM_RequestCancelProcedure()
 {
  if (GetGame().GetPlayerController() == this) Rpc(BCMM_CancelProcedureRequest);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_CancelProcedureRequest() { BCMM_SplintTreatment.CancelFor(this); }
 void BCMM_TreatmentFeedback(string message)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveTreatmentFeedback(message);
  else Rpc(BCMM_ReceiveTreatmentFeedback, message);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveTreatmentFeedback(string message)
 {
  SCR_PopUpNotification notification = SCR_PopUpNotification.GetInstance();
  if (notification) notification.PopupMsg(message, duration: 5);
 }
 void BCMM_RequestSplint(IEntity target, bool left)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_SplintRequest, BCMM_BodyBagComponent.Id(target), left);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_SplintRequest(RplId target, bool left)
 {
  if (!BCMM_SplintTreatment.Begin(this, BCMM_BodyBagComponent.Resolve(target), left))
   BCMM_TreatmentFeedback("Cannot apply splint. Check range, an unsupported fracture, your kit and free hands.");
 }
}
class BCMM_SplintLeftAction : ScriptedUserAction
{
 protected bool Left() { return true; }
 override bool CanBeShownScript(IEntity user)
 {
  return BCMM_TreatmentAccess.CanReach(user, GetOwner()) && BCMM_SplintTreatment.Eligible(GetOwner(), Left());
 }
 override bool CanBePerformedScript(IEntity user) { return CanBeShownScript(user) && BCMM_SplintTreatment.FindKit(user); }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestSplint(pOwnerEntity, Left());
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_SplintRightAction : BCMM_SplintLeftAction
{
 override protected bool Left() { return false; }
}
