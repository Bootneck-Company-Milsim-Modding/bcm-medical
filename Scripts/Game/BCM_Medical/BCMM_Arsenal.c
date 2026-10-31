// Original BCM medical catalog data. Native inventory transactions own purchases.
[BaseContainerProps()]
class BCMM_ArsenalItem : SCR_ArsenalItem
{
 [Attribute()] string CostKey;
 override int GetSupplyCost(SCR_EArsenalSupplyCostType supplyCostType,bool addAdditionalCosts=true) { return Math.Round(BCMM_TreatmentAccess.Setting(CostKey)); }
 override int GetMilitarySupplyAllocationCost(SCR_EArsenalSupplyCostType supplyCostType,bool addAdditionalCosts=true) { return GetSupplyCost(supplyCostType,addAdditionalCosts); }
 static float RemainingValue(IEntity item)
 {
  if(!item)return 1;
  BCMM_EquipmentItemComponent marker=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
  if(marker)return marker.RemainingValueFraction();
  return 1;
 }
 override int GetSupplyRefundAmount(SCR_EArsenalSupplyCostType supplyCostType,bool addAdditionalCosts=true,IEntity item=null)
 {
  if(item && BCMM_SplintTreatment.IsReservedSupply(item))return 0;
  return Math.Floor(GetSupplyCost(supplyCostType,addAdditionalCosts)*BCMM_TreatmentAccess.Setting("arsenalRefundFraction")*RemainingValue(item));
 }
}
modded class SCR_ArsenalComponent
{
 protected static ref array<SCR_ArsenalComponent> s_BCMMMedicalArsenals={};
 override void OnPostInit(IEntity owner) { super.OnPostInit(owner);s_BCMMMedicalArsenals.Insert(this); }
 override protected void OnDelete(IEntity owner) { s_BCMMMedicalArsenals.RemoveItem(this);super.OnDelete(owner); }
 static void BCMM_RefreshMedicalAvailability()
 {
  foreach(SCR_ArsenalComponent arsenal:s_BCMMMedicalArsenals)
  {
   if(!arsenal || !arsenal.GetOwner()) continue;
   SCR_Faction faction=arsenal.GetAssignedFaction();if(!faction) continue;
   string key=faction.GetFactionKey();if(key=="US" || key=="USSR" || key=="FIA") arsenal.RefreshArsenal();
  }
 }

 override bool GetFilteredArsenalItems(out notnull array<SCR_ArsenalItem> filteredArsenalItems,EArsenalItemDisplayType requiresDisplayType=-1)
 {
  super.GetFilteredArsenalItems(filteredArsenalItems,requiresDisplayType);
  SCR_Faction faction=GetAssignedFaction();if(!faction) { return !filteredArsenalItems.IsEmpty(); }
  string key=faction.GetFactionKey();if(key!="US" && key!="USSR" && key!="FIA") { return !filteredArsenalItems.IsEmpty(); }
  if(BCMM_TreatmentAccess.Setting("arsenal"+key+"Enabled")>0.5) { return !filteredArsenalItems.IsEmpty(); }
  array<SCR_ArsenalItem> visible={};
  foreach(SCR_ArsenalItem entry:filteredArsenalItems) { if(!BCMM_ArsenalItem.Cast(entry)) { visible.Insert(entry); } }
  filteredArsenalItems=visible;return !filteredArsenalItems.IsEmpty();
 }
}

