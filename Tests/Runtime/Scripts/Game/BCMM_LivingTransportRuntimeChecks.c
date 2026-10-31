class BCMM_LivingTransportRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_CharacterDamageManagerComponent m_ActorMedical;
 protected int m_Mode=1;
 protected bool m_Conscious;
 protected vector m_Original;
 protected int m_Layer;
 protected bool m_MovementBefore;
 protected SCR_ManualCamera m_ReviewCamera;
 protected IEntity m_UsedPoseAnchor;
 override protected void Prepare()
 {
  if(!m_Actor || !m_Target){m_Suite.Record(false,"Living transport fixture characters spawn");Finish();return;}
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 1 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_ActorMedical=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!m_Conscious){m_Medical.GetBloodHitZone().SetHealth(0);m_Medical.UpdateConsciousness();}
  else {m_Medical.BCMM_GetPatient().LeftLeg.Broken=true;m_Medical.BCMM_PublishFractures();}
  GetGame().GetCallqueue().CallLater(Begin,1800);
 }
 protected void Begin()
 {
  TraceParam support=new TraceParam();support.Start=m_Actor.GetOrigin()+"0 2 0";support.End=m_Actor.GetOrigin()-"0 2 0";
  support.Flags=TraceFlags.WORLD|TraceFlags.ENTS;support.LayerMask=EPhysicsLayerPresets.Projectile;
  array<IEntity> otherPatient={m_Target};support.ExcludeArray=otherPatient;
  float rawSupport=m_Actor.GetWorld().TraceMove(support);
  float floorSupport=BCMM_LivingTransport.TraceGround(m_Actor.GetWorld(),support,otherPatient);
  m_Suite.Record(rawSupport<0.4,"Ground regression fixture really intersects a native character above the deck");
  m_Suite.Record(Math.AbsFloat(support.Start[1]-4*floorSupport-m_Actor.GetOrigin()[1])<0.15,"Transport support trace skips a native character and finds the actual solid deck");
  m_Original=m_Target.GetOrigin();Physics p=m_Target.GetPhysics();if(p && p.GetNumGeoms()>0)m_Layer=p.GetGeomInteractionLayer(0);
  m_MovementBefore=ChimeraCharacter.Cast(m_Target).GetCharacterController().GetDisableMovementControls();
  SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  m_Suite.Record(BCMM_LivingTransport.Eligible(m_Actor,m_Target,m_Mode),"Native casualty transport passes initial range, life and treatment checks");
  BCMM_LivingTransport.Request(owner,m_Target,m_Mode);
  m_Suite.Record(m_Medical.BCMM_TransportMode()==m_Mode,"Authority starts requested native living transport without an unconscious consent prompt");
  GetGame().GetCallqueue().CallLater(CheckStarted,2500);
 }
 protected void CheckStarted()
 {
  m_Suite.Record(m_Medical.BCMM_TransportMode()==m_Mode && m_Medical.BCMM_TransportActor()==m_Actor,"Native living transport persists with exact carrier and patient");
  m_UsedPoseAnchor=m_Medical.BCMM_PoseAnchorEntity();
  m_Suite.Record(BCMM_PoseAnchor.Contains(m_UsedPoseAnchor,m_Target),"Original native casualty occupies the BCM pose holder without replacing the character");
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious()!=m_Conscious,"Transport preserves the casualty's real consciousness");
  m_Suite.Record(!ChimeraCharacter.Cast(m_Actor).GetCharacterController().CanJumpClimb(),"Carrier cannot jump or vault during transport");
  if(m_Mode==2)m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetStance()==ECharacterStance.CROUCH,"Dragging uses native crouch stance");
  vector poseHips[4];m_Target.GetAnimation().GetBoneMatrix(m_Target.GetAnimation().GetBoneIndex("Hips"),poseHips);
  PrintFormat("BCMM_POSE_NATIVE mode=%1 conscious=%2 root=%3 hips=%4 anchor=%5",m_Mode,m_Conscious,m_Target.GetOrigin(),poseHips[3],m_Medical.BCMM_PoseAnchorEntity().GetPrefabData().GetPrefabName());
  vector poseHead[4];vector poseHand[4];
  m_Target.GetAnimation().GetBoneMatrix(m_Target.GetAnimation().GetBoneIndex("Head"),poseHead);
  m_Target.GetAnimation().GetBoneMatrix(m_Target.GetAnimation().GetBoneIndex("LeftHand"),poseHand);
  vector poseWorld[4];m_Target.GetWorldTransform(poseWorld);
  PrintFormat("BCMM_POSE_BONES mode=%1 head=%2 hand=%3 up=%4 forward=%5",m_Mode,poseHead[3],poseHand[3],poseWorld[1],poseWorld[2]);
  if(m_Mode==1)m_Suite.Record(poseHips[3][1]>1.4 && poseHead[3][1]>1.2 && poseHand[3][1]<1.1,"Native carry actually plays the original shoulder-height folded casualty clip");
  else m_Suite.Record(poseHips[3][1]<0.4 && poseHead[3][1]>0.4 && poseHand[3][1]<0.3,"Native drag actually plays the original low-hip raised-torso casualty clip");
  if(GetGame().GetWorkspace() && GetGame().GetCameraManager())
  {
   vector centre=m_Actor.GetOrigin()+"0 0.85 0";vector origin=centre+"2.8 0.9 2.1";
   EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
   Math3D.AnglesToMatrix((centre-origin).VectorToAngles(),spawn.Transform);spawn.Transform[3]=origin;
   if(!m_ReviewCamera)m_ReviewCamera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));
   m_ReviewCamera.SetWorldTransform(spawn.Transform);m_ReviewCamera.SetInputEnabled(false);m_ReviewCamera.SetVerticalFOV(45);
   GetGame().GetCameraManager().SetCamera(m_ReviewCamera);m_ReviewCamera.ApplyTransform(0);
   GetGame().GetCallqueue().CallLater(CapturePose,200);
   if(m_Mode==1)GetGame().GetCallqueue().CallLater(ReviewRear,350);
  }
  m_Start=m_Actor.GetOrigin();
  vector t[4];m_Actor.GetWorldTransform(t);t[3]=t[3]+t[0]*0.3;ChimeraCharacter.Cast(m_Actor).Teleport(t);
  GetGame().GetCallqueue().CallLater(CheckMoved,700);
 }
 protected void CapturePose(){System.MakeScreenshot(string.Format("$profile:living-transport-mode-%1-conscious-%2",m_Mode,m_Conscious));}
 protected void ReviewRear()
 {
  if(!m_ReviewCamera || !m_Actor)return;
  vector centre=m_Actor.GetOrigin()+"0 0.85 0";vector origin=centre+"-2.8 0.9 -2.1";vector transform[4];
  Math3D.AnglesToMatrix((centre-origin).VectorToAngles(),transform);transform[3]=origin;
  m_ReviewCamera.SetWorldTransform(transform);m_ReviewCamera.ApplyTransform(0);
  GetGame().GetCallqueue().CallLater(CaptureRear,100);
 }
 protected void CaptureRear(){System.MakeScreenshot(string.Format("$profile:living-transport-carry-rear-conscious-%1",m_Conscious));}
 protected void CheckMoved()
 {
  m_Suite.Record(m_Medical.BCMM_TransportMode()==m_Mode && vector.Distance(m_Target.GetOrigin(),m_Original)>0.2,"Original casualty follows a small legal carrier movement");
  if(m_Conscious)m_Suite.Record(m_Medical.BCMM_GetPatient().LeftLeg.Broken,"Conscious broken-leg carrying preserves the fracture");
  BCMM_LivingTransport.Release(m_Actor);
  GetGame().GetCallqueue().CallLater(CheckReleased,900);
 }
 protected void CheckReleased()
 {
  m_Suite.Record(m_Medical.BCMM_TransportMode()==0 && !BCMM_LivingTransport.ForParticipant(m_Actor),"Release clears both participants' transport reservation");
  m_Suite.Record(!ChimeraCharacter.Cast(m_Target).GetCompartmentAccessComponent().IsInCompartment() && !m_UsedPoseAnchor,"Release detaches the original casualty and removes the empty replicated pose holder");
  Physics p=m_Target.GetPhysics();if(p && p.GetNumGeoms()>0)m_Suite.Record(p.GetGeomInteractionLayer(0)==m_Layer,"Release restores original casualty collision layers");
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().GetDisableMovementControls()==m_MovementBefore,"Release restores original patient movement-control policy");
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious()!=m_Conscious,"Release never heals or manufactures unconsciousness");
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().CanJumpClimb(),"Release restores the carrier's ordinary jump eligibility");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);SCR_EntityHelper.DeleteEntityAndChildren(m_Target);SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  if(m_ReviewCamera){SCR_EntityHelper.DeleteEntityAndChildren(m_ReviewCamera);m_ReviewCamera=null;}
  if(m_Mode==1 && !m_Conscious){m_Mode=2;Start(m_Suite);return;}
  if(!m_Conscious){m_Mode=1;m_Conscious=true;Start(m_Suite);return;}
  if(m_Mode==1 && m_Conscious){m_Mode=2;Start(m_Suite);return;}
  m_Suite.Complete();
 }
}
