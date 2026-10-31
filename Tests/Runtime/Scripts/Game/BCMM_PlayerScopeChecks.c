// Requires the native diagnostic runner. Not executed during recording.
class BCMM_PlayerScopeChecks
{
 static void Run(BCMM_DomainChecks suite)
 {
  BCMM_Settings defaults = new BCMM_Settings();
  suite.Record(defaults.Count() == 279 && defaults.Get("fieldProcedureProgressOnly") == 1, "Progress-only field procedures default enabled with one appended GM control");
  BCMM_SettingsDocument old = new BCMM_SettingsDocument();
  old.Schema = 33;
  old.Generation = 1;
  for (int index = 0; index < 270; index++)
  {
   old.Keys.Insert(defaults.Definition(index).Key);
   old.Values.Insert(defaults.ValueAt(index));
  }
  old.Values[defaults.IndexOf("totalLives")] = 7;
  old.Values[defaults.IndexOf("splintApplicationSeconds")] = 45;
  BCMM_Settings migrated = new BCMM_Settings();
  suite.Record(old.Decode(migrated) && migrated.Get("totalLives") == 7 && migrated.Get("splintApplicationSeconds") == 45 && migrated.Get("fieldProcedureProgressOnly") == 1, "Schema33 retains saved player medical/life values and appends progress-only default");
  old.Keys.Remove(0);
  old.Values.Remove(0);
  suite.Record(!old.Decode(new BCMM_Settings()), "Truncated schema33 cannot be accepted as a valid migration");
  BCMM_SettingsDocument current = new BCMM_SettingsDocument();
  current.Schema = 35;
  current.Generation = 2;
  for (int field = 0; field < defaults.Count(); field++)
  {
   current.Keys.Insert(defaults.Definition(field).Key);
   current.Values.Insert(defaults.ValueAt(field));
  }
  current.Values[defaults.IndexOf("fieldProcedureProgressOnly")] = 0;
  BCMM_Settings restored = new BCMM_Settings();
  suite.Record(current.Decode(restored) && restored.Get("fieldProcedureProgressOnly") == 0, "Schema35 can preserve an explicit legacy-presentation selection");
  current.Values[defaults.IndexOf("fieldProcedureProgressOnly")] = 0.5;
  suite.Record(!current.Decode(new BCMM_Settings()), "Progress-only saved toggle rejects fractional corrupt data");
  Resource hud = Resource.Load("{7ED896AAC4412D30}UI/layouts/BCMM/ProcedureHUD.layout");
  suite.Record(hud && hud.IsValid(), "Owner treatment-progress HUD resource loads independently of medical menu");
  BCMM_SettingsDocument prior34=new BCMM_SettingsDocument();prior34.Schema=34;prior34.Generation=1;
  for(int oldField=0;oldField<271;oldField++){prior34.Keys.Insert(defaults.Definition(oldField).Key);prior34.Values.Insert(defaults.ValueAt(oldField));}
  prior34.Values[defaults.IndexOf("totalLives")]=7;prior34.Values[defaults.IndexOf("fieldProcedureProgressOnly")]=0;
  BCMM_Settings migrated34=new BCMM_Settings();
  suite.Record(prior34.Decode(migrated34) && migrated34.Count()==279 && migrated34.Get("totalLives")==7 && migrated34.Get("fieldProcedureProgressOnly")==0 && migrated34.Get("weaponRetentionPolicy")==1 && migrated34.Get("casualtyCarryEnabled")==1,"Complete schema34 preserves existing choices and appends sling/transport defaults");
  prior34.Keys.Remove(0);prior34.Values.Remove(0);
  suite.Record(!prior34.Decode(new BCMM_Settings()),"Truncated schema34 cannot silently become a valid player settings profile");
 }
}
