// Isolated native visual review only; never shipped as gameplay.
class BCMM_MedicalArtReview:BCMM_SplintRuntimeChecks
{
 protected SCR_ManualCamera m_Camera;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");m_Actor.SetOrigin("100 2.25 0");m_Target.SetOrigin("102 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai) ai.DeactivateAI();ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) ai.DeactivateAI();
  RplComponent ownership=RplComponent.Cast(m_Actor.FindComponent(RplComponent));if(ownership) ownership.GiveExt(GetGame().GetPlayerController().GetRplIdentity(),false);
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);GetGame().GetCallqueue().CallLater(Show,2000);
 }
 protected void Show()
 {
  string kind;System.GetCLIParam("bcmmMedicalArtReview",kind);IEntity subject=m_Target;
  if(kind=="tablet") { subject=m_Actor;GetGame().GetCallqueue().CallLater(TabletPlayback,100); }
  else { m_Medical=SCR_CharacterDamageManagerComponent.Cast(subject.FindComponent(SCR_CharacterDamageManagerComponent));m_Medical.BCMM_GetPatient().Equipment.OxygenActive=true;BCMM_VisualStatusBridge.Publish(m_Medical); }
  vector body[4];vector head[4];subject.GetWorldTransform(body);subject.GetAnimation().GetBoneMatrix(subject.GetAnimation().GetBoneIndex("Head"),head);vector centre=BCMM_GripMath.Point(body,head[3]);
  PrintFormat("BCMM_ART_HEAD right=%1 up=%2 forward=%3 local=%4",head[0],head[1],head[2],head[3]);
  vector origin=centre+body[0]*0.38+body[2]*0.85+"0 0.03 0";
  vector look=centre+Vector(0,-0.04,0);EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;Math3D.AnglesToMatrix((look-origin).VectorToAngles(),spawn.Transform);spawn.Transform[3]=origin;
  m_Camera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));m_Camera.SetInputEnabled(false);m_Camera.SetVerticalFOV(40);GetGame().GetCameraManager().SetCamera(m_Camera);PrintFormat("BCMM_ART_REVIEW_READY kind=%1",kind);
 }
 protected int m_TabletSamples;
 protected void TabletPlayback()
 {
  bool active=BCMM_CPRAnimation.IsActive(m_Actor,BCMM_FieldClips.TABLETSELF_GRAPH);bool started;
  if(!active) started=BCMM_CPRAnimation.Start(m_Actor,null,BCMM_FieldClips.TABLETSELF_GRAPH,BCMM_FieldClips.TABLETSELF_INSTANCE);
  vector hand[4];m_Actor.GetAnimation().GetBoneMatrix(m_Actor.GetAnimation().GetBoneIndex("RightHand"),hand);
  PrintFormat("BCMM_TABLET_ART_PLAYBACK active=%1 started=%2 owner=%3 hand=%4",active,started,RplComponent.Cast(m_Actor.FindComponent(RplComponent)).IsOwner(),hand[3]);
  if(m_TabletSamples++<240) GetGame().GetCallqueue().CallLater(TabletPlayback,500);
 }
}
