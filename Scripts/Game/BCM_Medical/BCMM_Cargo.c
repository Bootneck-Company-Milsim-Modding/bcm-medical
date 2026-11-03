// Original BCM native vehicle-cargo implementation. No ACE dependency.
enum BCMM_BagCargoState { WORLD, LOADING, STORED, UNLOADING, VEHICLE_LOST }
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_BodyBagComponentClass : ScriptComponentClass {}
class BCMM_BodyBagComponent : ScriptComponent
{
 static ref array<BCMM_BodyBagComponent> Instances = {};
 [RplProp()] protected BCMM_BagCargoState m_State;
 [RplProp()] protected RplId m_Vehicle = RplId.Invalid();
 [RplProp()] protected int m_Revision;
 [RplProp()] protected string m_DeceasedName;
 [RplProp()] protected int m_RemainsToken;
 int RemainsToken() { return m_RemainsToken; }
 void SetRemainsToken(int remainsToken)
 {
  if(!Replication.IsServer())
   return;
  m_RemainsToken=remainsToken;
  Replication.BumpMe();
 }
 [RplProp()] protected bool m_Ready = true;
 bool Ready() { return m_Ready; }
 [RplProp()] protected bool m_Searching;
 bool Searching() { return m_Searching; }
 void SetSearching(bool value)
 {
  if(!Replication.IsServer() || value==m_Searching)return;
  m_Searching=value;m_Revision++;Replication.BumpMe();
 }
 void SetReady(bool value)
 {
  if (!Replication.IsServer()) return;
  m_Ready = value;
  Replication.BumpMe();
 }
 [RplProp()] protected RplId m_DraggingActor = RplId.Invalid();
 [RplProp(onRplName:"ApplyDragCollision")] protected BCMM_TransportMode m_DragMode;
 protected ref BCMM_TransportCollision m_DragCollision=new BCMM_TransportCollision();
 protected void ApplyDragCollision()
 {
  if(m_DragMode==BCMM_TransportMode.BODY_BAG_MANUAL_DRAG || m_DragMode==BCMM_TransportMode.BODY_BAG_ROPE_DRAG)
   m_DragCollision.Apply(GetOwner(),EPhysicsLayerPresets.ItemFireView);
  else m_DragCollision.Restore(GetOwner());
 }
 RplId DraggingActor() { return m_DraggingActor; }
 BCMM_TransportMode DragMode() { return m_DragMode; }
 void SetDragging(RplId actor, BCMM_TransportMode mode)
 {
  if (!Replication.IsServer()) return;
  m_DraggingActor = actor;
  m_DragMode = mode;
  ApplyDragCollision();
  m_Revision++;
  Replication.BumpMe();
 }
 protected float m_ReconcileTimer;
 string DeceasedName() { return m_DeceasedName; }
 void SetDeceasedName(string name)
 {
  if (!Replication.IsServer()) return;
  m_DeceasedName = name;
  Replication.BumpMe();
 }
 override protected void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  Instances.Insert(this);
  BCMM_RemainsProtection.ProtectBag(owner);
  SetEventMask(owner, EntityEvent.FRAME);
 }
 override protected void EOnFrame(IEntity owner, float timeSlice)
 {
  ApplyDragCollision();
  if (m_State == BCMM_BagCargoState.VEHICLE_LOST) return;
  if (!Replication.IsServer() || m_State == BCMM_BagCargoState.LOADING || m_State == BCMM_BagCargoState.UNLOADING) return;
  m_ReconcileTimer += timeSlice;
  if (m_ReconcileTimer < 0.25) return;
  m_ReconcileTimer = 0;
  if (m_State == BCMM_BagCargoState.STORED)
  {
   IEntity storedVehicle = Resolve(m_Vehicle);
   SCR_DamageManagerComponent damage;
   if (storedVehicle) damage = SCR_DamageManagerComponent.GetDamageManager(storedVehicle);
   // Native hierarchy remains the authority: a completed external move away
   // from a wreck must reconcile its new destination before any cleanup.
   InventoryStorageManagerComponent storedInventory;
   if (storedVehicle) storedInventory = InventoryStorageManagerComponent.Cast(storedVehicle.FindComponent(InventoryStorageManagerComponent));
   if (damage && damage.IsDestroyed() && storedInventory && storedInventory.Contains(owner))
   {
    ResolveVehicleLoss(storedVehicle);
    return;
   }
  }
  InventoryItemComponent item = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
  if (!item) return;
  InventoryStorageSlot slot = item.GetParentSlot();
  if (m_State == BCMM_BagCargoState.STORED && !slot)
  {
   IEntity vehicle = Resolve(m_Vehicle);
   BCMM_VehicleCargoComponent cargo;
   if (vehicle) cargo = BCMM_VehicleCargoComponent.Cast(vehicle.FindComponent(BCMM_VehicleCargoComponent));
   if (cargo) cargo.RemoveBag(owner);
   Finish(false, RplId.Invalid());
  }
  else if (slot && slot.GetStorage())
  {
   IEntity vehicle = BCMM_BagInventoryPolicy.VehicleOwner(slot.GetStorage());
   if (!vehicle) return;
   SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
   if (!inventory || !inventory.Contains(owner)) return;
   if (m_State == BCMM_BagCargoState.STORED)
   {
    if (m_Vehicle == Id(vehicle)) return;
    // A native UI/privileged transfer can move A -> B without a world frame.
    // Release A's manifest before reconciling B through the existing equipment
    // coordinator. Equipment already left in A remains there by design.
    IEntity previous = Resolve(m_Vehicle);
    BCMM_VehicleCargoComponent oldCargo;
    if (previous) oldCargo = BCMM_VehicleCargoComponent.Cast(previous.FindComponent(BCMM_VehicleCargoComponent));
    if (oldCargo) oldCargo.RemoveBag(owner);
    Finish(false, RplId.Invalid());
   }
   BCMM_CargoOperation.ReconcileDeposit(vehicle, owner, m_Revision);
  }
 }
 override void OnDelete(IEntity owner)
 {
  m_DragCollision.Restore(owner);
  if(GetGame()) GetGame().GetCallqueue().Remove(DeleteLostRemains);
  if (Replication.IsServer())
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   if(mode) mode.BCMM_RemainsDeleted(m_RemainsToken,owner);
   IEntity vehicle=Resolve(m_Vehicle);
   BCMM_VehicleCargoComponent cargo;
   if(vehicle) cargo=BCMM_VehicleCargoComponent.Cast(vehicle.FindComponent(BCMM_VehicleCargoComponent));
   if(cargo) cargo.RemoveBag(owner);
  }
  BCMM_RemainsProtection.Forget(owner); Instances.RemoveItem(this); super.OnDelete(owner);
 }
 void ResolveVehicleLoss(IEntity vehicle)
 {
  if(!Replication.IsServer() || !vehicle || m_State==BCMM_BagCargoState.VEHICLE_LOST) return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode && m_RemainsToken>0) mode.BCMM_CommitRemains(m_RemainsToken,GetOwner(),BCMM_RemainsDisposition.VEHICLE_LOST);
  BCMM_VehicleCargoComponent cargo=BCMM_VehicleCargoComponent.Cast(vehicle.FindComponent(BCMM_VehicleCargoComponent));
  if(cargo) cargo.RemoveBag(GetOwner());
  m_State=BCMM_BagCargoState.VEHICLE_LOST;
  m_Revision++;
  Replication.BumpMe();
  // Never re-enter native inventory teardown from its current callback/frame.
  GetGame().GetCallqueue().CallLater(DeleteLostRemains,100,false);
 }
 protected void DeleteLostRemains()
 {
  if(Replication.IsServer() && GetOwner()) SCR_EntityHelper.DeleteEntityAndChildren(GetOwner());
 }
 BCMM_BagCargoState State() { return m_State; }
 RplId VehicleId() { return m_Vehicle; }
 int Revision() { return m_Revision; }
 bool Reserve(bool loading, int expectedRevision)
 {
  if (!Replication.IsServer() || !m_Ready || expectedRevision != m_Revision) return false;
  if ((loading && m_State != BCMM_BagCargoState.WORLD) || (!loading && m_State != BCMM_BagCargoState.STORED)) return false;
  if (loading) m_State = BCMM_BagCargoState.LOADING;
  else m_State = BCMM_BagCargoState.UNLOADING;
  m_Revision++;
  Replication.BumpMe();
  return true;
 }
 void Finish(bool stored, RplId vehicle)
 {
  if (!Replication.IsServer()) return;
  m_State = BCMM_BagCargoState.WORLD;
  m_Vehicle = RplId.Invalid();
  if (stored) { m_State = BCMM_BagCargoState.STORED; m_Vehicle = vehicle; }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode && m_RemainsToken>0) mode.BCMM_UpdateStoredRemains(m_RemainsToken,GetOwner(),Resolve(m_Vehicle));
  m_Revision++;
  Replication.BumpMe();
 }
 static RplId Id(IEntity entity)
 {
  if (!entity) return RplId.Invalid();
  RplComponent component = RplComponent.Cast(entity.FindComponent(RplComponent));
  if (component) return component.Id();
  return RplId.Invalid();
 }
 static IEntity Resolve(RplId id)
 {
  RplComponent component = RplComponent.Cast(Replication.FindItem(id));
  if (component) return component.GetEntity();
  return null;
 }
 static BCMM_BodyBagComponent Nearest(IEntity actor, float range)
 {
  if (!actor) return null;
  BCMM_BodyBagComponent result;
  float best = range * range;
  foreach (BCMM_BodyBagComponent candidate : Instances)
  {
   if (!candidate || !candidate.GetOwner() || candidate.State() != BCMM_BagCargoState.WORLD) continue;
   float distance = vector.DistanceSq(actor.GetOrigin(), candidate.GetOwner().GetOrigin());
   if (distance > best) continue;
   best = distance; result = candidate;
  }
  return result;
 }
}
// Standard native vehicle deletion reaches this without a BCM prefab override.
modded class SCR_VehicleInventoryStorageManagerComponent
{
 override protected void OnDelete(IEntity owner)
 {
  if(Replication.IsServer())
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   if(mode) mode.BCMM_RemainsVehicleDeleted(owner);
   // Snapshot: native child deletion can change the live instance list.
   array<BCMM_BodyBagComponent> bags={};
   bags.Copy(BCMM_BodyBagComponent.Instances);
   foreach(BCMM_BodyBagComponent bag:bags)
    if(bag && bag.GetOwner() && Contains(bag.GetOwner())) bag.ResolveVehicleLoss(owner);
  }
  super.OnDelete(owner);
 }
}
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_VehicleCargoComponentClass : ScriptComponentClass {}
class BCMM_VehicleCargoComponent : ScriptComponent
{
 [RplProp()] protected ref array<RplId> m_Manifest = {};
 [RplProp()] protected ref array<string> m_Names = {};
 string NameAt(int index)
 {
  if (m_Names.IsIndexValid(index)) return m_Names[index];
  return "";
 }
 protected ref array<IEntity> m_ServerBags = {};
 override protected void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);

 }
 int Count() { return m_Manifest.Count(); }
 RplId BagAt(int index)
 {
  if (m_Manifest.IsIndexValid(index)) return m_Manifest[index];
  return RplId.Invalid();
 }
 void RegisterBag(IEntity bag)
 {
  if (!Replication.IsServer() || m_ServerBags.Contains(bag)) return;
  m_ServerBags.Insert(bag);
  m_Manifest.Insert(BCMM_BodyBagComponent.Id(bag));
  BCMM_BodyBagComponent identity = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if (identity) m_Names.Insert(identity.DeceasedName()); else m_Names.Insert(string.Empty);
  Replication.BumpMe();
 }
 void RemoveBag(IEntity bag)
 {
  if (!Replication.IsServer()) return;
  m_ServerBags.RemoveItem(bag);
  int index = m_Manifest.Find(BCMM_BodyBagComponent.Id(bag));
  if (index >= 0) { m_Manifest.Remove(index); if (m_Names.IsIndexValid(index)) m_Names.Remove(index); }
  Replication.BumpMe();
 }
}

