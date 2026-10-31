// Retain stock clinical supplies when live BCM policy rejects their final commit.
modded class SCR_ConsumableItemComponent
{
 override void ApplyItemEffect(IEntity target, IEntity user, ItemUseParameters animParams, IEntity item, bool deleteItem = true)
 {
  bool denied;
  if (SCR_ConsumableSalineBag.Cast(m_ConsumableEffect))
  {
   denied = !target || !user || !BCMM_NativeTreatmentSafety.FluidAllowed(target, user);
  }
  else if (SCR_ConsumableMorphine.Cast(m_ConsumableEffect))
  {
   denied = !target || !user || !BCMM_NativeTreatmentSafety.MorphineAllowed(target, user);
  }
  if (!denied)
  {
   super.ApplyItemEffect(target, user, animParams, item, deleteItem);
   return;
  }
  // Native ApplyItemEffect deletes after ApplyEffect even when that effect refuses.
  // Clear its gadget/invoker state without applying or deleting the original item.
  IEntity source = GetOwner();
  if (source)
  {
   InventoryItemComponent inventoryItem = InventoryItemComponent.Cast(source.FindComponent(InventoryItemComponent));
   if (inventoryItem && user)
   {
    inventoryItem.RequestUserLock(user, false);
   }
   if (m_bAlternativeModelOnAction)
   {
    SetAlternativeModel(false);
   }
  }
  ModeClear(EGadgetMode.IN_HAND);
  SetTargetCharacter(null);
 }
}
