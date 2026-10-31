// Original coupled physiology and medication acceptance, using production classes.
class BCMM_OxygenMedicationChecks
{
 static void Run(notnull BCMM_DomainChecks suite)
 {
  BCMM_Settings settings=new BCMM_Settings();BCMM_Patient air=new BCMM_Patient();BCMM_Patient oxygen=new BCMM_Patient();
  air.LeftChest.CollapseFraction=0.8;oxygen.LeftChest.CollapseFraction=0.8;oxygen.Equipment.OxygenActive=true;
  air.AdvancePhysiology(25,1,settings);oxygen.AdvancePhysiology(25,1,settings);
  suite.Record(oxygen.OxygenSaturation>air.OxygenSaturation+0.15 && oxygen.LeftChest.CollapseFraction==0.8,"Oxygen improves saturation through remaining lung function without reinflating injury");
  suite.Record(oxygen.Fluids.RedCellReserve==1 && oxygen.Fluids.LastVolume==1,"Oxygen does not invent volume or oxygen-carrying cells");
  oxygen.Airway=BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;float before=oxygen.OxygenSaturation;oxygen.AdvancePhysiology(5,1,settings);
  suite.Record(oxygen.OxygenSaturation<before && oxygen.Airway!=BCMM_AirwayCondition.PATENT,"An obstructed airway defeats a connected oxygen mask");
  BCMM_Patient collapsed=new BCMM_Patient();collapsed.Equipment.OxygenActive=true;collapsed.LeftChest.CollapseFraction=1;collapsed.RightChest.CollapseFraction=1;collapsed.AdvancePhysiology(5,1,settings);
  suite.Record(collapsed.OxygenSaturation<0.95,"Oxygen cannot compensate for completely nonfunctioning lungs");
  BCMM_Settings noOxygen=new BCMM_Settings();noOxygen.Set("oxygenEnabled",0);BCMM_Patient disabled=new BCMM_Patient();disabled.LeftChest.CollapseFraction=0.8;disabled.Equipment.OxygenActive=true;disabled.AdvancePhysiology(25,1,noOxygen);
  suite.Record(Math.AbsFloat(disabled.OxygenSaturation-air.OxygenSaturation)<0.01,"Live oxygen-disable setting also removes its physiological benefit");
  BCMM_Patient tablets=new BCMM_Patient();tablets.AddWound(BCMM_BodyRegion.LEFT_ARM,2);tablets.AdvancePhysiology(1,1,settings);float untreated=tablets.Medication.Pain;
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(tablets,4,settings).IsEmpty(),"Stable patient with patent airway is eligible for oral medicine");
  tablets.Medication.Administer(4,settings);tablets.AdvancePhysiology(15,1,settings);float early=tablets.Medication.Pain;tablets.AdvancePhysiology(45,1,settings);
  suite.Record(early<untreated && tablets.Medication.Pain<early && tablets.Medication.BreathingScale==1 && tablets.Medication.PressureScale==1,"Tablet relief develops slowly without opioid breathing or pressure modifiers");
  suite.Record(!tablets.Medication.CanAdminister(4,settings),"Tablet-specific repeat limit applies independently of generic medication interval");
  tablets.Airway=BCMM_AirwayCondition.SOFT_TISSUE_OBSTRUCTION;suite.Record(!BCMM_ClinicalSafety.MedicationDenial(tablets,4,settings).IsEmpty(),"Unsafe swallowing conditions block oral administration");
  BCMM_Patient opioid=new BCMM_Patient();opioid.AddWound(BCMM_BodyRegion.LEFT_ARM,2);
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,5,settings).Contains("access"),"Fentanyl requires configured vascular access");opioid.Equipment.VascularAccess=true;
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,5,settings).IsEmpty(),"Stable patient with access passes stronger-opioid safeguards");
  opioid.Medication.Administer(5,settings);opioid.AdvancePhysiology(20,1,settings);
  suite.Record(opioid.Medication.Pain<5 && opioid.RespiratoryRate<12 && opioid.SystolicPressure<110,"Fast stronger opioid links analgesia to breathing and pressure costs");
  suite.Record(!BCMM_ClinicalSafety.MedicationDenial(opioid,5,settings).IsEmpty(),"Developing opioid depression prevents unsafe redosing");
  suite.Record(BCMM_ClinicalSafety.MedicationDenial(opioid,1,settings).IsEmpty(),"Naloxone recognises fentanyl-only exposure");
  opioid.Medication.Administer(1,settings);opioid.AdvancePhysiology(10,1,settings);suite.Record(opioid.Medication.BreathingScale>0.9 && opioid.Medication.Pain>10,"Naloxone reverses the stronger opioid and can remove its analgesia");
  BCMM_Patient residual=new BCMM_Patient();residual.Medication.Administer(5,settings);residual.AdvancePhysiology(205,1,settings);
  suite.Record(residual.Medication.ActiveDoses()==1 && residual.Medication.OpioidEffect>0 && residual.Medication.History().Contains("Fentanyl"),"Residual fentanyl exposure remains recorded after analgesic clearance");
  residual.AdvancePhysiology(120,1,settings);suite.Record(residual.Medication.ActiveDoses()==0 && residual.Medication.OpioidEffect==0,"Both fentanyl effects eventually clear without permanent depression");
  BCMM_Patient combined=new BCMM_Patient();combined.Medication.Administer(0,settings);combined.Medication.Administer(5,settings);combined.AdvancePhysiology(30,1,settings);
  suite.Record(combined.Medication.BreathingScale<0.3,"Morphine and fentanyl respiratory risks accumulate rather than overwrite one another");
  BCMM_SettingsDocument prior=new BCMM_SettingsDocument();prior.Schema=23;prior.Generation=1;for(int i=0;i<177;i++) { prior.Keys.Insert(settings.Definition(i).Key);prior.Values.Insert(settings.ValueAt(i)); }
  BCMM_Settings upgraded=new BCMM_Settings();suite.Record(prior.Decode(upgraded) && upgraded.Count()==settings.Count() && upgraded.Get("oxygenEnabled")==1 && upgraded.Get("fentanylMedicOnly")==1,"Schema twenty-three retains existing values and gains oxygen and medication defaults");
  suite.Record(BCMM_EquipmentPolicy.RegionAllowed(18,BCMM_BodyRegion.HEAD) && !BCMM_EquipmentPolicy.RegionAllowed(18,BCMM_BodyRegion.LEFT_ARM),"Oxygen actions are scoped to the head");
 }
}
