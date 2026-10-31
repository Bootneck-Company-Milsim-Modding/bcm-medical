// Diagnostic observer state only. Never publish this addon.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMCargoTestPhase;
 [RplProp()] protected RplId m_BCMMCargoTestBag = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMCargoTestVehicle = RplId.Invalid();
 void BCMM_PublishCargoTest(int phase, IEntity bag, IEntity vehicle)
 {
  if (!Replication.IsServer()) return;
  m_BCMMCargoTestBag = BCMM_BodyBagComponent.Id(bag);
  m_BCMMCargoTestVehicle = BCMM_BodyBagComponent.Id(vehicle);
  m_BCMMCargoTestPhase = phase;
  Replication.BumpMe();
  PrintFormat("BCMM_NET_PHASE %1", phase);
 }
 override void OnPlayerConnected(int playerId)
 {
  super.OnPlayerConnected(playerId);
  if (Replication.IsServer()) GetGame().GetCallqueue().CallLater(BCMM_AssignObserverBody, 1500, false, playerId);
 }
 protected void BCMM_AssignObserverBody(int playerId)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  if (!player || player.GetControlledEntity()) return;
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = "105 2 3";
  string savedWorldNetwork;
  if(System.GetCLIParam("bcmmWorldPersistenceNetwork",savedWorldNetwork))spawn.Transform[3]=Vector(108,1.2,0.5*playerId);
  string clinicalNetwork;
  if (System.GetCLIParam("bcmmSplintNetwork", clinicalNetwork) || System.GetCLIParam("bcmmDragNetwork", clinicalNetwork)) spawn.Transform[3] = Vector(100.6, 1.2, 2 + 0.8 * playerId);
  if (System.GetCLIParam("bcmmSplintNetwork", clinicalNetwork) && playerId == 1) spawn.Transform[3] = "100 1.2 -0.8";
  string stretcherNetwork;
  if (System.GetCLIParam("bcmmStretcherNetwork",stretcherNetwork))
  {
   if(playerId==1)spawn.Transform[3]="99.1 2.25 -0.7";
   else spawn.Transform[3]="105 2.25 4";
  }
  string livingTransport;if(System.GetCLIParam("bcmmLivingTransportNetwork",livingTransport))spawn.Transform[3]=Vector(100+(playerId-1)*1.0,1.2,0);
  string medicalNetwork;
  string burialNetwork;
  if(System.GetCLIParam("bcmmBurialNetwork",burialNetwork))spawn.Transform[3]=BCMM_BurialNetSpawn(playerId);
  if (System.GetCLIParam("bcmmMedicalNetwork", medicalNetwork)) spawn.Transform[3] = Vector(100.8, 1.2, 0.6 * playerId);
  if(BCMM_CatalogueChecks.Enabled())spawn.Transform[3]=BCMM_CatalogueChecks.Site()+Vector(0,0.2,2*playerId);
  IEntity character = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  if (character)
  {
   // A controlled pointer alone is not native character possession. Transfer
   // replication ownership as the native possession path does, so client-side
   // item commands are exercised by the actual owning character.
   string nativeItemFixture;
   if (System.GetCLIParam("bcmmLivingTransportNetwork",nativeItemFixture) || System.GetCLIParam("bcmmWorldPersistenceNetwork",nativeItemFixture) || System.GetCLIParam("bcmmBurialNetwork",nativeItemFixture) || System.GetCLIParam("bcmmSplintNetwork", nativeItemFixture) || System.GetCLIParam("bcmmAnimationNetwork", nativeItemFixture) || System.GetCLIParam("bcmmMedicalNetwork", nativeItemFixture) || System.GetCLIParam("bcmmStretcherNetwork",nativeItemFixture) || System.GetCLIParam("bcmmOxygenNetwork",nativeItemFixture) || System.GetCLIParam("bcmmArsenalNetwork",nativeItemFixture) || System.GetCLIParam("bcmmFluidNetwork",nativeItemFixture) || System.GetCLIParam("bcmmAppliedEquipmentNetwork",nativeItemFixture) || System.GetCLIParam("bcmmOxygenRemovalNetwork",nativeItemFixture) || System.GetCLIParam("bcmmFluidLineNetwork",nativeItemFixture) || System.GetCLIParam("bcmmFluidBurdenNetwork",nativeItemFixture) || System.GetCLIParam("bcmmStockCommitNetwork",nativeItemFixture))
   {
    RplComponent replication = RplComponent.Cast(character.FindComponent(RplComponent));
    if (replication) replication.GiveExt(player.GetRplIdentity(), false);
    AIControlComponent control = AIControlComponent.Cast(character.FindComponent(AIControlComponent));
    if (control) control.DeactivateAI();
   }
   PrintFormat("BCMM_NET_BODY player=%1 assigned=%2", playerId, player.SetControlledEntity(character));
  }
 }
 int BCMM_CargoTestPhase() { return m_BCMMCargoTestPhase; }
 RplId BCMM_CargoTestBag() { return m_BCMMCargoTestBag; }
 RplId BCMM_CargoTestVehicle() { return m_BCMMCargoTestVehicle; }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMObservedCargoPhase;
 protected int m_BCMMLoggedCargoPhase = -1;
 protected int m_BCMMLoggedBagPhase = -1;
 protected float m_BCMMNextObservation;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string enabled;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmCargoObserver", enabled) || world.GetWorldTime() < m_BCMMNextObservation) return;
  m_BCMMNextObservation = world.GetWorldTime() + 250;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  if (!mode || mode.BCMM_CargoTestPhase() <= m_BCMMObservedCargoPhase) return;
  IEntity bagEntity = BCMM_BodyBagComponent.Resolve(mode.BCMM_CargoTestBag());
  IEntity vehicle = BCMM_BodyBagComponent.Resolve(mode.BCMM_CargoTestVehicle());
  if (m_BCMMLoggedCargoPhase != mode.BCMM_CargoTestPhase()) { m_BCMMLoggedCargoPhase = mode.BCMM_CargoTestPhase(); PrintFormat("BCMM_NET_WAIT phase=%1 bag=%2 vehicle=%3", m_BCMMLoggedCargoPhase, bagEntity, vehicle); }
  if (!vehicle) return;
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(vehicle.FindComponent(BCMM_VehicleCargoComponent));
  if (!cargo) return;
  bool stored = mode.BCMM_CargoTestPhase() % 2 == 1;
  if (stored)
  {
   // Native inventory legitimately streams out hidden bags. A lightweight
   // vehicle manifest must remain available without forcing bag entities in.
   if (cargo.Count() != 1 || cargo.BagAt(0) != mode.BCMM_CargoTestBag() || cargo.NameAt(0) != "BCM cargo test casualty") return;
  }
  else
  {
   if (!bagEntity) return;
   BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(bagEntity.FindComponent(BCMM_BodyBagComponent));
   SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
   if (m_BCMMLoggedBagPhase != mode.BCMM_CargoTestPhase() && bag) { m_BCMMLoggedBagPhase = mode.BCMM_CargoTestPhase(); PrintFormat("BCMM_NET_BAG state=%1 name=%2 cargo=%3 contained=%4", bag.State(), bag.DeceasedName(), cargo.Count(), inventory.Contains(bagEntity)); }
   if (!bag || !inventory || bag.DeceasedName() != "BCM cargo test casualty" || bag.State() != BCMM_BagCargoState.WORLD || bag.VehicleId() != RplId.Invalid() || cargo.Count() != 0 || inventory.Contains(bagEntity)) return;
  }  m_BCMMObservedCargoPhase = mode.BCMM_CargoTestPhase();
  PrintFormat("BCMM_NET_OBSERVED phase=%1 bag=%2 vehicle=%3", m_BCMMObservedCargoPhase, mode.BCMM_CargoTestBag(), mode.BCMM_CargoTestVehicle());
 }
}


