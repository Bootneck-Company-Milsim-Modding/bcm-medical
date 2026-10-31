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
  if (Get("bandageReopenMinSeconds") > Get("bandageReopenMaxSeconds") || Get("ropeBreakDistanceMetres") <= Get("ropeLengthMetres") || Get("fluidOverloadThresholdFraction")>=Get("fluidOverloadSevereFraction"))
  {
   m_Values[index] = old;
   return false;
  }
  return true;
 }
 // Import the server's complete snapshot atomically: related limits may change together.
 bool ImportReplicatedValues(notnull array<float> values)
 {
  if (values.Count() != Count()) return false;
  for (int i=0;i<Count();i++) if (!m_Definitions[i].Accepts(values[i])) return false;
  if (values[IndexOf("bandageReopenMinSeconds")] > values[IndexOf("bandageReopenMaxSeconds")] || values[IndexOf("ropeBreakDistanceMetres")] <= values[IndexOf("ropeLengthMetres")] || values[IndexOf("fluidOverloadThresholdFraction")]>=values[IndexOf("fluidOverloadSevereFraction")]) return false;
  m_Values.Copy(values); return true;
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
 // Removed body-bag respawn controls are validated only when importing their
 // historical schemas. They never become active settings in the current mod.
 protected bool AcceptRetiredKey(string key,float value)
 {
  if(Schema<27 || Schema>29 || value!=Math.Round(value))return false;
  if(key=="requireBodyDisposition")return value>=0 && value<=1;
  if(key=="dispositionTimeoutSeconds")return value>=0 && value<=86400;
  if(Schema>=28 && key=="respawnRequestsEnabled")return value>=0 && value<=1;
  if(Schema>=28 && key=="respawnRequestCooldownSeconds")return value>=1 && value<=86400;
  if(Schema==29 && key=="spectatorWhileWaitingEnabled")return value>=0 && value<=1;
  return false;
 }
 bool Decode(notnull BCMM_Settings settings)
 {
  if (Schema < 1 || Schema > 35) return false;
  if (Generation <= 0 || !Keys || !Values) return false;
  if (Keys.Count() != Values.Count()) return false;
  if (Schema == 1 && Keys.Count() != 27) return false;
  if (Schema == 2 && Keys.Count() != 29) return false;
  if (Schema == 3 && Keys.Count() != 41) return false;
  if (Schema == 4 && Keys.Count() != 43) return false;
  if (Schema == 5 && Keys.Count() != 46) return false;
  if (Schema == 6 && Keys.Count() != 49) return false;
  if (Schema == 7 && Keys.Count() != 50) return false;
  if (Schema == 8 && Keys.Count() != 52) return false;
  if (Schema == 9 && Keys.Count() != 55) return false;
  if (Schema == 10 && Keys.Count() != 56) return false;
  if (Schema == 11 && Keys.Count() != 77) return false;
  if (Schema == 12 && Keys.Count() != 78) return false;
  if (Schema == 13 && Keys.Count() != 96) return false;
  if (Schema == 14 && Keys.Count() != 103) return false;
  if (Schema == 15 && Keys.Count() != 105) return false;
  if (Schema == 16 && Keys.Count() != 111) return false;
  if (Schema == 16) { for (int sixteenth = 0; sixteenth < 111; sixteenth++) if (!Keys.Contains(settings.Definition(sixteenth).Key)) return false; }
  if (Schema == 17 && Keys.Count() != 115) return false;
  if (Schema == 17) { for (int seventeenth = 0; seventeenth < 115; seventeenth++) if (!Keys.Contains(settings.Definition(seventeenth).Key)) return false; }
  if (Schema == 18 && Keys.Count() != 123) return false;
  if (Schema == 18) { for (int eighteenth = 0; eighteenth < 123; eighteenth++) if (!Keys.Contains(settings.Definition(eighteenth).Key)) return false; }
  if (Schema == 19 && Keys.Count() != 136) return false;
  if (Schema == 19) { for (int nineteenth=0;nineteenth<136;nineteenth++) if (!Keys.Contains(settings.Definition(nineteenth).Key)) return false; }
  if (Schema == 20 && Keys.Count() != 144) return false;
  if (Schema == 20) { for(int twentieth=0;twentieth<144;twentieth++) if(!Keys.Contains(settings.Definition(twentieth).Key)) return false; }
  if (Schema == 21 && Keys.Count() != 168) return false;
  if (Schema == 21) { for(int twentyFirst=0;twentyFirst<168;twentyFirst++) if(!Keys.Contains(settings.Definition(twentyFirst).Key)) return false; }
  if (Schema == 22 && Keys.Count() != 169) return false;
  if (Schema == 22) { for(int twentySecond=0;twentySecond<169;twentySecond++) if(!Keys.Contains(settings.Definition(twentySecond).Key)) return false; }
  if (Schema == 23 && Keys.Count() != 177) return false;
  if (Schema == 23) { for(int previous=0;previous<177;previous++) if(!Keys.Contains(settings.Definition(previous).Key)) return false; }
  if (Schema == 24 && Keys.Count() != 210) return false;
  if (Schema == 24) { for(int prior=0;prior<210;prior++) if(!Keys.Contains(settings.Definition(prior).Key)) return false; }
  if (Schema == 25 && Keys.Count() != 250) return false;
  if (Schema == 25) { for(int priorMedical=0;priorMedical<250;priorMedical++) if(!Keys.Contains(settings.Definition(priorMedical).Key)) return false; }
  if (Schema == 26 && Keys.Count() != 253) return false;
  if (Schema == 26) { for(int priorBurial=0;priorBurial<253;priorBurial++) if(!Keys.Contains(settings.Definition(priorBurial).Key)) return false; }
  if (Schema == 27 && Keys.Count() != 256) return false;
  if (Schema == 28 && Keys.Count() != 258) return false;
  if (Schema == 29 && Keys.Count() != 259) return false;
  if (Schema == 30 && Keys.Count() != 254) return false;
  if (Schema == 31 && Keys.Count() != 260) return false;
  if (Schema == 32 && Keys.Count() != 263) return false;
  if (Schema == 33 && Keys.Count() != 270) return false;
  if (Schema == 34 && Keys.Count() != 271) return false;
  if (Schema == 35 && Keys.Count() != settings.Count()) return false;
  if (Schema >= 27)
  {
   int requiredCount=254;if(Schema==31)requiredCount=260;if(Schema==32)requiredCount=263;if(Schema==33)requiredCount=270;if(Schema==34)requiredCount=271;if(Schema>=35)requiredCount=settings.Count();
   for(int current=0;current<requiredCount;current++) if(!Keys.Contains(settings.Definition(current).Key)) return false;
   if(Schema<=29 && (!Keys.Contains("requireBodyDisposition") || !Keys.Contains("dispositionTimeoutSeconds")))return false;
   if(Schema>=28 && Schema<=29 && (!Keys.Contains("respawnRequestsEnabled") || !Keys.Contains("respawnRequestCooldownSeconds")))return false;
   if(Schema==29 && !Keys.Contains("spectatorWhileWaitingEnabled"))return false;
  }
  if (Schema == 15) { for (int fifteenth = 0; fifteenth < 105; fifteenth++) if (!Keys.Contains(settings.Definition(fifteenth).Key)) return false; }
  if (Schema == 14) { for (int fourteenth = 0; fourteenth < 103; fourteenth++) if (!Keys.Contains(settings.Definition(fourteenth).Key)) return false; }
  if (Schema == 13) { for (int thirteenth = 0; thirteenth < 96; thirteenth++) if (!Keys.Contains(settings.Definition(thirteenth).Key)) return false; }
  if (Schema == 12) { for (int twelfth = 0; twelfth < 78; twelfth++) if (!Keys.Contains(settings.Definition(twelfth).Key)) return false; }
  if (Schema == 11) { for (int eleventh = 0; eleventh < 77; eleventh++) if (!Keys.Contains(settings.Definition(eleventh).Key)) return false; }
  if (Schema == 10) { for (int tenth = 0; tenth < 56; tenth++) if (!Keys.Contains(settings.Definition(tenth).Key)) return false; }
  array<string> seen = {};
  // Apply in two passes so a valid minimum/maximum pair can both move upward
  // or downward without an intermediate cross-field validation failure.
  foreach (int index, string key : Keys)
  {
   int target = settings.IndexOf(key);
   if(seen.Contains(key))return false;
   if(target<0)
   {
    if(!AcceptRetiredKey(key,Values[index]))return false;
   }
   else if(!settings.Definition(target).Accepts(Values[index]))return false;
   seen.Insert(key);
  }
  if (Schema == 1) { for (int legacy = 0; legacy < 27; legacy++) if (!seen.Contains(settings.Definition(legacy).Key)) return false; }
  if (Schema == 2) { for (int previous = 0; previous < 29; previous++) if (!seen.Contains(settings.Definition(previous).Key)) return false; }
  if (Schema == 3) { for (int prior = 0; prior < 41; prior++) if (!seen.Contains(settings.Definition(prior).Key)) return false; }
  if (Schema == 4) { for (int fourth = 0; fourth < 43; fourth++) if (!seen.Contains(settings.Definition(fourth).Key)) return false; }
  if (Schema == 5) { for (int fifth = 0; fifth < 46; fifth++) if (!seen.Contains(settings.Definition(fifth).Key)) return false; }
  if (Schema == 6) { for (int sixth = 0; sixth < 49; sixth++) if (!seen.Contains(settings.Definition(sixth).Key)) return false; }
  if (Schema == 7) { for (int seventh = 0; seventh < 50; seventh++) if (!seen.Contains(settings.Definition(seventh).Key)) return false; }
  if (Schema == 8) { for (int eighth = 0; eighth < 52; eighth++) if (!seen.Contains(settings.Definition(eighth).Key)) return false; }
  if (Schema == 9) { for (int ninth = 0; ninth < 55; ninth++) if (!seen.Contains(settings.Definition(ninth).Key)) return false; }
  float minWear; float maxWear; float rope; float release;
  float burdenOnset=settings.Get("fluidOverloadThresholdFraction");float burdenSevere=settings.Get("fluidOverloadSevereFraction");
  foreach (int index, string key : Keys)
  {
   if(key=="fluidOverloadThresholdFraction"){burdenOnset=Values[index];}
   if(key=="fluidOverloadSevereFraction"){burdenSevere=Values[index];}
   if (key == "bandageReopenMinSeconds") minWear = Values[index];
   if (key == "bandageReopenMaxSeconds") maxWear = Values[index];
   if (key == "ropeLengthMetres") rope = Values[index];
   if (key == "ropeBreakDistanceMetres") release = Values[index];
  }
  if (minWear > maxWear || rope >= release || burdenOnset>=burdenSevere) return false;
  // Build a normalised vector in catalogue order after complete validation.
  foreach (int index, string key : Keys) if(settings.IndexOf(key)>=0) settings.SetUncheckedForDocument(key, Values[index]);
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
  document.Schema = 35;
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
