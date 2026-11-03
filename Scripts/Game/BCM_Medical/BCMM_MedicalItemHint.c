// BCM supplies use native animation categories, but their treatment choices
// belong to the medical menu. Do not label every injector as morphine.
modded class SCR_AvailableActionsWidget
{
 override void SetText(string text,string name,EInputDeviceType currentInputDevice=EInputDeviceType.KEYBOARD)
 {
  if(text=="GadgetActivate")
  {
   PlayerController player=GetGame().GetPlayerController();
   ChimeraCharacter actor;if(player)actor=ChimeraCharacter.Cast(player.GetControlledEntity());
   IEntity held;if(actor)held=actor.GetCharacterController().GetAttachedGadgetAtLeftHandSlot();
   SCR_ConsumableItemComponent item;if(held)item=SCR_ConsumableItemComponent.Cast(held.FindComponent(SCR_ConsumableItemComponent));
   if(item && BCMM_ProcedurePresentationEffect.Cast(item.GetConsumableEffect()))
   {
    text="BCMM_OpenMedical";name="Medical menu";
   }
  }
  super.SetText(text,name,currentInputDevice);
 }
}
