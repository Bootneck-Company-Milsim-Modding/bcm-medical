// Actual native JSON round trips and strict corruption boundaries for optional stock state.
class BCMM_NativeSalineCodecChecks
{
 static bool Read(string json, out BCMM_SavedNativeSaline restored)
 {
  SCR_PersistenceJsonLoadContext load = new SCR_PersistenceJsonLoadContext();
  return load.LoadFromString(json) && BCMM_PatientSaveValidation.StateShapeValid(load, "saline") && load.ReadValue("saline", restored);
 }
 static void Run(BCMM_DomainChecks suite)
 {
  SCR_PersistenceJsonLoadContext absentOptional = new SCR_PersistenceJsonLoadContext();
  suite.Record(absentOptional.LoadFromString("{}") && absentOptional.CanSeekMembers() && !BCMM_PatientSaveValidation.HasState(absentOptional, "bcmmNativeSaline"), "Seekable native JSON preserves genuinely absent optional clinical data");
  SCR_PersistenceJsonLoadContext scalarOptional = new SCR_PersistenceJsonLoadContext();
  bool scalarLoaded = scalarOptional.LoadFromString("{\"bcmmNativeSaline\":5}");
  suite.Record(scalarLoaded && scalarOptional.CanSeekMembers() && BCMM_PatientSaveValidation.HasState(scalarOptional, "bcmmNativeSaline") && !BCMM_PatientSaveValidation.StateShapeValid(scalarOptional, "bcmmNativeSaline"), "Seekable native JSON rejects a present scalar before invoking the native class reader");
  BCMM_SavedNativeSaline state = new BCMM_SavedNativeSaline();
  state.SourceId = "01234567-89ab-cdef-0123-456789abcdef";
  state.LifeSeconds = 45;
  state.EffectSeconds = 30;
  state.DPS = -1;
  SCR_PersistenceJsonSaveContext save = new SCR_PersistenceJsonSaveContext();
  BCMM_SavedNativeSaline restored;
  suite.Record(save.WriteValue("saline", state) && Read(save.SaveToString(), restored) && restored != state && restored.SourceId == state.SourceId && restored.LifeSeconds == 45 && restored.EffectSeconds == 30 && restored.DPS == -1, "Actual native stock-state codec preserves original UUID and remaining simulation/effect time");
  string prefix = "{\"saline\":{\"$type\":\"BCMM_SavedNativeSaline\",\"schema\":1,\"source\":\"01234567-89ab-cdef-0123-456789abcdef\",";
  suite.Record(Read(prefix + "\"life\":45,\"effect\":0,\"dps\":0}}", restored) && restored.EffectSeconds == 0, "Inactive original stock source retains only its remaining lifetime");
  suite.Record(!Read(prefix + "\"life\":-1,\"effect\":0,\"dps\":0}}", restored), "Negative saved stock lifetime is rejected by actual codec");
  suite.Record(!Read(prefix + "\"life\":45,\"effect\":50,\"dps\":-1}}", restored), "Stock effect cannot exceed original remaining source lifetime");
  suite.Record(!Read(prefix + "\"life\":45,\"effect\":20,\"dps\":1}}", restored), "Positive damage cannot masquerade as saved stock fluid delivery");
  suite.Record(!Read(prefix + "\"life\":45,\"effect\":20}}", restored), "Incomplete stock state cannot silently supply a default delivery rate");
  suite.Record(!Read("{\"saline\":{\"$type\":\"BCMM_SavedNativeSaline\",\"schema\":2,\"source\":\"01234567-89ab-cdef-0123-456789abcdef\",\"life\":45,\"effect\":20,\"dps\":-1}}", restored), "Unknown stock-state schema cannot silently resume delivery");
  suite.Record(!Read("{\"saline\":{\"$type\":\"BCMM_SavedNativeSaline\",\"schema\":1,\"source\":\"synthetic\",\"life\":45,\"effect\":20,\"dps\":-1}}", restored), "Noncanonical stock source identity cannot create a pending restoration");
 }
}


