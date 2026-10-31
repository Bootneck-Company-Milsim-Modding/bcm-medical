// Original BCM role authority. Future loadout code may call SetMedic on the server.
modded class SCR_CharacterDamageManagerComponent
{
 [RplProp()] protected bool m_BCMMMedic;
 bool BCMM_IsMedic() { return m_BCMMMedic; }
 void BCMM_SetMedic(bool enabled)
 {
  if (!Replication.IsServer()) return;
  m_BCMMMedic = enabled;
  Replication.BumpMe();
 }
}
class BCMM_MedicRole
{
 static bool IsMedic(IEntity actor)
 {
  if (!actor) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(actor.FindComponent(SCR_CharacterDamageManagerComponent));
  return medical && medical.BCMM_IsMedic();
 }
 static float Duration(IEntity actor, float seconds)
 {
  if (!IsMedic(actor)) return seconds;
  return seconds * (1 - BCMM_TreatmentAccess.Setting("medicTimeReductionPercent") * 0.01);
 }
 static bool Allows(IEntity actor, string restriction)
 {
  return BCMM_TreatmentAccess.Setting(restriction) < 0.5 || IsMedic(actor);
 }
 static bool EquipmentAllowed(IEntity actor, int procedure)
 {
  if(procedure==BCMM_EquipmentProcedure.CONNECT_OXYGEN || procedure==BCMM_EquipmentProcedure.DISCONNECT_OXYGEN) return Allows(actor,"medicOnlyOxygen");
  if (procedure == BCMM_EquipmentProcedure.APPLY_PADS || procedure == BCMM_EquipmentProcedure.ANALYSE || procedure == BCMM_EquipmentProcedure.SHOCK) return Allows(actor, "medicOnlyAED");
  if (BCMM_EquipmentPolicy.IsLineConnection(procedure) || procedure==BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE || procedure == BCMM_EquipmentProcedure.INSERT_IV || BCMM_EquipmentPolicy.IsInfusion(procedure)) return Allows(actor, "medicOnlyFluids");
  if (procedure == BCMM_EquipmentProcedure.INSERT_AIRWAY || procedure == BCMM_EquipmentProcedure.SUCTION || procedure == BCMM_EquipmentProcedure.NCD_LEFT || procedure == BCMM_EquipmentProcedure.NCD_RIGHT || procedure == BCMM_EquipmentProcedure.DRAIN_LEFT || procedure == BCMM_EquipmentProcedure.DRAIN_RIGHT) return Allows(actor, "medicOnlyAdvancedAirway");
  return true;
 }
}
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class BCMM_SetMedicContextAction : SCR_SelectedEntitiesContextAction
{
 [Attribute("1")] protected bool m_EnableMedic;
 override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
 {
  return selectedEntity && selectedEntity.GetEntityType() == EEditableEntityType.CHARACTER;
 }
 override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
 {
  if (!selectedEntity) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(selectedEntity.GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
  return medical && !medical.IsDestroyed() && medical.BCMM_IsMedic() != m_EnableMedic;
 }
 override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
 {
  if (!Replication.IsServer() || !CanBePerformed(selectedEntity, cursorWorldPosition, 0)) return;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(selectedEntity.GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
  medical.BCMM_SetMedic(m_EnableMedic);
 }
}
