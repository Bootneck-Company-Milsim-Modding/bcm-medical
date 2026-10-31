// Original BCM gameplay model. These probabilities/timings are configurable
// game rules, not clinical claims about bullet removal or wound closure.
enum BCMM_InjuryCause { GUNSHOT, LACERATION, PUNCTURE, ABRASION, BLAST }
enum BCMM_BodyRegion { HEAD, CHEST, ABDOMEN, LEFT_ARM, RIGHT_ARM, LEFT_LEG, RIGHT_LEG }
enum BCMM_Activity { RESTING, WALKING, RUNNING, CRAWLING }
enum BCMM_BleedCompartment { SOFT_TISSUE, ABDOMEN, LEFT_PLEURAL, RIGHT_PLEURAL, CRANIAL }

class BCMM_Fracture
{
 bool Broken;
 bool Splinted;
 bool ApplySplint()
 {
  if (!Broken || Splinted) return false;
  Splinted = true;
  return true;
 }
 bool DefinitiveRepair(bool facility, bool surgicalKit, bool qualified)
 {
  if (!Broken || !facility || !surgicalKit || !qualified) return false;
  Broken = false;
  Splinted = false;
  return true;
 }
 float MovementScale(notnull BCMM_Settings settings)
 {
  if (!Broken) return 1;
  if (Splinted) return settings.Get("fractureSplintedSpeedScale");
  return settings.Get("fractureUnsplitSpeedScale");
 }
 bool AllowsJump() { return !Broken; }
 float MotionBloodLoss(float seconds, BCMM_Activity activity, notnull BCMM_Settings settings)
 {
  if (!Broken || !(seconds > 0) || (activity == BCMM_Activity.RESTING || activity == BCMM_Activity.CRAWLING)) return 0;
  float rate = settings.Get("fractureWalkingBleedMlPerSecond");
  if (activity == BCMM_Activity.RUNNING) rate = settings.Get("fractureRunningBleedMlPerSecond");
  // Unsplinted movement is slower; immobilisation is still beneficial. Both
  // paths lose blood only while loading the limb, not while carried/crawling.
  return rate * seconds;
 }
}

class BCMM_InternalBleeding
{
 int Id;
 BCMM_BodyRegion Region;
 BCMM_BleedCompartment Compartment;
 float RateMlPerSecond;
 float PooledMl;
 bool Repaired;
 float Advance(float seconds,float clotSupport=1)
 {
  if (Repaired || !(seconds > 0)) return 0;
  float loss = Math.Max(0, RateMlPerSecond) * seconds * Math.Clamp(clotSupport,0,3);
  PooledMl += loss;
  return loss;
 }
 // Only pleural collections are drained by this procedure. Draining does not
 // return blood to the circulation and does not repair the bleeding source.
 float DrainPleural(float requestedMl)
 {
  if (Compartment != BCMM_BleedCompartment.LEFT_PLEURAL && Compartment != BCMM_BleedCompartment.RIGHT_PLEURAL) return 0;
  float removed = Math.Min(PooledMl, Math.Max(0, requestedMl));
  PooledMl -= removed;
  return removed;
 }
}

class BCMM_ProcedureCapabilities
{
 bool SurgeryKit;
 bool SutureKit;
 bool ChestDrainKit;
 bool QualifiedSurgeon;
 bool AtMedicalFacility;
 // Populated/revalidated by future authority inventory/facility adapters.
 bool CanOperate(BCMM_BodyRegion region, notnull BCMM_Settings settings)
 {
  if (!SurgeryKit || !QualifiedSurgeon) return false;
  bool critical = region == BCMM_BodyRegion.HEAD || region == BCMM_BodyRegion.CHEST || region == BCMM_BodyRegion.ABDOMEN;
  return !critical || settings.Get("criticalSurgeryFacilityOnly") < 0.5 || AtMedicalFacility;
 }
}

