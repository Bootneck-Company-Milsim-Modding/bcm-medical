// Original BCM implementation. Retains the same native weapon entity; never clones it.
class BCMM_SlingWeaponClaim
{
 IEntity Actor;
 IEntity Weapon;
 IEntity Sling;
 float Expires;
 bool DropObserved;
}
class BCMM_WeaponRetention
{
 static const ResourceName SLING = "{1AC68034B6B789EF}Prefabs/Items/BCMM_WeaponSling.et";
 protected static ref map<IEntity,ref BCMM_SlingWeaponClaim> s_Claims = new map<IEntity,ref BCMM_SlingWeaponClaim>();
 static void Reset(){s_Claims.Clear();}
 static bool Supported(EWeaponType type)
 {
  return type==EWeaponType.WT_RIFLE || type==EWeaponType.WT_SNIPERRIFLE || type==EWeaponType.WT_MACHINEGUN;
 }
 static IEntity FindSling(InventoryStorageManagerComponent inventory)
 {
  if(!inventory)return null;
  array<IEntity> items={};inventory.GetItems(items,EStoragePurpose.PURPOSE_ANY);
  foreach(IEntity item:items)
  {
   if(item && item.GetPrefabData() && item.GetPrefabData().GetPrefabName()==SLING)return item;
  }
  return null;
 }
 static void BeforeUnconscious(IEntity actor)
 {
  if(!Replication.IsServer() || !actor)return;
  // Autonomous NPC weapon policy belongs to the separate AI mod.
  if(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor)<=0)return;
  int policy=Math.Round(BCMM_TreatmentAccess.Setting("weaponRetentionPolicy"));
  if(policy<=0)return;
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);if(!character)return;
  CharacterControllerComponent controller=character.GetCharacterController();if(!controller)return;
  BaseWeaponManagerComponent manager=controller.GetWeaponManagerComponent();if(!manager)return;
  WeaponSlotComponent slot=manager.GetCurrentSlot();if(!slot || !Supported(slot.GetWeaponType()))return;
  IEntity weapon=slot.GetWeaponEntity();if(!weapon)return;
  InventoryStorageManagerComponent inventory=controller.GetInventoryStorageManager();
  IEntity sling=FindSling(inventory);if(policy==1 && !sling)return;
  BCMM_SlingWeaponClaim claim=new BCMM_SlingWeaponClaim();
  claim.Actor=actor;claim.Weapon=weapon;claim.Sling=sling;claim.Expires=GetGame().GetWorld().GetWorldTime()+2500;
  s_Claims.Set(actor,claim);
  GetGame().GetCallqueue().CallLater(Expire,3000,false,actor,claim);
 }
 static void Expire(IEntity actor,BCMM_SlingWeaponClaim expected)
 {
  BCMM_SlingWeaponClaim current;if(s_Claims.Find(actor,current) && current==expected)s_Claims.Remove(actor);
 }
 static void Dropped(IEntity actor,IEntity weapon)
 {
  if(!Replication.IsServer() || !actor || !weapon)return;
  BCMM_SlingWeaponClaim claim;
  if(!s_Claims.Find(actor,claim) || claim.Weapon!=weapon || claim.DropObserved)return;
  if(GetGame().GetWorld().GetWorldTime()>claim.Expires)return;
  claim.DropObserved=true;
  // Let the native drop transaction finish before reinserting its original entity.
  GetGame().GetCallqueue().CallLater(Restore,1,false,actor,claim);
 }
 static void Restore(IEntity actor,BCMM_SlingWeaponClaim claim)
 {
  if(!actor || !claim || !claim.Weapon || GetGame().GetWorld().GetWorldTime()>claim.Expires)return;
  BCMM_SlingWeaponClaim current;if(!s_Claims.Find(actor,current) || current!=claim)return;
  s_Claims.Remove(actor);
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);if(!character)return;
  CharacterControllerComponent controller=character.GetCharacterController();
  if(!controller || controller.GetLifeState()==ECharacterLifeState.DEAD || !controller.IsUnconscious())return;
  InventoryStorageManagerComponent inventory=controller.GetInventoryStorageManager();if(!inventory)return;
  int policy=Math.Round(BCMM_TreatmentAccess.Setting("weaponRetentionPolicy"));
  if(policy<=0 || (policy==1 && (!claim.Sling || !inventory.Contains(claim.Sling))))return;
  if(inventory.Contains(claim.Weapon))return;
  // Never reclaim a weapon that somebody else acquired during the drop.
  InventoryItemComponent item=InventoryItemComponent.Cast(claim.Weapon.FindComponent(InventoryItemComponent));
  if(!item || item.GetParentSlot())return;
  if(vector.Distance(actor.GetOrigin(),claim.Weapon.GetOrigin())>2)return;
  inventory.TryInsertItem(claim.Weapon,EStoragePurpose.PURPOSE_ANY);
 }
}
modded class SCR_CharacterControllerComponent
{
 override protected void OnWeaponDropped(IEntity pWeaponEntity,WeaponSlotComponent pWeaponSlot)
 {
  super.OnWeaponDropped(pWeaponEntity,pWeaponSlot);
  BCMM_WeaponRetention.Dropped(GetOwner(),pWeaponEntity);
 }
}
