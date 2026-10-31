// Original integrated scenarios. All time advances run the production patient model.
class BCMM_LinkedClinicalChecks
{
 static void Run(notnull BCMM_DomainChecks suite)
 {
  BCMM_Settings settings=new BCMM_Settings();
  BCMM_Patient untreated=new BCMM_Patient();untreated.AddWound(BCMM_BodyRegion.LEFT_ARM,12);untreated.AddWound(BCMM_BodyRegion.RIGHT_LEG,8);
  float blood=1;
  for(int t=0;t<240;t++)
  {
   blood=Math.Max(0,blood-untreated.AdvanceTrauma(1,BCMM_Activity.RESTING,settings)/settings.Get("normalBloodVolumeMl"));
   untreated.AdvancePhysiology(1,blood,settings);
  }
  suite.Record(blood<0.1 && !untreated.HasSpontaneousCirculation,"Untreated multiple wounds drain reserve and produce arrest");
  suite.Record(!untreated.IsShockAdvised(),"Blood-loss arrest requires bleeding control and resuscitation rather than AED shock");
  untreated.AdvancePhysiology(400,blood,settings);suite.Record(untreated.Terminal,"Uncorrected blood-loss arrest reaches terminal injury");

  BCMM_Patient treated=new BCMM_Patient();int arm=treated.AddWound(BCMM_BodyRegion.LEFT_ARM,12);int leg=treated.AddWound(BCMM_BodyRegion.RIGHT_LEG,8);
  float treatedBlood=1;
  for(int second=0;second<80;second++) { treatedBlood-=treated.AdvanceTrauma(1,BCMM_Activity.RESTING,settings)/5000;treated.AdvancePhysiology(1,treatedBlood,settings); }
  suite.Record(treated.HasSpontaneousCirculation && treated.HeartRate>75 && treated.SystolicPressure<90,"Moderate haemorrhage links compensatory pulse to falling pressure");
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(treated,BCMM_MedicationKind.MORPHINE,settings).Contains("pressure"),"Hypotension blocks further opioid even when pulse is fast");
  suite.Record(!BCMM_ClinicalSafety.MedicationDenial(treated,BCMM_MedicationKind.TXA,settings).IsEmpty(),"TXA remains subject to its configured vascular access requirement");
  treated.Equipment.VascularAccess=true;treated.Medication.Administer(BCMM_MedicationKind.TXA,settings);treated.AdvancePhysiology(30,treatedBlood,settings);
  suite.Record(treated.Medication.TXAEffect>0.9 && treated.TotalExternalFlowMlPerSecond()==20,"TXA does not replace physical control of open external wounds");
  treated.FindWound(arm).Dress(settings,0);treated.FindWound(leg).Dress(settings,0);
  suite.Record(treated.TotalExternalFlowMlPerSecond()==0,"Separate pressure dressings control both external bleeding sites");
  int internal=treated.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN,BCMM_BleedCompartment.ABDOMEN,5);
  float internalLoss=treated.AdvanceInternalTrauma(1,BCMM_Activity.RESTING,settings);
  suite.Record(internalLoss>0 && internalLoss<5 && !treated.FindInternalBleed(internal).Repaired,"TXA partially supports internal clot stability without repairing the source");
  BCMM_ProcedureCapabilities surgeon=new BCMM_ProcedureCapabilities();surgeon.SurgeryKit=true;surgeon.QualifiedSurgeon=true;surgeon.AtMedicalFacility=true;
  suite.Record(treated.RepairInternalBleeding(internal,surgeon,settings),"Definitive surgery can repair the remaining internal source");
  suite.Record(treated.AdvanceTrauma(60,BCMM_Activity.RESTING,settings)==0,"After external control and internal repair, ongoing blood loss stops");
  suite.Record(BCMM_ClinicalSafety.FluidDenial(treated,treatedBlood,settings).IsEmpty(),"Controlled haemorrhagic patient with access and deficit remains fluid eligible");
  treated.AdvancePhysiology(2,1,settings);
  suite.Record(treated.HasSpontaneousCirculation && treated.HeartRate<90 && treated.SystolicPressure>110,"Restoring reserve after source control improves pulse and pressure");
  suite.Record(BCMM_ClinicalSafety.FluidDenial(treated,1,settings).Contains("full"),"Full reserve rejects an unnecessary further infusion");

  BCMM_Patient opioid=new BCMM_Patient();opioid.AddWound(BCMM_BodyRegion.LEFT_ARM,1);
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,BCMM_MedicationKind.MORPHINE,settings).IsEmpty(),"Stable painful patient can receive morphine");
  opioid.Medication.Administer(BCMM_MedicationKind.MORPHINE,settings);opioid.AdvancePhysiology(60,1,settings);
  suite.Record(opioid.OxygenSaturation>0.96 && opioid.RespiratoryRate>=10 && opioid.HasSpontaneousCirculation,"One ordinary opioid dose does not automatically cause severe hypoxia");
  opioid.HeartRate=40;suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,0,settings).Contains("pulse"),"Low pulse independently blocks more morphine");
  opioid.HeartRate=75;opioid.RespiratoryRate=6;suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,0,settings).Contains("breathing"),"Slow breathing independently blocks more morphine");
  opioid.RespiratoryRate=16;opioid.OxygenSaturation=0.8;suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,0,settings).Contains("oxygen"),"Low saturation independently blocks more morphine");
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,1,settings).IsEmpty(),"Naloxone remains available for active opioid effects");
  BCMM_Patient healthy=new BCMM_Patient();suite.Record(!BCMM_ClinicalSafety.MedicationDenial(healthy,1,settings).IsEmpty(),"Naloxone is not a general purpose trauma cure");
  healthy.HeartRate=180;suite.Record(BCMM_ClinicalSafety.MedicationDenial(healthy,2,settings).Contains("pulse"),"Epinephrine is blocked at the configured high pulse boundary");
  healthy.HeartRate=75;healthy.SystolicPressure=180;suite.Record(BCMM_ClinicalSafety.MedicationDenial(healthy,2,settings).Contains("pressure"),"Epinephrine is blocked at the configured pressure boundary");
  healthy.Equipment.VascularAccess=true;suite.Record(BCMM_ClinicalSafety.FluidDenial(healthy,0.8,settings).Contains("pressure"),"High pressure rejects fluid despite an apparent reserve deficit");
  healthy.HasSpontaneousCirculation=false;healthy.Rhythm=BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;suite.Record(BCMM_ClinicalSafety.MedicationDenial(healthy,2,settings).IsEmpty(),"Resuscitation epinephrine does not mistake pulseless arrest for high-rate spontaneous circulation");

  BCMM_Settings risky=new BCMM_Settings();risky.Set("medicationRepeatSeconds",0);risky.Set("epinephrineOnsetSeconds",1);risky.Set("epinephrineHeartRateBoost",100);risky.Set("epinephrinePressureBoost",0);
  BCMM_Patient overdose=new BCMM_Patient();overdose.Medication.Administer(2,risky);overdose.Medication.Administer(2,risky);overdose.Medication.Administer(2,risky);
  overdose.AdvancePhysiology(1,1,risky);
  suite.Record(overdose.HeartRate>220 && overdose.PerfusionFraction<0.6 && overdose.HasSpontaneousCirculation,"Extreme stimulant tachycardia first impairs filling rather than instantly killing");
  overdose.AdvancePhysiology(16,1,risky);
  suite.Record(!overdose.HasSpontaneousCirculation && overdose.Rhythm==BCMM_CardiacRhythm.PULSELESS_VENTRICULAR_TACHYCARDIA && overdose.IsShockAdvised(),"Sustained unstable tachycardia can deteriorate into shockable pulseless VT");
  overdose.DeliverShock();overdose.StartCompressions(2,3);overdose.DeliverAssistedVentilation(10);overdose.AdvancePhysiology(10,1,risky);
  suite.Record(!overdose.HasSpontaneousCirculation,"A shock and CPR cannot ignore an ongoing unstable drug effect");
  BCMM_Settings lowRest=new BCMM_Settings();lowRest.Set("normalHeartRate",50);BCMM_Patient low=new BCMM_Patient();low.AdvancePhysiology(120,1,lowRest);
  suite.Record(low.HasSpontaneousCirculation && low.SystolicPressure>110,"A low stable resting pulse is not treated as arrest");

  BCMM_Patient healedInjury=new BCMM_Patient();healedInjury.Equipment.VascularAccess=true;int closed=healedInjury.AddWound(BCMM_BodyRegion.LEFT_ARM,1);healedInjury.FindWound(closed).Dress(settings,0);healedInjury.FindWound(closed).CloseWithSutures();
  suite.Record(!BCMM_ClinicalSafety.MedicationDenial(healedInjury,BCMM_MedicationKind.TXA,settings).IsEmpty(),"Completely closed injuries do not falsely indicate TXA");
  healedInjury.SystolicPressure=-1;suite.Record(BCMM_ClinicalSafety.MedicationDenial(healedInjury,0,settings).Contains("invalid"),"Invalid physiological values cannot pass medication comparisons");
  BCMM_SettingsDocument schema21=new BCMM_SettingsDocument();schema21.Schema=21;schema21.Generation=1;BCMM_Settings twentyOneSource=new BCMM_Settings();
  for(int priorKey=0;priorKey<168;priorKey++) { schema21.Keys.Insert(twentyOneSource.Definition(priorKey).Key);schema21.Values.Insert(twentyOneSource.ValueAt(priorKey)); }
  BCMM_Settings twentyTwo=new BCMM_Settings();suite.Record(schema21.Decode(twentyTwo) && twentyTwo.Get("naturalBloodRegenerationEnabled")==0,"Schema twenty-one preserves existing controls and gains explicit passive blood policy");
  BCMM_SettingsDocument prior=new BCMM_SettingsDocument();prior.Schema=20;prior.Generation=2;
  BCMM_Settings defaults=new BCMM_Settings();for(int i=0;i<144;i++) { prior.Keys.Insert(defaults.Definition(i).Key);prior.Values.Insert(defaults.ValueAt(i)); }
  prior.Values[defaults.IndexOf("morphineOnsetSeconds")]=45;
  BCMM_Settings upgraded=new BCMM_Settings();suite.Record(prior.Decode(upgraded) && upgraded.Get("morphineOnsetSeconds")==45 && upgraded.Get("epinephrineMaximumHeartRate")==180,"Schema twenty preserves existing choices and adds clinical safeguards");
 }
}