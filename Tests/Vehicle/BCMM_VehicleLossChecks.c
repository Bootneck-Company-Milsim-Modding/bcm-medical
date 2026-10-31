// Authority-only native destruction; equipment already deposited remains vehicle cargo.
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_LossTestKill() { m_BCMMDeathPermitted=true; Kill(Instigator.CreateInstigator(GetOwner())); }
}
modded class BCMM_VehicleLifecycleChecks
{
 protected IEntity m_OutsideBag;
 protected IEntity m_WreckCandidate;
 protected int m_LossToken;
 protected IEntity m_DeleteVehicle;
 protected int m_DeleteToken;
 protected IEntity m_SeedCorpse;
 protected int m_ManualToken;
 protected IEntity m_ManualGear;
 protected IEntity SpawnDeadSource()
 {
  IEntity corpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"0 0 15");
  if(!corpse) return null;
  SCR_CharacterDamageManagerComponent.Cast(corpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_LossTestKill();
  return corpse;
 }
 protected int SeedDeath(IEntity bag)
 {
  IEntity corpse=m_SeedCorpse;
  if(!corpse) return 0;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  int token=mode.BCMM_RegisterRemains(corpse);
  if(!mode.BCMM_TransferRemains(corpse,bag)) return 0;
  SCR_EntityHelper.DeleteEntityAndChildren(corpse);
  return token;
 }
 override protected void Loaded()
 {
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent));
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(inventory.Contains(m_Bag) && inventory.Contains(m_Gear) && state.State()==BCMM_BagCargoState.STORED,"native loss fixture has exact stored bag and deposited equipment");
  m_SeedCorpse=SpawnDeadSource();
  m_OutsideBag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et",m_Actor.GetOrigin()+"0 0 8");
  m_WreckCandidate=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et",m_Actor.GetOrigin()+"0 0 10");
  Check(m_OutsideBag && m_WreckCandidate,"independent world bag and wreck-insertion candidate spawn");
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.GetDamageManager(m_Vehicle);
  Check(damage!=null,"ordinary native truck has real damage manager");
  if(m_Failures){Finish();return;}
  GetGame().GetCallqueue().CallLater(BindAndDestroy,1500,false);
 }
 protected void BindAndDestroy()
 {
  m_LossToken=SeedDeath(m_Bag);
  Check(m_LossToken>0,"stored loss fixture carries exact original native death token");
  if(m_Failures){Finish();return;}
  SCR_DamageManagerComponent.GetDamageManager(m_Vehicle).Kill(Instigator.CreateInstigator(m_Actor));
  GetGame().GetCallqueue().CallLater(VerifyWreckLoss,3000,false);
 }
 protected void VerifyWreckLoss()
 {
  SCR_DamageManagerComponent damage=SCR_DamageManagerComponent.GetDamageManager(m_Vehicle);
  Check(damage && damage.IsDestroyed(),"native damage genuinely destroys loaded vehicle");
  Check(!m_Bag || m_Bag.IsDeleted(),"destroyed vehicle disposes its exact stored remains without ejection");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_LossToken);
  Check(record && record.Disposition==BCMM_RemainsDisposition.VEHICLE_LOST,"wreck disposal retains VEHICLE_LOST after physical bag deletion");
  Check(m_OutsideBag && !m_OutsideBag.IsDeleted(),"vehicle loss preserves independent world remains");
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  Check(!BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_WreckCandidate.GetPrefabData().GetPrefabName()),"wreck offers no eligible Medical native bag storage");
  array<BaseInventoryStorageComponent> storages={};inventory.GetStorages(storages,EStoragePurpose.PURPOSE_DEPOSIT);
  bool accepted;
  foreach(BaseInventoryStorageComponent storage:storages)
   if(inventory.TryInsertItemInStorage(m_WreckCandidate,storage)) accepted=true;
  Check(!accepted && !inventory.Contains(m_WreckCandidate),"actual native storage insertion rejects a new filled bag into wreck");
  Check(!m_Gear || m_Gear.IsDeleted() || inventory.Contains(m_Gear),"deposited equipment follows native wreck lifetime without being moved to a world bag");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
  GetGame().GetCallqueue().CallLater(VerifyDeletedWreck,1500,false);
 }
 protected void VerifyDeletedWreck()
 {
  Check(!m_Gear || m_Gear.IsDeleted(),"deleting wreck disposes its native equipment without duplication");
  Check(m_OutsideBag && m_WreckCandidate,"wreck deletion preserves both independent world bags");
  m_SeedCorpse=SpawnDeadSource();
  m_DeleteVehicle=Spawn("{2D74C39A650A3030}Prefabs/Vehicles/Wheeled/M923A1/M923A1_engineer.et",m_Actor.GetOrigin()+"0 2 -15");
  Check(m_DeleteVehicle && m_SeedCorpse,"direct-deletion fixture spawns second native vehicle and casualty");
  if(m_Failures){Finish();return;}
  GetGame().GetCallqueue().CallLater(StoreBeforeDeletion,2000,false);
 }
 protected void StoreBeforeDeletion()
 {
  m_DeleteToken=SeedDeath(m_OutsideBag);
  Check(m_DeleteToken>0 && m_DeleteToken!=m_LossToken,"second native DEAD casualty receives a distinct death token");
  if(m_Failures){Finish();return;}
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_DeleteVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BaseInventoryStorageComponent storage=BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_OutsideBag.GetPrefabData().GetPrefabName());
  Check(storage && inventory.TryInsertItemInStorage(m_OutsideBag,storage),"native API stores original second bag before direct vehicle deletion");
  GetGame().GetCallqueue().CallLater(DeleteLoadedVehicle,1000,false);
 }
 protected void DeleteLoadedVehicle()
 {
  Check(BCMM_BodyBagComponent.Cast(m_OutsideBag.FindComponent(BCMM_BodyBagComponent)).State()==BCMM_BagCargoState.STORED,"second bag reconciles stored state before hierarchy deletion");
  SCR_EntityHelper.DeleteEntityAndChildren(m_DeleteVehicle);
  GetGame().GetCallqueue().CallLater(VerifyDirectDeletion,1500,false);
 }
 protected void VerifyDirectDeletion()
 {
  Check(!m_OutsideBag || m_OutsideBag.IsDeleted(),"direct vehicle deletion removes its exact stored bag");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_DeleteToken);
  Check(record && record.Disposition==BCMM_RemainsDisposition.VEHICLE_LOST,"direct hierarchy deletion commits VEHICLE_LOST rather than admin release");
  Check(m_WreckCandidate && !m_WreckCandidate.IsDeleted(),"second vehicle deletion leaves unrelated world bag untouched");
  m_SeedCorpse=SpawnDeadSource();
  m_DeleteVehicle=Spawn("{2D74C39A650A3030}Prefabs/Vehicles/Wheeled/M923A1/M923A1_engineer.et",m_Actor.GetOrigin()+"0 2 -15");
  m_ManualGear=Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et",m_Actor.GetOrigin()+"0 1 3");
  Check(m_SeedCorpse && m_DeleteVehicle && m_ManualGear,"manual stored-bag deletion spawns independent live vehicle, native casualty and equipment");
  GetGame().GetCallqueue().CallLater(StoreBeforeManualBagDeletion,2000,false);
 }
 protected void StoreBeforeManualBagDeletion()
 {
  m_ManualToken=SeedDeath(m_WreckCandidate);
  Check(m_ManualToken>0 && m_ManualToken!=m_DeleteToken,"manual deletion uses a distinct original native death token");
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_DeleteVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BaseInventoryStorageComponent storage=BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_WreckCandidate.GetPrefabData().GetPrefabName());
  Check(storage && inventory.TryInsertItemInStorage(m_WreckCandidate,storage),"manual-deletion bag enters actual native vehicle slot");
  Check(inventory.TryInsertItem(m_ManualGear,EStoragePurpose.PURPOSE_DEPOSIT),"unrelated exact equipment enters surviving vehicle");
  GetGame().GetCallqueue().CallLater(DeleteStoredBagOnly,1000,false);
 }
 protected void DeleteStoredBagOnly()
 {
  Check(BCMM_BodyBagComponent.Cast(m_WreckCandidate.FindComponent(BCMM_BodyBagComponent)).State()==BCMM_BagCargoState.STORED,"manual-deletion bag reconciles real vehicle ownership");
  SCR_EntityHelper.DeleteEntityAndChildren(m_WreckCandidate);
  GetGame().GetCallqueue().CallLater(VerifyManualBagDeletion,1000,false);
 }
 protected void VerifyManualBagDeletion()
 {
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_ManualToken);
  Check(!m_WreckCandidate && record && record.Disposition==BCMM_RemainsDisposition.ADMIN_RELEASED && !record.DeletionPending,"explicit stored-bag deletion becomes ADMIN_RELEASED after child-first grace interval");
  Check(m_DeleteVehicle && !SCR_DamageManagerComponent.GetDamageManager(m_DeleteVehicle).IsDestroyed(),"manual stored-bag deletion preserves actual live parent vehicle");
  Check(m_ManualGear && InventoryStorageManagerComponent.Cast(m_DeleteVehicle.FindComponent(InventoryStorageManagerComponent)).Contains(m_ManualGear),"manual stored-bag deletion preserves exact unrelated vehicle equipment");
  SCR_EntityHelper.DeleteEntityAndChildren(m_DeleteVehicle);
  GetGame().GetCallqueue().CallLater(VerifyManualReasonStable,500,false);
 }
 protected void VerifyManualReasonStable()
 {
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_ManualToken);
  Check(record && record.Disposition==BCMM_RemainsDisposition.ADMIN_RELEASED,"later parent deletion cannot rewrite completed manual bag disposition");
  Finish();
 }
}
