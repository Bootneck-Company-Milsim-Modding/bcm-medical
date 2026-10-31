// Original narrowly scoped inventory rule: filled bags are vehicle cargo,
// never portable personal inventory. Ordinary items retain native checks.
class BCMM_BagInventoryPolicy
{
 // Existing inventory UI may outlive the Search action's visibility. Recheck
 // the exact containing bag at the native removal boundary, including nested kit.
 static bool RemovalLocked(IEntity owner)
 {
  for(int guard=0;owner && guard<64;guard++)
  {
   BCMM_BodyBagComponent bag=BCMM_BodyBagComponent.Cast(owner.FindComponent(BCMM_BodyBagComponent));
   if(bag)
   {
    BCMM_BagStorageComponent storage=BCMM_BagStorageComponent.Cast(owner.FindComponent(BCMM_BagStorageComponent));
    return !bag.Ready() && (!storage || !storage.IsTransferring());
   }
   owner=owner.GetParent();
  }
  return false;
 }
 static bool VehicleDeposit(BaseInventoryStorageComponent storage)
 {
  if (!storage) return false;
  IEntity owner = storage.GetOwner();
  for (int guard = 0; owner && guard < 32; guard++)
  {
   if (ChimeraCharacter.Cast(owner)) return false;
   if (owner.FindComponent(SCR_VehicleInventoryStorageManagerComponent))
   {
    SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.GetDamageManager(owner);
    return !damage || !damage.IsDestroyed();
   }
   InventoryItemComponent item = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
   // A movable backpack/crate stays ordinary inventory when placed in cargo.
   // Only fixed authored children may resolve through to a vehicle ancestor.
   if (item && item.GetParentSlot()) return false;
   owner = owner.GetParent();
  }
  return false;
 }
 static BaseInventoryStorageComponent FindVehicleStorage(SCR_VehicleInventoryStorageManagerComponent inventory, ResourceName resource)
 {
  if (!inventory) return null;
  BaseInventoryStorageComponent first = inventory.FindStorageForResource(resource, EStoragePurpose.PURPOSE_DEPOSIT);
  if (VehicleDeposit(first)) return first;
  array<BaseInventoryStorageComponent> storages = {};
  inventory.GetStorages(storages, EStoragePurpose.PURPOSE_DEPOSIT);
  foreach (BaseInventoryStorageComponent storage : storages)
   if (VehicleDeposit(storage) && inventory.CanInsertResourceInStorage(resource, storage)) return storage;
  return null;
 }
 static IEntity VehicleOwner(BaseInventoryStorageComponent storage)
 {
  if (!VehicleDeposit(storage)) return null;
  IEntity owner = storage.GetOwner();
  for (int guard = 0; owner && guard < 32; guard++)
  {
   if (owner.FindComponent(SCR_VehicleInventoryStorageManagerComponent)) return owner;
   owner = owner.GetParent();
  }
  return null;
 }
 static bool FilledResource(ResourceName name)
 {
  return name == "{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et" || name == "Prefabs/BodyBags/BCMM_FilledBodyBag.et";
 }
}
modded class ScriptedBaseInventoryStorageComponent
{
 // Native caches this override flag: eligibility itself must remain dynamic.
 override bool OnOverrideCanRemoveItem() { return true; }
 override bool CanRemoveItem(IEntity item)
 {
  if(BCMM_BagInventoryPolicy.RemovalLocked(GetOwner()))return false;
  return super.CanRemoveItem(item);
 }
}
modded class SCR_UniversalInventoryStorageComponent
{
 override bool CanStoreItem(IEntity item, int slotID)
 {
  BCMM_BodyBagComponent bag;
  if(item)bag=BCMM_BodyBagComponent.Cast(item.FindComponent(BCMM_BodyBagComponent));
  if(bag && (!bag.Ready() || BCMM_BagSearchSessions.IsSearched(item) || !BCMM_BagInventoryPolicy.VehicleDeposit(this)))return false;
  return super.CanStoreItem(item, slotID);
 }
 override bool CanStoreResource(ResourceName resourceName, int slotID)
 {
  if (BCMM_BagInventoryPolicy.FilledResource(resourceName) && !BCMM_BagInventoryPolicy.VehicleDeposit(this)) return false;
  return super.CanStoreResource(resourceName, slotID);
 }
}
