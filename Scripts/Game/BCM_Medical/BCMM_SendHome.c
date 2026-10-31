// Native healing-area eligibility retains each station's own geometry/range.
modded class SCR_HealSupportStationComponent
{
 protected static ref array<SCR_HealSupportStationComponent> s_BCMMHealingStations={};
 override void OnPostInit(IEntity owner) { super.OnPostInit(owner);s_BCMMHealingStations.Insert(this); }
 override void OnDelete(IEntity owner) { s_BCMMHealingStations.RemoveItem(this);super.OnDelete(owner); }
 static bool BCMM_FindHealingArea(IEntity target)
 {
  if(!target) return false;
  foreach(SCR_HealSupportStationComponent station:s_BCMMHealingStations)
   if(station && station.GetOwner() && station.IsEnabled() && station.UsesRange() && station.IsInRange(target,target.GetOrigin())) return true;
  return false;
 }
}
class BCMM_SendHomeOperation
{
 static bool CanBegin(IEntity actor,IEntity bag)
 {
  if(!actor || !bag) return false;
  if(BCMM_BagSearchSessions.IsSearched(bag))return false;
  if(BCMM_BaggingOperation.Setting("sendHomeEnabled")<0.5) return false;
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);
  if(!character || character.IsInVehicle()) return false;
  CharacterControllerComponent controller=character.GetCharacterController();
  if(!controller || controller.GetLifeState()!=ECharacterLifeState.ALIVE || controller.IsUnconscious() || controller.IsSwimming() || controller.IsFalling()) return false;
  if(vector.Distance(actor.GetOrigin(),bag.GetOrigin())>BCMM_BaggingOperation.Setting("baggingRangeMetres")) return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  InventoryItemComponent item=InventoryItemComponent.Cast(bag.FindComponent(InventoryItemComponent));
  return state && state.Ready() && state.RemainsToken()>0 && state.State()==BCMM_BagCargoState.WORLD && state.DraggingActor()==RplId.Invalid() && !bag.GetParent() && item && !item.IsLocked() && !item.GetParentSlot() && SCR_HealSupportStationComponent.BCMM_FindHealingArea(bag);
 }
 static bool Begin(IEntity actor,IEntity bag)
 {
  if(!Replication.IsServer() || !CanBegin(actor,bag)) return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(state.RemainsToken());
  if(!record || record.Physical!=bag || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) return false;
  // Synchronous authority reservation and commit: no delayed actor can outlive
  // this request. Native range/station checks have just been repeated here.
  state.SetReady(false);
  if(!mode.BCMM_CommitRemains(state.RemainsToken(),bag,BCMM_RemainsDisposition.SENT_HOME)) { state.SetReady(true);return false; }
  int playerId=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  SCR_PlayerController recipient=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  if(recipient && recipient.GetControlledEntity()==actor) recipient.BCMM_SendRemainsNotice(record.Token,record.DeceasedName,BCMM_RemainsDisposition.SENT_HOME);
  SCR_EntityHelper.DeleteEntityAndChildren(bag);
  return true;
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestSendHome(IEntity bag) { if(GetGame().GetPlayerController()==this && bag) Rpc(BCMM_SendHomeRequest,BCMM_BodyBagComponent.Id(bag)); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_SendHomeRequest(RplId bag) { BCMM_SendHomeOperation.Begin(GetControlledEntity(),BCMM_BodyBagComponent.Resolve(bag)); }
}
class BCMM_SendHomeAction:ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user) { return BCMM_SendHomeOperation.CanBegin(user,GetOwner()); }
 override void PerformAction(IEntity pOwnerEntity,IEntity pUserEntity)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());if(player) player.BCMM_RequestSendHome(pOwnerEntity);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
