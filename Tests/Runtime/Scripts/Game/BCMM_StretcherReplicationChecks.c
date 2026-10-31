// Unpublished connected-owner and already-occupied join acceptance.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMStretcherPhase;
 [RplProp()] protected RplId m_BCMMTestStretcher = RplId.Invalid();
 [RplProp()] protected RplId m_BCMMTestStretcherPatient = RplId.Invalid();
 void BCMM_PublishStretcher(int phase,IEntity stretcher,IEntity patient)
 {
  m_BCMMTestStretcher=BCMM_BodyBagComponent.Id(stretcher);m_BCMMTestStretcherPatient=BCMM_BodyBagComponent.Id(patient);
  m_BCMMStretcherPhase=phase;Replication.BumpMe();PrintFormat("BCMM_STRETCHER_PHASE %1",phase);
 }
 int BCMM_StretcherPhase(){return m_BCMMStretcherPhase;}
 IEntity BCMM_TestStretcher(){return BCMM_BodyBagComponent.Resolve(m_BCMMTestStretcher);}
 IEntity BCMM_TestStretcherPatient(){return BCMM_BodyBagComponent.Resolve(m_BCMMTestStretcherPatient);}
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMStretcherPlaceSent;
 protected bool m_BCMMStretcherRemoveSent;
 protected int m_BCMMStretcherObserved;
 protected bool m_BCMMGripSent;
 protected bool m_BCMMCarryPositioned;
 protected bool m_BCMMGripReleaseSent;
 protected bool m_BCMMStaleGripSent;
 protected float m_BCMMWalkStarted;
 protected float m_BCMMMirrorLogAt;
 protected float m_BCMMGripCheckStarted;
 protected float m_BCMMCarryInputLogAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmStretcherObserver",observer))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());if(!mode)return;
  IEntity entity=mode.BCMM_TestStretcher();IEntity patient=mode.BCMM_TestStretcherPatient();if(!entity || !patient)return;
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(entity.FindComponent(BCMM_StretcherComponent));if(!stretcher)return;
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());int phase=mode.BCMM_StretcherPhase();
  string review;
  if(System.GetCLIParam("bcmmStretcherReview",review) && phase>=6 && player && player.GetPlayerId()==3) { BCMM_StretcherReview.Update(entity); }
  if (player && player.GetPlayerId()<=2)
  {
   if(phase==10 || phase==14){m_BCMMCarryPositioned=false;m_BCMMGripSent=false;}
   if ((phase==5 || phase==11 || phase==15) && !m_BCMMCarryPositioned)
   {
    player.GetControlledEntity().SetAngles("0 0 0");
    player.GetControlledEntity().SetOrigin(stretcher.Handle(player.GetPlayerId()==1)+"0 0.1 0");
    m_BCMMCarryPositioned=true;
   }
   if ((phase==5 || phase==11 || phase==15) && !m_BCMMGripSent && stretcher.CanJoin(player.GetControlledEntity(),player.GetPlayerId()==1))
   {
    if(player.GetPlayerCamera()) GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
    int grip=1;if(player.GetPlayerId()==1)grip=0;
    player.BCMM_RequestStretcherGrip(entity,grip);m_BCMMGripSent=true;
    PrintFormat("BCMM_CARRY_GRIP_SENT player=%1",player.GetPlayerId());
   }
   if (phase==7)
   {
    if(m_BCMMWalkStarted==0)m_BCMMWalkStarted=world.GetWorldTime();
    CharacterControllerComponent control=ChimeraCharacter.Cast(player.GetControlledEntity()).GetCharacterController();
    if(world.GetWorldTime()>=m_BCMMCarryInputLogAt)
    {
     m_BCMMCarryInputLogAt=world.GetWorldTime()+1000;
     float speed;vector direction;control.GetInputContext().GetMovement(speed,direction);
     PrintFormat("BCMM_CARRY_INPUT player=%1 owner=%2 disabled=%3 speed=%4 direction=%5 heading=%6 pos=%7",player.GetPlayerId(),ChimeraCharacter.Cast(player.GetControlledEntity()).GetRplComponent().IsOwner(),control.GetDisableMovementControls(),speed,direction,control.GetInputContext().GetHeadingAngle(),player.GetControlledEntity().GetOrigin());
     Physics physics=entity.GetPhysics();
     for(int i=0;i<physics.GetNumGeoms();i++)PrintFormat("BCMM_CARRY_GEOM player=%1 index=%2 layer=%3",player.GetPlayerId(),i,physics.GetGeomInteractionLayer(i));
    }
    if(world.GetWorldTime()-m_BCMMWalkStarted<2000)
    {GetGame().GetInputManager().ActivateAction("CharacterRight");GetGame().GetInputManager().SetActionValue("CharacterRight",1);}
    else GetGame().GetInputManager().SetActionValue("CharacterRight",0);
   }
   if(phase==19 && player.GetPlayerId()==1 && !m_BCMMStretcherRemoveSent && stretcher.ActorCanUse(player.GetControlledEntity()))
   {player.BCMM_RequestStretcher(entity,null,true);m_BCMMStretcherRemoveSent=true;Print("BCMM_CARRY_OWNED_UNLOAD_SENT");}
   if(phase==15 && player.GetPlayerId()==1 && stretcher.IsCarried() && !m_BCMMStaleGripSent)
   {player.BCMM_SendStaleStretcherGrip(entity);m_BCMMStaleGripSent=true;Print("BCMM_CARRY_STALE_RELEASE_SENT");}
   if (phase==9 && player.GetPlayerId()==1 && !m_BCMMGripReleaseSent)
   {player.BCMM_RequestStretcherGrip(entity,2);m_BCMMGripReleaseSent=true;Print("BCMM_CARRY_RELEASE_SENT");}
  }
  if(player && player.GetPlayerId()==1)
  {
   if(phase==1 && !m_BCMMStretcherPlaceSent && stretcher.CanPlace(player.GetControlledEntity(),patient))
   {player.BCMM_RequestStretcher(entity,patient,false);m_BCMMStretcherPlaceSent=true;Print("BCMM_STRETCHER_OWNER_PLACED_REQUEST");}
   if(phase==3 && !m_BCMMStretcherRemoveSent && stretcher.ActorCanUse(player.GetControlledEntity()))
   {player.BCMM_RequestStretcher(entity,null,true);m_BCMMStretcherRemoveSent=true;Print("BCMM_STRETCHER_OWNER_REMOVE_REQUEST");}
  }
  if(phase<=m_BCMMStretcherObserved || phase==3 || phase==5 || phase==7 || phase==9 || phase==11 || phase==15 || phase==16 || phase==19)return;
  if(phase==6)
  {
   string mirror;
   if(System.GetCLIParam("bcmmStretcherSettingsMirror",mirror))
   {
    if(GetWorld().GetWorldTime()>m_BCMMMirrorLogAt){m_BCMMMirrorLogAt=GetWorld().GetWorldTime()+1000;PrintFormat("BCMM_MIRROR_CLIENT turn=%1 handle=%2 span=%3",mode.BCMM_ReadSetting("stretcherMaximumTurnDegreesPerSecond"),mode.BCMM_ReadSetting("stretcherHandleOffsetMetres"),mode.BCMM_ReadSetting("stretcherMaximumSpanMetres"));}
    if(mode.BCMM_ReadSetting("stretcherMaximumTurnDegreesPerSecond")!=150 || Math.AbsFloat(mode.BCMM_ReadSetting("stretcherHandleOffsetMetres")-1.7)>0.001 || Math.AbsFloat(mode.BCMM_ReadSetting("stretcherMaximumSpanMetres")-3.8)>0.001)return;
    BCMM_Settings snapshot=new BCMM_Settings(); mode.BCMM_CopySettings(snapshot);
    if(snapshot.Get("stretcherMaximumTurnDegreesPerSecond")!=150 || Math.AbsFloat(snapshot.Get("stretcherHandleOffsetMetres")-1.7)>0.001 || Math.AbsFloat(snapshot.Get("stretcherMaximumSpanMetres")-3.8)>0.001)return;
    Print("BCMM_CARRY_SETTINGS_MIRROR_PASSED");
   }
  }
  if(phase==20 && world.GetWorldTime()>m_BCMMMirrorLogAt)
  {
   m_BCMMMirrorLogAt=world.GetWorldTime()+500;
   PrintFormat("BCMM_UNLOAD_CLIENT occupied=%1 distance=%2 patient=%3 station=%4",stretcher.Patient()!=null,vector.Distance(patient.GetOrigin(),entity.GetOrigin()),patient.GetOrigin(),entity.GetOrigin());
  }
  if ((phase==6 || phase==8) && (!stretcher.IsCarried() || stretcher.Patient()!=patient)) return;
  if ((phase==10 || phase==14 || phase==18) && (stretcher.IsCarried() || stretcher.HasCarriers() || stretcher.Patient()!=patient)) return;
  if(phase==2 && (stretcher.Patient()!=patient || patient.GetOrigin()[1]<entity.GetOrigin()[1]+0.2))return;
  if((phase==4 || phase==20) && (stretcher.Patient() || vector.Distance(patient.GetOrigin(),entity.GetOrigin())<0.8))return;
  if(phase==8 && player && player.GetControlledEntity()==patient)
  {
   if(!ChimeraCharacter.Cast(patient).GetRplComponent().IsOwner() || !ChimeraCharacter.Cast(patient).GetCharacterController().IsUnconscious() || vector.Distance(patient.GetOrigin(),entity.GetOrigin()+"0 0.43 0")>0.2)return;
   Print("BCMM_CARRY_PATIENT_OWNER_ANCHORED");
  }
  string appliedVisuals;
  if(System.GetCLIParam("bcmmAppliedEquipmentCheck",appliedVisuals) && (phase==6 || phase==8 || phase==10))
  {
   BCMM_CharacterPresentationComponent equipment=BCMM_CharacterPresentationComponent.Cast(patient.FindComponent(BCMM_CharacterPresentationComponent));
   SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
   int expected=8;if(phase==8) { expected=4; } if(phase==10) { expected=0; }
   if(!equipment || equipment.AppliedVisualCount()!=expected) { return; }
   if(phase==6 && (medical.BCMM_AppliedEquipment()!=63 || medical.BCMM_IVRegion()!=BCMM_BodyRegion.RIGHT_ARM || !equipment.AppliedVisualSlot(0) || !equipment.AppliedVisualSlot(6) || !equipment.AppliedVisualSlot(7))) { return; }
   if(phase==8 && (equipment.AppliedVisualSlot(0) || !equipment.AppliedVisualSlot(4) || !equipment.AppliedVisualSlot(6) || !equipment.AppliedVisualSlot(7))) { return; }
   if(phase==10 && (medical.BCMM_AppliedEquipment()!=0 || medical.BCMM_PulseMonitorActive())) { return; }
   PrintFormat("BCMM_EQUIPMENT_VISUAL_OBSERVED %1 count=%2",phase,equipment.AppliedVisualCount());
  }
  string presentation;
  if(System.GetCLIParam("bcmmGripPresentationCheck",presentation) && (phase==6 || phase==8 || phase==10))
  {
   for(int id=1;id<=2;id++)
   {
    IEntity carrier=GetPlayerManager().GetPlayerControlledEntity(id);
    if(!carrier) { return; }
    BCMM_CharacterPresentationComponent pose=BCMM_CharacterPresentationComponent.Cast(carrier.FindComponent(BCMM_CharacterPresentationComponent));
    if(!pose) { Print("BCMM_GRIP_COMPONENT_MISSING");return; }
    if(world.GetWorldTime()>m_BCMMMirrorLogAt)
    {
     if(id==2) { m_BCMMMirrorLogAt=world.GetWorldTime()+500; }
     PrintFormat("BCMM_GRIP_METRICS phase=%1 carrier=%2 weight=%3 error=%4 legs=%5",phase,id,pose.GripWeight(),pose.GripError(),pose.NativeFootTravel());
    }
    if(phase==10)
    {
     if(pose.GripWeight()>0.01) { return; }
    }
    else
    {
     if(m_BCMMGripCheckStarted<=0) { m_BCMMGripCheckStarted=world.GetWorldTime(); }
     if(pose.GripWeight()<0.99 || !(pose.GripError()>=0 && pose.GripError()<=0.08))
     {
      if(phase==6 && world.GetWorldTime()-m_BCMMGripCheckStarted>6000) { PrintFormat("BCMM_CHECK FAILED: Native carrier hand pose missed target carrier=%1 weight=%2 error=%3",id,pose.GripWeight(),pose.GripError()); }
      return;
     }
     if(phase==8 && pose.NativeFootTravel()<0.015) { return; }
    }
   }
   if(phase==10) { Print("BCMM_GRIP_RELEASE_OBSERVED 10"); }
   else { PrintFormat("BCMM_GRIP_OBSERVED %1",phase); }
  }
  m_BCMMStretcherObserved=phase;PrintFormat("BCMM_STRETCHER_OBSERVED %1",phase);
 }
}

