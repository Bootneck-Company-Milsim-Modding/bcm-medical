// Original BCM surgical item procedures. All choices and state changes are server-side.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_SurgeryKitComponentClass : ScriptComponentClass {}
class BCMM_SurgeryKitComponent : ScriptComponent {}
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_SutureKitComponentClass : ScriptComponentClass {}
class BCMM_SutureKitComponent : ScriptComponent {}
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_MedicalFacilityComponentClass : ScriptComponentClass {}
class BCMM_MedicalFacilityComponent : ScriptComponent
{
 protected static ref array<BCMM_MedicalFacilityComponent> s_Facilities = {};
 override protected void OnPostInit(IEntity owner) { super.OnPostInit(owner); s_Facilities.Insert(this); }
 override protected void OnDelete(IEntity owner) { s_Facilities.RemoveItem(this); super.OnDelete(owner); }
 static bool Supports(IEntity actor, IEntity patient)
 {
  if (!actor || !patient) return false;
  float radius = BCMM_TreatmentAccess.Setting("medicalFacilityRangeMetres");
  foreach (BCMM_MedicalFacilityComponent facility : s_Facilities)
  {
   if (!facility || !facility.GetOwner() || facility.GetOwner().GetParent()) continue;
   BCMM_StretcherComponent stretcher = BCMM_StretcherComponent.Cast(facility.GetOwner().FindComponent(BCMM_StretcherComponent));
   if (stretcher && stretcher.IsCarried()) continue;
   vector point = facility.GetOwner().GetOrigin();
   if (vector.Distance(actor.GetOrigin(), point) <= radius && vector.Distance(patient.GetOrigin(), point) <= radius) return true;
  }
  return false;
 }
}
enum BCMM_SurgicalProcedure { EXTRACT_PROJECTILE, SUTURE, REPAIR_LEFT_LEG, REPAIR_RIGHT_LEG, REPAIR_INTERNAL }
class BCMM_SurgicalTreatment : BCMM_SplintTreatment
{
 protected BCMM_SurgicalProcedure m_Procedure;
 protected int m_InjuryId;
 protected int m_Region = -1;
 static IEntity FindSupply(IEntity actor, bool suture)
 {
  if (!actor) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   if (suture && item.FindComponent(BCMM_SutureKitComponent)) return item;
   if (!suture && item.FindComponent(BCMM_SurgeryKitComponent)) return item;
  }
  return null;
 }
 static bool BeginSurgery(IEntity actor, IEntity target, BCMM_SurgicalProcedure procedure, SCR_PlayerController player = null, int region = -1)
 {
  if (!Replication.IsServer() || (player && player.GetControlledEntity() != actor) || !BCMM_TreatmentAccess.CanReach(actor, target)) return false;
  if (!BCMM_MedicRole.Allows(actor, "medicOnlySurgery")) return false;
  if (region < -1 || region > 6) return false;
  if (region >= 0 && procedure == BCMM_SurgicalProcedure.REPAIR_LEFT_LEG && region != BCMM_BodyRegion.LEFT_LEG) return false;
  if (region >= 0 && procedure == BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG && region != BCMM_BodyRegion.RIGHT_LEG) return false;
  IEntity kit = FindSupply(actor, procedure == BCMM_SurgicalProcedure.SUTURE);
  if (!kit) return false;
  BCMM_SurgicalTreatment treatment = new BCMM_SurgicalTreatment();
  treatment.m_Actor = actor;
  treatment.m_Target = target;
  treatment.m_Procedure = procedure;
  treatment.m_Region = region;
  if (!treatment.SelectInjury() || !treatment.StillEligible()) return false;
  string duration = "surgeryDurationSeconds";
  if (procedure == BCMM_SurgicalProcedure.SUTURE) duration = "sutureDurationSeconds";
  return StartOperation(treatment, actor, target, kit, BCMM_TreatmentAccess.Setting(duration), player);
 }
 protected BCMM_ProcedureCapabilities Capabilities()
 {
  BCMM_ProcedureCapabilities capability = new BCMM_ProcedureCapabilities();
  // Field kit users are permitted by current BCM gameplay policy. This does
  // not infer a profession from a client-supplied flag or equipment name.
  capability.QualifiedSurgeon = true;
  capability.SurgeryKit = m_Procedure != BCMM_SurgicalProcedure.SUTURE;
  capability.SutureKit = m_Procedure == BCMM_SurgicalProcedure.SUTURE;
  capability.AtMedicalFacility = BCMM_MedicalFacilityComponent.Supports(m_Actor, m_Target);
  return capability;
 }
 protected bool SelectInjury()
 {
  BCMM_Patient patient = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient();
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_LEFT_LEG || m_Procedure == BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG) return true;
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_INTERNAL)
  {
   for (int i = 0; i < patient.InternalBleedCount(); i++)
   {
    BCMM_InternalBleeding source = patient.InternalBleedAt(i);
    if (source && !source.Repaired && (m_Region < 0 || source.Region == m_Region)) { m_InjuryId = source.Id; return true; }
   }
   return false;
  }
  for (int index = 0; index < patient.WoundCount(); index++)
  {
   BCMM_Wound wound = patient.WoundAt(index);
   if (!wound || wound.Sutured || (m_Region >= 0 && wound.BodyZone != m_Region)) continue;
   if (m_Procedure == BCMM_SurgicalProcedure.EXTRACT_PROJECTILE && wound.RetainedProjectile) { m_InjuryId = wound.Id; return true; }
   if (m_Procedure == BCMM_SurgicalProcedure.SUTURE && !wound.RetainedProjectile && wound.FlowMlPerSecond() <= 0) { m_InjuryId = wound.Id; return true; }
  }
  return false;
 }
 override protected bool StillEligible()
 {
  if (!BCMM_MedicRole.Allows(m_Actor, "medicOnlySurgery")) return false;
  if (!m_Target) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed() || medical.BCMM_GetPatient().Terminal) return false;
  BCMM_Patient patient = medical.BCMM_GetPatient();
  BCMM_ProcedureCapabilities capability = Capabilities();
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_CopySettings(settings);
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_LEFT_LEG) return patient.LeftLeg.Broken && capability.AtMedicalFacility;
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG) return patient.RightLeg.Broken && capability.AtMedicalFacility;
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_INTERNAL)
  {
   BCMM_InternalBleeding source = patient.FindInternalBleed(m_InjuryId);
   return source && !source.Repaired && capability.CanOperate(source.Region, settings);
  }
  BCMM_Wound wound = patient.FindWound(m_InjuryId);
  if (!wound || wound.Sutured) return false;
  if (m_Procedure == BCMM_SurgicalProcedure.EXTRACT_PROJECTILE) return wound.RetainedProjectile && capability.CanOperate(wound.BodyZone, settings);
  return !wound.RetainedProjectile && wound.FlowMlPerSecond() <= 0;
 }
 protected override int ProcedureRegion()
 {
  if (m_Region >= 0) return m_Region;
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_LEFT_LEG) return BCMM_BodyRegion.LEFT_LEG;
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG) return BCMM_BodyRegion.RIGHT_LEG;
  BCMM_Patient patient = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient();
  if (m_Procedure == BCMM_SurgicalProcedure.REPAIR_INTERNAL) return patient.FindInternalBleed(m_InjuryId).Region;
  return patient.FindWound(m_InjuryId).BodyZone;
 }
 override protected string ProcedureName()
 {
  if (m_Procedure == BCMM_SurgicalProcedure.SUTURE) return "Suturing";
  return "Surgery";
 }
 override protected void ApplyProcedure()
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_Patient patient = medical.BCMM_GetPatient();
  BCMM_ProcedureCapabilities capability = Capabilities();
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_CopySettings(settings);
  bool success;
  switch (m_Procedure)
  {
   case BCMM_SurgicalProcedure.EXTRACT_PROJECTILE: success = patient.ExtractProjectile(m_InjuryId, capability, settings); break;
   case BCMM_SurgicalProcedure.SUTURE: success = patient.SutureWound(m_InjuryId, capability); break;
   case BCMM_SurgicalProcedure.REPAIR_LEFT_LEG: success = patient.LeftLeg.DefinitiveRepair(capability.AtMedicalFacility, true, true); break;
   case BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG: success = patient.RightLeg.DefinitiveRepair(capability.AtMedicalFacility, true, true); break;
   case BCMM_SurgicalProcedure.REPAIR_INTERNAL: success = patient.RepairInternalBleeding(m_InjuryId, capability, settings); break;
  }
  if (success && m_Procedure == BCMM_SurgicalProcedure.REPAIR_LEFT_LEG) medical.BCMM_RepairNativeLeg(true);
  if (success && m_Procedure == BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG) medical.BCMM_RepairNativeLeg(false);
  medical.BCMM_PublishFractures();
  if (m_Player && success) m_Player.BCMM_TreatmentFeedback(ProcedureName() + " completed.");
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestSurgery(IEntity target, BCMM_SurgicalProcedure procedure, int region = -1)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_SurgeryRequest, BCMM_BodyBagComponent.Id(target), procedure, region);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_SurgeryRequest(RplId target, int procedure, int region)
 {
  if (procedure < BCMM_SurgicalProcedure.EXTRACT_PROJECTILE || procedure > BCMM_SurgicalProcedure.REPAIR_INTERNAL) return;
  if (!BCMM_SurgicalTreatment.BeginSurgery(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(target), procedure, this, region))
   BCMM_TreatmentFeedback("Cannot begin procedure. Check casualty, kit, stabilised wound and medical-facility requirements.");
 }
}