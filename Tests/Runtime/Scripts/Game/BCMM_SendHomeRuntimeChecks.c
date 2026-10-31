// Actual native healing-area geometry and real original casualty equipment.
// Account identity is a test-only trusted binding, not backend authentication.
modded class SCR_BaseGameMode
{
 bool BCMM_TestHomeEnabled(bool enabled) { return m_BCMMSettings.Set("sendHomeEnabled",enabled); }
}
class BCMM_SendHomeRuntimeChecks:BCMM_BaggingRuntimeChecks
{
 protected SCR_BaseGameMode m_HomeMode;
 protected SCR_HealSupportStationComponent m_Station;
 protected IEntity m_WideFloor;
 protected int m_Balance;
 override protected void Prepare()
 {
  m_HomeMode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_HomeMode.BCMM_TestLifeIdentity(90003,"BCMM_TEST_HOME_ACCOUNT");
  m_Suite.Record(m_HomeMode.BCMM_ReserveLife(90003,m_Corpse) && m_HomeMode.BCMM_CommitLife(90003,m_Corpse),"actual living casualty has registered diagnostic deployment before native death");
  super.Prepare();
 }
 override protected void Bag()
 {
  BCMM_LifeAccount account=m_HomeMode.BCMM_AccountFor(90003);
  m_Suite.Record(account && !account.Ledger.IsAlive() && !m_HomeMode.BCMM_ConfirmLifeDeath(90003,m_Corpse),"Native death confirms the exact paid life once before independent physical bagging");
  super.Bag();
 }
 protected SCR_HealSupportStationComponent FindStation(IEntity entity)
 {
  SCR_HealSupportStationComponent station=SCR_HealSupportStationComponent.Cast(entity.FindComponent(SCR_HealSupportStationComponent));
  if(station && station.UsesRange()) return station;
  IEntity child=entity.GetChildren();
  while(child)
  {
   station=FindStation(child);if(station)return station;
   child=child.GetSibling();
  }
  return null;
 }
 override protected void Verify()
 {
  foreach(BCMM_BodyBagComponent bag:BCMM_BodyBagComponent.Instances)
   if(bag && bag.DeceasedName()=="BCM bagging test casualty")m_Bag=bag.GetOwner();
  m_Suite.Record(m_Bag && !m_Corpse && !m_Empty,"return-home fixture completes actual original-equipment bagging chain");
  if(!m_Bag){Finish();return;}
  m_Suite.Record(!BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag) && !BCMM_SendHomeOperation.Begin(m_Actor,m_Bag),"absence of native healing area rejects return-home without disposal");
  BCMM_LifeAccount account=m_HomeMode.BCMM_AccountFor(90003);
  m_Balance=account.Ledger.RemainingDeployments(m_HomeMode.BCMM_ReadSetting("totalLives"));
  m_Suite.Record(m_HomeMode.BCMM_LifeCanDeploy(90003),"A recoverable filled bag never blocks remaining Limited Lives allowance");
  EntitySpawnParams floor=new EntitySpawnParams();floor.TransformMode=ETransformMode.WORLD;floor.Transform[0]="100 0 0";floor.Transform[1]="0 100 0";floor.Transform[2]="0 0 100";floor.Transform[3]="100 0 0";
  m_WideFloor=GetGame().SpawnEntityPrefab(Resource.Load("{AE73676ABA995907}Prefabs/Structures/BuildingParts/Entry/Entry_Block_Platform_01_3x12.et"),GetGame().GetWorld(),floor);
  string facility;System.GetCLIParam("bcmmHomeFacility",facility);
  ResourceName facilityPrefab="{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et";
  if(facility=="USTent")facilityPrefab="{F8427C3EFC52644E}Prefabs/Structures/Military/Camps/TentUS_01/TentUSMedical_01.et";
  if(facility=="USSRTent")facilityPrefab="{CBE5E0D2DC08E178}Prefabs/Structures/Military/Camps/TentUSSR_01/TentUSSRMedical_01.et";
  if(facility=="FIATent")facilityPrefab="{9110077F59864EF1}Prefabs/Structures/Military/Camps/TentFIA_01/TentFIA_01_medical.et";
  PrintFormat("BCMM_HOME_FACILITY kind=%1 prefab=%2",facility,facilityPrefab);
  vector facilityPosition="125 2 0";if(facility=="USTent" || facility=="USSRTent" || facility=="FIATent")facilityPosition="125 0 0";
  m_Vehicle=Spawn(facilityPrefab,facilityPosition);
  m_Suite.Record(m_WideFloor && m_Vehicle,"real native healing facility and complete supporting collision footprint spawn");
  GetGame().GetCallqueue().CallLater(PlaceOutside,2500);
 }
 protected void PositionPair(vector position)
 {
  position[1]=0.35;m_Bag.SetOrigin(position);m_Bag.GetPhysics().SetVelocity(vector.Zero);m_Bag.Update();
  m_Actor.SetOrigin(position+"1 0 0");
 }
 protected void PlaceOutside()
 {
  m_Station=FindStation(m_Vehicle);
  m_Suite.Record(m_Station && m_Station.IsEnabled() && m_Station.UsesRange() && m_Station.GetRange()>0,"native facility supplies actual enabled range-based healing station");
  if(!m_Station){Finish();return;}
  PrintFormat("BCMM_HOME_GEOMETRY range=%1 position=%2",m_Station.GetRange(),m_Station.GetPosition());
  PositionPair(m_Station.GetPosition()+Vector(m_Station.GetRange()+10,0,0));
  GetGame().GetCallqueue().CallLater(CheckOutside,1000);
 }
 protected void CheckOutside()
 {
  m_Suite.Record(!m_Station.IsInRange(m_Bag,m_Bag.GetOrigin()) && !SCR_HealSupportStationComponent.BCMM_FindHealingArea(m_Bag),"native station geometry and Medical registry agree outside actual configured boundary");
  m_Suite.Record(!BCMM_SendHomeOperation.Begin(m_Actor,m_Bag),"actual outside boundary cannot dispose original recovery bag");
  PositionPair(m_Station.GetPosition()+Vector(m_Station.GetRange()*0.6,0,0));
  GetGame().GetCallqueue().CallLater(CheckInside,1500);
 }
 protected void CheckInside()
 {
  m_Suite.Record(m_Station.IsInRange(m_Bag,m_Bag.GetOrigin()) && SCR_HealSupportStationComponent.BCMM_FindHealingArea(m_Bag),"native station geometry and registry agree at actual inside point");
  m_Suite.Record(BCMM_SendHomeOperation.CanBegin(m_Actor,m_Bag),"nearby conscious performer can use actual native medical area");
  m_Station.SetEnabled(false);
  m_Suite.Record(!BCMM_SendHomeOperation.Begin(m_Actor,m_Bag) && m_Bag,"disabled native station cannot dispose original remains");
  m_Station.SetEnabled(true);
  m_Suite.Record(m_HomeMode.BCMM_TestHomeEnabled(false) && !BCMM_SendHomeOperation.Begin(m_Actor,m_Bag),"live return-home disable rejects otherwise valid native facility");
  m_HomeMode.BCMM_TestHomeEnabled(true);
  m_Actor.SetOrigin(m_Bag.GetOrigin()+"20 0 0");
  m_Suite.Record(!BCMM_SendHomeOperation.Begin(m_Actor,m_Bag),"distant performer cannot dispose bag inside valid station");
  m_Actor.SetOrigin(m_Bag.GetOrigin()+"1 0 0");
  GetGame().GetCallqueue().CallLater(CommitHome,1000);
 }
 protected void CommitHome()
 {
  m_Suite.Record(BCMM_SendHomeOperation.Begin(m_Actor,m_Bag),"authority commits exact recovery bag through actual native healing area");
  GetGame().GetCallqueue().CallLater(CheckCommitted,250);
 }
 protected void CheckCommitted()
 {
  BCMM_RemainsRecord record=m_HomeMode.BCMM_RemainsByToken(m_RemainsToken);
  BCMM_LifeAccount account=m_HomeMode.BCMM_AccountFor(90003);
  m_Suite.Record(!m_Bag && record && record.Disposition==BCMM_RemainsDisposition.SENT_HOME,"physical deletion preserves original token's SENT_HOME classification");
  foreach(IEntity original:m_OriginalItems)m_Suite.Record(!original || original.IsDeleted(),"return-home disposes exact original equipment still inside recovery bag");
  m_Suite.Record(m_HomeMode.BCMM_LifeCanDeploy(90003),"Return-home leaves Limited Lives eligibility unchanged");
  m_Suite.Record(account.Ledger.RemainingDeployments(m_HomeMode.BCMM_ReadSetting("totalLives"))==m_Balance,"return-home never refunds or manufactures deployment allowance");
  Finish();
 }
 override protected void Finish()
 {
  if(m_Bag)SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);if(m_Corpse)SCR_EntityHelper.DeleteEntityAndChildren(m_Corpse);if(m_Empty)SCR_EntityHelper.DeleteEntityAndChildren(m_Empty);
  if(m_Vehicle)
  {
   RplComponent rpl=RplComponent.Cast(m_Vehicle.FindComponent(RplComponent));
   PrintFormat("BCMM_HOME_CLEANUP replicated=%1",rpl!=null);
   if(rpl)SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
   else delete m_Vehicle;
  }
  if(m_Actor)SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  if(m_Floor)delete m_Floor;if(m_FloorNorth)delete m_FloorNorth;if(m_FloorSouth)delete m_FloorSouth;if(m_WideFloor)delete m_WideFloor;
  m_Suite.Complete();
 }
}
