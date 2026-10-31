// Dedicated owner/proxy/JIP acceptance on native Arland terrain.
modded class SCR_BaseGameMode
{
 [RplProp()] int m_BCMMBurialNetPhase;
 [RplProp()] int m_BCMMBurialNetOwner;
 [RplProp()] int m_BCMMBurialNetToken;
 [RplProp()] RplId m_BCMMBurialNetActor;
 [RplProp()] RplId m_BCMMBurialNetBag;
 [RplProp()] RplId m_BCMMBurialNetGrave;
 [RplProp()] RplId m_BCMMBurialNetTool;
 [RplProp()] string m_BCMMBurialNetName;
 [RplProp()] vector m_BCMMBurialNetSite;
 protected ref array<int> m_BCMMBurialNetReady={};
 protected ref array<int> m_BCMMBurialNetDig={};
 protected ref array<int> m_BCMMBurialNetMemorial={};
 protected bool m_BCMMBurialSearchBlocked;
 bool BCMM_BurialSearchWasBlocked() { return m_BCMMBurialSearchBlocked; }
 void BCMM_BurialSearchAttempt(int playerId,RplId target)
 {
  IEntity bag=BCMM_BodyBagComponent.Resolve(target);
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  IEntity actor;if(player)actor=player.GetControlledEntity();
  vector point;vector normal;
  BCMM_BodyBagComponent state;if(bag)state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if(playerId==m_BCMMBurialNetOwner && target==m_BCMMBurialNetBag && actor && bag && BCMM_BurialSite.HeldTool(actor) && vector.Distance(actor.GetOrigin(),bag.GetOrigin())<3 && BCMM_BurialSite.Trace(bag,actor,point,normal) && state && state.Ready() && BCMM_BagSearchSessions.IsSearched(bag) && !BCMM_BurialOperation.CanBegin(actor,bag))
  {
   m_BCMMBurialSearchBlocked=true;
   Print("BCMM_BURIAL_SEARCH authority refused actual owner burial RPC while native inventory was open");
  }
  else Print("BCMM_BURIAL_SEARCH native open search did not exclude otherwise eligible burial",LogLevel.ERROR);
 }
 vector BCMM_BurialNetSpawn(int playerId) { return m_BCMMBurialNetSite+Vector(3,0.3,playerId); }
 void BCMM_BurialNetTool(IEntity tool) { m_BCMMBurialNetTool=BCMM_BodyBagComponent.Id(tool);Replication.BumpMe(); }
 void BCMM_BurialNetReady(int id) { if(!m_BCMMBurialNetReady.Contains(id))m_BCMMBurialNetReady.Insert(id); }
 int BCMM_BurialNetReadyCount() { return m_BCMMBurialNetReady.Count(); }
 int BCMM_BurialNetFirstOwner()
 {
  if(m_BCMMBurialNetReady.Count())
  {
   return m_BCMMBurialNetReady[0];
  }
  return 0;
 }
 void BCMM_BurialNetAck(int id,int phase)
 {
  if(phase==1 && !m_BCMMBurialNetDig.Contains(id))m_BCMMBurialNetDig.Insert(id);
  if(phase==2 && !m_BCMMBurialNetMemorial.Contains(id))m_BCMMBurialNetMemorial.Insert(id);
 }
 int BCMM_BurialNetDigCount() { return m_BCMMBurialNetDig.Count(); }
 int BCMM_BurialNetMemorialCount() { return m_BCMMBurialNetMemorial.Count(); }
 void BCMM_BurialNetPublish(int phase,IEntity actor,IEntity bag,IEntity grave,int token,string name,int playerId)
 {
  m_BCMMBurialNetPhase=phase;
  m_BCMMBurialNetActor=BCMM_BodyBagComponent.Id(actor);
  if(bag)m_BCMMBurialNetBag=BCMM_BodyBagComponent.Id(bag);
  m_BCMMBurialNetGrave=BCMM_BodyBagComponent.Id(grave);
  m_BCMMBurialNetToken=token;
  m_BCMMBurialNetName=name;
  m_BCMMBurialNetOwner=playerId;
  if(actor)m_BCMMBurialNetSite=actor.GetOrigin();
  Replication.BumpMe();
  PrintFormat("BCMM_BURIAL_NET_PHASE %1",phase);
 }
}
modded class SCR_PlayerController
{
 protected bool m_BCMMBurialSearchRequested;
 protected bool m_BCMMBurialSearchAttempted;
 bool BCMM_TestBurialSearchComplete(IEntity bag)
 {
  if(!m_BCMMBurialSearchRequested)
  {
   ActionsManagerComponent manager=ActionsManagerComponent.Cast(bag.FindComponent(ActionsManagerComponent));
   array<BaseUserAction> actions={};if(manager)manager.GetActionsList(actions);
   foreach(BaseUserAction action:actions)
   {
    BCMM_SearchBagAction search=BCMM_SearchBagAction.Cast(action);
    if(search){search.PerformAction(bag,GetControlledEntity());m_BCMMBurialSearchRequested=true;break;}
   }
   return false;
  }
  BCMM_BagSearchView view=BCMM_FindSearchView(bag);
  if(!m_BCMMBurialSearchAttempted)
  {
   if(!view || view.Generation<=0 || !BCMM_BagViewOpen(bag))return false;
   m_BCMMBurialSearchAttempted=true;
   BCMM_RequestBurial(bag);
   Rpc(BCMM_TestBurialSearchAttemptServer,BCMM_BodyBagComponent.Id(bag));
   Print("BCMM_BURIAL_SEARCH owner sent production burial RPC with actual native bag inventory open");
   GetGame().GetCallqueue().CallLater(BCMM_TestCloseBurialSearch,750,false);
   return false;
  }
  return !view && !BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent)).Searching();
 }
 protected void BCMM_TestCloseBurialSearch()
 {
  SCR_InventoryStorageManagerComponent.Cast(GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent)).CloseInventory();
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_TestBurialSearchAttemptServer(RplId bag)
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BurialSearchAttempt(GetPlayerId(),bag);
 }
 void BCMM_BurialNetReady() { Rpc(BCMM_BurialNetReadyServer); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_BurialNetReadyServer() { SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BurialNetReady(GetPlayerId()); }
 void BCMM_BurialNetAck(int phase) { Rpc(BCMM_BurialNetAckServer,phase); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_BurialNetAckServer(int phase) { SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BurialNetAck(GetPlayerId(),phase); }
 void BCMM_BurialNetEquip(RplId tool) { Rpc(BCMM_BurialNetEquipOwner,tool); }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_BurialNetEquipOwner(RplId tool)
 {
  IEntity actor=GetControlledEntity();
  IEntity held=BCMM_BodyBagComponent.Resolve(tool);
  if(actor && held)SCR_GadgetManagerComponent.GetGadgetManager(actor).SetGadgetMode(held,EGadgetMode.IN_HAND);
 }
}
class BCMM_BurialNetworkChecks:BCMM_BurialRuntimeChecks
{
 protected int m_NetWaits;
 protected SCR_PlayerController m_NetOwner;
 override void Start(BCMM_DomainChecks suite)
 {
  super.Start(suite);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BurialNetPublish(0,m_Actor,null,null,0,"",0);
 }
 override protected void Prepare()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode.BCMM_BurialNetReadyCount()<2)
  {
   if(m_NetWaits++<160){GetGame().GetCallqueue().CallLater(Prepare,250,false);return;}
   m_Suite.Record(false,"dedicated burial fixture receives two actual ready clients");Finish();return;
  }
  super.Prepare();
 }
 override protected void BeginFullBurial()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  int playerId=mode.BCMM_BurialNetFirstOwner();
  m_NetOwner=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  m_Suite.Record(m_NetOwner!=null,"network burial has original performing native controller");
  if(!m_NetOwner){Finish();return;}
  RplComponent.Cast(m_Actor.FindComponent(RplComponent)).GiveExt(m_NetOwner.GetRplIdentity(),false);
  m_NetOwner.SetInitialMainEntity(m_Actor);
  m_NetOwner.BCMM_BurialNetEquip(BCMM_BodyBagComponent.Id(m_Shovel));
  mode.BCMM_BurialNetTool(m_Shovel);
  m_BurialStarted=GetGame().GetWorld().GetWorldTime();
  mode.BCMM_BurialNetPublish(1,m_Actor,m_Bag,null,m_RemainsToken,"BCM bagging test casualty",playerId);
  m_NetWaits=0;
  GetGame().GetCallqueue().CallLater(AwaitNetworkBurial,500,false);
 }
 protected void AwaitNetworkBurial()
 {
  if(m_Bag && m_NetWaits++<110){GetGame().GetCallqueue().CallLater(AwaitNetworkBurial,500,false);return;}
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Grave=FindGrave();
  BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(m_RemainsToken);
  m_Suite.Record(!m_Bag && m_Grave && record && record.Disposition==BCMM_RemainsDisposition.BURIED,"real owning-client production burial request commits grave and exact BURIED record");
  m_Suite.Record(mode.BCMM_BurialNetDigCount()==2,"owner and observer both see actual native digging item-use");
  string searching;
  if(System.GetCLIParam("bcmmBurialSearch",searching))m_Suite.Record(mode.BCMM_BurialSearchWasBlocked(),"authority excludes actual native open search before successful full burial");
  m_Suite.Record(GetGame().GetWorld().GetWorldTime()-m_BurialStarted>=30000,"dedicated owner burial retains full native thirty-second duration");
  foreach(IEntity original:m_OriginalItems)m_Suite.Record(!original || original.IsDeleted(),"network burial deletes exact original equipment still inside bag");
  if(!m_Grave){Finish();return;}
  mode.BCMM_BurialNetPublish(2,m_Actor,null,m_Grave,m_RemainsToken,record.DeceasedName,m_NetOwner.GetPlayerId());
  m_NetWaits=0;
  GetGame().GetCallqueue().CallLater(AwaitMemorialPeers,500,false);
 }
 protected void AwaitMemorialPeers()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode.BCMM_BurialNetMemorialCount()<3 && m_NetWaits++<120){GetGame().GetCallqueue().CallLater(AwaitMemorialPeers,500,false);return;}
  m_Suite.Record(mode.BCMM_BurialNetMemorialCount()==3,"original owner, observer and post-burial late joiner confirm exact memorial and notice privacy");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_Grave);
  m_Suite.Record(garbage && !garbage.Insert(m_Grave,1),"replicated completed memorial retains native lifetime protection");
  Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMBurialReadySent;
 protected bool m_BCMMBurialRequested;
 protected bool m_BCMMBurialDigSeen;
 protected bool m_BCMMBurialMemorialSeen;
 protected float m_BCMMBurialDigStart;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  string enabled;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmBurialObserver",enabled))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player)return;
  RplComponent controllerRpl=RplComponent.Cast(player.FindComponent(RplComponent));
  if(!controllerRpl || !controllerRpl.IsOwner())return;
  if(!m_BCMMBurialReadySent){m_BCMMBurialReadySent=true;player.BCMM_BurialNetReady();}
  IEntity actor=BCMM_BodyBagComponent.Resolve(mode.m_BCMMBurialNetActor);
  if(!actor || mode.m_BCMMBurialNetPhase<=0)return;
  bool owner=player.GetPlayerId()==mode.m_BCMMBurialNetOwner;
  CharacterControllerComponent character=ChimeraCharacter.Cast(actor).GetCharacterController();
  if(mode.m_BCMMBurialNetPhase==1)
  {
   RplComponent actorRpl=RplComponent.Cast(actor.FindComponent(RplComponent));
   if(owner && actorRpl.IsOwner() && player.GetControlledEntity()==actor && !BCMM_BurialSite.HeldTool(actor))
   {
    IEntity tool=BCMM_BodyBagComponent.Resolve(mode.m_BCMMBurialNetTool);
    if(tool)SCR_GadgetManagerComponent.GetGadgetManager(actor).SetGadgetMode(tool,EGadgetMode.IN_HAND);
   }
   if(owner && !m_BCMMBurialRequested && actorRpl.IsOwner() && player.GetControlledEntity()==actor && BCMM_BurialSite.HeldTool(actor))
   {
    IEntity bag=BCMM_BodyBagComponent.Resolve(mode.m_BCMMBurialNetBag);
    if(bag)
    {
     string searching;
     if(System.GetCLIParam("bcmmBurialSearch",searching) && !player.BCMM_TestBurialSearchComplete(bag))return;
     m_BCMMBurialRequested=true;
     player.BCMM_RequestBurial(bag);
     Print("BCMM_BURIAL_NET_OWNER_REQUEST production RPC ownedActor=1");
    }
   }
   bool valid=character.IsUsingItem();
   if(owner)valid=valid && player.BCMM_TestDiggingToken()==mode.m_BCMMBurialNetToken;
   else valid=valid && player.BCMM_TestDiggingToken()==0;
   if(valid && !m_BCMMBurialDigSeen)
   {
    if(m_BCMMBurialDigStart==0)m_BCMMBurialDigStart=world.GetWorldTime();
    if(world.GetWorldTime()-m_BCMMBurialDigStart<2000)return;
    m_BCMMBurialDigSeen=true;
    player.BCMM_BurialNetAck(1);
    PrintFormat("BCMM_BURIAL_NET_DIG_PASS owner=%1 nativeUsingItem=1 privateToken=%2",owner,player.BCMM_TestDiggingToken());
   }
  }
  if(mode.m_BCMMBurialNetPhase==2 && !m_BCMMBurialMemorialSeen)
  {
   IEntity grave=BCMM_BodyBagComponent.Resolve(mode.m_BCMMBurialNetGrave);
   BCMM_GraveMarkerComponent marker;
   if(grave)marker=BCMM_GraveMarkerComponent.Cast(grave.FindComponent(BCMM_GraveMarkerComponent));
   if(!marker || marker.RemainsToken()!=mode.m_BCMMBurialNetToken || marker.DeceasedName()!=mode.m_BCMMBurialNetName || BCMM_BodyBagComponent.Resolve(mode.m_BCMMBurialNetBag))return;
   if(owner && (player.BCMM_LastRemainsNoticeToken()!=mode.m_BCMMBurialNetToken || player.BCMM_LastRemainsNoticeKind()!=BCMM_RemainsDisposition.BURIED))return;
   if(!owner && player.BCMM_LastRemainsNoticeToken()!=0){Print("BCMM_BURIAL_NET_PRIVATE_FAIL",LogLevel.ERROR);return;}
   if(player.BCMM_TestDiggingToken()!=0)return;
   IEntity inspector=player.GetControlledEntity();
   if(!inspector)return;
   if(vector.Distance(inspector.GetOrigin(),grave.GetOrigin())>2)
   {
    vector pose[4];inspector.GetWorldTransform(pose);pose[3]=grave.GetOrigin()+"1 0.3 0";
    ChimeraCharacter.Cast(inspector).Teleport(pose);
    return;
   }
   if(WidgetManager.Translate("#BCMM-GraveMemorial")!="%1 died in service of his country.")return;
   ActionsManagerComponent actions=ActionsManagerComponent.Cast(grave.FindComponent(ActionsManagerComponent));
   array<BaseUserAction> registered={};if(actions)actions.GetActionsList(registered);
   BCMM_InspectGraveAction inspect;
   foreach(BaseUserAction candidate:registered)
   {
    inspect=BCMM_InspectGraveAction.Cast(candidate);
    if(inspect)break;
   }
   if(!inspect || !inspect.CanBeShownScript(inspector))return;
   inspect.PerformAction(grave,inspector);
   if(player.BCMM_LastInspectedGraveToken()!=mode.m_BCMMBurialNetToken)return;
   vector transform[4];grave.GetWorldTransform(transform);
   if(Math.AbsFloat(transform[0].Length()-0.59)>0.005)return;
   m_BCMMBurialMemorialSeen=true;
   player.BCMM_BurialNetAck(2);
   PrintFormat("BCMM_BURIAL_NET_MEMORIAL_PASS owner=%1 token=%2 name=%3 scale=%4 privateNotice=%5",owner,marker.RemainsToken(),marker.DeceasedName(),transform[0].Length(),player.BCMM_LastRemainsNoticeToken());
  }
 }
}
