// Diagnostic-only native action placement on an actually recovered casualty.
modded class BCMM_BaggingRuntimeChecks
{
 override protected void DragRecovered()
 {
  string enabled;
  if(!System.GetCLIParam("bcmmBeta01BagActions",enabled)){super.DragRecovered();return;}
  ActionsManagerComponent manager=ActionsManagerComponent.Cast(m_Bag.FindComponent(ActionsManagerComponent));
  UserActionContext context;if(manager)context=manager.GetContext("bcmm_bag");
  m_Suite.Record(context && context.GetActionsCount()==8,"Recovered bag exposes all eight actions through one native context");
  if(!context){Finish();return;}
  vector transform[4];context.GetTransformationWorld(transform);
  vector low,high;m_Bag.GetWorldBounds(low,high);vector centre=(low+high)*0.5;
  PrintFormat("BCMM_BAG_ACTION_ANCHOR point=%1 low=%2 high=%3 centre=%4",transform[3],low,high,centre);
  m_Suite.Record(Math.AbsFloat(transform[3][0]-centre[0])<0.2 && Math.AbsFloat(transform[3][2]-centre[2])<0.2,"Native bag action anchor lies centrally within the rendered bag footprint");
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  player.SetControlledEntity(m_Actor);
  vector pose[4];m_Actor.GetWorldTransform(pose);pose[3]=transform[3]+"0 0.3 -1.3";
  ChimeraCharacter.Cast(m_Actor).Teleport(pose);
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();controller.SetHeadingAngle(0,true);
  if(player.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
  GetGame().GetCallqueue().CallLater(BCMM_AimBagActions,1200);
 }
 protected void BCMM_AimBagActions()
 {
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  ScriptedCameraItem camera=controller.GetCameraHandlerComponent().GetCurrentCamera();
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(player.GetPlayerCamera())GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
  vector angles="55 0 0";if(camera)camera.SetBaseAngles(angles);
  PrintFormat("BCMM_BAG_ACTION_CAMERA controlled=%1 expected=%2 camera=%3 position=%4",player.GetControlledEntity(),m_Actor,GetGame().GetCameraManager().CurrentCamera(),GetGame().GetCameraManager().CurrentCamera().GetOrigin());
  BCMM_BagActionAim.Stage=1;GetGame().GetCallqueue().CallLater(BCMM_CaptureBagActions,1500);
 }
 protected void BCMM_CaptureBagActions()
 {
  vector transform[4];GetGame().GetCameraManager().CurrentCamera().GetWorldTransform(transform);
  m_Suite.Record(transform[2][1]<-0.5,"Native character input aims the actual player camera down toward the bag");
  FileIO.MakeDirectory("$profile:screenshots");System.MakeScreenshot("$profile:screenshots/recovered-bag-native-actions");
  BCMM_BagActionAim.Stage=-1;
  GetGame().GetCallqueue().CallLater(Finish,500);
 }
}

class BCMM_BagActionAim {static int Stage=-1;}
modded class SCR_CharacterControllerComponent
{
 override void OnPrepareControls(IEntity owner,ActionManager am,float dt,bool player)
 {
  super.OnPrepareControls(owner,am,dt,player);
  if(!player || BCMM_BagActionAim.Stage<0)return;
  vector angles="0 -0.9 0";
  GetInputContext().SetAimingAngles(angles);
 }
}
