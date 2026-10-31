// Real native standard faction boxes and their cached storage membership.
class BCMM_ArsenalRuntimeChecks:BCMM_SplintRuntimeChecks
{
 protected ref array<IEntity> m_Boxes={};
 override protected void Prepare()
 {
  FactionManager manager=GetGame().GetFactionManager();
  m_Suite.Record(manager && manager.GetFactionByKey("US") && manager.GetFactionByKey("USSR") && manager.GetFactionByKey("FIA"),"Native arsenal framework includes US, USSR and FIA from world initialization");
  GetGame().GetCallqueue().CallLater(SpawnBoxes,1500);
 }
 protected void SpawnBoxes()
 {
  m_Boxes.Insert(Spawn("{45C675ED3CFBBEC5}Prefabs/Props/Military/Compositions/US/ArsenalBox_US.et","105 2 0"));
  m_Boxes.Insert(Spawn("{AD59EDEAA26CE792}Prefabs/Props/Military/Compositions/USSR/ArsenalBox_USSR.et","110 2 0"));
  m_Boxes.Insert(Spawn("{B53B98CEA2D72735}Prefabs/Props/Military/Compositions/FIA/ArsenalBox_FIA.et","115 2 0"));
  GetGame().GetCallqueue().CallLater(InspectBoxes,3000);
 }
 protected void InspectBoxes()
 {
  array<string> factions={"US","USSR","FIA"};SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  for(int i=0;i<m_Boxes.Count();i++)
  {
   IEntity box=m_Boxes[i];SCR_ArsenalComponent arsenal;if(box) arsenal=SCR_ArsenalComponent.FindArsenalComponent(box);
   m_Suite.Record(arsenal && arsenal.GetAssignedFaction() && arsenal.GetAssignedFaction().GetFactionKey()==factions[i],"Native "+factions[i]+" box resolves the actual faction");if(!arsenal) continue;
   array<SCR_ArsenalItem> entries={};arsenal.GetFilteredArsenalItems(entries);int medical;foreach(SCR_ArsenalItem entry:entries) { if(BCMM_ArsenalItem.Cast(entry)) medical++; }
   m_Suite.Record(BCMM_ArsenalPurchasePolicy.Allows(arsenal,"{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"),"Authoritative native purchase policy permits the available medical item");
   m_Suite.Record(medical==29,"Native faction box actually offers the complete medical catalogue");
   SCR_ArsenalInventoryStorageManagerComponent inventory=SCR_ArsenalInventoryStorageManagerComponent.Cast(box.FindComponent(SCR_ArsenalInventoryStorageManagerComponent));
   m_Suite.Record(inventory && inventory.IsPrefabInArsenalStorage("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"),"Native arsenal cache contains the oxygen item");
   mode.BCMM_TestRoleSetting("arsenal"+factions[i]+"Enabled",0);entries.Clear();arsenal.GetFilteredArsenalItems(entries);medical=0;foreach(SCR_ArsenalItem filtered:entries) { if(BCMM_ArsenalItem.Cast(filtered)) medical++; }
   m_Suite.Record(!BCMM_ArsenalPurchasePolicy.Allows(arsenal,"{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"),"Authoritative native purchase policy rejects a hidden item even if the faction catalogue still contains it");
   m_Suite.Record(medical==0 && inventory && !inventory.IsPrefabInArsenalStorage("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"),"Live GM disable refreshes the actual arsenal list and native membership cache");
   mode.BCMM_TestRoleSetting("arsenal"+factions[i]+"Enabled",1);
   m_Suite.Record(inventory && inventory.IsPrefabInArsenalStorage("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"),"Live GM re-enable restores the native medical membership cache");
  }
  InspectFiniteRefunds();
 }
 protected bool m_TransactionNetwork;
 protected SCR_PlayerController m_TransactionPlayer;
 protected IEntity m_TransactionBox;
 protected IEntity m_TransactionPack;
 protected SCR_ResourceComponent m_TransactionResources;
 protected void BeginTransactions()
 {
  string network;
  m_TransactionNetwork=System.GetCLIParam("bcmmArsenalNetwork",network);
  if(m_TransactionNetwork)
  {
   m_TransactionPlayer=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
   if(!m_TransactionPlayer || !m_TransactionPlayer.GetControlledEntity())
   {
    if(m_OwnerWait++<100){GetGame().GetCallqueue().CallLater(BeginTransactions,250);return;}
    m_Suite.Record(false,"Arsenal network fixture receives actual owning player");Finish();return;
   }
   SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=m_TransactionPlayer.GetControlledEntity();
  }
  else
  {
   m_TransactionPlayer=SCR_PlayerController.Cast(GetGame().GetPlayerController());
   m_TransactionPlayer.SetControlledEntity(m_Actor);
  }
  vector matrix[4];m_Actor.GetWorldTransform(matrix);matrix[3]="100 1.2 0";ChimeraCharacter.Cast(m_Actor).Teleport(matrix);
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_TransactionPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(m_TransactionPack && m_Inventory.TryInsertItem(m_TransactionPack,EStoragePurpose.PURPOSE_ANY),"Native arsenal transaction equips the real buyer's backpack");
  m_TransactionBox=Spawn("{B6C4ED1CA1262410}Prefabs/BCMM_ArsenalTransaction.et","101 1.2 0");
  if(m_TransactionBox)m_TransactionResources=SCR_ResourceComponent.Cast(m_TransactionBox.FindComponent(SCR_ResourceComponent));
  GetGame().GetCallqueue().CallLater(InspectTransactionResources,2000);
 }
 protected float m_TransactionExpected;
 protected float m_TransactionOldCost;
 protected float m_TransactionOldRefund;
 protected string m_TransactionCostKey;
 protected IEntity m_PurchasedSupply;
 protected RplId m_RefundedItemId;
 protected void InspectTransactionResources()
 {
  SCR_ResourceContainer container;
  if(m_TransactionResources)container=m_TransactionResources.GetContainer(EResourceType.SUPPLIES);
  m_Suite.Record(container && container.GetResourceValue()==1000,"Actual diagnostic arsenal initializes a finite native supply container");
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(m_TransactionPlayer.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  m_Suite.Record(transactions && m_TransactionResources.GetConsumer(EResourceGeneratorID.DEFAULT,EResourceType.SUPPLIES) && m_TransactionResources.GetGenerator(EResourceGeneratorID.DEFAULT,EResourceType.SUPPLIES),"Real player controller and native arsenal expose transaction endpoints");
  if(!container || !transactions){Finish();return;}
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  SCR_ArsenalComponent arsenal=SCR_ArsenalComponent.FindArsenalComponent(m_TransactionBox);
  SCR_EntityCatalogEntry entry=SCR_EntityCatalogManagerComponent.GetInstance().GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM,"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",arsenal.GetAssignedFaction());
  BCMM_ArsenalItem data=BCMM_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
  m_TransactionCostKey=data.CostKey;
  m_TransactionOldCost=mode.BCMM_ReadSetting(m_TransactionCostKey);m_TransactionOldRefund=mode.BCMM_ReadSetting("arsenalRefundFraction");
  mode.BCMM_TestRoleSetting(m_TransactionCostKey,120);mode.BCMM_TestRoleSetting("arsenalRefundFraction",0.5);
  m_TransactionExpected=container.GetResourceValue()-120*m_TransactionResources.GetConsumer(EResourceGeneratorID.DEFAULT,EResourceType.SUPPLIES).GetBuyMultiplier();
  mode.BCMM_TestRoleSetting("arsenalUSEnabled",0);
  SendTransaction(1);
  GetGame().GetCallqueue().CallLater(CheckHiddenPurchaseDenied,5000);
 }
 protected void SendTransaction(int phase)
 {
  if(m_TransactionNetwork)
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishArsenalTransaction(phase,m_TransactionBox,m_TransactionPack,m_RefundedItemId);
   return;
  }
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(m_TransactionPlayer.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  if(phase<=2)transactions.RpcAsk_ArsenalRequestItem(Replication.FindItemId(m_TransactionResources),Replication.FindItemId(m_TransactionPack.FindComponent(BaseInventoryStorageComponent)),"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",EResourceType.SUPPLIES);
  else transactions.RpcAsk_ArsenalRefundItem(Replication.FindItemId(m_TransactionResources),m_RefundedItemId,EResourceType.SUPPLIES);
 }
 protected void CheckHiddenPurchaseDenied()
 {
  array<IEntity> contents={};BaseInventoryStorageComponent.Cast(m_TransactionPack.FindComponent(BaseInventoryStorageComponent)).GetAll(contents);
  m_Suite.Record(contents.IsEmpty() && m_TransactionResources.GetContainer(EResourceType.SUPPLIES).GetResourceValue()==1000,"Direct native purchase RPC for disabled Medical supply creates nothing and spends no resources");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("arsenalUSEnabled",1);
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(m_TransactionPlayer.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  SendTransaction(2);
  GetGame().GetCallqueue().CallLater(CheckNativePurchase,5000);
 }
 protected void CheckNativePurchase()
 {
  SCR_ResourceContainer container=m_TransactionResources.GetContainer(EResourceType.SUPPLIES);
  BaseInventoryStorageComponent storage=BaseInventoryStorageComponent.Cast(m_TransactionPack.FindComponent(BaseInventoryStorageComponent));
  array<IEntity> items={};storage.GetAll(items);
  foreach(IEntity item:items)
  {
   BCMM_EquipmentItemComponent supply=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
   if(supply && supply.IsFluidBag())m_PurchasedSupply=item;
  }
  PrintFormat("BCMM_ARSENAL_BUY actual=%1 expected=%2 purchased=%3",container.GetResourceValue(),m_TransactionExpected,m_PurchasedSupply);
  m_Suite.Record(m_PurchasedSupply && Math.AbsFloat(container.GetResourceValue()-m_TransactionExpected)<0.01,"Actual native purchase deducts exact supplies and creates the finite saline bag in buyer storage");
  if(!m_PurchasedSupply){Finish();return;}
  BCMM_EquipmentItemComponent marker=BCMM_EquipmentItemComponent.Cast(m_PurchasedSupply.FindComponent(BCMM_EquipmentItemComponent));marker.ConsumeFluidMl(125);
  float quote=SCR_ArsenalManagerComponent.GetItemRefundAmount(m_PurchasedSupply,SCR_ArsenalComponent.FindArsenalComponent(m_TransactionBox),false);
  m_Suite.Record(quote>0 && quote<=45,"Native depleted-saline refund quote retains the configured partial value");
  m_TransactionExpected=container.GetResourceValue()+quote;
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(m_TransactionPlayer.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  m_RefundedItemId=Replication.FindItemId(m_PurchasedSupply.FindComponent(InventoryItemComponent));
  SendTransaction(3);
  GetGame().GetCallqueue().CallLater(CheckNativeRefund,5000);
 }
 protected void CheckNativeRefund()
 {
  SCR_ResourceContainer container=m_TransactionResources.GetContainer(EResourceType.SUPPLIES);
  PrintFormat("BCMM_ARSENAL_REFUND actual=%1 expected=%2 item=%3",container.GetResourceValue(),m_TransactionExpected,m_PurchasedSupply);
  m_Suite.Record(!m_PurchasedSupply && Math.AbsFloat(container.GetResourceValue()-m_TransactionExpected)<0.01,"Actual native refund credits only partial saline value and removes the exact returned item");
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(m_TransactionPlayer.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  SendTransaction(4);
  GetGame().GetCallqueue().CallLater(CheckStaleRefund,8000);
 }
 protected void CheckStaleRefund()
 {
  m_Suite.Record(Math.AbsFloat(m_TransactionResources.GetContainer(EResourceType.SUPPLIES).GetResourceValue()-m_TransactionExpected)<0.01,"Repeated stale native refund RPC cannot credit the deleted supply a second time");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_TestRoleSetting(m_TransactionCostKey,m_TransactionOldCost);mode.BCMM_TestRoleSetting("arsenalRefundFraction",m_TransactionOldRefund);
  Finish();
 }
 protected void InspectFiniteRefunds()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  SCR_EntityCatalogManagerComponent catalogs=SCR_EntityCatalogManagerComponent.GetInstance();
  SCR_Faction faction=SCR_Faction.Cast(GetGame().GetFactionManager().GetFactionByKey("US"));
  array<ResourceName> prefabs={"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et","{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et","{6AB4B3F3FD216025}Prefabs/Items/BCMM_AED.et"};
  float savedRefund=mode.BCMM_ReadSetting("arsenalRefundFraction");
  m_Suite.Record(mode.BCMM_TestRoleSetting("arsenalRefundFraction",0.5),"Live Medical refund policy enables a one-half return fraction");
  SCR_ArsenalManagerComponent manager;
  bool hasManager=SCR_ArsenalManagerComponent.GetArsenalManager(manager);
  m_Suite.Record(hasManager && manager,"Native arsenal manager exists for personal-allocation calculation");
  bool allocation;if(manager){allocation=SCR_ArsenalManagerComponent.IsMilitarySupplyAllocationEnabled();manager.SetMilitarySupplyAllocationEnabled(true);}
  SCR_ArsenalComponent arsenal=SCR_ArsenalComponent.FindArsenalComponent(m_Boxes[0]);
  for(int index=0;index<prefabs.Count();index++)
  {
   IEntity item=Spawn(prefabs[index],"100 3 3");
   SCR_EntityCatalogEntry entry=catalogs.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM,prefabs[index],faction);
   BCMM_ArsenalItem data;if(entry)data=BCMM_ArsenalItem.Cast(entry.GetEntityDataOfType(SCR_ArsenalItem));
   BCMM_EquipmentItemComponent marker;if(item)marker=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
   m_Suite.Record(item && data && marker,"Actual finite supply resolves its native Medical catalogue price data");
   if(!item || !data || !marker)continue;
   float savedCost=mode.BCMM_ReadSetting(data.CostKey);mode.BCMM_TestRoleSetting(data.CostKey,120);
   m_Suite.Record(data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT,true,item)==60,"Fresh finite supply receives the configured full-item refund fraction");
   int expected;
   if(index==0){marker.ConsumeFluidMl(125);expected=45;}
   if(index==1){marker.ConsumeOxygen(1);expected=59;}
   if(index==2){int charges=marker.ShocksRemaining();m_Suite.Record(charges>0 && marker.Discharge(),"Native AED refund fixture spends exactly one existing charge");if(charges>0)expected=Math.Floor(60*(charges-1)/(float)charges);}
   m_Suite.Record(data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT,true,item)==expected,"Partial native fluid, oxygen and AED supplies refund only their remaining value");
   PrintFormat("BCMM_ARSENAL_FINITE type=%1 refund=%2 expected=%3",index,data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT,true,item),expected);
   if(index==1 || index==2)
   {
    string capacityKey="oxygenCapacityLitres";if(index==2)capacityKey="aedShocksPerDevice";
    float previousCapacity=mode.BCMM_ReadSetting(capacityKey);
    m_Suite.Record(mode.BCMM_TestRoleSetting(capacityKey,previousCapacity*2) && data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT,true,item)==expected,"Live capacity changes cannot refill or revalue an existing depleted oxygen/AED device");
    mode.BCMM_TestRoleSetting(capacityKey,previousCapacity);
   }
   if(manager)m_Suite.Record(manager.BCMM_TestMedicalRefundCalculator(item,arsenal)==expected,"Separate native personal-allocation calculator applies Medical depletion and refund policy");
   mode.BCMM_TestRoleSetting("arsenalRefundFraction",0);
   m_Suite.Record(data.GetSupplyRefundAmount(SCR_EArsenalSupplyCostType.DEFAULT,true,item)==0,"Zero refund policy also excludes partial finite supplies");
   if(manager)m_Suite.Record(manager.BCMM_TestMedicalRefundCalculator(item,arsenal)==0,"Zero Medical refund policy cannot be bypassed through personal allocation");
   mode.BCMM_TestRoleSetting("arsenalRefundFraction",0.5);mode.BCMM_TestRoleSetting(data.CostKey,savedCost);
   SCR_EntityHelper.DeleteEntityAndChildren(item);
  }
  if(manager)manager.SetMilitarySupplyAllocationEnabled(allocation);
  mode.BCMM_TestRoleSetting("arsenalRefundFraction",savedRefund);
  BeginTransactions();
 }
}

modded class SCR_EntityCatalogManagerComponent
{
 void BCMM_TestArsenalFactionManager(SCR_FactionManager manager) { m_FactionManager=manager; }
}

modded class SCR_ArsenalManagerComponent
{
 int BCMM_TestMedicalRefundCalculator(IEntity item,SCR_ArsenalComponent arsenal)
 {
  set<IEntity> counted=new set<IEntity>();
  return GetItemMilitarySupplyAllocationRefundCost(item,counted,SCR_EntityCatalogManagerComponent.GetInstance(),arsenal,arsenal.GetAssignedFaction());
 }
}
