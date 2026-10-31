// Original native Medical burial. Equipment previously deposited in a vehicle
// stays there; only the exact bag and equipment still inside it are disposed.
[ComponentEditorProps(category:"GameScripted/BCM Medical")]
class BCMM_GraveMarkerComponentClass:ScriptComponentClass {}
class BCMM_GraveMarkerComponent:ScriptComponent
{
 static ref array<BCMM_GraveMarkerComponent> Instances={};
 [RplProp()] protected string m_Name;
 [RplProp()] protected int m_Token;
 [RplProp()] protected float m_DeathSeconds;
 [RplProp()] protected float m_BurialSeconds;
 string DeceasedName() { return m_Name; }
 int RemainsToken() { return m_Token; }
 void SetMemorial(BCMM_RemainsRecord record)
 {
  if(!Replication.IsServer() || !record) return;
  m_Name=record.DeceasedName;m_Token=record.Token;m_DeathSeconds=record.DeathSeconds;
  m_BurialSeconds=GetOwner().GetWorld().GetWorldTime()*0.001;
  Replication.BumpMe();
 }
 override protected void OnPostInit(IEntity owner) { super.OnPostInit(owner);Instances.Insert(this);BCMM_RemainsProtection.ProtectBag(owner); }
 override void OnDelete(IEntity owner) { Instances.RemoveItem(this);BCMM_RemainsProtection.Forget(owner);super.OnDelete(owner); }
}
class BCMM_InspectGraveAction:ScriptedUserAction
{
 override bool GetActionNameScript(out string outName)
 {
  BCMM_GraveMarkerComponent marker=BCMM_GraveMarkerComponent.Cast(GetOwner().FindComponent(BCMM_GraveMarkerComponent));
  outName=WidgetManager.Translate("#BCMM-InspectGrave");
  return true;
 }
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_GraveMarkerComponent marker=BCMM_GraveMarkerComponent.Cast(GetOwner().FindComponent(BCMM_GraveMarkerComponent));
  return marker && marker.RemainsToken()>0 && !marker.DeceasedName().IsEmpty();
 }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(player && player.GetControlledEntity()==pUserEntity)player.BCMM_InspectGraveLocal(pOwnerEntity);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
 override bool CanBroadcastScript() { return false; }
}
class BCMM_BurialOperation
{
 protected static ref array<ref BCMM_BurialOperation> s_Active={};
 protected IEntity m_Actor;
 protected IEntity m_Bag;
 protected IEntity m_Grave;
 protected BCMM_BodyBagComponent m_State;
 protected int m_PlayerId;
 protected PlayerController m_Controller;
 protected int m_Token;
 protected bool m_Released;
 protected int m_ReplicationWaits;
 protected float m_GraveScale;
 static bool HasShovel(IEntity actor)
 {
  return BCMM_BurialSite.HeldTool(actor)!=null;
 }
 protected static bool ActorValid(IEntity actor,IEntity bag)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);
  if(BCMM_BaggingOperation.Setting("burialEnabled")<0.5) return false;
  if(!character || !bag || character.IsInVehicle() || !HasShovel(actor)) return false;
  CharacterControllerComponent controller=character.GetCharacterController();
  return controller && controller.GetLifeState()==ECharacterLifeState.ALIVE && !controller.IsUnconscious() && !controller.IsSwimming() && !controller.IsFalling() && vector.Distance(actor.GetOrigin(),bag.GetOrigin())<=BCMM_BaggingOperation.Setting("burialRangeMetres");
 }
 static bool CanBegin(IEntity actor,IEntity bag)
 {
  if(!ActorValid(actor,bag)) return false;
  if(BCMM_BagSearchSessions.IsSearched(bag))return false;
  vector contact;vector normal;if(!BCMM_BurialSite.Trace(bag,actor,contact,normal)) return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  InventoryItemComponent item=InventoryItemComponent.Cast(bag.FindComponent(InventoryItemComponent));
  return state && state.Ready() && state.RemainsToken()>0 && state.State()==BCMM_BagCargoState.WORLD && state.DraggingActor()==RplId.Invalid() && item && !item.IsLocked() && !item.GetParentSlot();
 }
 static bool Begin(IEntity actor,IEntity bag)
 {
  if(!Replication.IsServer() || !CanBegin(actor,bag)) return false;
  float graveScale=BCMM_BurialSite.AuthoredGraveScale();
  if(graveScale<=0) return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(state.RemainsToken());
  if(!record || record.Physical!=bag || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) return false;
  foreach(BCMM_BurialOperation active:s_Active) if(active.m_Actor==actor || active.m_Bag==bag) return false;
  BCMM_BurialOperation operation=new BCMM_BurialOperation();
  operation.m_Actor=actor;operation.m_Bag=bag;operation.m_State=state;operation.m_Token=state.RemainsToken();
  operation.m_GraveScale=graveScale;
  operation.m_PlayerId=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  if(operation.m_PlayerId>0)
  {
   operation.m_Controller=GetGame().GetPlayerManager().GetPlayerController(operation.m_PlayerId);
   if(!operation.m_Controller || operation.m_Controller.GetControlledEntity()!=actor) return false;
  }
  state.SetReady(false);s_Active.Insert(operation);
  SCR_PlayerController presentation = SCR_PlayerController.Cast(operation.m_Controller);
  if(presentation) presentation.BCMM_SetBurialAnimation(operation.m_Token,true,BCMM_BaggingOperation.Setting("burialDurationSeconds")*1000);
  PrintFormat("BCMM_BURIAL begin token=%1 duration=%2",operation.m_Token,BCMM_BaggingOperation.Setting("burialDurationSeconds"));
  GetGame().GetCallqueue().CallLater(operation.Tick,250,true);
  GetGame().GetCallqueue().CallLater(operation.CompleteDigging,BCMM_BaggingOperation.Setting("burialDurationSeconds")*1000,false);
  return true;
 }
 static bool CancelForPerformer(IEntity actor,int token)
 {
  if(!Replication.IsServer() || !actor || token<=0) return false;
  foreach(BCMM_BurialOperation operation:s_Active)
  {
   if(operation.m_Actor==actor && operation.m_Token==token)
   {
    operation.Cancel();
    return true;
   }
  }
  return false;
 }
 protected bool Valid()
 {
  if(m_Released || !ActorValid(m_Actor,m_Bag) || !m_State || m_State.RemainsToken()!=m_Token || m_State.State()!=BCMM_BagCargoState.WORLD) return false;
  vector contact;vector normal;if(!BCMM_BurialSite.Trace(m_Bag,m_Actor,contact,normal)) return false;
  InventoryItemComponent item=InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
  if(!item || item.IsLocked() || item.GetParentSlot() || m_Bag.GetParent()) return false;
  PlayerManager players=GetGame().GetPlayerManager();
  if(m_PlayerId>0 && (!m_Controller || players.GetPlayerController(m_PlayerId)!=m_Controller || m_Controller.GetControlledEntity()!=m_Actor)) return false;
  if(m_PlayerId<=0 && players.GetPlayerIdFromControlledEntity(m_Actor)>0) return false;
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_Token);
  return record && record.Physical==m_Bag && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION;
 }
 protected void Tick() { if(!Valid()) Cancel(); }
 protected void CompleteDigging()
 {
  if(!Valid()){Cancel();return;}
  EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
  vector contact;vector normal;if(!BCMM_BurialSite.Trace(m_Bag,m_Actor,contact,normal)){Cancel();return;}
  vector bagTransform[4];m_Bag.GetWorldTransform(bagTransform);
  vector forward=bagTransform[0]-normal*vector.Dot(bagTransform[0],normal);
  if(forward.LengthSq()<0.0001) forward=bagTransform[2]-normal*vector.Dot(bagTransform[2],normal);
  if(forward.LengthSq()<0.0001){Cancel();return;}
  forward.Normalize();Math3D.DirectionAndUpMatrix(forward,normal,spawn.Transform);
  for(int axis=0;axis<3;axis++)spawn.Transform[axis]=spawn.Transform[axis]*m_GraveScale;
  spawn.Transform[3]=Vector(m_Bag.GetOrigin()[0],contact[1]-0.1,m_Bag.GetOrigin()[2]);
  m_Grave=GetGame().SpawnEntityPrefab(Resource.Load("{6AB4B3F3FD21E001}Prefabs/Graves/BCMM_Grave.et"),GetGame().GetWorld(),spawn);
  BCMM_GraveMarkerComponent marker;
  if(m_Grave) marker=BCMM_GraveMarkerComponent.Cast(m_Grave.FindComponent(BCMM_GraveMarkerComponent));
  if(!marker){Cancel();return;}
  BCMM_BurialSite.AlignGrave(m_Grave,m_Bag);
  marker.SetMemorial(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_Token));
  GetGame().GetCallqueue().CallLater(Commit,100,false);
 }
 protected void Commit()
 {
  if(!Valid() || !m_Grave){Cancel();return;}
  if(BCMM_BodyBagComponent.Id(m_Grave)==RplId.Invalid())
  {
   if(m_ReplicationWaits++<20){GetGame().GetCallqueue().CallLater(Commit,100,false);return;}
   Cancel();return;
  }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(!mode.BCMM_CommitRemains(m_Token,m_Bag,BCMM_RemainsDisposition.BURIED)){Cancel();return;}
  SCR_PlayerController recipient=SCR_PlayerController.Cast(m_Controller);
  if(recipient) recipient.BCMM_SendRemainsNotice(m_Token,mode.BCMM_RemainsByToken(m_Token).DeceasedName,BCMM_RemainsDisposition.BURIED);
  SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);
  Release();
 }
 protected void Cancel()
 {
  if(m_Released) return;
  if(m_Actor && m_Bag)
  {
   CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
   PrintFormat("BCMM_BURIAL cancel token=%1 distance=%2 shovel=%3 falling=%4 life=%5 grave=%6",m_Token,vector.Distance(m_Actor.GetOrigin(),m_Bag.GetOrigin()),HasShovel(m_Actor),controller.IsFalling(),controller.GetLifeState(),m_Grave);
  }
  if(m_Grave) SCR_EntityHelper.DeleteEntityAndChildren(m_Grave);
  if(m_State) m_State.SetReady(true);
  Release();
 }
 protected void Release()
 {
  m_Released=true;
  SCR_PlayerController presentation=SCR_PlayerController.Cast(m_Controller);
  if(presentation) presentation.BCMM_SetBurialAnimation(m_Token,false);
  GetGame().GetCallqueue().Remove(Tick);GetGame().GetCallqueue().Remove(CompleteDigging);GetGame().GetCallqueue().Remove(Commit);
  s_Active.RemoveItem(this);
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestBurial(IEntity bag) { if(GetGame().GetPlayerController()==this && bag) Rpc(BCMM_BurialRequest,BCMM_BodyBagComponent.Id(bag)); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_BurialRequest(RplId bag) { BCMM_BurialOperation.Begin(GetControlledEntity(),BCMM_BodyBagComponent.Resolve(bag)); }
}
class BCMM_BuryBagAction:ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user) { return BCMM_BurialOperation.CanBegin(user,GetOwner()); }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());if(player) player.BCMM_RequestBurial(pOwnerEntity);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