// Native purchase validation must also enforce the current medical filter;
// merely hiding entries is insufficient for a direct client RPC request.
class BCMM_ArsenalPurchasePolicy
{
 static bool Allows(SCR_ArsenalComponent arsenal,ResourceName prefab)
 {
  if(!arsenal) { return false; }
  SCR_EntityCatalogManagerComponent catalogs=SCR_EntityCatalogManagerComponent.GetInstance();SCR_Faction faction=arsenal.GetAssignedFaction();
  if(!catalogs || !faction) { return true; }
  SCR_EntityCatalogEntry entry=catalogs.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM,prefab,faction);
  if(!entry || !BCMM_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem))) { return true; }
  if(!arsenal.IsArsenalEnabled()) { return false; }
  array<SCR_ArsenalItem> available={};arsenal.GetFilteredArsenalItems(available);
  foreach(SCR_ArsenalItem item:available) { if(item.GetItemResourceName()==prefab) { return true; } }
  return false;
 }
}
modded class SCR_ResourcePlayerControllerInventoryComponent
{
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 override protected void RpcAsk_ArsenalRequestItem_(RplId resourceComponentRplId,RplId storageComponentRplId,ResourceName resourceNameItem,EResourceType resourceType)
 {
  SCR_ResourceComponent source=SCR_ResourceComponent.Cast(Replication.FindItem(resourceComponentRplId));if(!source || !source.GetOwner()) { return; }
  SCR_ArsenalComponent arsenal=SCR_ArsenalComponent.FindArsenalComponent(source.GetOwner());
  if(!BCMM_ArsenalPurchasePolicy.Allows(arsenal,resourceNameItem)) { Print("BCMM_ARSENAL_REQUEST_DENIED: medical item unavailable under current faction/type settings",LogLevel.WARNING);return; }
  super.RpcAsk_ArsenalRequestItem_(resourceComponentRplId,storageComponentRplId,resourceNameItem,resourceType);
 }
}

// Personal allocation refunds have a separate native recursive path. Retain
// its ordinary-item/nested-storage accounting and adjust only Medical's own part.
modded class SCR_ArsenalManagerComponent
{
 override protected int GetItemMilitarySupplyAllocationRefundCost(notnull IEntity item,inout set<IEntity> countedItems,SCR_EntityCatalogManagerComponent entityCatalogManager,SCR_ArsenalComponent arsenalComponent,SCR_Faction faction)
 {
  if(countedItems.Contains(item))return 0;
  EntityPrefabData prefab=item.GetPrefabData();
  SCR_EntityCatalogEntry entry;if(prefab)entry=entityCatalogManager.GetEntryWithPrefabFromAnyCatalog(EEntityCatalogType.ITEM,prefab.GetPrefabName(),faction);
  BCMM_ArsenalItem medical;if(entry)medical=BCMM_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
  int total=super.GetItemMilitarySupplyAllocationRefundCost(item,countedItems,entityCatalogManager,arsenalComponent,faction);
  if(!medical || !prefab)return total;
  float multiplier=m_fRefundDifferentFactionItemMultiplier;
  if(entry.GetCatalogParent()==entityCatalogManager.GetFactionEntityCatalogOfType(EEntityCatalogType.ITEM,faction.GetFactionKey()))multiplier=m_fRefundSameFactionItemMultiplier;
  int own=SCR_ResourceSystemHelper.RoundRefundSupplyAmount(GetItemMilitarySupplyAllocationCost(prefab.GetPrefabName(),arsenalComponent)*multiplier);
  float available=BCMM_ArsenalItem.RemainingValue(item);
  if(BCMM_SplintTreatment.IsReservedSupply(item))available=0;
  return Math.Max(0,total-own+Math.Floor(own*BCMM_TreatmentAccess.Setting("arsenalRefundFraction")*available));
 }
}
modded class SCR_ResourcePlayerControllerInventoryComponent
{
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 override protected void RpcAsk_ArsenalRefundItem_(RplId resourceComponentRplId,RplId inventoryItemRplId,EResourceType resourceType)
 {
  InventoryItemComponent inventoryItem=InventoryItemComponent.Cast(Replication.FindItem(inventoryItemRplId));
  if(inventoryItem && BCMM_SplintTreatment.ContainsReservedSupply(inventoryItem.GetOwner()))
  {
   Print("BCMM_ARSENAL_REFUND_DENIED: supply reserved by an active medical procedure",LogLevel.WARNING);
   return;
  }
  super.RpcAsk_ArsenalRefundItem_(resourceComponentRplId,inventoryItemRplId,resourceType);
 }
}