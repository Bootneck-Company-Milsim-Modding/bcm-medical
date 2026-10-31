// Real native inventory fixture, diagnostic addon only.
class BCMM_CargoRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected IEntity m_Actor;
 protected IEntity m_Vehicle;
 protected IEntity m_Bag;
 protected IEntity m_Gear;
 protected int m_Fixture;
 protected int m_OldRevision;
 protected int m_WaitCount;
 protected bool m_Network;
 protected bool m_NestedPrepared;
 protected IEntity m_NestedContainer;
 void Start(BCMM_DomainChecks suite) { m_Suite = suite; string enabled; m_Network = System.GetCLIParam("bcmmCargoNetwork", enabled); SpawnFixture(); }
 protected IEntity Spawn(ResourceName path, vector position)
 {
  EntitySpawnParams parameters = new EntitySpawnParams();
  parameters.TransformMode = ETransformMode.WORLD;
  parameters.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(Resource.Load(path), GetGame().GetWorld(), parameters);
 }
 protected void SpawnFixture()
 {
  float baseX = 100 + 20 * m_Fixture;
  float surface = GetGame().GetWorld().GetSurfaceY(baseX, 0);
  m_WaitCount = 0;
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", Vector(baseX, surface + 0.2, 0));
  ResourceName vehicle = "{6AB4B3F3FD216002}Prefabs/Vehicles/BCMMedical/BCMM_CargoAmbulance.et";
  if (m_Fixture == 1) vehicle = "{6AB4B3F3FD216003}Prefabs/Vehicles/BCMMedical/BCMM_CargoHelicopter.et";
  if (m_Fixture == 2) vehicle = "{6AB4B3F3FD216004}Prefabs/Vehicles/BCMMedical/BCMM_CargoTruck.et";
  if (m_Fixture == 3) vehicle = "{6AB4B3F3FD216005}Prefabs/Vehicles/BCMMedical/BCMM_CargoJeep.et";
  m_Vehicle = Spawn(vehicle, Vector(baseX + 3, surface + 0.2, 0));
  m_Bag = Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et", Vector(baseX + 1, surface + 0.5, 0));
  m_Gear = Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et", Vector(baseX + 1, surface + 1, 0));
  if (m_Bag && m_Gear)
  {
   BCMM_BagStorageComponent storage = BCMM_BagStorageComponent.Cast(m_Bag.FindComponent(BCMM_BagStorageComponent));
   InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
   storage.SetTransferring(true);
   m_Suite.Record(bagInventory.TryInsertItemInStorage(m_Gear, storage), "Original equipment entity inserted into body bag for transfer fixture");

  }
  if (m_Vehicle) { m_Vehicle.GetPhysics().SetVelocity(vector.Zero); m_Vehicle.GetPhysics().SetActive(ActiveState.INACTIVE); }
  m_Suite.Record(m_Actor && m_Vehicle && m_Bag, "Cargo fixture entities spawn for vehicle " + m_Fixture);
  GetGame().GetCallqueue().CallLater(Load, 2000);
 }
 protected void Load()
 {
  if (!m_Bag || !m_Actor || !m_Vehicle) { FinishFixture(); return; }
  if (m_Fixture == 0 && !m_NestedPrepared)
  {
   m_NestedPrepared = true;
   m_NestedContainer = Spawn("{06B68C58B72EAAC6}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium.et", m_Vehicle.GetOrigin());
   SCR_VehicleInventoryStorageManagerComponent manager = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
   m_Suite.Record(m_NestedContainer && manager.TryInsertItem(m_NestedContainer, EStoragePurpose.PURPOSE_ANY), "Native movable backpack enters vehicle cargo for classification regression");
   GetGame().GetCallqueue().CallLater(CheckNestedContainer, 250);
   return;
  }
  if (!BCMM_CargoOperation.ActorCanUse(m_Actor, m_Vehicle, m_Bag) && m_WaitCount++ < 40) { GetGame().GetCallqueue().CallLater(Load, 250); return; }
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(m_Vehicle.FindComponent(BCMM_VehicleCargoComponent));
  m_Suite.Record(state && cargo, "Standalone bag/vehicle components exist");
  if (!state || !cargo) { FinishFixture(); return; }
  InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  BCMM_BagStorageComponent.Cast(m_Bag.FindComponent(BCMM_BagStorageComponent)).SetTransferring(false);
  InventoryItemComponent debugItem;
  if (m_Gear) debugItem = InventoryItemComponent.Cast(m_Gear.FindComponent(InventoryItemComponent));
  PrintFormat("BCMM_GEAR initial entity=%1 item=%2 slot=%3 managerContains=%4", m_Gear, debugItem, debugItem.GetParentSlot(), bagInventory.Contains(m_Gear));
  m_Suite.Record(m_Gear && bagInventory.Contains(m_Gear), "Equipment is initially inside the bag");
  state.SetDeceasedName("BCM cargo test casualty");
  m_Suite.Record(state.DeceasedName() == "BCM cargo test casualty", "Authority assigns deceased identity before loading" );
  m_Suite.Record(BCMM_NearbyCargo.Find(m_Bag) == m_Vehicle, "Bag look-at interaction discovers compatible nearby vehicle cargo");
  m_OldRevision = state.Revision();
  PrintFormat("BCMM_CARGO_ITEMS bag=%1 vehicle=%2",Replication.FindItemId(state),Replication.FindItemId(cargo));
  ChimeraCharacter actorCharacter = ChimeraCharacter.Cast(m_Actor);
  SCR_VehicleInventoryStorageManagerComponent manager = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  PrintFormat("BCMM_CARGO fixture=%1 actor=%2 vehicle=%3 bag=%4 life=%5 falling=%6 velocity=%7 cargo=%8", m_Fixture,m_Actor.GetOrigin(),m_Vehicle.GetOrigin(),m_Bag.GetOrigin(),actorCharacter.GetCharacterController().GetLifeState(),actorCharacter.GetCharacterController().IsFalling(),m_Vehicle.GetPhysics().GetVelocity(),manager);
  if (manager) PrintFormat("BCMM_CARGO storage=%1 actorValid=%2", manager.FindStorageForResource(m_Bag.GetPrefabData().GetPrefabName(), EStoragePurpose.PURPOSE_DEPOSIT), BCMM_CargoOperation.ActorCanUse(m_Actor,m_Vehicle,m_Bag));
  bool accepted;
  if (m_Fixture == 3)
  {
   BaseInventoryStorageComponent destination = manager.FindStorageForResource(m_Bag.GetPrefabData().GetPrefabName(), EStoragePurpose.PURPOSE_DEPOSIT);
   accepted = manager.TryInsertItemInStorage(m_Bag, destination);
  }
  else accepted = BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, true, m_OldRevision);
  m_Suite.Record(accepted, "Real native cargo insertion accepted for vehicle " + m_Fixture);
  m_Suite.Record(!BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, true, m_OldRevision), "Duplicate insertion/stale revision rejected");
  if (!accepted) { FinishFixture(); return; }
  m_WaitCount = 0;
  GetGame().GetCallqueue().CallLater(VerifyLoaded, 250);
 }
 protected void CheckNestedContainer()
 {
  InventoryItemComponent item = InventoryItemComponent.Cast(m_NestedContainer.FindComponent(InventoryItemComponent));
  BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(m_NestedContainer.FindComponent(BaseInventoryStorageComponent));
  m_Suite.Record(item && item.GetParentSlot() && storage, "Nested backpack has a real native vehicle parent slot and storage");
  m_Suite.Record(storage && !BCMM_BagInventoryPolicy.VehicleDeposit(storage), "Movable nested backpack is not direct vehicle body-bag cargo");
  Load();
 }
 protected void VerifyLoaded()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.State() == BCMM_BagCargoState.LOADING && m_WaitCount++ < 40) { GetGame().GetCallqueue().CallLater(VerifyLoaded, 250); return; }
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(m_Vehicle.FindComponent(BCMM_VehicleCargoComponent));
  m_Suite.Record(inventory.Contains(m_Bag) && state.State() == BCMM_BagCargoState.STORED, "Native containment and replicated stored state agree");
  InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  InventoryItemComponent gearItem = InventoryItemComponent.Cast(m_Gear.FindComponent(InventoryItemComponent));
  m_Suite.Record(inventory.Contains(m_Gear) && !bagInventory.Contains(m_Gear) && gearItem.GetParentSlot() && gearItem.GetParentSlot().GetStorage().GetOwner() != m_Bag, "Loading transfers the exact equipment entity into vehicle cargo");
  m_Suite.Record(cargo.Count() == 1 && cargo.BagAt(0) == BCMM_BodyBagComponent.Id(m_Bag), "Replicated manifest identifies exact bag");
  m_Suite.Record(state.VehicleId() == BCMM_BodyBagComponent.Id(m_Vehicle), "Bag identifies exact owning vehicle");
  if (m_Network)
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishCargoTest(m_Fixture * 2 + 1, m_Bag, m_Vehicle);
   GetGame().GetCallqueue().CallLater(Unload, 25000);
  }
  else Unload();
 }
 protected void Unload()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  bool accepted;
  if (m_Fixture == 1)
  {
   SCR_VehicleInventoryStorageManagerComponent nativeInventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
   InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
   accepted = nativeInventory.TryRemoveItemFromStorage(m_Bag, item.GetParentSlot().GetStorage(), null);
  }
  else accepted = BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, false, state.Revision());
  m_Suite.Record(accepted, "Native cargo removal accepted");
  m_WaitCount = 0;
  GetGame().GetCallqueue().CallLater(VerifyUnloaded, 250);
 }
 protected void VerifyUnloaded()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.State() != BCMM_BagCargoState.WORLD && m_WaitCount++ < 40) { GetGame().GetCallqueue().CallLater(VerifyUnloaded, 250); return; }
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(m_Vehicle.FindComponent(BCMM_VehicleCargoComponent));
  InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
  m_Suite.Record(!inventory.Contains(m_Bag) && !item.GetParentSlot() && state.State() == BCMM_BagCargoState.WORLD, "Unloaded bag returns to world without inventory ghost");
  InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(m_Gear && inventory.Contains(m_Gear) && !bagInventory.Contains(m_Gear), "Unloading leaves casualty equipment in vehicle as requested");
  m_Suite.Record(cargo.Count() == 0 && state.VehicleId() == RplId.Invalid(), "Unloading clears replicated manifest and vehicle ID");
  if (m_Network)
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishCargoTest(m_Fixture * 2 + 2, m_Bag, m_Vehicle);
   GetGame().GetCallqueue().CallLater(FinishFixture, 7000);
  }
  else FinishFixture();
 }
 protected void FinishFixture()
 {
  if (m_Bag) SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);
  if (m_Vehicle) SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Bag = null; m_Vehicle = null; m_Actor = null;
  m_Fixture++;
  if (m_Fixture < 4) SpawnFixture();
  else m_Suite.Complete();
 }
}
