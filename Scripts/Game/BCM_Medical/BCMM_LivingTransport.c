// Original native-character transport with two authorized original casualty poses.
class BCMM_TransportOffer
{
 int Token;int ActorPlayer;int PatientPlayer;int Mode;float Until;
 IEntity Actor;IEntity Patient;
 ref BCMM_CarryConsent Consent=new BCMM_CarryConsent();
}
class BCMM_LivingTransport
{
 protected static ref array<ref BCMM_TransportOffer> s_Offers={};
 protected static int s_NextToken;
 protected static float s_NextCheck;
 static ref array<SCR_CharacterDamageManagerComponent> States={};
 static void Reset(){s_Offers.Clear();s_NextToken=0;s_NextCheck=0;}
 static float Now(){return GetGame().GetWorld().GetWorldTime()*0.001;}
 static SCR_PlayerController Player(int id){return SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));}
 static CharacterControllerComponent Controller(IEntity entity)
 {
  ChimeraCharacter c=ChimeraCharacter.Cast(entity);
  if(!c) { return null; }
  return c.GetCharacterController();
 }
 static SCR_CharacterDamageManagerComponent ForParticipant(IEntity actor)
 {
  if(!actor)return null;
  foreach(SCR_CharacterDamageManagerComponent state:States)
   if(state && state.BCMM_TransportMode()>0 && (state.GetOwner()==actor || state.BCMM_TransportActor()==actor))return state;
  return null;
 }
 static float LimitMovement(IEntity actor,float injuryCap)
 {
  SCR_CharacterDamageManagerComponent transport=ForParticipant(actor);
  if(!transport || transport.BCMM_TransportActor()!=actor)return injuryCap;
  string setting="casualtyCarrySpeedFraction";if(transport.BCMM_TransportMode()==2)setting="casualtyDragSpeedFraction";
  return Math.Min(injuryCap,BCMM_TreatmentAccess.Setting(setting));
 }
 // People and their attached equipment are obstacles, never floor support.
 // Keep these extra exclusions local to the ground probe: the horizontal
 // obstruction trace must still detect other characters.
 static float TraceGround(BaseWorld world,TraceParam probe,array<IEntity> participants)
 {
  array<IEntity> ignored={};ignored.Copy(participants);
  for(int attempt=0;attempt<16;attempt++)
  {
   probe.ExcludeArray=ignored;float fraction=world.TraceMove(probe);
   IEntity hit=probe.TraceEnt;IEntity ancestor=hit;
   while(ancestor && !ChimeraCharacter.Cast(ancestor))ancestor=ancestor.GetParent();
   if(!ancestor)return fraction;
   ignored.Insert(hit);if(hit!=ancestor)ignored.Insert(ancestor);
  }
  return 1;
 }
 static bool Eligible(IEntity actor,IEntity patient,int mode,bool starting=true)
 {
  if(!actor || !patient || actor==patient || (mode!=1 && mode!=2))return false;
  if(BCMM_TreatmentAccess.Setting("casualtyCarryEnabled")<0.5 && mode==1)return false;
  if(BCMM_TreatmentAccess.Setting("casualtyDragEnabled")<0.5 && mode==2)return false;
  ChimeraCharacter a=ChimeraCharacter.Cast(actor);ChimeraCharacter p=ChimeraCharacter.Cast(patient);
  if(!a || !p || a.IsInVehicle() || actor.GetParent())return false;
  if(p.IsInVehicle() || patient.GetParent())
  {
   SCR_CharacterDamageManagerComponent held=ForParticipant(patient);
   if(starting || !held || held.GetOwner()!=patient || !BCMM_PoseAnchor.Contains(held.BCMM_PoseAnchorEntity(),patient))return false;
  }
  CharacterControllerComponent ac=a.GetCharacterController();CharacterControllerComponent pc=p.GetCharacterController();
  if(!ac || !pc || ac.GetLifeState()!=ECharacterLifeState.ALIVE || ac.IsUnconscious() || pc.GetLifeState()==ECharacterLifeState.DEAD)return false;
  if(ac.IsSwimming() || ac.IsFalling() || ac.IsClimbing() || ac.IsUsingItem() || pc.IsUsingItem())return false;
  if(starting)
  {
   if(BCMM_NativeBagDrag.HasActor(actor) || BCMM_NativeBagDrag.HasActor(patient))return false;
   if(ForParticipant(actor) || ForParticipant(patient) || BCMM_StretcherComponent.OnAnotherStretcher(patient) || BCMM_SplintTreatment.IsBusy(actor,patient))return false;
   if(vector.Distance(actor.GetOrigin(),patient.GetOrigin())>BCMM_TreatmentAccess.Setting("casualtyTransportRangeMetres"))return false;
   TraceParam sight=new TraceParam();sight.Start=a.EyePosition();sight.End=p.GetOrigin()+"0 0.6 0";
   sight.Flags=TraceFlags.WORLD|TraceFlags.ENTS;sight.LayerMask=EPhysicsLayerPresets.Projectile;array<IEntity> sightExclusions={actor,patient};sight.ExcludeArray=sightExclusions;
   if(actor.GetWorld().TraceMove(sight)<0.99)return false;
  }
  return true;
 }
 static void Notify(BCMM_TransportOffer offer,bool clear,string message)
 {
  SCR_PlayerController actor=Player(offer.ActorPlayer);SCR_PlayerController patient=Player(offer.PatientPlayer);
  int token=offer.Token;if(clear)token=0;
  if(actor)actor.BCMM_SendTransportOffer(token,BCMM_BodyBagComponent.Id(offer.Patient),offer.Mode,false,Math.Max(0,offer.Until-Now()),message);
  if(patient)patient.BCMM_SendTransportOffer(token,BCMM_BodyBagComponent.Id(offer.Actor),offer.Mode,true,Math.Max(0,offer.Until-Now()),message);
 }
 static void Request(SCR_PlayerController owner,IEntity patient,int mode)
 {
  if(!Replication.IsServer() || !owner)return;
  IEntity actor=owner.GetControlledEntity();
  if(!Eligible(actor,patient,mode)){owner.BCMM_TreatmentFeedback("Cannot transport: check range, line of sight, patient and active treatments.");return;}
  int actorId=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  int patientId=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(patient);
  bool conscious=!Controller(patient).IsUnconscious();
  if(!BCMM_CarryConsent.NeedsConsent(conscious,patientId>0)){Begin(actor,patient,mode);return;}
  foreach(BCMM_TransportOffer pending:s_Offers)
   if(pending.Actor==actor || pending.Patient==patient || pending.Actor==patient || pending.Patient==actor){owner.BCMM_TreatmentFeedback("A transport request is already pending.");return;}
  BCMM_TransportOffer offer=new BCMM_TransportOffer();offer.Token=++s_NextToken;
  offer.ActorPlayer=actorId;offer.PatientPlayer=patientId;offer.Actor=actor;offer.Patient=patient;offer.Mode=mode;
  BCMM_Settings settings=new BCMM_Settings();SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_CopySettings(settings);
  BCMM_TransportMode action=BCMM_TransportMode.CASUALTY_CARRY;if(mode==2)action=BCMM_TransportMode.CASUALTY_DRAG;
  if(!offer.Consent.Request(offer.Token,actorId,patientId,offer.Token,Now(),settings,action))return;
  offer.Until=Now()+settings.Get("carryConsentTimeoutSeconds");s_Offers.Insert(offer);
  Notify(offer,false,"Transport request pending: use the medical panel to accept, decline or cancel.");
 }
 static void Respond(SCR_PlayerController owner,int token,bool accept,bool cancel)
 {
  if(!Replication.IsServer() || !owner || token<=0)return;
  int player=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner.GetControlledEntity());
  for(int i=s_Offers.Count()-1;i>=0;i--)
  {
   BCMM_TransportOffer offer=s_Offers[i];if(offer.Token!=token)continue;
   if(cancel)
   {
    if(!offer.Consent.Cancel(token,player))return;
    Notify(offer,true,"Transport request cancelled.");s_Offers.Remove(i);return;
   }
   if(player!=offer.PatientPlayer || owner.GetControlledEntity()!=offer.Patient)return;
   bool valid=Player(offer.ActorPlayer) && Player(offer.ActorPlayer).GetControlledEntity()==offer.Actor && Now()<offer.Until && Eligible(offer.Actor,offer.Patient,offer.Mode);
   if(!offer.Consent.Respond(token,player,accept && valid,Now()))return;
   BCMM_TransportMode mode=BCMM_TransportMode.CASUALTY_CARRY;if(offer.Mode==2)mode=BCMM_TransportMode.CASUALTY_DRAG;
   bool granted=accept && valid && offer.Consent.Consume(token,offer.ActorPlayer,offer.PatientPlayer,token,Now(),mode);
   Notify(offer,true,"Transport request closed.");s_Offers.Remove(i);
   if(granted)Begin(offer.Actor,offer.Patient,offer.Mode);
   return;
  }
 }
 protected static bool Begin(IEntity actor,IEntity patient,int mode)
 {
  if(!Replication.IsServer() || !Eligible(actor,patient,mode))return false;
  SCR_CharacterDamageManagerComponent state=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!state)return false;
  state.BCMM_BeginTransport(actor,mode);return true;
 }
 static void Release(IEntity actor)
 {
  if(!Replication.IsServer())return;
  SCR_CharacterDamageManagerComponent state=ForParticipant(actor);if(state)state.BCMM_EndTransport();
 }
 static void Update()
 {
  if(Replication.IsServer() && Now()>=s_NextCheck)
  {
   s_NextCheck=Now()+0.1;
   for(int i=s_Offers.Count()-1;i>=0;i--)
   {
    BCMM_TransportOffer offer=s_Offers[i];
    bool valid=Now()<offer.Until && Player(offer.ActorPlayer) && Player(offer.PatientPlayer);
    if(valid)valid=Player(offer.ActorPlayer).GetControlledEntity()==offer.Actor && Player(offer.PatientPlayer).GetControlledEntity()==offer.Patient && Eligible(offer.Actor,offer.Patient,offer.Mode);
    if(!valid){offer.Consent.Revoke();Notify(offer,true,"Transport request expired or is no longer valid.");s_Offers.Remove(i);}
   }
  }
  foreach(SCR_CharacterDamageManagerComponent state:States)if(state && state.GetOwner())state.BCMM_UpdateTransport();
 }
}

