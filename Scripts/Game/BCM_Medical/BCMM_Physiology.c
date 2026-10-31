// Original deterministic gameplay physiology. Fractions are normalised game
// state, not a clinical prediction. The server supplies native blood reserve.
modded class BCMM_Patient
{
 float OxygenSaturation = 0.98;
 float HeartRate = 75;
 float SystolicPressure = 120;
 float RespiratoryRate = 16;
 float PerfusionFraction = 1;
 float OxygenDeliveryFraction=0.98;
 float HypoxicDebtSeconds;
 protected float m_CriticalExposure;
 protected float m_UnstableVitalExposure;
 protected float m_CorrectedExposure;
 protected float m_VentilationRemaining;
 protected int m_ShocksDelivered;

 int ShocksDelivered() { return m_ShocksDelivered; }
 bool DeliverAssistedVentilation(float seconds)
 {
  if (!CanVentilate() || !(seconds > 0) || seconds > 10) return false;
  m_VentilationRemaining = Math.Min(10, m_VentilationRemaining + seconds);
  return true;
 }
 bool DeliverShock()
 {
  if (!IsShockAdvised()) return false;
  m_ShocksDelivered++;
  Rhythm = BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;
  m_CorrectedExposure = 0;
  return true;
 }
 float LungCapacity(notnull BCMM_Settings settings)
 {
  float left = 1 - Math.Clamp(LeftChest.CollapseFraction, 0, 1);
  float right = 1 - Math.Clamp(RightChest.CollapseFraction, 0, 1);
  if (LeftChest.OpenWound && !LeftChest.Sealed) left *= 0.5;
  if (RightChest.OpenWound && !RightChest.Sealed) right *= 0.5;
  left *= 1 - Math.Clamp(LeftChest.PleuralBloodMl / (settings.Get("normalBloodVolumeMl") * 0.3), 0, 1);
  right *= 1 - Math.Clamp(RightChest.PleuralBloodMl / (settings.Get("normalBloodVolumeMl") * 0.3), 0, 1);
  return Math.Clamp((left + right) * 0.5, 0, 1);
 }
 float AdvancePhysiology(float seconds, float bloodFraction, notnull BCMM_Settings settings)
 {
  if (Terminal || !(seconds > 0) || !(bloodFraction >= 0 && bloodFraction <= 1)) return 0;
  // Fixed upper step bounds avoid skipping deterioration/recovery transitions.
  Fluids.ObserveBlood(bloodFraction);float volume=bloodFraction;float redistributed;
  float remaining = Math.Min(seconds, 3600);
  while (remaining > 0)
  {
   float step = Math.Min(remaining, 0.25);
   float water=Fluids.Redistribute(step,volume,settings);volume=Math.Max(0,volume-water);redistributed+=water;
   float clearancePerfusion=0;
   if(HasSpontaneousCirculation){clearancePerfusion=Math.Clamp(PerfusionFraction,0,1);}
   BCMM_AdvanceFluidBurden(step,water,clearancePerfusion,settings);
   AdvancePhysiologyStep(step, volume, settings);
   remaining -= step;
   if (Terminal) return redistributed;
  }
  return redistributed;
 }
 protected void AdvancePhysiologyStep(float seconds,float blood,notnull BCMM_Settings settings)
 {
  Equipment.Advance(seconds,this,settings);Medication.Advance(seconds,this,settings);
  bool ventilated=m_VentilationRemaining>0;
  m_VentilationRemaining=Math.Max(0,m_VentilationRemaining-seconds);
  float spontaneousRate=settings.Get("normalRespiratoryRate")*(1+(1-OxygenSaturation))*Medication.BreathingScale;
  if(Airway!=BCMM_AirwayCondition.PATENT) { spontaneousRate=0; }
  float ventilation=Math.Clamp(spontaneousRate/settings.Get("adequateRespiratoryRate"),0,1);
  float oxygenTarget;
  if(Airway==BCMM_AirwayCondition.PATENT && (HasSpontaneousCirculation || ventilated))
  {
   float lung=LungCapacity(settings);
   if(Equipment.OxygenActive && settings.Get("oxygenEnabled")>0.5 && lung>=settings.Get("oxygenMinimumLungFraction")) { lung+=settings.Get("oxygenLungCompensation")*(1-lung); }
   oxygenTarget=0.98*lung;
   if(!ventilated) { oxygenTarget*=ventilation; }
  }
  float oxygenStep=settings.Get("oxygenChangePerSecond")*seconds;
  OxygenSaturation=Math.Clamp(OxygenSaturation+Math.Clamp(oxygenTarget-OxygenSaturation,-oxygenStep,oxygenStep),0,1);

  // Compensation has finite reserve: late shock and profound hypoxia can slow the heart.
  float oxygenReserve=blood*Fluids.RedCellConcentration(settings);
  float candidateRate=settings.Get("normalHeartRate")+(1-oxygenReserve)*settings.Get("compensatoryHeartRateBoost");
  candidateRate+=Medication.Pain*0.01*settings.Get("painHeartRateBoost");
  candidateRate+=Medication.StimulantEffect*settings.Get("epinephrineHeartRateBoost");
  candidateRate*=Math.Clamp(blood/settings.Get("shockDecompensationBloodFraction"),0,1);
  candidateRate*=Math.Clamp(OxygenSaturation/settings.Get("unconsciousOxygenFraction"),0,1);
  float pumping=Math.Clamp(candidateRate/settings.Get("minimumEfficientHeartRate"),0,1);
  pumping*=Math.Clamp(settings.Get("maximumEfficientHeartRate")/Math.Max(settings.Get("maximumEfficientHeartRate"),candidateRate),0,1);
  float tension=1;
  if(LeftChest.Tension) { tension*=0.35; }
  if(RightChest.Tension) { tension*=0.35; }
  float spontaneousPerfusion=Math.Clamp(blood*tension*pumping,0,1);
  float pressureGain=settings.Get("normalSystolicPressure")*Medication.PressureScale+Medication.StimulantEffect*settings.Get("epinephrinePressureBoost");
  float candidatePressure=pressureGain*spontaneousPerfusion;

  float spontaneousDelivery=OxygenSaturation*spontaneousPerfusion*Fluids.RedCellConcentration(settings);
  bool critical=spontaneousDelivery<=settings.Get("criticalOxygenDeliveryFraction") || blood<=settings.Get("arrestBloodFraction") || OxygenSaturation<=settings.Get("arrestOxygenFraction");
  if(critical) { m_CriticalExposure+=seconds; } else { m_CriticalExposure=0; }
  bool slow=candidateRate<=settings.Get("criticalBradycardiaRate");
  bool fast=candidateRate>=settings.Get("criticalTachycardiaRate");
  bool unstableRate=(slow || fast) && candidatePressure<settings.Get("criticalRatePressureCeiling");
  bool unstable=unstableRate || candidatePressure<=settings.Get("criticalSystolicPressure");
  if(HasSpontaneousCirculation && unstable) { m_UnstableVitalExposure+=seconds; } else { m_UnstableVitalExposure=0; }
  if(HasSpontaneousCirculation && (m_CriticalExposure>=settings.Get("arrestOnsetSeconds") || m_UnstableVitalExposure>=settings.Get("criticalVitalExposureSeconds")))
  {
   HasSpontaneousCirculation=false;Rhythm=BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;
   if(OxygenSaturation<=settings.Get("arrestOxygenFraction") || slow) { Rhythm=BCMM_CardiacRhythm.ASYSTOLE; }
   else if(fast) { Rhythm=BCMM_CardiacRhythm.PULSELESS_VENTRICULAR_TACHYCARDIA; }
  }
  PerfusionFraction=spontaneousPerfusion;
  if(!HasSpontaneousCirculation)
  {
   PerfusionFraction=0;
   if(HasCompressions()) { PerfusionFraction=settings.Get("cprPerfusionFraction")*blood*tension; }
  }
  // A monitor rhythm, stimulant or shock is never a substitute for corrected causes.
  bool corrected=!critical && !unstable && spontaneousDelivery>=settings.Get("recoveryOxygenDeliveryFraction") && blood>settings.Get("arrestBloodFraction")+0.1 && OxygenSaturation>=settings.Get("unconsciousOxygenFraction") && !LeftChest.Tension && !RightChest.Tension;
  if(!HasSpontaneousCirculation && !IsShockAdvised() && corrected && HasCompressions()) { m_CorrectedExposure+=seconds; } else { m_CorrectedExposure=0; }
  if(m_CorrectedExposure>=settings.Get("roscStabilitySeconds"))
  {
   HasSpontaneousCirculation=true;Rhythm=BCMM_CardiacRhythm.SINUS;m_CompressionActor=0;m_CompressionSession=0;m_CorrectedExposure=0;PerfusionFraction=spontaneousPerfusion;
  }
  float oxygenDelivery=OxygenSaturation*PerfusionFraction*Fluids.RedCellConcentration(settings);OxygenDeliveryFraction=oxygenDelivery;
  if(oxygenDelivery<0.3) { HypoxicDebtSeconds+=seconds*(1-oxygenDelivery/0.3); }
  else { HypoxicDebtSeconds=Math.Max(0,HypoxicDebtSeconds-seconds*0.25); }
  SystolicPressure=pressureGain*PerfusionFraction;HeartRate=0;RespiratoryRate=0;
  if(HasSpontaneousCirculation) { HeartRate=candidateRate;RespiratoryRate=spontaneousRate; }
  if(HypoxicDebtSeconds>=settings.Get("hypoxicDebtLimitSeconds")) { MarkTerminal(); }
 }
}
