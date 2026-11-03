class BCMM_Beta01Packing : BCMM_DragRuntimeChecks
{
 protected IEntity m_Pack;
 protected InventoryStorageManagerComponent m_Inventory;
 protected BaseInventoryStorageComponent m_Storage;
 protected ref array<IEntity> m_Packed = {};
 protected int m_Next;
 protected float m_BandageVolume;
 override void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", "100 2 0");
  m_Pack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", "100 2 0");
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Storage = BaseInventoryStorageComponent.Cast(m_Pack.FindComponent(BaseInventoryStorageComponent));
  m_Suite.Record(m_Storage && m_Inventory.TryInsertItem(m_Pack, EStoragePurpose.PURPOSE_ANY), "Packing fixture equips native ALICE backpack");
  GetGame().GetCallqueue().CallLater(AddItem, 500);
 }
 protected void AddItem()
 {
  array<ResourceName> resources = {
   "{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et",
   "{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et",
   "{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et",
   "{6AB4B3F3FD216010}Prefabs/Items/BCMM_SutureKit.et",
   "{6AB4B3F3FD216007}Prefabs/Items/BCMM_DragRope.et",
   "{1AC68034B6B789EF}Prefabs/Items/BCMM_WeaponSling.et"
  };
  if (m_Next >= resources.Count()) { VerifyContents(); return; }
  IEntity item = Spawn(resources[m_Next], m_Actor.GetOrigin());
  InventoryItemComponent inventoryItem = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
  float volume = inventoryItem.GetTotalVolume();
  if (m_Next == 0) m_BandageVolume = volume;
  float ratio = volume / m_BandageVolume;
  PrintFormat("BCMM_PACKING resource=%1 nativeVolume=%2 bandageRatio=%3", resources[m_Next], volume, ratio);
  if (m_Next == 1) m_Suite.Record(ratio >= 1 && ratio <= 1.5, "Splint uses little more space than one native bandage");
  if (m_Next == 2 || m_Next == 3) m_Suite.Record(ratio >= 2 && ratio <= 3, "Surgery/suture kit uses two to three native bandages of packing volume");
  if (m_Next >= 4) m_Suite.Record(ratio <= 1.5, "Rope and sling have compact native packing cost");
  m_Packed.Insert(item);
  m_Suite.Record(m_Inventory.TryInsertItemInStorage(item, m_Storage), "Native backpack accepts compact medical item");
  m_Next++; GetGame().GetCallqueue().CallLater(AddItem, 300);
 }
 protected void VerifyContents()
 {
  foreach (IEntity item : m_Packed)
  {
   InventoryItemComponent marker = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
   m_Suite.Record(m_Inventory.Contains(item) && marker.GetParentSlot() && marker.GetParentSlot().GetStorage() == m_Storage, "Exact original item physically occupies native backpack alongside all other compact supplies");
  }
  PrintFormat("BCMM_PACKING_COMPLETE exactItems=%1", m_Packed.Count());
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor); m_Suite.Complete();
 }
}
