// Diagnostic-only native input, applied on the actual rescuer's owning client.
modded class ArmaReforgerScripted
{
 protected int m_BCMMTransportWalkPhase;
 protected float m_BCMMTransportWalkAt;
 protected float m_BCMMTransportWalkHeading;
 protected bool m_BCMMTransportWalking;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  BCMM_ApplyTransportTestInput(world);
  super.OnUpdate(world,timeslice);
 }
 protected void BCMM_ApplyTransportTestInput(BaseWorld world)
 {
  string flag;if(Replication.IsServer() || !System.GetCLIParam("bcmmLivingTransportObserver",flag))return;
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(!player || player.GetPlayerId()!=1 || !player.GetControlledEntity())return;
  IEntity owner=player.GetControlledEntity();
  CharacterControllerComponent controller=BCMM_LivingTransport.Controller(owner);
  InputManager actions=GetGame().GetInputManager();if(!actions || !controller)return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(!mode)return;
  int phase=mode.BCMM_LivingTestPhase();
  if(phase!=4 && phase!=7)
  {
   if(m_BCMMTransportWalking){actions.SetActionValue("CharacterForward",0);actions.SetActionValue("CharacterRight",0);m_BCMMTransportWalking=false;}
   return;
  }
  float now=world.GetWorldTime();
  if(m_BCMMTransportWalkPhase!=phase){m_BCMMTransportWalkPhase=phase;m_BCMMTransportWalkAt=now;m_BCMMTransportWalkHeading=owner.GetAngles()[0];}
  float elapsed=(now-m_BCMMTransportWalkAt)*0.001;
  float speed=0;if(elapsed<1.4)speed=1;
  float heading=m_BCMMTransportWalkHeading;if(elapsed>0.4)heading+=90;
  actions.ActivateContext("CharacterMovementContext");actions.SetActionValue("CharacterForward",speed);
  actions.SetActionValue("CharacterSprint",0);
  if(elapsed<1.4){controller.SetHeadingAngle(heading*Math.DEG2RAD,true);controller.GetInputContext().SetHeadingAngle(heading*Math.DEG2RAD);}
  m_BCMMTransportWalking=true;
 }
}
