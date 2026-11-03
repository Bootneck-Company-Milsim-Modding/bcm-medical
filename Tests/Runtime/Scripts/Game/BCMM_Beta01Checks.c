// Diagnostic-only evidence. These are settingsInstance assertions, not visual acceptance.
class BCMM_Beta01Checks
{
 static void RunIfRequested(BCMM_DomainChecks suite)
 {
  string enabled;
  if (System.GetCLIParam("bcmmBeta01State", enabled)) RunStateCycles(suite);
 }
 static void CheckSettingCycle(BCMM_DomainChecks suite, BCMM_SettingDefinition entry, int iteration)
 {
  BCMM_Settings values = new BCMM_Settings();
  float candidate = entry.DefaultValue;
  if (iteration % 4 == 1) candidate = entry.Minimum;
  if (iteration % 4 == 2) candidate = entry.Maximum;
  if (iteration % 4 == 3) candidate = entry.Minimum + Math.Floor((entry.Maximum-entry.Minimum)/entry.Step/2)*entry.Step;
  bool expected = Compatible(values, entry.Key, candidate);
  bool accepted = values.Set(entry.Key, candidate);
  suite.Record(accepted == expected, "BT01 native setting apply/expected rejection");
  float readback = entry.DefaultValue;
  if (expected) readback = candidate;
  suite.Record(Math.AbsFloat(values.Get(entry.Key)-readback)<0.0001, "BT01 native setting readback");
  suite.Record(!values.Set(entry.Key,entry.Maximum+entry.Step), "BT01 upper boundary rejection");
  suite.Record(Math.AbsFloat(values.Get(entry.Key)-readback)<0.0001, "BT01 rejected value leaves state unchanged");
 }
 static bool Compatible(BCMM_Settings values, string key, float candidate)
 {
  if(key=="bandageReopenMinSeconds") return candidate<=values.Get("bandageReopenMaxSeconds");
  if(key=="bandageReopenMaxSeconds") return candidate>=values.Get("bandageReopenMinSeconds");
  if(key=="ropeLengthMetres") return candidate<values.Get("ropeBreakDistanceMetres");
  if(key=="ropeBreakDistanceMetres") return candidate>values.Get("ropeLengthMetres");
  if(key=="fluidOverloadThresholdFraction") return candidate<values.Get("fluidOverloadSevereFraction");
  if(key=="fluidOverloadSevereFraction") return candidate>values.Get("fluidOverloadThresholdFraction");
  return true;
 }
 static void RunStateCycles(BCMM_DomainChecks suite)
 {
  BCMM_Settings defaults = new BCMM_Settings();
  for(int keyIndex=0;keyIndex<defaults.Count();keyIndex++)
  {
   BCMM_SettingDefinition entry = defaults.Definition(keyIndex);
   for(int iteration=0;iteration<200;iteration++) CheckSettingCycle(suite,entry,iteration);
   PrintFormat("BCMM_BETA_SETTING key=%1 attempts=200 stateAssertions=800 gameplayCycles=0",entry.Key);
  }
 }
 static void ExportNative()
 {
  FileIO.MakeDirectory("$profile:beta01-native");
  array<string> nativeUI={};FileIO.FindFiles(nativeUI.Insert,"scripts/Game/UI/",".c");
  foreach(string uiPath:nativeUI){string lower=uiPath;lower.ToLower();if(lower.Contains("hint") || lower.Contains("consum") || lower.Contains("gadget") || lower.Contains("contextual"))FileIO.CopyFile(uiPath,"$profile:beta01-native/"+FilePath.StripPath(uiPath));}
  array<string> paths = {
   "Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01_Base.et",
   "Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_01_Base.et",
   "Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01_base.et",
   "Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
   "Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
   "Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01_Base.et",
   "scripts/Game/UI/HUD/AvailableActions/Conditions/Game/Character/SCR_CharacterHasConsumableItemEquippedCondition.c",
   "scripts/Game/UI/HUD/AvailableActions/SCR_AvailableActionsDisplay.c",
   "scripts/Game/Components/Gadgets/SCR_ConsumableItemComponent.c",
   "scripts/Game/generated/Physics/Physics.c",
   "scripts/Game/generated/UserAction/ScriptedUserAction.c"
  };
  foreach (string path : paths) FileIO.CopyFile(path, "$profile:beta01-native/" + FilePath.StripPath(path));
  array<ResourceName> items = {
   "{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et",
   "{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
   "{6AB4B3F3FD216014}Prefabs/Items/BCMM_Epinephrine.et"
  };
  foreach (ResourceName item : items)
  {
   EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = "300 3 0";
   IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(item), GetGame().GetWorld(), spawn);
   if (!entity) continue;
   vector low, high; entity.GetWorldBounds(low, high);
   PrintFormat("BCMM_BETA_GEOMETRY item=%1 low=%2 high=%3 dimensions=%4", item, low - entity.GetOrigin(), high - entity.GetOrigin(), high - low);
   SCR_EntityHelper.DeleteEntityAndChildren(entity);
  }
 }
}
