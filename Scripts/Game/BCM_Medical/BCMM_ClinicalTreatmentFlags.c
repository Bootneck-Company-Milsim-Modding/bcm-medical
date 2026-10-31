// Owner-only examination snapshot; never an authorization to mutate a patient.
class BCMM_ClinicalTreatmentFlags
{
 static int ForPatient(IEntity actor, IEntity target, int region)
 {
  if (!Replication.IsServer() || region < 0 || region > 6 || !BCMM_TreatmentAccess.CanExamine(actor, target)) return 0;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed()) return 0;
  BCMM_Patient patient = medical.BCMM_GetPatient();
  if (patient.Terminal) return 0;
  int flags;
  if (medical.GetGroupDamageOverTime(BCMM_ClinicalChecks.NativeGroup(region), EDamageType.BLEEDING) > 0) flags |= 1;
  BCMM_ProcedureCapabilities capability = new BCMM_ProcedureCapabilities();
  capability.QualifiedSurgeon = true;
  capability.SurgeryKit = true;
  capability.AtMedicalFacility = BCMM_MedicalFacilityComponent.Supports(actor, target);
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_CopySettings(settings);
  for (int i = 0; i < patient.WoundCount(); i++)
  {
   BCMM_Wound wound = patient.WoundAt(i);
   if (!wound || wound.BodyZone != region || wound.Sutured) continue;
   if (wound.RetainedProjectile && capability.CanOperate(wound.BodyZone, settings)) flags |= 4;
   if (!wound.RetainedProjectile && wound.FlowMlPerSecond() <= 0) flags |= 8;
  }
  BCMM_Fracture leg;
  if (region == BCMM_BodyRegion.LEFT_LEG) leg = patient.LeftLeg;
  if (region == BCMM_BodyRegion.RIGHT_LEG) leg = patient.RightLeg;
  if (leg && leg.Broken)
  {
   if (!leg.Splinted) flags |= 2;
   if (capability.AtMedicalFacility) flags |= 16;
  }
  for (int n = 0; n < patient.InternalBleedCount(); n++)
  {
   BCMM_InternalBleeding source = patient.InternalBleedAt(n);
   if (source && source.Region == region && !source.Repaired && capability.CanOperate(source.Region, settings)) flags |= 32;
  }
  return flags;
 }
}