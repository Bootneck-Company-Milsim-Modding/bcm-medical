// Public saved-profile compatibility and malformed-input boundary acceptance.
class BCMM_FluidLineChecks
{
 static void Run(BCMM_DomainChecks suite)
 {
  BCMM_Settings defaults=new BCMM_Settings();
  suite.Record(defaults.Count()==279 && defaults.Get("continuousFluidLinesEnabled")==1 && defaults.Get("fluidLineConnectionSeconds")==8 && defaults.Get("fluidLineRemovalSeconds")==3,"Independent line controls append under existing circulation with explicit connection/removal timers");
  BCMM_SettingsDocument old=new BCMM_SettingsDocument();
  old.Schema=31;
  old.Generation=7;
  for(int index=0;index<260;index++)
  {
   old.Keys.Insert(defaults.Definition(index).Key);
   old.Values.Insert(defaults.ValueAt(index));
  }
  old.Values[defaults.IndexOf("redCellRecoverySeconds")]=7200;
  old.Values[defaults.IndexOf("totalLives")]=7;
  BCMM_Settings migrated=new BCMM_Settings();
  suite.Record(old.Decode(migrated) && migrated.Get("redCellRecoverySeconds")==7200 && migrated.Get("totalLives")==7 && migrated.Get("continuousFluidLinesEnabled")==1,"Actual historical schema31 profile retains recovery/lives policy while receiving the new independent-line defaults");
  old.Keys.Set(259,"continuousFluidLinesEnabled");
  old.Values.Set(259,1);
  suite.Record(!old.Decode(new BCMM_Settings()),"Historical profile cannot replace a required recovery control with a future line control");
  BCMM_SettingsDocument current=new BCMM_SettingsDocument();
  current.Schema=32;
  current.Generation=1;
  for(int field=0;field<263;field++)
  {
   current.Keys.Insert(defaults.Definition(field).Key);
   current.Values.Insert(defaults.ValueAt(field));
  }
  suite.Record(current.Decode(new BCMM_Settings()),"Historical schema32 profile validates with overload defaults");
  int removal=current.Keys.Find("fluidLineRemovalSeconds");
  current.Values.Set(removal,0);
  suite.Record(!current.Decode(new BCMM_Settings()),"A saved zero-duration line removal is rejected");
  current.Values.Set(removal,3);
  current.Keys.Set(removal,"requireBodyDisposition");
  suite.Record(!current.Decode(new BCMM_Settings()),"Schema32 cannot reactivate a removed body-bag respawn control instead of its required line setting");
  BCMM_SavedFluidLine state=new BCMM_SavedFluidLine();
  state.SourceId="00000000-0000-4000-8000-000000000001";
  state.AccessRegion=BCMM_BodyRegion.LEFT_ARM;
  suite.Record(state.Valid(),"Line binding accepts a canonical original source UUID and valid access limb");
  state.AccessRegion=BCMM_BodyRegion.HEAD;
  suite.Record(!state.Valid(),"Saved line cannot attach to a nonvascular region");
  state.AccessRegion=BCMM_BodyRegion.LEFT_ARM;
  state.SourceId="missing-device";
  suite.Record(!state.Valid(),"Saved line rejects malformed source identity instead of inventing a replacement");
  state.SourceId="00000000-0000-4000-8000-000000000001";
  state.Schema=2;
  suite.Record(!state.Valid(),"Unknown future line-binding schema fails closed");
 }
}
