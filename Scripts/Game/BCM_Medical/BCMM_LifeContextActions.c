// Native GM character context only; no separate BCM/body-bag menu.
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class BCMM_AdjustPlayerLivesContextAction : SCR_SelectedEntitiesContextAction
{
 [Attribute("1")] protected int m_Change;
 [Attribute("0")] protected bool m_Reset;
 override bool CanBeShown(SCR_EditableEntityComponent selectedEntity,vector cursorWorldPosition,int flags)
 {
  return selectedEntity && selectedEntity.GetEntityType()==EEditableEntityType.CHARACTER && GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(selectedEntity.GetOwner())>0;
 }
 override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity,vector cursorWorldPosition,int flags)
 {
  if(!CanBeShown(selectedEntity,cursorWorldPosition,flags))return false;
  // Server execution is checked against its actual actions component below.
  if(Replication.IsServer())return true;
  SCR_EditorManagerEntity editor=SCR_EditorManagerEntity.GetInstance();if(!editor || editor.IsLimited())return false;
  SCR_EditorModeEntity editMode=editor.FindModeEntity(EEditorMode.EDIT);return editMode && !editMode.IsLimited();
 }
 // No unscoped Perform override: only the executing native component supplies
 // its authenticated server editor context, never a client-supplied player ID.
 void BCMM_PerformFromEditor(SCR_AttributesManagerEditorComponent manager,set<SCR_EditableEntityComponent> selectedEntities)
 {
  if(!SCR_BaseGameMode.BCMM_IsSettingsEditor(manager) || !selectedEntities)return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(!mode)return;
  foreach(SCR_EditableEntityComponent selected:selectedEntities)
   if(CanBeShown(selected,vector.Zero,0))mode.BCMM_AdjustSelectedPlayerLives(manager,selected.GetOwner(),m_Change,m_Reset);
 }
}
modded class SCR_BaseActionsEditorComponent
{
 override protected void ActionPerform(SCR_BaseEditorAction action,SCR_EditableEntityComponent hoveredEntityComponent,set<SCR_EditableEntityComponent> selectedEntityComponents,vector cursorWorldPosition,int flags,int param)
 {
  BCMM_AdjustPlayerLivesContextAction allowance=BCMM_AdjustPlayerLivesContextAction.Cast(action);
  if(!allowance){super.ActionPerform(action,hoveredEntityComponent,selectedEntityComponents,cursorWorldPosition,flags,param);return;}
  if(!Replication.IsServer())return;
  SCR_EditorManagerEntity editor=GetManager();if(!editor)return;
  SCR_EditorModeEntity editMode=editor.FindModeEntity(EEditorMode.EDIT);if(!editMode)return;
  SCR_AttributesManagerEditorComponent attributes=SCR_AttributesManagerEditorComponent.Cast(editMode.FindComponent(SCR_AttributesManagerEditorComponent));
  allowance.BCMM_PerformFromEditor(attributes,selectedEntityComponents);
 }
}
modded class SCR_BaseGameMode
{
 bool BCMM_AdjustSelectedPlayerLives(SCR_AttributesManagerEditorComponent manager,IEntity selected,int change,bool reset)
 {
  if(!BCMM_IsSettingsEditor(manager) || !selected || !ChimeraCharacter.Cast(selected))return false;
  if((reset && change!=0) || (!reset && change!=1 && change!=-1))return false;
  int target=GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(selected);
  PlayerController player=GetGame().GetPlayerManager().GetPlayerController(target);
  if(target<=0 || !player || player.GetControlledEntity()!=selected || !SCR_BaseGameMode.Cast(this).BCMM_AccountFor(target))return false;
  if(!SCR_BaseGameMode.Cast(this).BCMM_AdjustLives(manager,target,change,reset))return false;
  BCMM_SettingsFeedback(manager,"BCM Medical: selected player's deployment allowance updated.");return true;
 }
}
