// Actual native open/close inventory, two controllers and original-session CAS.
modded class SCR_InventoryStorageManagerComponent
{
 bool BCMM_TestInventoryPermitted() { return m_bInventoryAccessAllowed; }
}
modded class BCMM_BagSearchSessions
{
 static int BCMM_TestGeneration(SCR_PlayerController controller,IEntity bag)
 {
  Prune();
  foreach(BCMM_BagSearchLease lease:s_Leases)
  {
   if(lease.Controller==controller && lease.Bag==bag)return lease.Generation;
  }
  return 0;
 }
}
modded class SCR_BaseGameMode
{
 protected ref map<int,int> m_BCMMSearchShown=new map<int,int>();
 void BCMM_TestSearchShown(int playerId,int generation) { m_BCMMSearchShown.Set(playerId,generation); }
 int BCMM_TestSearchShownGeneration(int playerId) { return m_BCMMSearchShown.Get(playerId); }
}
modded class SCR_PlayerController
{
 void BCMM_TestSearchOpen(RplId target,bool nativeInventory=false) { Rpc(BCMM_TestSearchOpenOwner,target,nativeInventory); }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_TestSearchOpenOwner(RplId target,bool nativeInventory)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(target);
  if(nativeInventory)
  {
   SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
   inventory.SetStorageToOpen(bag);inventory.OpenInventory();
   GetGame().GetCallqueue().CallLater(BCMM_TestAwaitSearchView,250,false,target,0);
   Print("BCMM_SEARCH_CLIENT direct native inventory reopen without custom Search action");
   return;
  }
  ActionsManagerComponent manager;
  if(bag)manager=ActionsManagerComponent.Cast(bag.FindComponent(ActionsManagerComponent));
  array<BaseUserAction> actions={};if(manager)manager.GetActionsList(actions);
  BCMM_SearchBagAction search;
  foreach(BaseUserAction action:actions)
  {
   search=BCMM_SearchBagAction.Cast(action);
   if(search)break;
  }
  if(!search){Print("BCMM_SEARCH_CLIENT no registered search action",LogLevel.ERROR);return;}
  PrintFormat("BCMM_SEARCH_CLIENT action actor=%1 bag=%2 distance=%3",GetControlledEntity().GetOrigin(),bag.GetOrigin(),vector.Distance(GetControlledEntity().GetOrigin(),bag.GetOrigin()));
  search.PerformAction(bag,GetControlledEntity());
  GetGame().GetCallqueue().CallLater(BCMM_TestAwaitSearchView,250,false,target,0);
 }
 protected void BCMM_TestAwaitSearchView(RplId target,int attempts)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(target);
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(attempts%10==0)
  {
   int generation;if(view)generation=view.Generation;
   SCR_InventoryMenuUI menu=SCR_InventoryMenuUI.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.Inventory20Menu));
   SCR_InventoryStorageBaseUI loot;if(menu)loot=menu.GetLootStorage();
   BaseInventoryStorageComponent navigation;if(loot)navigation=loot.GetCurrentNavigationStorage();
   PrintFormat("BCMM_SEARCH_CLIENT waiting attempt=%1 generation=%2 menu=%3 navigation=%4",attempts,generation,menu,navigation);
   IEntity actor=GetControlledEntity();
   SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(actor.FindComponent(SCR_InventoryStorageManagerComponent));
   PrintFormat("BCMM_SEARCH_CLIENT controlled life=%1 owner=%2 inventoryPermitted=%3",ChimeraCharacter.Cast(actor).GetCharacterController().GetLifeState(),RplComponent.Cast(actor.FindComponent(RplComponent)).IsOwner(),inventory.BCMM_TestInventoryPermitted());
   PrintFormat("BCMM_SEARCH_CLIENT eligible=%1 falling=%2 distance=%3",BCMM_BagSearchSessions.Eligible(actor,bag),ChimeraCharacter.Cast(actor).GetCharacterController().IsFalling(),vector.Distance(actor.GetOrigin(),bag.GetOrigin()));
  }
  if(view && view.Generation>0 && BCMM_BagViewOpen(bag))
  {
   PrintFormat("BCMM_SEARCH_CLIENT VIEW_PASS generation=%1 nativeOpen=1",view.Generation);
   Rpc(BCMM_TestSearchShownServer,view.Generation);
   return;
  }
  if(attempts<30){GetGame().GetCallqueue().CallLater(BCMM_TestAwaitSearchView,250,false,target,attempts+1);return;}
  Print("BCMM_SEARCH_CLIENT actual native bag view did not open",LogLevel.ERROR);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_TestSearchShownServer(int generation) { SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestSearchShown(GetPlayerId(),generation); }
 void BCMM_TestSearchClose() { Rpc(BCMM_TestSearchCloseOwner); }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_TestSearchCloseOwner()
 {
  SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
  inventory.CloseInventory();
 }
 void BCMM_TestStaleSearchReply(RplId target,int stale) { Rpc(BCMM_TestStaleSearchReplyOwner,target,stale); }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_TestStaleSearchReplyOwner(RplId target,int stale)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(target);
  BCMM_BagSearchView current=BCMM_FindSearchView(bag);
  int generation;if(current)generation=current.Generation;
  int request;if(current)request=current.Request;
  if(request>1)
  {
   BCMM_SearchDecision(target,stale,false,request-1);
   BCMM_SearchDecision(target,0,false,request-1);
  }
  BCMM_SearchRenewDenied(target,stale);
  current=BCMM_FindSearchView(bag);
  if(current && current.Generation==generation && generation!=stale && BCMM_BagViewOpen(bag))Print("BCMM_SEARCH_CLIENT STALE_REPLY_PASS current native view retained");
  else Print("BCMM_SEARCH_CLIENT stale reply closed current native view",LogLevel.ERROR);
 }
}
modded class BCMM_VehicleLifecycleChecks
{
 protected SCR_PlayerController m_SearchPeer;
 protected int m_SearchStage;
 protected int m_SearchWaits;
 protected int m_SearchFirstGeneration;
 protected int m_SearchPeerGeneration;
 protected int m_SearchNextGeneration;
 protected int m_SearchPeerId;
 protected int m_SearchInitialRevision;
 protected IEntity m_SearchReplacement;
 override protected void RequestHomeOnOwner()
 {
  Check(BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag),"search contention fixture initially permits actual native return-home");
  if(m_Failures){Finish();return;}
  m_SearchInitialRevision=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).Revision();
  array<int> players={};GetGame().GetPlayerManager().GetPlayers(players);
  foreach(int id:players)
  {
   SCR_PlayerController peer=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
   if(peer && peer!=m_Controller && peer.GetControlledEntity()){m_SearchPeer=peer;break;}
  }
  Check(m_SearchPeer!=null,"two real native controllers can independently search the same world bag");
  if(!m_SearchPeer){Finish();return;}
  m_SearchPeerId=m_SearchPeer.GetPlayerId();
  m_Controller.BCMM_TestSearchOpen(BCMM_BodyBagComponent.Id(m_Bag));
  GetGame().GetCallqueue().CallLater(SearchStage,250,false);
 }
 protected bool SearchShown(SCR_PlayerController player,int generation)
 {
  return generation>0 && SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestSearchShownGeneration(player.GetPlayerId())==generation;
 }
 protected void SearchStage()
 {
  int owner=BCMM_BagSearchSessions.BCMM_TestGeneration(m_Controller,m_Bag);
  int peer=BCMM_BagSearchSessions.BCMM_TestGeneration(m_SearchPeer,m_Bag);
  bool next;
  if(m_SearchStage==0 && SearchShown(m_Controller,owner))
  {
   m_SearchFirstGeneration=owner;
   Check(BCMM_BagSearchSessions.IsSearched(m_Bag) && !BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag) && !BCMM_SendHomeOperation.Begin(m_Actor,m_Bag) && m_Bag,"actual native open search excludes authority return-home without disposing bag");
   SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
   BaseInventoryStorageComponent storage=BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_Bag.GetPrefabData().GetPrefabName());
   Check(storage && !inventory.TryInsertItemInStorage(m_Bag,storage),"native direct cargo insertion cannot move an actively searched bag");
   Check(!BCMM_CargoOperation.Begin(m_Actor,m_Vehicle,m_Bag,true,BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).Revision()),"authority cargo action excludes actual active search");
   Check(!BCMM_NativeBagDrag.Begin(m_Actor,m_Bag,false,BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).Revision()),"authority manual dragging excludes actual active search");
   m_SearchPeer.BCMM_VehicleMove(m_Bag.GetOrigin()+"1.5 0.2 1");
   m_SearchStage=1;m_SearchWaits=0;
   GetGame().GetCallqueue().CallLater(OpenSecondSearch,1000,false);return;
  }
  if(m_SearchStage==2 && SearchShown(m_SearchPeer,peer))
  {
   m_SearchPeerGeneration=peer;
   Check(owner==m_SearchFirstGeneration && peer!=owner,"simultaneous native inventory views have independent original-controller generations");
   m_Controller.BCMM_TestSearchClose();m_SearchStage=3;next=true;
  }
  else if(m_SearchStage==3 && owner==0 && peer==m_SearchPeerGeneration)
  {
   Check(BCMM_BagSearchSessions.IsSearched(m_Bag) && !BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag),"closing one actual inventory cannot release another player's open search");
   m_Controller.BCMM_TestSearchOpen(BCMM_BodyBagComponent.Id(m_Bag),true);m_SearchStage=4;next=true;
  }
  else if(m_SearchStage==4 && owner!=m_SearchFirstGeneration && SearchShown(m_Controller,owner))
  {
   m_SearchNextGeneration=owner;
   BCMM_BagSearchSessions.Release(m_Controller,m_SearchFirstGeneration);
   Check(!BCMM_BagSearchSessions.Renew(m_Controller,m_SearchFirstGeneration) && BCMM_BagSearchSessions.BCMM_TestGeneration(m_Controller,m_Bag)==owner,"stale close or heartbeat cannot release a reopened search on the same original bag");
   m_Controller.BCMM_TestStaleSearchReply(BCMM_BodyBagComponent.Id(m_Bag),m_SearchFirstGeneration);
   BCMM_BagSearchSessions.Release(m_Controller,m_SearchPeerGeneration);
   Check(BCMM_BagSearchSessions.BCMM_TestGeneration(m_SearchPeer,m_Bag)==peer,"one controller cannot close another controller's search generation");
   string disconnected;
   if(System.GetCLIParam("bcmmSearchDisconnect",disconnected))PrintFormat("BCMM_SEARCH waitingDisconnect player=%1",m_SearchPeerId);
   else m_SearchPeer.BCMM_TestSearchClose();
   m_SearchStage=5;next=true;
  }
  else if(m_SearchStage==5 && peer==0 && owner==m_SearchNextGeneration)
  {
   string disconnected;
   if(System.GetCLIParam("bcmmSearchDisconnect",disconnected))
   {
    array<int> connected={};GetGame().GetPlayerManager().GetPlayers(connected);
    if(connected.Contains(m_SearchPeerId))
    {
     if(m_SearchWaits++>180){Check(false,"native server observes exact searcher's disconnect");Finish();return;}
     GetGame().GetCallqueue().CallLater(SearchStage,250,false);return;
    }
    Check(true,"native server observes disconnected searcher and its expired original lease");
    PrintFormat("BCMM_SEARCH disconnected=1 player=%1",m_SearchPeerId);
   }
   Check(BCMM_BagSearchSessions.IsSearched(m_Bag),"original owner's reopened native view still excludes disposal after peer closes");
   m_SearchReplacement=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Bag.GetOrigin()+"1.5 0.2 -1");
   Check(m_SearchReplacement!=null,"native search possession fixture creates distinct nearby live character");
   if(!m_SearchReplacement){Finish();return;}
   AIControlComponent ai=AIControlComponent.Cast(m_SearchReplacement.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
   m_Controller.SetPossessedEntity(m_SearchReplacement);
   m_SearchStage=6;next=true;
  }
  else if(m_SearchStage==6 && owner==0 && peer==0 && m_Controller.GetControlledEntity()==m_SearchReplacement && BCMM_BagSearchSessions.Eligible(m_SearchReplacement,m_Bag))
  {
   Check(!BCMM_BagSearchSessions.Renew(m_Controller,m_SearchNextGeneration) && !BCMM_BagSearchSessions.IsSearched(m_Bag),"changing actual native controlled character invalidates original search even while replacement is eligible beside bag");
   m_Controller.SetPossessedEntity(null);
   AIControlComponent ai=AIControlComponent.Cast(m_SearchReplacement.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
   GetGame().GetCallqueue().CallLater(ReopenOriginalCharacterSearch,1000,false);return;
  }
  else if(m_SearchStage==7 && owner!=m_SearchNextGeneration && SearchShown(m_Controller,owner))
  {
   Check(m_Controller.GetControlledEntity()==m_Actor && owner>m_SearchNextGeneration,"restored actual original character receives fresh search generation");
   SCR_EntityHelper.DeleteEntityAndChildren(m_SearchReplacement);
   m_Controller.BCMM_VehicleMove(m_Bag.GetOrigin()+"20 0.2 0");m_SearchStage=8;next=true;
  }
  else if(m_SearchStage==8 && owner==0 && peer==0 && vector.Distance(m_Actor.GetOrigin(),m_Bag.GetOrigin())>10)
  {
   Check(!BCMM_BagSearchSessions.IsSearched(m_Bag) && !BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).Searching(),"moving original owner away clears authority and replicated search reservation");
   m_Controller.BCMM_VehicleMove(m_Bag.GetOrigin()+"1 0.2 0");
   GetGame().GetCallqueue().CallLater(HomeAfterSearch,1500,false);return;
  }
  if(next)m_SearchWaits=0;
  if(m_SearchWaits++>180){Check(false,"actual native search-session stage completes before deadline");Finish();return;}
  if(m_Failures){Finish();return;}
  GetGame().GetCallqueue().CallLater(SearchStage,250,false);
 }
 protected void OpenSecondSearch()
 {
  m_SearchPeer.BCMM_TestSearchOpen(BCMM_BodyBagComponent.Id(m_Bag));m_SearchStage=2;
  GetGame().GetCallqueue().CallLater(SearchStage,250,false);
 }
 protected void HomeAfterSearch()
 {
  Check(BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag),"closing and leaving native searches restores real home eligibility");
  Check(BCMM_CargoOperation.ActorCanUse(m_Actor,m_Vehicle,m_Bag),"old transport revision is tested with otherwise valid actual cargo reach, actor and stationary vehicle");
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(state.Revision()>m_SearchInitialRevision && !BCMM_CargoOperation.Begin(m_Actor,m_Vehicle,m_Bag,true,m_SearchInitialRevision),"transport request captured before search cannot execute after newer search generations close");
  super.RequestHomeOnOwner();
 }
 protected void ReopenOriginalCharacterSearch()
 {
  m_Controller.BCMM_TestSearchOpen(BCMM_BodyBagComponent.Id(m_Bag));m_SearchStage=7;m_SearchWaits=0;
  GetGame().GetCallqueue().CallLater(SearchStage,250,false);
 }
}
