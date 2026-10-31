class BCMM_WeaponSlingRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Weapon;
 protected IEntity m_Sling;
 protected int m_Case;
 protected int m_WakeWait;
 protected int m_EquipWait;
 protected ref array<IEntity> m_OriginalParts={};
 override protected void Prepare()
 {
  if(!m_Actor || !m_Target){m_Suite.Record(false,"Sling fixture characters spawn");Finish();return;}
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Sling fixture equips native inventory");
  m_Sling=Spawn(BCMM_WeaponRetention.SLING,m_Actor.GetOrigin());
  m_Suite.Record(m_Sling && m_Sling.FindComponent(InventoryItemComponent) && !m_Sling.FindComponent(SCR_ConsumableItemComponent),"Imported sling spawns as a non-consumable inventory item");
  m_Suite.Record(Resource.Load("{CA7F21A8CF938319}Assets/Medical/WeaponSling/BCMM_WeaponSling.xob").IsValid(),"Approved sling native model loads");
  m_Weapon=Spawn("{F97A4AC994231900}Prefabs/Weapons/Rifles/M16/Rifle_M16A2_carbine.et",m_Actor.GetOrigin());
  if(!m_Weapon || !m_Sling){m_Suite.Record(false,"Sling fixture original items spawn");Finish();return;}
  GetGame().GetCallqueue().CallLater(EquipCase,500);
 }
 protected void EquipCase()
 {
  if(m_WakeWait==0)m_Medical.FullHeal();
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  if(controller.IsUnconscious() || controller.IsChangingItem())
  {
   if(m_WakeWait++<60){GetGame().GetCallqueue().CallLater(EquipCase,250);return;}
   m_Suite.Record(false,"Original player finishes native wake-up before the next retention case");Finish();return;
  }
  m_WakeWait=0;
  int policy=1;if(m_Case==2)policy=2;if(m_Case==3)policy=0;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("weaponRetentionPolicy",policy);
  bool needsSling=m_Case==1 || m_Case==3 || m_Case==4;
  if(needsSling && !m_Inventory.Contains(m_Sling))m_Suite.Record(m_Inventory.TryInsertItem(m_Sling,EStoragePurpose.PURPOSE_ANY),"Exact sling enters native inventory");
  if(!needsSling && m_Inventory.Contains(m_Sling))m_Suite.Record(SCR_InventoryStorageManagerComponent.Cast(m_Inventory).TryRemoveItemFromInventory(m_Sling),"Exact sling can be removed without equipping");
  if(!m_Inventory.Contains(m_Weapon))m_Suite.Record(m_Inventory.TryInsertItem(m_Weapon,EStoragePurpose.PURPOSE_ANY),"Same original rifle enters inventory for next case");
  BaseWeaponComponent weapon=BaseWeaponComponent.Cast(m_Weapon.FindComponent(BaseWeaponComponent));
  controller.TryEquipRightHandItem(m_Weapon,EEquipItemType.EEquipTypeWeapon,true);
  GetGame().GetCallqueue().CallLater(DepleteWithWeapon,2000);
 }
 protected void DepleteWithWeapon()
 {
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  BaseWeaponManagerComponent manager=controller.GetWeaponManagerComponent();
  if(!manager.GetCurrentWeapon() || manager.GetCurrentWeapon().GetOwner()!=m_Weapon)
  {
   if(m_EquipWait++<16)
   {
    controller.TryEquipRightHandItem(m_Weapon,EEquipItemType.EEquipTypeWeapon,true);
    GetGame().GetCallqueue().CallLater(DepleteWithWeapon,500);return;
   }
   m_Suite.Record(false,"Native fixture can hold the original rifle after wake-up");Finish();return;
  }
  m_EquipWait=0;
  m_Suite.Record(manager.GetCurrentWeapon() && manager.GetCurrentWeapon().GetOwner()==m_Weapon,"Exact original rifle is held before retention case "+m_Case);
  PrintFormat("BCMM_SLING_CASE case=%1 player=%2 held=%3 sling=%4",m_Case,GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Actor),manager.GetCurrentWeapon(),m_Inventory.Contains(m_Sling));
  if(m_Case==4)controller.DropWeapon(manager.GetCurrentSlot());
  else {m_Medical.GetBloodHitZone().SetHealth(0);m_Medical.UpdateConsciousness();}
  GetGame().GetCallqueue().CallLater(InspectCase,1500);
 }
 protected void InspectCase()
 {
  bool retained=m_Case==1 || m_Case==2;
  m_Suite.Record(m_Weapon && m_Inventory.Contains(m_Weapon)==retained,"Original rifle retention matches policy/sling case "+m_Case);
  if(m_Case!=4)m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUnconscious(),"Retention never prevents native unconsciousness");
  if(m_Case==1)m_Suite.Record(m_Sling && m_Inventory.Contains(m_Sling),"Retention does not consume or equip the sling");
  m_Case++;
  if(m_Case<5)
  {
   if(m_Weapon && !m_Inventory.Contains(m_Weapon))SCR_EntityHelper.DeleteEntityAndChildren(m_Weapon);
   if(m_Sling && !m_Inventory.Contains(m_Sling))SCR_EntityHelper.DeleteEntityAndChildren(m_Sling);
   SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);SCR_EntityHelper.DeleteEntityAndChildren(m_Target);SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
   m_WakeWait=0;m_EquipWait=0;Start(m_Suite);return;
  }
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("weaponRetentionPolicy",1);
  if(m_Weapon && !m_Inventory.Contains(m_Weapon))SCR_EntityHelper.DeleteEntityAndChildren(m_Weapon);
  if(m_Sling && !m_Inventory.Contains(m_Sling))SCR_EntityHelper.DeleteEntityAndChildren(m_Sling);
  Finish();
 }
}
