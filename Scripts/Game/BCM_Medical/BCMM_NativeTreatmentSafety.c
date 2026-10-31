// Original adapters keep native inventory treatment routes within BCM clinical policy.
class BCMM_NativeTreatmentSafety
{
 static BCMM_Settings Settings()
 {
  BCMM_Settings settings=new BCMM_Settings();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(mode) { mode.BCMM_CopySettings(settings); }return settings;
 }
 static bool FluidAllowed(IEntity target,IEntity user=null)
 {
  if(!target) { return false; }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical || medical.IsDestroyed() || !medical.GetBloodHitZone()) { return false; }
  if(user && !BCMM_MedicRole.EquipmentAllowed(user,BCMM_EquipmentProcedure.INFUSE)) { return false; }
  if(Replication.IsServer() && (BCMM_FluidLineSession.ForPatient(target) || BCMM_FluidLineTransfer.PatientPending(target)))return false;
  if(!Replication.IsServer() && medical.BCMM_FluidLineActive())return false;
  BCMM_Settings settings=Settings();
  if(Replication.IsServer()) { return BCMM_ClinicalSafety.FluidDenial(medical.BCMM_GetPatient(),medical.GetBloodHitZone().GetHealthScaled(),settings,BCMM_FluidKind.SALINE).IsEmpty(); }
  if(medical.BCMM_WaterLoadingWithheld()){return false;}
  if(!(medical.BCMM_AppliedEquipment() & 32)) { return false; }
  float deficit=(1-medical.GetBloodHitZone().GetHealthScaled())*settings.Get("normalBloodVolumeMl");
  if(deficit<0.001) { return false; }
  return settings.Get("fluidSafetyChecksEnabled")<0.5 || (medical.BCMM_SystolicPressure()<settings.Get("fluidMaximumSystolicPressure") && deficit>=settings.Get("fluidMinimumDeficitMl"));
 }
 static bool MorphineAllowed(IEntity target,IEntity user)
 {
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical || medical.IsDestroyed() || !BCMM_MedicRole.Allows(user,"medicOnlyMedication")) { return false; }
  BCMM_Settings settings=Settings();
  if(Replication.IsServer()) { return BCMM_ClinicalSafety.MedicationDenial(medical.BCMM_GetPatient(),BCMM_MedicationKind.MORPHINE,settings).IsEmpty(); }
  if(settings.Get("medicationVitalChecksEnabled")<0.5) { return true; }
  return !medical.BCMM_InCardiacArrest() && medical.BCMM_Pulse()>=settings.Get("morphineMinimumHeartRate") && medical.BCMM_SystolicPressure()>=settings.Get("morphineMinimumSystolicPressure") && medical.BCMM_RespiratoryRate()>=settings.Get("morphineMinimumRespiratoryRate") && medical.BCMM_OxygenSaturation()>=settings.Get("morphineMinimumOxygenFraction");
 }
}
[BaseContainerProps()]
modded class SCR_ConsumableSalineBag
{
 override bool CanApplyEffect(notnull IEntity target,notnull IEntity user,out SCR_EConsumableFailReason failReason=SCR_EConsumableFailReason.NONE)
 {
  return BCMM_NativeTreatmentSafety.FluidAllowed(target,user) && super.CanApplyEffect(target,user,failReason);
 }
 override bool CanApplyEffectToHZ(notnull IEntity target,notnull IEntity user,ECharacterHitZoneGroup group,out SCR_EConsumableFailReason failReason=SCR_EConsumableFailReason.NONE)
 {
  return BCMM_NativeTreatmentSafety.FluidAllowed(target,user) && super.CanApplyEffectToHZ(target,user,group,failReason);
 }
 override void ApplyEffect(notnull IEntity target,notnull IEntity user,IEntity item,ItemUseParameters animParams)
 {
  if (!item || (Replication.IsServer() && BCMM_StockSourceLifetime.Expired(item))) { return; }
  if(Replication.IsServer() && !BCMM_NativeTreatmentSafety.FluidAllowed(target,user)) { return; }
  super.ApplyEffect(target,user,item,animParams);
  BCMM_NativeSalineBinding.ObserveOriginal(target,item,m_fItemRegenerationDuration);
  BCMM_StockSourceLifetime.Observe(target,item,m_fItemRegenerationDuration);
 }
}
[BaseContainerProps()]
modded class SCR_ConsumableMorphine
{
 override bool CanApplyEffect(notnull IEntity target,notnull IEntity user,out SCR_EConsumableFailReason failReason=SCR_EConsumableFailReason.NONE)
 {
  return BCMM_NativeTreatmentSafety.MorphineAllowed(target,user) && super.CanApplyEffect(target,user,failReason);
 }
 override bool CanApplyEffectToHZ(notnull IEntity target,notnull IEntity user,ECharacterHitZoneGroup group,out SCR_EConsumableFailReason failReason=SCR_EConsumableFailReason.NONE)
 {
  return BCMM_NativeTreatmentSafety.MorphineAllowed(target,user) && super.CanApplyEffectToHZ(target,user,group,failReason);
 }
 override void ApplyEffect(notnull IEntity target,notnull IEntity user,IEntity item,ItemUseParameters animParams)
 {
  if(!Replication.IsServer() || !BCMM_NativeTreatmentSafety.MorphineAllowed(target,user)) { return; }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  medical.BCMM_GetPatient().Medication.Administer(BCMM_MedicationKind.MORPHINE,BCMM_NativeTreatmentSafety.Settings());
 }
}
