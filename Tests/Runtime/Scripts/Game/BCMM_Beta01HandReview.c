// Native rendered hand-placement evidence. Screenshots require visual review;
// successful attachment alone does not certify a correct grip.
class BCMM_Beta01HandReview : BCMM_SplintRuntimeChecks
{
 protected ref array<ResourceName> m_Items;
 protected int m_Index;
 protected int m_Wait;
 protected int m_Captured;
 protected int m_View;
 protected int m_ToggleCycles;
 protected SCR_ManualCamera m_Camera;
 override protected void Prepare()
 {
  FileIO.MakeDirectory("$profile:screenshots");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Target);m_Target=null;
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Hand review receives native storage");
  m_Items=BCMM_Beta01ItemResources.All();string hold;if(System.GetCLIParam("bcmmBeta01HandHold",hold))m_Index=10;GetGame().GetCallqueue().CallLater(NextItem,1000);
 }
 protected void NextItem()
 {
  if(m_Index>=m_Items.Count())
  {
   m_Suite.Record(m_Captured==26,"All 26 handheld entries have three native captures");
   PrintFormat("BCMM_HAND_REVIEW_COMPLETE items=%1",m_Captured);if(m_Camera)delete m_Camera;Finish();return;
  }
  m_Kit=Spawn(m_Items[m_Index],m_Actor.GetOrigin());
  if(!m_Kit || !m_Kit.FindComponent(SCR_GadgetComponent))
  {
   if(m_Kit)SCR_EntityHelper.DeleteEntityAndChildren(m_Kit);m_Index++;NextItem();return;
  }
  m_Suite.Record(m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Hand review inserts exact original supply");
  m_Wait=0;GetGame().GetCallqueue().CallLater(EquipReview,300);
 }
 protected void EquipReview()
 {
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Kit,EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(CheckHand,100);
 }
 protected void CheckHand()
 {
  if(ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot()!=m_Kit)
  {
   if(m_Wait++<80){GetGame().GetCallqueue().CallLater(CheckHand,100);return;}
   m_Suite.Record(false,"Hand review reaches actual gadget hand slot");Finish();return;
  }
  m_View=0;GetGame().GetCallqueue().CallLater(ShowHand,1400);
 }
 protected void ShowHand()
 {
  if(m_View==2)
  {
   CameraBase playerCamera=GetGame().GetPlayerController().GetPlayerCamera();
   m_Suite.Record(playerCamera && GetGame().GetCameraManager().SetCamera(playerCamera),"Hand review restores native player camera");
  }
  else
  {
   vector bone[4];m_Actor.GetAnimation().GetBoneMatrix(m_Actor.GetAnimation().GetBoneIndex("LeftHand"),bone);
   vector body[4];m_Actor.GetWorldTransform(body);vector centre=BCMM_GripMath.Point(body,bone[3]);
   vector offset=body[2]*0.65+body[0]*0.3+"0 0.12 0";
   if(m_View==1)offset=body[0]*0.7-body[2]*0.15+"0 0.1 0";
   EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
   Math3D.AnglesToMatrix((-offset).VectorToAngles(),spawn.Transform);spawn.Transform[3]=centre+offset;
   if(!m_Camera)m_Camera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));
   m_Camera.SetWorldTransform(spawn.Transform);m_Camera.SetInputEnabled(false);m_Camera.SetVerticalFOV(45);
   GetGame().GetCameraManager().SetCamera(m_Camera);m_Camera.ApplyTransform(0);
  }
  GetGame().GetCallqueue().CallLater(CaptureHand,450);
 }
 protected void CaptureHand()
 {
  System.MakeScreenshot(string.Format("$profile:screenshots/hand-%1-view-%2",m_Index,m_View));
  if(++m_View<3){ShowHand();return;}
  PrintFormat("BCMM_HAND_REVIEW item=%1 resource=%2 views=3",m_Index,m_Items[m_Index]);m_Captured++;
  string hold;if(System.GetCLIParam("bcmmBeta01HandHold",hold))
  {
   if(m_Camera){delete m_Camera;m_Camera=null;}Print("BCMM_HINT_KEY_READY");
   if(System.GetCLIParam("bcmmBeta01HintAuto",hold))
   {
    BCMM_MedicalInput.BCMM_InvokeOpenHandler();
    GetGame().GetCallqueue().CallLater(CheckHeldMenu,500);
   }
   return;
  }
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).RemoveHeldGadget();m_Wait=0;
  GetGame().GetCallqueue().CallLater(WaitStow,150);
 }
 protected void WaitStow()
 {
  if(ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetAttachedGadgetAtLeftHandSlot())
  {
   if(m_Wait++<80){GetGame().GetCallqueue().CallLater(WaitStow,100);return;}
   m_Suite.Record(false,"Hand review stows original supply");Finish();return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Kit);m_Index++;NextItem();
 }
 protected void CheckHeldMenu()
 {
  BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
  m_Suite.Record(menu && menu.GetRootWidget(),"Production medical input handler opens native menu while injector is held");
  m_Suite.Record(menu && menu.BCMM_ToggleActivatedDuringUpdate(),"Native menu update activates the rebindable medical toggle");
  m_Suite.Record(m_Inventory.Contains(m_Kit),"Opening medical menu preserves the exact original injector without consumption");
  System.MakeScreenshot("$profile:screenshots/held-injector-medical-menu");
  BCMM_MedicalInput.BCMM_InvokeOpenHandler();
  GetGame().GetCallqueue().CallLater(CheckToggleClosed,500);
 }
 protected void CheckToggleClosed()
 {
  m_Suite.Record(!BCMM_MedicalMenu.Current(),"Invoking the same production medical toggle closes the open menu");
  string countText;int total=1;
  if(System.GetCLIParam("bcmmBeta01ToggleCycles",countText))total=Math.ClampInt(countText.ToInt(),1,200);
  m_ToggleCycles++;
  if(m_ToggleCycles>=total){PrintFormat("BCMM_TOGGLE_CYCLES_COMPLETE cycles=%1",m_ToggleCycles);GetGame().GetCallqueue().CallLater(Finish,500);return;}
  BCMM_MedicalInput.BCMM_InvokeOpenHandler();
  GetGame().GetCallqueue().CallLater(CheckToggleReopened,500);
 }
 protected void CheckToggleReopened()
 {
  BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
  m_Suite.Record(menu!=null,"Medical toggle reopens cleanly after closure");
  BCMM_MedicalInput.BCMM_InvokeOpenHandler();GetGame().GetCallqueue().CallLater(CheckToggleClosed,500);
 }
}