modded class BCMM_Patient
{
 ref BCMM_Fracture LeftLeg = new BCMM_Fracture();
 ref BCMM_Fracture RightLeg = new BCMM_Fracture();
 protected ref array<ref BCMM_InternalBleeding> m_InternalBleeds = {};
 protected int m_NextInternalId;
 protected float m_TotalBloodLostMl;

 int InternalBleedCount() { return m_InternalBleeds.Count(); }
 BCMM_InternalBleeding InternalBleedAt(int index)
 {
  if (!m_InternalBleeds.IsIndexValid(index)) return null;
  return m_InternalBleeds[index];
 }
 int RegisterInjury(BCMM_BodyRegion region, BCMM_InjuryCause cause, float flowMlPerSecond, float projectileSample, notnull BCMM_Settings settings)
 {
  if (!(projectileSample >= 0 && projectileSample <= 1)) return 0;
  int id = AddWound(region, flowMlPerSecond);
  BCMM_Wound wound = FindWound(id);
  if (!wound) return 0;
  wound.Cause = cause;
  wound.RetainedProjectile = cause == BCMM_InjuryCause.GUNSHOT && projectileSample < settings.Get("retainedBulletChance");
  return id;
 }
 bool DressWound(int id, float timingSample, notnull BCMM_Settings settings)
 {
  BCMM_Wound wound = FindWound(id);
  return !Terminal && wound && wound.Dress(settings, timingSample);
 }
 bool ExtractProjectile(int id, notnull BCMM_ProcedureCapabilities capability, notnull BCMM_Settings settings)
 {
  BCMM_Wound wound = FindWound(id);
  if (Terminal || !wound || !wound.RetainedProjectile || !capability.CanOperate(wound.BodyZone, settings)) return false;
  wound.RetainedProjectile = false;
  // Extraction alone neither sutures nor repairs damaged organs.
  return true;
 }
 bool SutureWound(int id, notnull BCMM_ProcedureCapabilities capability)
 {
  BCMM_Wound wound = FindWound(id);
  if (Terminal || !capability.SutureKit || !capability.QualifiedSurgeon || !wound) return false;
  return wound.CloseWithSutures();
 }
 int AddInternalBleeding(BCMM_BodyRegion region, BCMM_BleedCompartment compartment, float flowMlPerSecond)
 {
  if (Terminal || !(flowMlPerSecond >= 0)) return 0;
  BCMM_InternalBleeding source = new BCMM_InternalBleeding();
  source.Id = ++m_NextInternalId;
  source.Region = region;
  source.Compartment = compartment;
  source.RateMlPerSecond = flowMlPerSecond;
  m_InternalBleeds.Insert(source);
  return source.Id;
 }
 BCMM_InternalBleeding FindInternalBleed(int id)
 {
  foreach (BCMM_InternalBleeding source : m_InternalBleeds)
   if (source.Id == id) return source;
  return null;
 }
 bool RepairInternalBleeding(int id, notnull BCMM_ProcedureCapabilities capability, notnull BCMM_Settings settings)
 {
  BCMM_InternalBleeding source = FindInternalBleed(id);
  if (Terminal || !source || source.Repaired || !capability.CanOperate(source.Region, settings)) return false;
  source.Repaired = true;
  return true;
 }
 float TotalBloodLostMl() { return m_TotalBloodLostMl; }
 float AdvanceTrauma(float seconds, BCMM_Activity activity, notnull BCMM_Settings settings)
 {
  if (Terminal || !(seconds > 0)) return 0;
  float loss;
  foreach (BCMM_Wound wound : m_Wounds) loss += wound.Advance(seconds, activity, settings, Medication.ClotWearScale(settings)*Fluids.DressingWearMultiplier(settings));
  m_TotalBloodLostMl += loss;
  return loss + AdvanceInternalTrauma(seconds, activity, settings);
 }
 float AdvanceInternalTrauma(float seconds, BCMM_Activity activity, notnull BCMM_Settings settings)
 {
  if (Terminal || !(seconds > 0)) return 0;
  float loss;
  foreach (BCMM_InternalBleeding source : m_InternalBleeds)
  {
   float sourceLoss = source.Advance(seconds,(1-Medication.TXAEffect*settings.Get("txaInternalBleedReduction"))*Fluids.BleedingMultiplier(settings));
   loss += sourceLoss;
   if (source.Compartment == BCMM_BleedCompartment.LEFT_PLEURAL) LeftChest.PleuralBloodMl += sourceLoss;
   if (source.Compartment == BCMM_BleedCompartment.RIGHT_PLEURAL) RightChest.PleuralBloodMl += sourceLoss;
  }
  loss += LeftLeg.MotionBloodLoss(seconds, activity, settings);
  loss += RightLeg.MotionBloodLoss(seconds, activity, settings);
  m_TotalBloodLostMl += loss;
  return loss;
 }
 float LegMovementScale(notnull BCMM_Settings settings)
 {
  return Math.Min(LeftLeg.MovementScale(settings), RightLeg.MovementScale(settings));
 }
 bool LegsAllowJump() { return LeftLeg.AllowsJump() && RightLeg.AllowsJump(); }
}
