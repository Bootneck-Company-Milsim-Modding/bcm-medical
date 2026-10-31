// Read-only inspection uses the replicated memorial; the corpse no longer exists.
modded class SCR_PlayerController
{
 protected int m_BCMMLastInspectedGraveToken;
 int BCMM_LastInspectedGraveToken() { return m_BCMMLastInspectedGraveToken; }
 bool BCMM_InspectGraveLocal(IEntity grave)
 {
  if(GetGame().GetPlayerController()!=this || !grave)return false;
  ChimeraCharacter actor=ChimeraCharacter.Cast(GetControlledEntity());
  if(!actor || vector.Distance(actor.GetOrigin(),grave.GetOrigin())>3)return false;
  CharacterControllerComponent character=actor.GetCharacterController();
  if(!character || character.GetLifeState()!=ECharacterLifeState.ALIVE || character.IsUnconscious())return false;
  BCMM_GraveMarkerComponent marker=BCMM_GraveMarkerComponent.Cast(grave.FindComponent(BCMM_GraveMarkerComponent));
  if(!marker || marker.RemainsToken()<=0 || marker.DeceasedName().IsEmpty())return false;
  string message=WidgetManager.Translate("#BCMM-GraveMemorial");
  if(message.IsEmpty() || message=="#BCMM-GraveMemorial")message="%1 died in service of his country.";
  message=string.Format(message,marker.DeceasedName());
  bool shown=SCR_HintManagerComponent.ShowCustomHint(message,WidgetManager.Translate("#BCMM-GraveTitle"),8);
  if(shown)m_BCMMLastInspectedGraveToken=marker.RemainsToken();
  PrintFormat("BCMM_GRAVE_INSPECT token=%1 hintShown=%2 text=%3",marker.RemainsToken(),shown,message);
  return shown;
 }
}
