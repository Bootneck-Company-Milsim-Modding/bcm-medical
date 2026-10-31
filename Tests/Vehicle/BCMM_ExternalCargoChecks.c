// Authority-only native transfer outside the Medical coordinator.
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_ExternalTestKill()
 {
  m_BCMMDeathPermitted=true;
  Kill(Instigator.CreateInstigator(GetOwner()));
 }
}
modded class BCMM_VehicleLifecycleChecks
{
 protected IEntity m_ExternalVehicle;
 protected IEntity m_ExternalCorpse;
 protected int m_ExternalToken;
 override protected void Loaded()
 {
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(InventoryStorageManagerComponent));
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(inventory.Contains(m_Bag) && inventory.Contains(m_Gear) && state.State() == BCMM_BagCargoState.STORED, "external-transfer fixture commits original native cargo and equipment");
  m_ExternalVehicle = Spawn("{2D74C39A650A3030}Prefabs/Vehicles/Wheeled/M923A1/M923A1_engineer.et", m_Vehicle.GetOrigin() + Vector(12, 2, 0));
  Check(m_ExternalVehicle != null, "second ordinary native vehicle spawns");
  m_ExternalCorpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"0 0 15");
  Check(m_ExternalCorpse!=null,"external transfer fixture spawns actual native death source");
  if(!m_ExternalCorpse){Finish();return;}
  SCR_CharacterDamageManagerComponent.Cast(m_ExternalCorpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_ExternalTestKill();
  GetGame().GetCallqueue().CallLater(MoveOutsideCoordinator, 3000, false);
 }
 protected void MoveOutsideCoordinator()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_ExternalToken=mode.BCMM_RegisterRemains(m_ExternalCorpse);
  Check(m_ExternalToken>0 && mode.BCMM_TransferRemains(m_ExternalCorpse,m_Bag),"exact original native death token binds to first vehicle's stored bag");
  if(m_Failures){Finish();return;}
  SCR_EntityHelper.DeleteEntityAndChildren(m_ExternalCorpse);
  SCR_VehicleInventoryStorageManagerComponent source = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  SCR_VehicleInventoryStorageManagerComponent destination = SCR_VehicleInventoryStorageManagerComponent.Cast(m_ExternalVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BaseInventoryStorageComponent storage = BCMM_BagInventoryPolicy.FindVehicleStorage(destination, m_Bag.GetPrefabData().GetPrefabName());
  Check(storage && source.TryMoveItemToStorage(m_Bag, storage), "privileged native API transfers exact stored bag A to B outside coordinator");
  GetGame().GetCallqueue().CallLater(CheckExternalAssociation, 1500, false);
 }
 protected void CheckExternalAssociation()
 {
  SCR_VehicleInventoryStorageManagerComponent source = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  SCR_VehicleInventoryStorageManagerComponent destination = SCR_VehicleInventoryStorageManagerComponent.Cast(m_ExternalVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(destination.Contains(m_Bag) && !source.Contains(m_Bag), "native storage really moved bag to second vehicle");
  Check(state.State() == BCMM_BagCargoState.STORED && state.VehicleId() == BCMM_BodyBagComponent.Id(m_ExternalVehicle), "Medical reconciles exact new native vehicle after external move");
  Check(m_Gear && source.Contains(m_Gear) && !destination.Contains(m_Gear), "previously unloaded equipment stays in original vehicle under Medical contract");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_ExternalToken);
  Check(record && record.Physical==m_Bag && record.StoredVehicle==m_ExternalVehicle,"original death record follows actual new native vehicle after external transfer");
  if (m_Failures) { Finish(); return; }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
  m_Actor.SetOrigin(m_ExternalVehicle.GetOrigin() + Vector(-3, 0.5, 0));
  GetGame().GetCallqueue().CallLater(RecoverExternalBag, 2000, false);
 }
 protected void RecoverExternalBag()
 {
  Check(m_Bag && !m_Bag.IsDeleted(), "deleting former vehicle does not delete externally transferred bag");
  Check(!m_Gear || m_Gear.IsDeleted(), "equipment intentionally retained in deleted former vehicle follows native vehicle lifetime");
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  Check(state.VehicleId() == BCMM_BodyBagComponent.Id(m_ExternalVehicle), "new vehicle association survives former vehicle deletion");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_ExternalToken);
  Check(record && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"former vehicle deletion cannot release externally transferred original death token");
  Check(BCMM_CargoOperation.Begin(m_Actor, m_ExternalVehicle, m_Bag, false, state.Revision()), "ordinary Medical unloading works from externally selected new vehicle");
  GetGame().GetCallqueue().CallLater(CheckExternalRecovery, 1500, false);
 }
 protected void CheckExternalRecovery()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
  Check(state.State() == BCMM_BagCargoState.WORLD && state.VehicleId() == RplId.Invalid() && !item.GetParentSlot(), "externally transferred original bag returns to an unlocked native world state");
  Check(state.Ready() && state.DeceasedName() == "Medical lifecycle casualty", "external transfer and recovery preserve exact bag readiness and deceased name");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_ExternalToken);
  Check(record && record.Physical==m_Bag && !record.StoredVehicle && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"unloading clears vehicle association while preserving original pending death record");
  Finish();
 }
}
