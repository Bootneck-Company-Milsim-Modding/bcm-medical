// Original stationary casualty placement. The native character retains inventory,
// damage, unconsciousness and movement replication; this component never heals it.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_StretcherComponentClass : ScriptComponentClass {}
class BCMM_StretcherComponent : ScriptComponent
{
 protected static ref array<BCMM_StretcherComponent> s_Stretchers = {};
 [RplProp()] protected RplId m_Patient = RplId.Invalid();
 protected int m_PatientPlacementToken;
 protected int m_LocalPatientPlacementToken;
 protected RplId m_ReleasedPatient = RplId.Invalid();
 protected RplId m_PlacementPatient = RplId.Invalid();
 protected vector m_PlacementPosition;
 protected float m_PlacementUntil;
 protected IEntity m_SearchActor;
 protected IEntity m_Nearest;
 protected float m_NearestDistance;
 override protected void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner); s_Stretchers.Insert(this);
  if (Replication.IsServer()) GetGame().GetCallqueue().CallLater(RefreshOccupant, 250, true);
 }
 override protected void OnDelete(IEntity owner)
 {
  if (GetGame()) GetGame().GetCallqueue().Remove(RefreshOccupant);
  s_Stretchers.RemoveItem(this);
  // A stationary patient remains a native world character if the stretcher is deleted.
  super.OnDelete(owner);
 }
 IEntity Patient()
 {
  if(m_Patient==m_ReleasedPatient) { return null; }
  return BCMM_BodyBagComponent.Resolve(m_Patient);
 }
 protected void RefreshOccupant()
 {
  IEntity patient = Patient();
  ChimeraCharacter character = ChimeraCharacter.Cast(patient);
  if (m_Patient != RplId.Invalid() && (!character || !character.GetCharacterController().IsUnconscious() || vector.Distance(patient.GetOrigin(), GetOwner().GetOrigin()) > 1.6))
  { m_Patient = RplId.Invalid(); Replication.BumpMe(); }
 }
 static bool OnAnotherStretcher(IEntity patient)
 {
  foreach (BCMM_StretcherComponent stretcher : s_Stretchers)
   if (stretcher && stretcher.Patient() == patient) return true;
  return false;
 }
 bool ActorCanUse(IEntity actor)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character || GetOwner().GetParent() || character.IsInVehicle()) return false;
  CharacterControllerComponent controller = character.GetCharacterController();
  if (!controller || controller.GetLifeState() != ECharacterLifeState.ALIVE || controller.IsUnconscious() || controller.IsSwimming() || controller.IsFalling()) return false;
  if (vector.Distance(actor.GetOrigin(), GetOwner().GetOrigin()) > BCMM_TreatmentAccess.Setting("treatmentRangeMetres")) return false;
  TraceParam sight = new TraceParam();
  sight.Start = character.EyePosition(); sight.End = GetOwner().GetOrigin() + "0 0.5 0";
  sight.Flags = TraceFlags.WORLD | TraceFlags.ENTS; sight.LayerMask = EPhysicsLayerPresets.Projectile;
  array<IEntity> exclusions = {actor, GetOwner()};
  if (Patient()) exclusions.Insert(Patient());
  sight.ExcludeArray = exclusions;
  return actor.GetWorld().TraceMove(sight) >= 0.99;
 }
 bool CanPlace(IEntity actor, IEntity patient)
 {
  ChimeraCharacter casualty = ChimeraCharacter.Cast(patient);
  if (!casualty || actor == patient || Patient() || !ActorCanUse(actor) || casualty.IsInVehicle() || casualty.GetParent()) return false;
  CharacterControllerComponent controller = casualty.GetCharacterController();
  if (controller.GetLifeState() == ECharacterLifeState.DEAD || !controller.IsUnconscious()) return false;
  if (OnAnotherStretcher(patient) || BCMM_SplintTreatment.IsBusy(actor, patient)) return false;
  return BCMM_TreatmentAccess.CanReach(actor, patient);
 }
 IEntity FindCasualty(IEntity actor)
 {
  if (!ActorCanUse(actor) || Patient()) return null;
  m_SearchActor = actor; m_Nearest = null; m_NearestDistance = float.MAX;
  actor.GetWorld().QueryEntitiesBySphere(actor.GetOrigin(), BCMM_TreatmentAccess.Setting("treatmentRangeMetres"), Collect, null, EQueryEntitiesFlags.DYNAMIC);
  m_SearchActor = null; return m_Nearest;
 }
 void CommitPatientPlacement(IEntity patient,vector position,vector angles,bool unloading=false)
 {
  if(!Replication.IsServer() || !patient)return;
  m_PatientPlacementToken++;
  RplId id=BCMM_BodyBagComponent.Id(patient);
  ApplyPatientPlacement(id,position,angles,m_PatientPlacementToken,unloading);
  Rpc(ApplyPatientPlacement,id,position,angles,m_PatientPlacementToken,unloading);
  int playerId=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(patient);
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  if(player)player.BCMM_SendCasualtyPlacement(BCMM_BodyBagComponent.Id(GetOwner()),id,position,angles,m_PatientPlacementToken,unloading);
 }
 void BCMM_ApplyConfirmedPlacement(RplId id,vector position,vector angles,int token,bool unloading)
 {
  ApplyPatientPlacement(id,position,angles,token,unloading);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Broadcast)]
 protected void ApplyPatientPlacement(RplId id,vector position,vector angles,int token,bool unloading)
 {
  if(token<=m_LocalPatientPlacementToken) { return; }
  m_LocalPatientPlacementToken=token;
  if(unloading)
  {
   m_ReleasedPatient=id;
   if(m_Patient==id) { m_Patient=RplId.Invalid(); }
  }
  else
  {
   m_ReleasedPatient=RplId.Invalid();
  }
  ChimeraCharacter patient=ChimeraCharacter.Cast(BCMM_BodyBagComponent.Resolve(id));
  if(!patient || !patient.GetCharacterController().IsUnconscious()) { return; }
  bool nativeOwner=patient.GetRplComponent().IsOwner();
  string fixture;
  if(System.GetCLIParam("bcmmStretcherSettingsMirror",fixture))
  {
   PrintFormat("BCMM_PLACEMENT_APPLY server=%1 owner=%2 token=%3 unload=%4 old=%5 target=%6",Replication.IsServer(),nativeOwner,token,unloading,patient.GetOrigin(),position);
  }
  if(!Replication.IsServer() && !nativeOwner) { return; }
  vector transform[4];
  if(unloading) { patient.GetWorldTransform(transform); }
  else { GetOwner().GetWorldTransform(transform); }
  transform[3]=position;
  m_PlacementPatient=RplId.Invalid();
  if(unloading)
  {
   m_PlacementPatient=id; m_PlacementPosition=position;
   m_PlacementUntil=GetGame().GetWorld().GetWorldTime()+1000;
  }
  patient.Teleport(transform);
  patient.GetCharacterController().SetMovement(0,vector.Zero);
  patient.GetCharacterController().GetInputContext().SetMovement(0,vector.Zero);
  Physics physics=patient.GetPhysics();
  if(physics && physics.IsDynamic())
  {
   physics.SetVelocity(vector.Zero);
   physics.SetAngularVelocity(vector.Zero);
  }
  if(System.GetCLIParam("bcmmStretcherSettingsMirror",fixture))
  {
   PrintFormat("BCMM_PLACEMENT_APPLIED server=%1 token=%2 position=%3",Replication.IsServer(),token,patient.GetOrigin());
  }
 }
 void BCMM_UpdatePlacement()
 {
  if(m_PlacementPatient==RplId.Invalid()) { return; }
  if(GetGame().GetWorld().GetWorldTime()>=m_PlacementUntil)
  {
   m_PlacementPatient=RplId.Invalid(); return;
  }
  ChimeraCharacter patient=ChimeraCharacter.Cast(BCMM_BodyBagComponent.Resolve(m_PlacementPatient));
  if(!patient || !patient.GetCharacterController().IsUnconscious()) { return; }
  if(!Replication.IsServer() && !patient.GetRplComponent().IsOwner()) { return; }
  vector transform[4];patient.GetWorldTransform(transform);transform[3]=m_PlacementPosition;
  patient.Teleport(transform);
  patient.GetCharacterController().SetMovement(0,vector.Zero);
  patient.GetCharacterController().GetInputContext().SetMovement(0,vector.Zero);
  Physics physics=patient.GetPhysics();
  if(physics && physics.IsDynamic()) { physics.SetVelocity(vector.Zero);physics.SetAngularVelocity(vector.Zero); }
 }
 protected bool Collect(IEntity entity)
 {
  if (!CanPlace(m_SearchActor, entity)) return true;
  float distance = vector.Distance(m_SearchActor.GetOrigin(), entity.GetOrigin());
  if (distance < m_NearestDistance) { m_NearestDistance = distance; m_Nearest = entity; }
  return true;
 }
 protected bool ClearSpace(vector centre, IEntity patient)
 {
  // Test the full casualty footprint, not just the root point, against walls/ceilings.
  vector matrix[4]; GetOwner().GetWorldTransform(matrix);
  for (int i = -1; i <= 1; i++)
  {
   TraceSphere trace = new TraceSphere(); trace.Radius = 0.18;
   vector sideways = matrix[0] * (i * 0.16);
   trace.Start = centre + sideways - matrix[2] * 0.85 + "0 0.28 0";
   trace.End = centre + sideways + matrix[2] * 0.85 + "0 0.28 0";
   trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS; trace.LayerMask = EPhysicsLayerPresets.Projectile;
   array<IEntity> exclusions = {GetOwner(), patient};
   trace.ExcludeArray = exclusions;
   if (GetOwner().GetWorld().TraceMove(trace) < 0.99) return false;
  }
  return true;
 }
 bool Place(IEntity actor, IEntity patient)
 {
  if (!Replication.IsServer() || !CanPlace(actor, patient)) return false;
  vector matrix[4]; GetOwner().GetWorldTransform(matrix);
  vector position = matrix[3] + matrix[1] * 0.43;
  if (!ClearSpace(position, patient)) return false;
  m_Patient = BCMM_BodyBagComponent.Id(patient);
  if (m_Patient == RplId.Invalid()) return false;
  CommitPatientPlacement(patient,position,GetOwner().GetAngles());
  Replication.BumpMe(); return true;
 }
 bool Remove(IEntity actor)
 {
  if (!Replication.IsServer() || !ActorCanUse(actor)) return false;
  IEntity patient = Patient(); if (!patient || BCMM_SplintTreatment.IsBusy(actor, patient)) return false;
  vector matrix[4]; GetOwner().GetWorldTransform(matrix);
  for (int side = -1; side <= 1; side += 2)
  {
   vector position = matrix[3] + matrix[0] * (side * 1.5);
   TraceParam floor = new TraceParam(); floor.Start = position + "0 0.8 0"; floor.End = position - "0 0.6 0";
   floor.Flags = TraceFlags.WORLD | TraceFlags.ENTS; floor.LayerMask = EPhysicsLayerPresets.Projectile;
   array<IEntity> exclusions = {GetOwner(), actor, patient};
   floor.ExcludeArray = exclusions;
   float hit = GetOwner().GetWorld().TraceMove(floor);
   if (hit >= 1) continue;
   position = floor.Start + (floor.End - floor.Start) * hit + "0 0.08 0";
   if (!ClearSpace(position, patient)) continue;
   TraceSphere path = new TraceSphere(); path.Radius = 0.2;
   path.Start = patient.GetOrigin() + "0 0.35 0"; path.End = position + "0 0.35 0";
   path.Flags = TraceFlags.WORLD | TraceFlags.ENTS; path.LayerMask = EPhysicsLayerPresets.Projectile;
   path.ExcludeArray = exclusions;
   float pathHit=GetOwner().GetWorld().TraceMove(path);
   if (pathHit < 0.99) continue;
   CommitPatientPlacement(patient,position,patient.GetAngles(),true); m_Patient = RplId.Invalid(); Replication.BumpMe(); return true;
  }
  return false;
 }
}
modded class SCR_PlayerController
{
 void BCMM_SendCasualtyPlacement(RplId stretcher,RplId patient,vector position,vector angles,int token,bool unloading)
 {
  if(Replication.IsServer())Rpc(BCMM_CasualtyPlacementOwner,stretcher,patient,position,angles,token,unloading);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_CasualtyPlacementOwner(RplId stretcher,RplId patient,vector position,vector angles,int token,bool unloading)
 {
  IEntity entity=BCMM_BodyBagComponent.Resolve(stretcher);if(!entity)return;
  BCMM_StretcherComponent component=BCMM_StretcherComponent.Cast(entity.FindComponent(BCMM_StretcherComponent));
  if(component)component.BCMM_ApplyConfirmedPlacement(patient,position,angles,token,unloading);
 }

 void BCMM_RequestStretcher(IEntity stretcher, IEntity patient, bool unloading)
 {
  if (GetGame().GetPlayerController() == this)
   Rpc(BCMM_StretcherRequest, BCMM_BodyBagComponent.Id(stretcher), BCMM_BodyBagComponent.Id(patient), unloading);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_StretcherRequest(RplId stretcherId, RplId patientId, bool unloading)
 {
  IEntity stretcher = BCMM_BodyBagComponent.Resolve(stretcherId); if (!stretcher) return;
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(stretcher.FindComponent(BCMM_StretcherComponent));
  if (!component) return;
  bool success;
  if (unloading) success = component.Remove(GetControlledEntity());
  else success = component.Place(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(patientId));
  if (!success) BCMM_BandageFeedback.Send(this, "Stretcher unavailable: check casualty, range, occupancy and clear space.");
 }
}
class BCMM_PlaceOnStretcherAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return component && !component.Patient();
 }
 override bool CanBePerformedScript(IEntity user)
 {
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return component && component.FindCasualty(user);
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(pOwnerEntity.FindComponent(BCMM_StretcherComponent));
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (component && player) player.BCMM_RequestStretcher(pOwnerEntity, component.FindCasualty(pUserEntity), false);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_RemoveFromStretcherAction : BCMM_PlaceOnStretcherAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return component && component.Patient();
 }
 override bool CanBePerformedScript(IEntity user)
 {
  BCMM_StretcherComponent component = BCMM_StretcherComponent.Cast(GetOwner().FindComponent(BCMM_StretcherComponent));
  return component && component.Patient() && component.ActorCanUse(user);
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestStretcher(pOwnerEntity, null, true);
 }
}
