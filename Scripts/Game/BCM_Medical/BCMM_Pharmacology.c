// Original BCM game pharmacology. Values are abstract game units, never real doses.
enum BCMM_MedicationKind { MORPHINE, NALOXONE, EPINEPHRINE, TXA, PARACETAMOL, FENTANYL }
class BCMM_MedicationDose
{
 int Kind;
 float Age;
}
class BCMM_Pharmacology
{
 protected ref array<ref BCMM_MedicationDose> m_Doses = {};
 protected ref array<float> m_LastDose = {-100000, -100000, -100000, -100000, -100000, -100000};
 protected float m_Time;
 float Pain;
 float OpioidEffect;
 float BreathingScale = 1;
 float PressureScale = 1;
 float StimulantEffect;
 float TXAEffect;
 static string Name(int kind)
 {
  if (kind == BCMM_MedicationKind.MORPHINE) return "Morphine";
  if (kind == BCMM_MedicationKind.NALOXONE) return "Naloxone";
  if (kind == BCMM_MedicationKind.EPINEPHRINE) return "Epinephrine";
  if (kind == BCMM_MedicationKind.TXA) return "TXA";
  if(kind==BCMM_MedicationKind.PARACETAMOL) { return "Paracetamol tablets"; }
  if(kind==BCMM_MedicationKind.FENTANYL) { return "Fentanyl"; }
  return "Unknown medication";
 }
 static string Prefix(int kind)
 {
  if (kind == BCMM_MedicationKind.MORPHINE) return "morphine";
  if (kind == BCMM_MedicationKind.NALOXONE) return "naloxone";
  if (kind == BCMM_MedicationKind.TXA) return "txa";
  if(kind==BCMM_MedicationKind.PARACETAMOL) { return "paracetamol"; }
  if(kind==BCMM_MedicationKind.FENTANYL) { return "fentanyl"; }
  return "epinephrine";
 }
 bool HasActiveOpioid() { return HasActiveKind(BCMM_MedicationKind.MORPHINE) || HasActiveKind(BCMM_MedicationKind.FENTANYL); }
 static float DoseStrengthCurve(float age,float onset,float duration) { if(age<onset) { return age/onset; }return Math.Clamp(1-(age-onset)/duration,0,1); }
 float ClotWearScale(notnull BCMM_Settings settings)
 {
  return 1 - TXAEffect * settings.Get("txaClotWearReduction");
 }
 bool CanAdminister(int kind, notnull BCMM_Settings settings)
 {
  if (kind < 0 || kind > BCMM_MedicationKind.FENTANYL) return false;
  float repeat = settings.Get("medicationRepeatSeconds");
  if (kind == BCMM_MedicationKind.TXA) repeat = settings.Get("txaRepeatSeconds");

  if(kind==BCMM_MedicationKind.PARACETAMOL || kind==BCMM_MedicationKind.FENTANYL) { repeat=settings.Get(Prefix(kind)+"RepeatSeconds"); }
  if(m_Time-m_LastDose[kind]<repeat) { return false; }
  int count;
  foreach (BCMM_MedicationDose dose : m_Doses) if (dose.Kind == kind) count++;
  if (kind == BCMM_MedicationKind.TXA) return count < settings.Get("txaMaxActiveDoses");
  if(kind==BCMM_MedicationKind.PARACETAMOL || kind==BCMM_MedicationKind.FENTANYL) { return count<settings.Get(Prefix(kind)+"MaxActiveDoses"); }
  return count < settings.Get("medicationMaxActiveDoses");
 }
 bool Administer(int kind, notnull BCMM_Settings settings)
 {
  if (!CanAdminister(kind, settings)) return false;
  BCMM_MedicationDose dose = new BCMM_MedicationDose();
  dose.Kind = kind;
  m_Doses.Insert(dose);
  m_LastDose[kind] = m_Time;
  return true;
 }
 bool HasActiveKind(int kind)
 {
  foreach(BCMM_MedicationDose dose:m_Doses) { if(dose.Kind==kind) { return true; } }
  return false;
 }
 int ActiveDoses() { return m_Doses.Count(); }
 protected float Strength(BCMM_MedicationDose dose, notnull BCMM_Settings settings)
 {
  float onset = settings.Get(Prefix(dose.Kind) + "OnsetSeconds");
  float duration = settings.Get(Prefix(dose.Kind) + "DurationSeconds");
  if (dose.Age < onset) return dose.Age / onset;
  return Math.Clamp(1 - (dose.Age - onset) / duration, 0, 1);
 }
 void Advance(float seconds, BCMM_Patient patient, notnull BCMM_Settings settings)
 {
  if (!(seconds > 0) || seconds > 3600) return;
  m_Time += seconds;
  float opioid;
  float antagonist;float tablets;float fentanylPain;float fentanylResidual;
  StimulantEffect = 0;
  TXAEffect = 0;
  for (int i = m_Doses.Count() - 1; i >= 0; i--)
  {
   BCMM_MedicationDose dose = m_Doses[i];
   dose.Age += seconds;
   float strength = Strength(dose, settings);
   float residual;
   if(dose.Kind==BCMM_MedicationKind.FENTANYL) { residual=BCMM_Pharmacology.DoseStrengthCurve(dose.Age,settings.Get("fentanylRespiratoryOnsetSeconds"),settings.Get("fentanylRespiratoryDurationSeconds"));fentanylResidual+=residual;fentanylPain+=strength; }
   if(dose.Kind==BCMM_MedicationKind.PARACETAMOL) { tablets+=strength; }
   if (strength <= 0 && residual<=0) { m_Doses.Remove(i); continue; }
   if (dose.Kind == BCMM_MedicationKind.TXA) TXAEffect += strength;
   if (dose.Kind == BCMM_MedicationKind.MORPHINE) opioid += strength;
   if (dose.Kind == BCMM_MedicationKind.NALOXONE) antagonist += strength;
   if (dose.Kind == BCMM_MedicationKind.EPINEPHRINE) StimulantEffect += strength;
  }
  TXAEffect = Math.Clamp(TXAEffect, 0, 1);
  float remaining=1-Math.Clamp(antagonist*settings.Get("naloxoneReversalFraction"),0,1);
  OpioidEffect=(opioid+fentanylResidual)*remaining;
  BreathingScale=Math.Clamp(1-(opioid*settings.Get("morphineRespiratoryDepression")+fentanylResidual*settings.Get("fentanylRespiratoryDepression"))*remaining,0,1);
  PressureScale=Math.Clamp(1-(opioid*settings.Get("morphinePressureDepression")+fentanylResidual*settings.Get("fentanylPressureDepression"))*remaining,0,1);
  Pain=Math.Clamp(InjuryPain(patient,settings)-(opioid*settings.Get("morphinePainReliefUnits")+fentanylPain*settings.Get("fentanylPainReliefUnits"))*remaining-tablets*settings.Get("paracetamolPainReliefUnits"),0,100);
 }
 static float InjuryPain(BCMM_Patient patient, notnull BCMM_Settings settings)
 {
  float pain;
  for (int i = 0; i < patient.WoundCount(); i++)
  {
   BCMM_Wound wound = patient.WoundAt(i);
   float amount = settings.Get("painWoundUnits");
   if (wound.Sutured) amount *= settings.Get("painSuturedMultiplier");
   else if (wound.Bandaged) amount *= settings.Get("painDressedMultiplier");
   pain += amount;
  }
  float fracture = settings.Get("painFractureUnits");
  if (patient.LeftLeg.Broken)
  {
   if (patient.LeftLeg.Splinted) pain += fracture * settings.Get("painSplintMultiplier");
   else pain += fracture;
  }
  if (patient.RightLeg.Broken)
  {
   if (patient.RightLeg.Splinted) pain += fracture * settings.Get("painSplintMultiplier");
   else pain += fracture;
  }
  for (int j = 0; j < patient.InternalBleedCount(); j++)
   if (!patient.InternalBleedAt(j).Repaired) pain += settings.Get("painInternalUnits");
  return Math.Clamp(pain, 0, 100);
 }
 string History()
 {
  string result = "Recorded BCM medication:";
  if (m_Doses.IsEmpty()) return result + "\nNone still active";
  array<int> counts = {0, 0, 0, 0, 0, 0};
  array<float> latest = {1000000, 1000000, 1000000, 1000000, 1000000, 1000000};
  foreach (BCMM_MedicationDose dose : m_Doses)
  {
   counts[dose.Kind] = counts[dose.Kind] + 1;
   latest[dose.Kind] = Math.Min(latest[dose.Kind], dose.Age);
  }
  for (int kind = 0; kind < BCMM_SupplyCatalog.MEDICATION_COUNT; kind++)
   if (counts[kind] > 0) result += "\n" + Name(kind) + " x" + counts[kind].ToString() + " / latest " + Math.Floor(latest[kind]).ToString() + " s";
  return result;
 }
}
