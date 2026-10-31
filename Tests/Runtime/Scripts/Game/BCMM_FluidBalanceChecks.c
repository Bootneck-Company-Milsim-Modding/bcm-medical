// Original fluid composition and tissue-delivery acceptance scenarios.
class BCMM_FluidBalanceChecks
{
 static void Run(notnull BCMM_DomainChecks suite)
 {
  BCMM_Settings settings=new BCMM_Settings();
  BCMM_FluidBalance saline=new BCMM_FluidBalance();
  BCMM_FluidBalance plasma=new BCMM_FluidBalance();
  BCMM_FluidBalance blood=new BCMM_FluidBalance();
  saline.ObserveBlood(0.5);plasma.ObserveBlood(0.5);blood.ObserveBlood(0.5);
  suite.Record(Math.AbsFloat(saline.RedCellReserve-0.5)<0.0001 && Math.AbsFloat(saline.ClottingReserve-0.5)<0.0001,"Haemorrhage removes red cells and clotting reserve with lost volume");
  saline.RecordDelivery(0,0.5,0.7,settings);plasma.RecordDelivery(1,0.5,0.7,settings);blood.RecordDelivery(2,0.5,0.7,settings);
  suite.Record(Math.AbsFloat(saline.RedCellReserve-0.5)<0.0001 && Math.AbsFloat(saline.ClottingReserve-0.5)<0.0001,"Saline replaces volume without inventing red cells or clotting factors");
  suite.Record(Math.AbsFloat(plasma.RedCellReserve-0.5)<0.0001 && Math.AbsFloat(plasma.ClottingReserve-0.7)<0.0001,"Plasma supports clotting reserve without replacing red cells");
  suite.Record(Math.AbsFloat(blood.RedCellReserve-0.7)<0.0001 && Math.AbsFloat(blood.ClottingReserve-0.7)<0.0001,"Whole blood restores volume, red cells and clotting reserve");
  suite.Record(saline.BleedingMultiplier(settings)>plasma.BleedingMultiplier(settings) && saline.DressingWearMultiplier(settings)>blood.DressingWearMultiplier(settings),"Dilution worsens residual bleeding and dressing wear rather than creating wounds");
  float redBefore=saline.RedCellReserve;float clotBefore=saline.ClottingReserve;
  float removed=saline.Redistribute(30,0.7,settings);
  suite.Record(removed>0 && removed<0.15 && saline.RedCellReserve==redBefore && saline.ClottingReserve==clotBefore,"Water redistribution lowers retained volume without discarding cellular or clotting reserves");
  float afterWater=saline.LastVolume;saline.ObserveBlood(afterWater*0.5);
  suite.Record(Math.AbsFloat(saline.RedCellReserve-redBefore*0.5)<0.0001 && Math.AbsFloat(saline.ClottingReserve-clotBefore*0.5)<0.0001,"Subsequent haemorrhage removes diluted components proportionally");
  suite.Record(plasma.TransientWater<0.05 && blood.TransientWater==0,"Plasma retention differs from saline and whole blood in the configurable game model");

  BCMM_Patient anaemic=new BCMM_Patient();anaemic.Fluids.ObserveBlood(0.12);anaemic.Fluids.RecordDelivery(0,0.12,0.9,settings);
  float volume=0.9;
  for(int t=0;t<5;t++) { volume-=anaemic.AdvancePhysiology(1,volume,settings); }
  suite.Record(anaemic.OxygenSaturation>0.97 && anaemic.HeartRate>100 && anaemic.SystolicPressure>90 && anaemic.OxygenDeliveryFraction<0.15,"Normal saturation and improved pressure cannot conceal critically depleted oxygen-carrying reserve");
  for(int arrest=0;arrest<20;arrest++) { volume-=anaemic.AdvancePhysiology(1,volume,settings); }
  suite.Record(!anaemic.HasSpontaneousCirculation && !anaemic.IsShockAdvised(),"Persistent critically poor oxygen delivery can progress to non-shockable arrest");
  float retainedRed=anaemic.Fluids.RedCellReserve;
  anaemic.StartCompressions(2,3);anaemic.DeliverAssistedVentilation(10);volume-=anaemic.AdvancePhysiology(10,volume,settings);
  suite.Record(!anaemic.HasSpontaneousCirculation && Math.AbsFloat(anaemic.Fluids.RedCellReserve-retainedRed)<0.0001,"CPR and ventilation do not manufacture missing red cells or bypass the reversible cause");

  anaemic.Fluids.RecordDelivery(2,volume,1,settings);volume=1;
  for(int recovery=0;recovery<25;recovery++) { anaemic.DeliverAssistedVentilation(2);volume-=anaemic.AdvancePhysiology(1,volume,settings); }
  anaemic.Fluids.RecordDelivery(2,volume,1,settings);volume=1;
  for(int furtherRecovery=0;furtherRecovery<25;furtherRecovery++) { anaemic.DeliverAssistedVentilation(2);volume-=anaemic.AdvancePhysiology(1,volume,settings); }
  PrintFormat("BCMM_FLUID_RECOVERY pulse=%1 red=%2 delivery=%3 terminal=%4",anaemic.HeartRate,anaemic.Fluids.RedCellReserve,anaemic.OxygenDeliveryFraction,anaemic.Terminal);
  suite.Record(anaemic.HasSpontaneousCirculation && anaemic.HeartRate>0 && anaemic.OxygenDeliveryFraction>=settings.Get("recoveryOxygenDeliveryFraction"),"Correcting oxygen-carrying reserve with blood enables sustained CPR and ventilation to achieve ROSC");
  BCMM_Patient replacement=new BCMM_Patient();replacement.Fluids.ObserveBlood(0.5);replacement.Fluids.RecordDelivery(2,0.5,0.7,settings);replacement.AdvancePhysiology(10,0.7,settings);
  BCMM_Patient dilution=new BCMM_Patient();dilution.Fluids.ObserveBlood(0.5);dilution.Fluids.RecordDelivery(0,0.5,0.7,settings);dilution.AdvancePhysiology(10,0.7,settings);
  suite.Record(replacement.OxygenSaturation>0.97 && dilution.OxygenSaturation>0.97 && replacement.OxygenDeliveryFraction>dilution.OxygenDeliveryFraction+0.1,"Equivalent saturation does not make saline and whole blood equivalent oxygen transport");
  suite.Record(dilution.HeartRate>replacement.HeartRate,"Red-cell depletion continues to drive compensatory pulse after volume replacement");
  BCMM_Settings legacy=new BCMM_Settings();legacy.Set("fluidCompositionEnabled",0);BCMM_FluidBalance disabled=new BCMM_FluidBalance();disabled.ObserveBlood(0.5);disabled.RecordDelivery(0,0.5,0.7,legacy);
  suite.Record(Math.AbsFloat(disabled.RedCellReserve-0.7)<0.0001 && disabled.RedCellConcentration(legacy)==1 && disabled.Redistribute(60,0.7,legacy)==0,"Disabling composition retains the configurable legacy volume-only abstraction");
  BCMM_SettingsDocument old=new BCMM_SettingsDocument();old.Schema=22;old.Generation=1;
  for(int key=0;key<169;key++) { old.Keys.Insert(settings.Definition(key).Key);old.Values.Insert(settings.ValueAt(key)); }
  BCMM_Settings upgraded=new BCMM_Settings();suite.Record(old.Decode(upgraded) && upgraded.Count()==settings.Count() && upgraded.Get("fluidCompositionEnabled")==1,"Schema twenty-two migrates all existing controls and adds typed-fluid defaults");
  suite.Record(BCMM_EquipmentPolicy.Kind(BCMM_EquipmentProcedure.INFUSE_SALINE)==BCMM_EquipmentKind.SALINE && BCMM_EquipmentPolicy.Kind(BCMM_EquipmentProcedure.INFUSE_PLASMA)==BCMM_EquipmentKind.PLASMA,"Typed infusions require their distinct physical supply types");
  Resource salineItem=Resource.Load("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et");Resource plasmaItem=Resource.Load("{B6D83F6853C68D73}Prefabs/Items/BCMM_Plasma.et");
  suite.Record(salineItem && salineItem.IsValid() && plasmaItem && plasmaItem.IsValid() && BCMM_SupplyCatalog.COUNT==25,"Distinct saline and plasma inventory prefabs load into the expanded supply catalogue");
 }
}
