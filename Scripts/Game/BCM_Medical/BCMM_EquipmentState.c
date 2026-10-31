// Original BCM equipment state. These are gameplay rules and abstract reserve units.
enum BCMM_EquipmentKind { CHEST_SEAL, NCD, AIRWAY, SUCTION, BVM, AED, PADS, CHEST_DRAIN, IV_ACCESS, FLUID, MONITOR, PULSE_OXIMETER, SALINE, PLASMA, OXYGEN }
enum BCMM_EquipmentProcedure { SEAL_LEFT, SEAL_RIGHT, NCD_LEFT, NCD_RIGHT, INSERT_AIRWAY, SUCTION, VENTILATE, APPLY_PADS, ANALYSE, SHOCK, DRAIN_LEFT, DRAIN_RIGHT, INSERT_IV, INFUSE, MONITOR, PULSE_OXIMETER, INFUSE_SALINE, INFUSE_PLASMA, CONNECT_OXYGEN, DISCONNECT_OXYGEN, CONNECT_WHOLE_BLOOD_LINE, CONNECT_SALINE_LINE, CONNECT_PLASMA_LINE, DISCONNECT_FLUID_LINE }
class BCMM_EquipmentState
{
 bool OxygenActive;
 bool AirwayAdjunct;
 bool PadsAttached;
 bool VascularAccess;
 int VascularAccessRegion = BCMM_BodyRegion.LEFT_ARM;
 bool LeftDrain;
 bool RightDrain;
 bool LeftDecompressed;
 bool RightDecompressed;
 bool LastAnalysisAdvised;
 int AnalysisDevice;
 int MonitorChecks;
 float AnalysisAge = 1000000;
 void Analyse(BCMM_Patient patient, int device)
 {
  AnalysisDevice = device;
  AnalysisAge = 0;
  LastAnalysisAdvised = patient.IsShockAdvised();
 }
 bool MayShock(BCMM_Patient patient, int device, notnull BCMM_Settings settings)
 {
  return PadsAttached && device > 0 && device == AnalysisDevice && AnalysisAge <= settings.Get("aedAnalysisValiditySeconds") && LastAnalysisAdvised && patient.IsShockAdvised() && !patient.HasCompressions();
 }
 void Advance(float seconds, BCMM_Patient patient, notnull BCMM_Settings settings)
 {
  AnalysisAge = Math.Min(1000000, AnalysisAge + seconds);
  if (LeftDrain) Drain(patient, true, seconds * settings.Get("chestDrainMlPerSecond"));
  if (RightDrain) Drain(patient, false, seconds * settings.Get("chestDrainMlPerSecond"));
  float recovery = seconds * settings.Get("lungReinflationPerSecond");
  if ((LeftDecompressed || LeftDrain || patient.LeftChest.Sealed) && !patient.LeftChest.Tension && (!patient.LeftChest.OpenWound || patient.LeftChest.Sealed))
   patient.LeftChest.CollapseFraction = Math.Max(0, patient.LeftChest.CollapseFraction - recovery);
  if ((RightDecompressed || RightDrain || patient.RightChest.Sealed) && !patient.RightChest.Tension && (!patient.RightChest.OpenWound || patient.RightChest.Sealed))
   patient.RightChest.CollapseFraction = Math.Max(0, patient.RightChest.CollapseFraction - recovery);
 }
 static float Drain(BCMM_Patient patient, bool left, float requested)
 {
  BCMM_ChestSide side = patient.RightChest;
  BCMM_BleedCompartment compartment = BCMM_BleedCompartment.RIGHT_PLEURAL;
  if (left) { side = patient.LeftChest; compartment = BCMM_BleedCompartment.LEFT_PLEURAL; }
  float removed = Math.Min(side.PleuralBloodMl, Math.Max(0, requested));
  float remaining = removed;
  for (int i = 0; i < patient.InternalBleedCount(); i++)
  {
   BCMM_InternalBleeding source = patient.InternalBleedAt(i);
   if (source.Compartment == compartment) remaining -= source.DrainPleural(remaining);
  }
  side.PleuralBloodMl -= removed;
  return removed;
 }
}
