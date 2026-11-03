// Diagnostic addon only. Exercises Medical's own cargo/equipment contract.
// Identical local static geometry on every peer, as with preloaded world geometry.
class BCMM_VehicleTestPlatform
{
 static IEntity Floor;
 static IEntity Ensure()
 {
  if (Floor) return Floor;
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[0] = "20 0 0"; spawn.Transform[1] = "0 20 0"; spawn.Transform[2] = "0 0 20"; spawn.Transform[3] = "100 0 0";
  Floor = GetGame().SpawnEntityPrefab(Resource.Load("{AE73676ABA995907}Prefabs/Structures/BuildingParts/Entry/Entry_Block_Platform_01_3x12.et"), GetGame().GetWorld(), spawn);
  if (Floor) Floor.Update();
  return Floor;
 }
}
modded class SCR_BaseGameMode
{
 protected ref array<int> m_BCMMVehicleReadyPlayers = {};
 void BCMM_VehicleClientReady(int playerId) { if (!m_BCMMVehicleReadyPlayers.Contains(playerId)) m_BCMMVehicleReadyPlayers.Insert(playerId); }
 bool BCMM_VehicleIsClientReady(int playerId) { return m_BCMMVehicleReadyPlayers.Contains(playerId); }
 [RplProp()] int m_BCMMVehiclePhase;
 [RplProp()] RplId m_BCMMVehicleBag;
 [RplProp()] RplId m_BCMMVehicle;
 [RplProp()] RplId m_BCMMVehicleGear;
 [RplProp()] RplId m_BCMMVehicleActor;
 [RplProp()] int m_BCMMVehiclePlayer;
 void BCMM_VehiclePublish(int phase, IEntity bag, IEntity vehicle, IEntity gear, IEntity actor)
 {
  m_BCMMVehiclePhase = phase;
  m_BCMMVehicleBag = BCMM_BodyBagComponent.Id(bag);
  m_BCMMVehicle = BCMM_BodyBagComponent.Id(vehicle);
  m_BCMMVehicleGear = BCMM_BodyBagComponent.Id(gear);
  m_BCMMVehicleActor = BCMM_BodyBagComponent.Id(actor);
  m_BCMMVehiclePlayer = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  Replication.BumpMe();
  PrintFormat("BCMM_VEHICLE_PHASE %1", phase);
 }
}
modded class SCR_PlayerController
{
 void BCMM_VehicleReady() { Rpc(RpcAsk_BCMM_VehicleReady); }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void RpcAsk_BCMM_VehicleReady()
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_VehicleClientReady(GetPlayerId());
  PrintFormat("BCMM_VEHICLE_CLIENT_READY player=%1", GetPlayerId());
 }
 void BCMM_VehicleMove(vector position) { Rpc(RpcDo_BCMM_VehicleMove, position); }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void RpcDo_BCMM_VehicleMove(vector position)
 {
  SCR_ChimeraCharacter actor = SCR_ChimeraCharacter.Cast(GetControlledEntity());
  if (!actor) return;
  vector transform[4]; actor.GetWorldTransform(transform); transform[3] = position;
  actor.Teleport(transform);
 }
}
modded class ArmaReforgerScripted
{
 protected ref BCMM_VehicleLifecycleChecks m_BCMMVehicleChecks;
 protected int m_BCMMVehicleObserved;
 protected float m_BCMMVehicleDiagnostic;
 protected int m_BCMMVehicleOpened;
 protected bool m_BCMMVehicleReadySent;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (world.IsEditMode() || !GetGame().GetGameMode()) return;
  BCMM_VehicleTestPlatform.Ensure();
  if (Replication.IsServer())
  {
   if (!m_BCMMVehicleChecks)
   {
    m_BCMMVehicleChecks = new BCMM_VehicleLifecycleChecks();
    GetGame().GetCallqueue().CallLater(m_BCMMVehicleChecks.Start, 2000, false);
   }
   return;
  }
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  SCR_PlayerController localController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (mode && localController && !m_BCMMVehicleReadySent)
  {
   RplComponent controllerRpl = RplComponent.Cast(localController.FindComponent(RplComponent));
   if (controllerRpl && controllerRpl.IsOwner()) { localController.BCMM_VehicleReady(); m_BCMMVehicleReadySent = true; }
  }
  if (!mode || mode.m_BCMMVehiclePhase <= m_BCMMVehicleObserved) return;
  IEntity bag = BCMM_BodyBagComponent.Resolve(mode.m_BCMMVehicleBag);
  IEntity vehicle = BCMM_BodyBagComponent.Resolve(mode.m_BCMMVehicle);
  IEntity gear = BCMM_BodyBagComponent.Resolve(mode.m_BCMMVehicleGear);
  IEntity actor = BCMM_BodyBagComponent.Resolve(mode.m_BCMMVehicleActor);
  PlayerController controller = GetGame().GetPlayerController();
  if (world.GetWorldTime() > m_BCMMVehicleDiagnostic)
  {
   m_BCMMVehicleDiagnostic = world.GetWorldTime() + 5000;
   PrintFormat("BCMM_VEHICLE_CLIENT diagnostic phase=%1 bag=%2 vehicle=%3 gear=%4 actor=%5 controlled=%6", mode.m_BCMMVehiclePhase, bag, vehicle, gear, actor, controller && controller.GetControlledEntity());
  }
  if (!vehicle || !actor || !controller) return;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(vehicle.FindComponent(InventoryStorageManagerComponent));
  IEntity localActor = controller.GetControlledEntity();
  if (!localActor) return;
  SCR_InventoryStorageManagerComponent personal = SCR_InventoryStorageManagerComponent.Cast(localActor.FindComponent(SCR_InventoryStorageManagerComponent));
  if (personal && m_BCMMVehicleOpened != mode.m_BCMMVehiclePhase)
  {
   personal.SetStorageToOpen(vehicle); personal.OpenInventory();
   m_BCMMVehicleOpened = mode.m_BCMMVehiclePhase;
  }
  int bags = inventory.GetDepositItemCountByResource(localActor, "{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et");
  int equipment = inventory.GetDepositItemCountByResource(localActor, "{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et");
  bool valid = equipment == 1;
  BCMM_BodyBagComponent state;
  if (bag) state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if (mode.m_BCMMVehiclePhase == 1) valid = valid && bags == 1;
  if (mode.m_BCMMVehiclePhase == 2) valid = valid && bags == 0 && bag && state && state.State() == BCMM_BagCargoState.WORLD && state.VehicleId() == RplId.Invalid() && !bag.GetParent() && (bag.GetFlags() & EntityFlags.VISIBLE) != 0;
  if (mode.m_BCMMVehiclePhase == 3) valid = valid && bags == 0 && !bag;
  if (controller.GetPlayerId() == mode.m_BCMMVehiclePlayer)
  {
   RplComponent rpl = RplComponent.Cast(actor.FindComponent(RplComponent));
   valid = valid && rpl && rpl.IsOwner() && controller.GetControlledEntity() == actor;
   if (valid && m_BCMMVehicleObserved == 0) Print("BCMM_VEHICLE_CLIENT OWNER_CONFIRMED");
  }
  if (!valid) return;
  m_BCMMVehicleObserved = mode.m_BCMMVehiclePhase;
  if (personal) personal.CloseInventory();
  PrintFormat("BCMM_VEHICLE_CLIENT PASS phase=%1", m_BCMMVehicleObserved);
 }
}
class BCMM_VehicleLifecycleChecks
{
 protected IEntity m_Actor;
 protected IEntity m_Vehicle;
 protected IEntity m_Bag;
 protected IEntity m_Gear;
 protected SCR_PlayerController m_Controller;
 protected int m_Checks;
 protected int m_Failures;
 protected int m_Waits;
 protected vector m_FlightOrigin;
 protected float m_FlightStarted;
 protected float m_MaxLift;
 protected float m_MaxHorizontal;
 protected float m_NextFlightLog;
 protected bool m_Piloted;
 protected bool m_AuthorityOnly;
 protected int m_UnloadWaits;
 protected IEntity m_Floor;
 protected vector m_PeerSpawn;
 protected void Check(bool valid, string label)
 {
  m_Checks++; if (!valid) m_Failures++;
  PrintFormat("BCMM_VEHICLE_CHECK check=%1 pass=%2 %3", m_Checks, valid, label);
 }
 protected IEntity Spawn(ResourceName prefab, vector position)
 {
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawn);
 }
 void Start()
 {
  string pilot; m_Piloted = System.GetCLIParam("bcmmPilotedVehicle", pilot);
  m_AuthorityOnly = System.GetCLIParam("bcmmVehicleAuthority", pilot);
  string prefab;
  if (!System.GetCLIParam("bcmmVehiclePrefab", prefab)) { Check(false, "canonical vehicle prefab required"); Finish(); return; }
  m_Floor = BCMM_VehicleTestPlatform.Ensure();
  if (!m_Floor) { Check(false, "large physical native test platform spawns"); Finish(); return; }
  m_Floor.Update();
  vector minimum; vector maximum; m_Floor.GetWorldBounds(minimum, maximum);
  vector floorTransform[4]; m_Floor.GetWorldTransform(floorTransform);
  PrintFormat("BCMM_VEHICLE_PLATFORM minimum=%1 maximum=%2 scale=%3", minimum, maximum, floorTransform[0].Length());
  vector center = (minimum + maximum) * 0.5;
  float ground = maximum[1];
  Check(maximum[0] - minimum[0] > 50 && maximum[2] - minimum[2] > 50, "native local platform on every peer supports complete vehicle and flight footprint");
  m_Vehicle = Spawn(prefab, Vector(center[0] + 3, ground + 0.2, center[2]));
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", Vector(center[0], ground + 0.2, center[2]));
  m_Bag = Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et", Vector(center[0] + 1, ground + 0.5, center[2]));
  m_Gear = Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et", Vector(center[0] + 1, ground + 1, center[2]));
  m_PeerSpawn = Vector(center[0] + 7, ground + 0.8, center[2]);
  Check(m_Vehicle && m_Actor && m_Bag && m_Gear, "Medical and native vehicle fixture entities spawn");
  if (!m_Vehicle || !m_Actor || !m_Bag || !m_Gear) { Finish(); return; }
  BCMM_BagStorageComponent storage = BCMM_BagStorageComponent.Cast(m_Bag.FindComponent(BCMM_BagStorageComponent));
  storage.SetTransferring(true);
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  Check(inventory.TryInsertItemInStorage(m_Gear, storage), "exact real equipment item enters native Medical bag storage");
  BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).SetDeceasedName("Medical lifecycle casualty");
  GetGame().GetCallqueue().CallLater(Load, 2000, false);
 }
 protected void Load()
 {
  array<int> players = {}; GetGame().GetPlayerManager().GetPlayers(players);
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  bool clientReady = m_AuthorityOnly;
  if (!clientReady && players.Count() > 0) clientReady = mode.BCMM_VehicleIsClientReady(players[0]);
  if (!clientReady || !BCMM_CargoOperation.ActorCanUse(m_Actor, m_Vehicle, m_Bag))
  {
   if (m_Waits++ < 120) { GetGame().GetCallqueue().CallLater(Load, 250, false); return; }
   Check(false, "owned performer and stationary native cargo become available"); Finish(); return;
  }
  if (!m_AuthorityOnly)
  {
   m_Controller = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(players[0]));
   RplComponent actorRpl = RplComponent.Cast(m_Actor.FindComponent(RplComponent));
   actorRpl.GiveExt(m_Controller.GetRplIdentity(), false);
   AIControlComponent ai = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));
   if (ai) ai.DeactivateAI();
   m_Controller.SetInitialMainEntity(m_Actor);
   Check(m_Controller.GetControlledEntity() == m_Actor, "native initial-main-entity transfer binds actual cargo requester");
  }
  BCMM_BagStorageComponent.Cast(m_Bag.FindComponent(BCMM_BagStorageComponent)).SetTransferring(false);
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, true, state.Revision()), "Medical cargo operation starts on ordinary native vehicle");
  if (m_Failures) { Finish(); return; }
  GetGame().GetCallqueue().CallLater(Loaded, 2000, false);
 }
 protected void Loaded()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  InventoryStorageManagerComponent vehicle = InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent));
  InventoryStorageManagerComponent bag = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  Check(state.State() == BCMM_BagCargoState.STORED && vehicle.Contains(m_Bag) && state.VehicleId() == BCMM_BodyBagComponent.Id(m_Vehicle), "native stored bag agrees with exact Medical vehicle identity");
  Check(vehicle.Contains(m_Gear) && !bag.Contains(m_Gear), "Medical loading moves exact original equipment into vehicle storage");
  if (m_Failures) { Finish(); return; }
  Publish(1);
  GetGame().GetCallqueue().CallLater(EnsurePeerBody, 1000, true);
  if (m_Piloted)
  {
   m_FlightOrigin = m_Vehicle.GetOrigin(); m_FlightStarted = GetGame().GetWorld().GetWorldTime() * 0.001;
   GetGame().GetCallqueue().CallLater(BeginPilot, 10000, false);
  }
  else GetGame().GetCallqueue().CallLater(Unload, 20000, false);
 }
 protected void BeginPilot()
 {
  m_FlightStarted = GetGame().GetWorld().GetWorldTime() * 0.001;
  m_Controller.BCMM_TestPilotStart(BCMM_BodyBagComponent.Id(m_Vehicle));
  GetGame().GetCallqueue().CallLater(Flight, 2000, false);
 }
 protected void EnsurePeerBody()
 {
  array<int> players = {}; GetGame().GetPlayerManager().GetPlayers(players);
  foreach (int id : players)
  {
   SCR_PlayerController controller = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
   if (!controller || controller.GetControlledEntity() || !SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_VehicleIsClientReady(id)) continue;
   IEntity peer = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", m_PeerSpawn);
   RplComponent.Cast(peer.FindComponent(RplComponent)).GiveExt(controller.GetRplIdentity(), false);
   AIControlComponent ai = AIControlComponent.Cast(peer.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
   controller.SetInitialMainEntity(peer);
  }
 }
 protected void Flight()
 {
  HelicopterControllerComponent helicopter = HelicopterControllerComponent.Cast(m_Vehicle.FindComponent(HelicopterControllerComponent));
  VehicleHelicopterSimulation simulation = helicopter.GetSimulation();
  float elapsed = GetGame().GetWorld().GetWorldTime() * 0.001 - m_FlightStarted;
  vector displacement = m_Vehicle.GetOrigin() - m_FlightOrigin;
  m_MaxLift = Math.Max(m_MaxLift, displacement[1]); displacement[1] = 0;
  m_MaxHorizontal = Math.Max(m_MaxHorizontal, displacement.Length());
  if (elapsed >= m_NextFlightLog)
  {
   m_NextFlightLog = elapsed + 5;
   PrintFormat("BCMM_VEHICLE_FLIGHT elapsed=%1 lift=%2 horizontal=%3 contact=%4 rpm=%5", elapsed, m_MaxLift, m_MaxHorizontal, simulation.HasAnyGroundContact(), simulation.RotorGetRPM(0));
  }
  if (elapsed > 45 && m_MaxLift > 3 && simulation.HasAnyGroundContact() && simulation.GetVelocity().Length() < 1.5)
  {
   Check(helicopter.GetPilotCompartmentSlot().GetOccupant() == m_Actor, "actual owned native pilot remains seated through flight and landing");
   Check(m_MaxLift > 3 && m_MaxHorizontal > 2, "native collective and cyclic fly loaded Medical cargo under gravity");
   m_Controller.BCMM_TestPilotExit(m_Vehicle.GetOrigin() + Vector(4, 0, 0));
   GetGame().GetCallqueue().CallLater(Unload, 3000, false); return;
  }
  if (elapsed > 240) { Check(false, "native piloted takeoff and landing deadline after allowing rotor spool-up"); Finish(); return; }
  GetGame().GetCallqueue().CallLater(Flight, 2000, false);
 }
 protected void Unload()
 {
  if (m_Piloted)
  {
   float limit = SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("cargoMaxVehicleSpeedMetresPerSecond");
   if (m_Vehicle.GetPhysics().GetVelocity().Length() > limit && m_UnloadWaits++ < 40)
   {
    GetGame().GetCallqueue().CallLater(Unload, 500, false); return;
   }
   m_Controller.BCMM_VehicleMove(m_Vehicle.GetOrigin() + Vector(4, 0.2, 0));
   array<int> players = {}; GetGame().GetPlayerManager().GetPlayers(players);
   foreach (int id : players)
   {
    SCR_PlayerController peer = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(id));
    if (peer && peer != m_Controller) peer.BCMM_VehicleMove(m_Vehicle.GetOrigin() + Vector(-3, 0.2, 1));
   }
   GetGame().GetCallqueue().CallLater(AttemptUnload, 1500, false); return;
  }
  AttemptUnload();
 }
 protected void AttemptUnload()
 {
  ChimeraCharacter actor = ChimeraCharacter.Cast(m_Actor);
  PrintFormat("BCMM_VEHICLE_UNLOAD actor=%1 vehicle=%2 bag=%3 life=%4 falling=%5 speed=%6", m_Actor.GetOrigin(), m_Vehicle.GetOrigin(), m_Bag.GetOrigin(), actor.GetCharacterController().GetLifeState(), actor.GetCharacterController().IsFalling(), m_Vehicle.GetPhysics().GetVelocity());
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, false, state.Revision()), "Medical unloading begins after native cargo or piloted flight");
  GetGame().GetCallqueue().CallLater(Unloaded, 2000, false);
 }
 protected void Unloaded()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  InventoryStorageManagerComponent vehicle = InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent));
  InventoryStorageManagerComponent bag = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
  Check(state.State() == BCMM_BagCargoState.WORLD && state.VehicleId() == RplId.Invalid() && !item.GetParentSlot() && !vehicle.Contains(m_Bag), "Medical unload clears exact native slot and vehicle ownership");
  Check(vehicle.Contains(m_Gear) && !bag.Contains(m_Gear), "unloading deliberately leaves exact equipment in vehicle as requested");
  Publish(2); GetGame().GetCallqueue().CallLater(DeleteBag, 5000, false);
 }
 protected void DeleteBag()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Bag); Publish(3);
  Check(m_Gear && InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent)).Contains(m_Gear), "deleting unloaded bag preserves equipment left in vehicle");
  GetGame().GetCallqueue().CallLater(Finish, 5000, false);
 }
 protected void Publish(int phase) { SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_VehiclePublish(phase, m_Bag, m_Vehicle, m_Gear, m_Actor); }
 protected void Finish() { GetGame().GetCallqueue().Remove(EnsurePeerBody); PrintFormat("BCMM_VEHICLE_COMPLETE checks=%1 failures=%2", m_Checks, m_Failures); }
}