modded class SCR_CharacterDamageManagerComponent
{
 [RplProp(onRplName:"BCMM_TransportChanged")] protected RplId m_BCMMCarrier=RplId.Invalid();
 [RplProp(onRplName:"BCMM_TransportChanged")] protected int m_BCMMLivingTransportMode;
 [RplProp(onRplName:"BCMM_TransportChanged")] protected RplId m_BCMMPoseAnchor=RplId.Invalid();
 protected IEntity m_BCMMLocalPoseAnchor;
 protected bool m_BCMMPoseSeated;
 protected float m_BCMMPoseRetry;
 protected vector m_BCMMTransportLastActor;
 protected vector m_BCMMTransportSafeGround;
 protected int m_BCMMTransportActorPlayer;
 protected int m_BCMMTransportPatientPlayer;
 protected bool m_BCMMTransportApplied;
 protected ref BCMM_TransportCollision m_BCMMTransportCollision=new BCMM_TransportCollision();
 protected bool m_BCMMTransportInitiallyConscious;
 protected float m_BCMMTransportPoseUntil;
 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);BCMM_LivingTransport.States.Insert(this);
 }
 override protected void OnDelete(IEntity owner)
 {
  IEntity anchor=BCMM_PoseAnchorEntity();
  IEntity actor=BCMM_TransportActor();
  if(actor)
  {
   SCR_CharacterDamageManagerComponent actorMedical=SCR_CharacterDamageManagerComponent.Cast(actor.FindComponent(SCR_CharacterDamageManagerComponent));
   if(actorMedical)actorMedical.BCMM_ApplyMovement();
  }
  BCMM_RestoreTransportControls();
  if(anchor && Replication.IsServer())GetGame().GetCallqueue().CallLater(BCMM_PoseAnchor.Cleanup,500,false,anchor,owner,0);
  BCMM_LivingTransport.States.RemoveItem(this);super.OnDelete(owner);
 }
 int BCMM_TransportMode(){return m_BCMMLivingTransportMode;}
 IEntity BCMM_TransportActor(){return BCMM_BodyBagComponent.Resolve(m_BCMMCarrier);}
 void BCMM_BeginTransport(IEntity actor,int mode)
 {
  IEntity anchor=BCMM_PoseAnchor.Spawn(actor,mode);if(!anchor)return;
  m_BCMMPoseAnchor=BCMM_BodyBagComponent.Id(anchor);m_BCMMLocalPoseAnchor=anchor;m_BCMMPoseSeated=false;m_BCMMPoseRetry=0;
  m_BCMMCarrier=BCMM_BodyBagComponent.Id(actor);m_BCMMLivingTransportMode=mode;
  m_BCMMTransportLastActor=actor.GetOrigin();m_BCMMTransportSafeGround=GetOwner().GetOrigin();
  m_BCMMTransportActorPlayer=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  m_BCMMTransportPatientPlayer=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetOwner());
  m_BCMMTransportInitiallyConscious=!BCMM_LivingTransport.Controller(GetOwner()).IsUnconscious();
  m_BCMMTransportPoseUntil=GetGame().GetWorld().GetWorldTime()+5000;
  Replication.BumpMe();
 }
 protected void BCMM_TransportChanged(){if(m_BCMMLivingTransportMode==0)BCMM_RestoreTransportControls();}
 protected void BCMM_RestoreTransportControls()
 {
  m_BCMMTransportCollision.Restore(GetOwner());
  BCMM_StopCasualtyPose();
  // Native compartment handling owns collision/gravity and clinical control
  // flags. Only the active input hook is suppressed, so waking cannot restore
  // a stale disabled-control snapshot captured while unconscious.
  m_BCMMTransportApplied=false;
 }
 void BCMM_EndTransport(string reason="release")
 {
  if(!Replication.IsServer())return;
  IEntity actor=BCMM_TransportActor();
  string diagnostic;if(System.GetCLIParam("bcmmLivingTransportNetwork",diagnostic))PrintFormat("BCMM_TRANSPORT_END reason=%1 mode=%2 actor=%3 patient=%4 falling=%5",reason,m_BCMMLivingTransportMode,actor,GetOwner().GetOrigin(),actor && BCMM_LivingTransport.Controller(actor).IsFalling());
  IEntity anchor=BCMM_PoseAnchorEntity();RplId anchorId=m_BCMMPoseAnchor;
  m_BCMMLivingTransportMode=0;m_BCMMCarrier=RplId.Invalid();
  BCMM_RestoreTransportControls();
  m_BCMMPoseAnchor=RplId.Invalid();
  ChimeraCharacter patient=ChimeraCharacter.Cast(GetOwner());
  if(patient)
  {
   if(!patient.GetCompartmentAccessComponent().IsInCompartment()){vector transform[4];patient.GetWorldTransform(transform);transform[3]=m_BCMMTransportSafeGround;patient.Teleport(transform);}
   Rpc(BCMM_TransportReleasedPlacement,anchorId,m_BCMMTransportSafeGround,patient.GetAngles());
   if(anchor)GetGame().GetCallqueue().CallLater(BCMM_PoseAnchor.Cleanup,500,false,anchor,patient,0);
  }
  if(actor)
  {
   SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(actor.FindComponent(SCR_CharacterDamageManagerComponent));
   if(medical)medical.BCMM_ApplyMovement();
  }
  Replication.BumpMe();
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Broadcast)]
 protected void BCMM_TransportReleasedPlacement(RplId anchorId,vector position,vector angles)
 {
  ChimeraCharacter patient=ChimeraCharacter.Cast(GetOwner());
  if(!patient || (!Replication.IsServer() && !patient.GetRplComponent().IsOwner()))return;
  BCMM_PoseAnchor.Exit(BCMM_BodyBagComponent.Resolve(anchorId),patient,position,angles);
  if(!patient.GetCompartmentAccessComponent().IsInCompartment()){vector transform[4];Math3D.AnglesToMatrix(angles,transform);transform[3]=position;patient.Teleport(transform);}
 }
 void BCMM_UpdateTransport()
 {
  if(m_BCMMLivingTransportMode==0){if(m_BCMMTransportApplied)BCMM_RestoreTransportControls();return;}
  IEntity actor=BCMM_TransportActor();IEntity patient=GetOwner();
  if(!actor){if(Replication.IsServer())BCMM_EndTransport();return;}
  m_BCMMTransportCollision.Apply(patient,EPhysicsLayerPresets.FireGeo);
  if(Replication.IsServer())
  {
   bool valid=BCMM_LivingTransport.Eligible(actor,patient,m_BCMMLivingTransportMode,false);
   if(m_BCMMTransportPatientPlayer>0 && !m_BCMMTransportInitiallyConscious && !BCMM_LivingTransport.Controller(patient).IsUnconscious())valid=false;
   if(m_BCMMTransportActorPlayer>0 && GetGame().GetPlayerManager().GetPlayerControlledEntity(m_BCMMTransportActorPlayer)!=actor)valid=false;
   if(m_BCMMTransportPatientPlayer>0 && GetGame().GetPlayerManager().GetPlayerControlledEntity(m_BCMMTransportPatientPlayer)!=patient)valid=false;
   if(vector.Distance(actor.GetOrigin(),m_BCMMTransportLastActor)>BCMM_TreatmentAccess.Setting("casualtyTransportMaxStepMetres"))valid=false;
   if(!valid){BCMM_EndTransport("eligibility-or-step");return;}
   m_BCMMTransportLastActor=actor.GetOrigin();
  }
  CharacterControllerComponent ac=BCMM_LivingTransport.Controller(actor);CharacterControllerComponent pc=BCMM_LivingTransport.Controller(patient);
  if(!ac || !pc)return;
  RplComponent actorReplication=ChimeraCharacter.Cast(actor).GetRplComponent();
  bool actorAuthority=Replication.IsServer() || (actorReplication && actorReplication.IsOwner());
  if(m_BCMMLivingTransportMode==2 && actorAuthority)ac.ForceStance(ECharacterStance.CROUCH);
  float cap=BCMM_TreatmentAccess.Setting("casualtyCarrySpeedFraction");if(m_BCMMLivingTransportMode==2)cap=BCMM_TreatmentAccess.Setting("casualtyDragSpeedFraction");
  SCR_CharacterDamageManagerComponent actorMedical=SCR_CharacterDamageManagerComponent.Cast(actor.FindComponent(SCR_CharacterDamageManagerComponent));
  if(actorMedical)cap=Math.Min(cap,actorMedical.BCMM_MovementCap());
  if(actorAuthority){ac.OverrideMaxSpeed(cap);ac.SetWeaponRaised(false);ac.SetFireWeaponWanted(false);}
  BCMM_EnsureCasualtyPose();
  if(!Replication.IsServer() && !ChimeraCharacter.Cast(patient).GetRplComponent().IsOwner())return;
  m_BCMMTransportApplied=true;
  pc.SetMovement(0,vector.Zero);pc.GetInputContext().SetMovement(0,vector.Zero);
  if(Replication.IsServer() && !m_BCMMPoseSeated && GetGame().GetWorld().GetWorldTime()>m_BCMMTransportPoseUntil){BCMM_EndTransport();return;}
  if(Replication.IsServer() && m_BCMMPoseSeated && !BCMM_PoseAnchor.Contains(BCMM_PoseAnchorEntity(),patient)){BCMM_EndTransport();return;}
  vector matrix[4];actor.GetWorldTransform(matrix);
  // The authored poses already include their height above the floor. The
  // release clearance belongs only to the detached native character root.
  float distance=0;float height=-0.08;if(m_BCMMLivingTransportMode==2)distance=1.05;
  vector desired=matrix[3]-matrix[2]*distance;
  TraceParam floor=new TraceParam();floor.Start=desired+"0 1.5 0";floor.End=desired-"0 2 0";
  floor.Flags=TraceFlags.WORLD|TraceFlags.ENTS;floor.LayerMask=EPhysicsLayerPresets.Projectile;array<IEntity> exclusions={actor,patient};floor.ExcludeArray=exclusions;
  float ground=BCMM_LivingTransport.TraceGround(patient.GetWorld(),floor,exclusions);
  if(ground>=1){if(Replication.IsServer())BCMM_EndTransport("no-ground");return;}
  vector safe=Vector(desired[0],floor.Start[1]-3.5*ground+0.08,desired[2]);
  if(Math.AbsFloat(safe[1]-actor.GetOrigin()[1])>0.75){if(Replication.IsServer())BCMM_EndTransport("ground-height");return;}
  desired[1]=safe[1]+height;
  TraceParam path=new TraceParam();path.Start=patient.GetOrigin()+"0 0.3 0";path.End=desired+"0 0.3 0";
  path.Flags=TraceFlags.WORLD|TraceFlags.ENTS;path.LayerMask=EPhysicsLayerPresets.Projectile;path.ExcludeArray=exclusions;
  if(patient.GetWorld().TraceMove(path)<0.99){if(Replication.IsServer())BCMM_EndTransport("path-blocked");return;}
  m_BCMMTransportSafeGround=safe;matrix[3]=desired;
  if(m_BCMMLivingTransportMode==2){matrix[0]=-matrix[0];matrix[2]=-matrix[2];}
  IEntity poseAnchor=BCMM_PoseAnchorEntity();if(poseAnchor)poseAnchor.SetWorldTransform(matrix);
 }
 IEntity BCMM_PoseAnchorEntity(){return BCMM_BodyBagComponent.Resolve(m_BCMMPoseAnchor);}
 void BCMM_EnsureCasualtyPose()
 {
  ChimeraCharacter patient=ChimeraCharacter.Cast(GetOwner());
  if(!patient || this.BCMM_TransportMode()==0)return;
  IEntity actor=BCMM_TransportActor();IEntity anchor=BCMM_PoseAnchorEntity();if(!actor || !anchor)return;
  m_BCMMLocalPoseAnchor=anchor;
  vector transform[4];actor.GetWorldTransform(transform);
  if(m_BCMMLivingTransportMode==2){transform[3]=transform[3]-transform[2]*1.05;transform[0]=-transform[0];transform[2]=-transform[2];}
  anchor.SetWorldTransform(transform);
  if(!Replication.IsServer() && !patient.GetRplComponent().IsOwner())return;
  CharacterControllerComponent controller=patient.GetCharacterController();
  m_BCMMTransportApplied=true;
  if(BCMM_PoseAnchor.Contains(anchor,patient)){m_BCMMPoseSeated=true;return;}
  if(!patient.GetRplComponent().IsOwner())return;
  if(m_BCMMPoseSeated || BCMM_LivingTransport.Now()<m_BCMMPoseRetry)return;
  m_BCMMPoseRetry=BCMM_LivingTransport.Now()+0.25;
  BCMM_PoseAnchor.Enter(anchor,patient);
 }
 void BCMM_StopCasualtyPose()
 {
  IEntity anchor=m_BCMMLocalPoseAnchor;if(!anchor)anchor=BCMM_PoseAnchorEntity();
  vector position=GetOwner().GetOrigin();if(Replication.IsServer())position=m_BCMMTransportSafeGround;
  BCMM_PoseAnchor.Exit(anchor,GetOwner(),position,GetOwner().GetAngles());
  m_BCMMLocalPoseAnchor=null;m_BCMMPoseSeated=false;
 }

}
modded class SCR_CharacterCommandHandlerComponent
{
 override bool HandleFalling(CharacterInputContext pInputCtx,float pDt,int pCurrentCommandID)
 {
  SCR_CharacterDamageManagerComponent state=BCMM_LivingTransport.ForParticipant(GetCharacter());
  if(state && state.GetOwner()==GetCharacter())return false;
  return HandleFallingDefault(pInputCtx,pDt,pCurrentCommandID);
 }
}
modded class SCR_CharacterControllerComponent
{
 override protected void OnApplyControls(IEntity owner,float timeSlice)
 {
  super.OnApplyControls(owner,timeSlice);
  RplComponent replication=RplComponent.Cast(owner.FindComponent(RplComponent));
  if(!Replication.IsServer() && replication && !replication.IsOwner())return;
  SCR_CharacterDamageManagerComponent state=BCMM_LivingTransport.ForParticipant(owner);
  if(!state || state.GetOwner()!=owner)return;
  SetMovement(0,vector.Zero);GetInputContext().SetMovement(0,vector.Zero);GetInputContext().SetJump(0);
  SetFireWeaponWanted(false);SetThrow(false,true);
 }
 override bool CanJumpClimb()
 {
  if(BCMM_LivingTransport.ForParticipant(GetOwner()))return false;
  return super.CanJumpClimb();
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);if(!world.IsEditMode())BCMM_LivingTransport.Update();
 }
}
