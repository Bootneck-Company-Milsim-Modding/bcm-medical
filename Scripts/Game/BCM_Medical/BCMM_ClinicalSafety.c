// Original BCM game safeguards. Thresholds are server settings, not real dosing advice.
class BCMM_ClinicalSafety
{
 static string MedicationDenial(BCMM_Patient patient,int kind,notnull BCMM_Settings settings)
 {
  if(!patient || patient.Terminal || kind<0 || kind>BCMM_MedicationKind.FENTANYL) { return "Patient or medication unavailable."; }
  if(!(patient.HeartRate>=0 && patient.HeartRate<=1000 && patient.SystolicPressure>=0 && patient.SystolicPressure<=1000 && patient.RespiratoryRate>=0 && patient.RespiratoryRate<=200 && patient.OxygenSaturation>=0 && patient.OxygenSaturation<=1)) { return "Medication withheld: invalid vital state."; }
  if(settings.Get("medicationVitalChecksEnabled")>0.5)
  {
   if(kind==BCMM_MedicationKind.MORPHINE)
   {
    if(!patient.HasSpontaneousCirculation) { return "Morphine withheld: no spontaneous circulation."; }
    if(patient.HeartRate<settings.Get("morphineMinimumHeartRate")) { return "Morphine withheld: pulse too low."; }
    if(patient.SystolicPressure<settings.Get("morphineMinimumSystolicPressure")) { return "Morphine withheld: systolic pressure too low."; }
    if(patient.RespiratoryRate<settings.Get("morphineMinimumRespiratoryRate")) { return "Morphine withheld: breathing too slow."; }
    if(patient.OxygenSaturation<settings.Get("morphineMinimumOxygenFraction")) { return "Morphine withheld: oxygen saturation too low."; }
   }
   if(kind==BCMM_MedicationKind.EPINEPHRINE && patient.HasSpontaneousCirculation)
   {
    if(settings.Get("epinephrineWithPulseEnabled")<0.5) { return "Epinephrine is restricted to cardiac arrest by GM settings."; }
    if(patient.HeartRate>=settings.Get("epinephrineMaximumHeartRate")) { return "Epinephrine withheld: pulse too high."; }
    if(patient.SystolicPressure>=settings.Get("epinephrineMaximumSystolicPressure")) { return "Epinephrine withheld: systolic pressure too high."; }
   }
  }
  if(kind==BCMM_MedicationKind.NALOXONE && settings.Get("naloxoneRequiresOpioid")>0.5 && !patient.Medication.HasActiveOpioid()) { return "Naloxone unavailable: no active opioid recorded."; }
  if(kind==BCMM_MedicationKind.PARACETAMOL && (!patient.HasSpontaneousCirculation || patient.Airway!=BCMM_AirwayCondition.PATENT || patient.RespiratoryRate<settings.Get("oralMinimumRespiratoryRate"))) { return "Tablets withheld: unsafe swallowing conditions."; }
  if(kind==BCMM_MedicationKind.FENTANYL)
  {
   if(!patient.HasSpontaneousCirculation) { return "Fentanyl withheld: no spontaneous circulation."; }
   if(settings.Get("fentanylRequiresVascularAccess")>0.5 && !patient.Equipment.VascularAccess) { return "Fentanyl requires vascular access."; }
   if(settings.Get("medicationVitalChecksEnabled")>0.5 && (patient.HeartRate<settings.Get("fentanylMinimumHeartRate") || patient.SystolicPressure<settings.Get("fentanylMinimumSystolicPressure") || patient.RespiratoryRate<settings.Get("fentanylMinimumRespiratoryRate") || patient.OxygenSaturation<settings.Get("fentanylMinimumOxygenFraction"))) { return "Fentanyl withheld: unsafe pulse, pressure, breathing or saturation."; }
  }
  if(kind==BCMM_MedicationKind.TXA)
  {
   if(settings.Get("txaRequiresVascularAccess")>0.5 && !patient.Equipment.VascularAccess) { return "TXA requires vascular access."; }
   bool indicated;
   for(int woundIndex=0;woundIndex<patient.WoundCount();woundIndex++)
   {
    BCMM_Wound wound=patient.WoundAt(woundIndex);if(!wound.Sutured && wound.UncontrolledFlowMlPerSecond>0) { indicated=true; }
   }
   for(int sourceIndex=0;sourceIndex<patient.InternalBleedCount();sourceIndex++)
   {
    BCMM_InternalBleeding source=patient.InternalBleedAt(sourceIndex);if(!source.Repaired && source.RateMlPerSecond>0) { indicated=true; }
   }
   if(!indicated) { return "TXA unavailable: no open/dressed bleeding wound or unrepaired internal source."; }
  }
  if(!patient.Medication.CanAdminister(kind,settings)) { return "Medication withheld: repeat interval or active dose limit."; }
  return "";
 }
 static string FluidDenial(BCMM_Patient patient,float bloodFraction,notnull BCMM_Settings settings,int fluidKind=BCMM_FluidKind.WHOLE_BLOOD)
 {
  if(!patient || patient.Terminal || !(bloodFraction>=0 && bloodFraction<=1)) { return "Patient reserve unavailable."; }
  if(!(patient.SystolicPressure>=0 && patient.SystolicPressure<=1000)) { return "Infusion withheld: invalid pressure state."; }
  if(!patient.Equipment.VascularAccess) { return "Establish vascular access before infusion."; }
  if(patient.BCMM_WithholdAdditionalWater(fluidKind,settings)){return "Infusion withheld: redistributed-fluid burden exceeds the GM water-loading limit.";}
  float deficit=(1-bloodFraction)*settings.Get("normalBloodVolumeMl");
  if(deficit<0.001) { return "Infusion unnecessary: reserve already full."; }
  if(settings.Get("fluidSafetyChecksEnabled")>0.5)
  {
   if(patient.SystolicPressure>=settings.Get("fluidMaximumSystolicPressure")) { return "Infusion withheld: systolic pressure at the GM stop limit."; }
   if(deficit<settings.Get("fluidMinimumDeficitMl")) { return "Infusion unnecessary: deficit below the GM minimum."; }
  }
  return "";
 }
}