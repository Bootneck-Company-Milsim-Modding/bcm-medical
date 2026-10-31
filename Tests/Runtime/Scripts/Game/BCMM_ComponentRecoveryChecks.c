// Deterministic acceptance of opt-in component recovery and legacy settings.
class BCMM_ComponentRecoveryChecks
{
 static BCMM_Patient Patient(float volume=1)
 {
  BCMM_Patient patient=new BCMM_Patient();patient.Fluids.LastVolume=volume;patient.Fluids.RedCellReserve=0.4;patient.Fluids.ClottingReserve=0.4;patient.Fluids.TransientWater=0.05;return patient;
 }
 static void Run(BCMM_DomainChecks suite)
 {
  BCMM_Settings settings=new BCMM_Settings();BCMM_Patient patient=Patient();patient.LeftLeg.Broken=true;
  suite.Record(settings.Count()==279 && settings.Get("componentRecoveryEnabled")==0 && settings.Get("naturalBloodRegenerationEnabled")==0,"Component recovery and native passive blood-volume regeneration both remain disabled by default");
  patient.BCMM_AdvanceComponentRecovery(900,1,true,settings);suite.Record(patient.Fluids.RedCellReserve==0.4 && patient.Fluids.ClottingReserve==0.4,"Default-disabled component recovery leaves both reserves unchanged");
  settings.Set("componentRecoveryEnabled",1);suite.Record(!patient.BCMM_CanRecoverComponents(1,false,settings) && patient.BCMM_CanRecoverComponents(1,true,settings),"Enabled recovery requires actual configured facility availability for an otherwise stable patient");
  patient.BCMM_AdvanceComponentRecovery(900,1,true,settings);float red=0.4+0.6*(1-Math.Pow(2.718281828,-0.25));float clot=0.4+0.6*(1-Math.Pow(2.718281828,-1));
  suite.Record(Math.AbsFloat(patient.Fluids.RedCellReserve-red)<0.00001 && Math.AbsFloat(patient.Fluids.ClottingReserve-clot)<0.00001 && clot>red,"Independent red-cell and clotting recovery follow their exact configured game time constants");
  suite.Record(patient.Fluids.LastVolume==1 && patient.Fluids.TransientWater==0.05 && patient.LeftLeg.Broken,"Component recovery neither adds volume, removes fluid redistribution nor repairs fractures");
  BCMM_Patient partitioned=Patient();for(int part=0;part<300;part++)partitioned.BCMM_AdvanceComponentRecovery(3,1,true,settings);
  suite.Record(Math.AbsFloat(partitioned.Fluids.RedCellReserve-red)<0.0001 && Math.AbsFloat(partitioned.Fluids.ClottingReserve-clot)<0.0001,"Recovery is stable across equivalent tick partitions");
  float held=patient.Fluids.RedCellReserve;patient.BCMM_AdvanceComponentRecovery(900,1,false,settings);suite.Record(patient.Fluids.RedCellReserve==held,"Leaving a required facility pauses existing recovery");
  settings.Set("componentRecoveryRequiresFacility",0);patient.BCMM_AdvanceComponentRecovery(30,1,false,settings);suite.Record(patient.Fluids.RedCellReserve>held,"Explicit field-recovery policy resumes progression without a facility");settings.Set("componentRecoveryRequiresFacility",1);
  patient=Patient();suite.Record(!patient.BCMM_CanRecoverComponents(0.79,true,settings),"Inadequate circulating volume cannot be bypassed by component recovery");
  patient.SystolicPressure=89;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Insufficient systolic pressure pauses component recovery");patient.SystolicPressure=120;
  patient.OxygenSaturation=0.1;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Severe hypoxaemia prevents stabilised component recovery");patient.OxygenSaturation=0.98;
  patient.RespiratoryRate=0;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Absent spontaneous breathing prevents component recovery");patient.RespiratoryRate=16;
  patient.LeftChest.Tension=true;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Unresolved tension prevents component recovery");patient.LeftChest.Tension=false;
  patient.HasSpontaneousCirculation=false;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Cardiac arrest cannot recover components instead of receiving resuscitation");patient.HasSpontaneousCirculation=true;
  patient.Terminal=true;suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Recovery never revives a terminal patient");patient.Terminal=false;
  int wound=patient.AddWound(BCMM_BodyRegion.LEFT_ARM,1);suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"Ongoing external bleeding blocks component recovery");patient.FindWound(wound).Sutured=true;
  suite.Record(patient.BCMM_CanRecoverComponents(1,true,settings),"Definitive external bleeding control permits stable recovery again");
  patient.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN,BCMM_BleedCompartment.ABDOMEN,1);suite.Record(!patient.BCMM_CanRecoverComponents(1,true,settings),"An unrepaired internal source blocks component recovery");patient.InternalBleedAt(0).Repaired=true;suite.Record(patient.BCMM_CanRecoverComponents(1,true,settings),"Definitive internal repair restores recovery eligibility without discarding pooled blood");
  BCMM_FluidBalance invalid=new BCMM_FluidBalance();invalid.RedCellReserve=-0.1;invalid.BCMM_RecoverComposition(30,1,60,30);suite.Record(invalid.RedCellReserve==-0.1 && invalid.ClottingReserve==1,"Malformed component state cannot be silently repaired by the recovery path");
  patient.BCMM_AdvanceComponentRecovery(-1,1,true,settings);patient.BCMM_AdvanceComponentRecovery(3601,1,true,settings);suite.Record(patient.Fluids.RedCellReserve==0.4,"Invalid or unbounded elapsed time cannot grant component recovery");
  settings.Set("fluidCompositionEnabled",0);patient.BCMM_AdvanceComponentRecovery(900,1,true,settings);suite.Record(patient.Fluids.RedCellReserve==0.4,"Legacy shared-reserve mode does not secretly run component recovery");settings.Set("fluidCompositionEnabled",1);
  BCMM_Patient partial=Patient(0.8);partial.BCMM_AdvanceComponentRecovery(3600,0.8,true,settings);suite.Record(partial.Fluids.RedCellReserve>0.4 && partial.Fluids.RedCellReserve<0.8 && partial.Fluids.ClottingReserve<=0.8 && partial.Fluids.LastVolume==0.8,"Recovery targets supported existing volume and cannot refill a partial circulation");
  suite.Record(!settings.Set("redCellRecoverySeconds",0) && !settings.Set("clottingRecoverySeconds",0),"Zero recovery time constants fail settings validation");
  BCMM_SettingsDocument old=new BCMM_SettingsDocument();old.Schema=30;old.Generation=1;for(int index=0;index<254;index++){old.Keys.Insert(settings.Definition(index).Key);old.Values.Insert(settings.ValueAt(index));}old.Values[settings.IndexOf("maskScale")]=1.25;
  BCMM_Settings migrated=new BCMM_Settings();suite.Record(old.Decode(migrated) && migrated.Count()==279 && migrated.Get("maskScale")==1.25 && migrated.Get("componentRecoveryEnabled")==0 && migrated.Get("componentRecoveryMinimumSystolicPressure")==90,"Schema thirty preserves old controls and migrates to six conservative component-recovery defaults");
  old.Keys.Set(253,"componentRecoveryEnabled");old.Values.Set(253,1);suite.Record(!old.Decode(new BCMM_Settings()),"Schema thirty cannot replace a required old key with a new recovery control");
  BCMM_SettingsDocument current=new BCMM_SettingsDocument();current.Schema=31;current.Generation=1;for(int field=0;field<260;field++){current.Keys.Insert(settings.Definition(field).Key);current.Values.Insert(settings.ValueAt(field));}
  suite.Record(current.Decode(new BCMM_Settings()),"Historical schema thirty-one settings migrate with independent drip defaults");current.Keys.Set(current.Keys.Find("componentRecoveryMinimumSystolicPressure"),"requireBodyDisposition");current.Values.Set(current.Keys.Find("requireBodyDisposition"),1);
  suite.Record(!current.Decode(new BCMM_Settings()),"Schema thirty-one cannot replace a required recovery key with a retired body-bag respawn key");
 }
}
