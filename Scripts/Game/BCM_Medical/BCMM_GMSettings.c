// Original native GM adapter. The editor manager identifies the authenticated
// actor; an arbitrary player ID in an attribute payload is never trusted.
modded class SCR_BaseGameMode
{
 protected ref BCMM_Settings m_BCMMSettings = new BCMM_Settings();
 protected ref BCMM_SettingsStore m_BCMMSettingsStore;
 [RplProp()] protected ref array<float> m_BCMMReplicatedSettings = {};
 [RplProp()] protected int m_BCMMSettingsGeneration;

 override void OnGameStart()
 {
  super.OnGameStart();
  if (!Replication.IsServer()) return;
  m_BCMMSettingsStore = new BCMM_SettingsStore();
  m_BCMMSettingsStore.Load(m_BCMMSettings);
  BCMM_PublishSettings();
 }
 protected void BCMM_PublishSettings()
 {
  m_BCMMSettings.ExportValues(m_BCMMReplicatedSettings);
  m_BCMMSettingsGeneration = m_BCMMSettingsStore.Generation();
  Replication.BumpMe();
 }
 float BCMM_ReadSetting(string key)
 {
  if (Replication.IsServer()) return m_BCMMSettings.Get(key);
  int index = m_BCMMSettings.IndexOf(key);
  if (m_BCMMReplicatedSettings.IsIndexValid(index)) return m_BCMMReplicatedSettings[index];
  return m_BCMMSettings.Get(key);
 }
 static bool BCMM_IsSettingsEditor(SCR_AttributesManagerEditorComponent manager)
 {
  if (!Replication.IsServer() || !manager) return false;
  SCR_EditorManagerEntity editor = manager.GetManager();
  if (!editor || editor.GetPlayerID() <= 0 || editor.IsLimited()) return false;
  SCR_EditorModeEntity editMode = editor.FindModeEntity(EEditorMode.EDIT);
  return editMode && !editMode.IsLimited();
 }
 protected void BCMM_SettingsFeedback(SCR_AttributesManagerEditorComponent manager, string message)
 {
  if (!manager || !manager.GetManager()) return;
  int actor = manager.GetManager().GetPlayerID();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(actor));
  if (player) player.BCMM_SendSettingsResult(message);
 }
 bool BCMM_ChangeSetting(string key, float value, SCR_AttributesManagerEditorComponent manager)
 {
  if (!BCMM_IsSettingsEditor(manager) || !m_BCMMSettingsStore) return false;
  BCMM_Settings candidate = new BCMM_Settings();
  candidate.CopyFrom(m_BCMMSettings);
  if (!candidate.Set(key, value)) { BCMM_SettingsFeedback(manager, "BCM Medical: invalid value or conflicting range; change not saved."); return false; }
  if (!m_BCMMSettingsStore.Save(candidate))
  {
   BCMM_SettingsFeedback(manager, "BCM Medical: SAVE FAILED. Server settings were not changed. Check profile access and server logs.");
   Print("BCMM settings SAVE FAILED; change was not applied: " + key, LogLevel.ERROR);
   return false;
  }
  m_BCMMSettings.CopyFrom(candidate);
  BCMM_SettingsFeedback(manager, "BCM Medical: saved on server.");
  BCMM_PublishSettings();
  PrintFormat("BCMM settings saved key=%1 value=%2 generation=%3", key, value, m_BCMMSettingsGeneration);
  return true;
 }
}

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class BCMM_NumberSettingAttribute : SCR_BaseValueListEditorAttribute
{
 [Attribute()] protected string m_Key;
 override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(item);
  if (!mode || !IsGameMode(item)) return null;
  return SCR_BaseEditorAttributeVar.CreateFloat(mode.BCMM_ReadSetting(m_Key));
 }
 override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(item);
  if (mode && var) mode.BCMM_ChangeSetting(m_Key, var.GetFloat(), manager);
 }
}

[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class BCMM_ToggleSettingAttribute : SCR_BaseEditorAttribute
{
 [Attribute()] protected string m_Key;
 override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(item);
  if (!mode || !IsGameMode(item)) return null;
  return SCR_BaseEditorAttributeVar.CreateBool(mode.BCMM_ReadSetting(m_Key) > 0.5);
 }
 override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(item);
  if (!mode || !var) return;
  float numericValue;
  if (var.GetBool()) numericValue = 1;
  mode.BCMM_ChangeSetting(m_Key, numericValue, manager);
 }
}
