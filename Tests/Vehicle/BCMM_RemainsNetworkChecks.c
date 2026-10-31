// Actual owning-client production RPC and owner-only confirmation privacy.
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_NetworkRemainsKill() { m_BCMMDeathPermitted=true;Kill(Instigator.CreateInstigator(GetOwner())); }
}
modded class SCR_BaseGameMode
{
 [RplProp()] int m_BCMMNoticeExpected;
 [RplProp()] bool m_BCMMHomeCommitted;
 protected bool m_BCMMNoticeOwnerAck;
 void BCMM_TestSetExpectedNotice(int token) { m_BCMMNoticeExpected=token;Replication.BumpMe(); }
 void BCMM_TestSetHomeCommitted() { m_BCMMHomeCommitted=true;Replication.BumpMe(); }
 void BCMM_TestNoticeAck(int playerId,int token)
 {
  if(playerId==m_BCMMVehiclePlayer && token==m_BCMMNoticeExpected) m_BCMMNoticeOwnerAck=true;
 }
 bool BCMM_TestHasNoticeAck() { return m_BCMMNoticeOwnerAck; }
}
modded class SCR_PlayerController
{
 void BCMM_TestPerformHome(RplId bag) { Rpc(BCMM_TestHomeOwner,bag); }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_TestHomeOwner(RplId bag)
 {
  IEntity target=BCMM_BodyBagComponent.Resolve(bag);
  PrintFormat("BCMM_REMAINS_CLIENT REQUEST target=%1",target);
  if(target) BCMM_RequestSendHome(target);
 }
 void BCMM_TestAckNotice(int token) { Rpc(BCMM_TestNoticeServer,token); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_TestNoticeServer(int token)
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestNoticeAck(GetPlayerId(),token);
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMHomeOwnerObserved;
 protected bool m_BCMMHomePrivateObserved;
 protected float m_BCMMHomeObservedAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  if(Replication.IsServer() || world.IsEditMode()) return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player || mode.m_BCMMNoticeExpected<=0) return;
  if(player.GetPlayerId()==mode.m_BCMMVehiclePlayer && !m_BCMMHomeOwnerObserved && player.BCMM_LastRemainsNoticeToken()==mode.m_BCMMNoticeExpected && player.BCMM_LastRemainsNoticeKind()==BCMM_RemainsDisposition.SENT_HOME)
  {
   IEntity actor=BCMM_BodyBagComponent.Resolve(mode.m_BCMMVehicleActor);
   RplComponent actorRpl;if(actor)actorRpl=RplComponent.Cast(actor.FindComponent(RplComponent));
   if(actorRpl && actorRpl.IsOwner() && player.GetControlledEntity()==actor)
   {
    m_BCMMHomeOwnerObserved=true;
    PrintFormat("BCMM_REMAINS_CLIENT OWNER_PASS token=%1 ownedActor=1",mode.m_BCMMNoticeExpected);
    player.BCMM_TestAckNotice(mode.m_BCMMNoticeExpected);
   }
  }
  if(mode.m_BCMMHomeCommitted && mode.m_BCMMVehiclePhase==3 && player.GetPlayerId()!=mode.m_BCMMVehiclePlayer && !m_BCMMHomePrivateObserved)
  {
   if(m_BCMMHomeObservedAt==0)m_BCMMHomeObservedAt=world.GetWorldTime();
   if(world.GetWorldTime()-m_BCMMHomeObservedAt<2000)return;
   m_BCMMHomePrivateObserved=true;
   if(player.BCMM_LastRemainsNoticeToken()==0)Print("BCMM_REMAINS_CLIENT PRIVATE_PASS no owner confirmation delivered to observer");
   else Print("BCMM_REMAINS_CLIENT PRIVATE_FAIL owner confirmation leaked",LogLevel.ERROR);
  }
 }
}
modded class BCMM_VehicleLifecycleChecks
{
 protected IEntity m_NoticeCorpse;
 protected IEntity m_NoticeAmbulance;
 protected int m_NoticeWaits;
 override protected void Loaded()
 {
  super.Loaded();
  m_NoticeCorpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"0 0 15");
  Check(m_NoticeCorpse!=null,"network remains fixture spawns actual native death source");
  if(!m_NoticeCorpse)return;
  SCR_CharacterDamageManagerComponent.Cast(m_NoticeCorpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_NetworkRemainsKill();
  GetGame().GetCallqueue().CallLater(BindNetworkDeath,1500,false);
 }
 protected void BindNetworkDeath()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  int token=mode.BCMM_RegisterRemains(m_NoticeCorpse);
  Check(token>0 && mode.BCMM_TransferRemains(m_NoticeCorpse,m_Bag),"network bag inherits exact original native corpse token");
  mode.BCMM_TestSetExpectedNotice(token);
  SCR_EntityHelper.DeleteEntityAndChildren(m_NoticeCorpse);
 }
 protected SCR_HealSupportStationComponent FindNoticeStation(IEntity entity)
 {
  SCR_HealSupportStationComponent station=SCR_HealSupportStationComponent.Cast(entity.FindComponent(SCR_HealSupportStationComponent));
  if(station && station.UsesRange())return station;
  IEntity child=entity.GetChildren();
  while(child)
  {
   station=FindNoticeStation(child);
   if(station)
   {
    return station;
   }
   child=child.GetSibling();
  }
  return null;
 }
 override protected void DeleteBag()
 {
  vector ambulancePosition="125 2 0";string searching;
  if(System.GetCLIParam("bcmmSearchSessions",searching))ambulancePosition="109 2 2";
  m_NoticeAmbulance=Spawn("{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",ambulancePosition);
  Check(m_NoticeAmbulance!=null,"real native ambulance spawns for owning-client return-home request");
  GetGame().GetCallqueue().CallLater(PositionNoticePair,2500,false);
 }
 protected void PositionNoticePair()
 {
  Print("BCMM_REMAINS_NETWORK locating native ambulance station");
  SCR_HealSupportStationComponent station=FindNoticeStation(m_NoticeAmbulance);
  Check(station && station.IsEnabled(),"network fixture resolves actual enabled native healing station");
  if(!station){Finish();return;}
  vector position=station.GetPosition()+Vector(station.GetRange()*0.6,0,0);position[1]=0.35;
  string searching;
  if(System.GetCLIParam("bcmmSearchSessions",searching))
  {
   position=m_Bag.GetOrigin();
   PrintFormat("BCMM_SEARCH_SETUP bag=%1 station=%2 range=%3 inside=%4",position,station.GetPosition(),station.GetRange(),station.IsInRange(m_Bag,position));
  }
  else {m_Bag.SetOrigin(position);m_Bag.GetPhysics().SetVelocity(vector.Zero);m_Bag.Update();}
  m_Controller.BCMM_VehicleMove(position+"1 0 0");
  GetGame().GetCallqueue().CallLater(RequestHomeOnOwner,1500,false);
 }
 protected void RequestHomeOnOwner()
 {
  Check(BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag),"actual owned performer reaches valid native healing area");
  if(m_Failures){Finish();return;}
  m_Controller.BCMM_TestPerformHome(BCMM_BodyBagComponent.Id(m_Bag));
  GetGame().GetCallqueue().CallLater(AwaitHomeNotice,500,false);
 }
 protected void AwaitHomeNotice()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if((m_Bag || !mode.BCMM_TestHasNoticeAck()) && m_NoticeWaits++<40){GetGame().GetCallqueue().CallLater(AwaitHomeNotice,500,false);return;}
  Check(!m_Bag && mode.BCMM_TestHasNoticeAck(),"real owner production RPC disposes bag and original owning actor receives exact confirmation token");
  BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(mode.m_BCMMNoticeExpected);
  Check(record && record.Disposition==BCMM_RemainsDisposition.SENT_HOME,"network return-home retains SENT_HOME after physical deletion");
  mode.BCMM_TestSetHomeCommitted();
  m_Controller.BCMM_VehicleMove(m_Vehicle.GetOrigin()+"4 0.2 0");
  GetGame().GetCallqueue().CallLater(PublishHomeComplete,1500,false);
 }
 protected void PublishHomeComplete()
 {
  Check(m_Gear && InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent)).Contains(m_Gear),"owner-requested return-home preserves original equipment already left in first vehicle");
  Publish(3);
  GetGame().GetCallqueue().CallLater(Finish,7000,false);
 }
}
