// Original two-player transport. No inventory/seat/parent attachment is used.
// Server reservations own motion; replicated transforms also anchor a locally owned patient.
modded class BCMM_StretcherComponent
{
 [RplProp()] protected RplId m_FrontCarrier = RplId.Invalid();
 [RplProp()] protected RplId m_RearCarrier = RplId.Invalid();
 [RplProp()] protected bool m_Carried;
 [RplProp()] protected int m_GripEpoch;
 [RplProp()] protected vector m_TransportPosition;
 [RplProp()] protected float m_TransportYaw;
 [RplProp(onRplName: "ApplyTransportTransform")] protected int m_TransportRevision;
 protected bool m_CollisionReduced;
 protected ref array<int> m_FullGeomIndices = {};
 protected ref array<int> m_FullGeomLayers = {};
 protected float m_SettlingUntil;
 protected int m_FrontPlayer;
 protected int m_RearPlayer;
 protected float m_ReservationExpiry;
 protected vector m_LastSupportedPosition;
 protected float m_LastSupportedYaw;
 override protected void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  if (Replication.IsServer()) GetGame().GetCallqueue().CallLater(CarryTick, 50, true);
 }
 override protected void OnDelete(IEntity owner)
 {
  if (GetGame()) GetGame().GetCallqueue().Remove(CarryTick);
  m_Carried=false;ApplyCarryCollision();
  super.OnDelete(owner);
 }
 int GripEpoch() { return m_GripEpoch; }
 bool IsCarried() { return m_Carried; }
 bool HasCarriers() { return m_FrontCarrier != RplId.Invalid() || m_RearCarrier != RplId.Invalid(); }
 bool HasCarrier(IEntity actor)
 {
  RplId id = BCMM_BodyBagComponent.Id(actor);
  return id != RplId.Invalid() && (id == m_FrontCarrier || id == m_RearCarrier);
 }
 static BCMM_StretcherComponent ForCarrier(IEntity actor)
 {
  foreach(BCMM_StretcherComponent stretcher:s_Stretchers)
  {
   if(stretcher && stretcher.HasCarrier(actor)) { return stretcher; }
  }
  return null;
 }
 static bool Carrying(IEntity actor)
 {
  foreach (BCMM_StretcherComponent stretcher : s_Stretchers)
   if (stretcher && stretcher.IsCarried() && stretcher.HasCarrier(actor)) return true;
  return false;
 }
 static bool Stabilises(IEntity patient)
 {
  return BCMM_TreatmentAccess.Setting("stretcherStabilityEnabled") > 0.5 && OnAnotherStretcher(patient);
 }
 protected bool CarrierValid(IEntity actor, int playerId)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character || playerId <= 0 || GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId) != actor || character.IsInVehicle()) return false;
  CharacterControllerComponent control = character.GetCharacterController();
  SCR_CharacterControllerComponent scripted = SCR_CharacterControllerComponent.Cast(control);
  if ((scripted && scripted.IsLoitering()) || BCMM_NativeBagDrag.ManualDragging(actor)) return false;
  return control && control.GetLifeState() == ECharacterLifeState.ALIVE && !control.IsUnconscious() && !control.IsFalling() && !control.IsSwimming() && !control.IsSprinting() && !control.IsWeaponRaised() && !control.IsUsingItem();
 }
 vector Handle(bool front)
 {
  vector matrix[4]; GetOwner().GetWorldTransform(matrix);
  float end = -BCMM_TreatmentAccess.Setting("stretcherHandleOffsetMetres"); if (front) end = -end;
  return matrix[3] + matrix[2] * end;
 }
 bool CanJoin(IEntity actor, bool front)
 {
  if (BCMM_TreatmentAccess.Setting("stretcherCarryEnabled") < 0.5 || HasCarrier(actor) || !ActorCanUse(actor)) return false;
  int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  if (!CarrierValid(actor, playerId) || BCMM_SplintTreatment.IsBusy(actor, Patient())) return false;
  if ((front && m_FrontCarrier != RplId.Invalid()) || (!front && m_RearCarrier != RplId.Invalid())) return false;
  foreach (BCMM_StretcherComponent other : s_Stretchers) if (other && other.HasCarrier(actor)) return false;
  vector offset = actor.GetOrigin() - Handle(front); offset[1] = 0;
  return offset.Length() <= BCMM_TreatmentAccess.Setting("stretcherJoinRangeMetres");
 }
 bool Join(IEntity actor, bool front)
 {
  if (!Replication.IsServer() || !CanJoin(actor, front)) return false;
  RplId id = BCMM_BodyBagComponent.Id(actor); if (id == RplId.Invalid()) return false;
  int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  if (!HasCarriers()) m_ReservationExpiry = GetGame().GetWorld().GetWorldTime() * 0.001 + BCMM_TreatmentAccess.Setting("stretcherPairTimeoutSeconds");
  if (front) { m_FrontCarrier = id; m_FrontPlayer = playerId; }
  else { m_RearCarrier = id; m_RearPlayer = playerId; }
  if (m_FrontCarrier != RplId.Invalid() && m_RearCarrier != RplId.Invalid())
  {
   m_LastSupportedPosition = GetOwner().GetOrigin(); m_LastSupportedYaw = GetOwner().GetAngles()[0];
   m_Carried = true; ApplyCarryCollision();
  }
  Replication.BumpMe(); return true;
 }
 bool ReleaseGrip(IEntity actor)
 {
  if (!Replication.IsServer() || !HasCarrier(actor)) return false;
  SetDown(); return true;
 }
 protected void PublishTransform(vector position, float yaw)
 {
  m_TransportPosition = position; m_TransportYaw = yaw; m_TransportRevision++;
  ApplyTransportTransform(); Replication.BumpMe();
 }
 protected void ApplyTransportTransform()
 {
  if (m_TransportRevision <= 0 || !GetOwner()) return;
  ApplyCarryCollision();
  GetOwner().SetAngles(Vector(m_TransportYaw,0,0)); GetOwner().SetOrigin(m_TransportPosition);
  if(!m_Carried)m_SettlingUntil=GetGame().GetWorld().GetWorldTime()+750;
  AnchorPatient();
 }
 protected void ApplyCarryCollision()
 {
  Physics physics=GetOwner().GetPhysics();if(!physics)return;
  if(m_Carried && !m_CollisionReduced && physics.GetNumGeoms()>1)
  {
   for(int i=0;i<physics.GetNumGeoms();i++)
   {
    vector low,high;physics.GetGeomWorldBounds(i,low,high);
    if(Math.Max(high[0]-low[0],high[2]-low[2])<1.5)continue;
    m_FullGeomIndices.Insert(i);m_FullGeomLayers.Insert(physics.GetGeomInteractionLayer(i));physics.SetGeomInteractionLayer(i,0);
   }
   m_CollisionReduced=true;
  }
  if(!m_Carried && m_CollisionReduced)
  {
   for(int j=0;j<m_FullGeomIndices.Count();j++)physics.SetGeomInteractionLayer(m_FullGeomIndices[j],m_FullGeomLayers[j]);
   m_FullGeomIndices.Clear();m_FullGeomLayers.Clear();m_CollisionReduced=false;
  }
 }
 void AnchorPatient()
 {
  ApplyCarryCollision();
  IEntity patient = Patient(); ChimeraCharacter character = ChimeraCharacter.Cast(patient);
  if (!character || !character.GetCharacterController().IsUnconscious()) return;
  RplComponent replication = character.GetRplComponent();
  if (!Replication.IsServer() && replication && !replication.IsOwner()) return;
  vector matrix[4]; GetOwner().GetWorldTransform(matrix);
  matrix[3] = matrix[3] + matrix[1] * 0.43;
  // Move the native character/ragdoll state throughout transport, not only its render transform.
  character.Teleport(matrix);
  if(!m_Carried)
  {
   character.GetCharacterController().SetMovement(0,vector.Zero);
   character.GetCharacterController().GetInputContext().SetMovement(0,vector.Zero);
   Physics physics=patient.GetPhysics();if(physics && physics.IsDynamic()){physics.SetVelocity(vector.Zero);physics.SetAngularVelocity(vector.Zero);}
  }
 }
 protected void SetDown(string reason = "player release")
 {
  m_GripEpoch++;
  bool lifted = m_Carried;
  if (lifted) Print("BCMM stretcher set down: "+reason);
  m_Carried = false; m_FrontCarrier = RplId.Invalid(); m_RearCarrier = RplId.Invalid(); m_FrontPlayer = 0; m_RearPlayer = 0;
  if (lifted) PublishTransform(m_LastSupportedPosition, m_LastSupportedYaw);
  else Replication.BumpMe();
 }
 protected bool Supported(vector centre, vector direction, array<IEntity> exclusions, out vector ground)
 {
  float high = -float.MAX; float low = float.MAX;
  for (int end = -1; end <= 1; end++)
  {
   TraceParam floor = new TraceParam();
   floor.Start = centre + direction * (end * 0.8) + "0 1 0";
   floor.End = floor.Start - "0 2 0";
   floor.Flags = TraceFlags.WORLD | TraceFlags.ENTS; floor.LayerMask = EPhysicsLayerPresets.Projectile; floor.ExcludeArray = exclusions;
   float hit = GetOwner().GetWorld().TraceMove(floor); if (hit >= 1) return false;
   float height = floor.Start[1] - 2 * hit; high = Math.Max(high,height); low = Math.Min(low,height);
  }
  if (high - low > BCMM_TreatmentAccess.Setting("stretcherMaximumHeightDifferenceMetres")) return false;
  ground = centre; ground[1] = high + 0.01; return true;
 }
 protected bool SweptClear(vector destination, vector direction, array<IEntity> exclusions)
 {
  // Sweep from the previous end positions to the new rotated positions.
  vector previous[4];GetOwner().GetWorldTransform(previous);
  for (int end = -1; end <= 1; end++)
  {
   TraceSphere path = new TraceSphere(); path.Radius = 0.33;
   vector offset = direction * (end * 0.85) + "0 0.65 0";
   path.Start = GetOwner().GetOrigin() + previous[2]*(end*0.85) + "0 0.65 0"; path.End = destination + offset;
   path.Flags = TraceFlags.WORLD | TraceFlags.ENTS; path.LayerMask = EPhysicsLayerPresets.Projectile; path.ExcludeArray = exclusions;
   if (GetOwner().GetWorld().TraceMove(path) < 0.99) return false;
  }
  TraceSphere length = new TraceSphere(); length.Radius = 0.33;
  length.Start = destination - direction * 0.85 + "0 0.65 0";
  length.End = destination + direction * 0.85 + "0 0.65 0";
  length.Flags = TraceFlags.WORLD | TraceFlags.ENTS; length.LayerMask = EPhysicsLayerPresets.Projectile; length.ExcludeArray = exclusions;
  return GetOwner().GetWorld().TraceMove(length) >= 0.99;
 }
 protected void CarryTick()
 {
  if (!HasCarriers()) return;
  IEntity front = BCMM_BodyBagComponent.Resolve(m_FrontCarrier); IEntity rear = BCMM_BodyBagComponent.Resolve(m_RearCarrier);
  if (BCMM_TreatmentAccess.Setting("stretcherCarryEnabled") < 0.5 || GetOwner().GetParent()) { SetDown(); return; }
  if (!m_Carried)
  {
   IEntity waiting = front; int playerId = m_FrontPlayer; bool atFront = true;
   if (!waiting) { waiting = rear; playerId = m_RearPlayer; atFront = false; }
   if (!CarrierValid(waiting,playerId) || vector.Distance(waiting.GetOrigin(),Handle(atFront)) > BCMM_TreatmentAccess.Setting("stretcherJoinRangeMetres") + 0.5 || GetGame().GetWorld().GetWorldTime() * 0.001 >= m_ReservationExpiry) SetDown();
   return;
  }
  if (!CarrierValid(front,m_FrontPlayer) || !CarrierValid(rear,m_RearPlayer)) { SetDown("carrier state or ownership"); return; }
  ChimeraCharacter casualty = ChimeraCharacter.Cast(Patient());
  if (casualty && !casualty.GetCharacterController().IsUnconscious()) { SetDown(); return; }
  vector separation = front.GetOrigin()-rear.GetOrigin();
  if (Math.AbsFloat(separation[1]) > BCMM_TreatmentAccess.Setting("stretcherMaximumHeightDifferenceMetres")) { SetDown(); return; }
  separation[1]=0; float span=separation.Length();
  float maximumSpeed=BCMM_TreatmentAccess.Setting("stretcherMaximumSpeedMetresPerSecond");
  float frontSpeed=ChimeraCharacter.Cast(front).GetCharacterController().GetVelocity().Length();
  float rearSpeed=ChimeraCharacter.Cast(rear).GetCharacterController().GetVelocity().Length();
  if (span < BCMM_TreatmentAccess.Setting("stretcherMinimumSpanMetres") || span > BCMM_TreatmentAccess.Setting("stretcherMaximumSpanMetres") || frontSpeed > maximumSpeed || rearSpeed > maximumSpeed)
  { PrintFormat("BCMM stretcher limit: span=%1 frontSpeed=%2 rearSpeed=%3",span,frontSpeed,rearSpeed);SetDown("carrier span or speed limit");return; }

  vector direction=separation/span; vector requestedAngles=direction.VectorToAngles();
  float turn=Math.AbsFloat(requestedAngles[0]-GetOwner().GetAngles()[0]);if(turn>180)turn=360-turn;
  if(turn>BCMM_TreatmentAccess.Setting("stretcherMaximumTurnDegreesPerSecond")*0.05){SetDown("turn rate limit");return;}
  vector centre=(front.GetOrigin()+rear.GetOrigin())*0.5;
  array<IEntity> exclusions={GetOwner(),front,rear}; if (Patient()) exclusions.Insert(Patient());
  vector ground;
  if (!Supported(centre,direction,exclusions,ground)) { SetDown("unsupported ground"); return; }
  vector destination=ground+Vector(0,BCMM_TreatmentAccess.Setting("stretcherCarryHeightMetres"),0);
  if (!SweptClear(destination,direction,exclusions)) { SetDown("blocked movement"); return; }
  vector angles=direction.VectorToAngles(); m_LastSupportedPosition=ground; m_LastSupportedYaw=angles[0];
  PublishTransform(destination,angles[0]);
 }
 override protected void RefreshOccupant()
 {
  if (!m_Carried && !IsSettling()) super.RefreshOccupant();
 }
 override bool Place(IEntity actor,IEntity patient)
 {
  if (HasCarriers()) return false;
  return super.Place(actor,patient);
 }
 override bool Remove(IEntity actor)
 {
  if (HasCarriers() || IsSettling()) return false;
  return super.Remove(actor);
 }
 bool IsSettling() { return GetGame().GetWorld().GetWorldTime()<m_SettlingUntil; }
 static void UpdateCarriedPatients()
 {
  foreach (BCMM_StretcherComponent stretcher : s_Stretchers)
  {
   if(!stretcher) { continue; }
   stretcher.BCMM_UpdatePlacement();
   if(stretcher.Patient()) { stretcher.AnchorPatient(); }
  }
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestStretcherGrip(IEntity stretcher,int command)
 {
  if(GetGame().GetPlayerController()!=this || !stretcher)return;
  BCMM_StretcherComponent component=BCMM_StretcherComponent.Cast(stretcher.FindComponent(BCMM_StretcherComponent));
  if(component)Rpc(BCMM_StretcherGripRequest,BCMM_BodyBagComponent.Id(stretcher),command,component.GripEpoch());
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_StretcherGripRequest(RplId id,int command,int expectedEpoch)
 {
  IEntity entity=BCMM_BodyBagComponent.Resolve(id); if (!entity) return;
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(entity.FindComponent(BCMM_StretcherComponent)); if (!stretcher) return;
  if(expectedEpoch!=stretcher.GripEpoch())return;
  bool success;
  if (command==0) success=stretcher.Join(GetControlledEntity(),true);
  if (command==1) success=stretcher.Join(GetControlledEntity(),false);
  if (command==2) success=stretcher.ReleaseGrip(GetControlledEntity());
  if (!success) BCMM_BandageFeedback.Send(this,"Stretcher grip unavailable: approach a free end with your weapon lowered. Two players are required.");
 }
}
class BCMM_StretcherFrontGripAction : ScriptedUserAction
{
 protected int GripCommand() { return 0; }
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return BCMM_TreatmentAccess.Setting("stretcherCarryEnabled")>0.5 && stretcher && !stretcher.IsCarried() && !stretcher.HasCarrier(user);
 }
 override bool CanBePerformedScript(IEntity user)
 {
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return stretcher && stretcher.CanJoin(user,GripCommand()==0);
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestStretcherGrip(pOwnerEntity,GripCommand());
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_StretcherRearGripAction : BCMM_StretcherFrontGripAction { override protected int GripCommand() { return 1; } }
class BCMM_StretcherReleaseGripAction : BCMM_StretcherFrontGripAction
{
 override protected int GripCommand() { return 2; }
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));return stretcher && stretcher.HasCarrier(user);
 }
 override bool CanBePerformedScript(IEntity user) { return CanBeShownScript(user); }
}
modded class SCR_CharacterControllerComponent
{
 override void OnPrepareControls(IEntity owner,ActionManager am,float dt,bool player)
 {
  super.OnPrepareControls(owner,am,dt,player);
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
  float cap=1; if (medical) cap=medical.BCMM_MovementCap();
  if (BCMM_StretcherComponent.Carrying(owner)) cap=Math.Min(cap,BCMM_TreatmentAccess.Setting("stretcherCarrySpeedFraction"));
  OverrideMaxSpeed(cap);
 }
 override bool CanJumpClimb()
 {
  if (BCMM_StretcherComponent.Carrying(GetCharacter())) return false;
  return super.CanJumpClimb();
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);if (!world.IsEditMode()) BCMM_StretcherComponent.UpdateCarriedPatients();
 }
}
