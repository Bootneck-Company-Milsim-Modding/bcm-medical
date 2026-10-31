// Original native GM adapter. The editor manager identifies the authenticated
// actor; an arbitrary player ID in an attribute payload is never trusted.
modded class SCR_BaseGameMode
{
 protected ref BCMM_Settings m_BCMMSettings = new BCMM_Settings();
 protected ref BCMM_SettingsStore m_BCMMSettingsStore;
 protected ref array<float> m_BCMMReplicatedSettings = {};

 [RplProp()] protected int m_BCMMSettingsGeneration;
 protected int m_BCMMSettingsRevision;
 protected bool m_BCMMHasSettingsSnapshot;

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
  m_BCMMSettingsRevision++;
  array<int> players={}; GetGame().GetPlayerManager().GetPlayers(players);
  foreach(int playerId:players)
  {
   SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
   if(player) player.BCMM_SendSettingsSnapshot(m_BCMMReplicatedSettings,m_BCMMSettingsRevision);
  }
  Replication.BumpMe();
  SCR_ArsenalComponent.BCMM_RefreshMedicalAvailability();
 }
 float BCMM_ReadSetting(string key)
 {
  if (Replication.IsServer()) return m_BCMMSettings.Get(key);
  int index = m_BCMMSettings.IndexOf(key);
  if (m_BCMMReplicatedSettings.IsIndexValid(index)) return m_BCMMReplicatedSettings[index];
  return m_BCMMSettings.Get(key);
 }
 void BCMM_CopySettings(notnull BCMM_Settings snapshot)
 {
  snapshot.CopyFrom(m_BCMMSettings);
  if (!Replication.IsServer()) snapshot.ImportReplicatedValues(m_BCMMReplicatedSettings);
 }
 bool BCMM_HasSettingsSnapshot() { return m_BCMMHasSettingsSnapshot; }
 void BCMM_SendCurrentSettings(SCR_PlayerController player)
 {
  if(Replication.IsServer() && player && m_BCMMSettingsRevision>0)player.BCMM_SendSettingsSnapshot(m_BCMMReplicatedSettings,m_BCMMSettingsRevision);
 }
 void BCMM_AcceptSettingsSnapshot(notnull array<float> values,int revision)
 {
  string fixtureTrace; if(System.GetCLIParam("bcmmStretcherSettingsMirror",fixtureTrace))PrintFormat("BCMM_SNAPSHOT_RECEIVE count=%1 revision=%2 current=%3 server=%4",values.Count(),revision,m_BCMMSettingsRevision,Replication.IsServer());
  if(Replication.IsServer() || revision<=m_BCMMSettingsRevision)return;
  BCMM_Settings candidate=new BCMM_Settings();
  if(!candidate.ImportReplicatedValues(values)){Print("BCMM settings snapshot rejected: invalid values",LogLevel.WARNING);return;}
  candidate.ExportValues(m_BCMMReplicatedSettings);m_BCMMSettingsRevision=revision;m_BCMMHasSettingsSnapshot=true;
  SCR_ArsenalComponent.BCMM_RefreshMedicalAvailability();
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

// Display-only section rows share the same category as every setting. They have
// no writable value and are excluded from session serialization.
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class BCMM_SectionHeadingAttribute : SCR_BaseEditorAttribute
{
 override bool BoolAllowDuplicate() { return true; }
 override bool IsSerializable() { return false; }
 override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
 {
  if (!IsGameMode(item)) return null;
  return SCR_BaseEditorAttributeVar.CreateBool(false);
 }
}
class BCMM_SectionHeadingUI : SCR_BaseEditorAttributeUIComponent
{
 override void Init(Widget w, SCR_BaseEditorAttribute attribute)
 {
  TextWidget heading = TextWidget.Cast(w.FindAnyWidget("SectionTitle"));
  if (heading && attribute.GetUIInfo()) heading.SetText(attribute.GetUIInfo().GetName());
 }
 override void SetFromVar(SCR_BaseEditorAttributeVar var) {}
}

// Reliable owner snapshots support initial join and live GM changes without array properties.
modded class SCR_PlayerController
{
 void BCMM_RequestSettingsSnapshot() { if(GetGame().GetPlayerController()==this)Rpc(BCMM_SettingsSnapshotRequest); }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_SettingsSnapshotRequest()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  string fixtureTrace; if(System.GetCLIParam("bcmmStretcherSettingsMirror",fixtureTrace))Print("BCMM_SNAPSHOT_REQUEST_RECEIVED");
  if(mode)mode.BCMM_SendCurrentSettings(this);
 }
 void BCMM_SendSettingsSnapshot(notnull array<float> values,int revision)
 {
  string fixtureTrace; if(System.GetCLIParam("bcmmStretcherSettingsMirror",fixtureTrace))PrintFormat("BCMM_SNAPSHOT_SEND player=%1 count=%2 revision=%3",GetPlayerId(),values.Count(),revision);
  if(Replication.IsServer())Rpc(BCMM_SettingsSnapshotOwner,values,revision);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_SettingsSnapshotOwner(array<float> values,int revision)
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode && values)mode.BCMM_AcceptSettingsSnapshot(values,revision);
 }
}
modded class ArmaReforgerScripted
{
 protected float m_BCMMNextSettingsRequest;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  if(world.IsEditMode() || Replication.IsServer() || world.GetWorldTime()<m_BCMMNextSettingsRequest)return;
  m_BCMMNextSettingsRequest=world.GetWorldTime()+1000;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(mode && player && !mode.BCMM_HasSettingsSnapshot())player.BCMM_RequestSettingsSnapshot();
 }
}
