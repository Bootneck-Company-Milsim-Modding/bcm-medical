// Staged deterministic invariants for the optional normalized burden model.
class BCMM_FluidBurdenChecks
{
 static void Run(BCMM_DomainChecks suite)
 {
  BCMM_Settings settings=new BCMM_Settings();
  suite.Record(settings.Count()==279 && settings.Get("fluidOverloadEnabled")==0,"Optional burden appends seven controls and leaves overload disabled by default");
  BCMM_Patient disabled=new BCMM_Patient();
  BCMM_FluidBurdenState state=new BCMM_FluidBurdenState();
  state.Restore(0.4);
  disabled.BCMM_RestoreFluidBurden(state);
  settings.Set("fluidOverloadStopSeverity",0);
  suite.Record(disabled.LungCapacity(settings)==1 && !disabled.BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,settings),"Disabled overload cannot impair lungs or withhold fluids even at a zero configured stop severity");
  settings.Set("fluidOverloadStopSeverity",0.5);
  settings.Set("fluidOverloadEnabled",1);
  suite.Record(Math.AbsFloat(disabled.LungCapacity(settings)-0.65)<0.0001 && !disabled.LeftChest.Tension && !disabled.RightChest.Tension && disabled.LeftChest.PleuralBloodMl==0 && disabled.RightChest.PleuralBloodMl==0,"Systemic burden reduces remaining lung capacity without inventing tension or pleural blood");
  disabled.LeftChest.CollapseFraction=0.5;
  suite.Record(Math.AbsFloat(disabled.LungCapacity(settings)-0.4875)<0.0001,"Burden compounds structural lung loss without overwriting the original injury");
  BCMM_FluidBurdenState full=new BCMM_FluidBurdenState();
  full.Restore(0.4);
  BCMM_FluidBurdenState poor=new BCMM_FluidBurdenState();
  poor.Restore(0.4);
  BCMM_FluidBurdenState arrest=new BCMM_FluidBurdenState();
  arrest.Restore(0.4);
  full.Advance(1800,0,1,settings);
  poor.Advance(1800,0,0.5,settings);
  arrest.Advance(1800,0,0,settings);
  suite.Record(Math.AbsFloat(full.LoadFraction()-0.1471518)<0.0001 && poor.LoadFraction()>full.LoadFraction() && arrest.LoadFraction()==0.4,"Active clearance follows its timescale, slows with poor perfusion and awards none during arrest");
  BCMM_FluidBurdenState one=new BCMM_FluidBurdenState();
  one.Restore(0.2);
  BCMM_FluidBurdenState many=new BCMM_FluidBurdenState();
  many.Restore(0.2);
  one.Advance(600,0.1,1,settings);
  for(int tick=0;tick<2400;tick++)
  {
   many.Advance(0.25,0.1/2400,1,settings);
  }
  suite.Record(Math.AbsFloat(one.LoadFraction()-many.LoadFraction())<0.0005,"Equivalent active-time input and clearance do not depend materially on tick partitioning");
  float held=one.LoadFraction();
  suite.Record(!one.Advance(-1,0,1,settings) && !one.Advance(3601,0,1,settings) && !one.Advance(1,-1,1,settings) && !one.Advance(1,0,1.1,settings) && !one.Restore(-1) && one.LoadFraction()==held,"Invalid elapsed time, perfusion, input or saved load cannot silently heal or increase burden");
  BCMM_Patient normal=new BCMM_Patient();
  BCMM_Patient impaired=new BCMM_Patient();
  impaired.BCMM_RestoreFluidBurden(state);
  normal.AdvancePhysiology(20,1,settings);
  float removed=impaired.AdvancePhysiology(20,1,settings);
  suite.Record(impaired.OxygenSaturation<normal.OxygenSaturation-0.1 && impaired.RespiratoryRate>normal.RespiratoryRate && !impaired.Terminal && impaired.HasSpontaneousCirculation,"Optional burden acts through the existing gradual oxygenation/respiratory model rather than an instant arrest or death trigger");
  suite.Record(removed==0 && impaired.Fluids.LastVolume==1 && impaired.Fluids.RedCellReserve==1 && impaired.Fluids.ClottingReserve==1,"Clearing systemic burden neither refills native circulating volume nor creates cells or clotting reserve");
  BCMM_Patient oxygen=new BCMM_Patient();
  BCMM_Patient unsupported=new BCMM_Patient();
  oxygen.BCMM_RestoreFluidBurden(state);
  unsupported.BCMM_RestoreFluidBurden(state);
  oxygen.OxygenSaturation=0.7;
  unsupported.OxygenSaturation=0.7;
  oxygen.Equipment.OxygenActive=true;
  oxygen.AdvancePhysiology(20,1,settings);
  unsupported.AdvancePhysiology(20,1,settings);
  suite.Record(oxygen.OxygenSaturation>unsupported.OxygenSaturation+0.04 && Math.AbsFloat(oxygen.BCMM_FluidBurdenFraction()-unsupported.BCMM_FluidBurdenFraction())<0.0001,"Oxygen support improves delivery without erasing the underlying saved burden");
  BCMM_Patient redistributed=new BCMM_Patient();
  redistributed.Fluids.TransientWater=0.2;
  float water=redistributed.AdvancePhysiology(30,1,settings);
  suite.Record(water>0 && redistributed.BCMM_FluidBurdenFraction()>0 && redistributed.BCMM_FluidBurdenFraction()<=water && redistributed.Fluids.LastVolume<1,"Actual typed-fluid redistribution is recorded as separate burden rather than silently disappearing from all state");
  BCMM_Patient gate=new BCMM_Patient();
  gate.Equipment.VascularAccess=true;
  gate.BCMM_RestoreFluidBurden(state);
  suite.Record(gate.BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,settings) && gate.BCMM_WithholdAdditionalWater(BCMM_FluidKind.PLASMA,settings) && !gate.BCMM_WithholdAdditionalWater(BCMM_FluidKind.WHOLE_BLOOD,settings),"Water-loading policy distinguishes saline/plasma from whole blood");
  suite.Record(!BCMM_ClinicalSafety.FluidDenial(gate,0.7,settings,BCMM_FluidKind.SALINE).IsEmpty() && BCMM_ClinicalSafety.FluidDenial(gate,0.7,settings,BCMM_FluidKind.WHOLE_BLOOD).IsEmpty(),"Shared delivery policy withholds further water while preserving ordinary whole-blood access/volume/pressure guards");
  settings.Set("fluidSafetyChecksEnabled",0);
  suite.Record(!gate.BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,settings),"Explicit GM fluid-safety override remains effective");
  settings.Set("fluidSafetyChecksEnabled",1);
  settings.Set("fluidCompositionEnabled",0);
  suite.Record(gate.LungCapacity(settings)==1 && !gate.BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,settings),"Legacy shared-reserve mode cannot silently retain typed overload effects");
  settings.Set("fluidCompositionEnabled",1);
  suite.Record(!settings.Set("fluidOverloadThresholdFraction",0.5) && !settings.Set("fluidOverloadSevereFraction",0.1),"Conflicting live burden threshold pairs are rejected atomically");
  BCMM_SettingsDocument old=new BCMM_SettingsDocument();
  old.Schema=32;
  old.Generation=1;
  for(int field=0;field<263;field++)
  {
   old.Keys.Insert(settings.Definition(field).Key);
   old.Values.Insert(settings.ValueAt(field));
  }
  old.Values[settings.IndexOf("fluidLineConnectionSeconds")]=12;
  BCMM_Settings migrated=new BCMM_Settings();
  suite.Record(old.Decode(migrated) && migrated.Count()==279 && migrated.Get("fluidLineConnectionSeconds")==12 && migrated.Get("fluidOverloadEnabled")==0,"Schema32 retains independent-line controls and appends conservative optional burden defaults");
  BCMM_SettingsDocument current=new BCMM_SettingsDocument();
  current.Schema=35;
  current.Generation=1;
  for(int index=0;index<settings.Count();index++)
  {
   current.Keys.Insert(settings.Definition(index).Key);
   current.Values.Insert(settings.ValueAt(index));
  }
  suite.Record(current.Decode(new BCMM_Settings()),"Complete schema34 settings validate");
  current.Values[current.Keys.Find("fluidOverloadThresholdFraction")]=0.5;
  current.Values[current.Keys.Find("fluidOverloadSevereFraction")]=0.4;
  suite.Record(!current.Decode(new BCMM_Settings()),"A complete but contradictory saved burden pair cannot bypass document validation");
 }
}
