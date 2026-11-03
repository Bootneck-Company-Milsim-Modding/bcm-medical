// Native inventory/character preview review. Gameplay artwork is not replaced
// by this fixture; it uses each production prefab and its actual native icon.
class BCMM_Beta01ItemReview
{
 protected static ref BCMM_Beta01ItemReview s_Review;
 protected IEntity m_Actor;
 protected SCR_InventoryStorageManagerComponent m_Inventory;
 protected int m_Next;
 protected IEntity m_Crate;
 protected BaseInventoryStorageComponent m_CrateStorage;
 protected ref array<IEntity> m_Items = {};
 protected ref array<ResourceName> m_Resources = {};
 protected InventoryStorageManagerComponent m_CrateInventory;
 protected int m_EquipItem;
 protected int m_EquipCycle;
 protected int m_EquipTarget;
 protected int m_EquipWait;
 protected int m_EquipFailures;
 protected int m_CompletedCycles;
 static bool Requested()
 {
  string enabled;
  return System.GetCLIParam("bcmmBeta01Items", enabled);
 }
 static void Start(IEntity actor)
 {
  s_Review = new BCMM_Beta01ItemReview();
  s_Review.m_Actor = actor;
  s_Review.m_Inventory = SCR_InventoryStorageManagerComponent.Cast(actor.FindComponent(SCR_InventoryStorageManagerComponent));
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = actor.GetOrigin() + "1 0 0";
  s_Review.m_Crate = GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA126B001}Prefabs/BCMM_Beta01ReviewCrate.et"), GetGame().GetWorld(), spawn);
  if (!s_Review.m_Crate) return;
  s_Review.m_CrateStorage = BaseInventoryStorageComponent.Cast(s_Review.m_Crate.FindComponent(BaseInventoryStorageComponent));
  s_Review.m_CrateInventory = InventoryStorageManagerComponent.Cast(s_Review.m_Crate.FindComponent(InventoryStorageManagerComponent));
  s_Review.m_Resources = BCMM_Beta01ItemResources.All();
  string equipFrom;
  if (System.GetCLIParam("bcmmBeta01EquipFrom", equipFrom))
  {
   int startIndex = -1;
   foreach (int index, ResourceName entry : s_Review.m_Resources)
    if (entry.EndsWith("/" + equipFrom + ".et")) startIndex = index;
   if (startIndex < 0) { Print("BCMM_BETA_EQUIP_FAILED invalid start resource"); return; }
   s_Review.m_EquipItem = startIndex;
  }
  GetGame().GetCallqueue().CallLater(s_Review.AddNext, 100);
 }
 protected void AddNext()
 {
  if (m_Next >= m_Resources.Count())
  {
   m_Inventory.SetLootStorage(m_Crate); m_Inventory.OpenInventory();
   PrintFormat("BCMM_BETA_ITEM_REVIEW_READY inserted=%1 total=%2", m_Items.Count(), m_Resources.Count());
   GetGame().GetCallqueue().CallLater(Capture, 2000);
   string repetitions;
   if (System.GetCLIParam("bcmmBeta01EquipCycles", repetitions))
   {
    m_EquipTarget = Math.ClampInt(repetitions.ToInt(), 1, 200);
    GetGame().GetCallqueue().CallLater(BeginEquipItem, 3000);
   }
   return;
  }
  ResourceName resource = m_Resources[m_Next++];
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = m_Actor.GetOrigin() + "0 1 0";
  IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(resource), GetGame().GetWorld(), spawn);
  bool inserted = item && m_CrateInventory.TryInsertItemInStorage(item, m_CrateStorage);
  if (inserted) m_Items.Insert(item);
  vector low, high; if (item) item.GetWorldBounds(low, high);
  PrintFormat("BCMM_BETA_ITEM resource=%1 inserted=%2 dimensions=%3", resource, inserted, high-low);
  GetGame().GetCallqueue().CallLater(AddNext, 100);
 }
 protected void Capture()
 {
  FileIO.MakeDirectory("$profile:screenshots");
  System.MakeScreenshot("$profile:screenshots/medical-inventory-overview");
  Print("BCMM_BETA_INVENTORY_SCREENSHOT_REQUESTED");
 }
 protected void BeginEquipItem()
 {
  if (m_EquipItem >= m_Items.Count())
  {
   PrintFormat("BCMM_BETA_EQUIP_COMPLETE completedCycles=%1 failures=%2", m_CompletedCycles, m_EquipFailures);
   return;
  }
  IEntity item = m_Items[m_EquipItem];
  if (!item || !item.FindComponent(SCR_GadgetComponent))
  {
   PrintFormat("BCMM_BETA_EQUIP_NOT_APPLICABLE item=%1 inventoryOnly=1", item);
   m_EquipItem++; m_EquipCycle = 0; GetGame().GetCallqueue().CallLater(BeginEquipItem, 100); return;
  }
  BaseInventoryStorageComponent storage = m_Inventory.FindStorageForItem(item, EStoragePurpose.PURPOSE_ANY);
  InventoryItemComponent marker = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
  bool accepted = false;
  if (storage && marker && marker.GetParentSlot()) accepted = m_Inventory.TryMoveItemToStorage(item, storage);
  else if (storage && marker) accepted = m_Inventory.TryInsertItemInStorage(item, storage);
  if (!accepted)
  {
   PrintFormat("BCMM_BETA_EQUIP_TRANSFER target=%1 actorContains=%2 crateContains=%3 slot=%4", storage, m_Inventory.Contains(item), m_CrateInventory.Contains(item), InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent)).GetParentSlot());
   Failed("item cannot enter actor inventory"); return;
  }
  GetGame().GetCallqueue().CallLater(Equip, 300);
 }
 protected void Equip()
 {
  m_EquipWait = 0;
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Items[m_EquipItem], EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(CheckEquipped, 100);
 }
 protected bool PreviewPresent()
 {
  SCR_InventoryMenuUI menu = SCR_InventoryMenuUI.GetInventoryMenu();
  return menu && menu.BCMM_BetaPreviewNode(m_Actor.GetAnimation().GetBoneIndex("Head"));
 }
 protected void CheckEquipped()
 {
  if (ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot() != m_Items[m_EquipItem])
  {
   if (m_EquipWait++ < 100) { GetGame().GetCallqueue().CallLater(CheckEquipped, 100); return; }
   Failed("native gadget never reached the actual hand slot"); return;
  }
  if (!PreviewPresent())
  {
   if (m_EquipWait++ < 30) { GetGame().GetCallqueue().CallLater(CheckEquipped, 100); return; }
   Failed("native central character preview lost its head node for three seconds"); return;
  }
  if (m_EquipCycle == 0 || m_EquipCycle == 99 || m_EquipCycle == 199)
   System.MakeScreenshot("$profile:screenshots/equip-" + m_EquipItem.ToString() + "-" + m_EquipCycle.ToString());
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).RemoveHeldGadget();
  m_EquipWait = 0;
  GetGame().GetCallqueue().CallLater(CheckStowed, 100);
 }
 protected void CheckStowed()
 {
  if (ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot())
  {
   if (m_EquipWait++ < 100) { GetGame().GetCallqueue().CallLater(CheckStowed, 100); return; }
   Failed("native gadget did not leave the hand slot"); return;
  }
  if (!m_Inventory.Contains(m_Items[m_EquipItem]) || !PreviewPresent())
  {
   if (m_EquipWait++ < 30) { GetGame().GetCallqueue().CallLater(CheckStowed, 100); return; }
   Failed("stowing lost the original supply or character preview for three seconds"); return;
  }
  m_EquipCycle++; m_CompletedCycles++;
  if (m_EquipCycle == 1 || m_EquipCycle % 20 == 0)
   PrintFormat("BCMM_BETA_EQUIP_CYCLES resource=%1 completed=%2 failures=%3", m_Resources[m_EquipItem], m_EquipCycle, m_EquipFailures);
  if (m_EquipCycle < m_EquipTarget) { GetGame().GetCallqueue().CallLater(Equip, 100); return; }
  InventoryItemComponent original = InventoryItemComponent.Cast(m_Items[m_EquipItem].FindComponent(InventoryItemComponent));
  if (original && original.GetParentSlot()) m_Inventory.TryRemoveItemFromStorage(m_Items[m_EquipItem], original.GetParentSlot().GetStorage());
  m_EquipItem++; m_EquipCycle = 0; GetGame().GetCallqueue().CallLater(BeginEquipItem, 500);
 }
 protected void Failed(string reason)
 {
  m_EquipFailures++;
  PrintFormat("BCMM_BETA_EQUIP_FAILED item=%1 cycle=%2 reason=%3", m_EquipItem, m_EquipCycle, reason);
  System.MakeScreenshot("$profile:screenshots/failed-" + m_EquipItem.ToString() + "-" + m_EquipCycle.ToString());
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).RemoveHeldGadget();
  m_EquipItem++; m_EquipCycle = 0; GetGame().GetCallqueue().CallLater(BeginEquipItem, 500);
 }
}
modded class SCR_InventoryMenuUI
{
 bool BCMM_BetaPreviewNode(TNodeId node)
 {
  if (!m_wPlayerRender || !m_wPlayerRender.IsVisible()) return false;
  vector offset[4]; Math3D.MatrixIdentity4(offset); vector projected;
  return m_wPlayerRender.TryGetItemNodePositionInWidgetSpace(node, offset, projected);
 }
}
