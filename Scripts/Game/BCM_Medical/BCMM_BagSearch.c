// Open inventory views are coordinated with authority. Native inventory still
// owns all item transactions; these expiring sessions only exclude operations.
class BCMM_BagSearchLease
{
 SCR_PlayerController Controller;
 IEntity Actor;
 IEntity Bag;
 int Generation;
 int Request;
 float Deadline;
}
class BCMM_BagSearchSessions
{
 protected static ref array<ref BCMM_BagSearchLease> s_Leases={};
 protected static int s_NextGeneration;
 protected static bool s_Ticking;
 static IEntity ContainingBag(IEntity entity)
 {
  for(int guard=0;entity && guard<64;guard++)
  {
   if(entity.FindComponent(BCMM_BodyBagComponent))return entity;
   entity=entity.GetParent();
  }
  return null;
 }
 static bool Eligible(IEntity actor,IEntity bag)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);
  if(!character || !bag || character.IsInVehicle() || vector.Distance(actor.GetOrigin(),bag.GetOrigin())>3)return false;
  CharacterControllerComponent controller=character.GetCharacterController();
  if(!controller || controller.GetLifeState()!=ECharacterLifeState.ALIVE || controller.IsUnconscious() || controller.IsSwimming() || controller.IsFalling())return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  return state && state.Ready() && state.State()==BCMM_BagCargoState.WORLD && state.DraggingActor()==RplId.Invalid() && !bag.GetParent();
 }
 protected static bool Valid(BCMM_BagSearchLease lease)
 {
  if(!lease.Controller || !lease.Actor || !lease.Bag || GetGame().GetWorld().GetWorldTime()>lease.Deadline)return false;
  PlayerManager players=GetGame().GetPlayerManager();
  return players.GetPlayerController(lease.Controller.GetPlayerId())==lease.Controller && lease.Controller.GetControlledEntity()==lease.Actor && Eligible(lease.Actor,lease.Bag);
 }
 protected static void Publish(IEntity bag)
 {
  if(!bag)return;
  bool searched;
  foreach(BCMM_BagSearchLease lease:s_Leases)
  {
   if(lease.Bag==bag){searched=true;break;}
  }
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if(state)state.SetSearching(searched);
 }
 protected static void Prune()
 {
  for(int index=s_Leases.Count()-1;index>=0;index--)
  {
   if(Valid(s_Leases[index]))continue;
   IEntity bag=s_Leases[index].Bag;
   s_Leases.Remove(index);
   Publish(bag);
  }
 }
 protected static void Tick()
 {
  Prune();
  if(s_Leases.IsEmpty())
  {
   GetGame().GetCallqueue().Remove(Tick);
   s_Ticking=false;
  }
 }
 static bool IsSearched(IEntity bag)
 {
  if(!bag)return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if(!Replication.IsServer())return state && state.Searching();
  Prune();
  foreach(BCMM_BagSearchLease lease:s_Leases)
  {
   if(lease.Bag==bag)return true;
  }
  return false;
 }
 static int Acquire(SCR_PlayerController controller,IEntity bag,int request)
 {
  if(!Replication.IsServer() || request<=0 || !controller || controller.GetPlayerId()<=0 || GetGame().GetPlayerManager().GetPlayerController(controller.GetPlayerId())!=controller || !Eligible(controller.GetControlledEntity(),bag))return 0;
  Prune();
  foreach(BCMM_BagSearchLease existing:s_Leases)
  {
   if(existing.Controller==controller && existing.Bag==bag && existing.Request==request)
   {
    existing.Deadline=GetGame().GetWorld().GetWorldTime()+3500;
    return existing.Generation;
   }
  }
  for(int index=s_Leases.Count()-1;index>=0;index--)
  {
   if(s_Leases[index].Controller==controller && s_Leases[index].Bag==bag)s_Leases.Remove(index);
  }
  BCMM_BagSearchLease lease=new BCMM_BagSearchLease();
  lease.Controller=controller;lease.Actor=controller.GetControlledEntity();lease.Bag=bag;
  lease.Generation=++s_NextGeneration;lease.Request=request;lease.Deadline=GetGame().GetWorld().GetWorldTime()+3500;
  s_Leases.Insert(lease);Publish(bag);
  if(!s_Ticking){s_Ticking=true;GetGame().GetCallqueue().CallLater(Tick,500,true);}
  return lease.Generation;
 }
 static bool Renew(SCR_PlayerController controller,int generation)
 {
  if(!Replication.IsServer())return false;
  Prune();
  foreach(BCMM_BagSearchLease lease:s_Leases)
  {
   if(lease.Controller==controller && lease.Generation==generation)
   {
    lease.Deadline=GetGame().GetWorld().GetWorldTime()+3500;
    return true;
   }
  }
  return false;
 }
 static void Release(SCR_PlayerController controller,int generation)
 {
  if(!Replication.IsServer())return;
  for(int index=s_Leases.Count()-1;index>=0;index--)
  {
   if(s_Leases[index].Controller!=controller || s_Leases[index].Generation!=generation)continue;
   IEntity bag=s_Leases[index].Bag;s_Leases.Remove(index);Publish(bag);
  }
 }
 static void Reset()
 {
  GetGame().GetCallqueue().Remove(Tick);s_Ticking=false;
  foreach(BCMM_BagSearchLease lease:s_Leases)
  {
   BCMM_BodyBagComponent state;
   if(lease.Bag)state=BCMM_BodyBagComponent.Cast(lease.Bag.FindComponent(BCMM_BodyBagComponent));
   if(state)state.SetSearching(false);
  }
  s_Leases.Clear();
 }
}
class BCMM_BagSearchView
{
 IEntity Bag;
 int Generation;
 int Request;
 float GraceUntil;
}
modded class SCR_BaseGameMode
{
 override void OnGameStart()
 {
  super.OnGameStart();
  if(Replication.IsServer())BCMM_BagSearchSessions.Reset();
 }
}
modded class SCR_PlayerController
{
 protected ref array<ref BCMM_BagSearchView> m_BCMMSearchViews={};
 protected bool m_BCMMSearchViewTicking;
 protected int m_BCMMSearchRequest;
 protected BCMM_BagSearchView BCMM_FindSearchView(IEntity bag)
 {
  foreach(BCMM_BagSearchView view:m_BCMMSearchViews)
  {
   if(view.Bag==bag)return view;
  }
  return null;
 }
 void BCMM_RequestBagSearch(IEntity bag,bool openInventory=true)
 {
  if(GetGame().GetPlayerController()!=this || !BCMM_BagSearchSessions.Eligible(GetControlledEntity(),bag))return;
  if(BCMM_FindSearchView(bag))return;
  BCMM_BagSearchView view=new BCMM_BagSearchView();view.Bag=bag;view.Request=++m_BCMMSearchRequest;view.GraceUntil=GetGame().GetWorld().GetWorldTime()+3000;
  m_BCMMSearchViews.Insert(view);
  Rpc(BCMM_AskBagSearch,BCMM_BodyBagComponent.Id(bag),openInventory,view.Request);
  if(!m_BCMMSearchViewTicking){m_BCMMSearchViewTicking=true;GetGame().GetCallqueue().CallLater(BCMM_SearchViewTick,500,true);}
 }
 void BCMM_ObserveNativeBagSearch(IEntity bag)
 {
  if(GetGame().GetPlayerController()!=this)return;
  if(BCMM_BagSearchSessions.Eligible(GetControlledEntity(),bag))BCMM_RequestBagSearch(bag,false);
  else GetGame().GetCallqueue().CallLater(BCMM_CloseBagViews,1,false,bag);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_AskBagSearch(RplId id,bool openInventory,int request)
 {
  int generation=BCMM_BagSearchSessions.Acquire(this,BCMM_BodyBagComponent.Resolve(id),request);
  if(GetGame().GetPlayerController()==this)BCMM_SearchDecision(id,generation,openInventory,request);
  else Rpc(BCMM_SearchDecision,id,generation,openInventory,request);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_SearchDecision(RplId id,int generation,bool openInventory,int request)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(id);
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(!view || view.Request!=request)
  {
   if(generation>0)Rpc(BCMM_AskCloseBagSearch,generation);
   return;
  }
  if(!view || generation<=0 || !BCMM_BagSearchSessions.Eligible(GetControlledEntity(),bag))
  {
   if(generation>0)Rpc(BCMM_AskCloseBagSearch,generation);
   BCMM_CloseBagViews(bag);
   if(view)m_BCMMSearchViews.RemoveItem(view);
   return;
  }
  view.Generation=generation;view.GraceUntil=GetGame().GetWorld().GetWorldTime()+2000;
  if(openInventory)
  {
   SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
   if(inventory)
   {
    CharacterVicinityComponent vicinity=CharacterVicinityComponent.Cast(GetControlledEntity().FindComponent(CharacterVicinityComponent));
    if(vicinity)
    {
     array<IEntity> discovered={};vicinity.GetAvailableItems(discovered);vicinity.ManipulationComplete();
    }
    inventory.CloseInventory();
    BCMM_OpenSearchInventory(id,generation,request);
   }
  }
 }
 protected void BCMM_OpenSearchInventory(RplId id,int generation,int request)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(id);
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(!view || view.Generation!=generation || view.Request!=request || !BCMM_BagSearchSessions.Eligible(GetControlledEntity(),bag))return;
  SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
  if(!inventory)return;
  inventory.SetStorageToOpen(bag);inventory.OpenInventory();
  GetGame().GetCallqueue().CallLater(BCMM_EnsureSearchNavigation,150,false,id,generation,request);
 }
 protected void BCMM_EnsureSearchNavigation(RplId id,int generation,int request)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(id);
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(!view || view.Generation!=generation || view.Request!=request || !BCMM_BagSearchSessions.Eligible(GetControlledEntity(),bag) || BCMM_BagViewOpen(bag))return;
  SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
  SCR_InventoryMenuUI menu=SCR_InventoryMenuUI.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.Inventory20Menu));
  if(inventory)
  {
   inventory.SetStorageToOpen(bag);
   if(menu)menu.SetOpenStorage();
   else inventory.OpenInventory();
  }
  if(GetGame().GetWorld().GetWorldTime()<view.GraceUntil)GetGame().GetCallqueue().CallLater(BCMM_EnsureSearchNavigation,150,false,id,generation,request);
 }
 protected bool BCMM_BagViewOpen(IEntity bag)
 {
  SCR_InventoryMenuUI menu=SCR_InventoryMenuUI.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.Inventory20Menu));
  if(!menu || !bag)return false;
  SCR_InventoryStorageBaseUI loot=menu.GetLootStorage();
  if(loot && loot.GetCurrentNavigationStorage() && BCMM_BagSearchSessions.ContainingBag(loot.GetCurrentNavigationStorage().GetOwner())==bag)return true;
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent));
  array<BaseInventoryStorageComponent> storages={};
  if(inventory)inventory.GetStorages(storages,EStoragePurpose.PURPOSE_ANY);
  foreach(BaseInventoryStorageComponent storage:storages)
  {
   if(menu.GetOpenedStorage(storage))return true;
   SCR_InventoryStorageBaseUI ui=menu.GetStorageUIByBaseStorageComponent(storage);
   if(ui && ui.GetCurrentNavigationStorage() && BCMM_BagSearchSessions.ContainingBag(ui.GetCurrentNavigationStorage().GetOwner())==bag)return true;
  }
  return false;
 }
 protected void BCMM_CloseBagViews(IEntity bag)
 {
  SCR_InventoryMenuUI menu=SCR_InventoryMenuUI.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.Inventory20Menu));
  if(!menu || !bag)return;
  SCR_InventoryStorageBaseUI loot=menu.GetLootStorage();
  if(loot && loot.GetCurrentNavigationStorage() && BCMM_BagSearchSessions.ContainingBag(loot.GetCurrentNavigationStorage().GetOwner())==bag)loot.Home();
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent));
  array<BaseInventoryStorageComponent> storages={};
  if(inventory)inventory.GetStorages(storages,EStoragePurpose.PURPOSE_ANY);
  foreach(BaseInventoryStorageComponent storage:storages)
  {
   SCR_InventoryOpenedStorageUI opened=menu.GetOpenedStorage(storage);
   if(opened){opened.CloseStorage();continue;}
   SCR_InventoryStorageBaseUI ui=menu.GetStorageUIByBaseStorageComponent(storage);
   if(ui && ui.GetCurrentNavigationStorage() && BCMM_BagSearchSessions.ContainingBag(ui.GetCurrentNavigationStorage().GetOwner())==bag)ui.Home();
  }
 }
 protected void BCMM_SearchViewTick()
 {
  if(GetGame().GetPlayerController()!=this)
  {
   m_BCMMSearchViews.Clear();GetGame().GetCallqueue().Remove(BCMM_SearchViewTick);m_BCMMSearchViewTicking=false;return;
  }
  for(int index=m_BCMMSearchViews.Count()-1;index>=0;index--)
  {
   BCMM_BagSearchView view=m_BCMMSearchViews[index];
   bool eligible=BCMM_BagSearchSessions.Eligible(GetControlledEntity(),view.Bag);
   bool open=BCMM_BagViewOpen(view.Bag);
   if(!eligible || (!open && GetGame().GetWorld().GetWorldTime()>view.GraceUntil))
   {
    if(view.Generation>0)Rpc(BCMM_AskCloseBagSearch,view.Generation);
    if(!eligible)BCMM_CloseBagViews(view.Bag);
    m_BCMMSearchViews.Remove(index);
   }
   else if(open && view.Generation>0)Rpc(BCMM_AskRenewBagSearch,BCMM_BodyBagComponent.Id(view.Bag),view.Generation);
  }
  if(m_BCMMSearchViews.IsEmpty())
  {
   GetGame().GetCallqueue().Remove(BCMM_SearchViewTick);m_BCMMSearchViewTicking=false;
  }
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_AskRenewBagSearch(RplId bag,int generation)
 {
  if(!BCMM_BagSearchSessions.Renew(this,generation))
  {
   if(GetGame().GetPlayerController()==this)BCMM_SearchRenewDenied(bag,generation);
   else Rpc(BCMM_SearchRenewDenied,bag,generation);
  }
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_SearchRenewDenied(RplId id,int generation)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(id);
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(!view || view.Generation!=generation)return;
  BCMM_CloseBagViews(bag);m_BCMMSearchViews.RemoveItem(view);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_AskCloseBagSearch(int generation) { BCMM_BagSearchSessions.Release(this,generation); }
 void ~SCR_PlayerController()
 {
  if(GetGame() && GetGame().GetCallqueue())
  {
   GetGame().GetCallqueue().Remove(BCMM_SearchViewTick);
   GetGame().GetCallqueue().Remove(BCMM_OpenSearchInventory);
   GetGame().GetCallqueue().Remove(BCMM_EnsureSearchNavigation);
   GetGame().GetCallqueue().Remove(BCMM_CloseBagViews);
  }
 }
}
// Also covers containers opened directly from native vicinity/nested UI.
modded class SCR_InventoryStorageBaseUI
{
 protected void BCMM_ObserveBagView()
 {
  BaseInventoryStorageComponent storage=GetCurrentNavigationStorage();
  if(!storage)return;
  IEntity bag=BCMM_BagSearchSessions.ContainingBag(storage.GetOwner());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(player && bag)player.BCMM_ObserveNativeBagSearch(bag);
 }
 override void HandlerAttached(Widget w) { super.HandlerAttached(w);BCMM_ObserveBagView(); }
 override void Traverse(BaseInventoryStorageComponent storage) { super.Traverse(storage);BCMM_ObserveBagView(); }
}