// Feed native prepared character input, after the player's idle controls are prepared.
modded class SCR_CharacterControllerComponent
{
 protected float m_BCMMCarryWalkAt;
 override void OnPrepareControls(IEntity owner,ActionManager am,float dt,bool player)
 {
  super.OnPrepareControls(owner,am,dt,player);string observer;
  if (!player || !System.GetCLIParam("bcmmStretcherObserver",observer)) return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  PlayerController local=GetGame().GetPlayerController();
  if (!mode || mode.BCMM_StretcherPhase()!=7 || !local || local.GetPlayerId()>2 || local.GetControlledEntity()!=owner) return;
  float now=GetGame().GetWorld().GetWorldTime();if(m_BCMMCarryWalkAt==0){m_BCMMCarryWalkAt=now;PrintFormat("BCMM_CARRY_PREPARED player=%1",local.GetPlayerId());}
  GetInputContext().SetHeadingAngle(0);GetInputContext().SetAimingAngles(vector.Zero);
  SetHeadingAngle(0);
  if (now-m_BCMMCarryWalkAt<2000) { am.ActivateAction("CharacterRight");am.SetActionValue("CharacterRight",1);GetInputContext().SetMovement(1,"1 0 0"); }
 }
}

modded class SCR_PlayerController
{
 void BCMM_SendStaleStretcherGrip(IEntity entity)
 {
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.Cast(entity.FindComponent(BCMM_StretcherComponent));
  Rpc(BCMM_StretcherGripRequest,BCMM_BodyBagComponent.Id(entity),2,stretcher.GripEpoch()-1);
 }
}

class BCMM_StretcherReview
{
 static SCR_ManualCamera Camera;
 static void Update(IEntity stretcher)
 {
  vector frame[4];stretcher.GetWorldTransform(frame);
  vector origin=frame[3]+frame[0]*3.5+frame[2]*3.1+"0 1.8 0";
  vector target=frame[3]+"0 0.5 0";vector transform[4];Math3D.AnglesToMatrix((target-origin).VectorToAngles(),transform);transform[3]=origin;
  if(!Camera)
  {
   EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
   for(int i=0;i<4;i++) { spawn.Transform[i]=transform[i]; }
   Camera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));
   Camera.SetInputEnabled(false);Camera.SetVerticalFOV(45);
  }
  Camera.SetWorldTransform(transform);GetGame().GetCameraManager().SetCamera(Camera);Camera.ApplyTransform(0);
 }
}
