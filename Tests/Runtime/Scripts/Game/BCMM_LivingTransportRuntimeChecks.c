class BCMM_LivingTransportRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_CharacterDamageManagerComponent m_ActorMedical;
 protected int m_Mode=1;
 protected bool m_Conscious;
 protected vector m_Original;
 protected int m_Layer;
 protected int m_BodyLayer;
 protected ref array<int> m_OriginalGeometry={};
 protected bool m_MovementBefore;
 protected SCR_ManualCamera m_ReviewCamera;
 protected IEntity m_UsedPoseAnchor;
 protected IEntity m_CollisionProbe;
 protected ref array<IEntity> m_OriginalPatientItems={};
 override protected void Prepare()
 {
  string faction;
  if(System.GetCLIParam("bcmmTransportPatientFaction",faction) && faction!="US")
  {
   ResourceName resource="{98EB9CDD85B8C92C}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Unarmed.et";
   if(faction=="FIA")resource="{854C04F0EA2129CC}Prefabs/Characters/Factions/INDFOR/FIA/Character_FIA_Unarmed.et";
   SCR_EntityHelper.DeleteEntityAndChildren(m_Target);m_Target=Spawn(resource,"101 1.2 0");
   PrintFormat("BCMM_TRANSPORT_FACTION faction=%1 patient=%2",faction,m_Target);
  }
  m_CollisionProbe=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","105 3 0");
  SCR_CharacterDamageManagerComponent probeMedical=SCR_CharacterDamageManagerComponent.Cast(m_CollisionProbe.FindComponent(SCR_CharacterDamageManagerComponent));
  probeMedical.GetBloodHitZone().SetHealth(0);probeMedical.UpdateConsciousness();
  if(!m_Actor || !m_Target){m_Suite.Record(false,"Living transport fixture characters spawn");Finish();return;}
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 1 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
  string activeAI;
  if(System.GetCLIParam("bcmmTransportActiveAI",activeAI)){if(ai)ai.ActivateAI();}
  else if(ai)ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_ActorMedical=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!m_Conscious){m_Medical.GetBloodHitZone().SetHealth(0);m_Medical.UpdateConsciousness();}
  else {m_Medical.BCMM_GetPatient().LeftLeg.Broken=true;m_Medical.BCMM_PublishFractures();}
  GetGame().GetCallqueue().CallLater(Begin,1800);
 }
 protected void Begin()
 {
  Physics probePhysics=m_CollisionProbe.GetPhysics();int probeBefore=probePhysics.GetInteractionLayer();
  for(int probeIndex=0;probeIndex<probePhysics.GetNumGeoms();probeIndex++)probePhysics.SetGeomInteractionLayer(probeIndex,probePhysics.GetGeomInteractionLayer(probeIndex));
  PrintFormat("BCMM_COLLISION_IDENTITY_PROBE unconscious=1 beforeBody=%1 afterBody=%2",probeBefore,probePhysics.GetInteractionLayer());
  SCR_EntityHelper.DeleteEntityAndChildren(m_CollisionProbe);
  TraceParam support=new TraceParam();support.Start=m_Actor.GetOrigin()+"0 2 0";support.End=m_Actor.GetOrigin()-"0 2 0";
  support.Flags=TraceFlags.WORLD|TraceFlags.ENTS;support.LayerMask=EPhysicsLayerPresets.Projectile;
  array<IEntity> otherPatient={m_Target};support.ExcludeArray=otherPatient;
  float rawSupport=m_Actor.GetWorld().TraceMove(support);
  float floorSupport=BCMM_LivingTransport.TraceGround(m_Actor.GetWorld(),support,otherPatient);
  m_Suite.Record(rawSupport<0.4,"Ground regression fixture really intersects a native character above the deck");
  m_Suite.Record(Math.AbsFloat(support.Start[1]-4*floorSupport-m_Actor.GetOrigin()[1])<0.15,"Transport support trace skips a native character and finds the actual solid deck");
  m_Original=m_Target.GetOrigin();Physics p=m_Target.GetPhysics();if(p && p.GetNumGeoms()>0)m_Layer=p.GetGeomInteractionLayer(0);
  m_BodyLayer=0;m_OriginalGeometry.Clear();
  if(p)for(int geom=0;geom<p.GetNumGeoms();geom++){int layer=p.GetGeomInteractionLayer(geom);m_OriginalGeometry.Insert(layer);m_BodyLayer=m_BodyLayer|layer;}
  m_MovementBefore=ChimeraCharacter.Cast(m_Target).GetCharacterController().GetDisableMovementControls();
  string activeAI;
  if(System.GetCLIParam("bcmmTransportActiveAI",activeAI) && m_Conscious)
  {
   AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
   m_Suite.Record(ai && ai.IsAIActivated(),"Conscious AI casualty has active native AI before transport");
   m_Suite.Record(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Target)<=0,"Active AI casualty is not a human-controlled patient");
  }
  SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  // Take the manifest after native spawn/incapacitation has settled, at the
  // actual transport boundary rather than during asynchronous loadout setup.
  m_OriginalPatientItems.Clear();m_Inventory.GetItems(m_OriginalPatientItems,EStoragePurpose.PURPOSE_ANY);
  m_Suite.Record(!m_OriginalPatientItems.IsEmpty(),"Transport patient starts with native faction inventory");
  PrintFormat("BCMM_TRANSPORT_INVENTORY_BEFORE mode=%1 conscious=%2 count=%3",m_Mode,m_Conscious,m_OriginalPatientItems.Count());
  m_Suite.Record(BCMM_LivingTransport.Eligible(m_Actor,m_Target,m_Mode),"Native casualty transport passes initial range, life and treatment checks");
  BCMM_LivingTransport.Request(owner,m_Target,m_Mode);
  m_Suite.Record(m_Medical.BCMM_TransportMode()==m_Mode,"Authority starts requested native living transport without an unconscious consent prompt");
  GetGame().GetCallqueue().CallLater(CheckStarted,2500);
 }
 protected void CheckStarted()
 {
  m_Suite.Record(m_Medical.BCMM_TransportMode()==m_Mode && m_Medical.BCMM_TransportActor()==m_Actor,"Native living transport persists with exact carrier and patient");
  m_UsedPoseAnchor=m_Medical.BCMM_PoseAnchorEntity();
  Physics transportedPhysics=m_Target.GetPhysics();
  m_Suite.Record(transportedPhysics && transportedPhysics.GetInteractionLayer()==EPhysicsLayerPresets.FireGeo,"Carried or dragged casualty retains damage geometry without physical carrier collision");
  if(transportedPhysics)PrintFormat("BCMM_CASUALTY_COLLISION before=%1 during=%2 body=%3",m_Layer,transportedPhysics.GetGeomInteractionLayer(0),transportedPhysics.GetInteractionLayer());
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
  if(p)PrintFormat("BCMM_COLLISION_RELEASE expectedBody=%1 actualBody=%2 expectedGeom=%3 actualGeom=%4 count=%5",m_BodyLayer,p.GetInteractionLayer(),m_Layer,p.GetGeomInteractionLayer(0),p.GetNumGeoms());
  m_Suite.Record(p && p.GetInteractionLayer()==m_BodyLayer,"Casualty release restores the effective union of its own original collision geometries");
  m_Suite.Record(p && p.GetNumGeoms()==m_OriginalGeometry.Count(),"Casualty release retains every original collision geometry");
  if(p)for(int geom=0;geom<Math.Min(p.GetNumGeoms(),m_OriginalGeometry.Count());geom++)m_Suite.Record(p.GetGeomInteractionLayer(geom)==m_OriginalGeometry[geom],"Casualty release restores exact original geometry mask");
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().GetDisableMovementControls()==m_MovementBefore,"Release restores original patient movement-control policy");
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious()!=m_Conscious,"Release never heals or manufactures unconsciousness");
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().CanJumpClimb(),"Release restores the carrier's ordinary jump eligibility");
  Physics restoredBody=m_Target.GetPhysics();
  if(restoredBody && restoredBody.GetNumGeoms()>0)
  {
   BCMM_TransportCollision guard=new BCMM_TransportCollision();
   int originalMask=restoredBody.GetGeomInteractionLayer(0);
   guard.Apply(m_Target,EPhysicsLayerPresets.FireGeo);
   restoredBody.SetGeomInteractionLayer(0,EPhysicsLayerDefs.Camera);
   guard.Restore(m_Target);
   m_Suite.Record(restoredBody.GetGeomInteractionLayer(0)==EPhysicsLayerDefs.Camera,"Release preserves a subsequent native geometry policy change");
   restoredBody.SetGeomInteractionLayer(0,originalMask);
   guard.Apply(m_Target,EPhysicsLayerPresets.FireGeo);
   restoredBody.SetGeomInteractionLayer(0,EPhysicsLayerDefs.Camera);
   guard.Apply(m_Target,EPhysicsLayerPresets.FireGeo);
   guard.Restore(m_Target);
   m_Suite.Record(restoredBody.GetGeomInteractionLayer(0)==EPhysicsLayerDefs.Camera,"Repeated suppression restores the latest native geometry policy");
   restoredBody.SetGeomInteractionLayer(0,originalMask);
  }
  string activeAI;
  if(System.GetCLIParam("bcmmTransportActiveAI",activeAI) && m_Conscious)
  {
   AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
   m_Suite.Record(ai && ai.IsAIActivated(),"Releasing a conscious AI casualty preserves active native AI");
  }
  array<IEntity> releasedItems={};m_Inventory.GetItems(releasedItems,EStoragePurpose.PURPOSE_ANY);
  PrintFormat("BCMM_TRANSPORT_INVENTORY_AFTER mode=%1 conscious=%2 count=%3",m_Mode,m_Conscious,releasedItems.Count());
  foreach(IEntity added:releasedItems)if(!m_OriginalPatientItems.Contains(added))PrintFormat("BCMM_TRANSPORT_INVENTORY_ADDED item=%1",added);
  m_Suite.Record(releasedItems.Count()==m_OriginalPatientItems.Count(),"Transport preserves complete native faction inventory count");
  foreach(IEntity original:m_OriginalPatientItems)m_Suite.Record(original && releasedItems.Contains(original),"Transport retains each exact original faction inventory entity");
  m_OriginalPatientItems.Clear();m_Inventory=null;
  // Let local native Physics handles leave scope before deleting their entity.
  GetGame().GetCallqueue().CallLater(CleanupTransportCase,100);
 }
 protected void CleanupTransportCase()
 {
  Print("BCMM_TRANSPORT_CLEANUP actor");SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  Print("BCMM_TRANSPORT_CLEANUP patient");SCR_EntityHelper.DeleteEntityAndChildren(m_Target);
  Print("BCMM_TRANSPORT_CLEANUP floor");SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  if(m_ReviewCamera){SCR_EntityHelper.DeleteEntityAndChildren(m_ReviewCamera);m_ReviewCamera=null;}
  if(m_Mode==1 && !m_Conscious){m_Mode=2;Start(m_Suite);return;}
  if(!m_Conscious){m_Mode=1;m_Conscious=true;Start(m_Suite);return;}
  if(m_Mode==1 && m_Conscious){m_Mode=2;Start(m_Suite);return;}
  m_Suite.Complete();
 }
}
modded class BCMM_TransportCollision
{
 override void Restore(IEntity entity)
 {
  string diagnostic;
  if(m_Entity && System.GetCLIParam("bcmmLivingTransportRuntime",diagnostic))
  {
   PrintFormat("BCMM_COLLISION_RESTORE savedBody=%1 geoms=%2 sameBody=%3",m_Previous,m_Geometry.Count(),entity && entity==m_Entity);
   for(int g=0;g<m_Geometry.Count();g++)PrintFormat("BCMM_COLLISION_GEOM index=%1 saved=%2",g,m_Geometry[g]);
  }
  super.Restore(entity);
 }
}
