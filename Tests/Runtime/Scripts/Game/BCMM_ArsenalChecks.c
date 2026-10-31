// Native config reflection checks, using the exact base GUIDs resolved by the game.
class BCMM_ArsenalChecks
{
 static bool Inspect(ResourceName name,string faction,BCMM_DomainChecks suite=null)
 {
  Resource resource=Resource.Load(name);if(!resource || !resource.IsValid()) { if(suite) suite.Record(false,"Native faction medical catalogue loads");return false; }
  SCR_EntityCatalogMultiList catalog=SCR_EntityCatalogMultiList.Cast(BaseContainerTools.CreateInstanceFromContainer(resource.GetResource().ToBaseContainer()));
  if(!catalog) { if(suite) suite.Record(false,"Native faction catalogue reflects its multilist");return false; }
  array<SCR_EntityCatalogMultiListEntry> lists={};catalog.GetMultiList(lists);int medical;int other;bool valid=true;array<ResourceName> seen={};
  BCMM_Settings settings=new BCMM_Settings();
  foreach(SCR_EntityCatalogMultiListEntry list:lists)
  {
   foreach(SCR_EntityCatalogEntry entry:list.m_aEntities)
   {
    BCMM_ArsenalItem data=BCMM_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
    if(!data) { other++;continue; }
    medical++;ResourceName prefab=entry.GetPrefab();
    if(seen.Contains(prefab) || settings.IndexOf(data.CostKey)<0 || data.GetItemType()!=SCR_EArsenalItemType.HEAL) valid=false;
    seen.Insert(prefab);Resource item=Resource.Load(prefab);
    if(!item || !item.IsValid()) { valid=false;continue; }
    BaseContainerList components=item.GetResource().ToBaseContainer().GetObjectArray("components");bool inventory;
    for(int i=0;i<components.Count();i++) { if(components.Get(i).GetClassName()=="InventoryItemComponent") inventory=true; }
    if(!inventory || data.GetSupplyCost(SCR_EArsenalSupplyCostType.DEFAULT)<0 || data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT)!=0) valid=false;
   }
  }
  valid=valid && medical==29 && other>20;
  if(suite) suite.Record(valid,"Base "+faction+" catalogue resolves twenty-nine distinct inventory medical entries, GM costs and retained ordinary equipment");
  if(valid) PrintFormat("BCMM_ARSENAL_CATALOG_OK faction=%1 count=%2 ordinary=%3",faction,medical,other);
  return valid;
 }
 static void Run(BCMM_DomainChecks suite)
 {
  Inspect("{5F7EC52FC40A03E2}Configs/EntityCatalog/US/InventoryItems_EntityCatalog_US.conf","US",suite);
  Inspect("{C53421647C3D0D2E}Configs/EntityCatalog/USSR/InventoryItems_EntityCatalog_USSR.conf","USSR",suite);
  Inspect("{E908001749419691}Configs/EntityCatalog/FIA/InventoryItems_EntityCatalog_FIA.conf","FIA",suite);
  Resource ui=Resource.Load("{4CC149958FD15308}Configs/Arsenal/ArsenalItemTypeUIConfig.conf");
  SCR_ArsenalItemTypeUIConfig info=SCR_ArsenalItemTypeUIConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(ui.GetResource().ToBaseContainer()));SCR_UIInfo label;
  suite.Record(info && info.GetInfoForType(SCR_EArsenalItemType.HEAL,label) && label.GetName()=="BCM Medical","Native arsenal medical heading reads BCM Medical");
  BCMM_Settings settings=new BCMM_Settings();BCMM_SettingsDocument prior=new BCMM_SettingsDocument();prior.Schema=24;prior.Generation=1;
  for(int i=0;i<210;i++) { prior.Keys.Insert(settings.Definition(i).Key);prior.Values.Insert(settings.ValueAt(i)); }
  BCMM_Settings migrated=new BCMM_Settings();suite.Record(prior.Decode(migrated) && migrated.Count()==279 && migrated.Get("maskScale")==1.45 && migrated.Get("tabletAnimationEnabled")==1,"Schema twenty-four preserves settings and appends mask, tablets and arsenal defaults");
 }
}
