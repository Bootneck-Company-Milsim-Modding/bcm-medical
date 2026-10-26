// Original BCM server settings. Stable named keys, validated before use.
class BCMM_SettingDefinition
{
 string Key;
 float DefaultValue;
 float Minimum;
 float Maximum;
 float Step;
 bool Toggle;
 void BCMM_SettingDefinition(string key, float defaultValue, float minimum, float maximum, float step, bool toggle)
 {
  Key = key; DefaultValue = defaultValue; Minimum = minimum; Maximum = maximum; Step = step; Toggle = toggle;
 }
 bool Accepts(float value)
 {
  if (!(value >= Minimum && value <= Maximum)) return false;
  float steps = (value - Minimum) / Step;
  return Math.AbsFloat(steps - Math.Round(steps)) < 0.001;
 }
}

class BCMM_Settings
{
 protected ref array<ref BCMM_SettingDefinition> m_Definitions = {};
 protected ref array<float> m_Values = {};
 void BCMM_Settings()
 {
  BCMM_SettingsCatalog.Build(m_Definitions);
  foreach (BCMM_SettingDefinition definition : m_Definitions) m_Values.Insert(definition.DefaultValue);
 }
 int Count() { return m_Definitions.Count(); }
 BCMM_SettingDefinition Definition(int index) { return m_Definitions[index]; }
 float ValueAt(int index) { return m_Values[index]; }
 int IndexOf(string key)
 {
  foreach (int index, BCMM_SettingDefinition definition : m_Definitions)
   if (definition.Key == key) return index;
  return -1;
 }
 float Get(string key)
 {
  int index = IndexOf(key);
  if (index < 0) { Print("BCMM unknown setting: " + key, LogLevel.ERROR); return 0; }
  return m_Values[index];
 }
 bool Set(string key, float value)
 {
  int index = IndexOf(key);
  if (index < 0 || !m_Definitions[index].Accepts(value)) return false;
  float old = m_Values[index];
  m_Values[index] = value;
  if (Get("bandageReopenMinSeconds") > Get("bandageReopenMaxSeconds") || Get("ropeBreakDistanceMetres") <= Get("ropeLengthMetres"))
  {
   m_Values[index] = old;
   return false;
  }
  return true;
 }
 void CopyFrom(notnull BCMM_Settings other) { m_Values.Copy(other.m_Values); }
 void ExportValues(notnull array<float> output) { output.Copy(m_Values); }
}

// Both files are server-profile-local. A failed write to the inactive slot
// leaves the previous valid generation available. A reread must match before
// any GM change is applied to the running server.
class BCMM_SettingsDocument : JsonApiStruct
{
 int Schema;
 int Generation;
 ref array<string> Keys = {};
 ref array<float> Values = {};
 void BCMM_SettingsDocument()
 {
  RegV("Schema"); RegV("Generation"); RegV("Keys"); RegV("Values");
 }
 bool Decode(notnull BCMM_Settings settings)
 {
  if (Schema != 1 || Generation <= 0 || !Keys || !Values || Keys.Count() != Values.Count() || Keys.Count() != settings.Count()) return false;
  array<string> seen = {};
  // Apply in two passes so a valid minimum/maximum pair can both move upward
  // or downward without an intermediate cross-field validation failure.
  foreach (int index, string key : Keys)
  {
   int target = settings.IndexOf(key);
   if (target < 0 || seen.Contains(key) || !settings.Definition(target).Accepts(Values[index])) return false;
   seen.Insert(key);
  }
  float minWear; float maxWear; float rope; float release;
  foreach (int index, string key : Keys)
  {
   if (key == "bandageReopenMinSeconds") minWear = Values[index];
   if (key == "bandageReopenMaxSeconds") maxWear = Values[index];
   if (key == "ropeLengthMetres") rope = Values[index];
   if (key == "ropeBreakDistanceMetres") release = Values[index];
  }
  if (minWear > maxWear || rope >= release) return false;
  // Build a normalised vector in catalogue order after complete validation.
  foreach (int index, string key : Keys) settings.SetUncheckedForDocument(key, Values[index]);
  return true;
 }
}

// Internal deserialisation seam: never call this from player/GM commands.
modded class BCMM_Settings
{
 void SetUncheckedForDocument(string key, float value) { m_Values[IndexOf(key)] = value; }
}

class BCMM_SettingsStore
{
 protected string m_Directory;
 protected int m_Generation;
 protected bool m_Writable = true;
 void BCMM_SettingsStore(string directory = "$profile:BCM_Medical") { m_Directory = directory; }
 bool IsWritable() { return m_Writable; }
 int Generation() { return m_Generation; }
 protected string Slot(int generation)
 {
  if (generation % 2 == 0) return m_Directory + "/settings-a.json";
  return m_Directory + "/settings-b.json";
 }
 bool Load(notnull BCMM_Settings output)
 {
  bool present;
  bool loaded;
  m_Generation = 0;
  for (int slot = 0; slot < 2; slot++)
  {
   string path = Slot(slot);
   if (!FileIO.FileExists(path)) continue;
   present = true;
   JsonLoadContext syntaxCheck = new JsonLoadContext();
   if (!syntaxCheck.LoadFromFile(path)) continue;
   BCMM_SettingsDocument document = new BCMM_SettingsDocument();
   document.Keys = new array<string>();
   document.Values = new array<float>();
   BCMM_Settings candidate = new BCMM_Settings();
   bool parsed = document.LoadFromFile(path);
   bool decoded = parsed && document.Decode(candidate);
   PrintFormat("BCMM settings read path=%1 parsed=%2 valid=%3 generation=%4", path, parsed, decoded, document.Generation);
   if (!decoded) continue;
   if (document.Generation <= m_Generation) continue;
   output.CopyFrom(candidate);
   m_Generation = document.Generation;
   loaded = true;
  }
  m_Writable = !present || loaded;
  if (present && !loaded) Print("BCMM settings invalid: files preserved; edits locked until repaired.", LogLevel.ERROR);
  return loaded;
 }
 bool Save(notnull BCMM_Settings settings)
 {
  if (!Replication.IsServer() || !m_Writable || m_Generation >= 1000000000) return false;
  FileIO.MakeDirectory(m_Directory);
  BCMM_SettingsDocument document = new BCMM_SettingsDocument();
  document.Schema = 1;
  document.Generation = m_Generation + 1;
  for (int i = 0; i < settings.Count(); i++)
  {
   document.Keys.Insert(settings.Definition(i).Key);
   document.Values.Insert(settings.ValueAt(i));
  }
  string path = Slot(document.Generation);
  if (!document.PackToFile(path)) return false;
  JsonLoadContext syntaxCheck = new JsonLoadContext();
  if (!syntaxCheck.LoadFromFile(path)) return false;
  BCMM_SettingsDocument verify = new BCMM_SettingsDocument();
  verify.Keys = new array<string>(); verify.Values = new array<float>();
  BCMM_Settings verifiedSettings = new BCMM_Settings();
  if (!verify.LoadFromFile(path) || !verify.Decode(verifiedSettings) || verify.Generation != document.Generation) return false;
  for (int i = 0; i < settings.Count(); i++)
   if (Math.AbsFloat(settings.ValueAt(i) - verifiedSettings.ValueAt(i)) > 0.0001) return false;
  m_Generation = document.Generation;
  return true;
 }
}