// The native manager remains the sole owner of inventory hierarchy/visibility.
// Published BCM state is committed only after the asynchronous inventory result.
class BCMM_CargoOperation : ScriptedInventoryOperationCallback
{
 protected static ref array<ref BCMM_CargoOperation> s_Pending = {};
 protected IEntity m_Bag;
 protected IEntity m_Vehicle;
 protected BCMM_BodyBagComponent m_State;
 protected BCMM_VehicleCargoComponent m_Cargo;
 protected SCR_VehicleInventoryStorageManagerComponent m_Inventory;
 protected InventoryStorageManagerComponent m_BagInventory;
 protected BCMM_BagStorageComponent m_BagStorage;
 protected BaseInventoryStorageComponent m_Deposit;
 protected ref array<ref BCMM_BagTransferEntry> m_Gear = {};
 protected int m_GearIndex;
 protected int m_RestoreIndex;
 protected int m_Phase; // 0 bag operation, 1 gear transfer, 2 rollback
 protected bool m_Loading;
 protected bool m_Complete;
 protected bool m_RestoreFailed;
 protected bool m_AlreadyStored;
 static float InteractionRange()
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) return mode.BCMM_ReadSetting("cargoInteractionRangeMetres");
  return 0;
 }
 static bool ActorCanUse(IEntity actor, IEntity vehicle, IEntity bag)
 {
  if (!actor || !vehicle || !bag) return false;
  if(BCMM_BagSearchSessions.IsSearched(bag))return false;
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character || character.IsInVehicle()) return false;
  CharacterControllerComponent controller = character.GetCharacterController();
  if (!controller || controller.GetLifeState() != ECharacterLifeState.ALIVE || controller.IsUnconscious() || controller.IsSwimming() || controller.IsFalling()) return false;
  float range = InteractionRange();
  if (vector.Distance(actor.GetOrigin(), vehicle.GetOrigin()) > range || vector.Distance(actor.GetOrigin(), bag.GetOrigin()) > range || vector.Distance(bag.GetOrigin(), vehicle.GetOrigin()) > range) return false;
  SCR_DamageManagerComponent damage = SCR_DamageManagerComponent.GetDamageManager(vehicle);
  if (damage && damage.IsDestroyed()) return false;
  Physics physics = vehicle.GetPhysics();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (physics && mode && physics.GetVelocity().Length() > mode.BCMM_ReadSetting("cargoMaxVehicleSpeedMetresPerSecond")) return false;
  return true;
 }
 static bool Begin(IEntity actor, IEntity vehicle, IEntity bag, bool loading, int revision)
 {
  if (!Replication.IsServer() || !ActorCanUse(actor, vehicle, bag)) return false;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if (!state || state.Revision() != revision) return false;
  if (state.DraggingActor() != RplId.Invalid())
  {
   if (!BCMM_NativeBagDrag.Release(actor, bag)) return false;
   revision = state.Revision();
  }
  return Start(vehicle, bag, loading, revision, false);
 }
 // Native inventory UI already authenticates its own insertion. Reconciliation
 // uses this only for a bag actually contained in that exact native vehicle.
 static bool ReconcileDeposit(IEntity vehicle, IEntity bag, int revision)
 {
  if (!Replication.IsServer() || !vehicle || !bag) return false;
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  if (!inventory || !inventory.Contains(bag)) return false;
  return Start(vehicle, bag, true, revision, true);
 }
 protected static bool Start(IEntity vehicle, IEntity bag, bool loading, int revision, bool alreadyStored)
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  InventoryItemComponent item = InventoryItemComponent.Cast(bag.FindComponent(InventoryItemComponent));
  if (!state || !inventory || !item) return false;
  BaseInventoryStorageComponent storage;
  if (loading && !alreadyStored)
  {
   if (item.GetParentSlot() || bag.GetParent()) return false;
   storage = BCMM_BagInventoryPolicy.FindVehicleStorage(inventory, bag.GetPrefabData().GetPrefabName());
   if (!storage) return false;
  }
  if (!loading)
  {
   if (state.VehicleId() != BCMM_BodyBagComponent.Id(vehicle) || !inventory.Contains(bag) || !item.GetParentSlot()) return false;
   storage = item.GetParentSlot().GetStorage();
  }
  if (!state.Reserve(loading, revision)) return false;
  BCMM_CargoOperation operation = new BCMM_CargoOperation();
  operation.m_Bag = bag;
  operation.m_Vehicle = vehicle;
  operation.m_State = state;
  operation.m_Inventory = inventory;
  operation.m_Cargo = BCMM_VehicleCargoComponent.Cast(vehicle.FindComponent(BCMM_VehicleCargoComponent));
  operation.m_Deposit = storage;
  operation.m_Loading = loading;
  operation.m_AlreadyStored = alreadyStored;
  operation.m_BagInventory = InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent));
  operation.m_BagStorage = BCMM_BagStorageComponent.Cast(bag.FindComponent(BCMM_BagStorageComponent));
  s_Pending.Insert(operation);
  if (loading)
  {
   operation.SnapshotGear();
   operation.m_Phase = 1;
   operation.MoveNextGear();
   return true;
  }
  bool accepted = inventory.TryRemoveItemFromStorage(bag, storage, operation);
  if (!accepted && !operation.m_Complete) operation.OnFailed();
  return accepted;
 }
 protected void SnapshotGear()
 {
  if (!m_BagInventory) return;
  array<IEntity> items = {};
  m_BagInventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity gear : items)
  {
   InventoryItemComponent item;
   if (gear) item = InventoryItemComponent.Cast(gear.FindComponent(InventoryItemComponent));
   if (!item || !item.GetParentSlot() || !item.GetParentSlot().GetStorage()) continue;
   if (item.GetParentSlot().GetStorage().GetOwner() != m_Bag) continue;
   bool duplicate = false;
   foreach (BCMM_BagTransferEntry existing : m_Gear) if (existing.Item == gear) duplicate = true;
   if (duplicate) continue;
   BCMM_BagTransferEntry entry = new BCMM_BagTransferEntry();
   entry.Item = gear;
   entry.Storage = item.GetParentSlot().GetStorage();
   entry.Slot = item.GetParentSlot().GetID();
   m_Gear.Insert(entry);
  }
 }
 protected void MoveNextGear()
 {
  if (!m_Bag || !m_Vehicle || !m_Inventory) { Rollback(); return; }
  if (m_GearIndex >= m_Gear.Count())
  {
   m_Phase = 0;
   if (m_AlreadyStored) { CompleteBag(); return; }
   if (!m_Inventory.TryInsertItemInStorage(m_Bag, m_Deposit, -1, this) && !m_Complete && m_Phase == 0) Rollback();
   return;
  }
  IEntity gear = m_Gear[m_GearIndex].Item;
  BaseInventoryStorageComponent storage;
  if (gear && gear.GetPrefabData()) storage = m_Inventory.FindStorageForResource(gear.GetPrefabData().GetPrefabName(), EStoragePurpose.PURPOSE_DEPOSIT);
  PrintFormat("BCMM_CARGO_GEAR index=%1/%2 item=%3 destination=%4", m_GearIndex, m_Gear.Count(), gear, storage);
  if (!storage || !m_BagInventory || !m_BagInventory.TryMoveItemToStorage(gear, storage, -1, this))
   if (!m_Complete && m_Phase == 1) Rollback();
 }
 override protected void OnComplete()
 {
  if (m_Complete) return;
  // Native inventory may complete synchronously. Yield between items so a
  // fully loaded casualty cannot exhaust the script stack in one callback.
  if (m_Phase == 1) { m_GearIndex++; GetGame().GetCallqueue().CallLater(MoveNextGear, 1); return; }
  if (m_Phase == 2) { m_RestoreIndex--; GetGame().GetCallqueue().CallLater(RestoreNext, 1); return; }
  bool contained = m_Inventory && m_Bag && m_Inventory.Contains(m_Bag);
  if (m_Loading && !contained) { Rollback(); return; }
  CompleteBag();
 }
 override protected void OnFailed()
 {
  PrintFormat("BCMM_CARGO_CALLBACK_FAILED phase=%1 index=%2", m_Phase, m_GearIndex);
  if (m_Complete) return;
  if (m_Phase == 2)
  {
   m_RestoreFailed = true;
   m_RestoreIndex--;
   GetGame().GetCallqueue().CallLater(RestoreNext, 1);
   return;
  }
  if (m_Loading) Rollback();
  else CompleteBag();
 }
 protected void Rollback()
 {
  PrintFormat("BCMM_CARGO_ROLLBACK phase=%1 index=%2", m_Phase, m_GearIndex);
  if (m_Complete || m_Phase == 2) return;
  m_Phase = 2;
  if (m_BagStorage) m_BagStorage.SetTransferring(true);
  m_RestoreIndex = m_Gear.Count() - 1;
  RestoreNext();
 }
 protected void RestoreNext()
 {
  if (m_RestoreIndex < 0)
  {
   if (m_BagStorage) m_BagStorage.SetTransferring(false);
   if (m_RestoreFailed) Print("BCMM cargo rollback incomplete: original gear preserved in bag/vehicle; no items cloned or deleted", LogLevel.ERROR);
   CompleteBag();
   return;
  }
  BCMM_BagTransferEntry entry = m_Gear[m_RestoreIndex];
  InventoryItemComponent item;
  if (entry.Item) item = InventoryItemComponent.Cast(entry.Item.FindComponent(InventoryItemComponent));
  if (item && item.GetParentSlot() && item.GetParentSlot().GetStorage() == entry.Storage)
  {
   m_RestoreIndex--;
   GetGame().GetCallqueue().CallLater(RestoreNext, 1);
   return;
  }
  if (!entry.Item || !m_Inventory || !m_Inventory.Contains(entry.Item))
  {
   m_RestoreIndex--;
   GetGame().GetCallqueue().CallLater(RestoreNext, 1);
   return;
  }
  if (!entry.Storage || !m_Inventory.TryMoveItemToStorage(entry.Item, entry.Storage, entry.Slot, this))
   if (!m_Complete && m_Phase == 2) OnFailed();
 }
 protected void CompleteBag()
 {
  if (m_Complete) return;
  bool contained = m_Inventory && m_Bag && m_Inventory.Contains(m_Bag);
  if (m_State) m_State.Finish(contained, BCMM_BodyBagComponent.Id(m_Vehicle));
  if (m_Cargo)
  {
   if (contained) m_Cargo.RegisterBag(m_Bag);
   else m_Cargo.RemoveBag(m_Bag);
  }
  if (!contained && m_Bag && !m_Loading)
  {
   InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
   if (item) item.PlaceOnGround(m_Vehicle);
  }
  m_Complete = true;
  s_Pending.RemoveItem(this);
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestBagCargo(IEntity vehicle, IEntity bag, bool loading)
 {
  if (GetGame().GetPlayerController() != this || !bag) return;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if (!state) return;
  Rpc(BCMM_CargoRequest, BCMM_BodyBagComponent.Id(vehicle), BCMM_BodyBagComponent.Id(bag), loading, state.Revision());
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_CargoRequest(RplId vehicle, RplId bag, bool loading, int revision)
 {
  // The RPC owner supplies the actor; there is no client-supplied actor ID.
  BCMM_CargoOperation.Begin(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(vehicle), BCMM_BodyBagComponent.Resolve(bag), loading, revision);
 }
}
class BCMM_LoadBagAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(GetOwner().FindComponent(BCMM_BodyBagComponent));
  return bag && bag.Ready() && bag.State() == BCMM_BagCargoState.WORLD && BCMM_NearbyCargo.Find(GetOwner());
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  IEntity vehicle = BCMM_NearbyCargo.Find(pOwnerEntity);
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player && vehicle) player.BCMM_RequestBagCargo(vehicle, pOwnerEntity, true);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}class BCMM_UnloadBagAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(GetOwner().FindComponent(BCMM_VehicleCargoComponent));
  return cargo && cargo.Count() > 0;
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(pOwnerEntity.FindComponent(BCMM_VehicleCargoComponent));
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player && cargo) player.BCMM_RequestBagCargo(pOwnerEntity, BCMM_BodyBagComponent.Resolve(cargo.BagAt(0)), false);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_NearbyCargo
{
 protected IEntity m_Bag;
 protected IEntity m_Best;
 protected float m_Distance;
 static IEntity Find(IEntity bag)
 {
  if (!bag) return null;
  BCMM_NearbyCargo query = new BCMM_NearbyCargo();
  query.m_Bag = bag;
  query.m_Distance = BCMM_CargoOperation.InteractionRange();
  bag.GetWorld().QueryEntitiesBySphere(bag.GetOrigin(), query.m_Distance, query.Collect, null, EQueryEntitiesFlags.DYNAMIC);
  return query.m_Best;
 }
 protected bool Collect(IEntity entity)
 {
  IEntity vehicle = entity.GetRootParent();
  if (!vehicle || vehicle == m_Bag) return true;
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  if (!inventory || !m_Bag.GetPrefabData()) return true;
  float distance = vector.Distance(vehicle.GetOrigin(), m_Bag.GetOrigin());
  if (distance > m_Distance || !BCMM_BagInventoryPolicy.FindVehicleStorage(inventory, m_Bag.GetPrefabData().GetPrefabName())) return true;
  m_Distance = distance; m_Best = vehicle;
  return true;
 }
}
class BCMM_BagUIInfo : SCR_InventoryUIInfo
{
 override string GetInventoryItemName(InventoryItemComponent item)
 {
  if (!item) return "Body bag";
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(item.GetOwner().FindComponent(BCMM_BodyBagComponent));
  if (bag && !bag.DeceasedName().IsEmpty()) return "Body bag: " + bag.DeceasedName();
  return "Body bag";
 }
}
class BCMM_BagNameAction : ScriptedUserAction
{
 override bool GetActionNameScript(out string outName)
 {
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(GetOwner().FindComponent(BCMM_BodyBagComponent));
  outName = "Body bag";
  if (bag && !bag.DeceasedName().IsEmpty()) outName = "Body bag: " + bag.DeceasedName();
  return true;
 }
}
class BCMM_SearchBagAction : SCR_OpenStorageAction
{
 override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager,IEntity pOwnerEntity,IEntity pUserEntity)
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(player && player.GetControlledEntity()==pUserEntity)player.BCMM_RequestBagSearch(pOwnerEntity);
 }
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(GetOwner().FindComponent(BCMM_BodyBagComponent));
  return bag && BCMM_BagSearchSessions.Eligible(user,GetOwner()) && super.CanBeShownScript(user);
 }
}
