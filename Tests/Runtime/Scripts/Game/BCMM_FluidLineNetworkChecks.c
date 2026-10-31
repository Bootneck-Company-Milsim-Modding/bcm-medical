modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMLinePhase;
 [RplProp()] protected RplId m_BCMMLinePatient=RplId.Invalid();
 void BCMM_PublishLineTest(int phase,IEntity patient)
 {
  m_BCMMLinePhase=phase;
  m_BCMMLinePatient=BCMM_BodyBagComponent.Id(patient);
  Replication.BumpMe();
  PrintFormat("BCMM_LINE_PHASE %1",phase);
 }
 int BCMM_LinePhase()
 {
  return m_BCMMLinePhase;
 }
 IEntity BCMM_LinePatient()
 {
  return BCMM_BodyBagComponent.Resolve(m_BCMMLinePatient);
 }
}
modded class BCMM_MedicalMenu
{
 bool BCMM_TestLineConnectAvailable()
 {
  SelectRegion(BCMM_BodyRegion.LEFT_ARM);
  SelectTab(6);
  Widget row=GetRootWidget().FindAnyWidget("Action5");
  return row && row.IsEnabled() && TextWidget.Cast(row.FindAnyWidget("Text")).GetText().IndexOf("Connect whole blood drip")==0;
 }
 bool BCMM_TestLineRemovalAvailable()
 {
  SelectRegion(BCMM_BodyRegion.LEFT_ARM);
  SelectTab(6);
  MoreActions();
  Widget row=GetRootWidget().FindAnyWidget("Action2");
  return m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+BCMM_EquipmentKind.FLUID]==0 && row && row.IsEnabled() && TextWidget.Cast(row.FindAnyWidget("Text")).GetText()=="Disconnect fluid line";
 }
}
class BCMM_FluidLineNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected InventoryStorageManagerComponent m_PatientInventory;
 protected float m_StartContents;
 protected int m_LineWait;
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity())
  {
   if(m_OwnerWait++<100)
   {
    GetGame().GetCallqueue().CallLater(Prepare,250);
    return;
   }
   m_Suite.Record(false,"Line network fixture receives actual owner");
   Finish();
   return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Actor=player.GetControlledEntity();
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.25 0");
  m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
  if(ai)
  {
   ai.DeactivateAI();
  }
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_PatientInventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  IEntity actorPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  IEntity patientPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
  m_Kit=Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_Actor.GetOrigin());
  m_Suite.Record(actorPack && m_Inventory.TryInsertItem(actorPack,EStoragePurpose.PURPOSE_ANY) && patientPack && m_PatientInventory.TryInsertItem(patientPack,EStoragePurpose.PURPOSE_ANY) && m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Actual network owner holds original finite bag and patient has real native capacity");
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  BCMM_VisualStatusBridge.Publish(m_Medical);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("fluidLineConnectionSeconds",3);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("fluidLineRemovalSeconds",3);
  GetGame().GetCallqueue().CallLater(PublishLineReady,1000);
 }
 protected void PublishLineReady()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLineTest(1,m_Target);
  WaitConnected();
 }
 protected void WaitConnected()
 {
  if(BCMM_FluidLineSession.ForPatient(m_Target))
  {
   m_Suite.Record(m_PatientInventory.Contains(m_Kit) && !m_Inventory.Contains(m_Kit),"Actual owner menu/RPC moves original source into patient inventory");
   m_StartContents=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl();
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLineTest(2,m_Target);
   GetGame().GetCallqueue().CallLater(CheckIndependentLine,16000);
   return;
  }
  if(m_LineWait++<120)
  {
   GetGame().GetCallqueue().CallLater(WaitConnected,250);
   return;
  }
  m_Suite.Record(false,"Actual owner connection RPC establishes the independent line");
  Finish();
 }
 protected void CheckIndependentLine()
 {
  float remaining=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl();
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && remaining<m_StartContents-50 && remaining>0 && !BCMM_TreatmentAccess.CanReach(m_Actor,m_Target) && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Dedicated original patient-owned bag continues finite flow while actual owner has left reach and freed both hands");
  m_LineWait=0;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLineTest(3,m_Target);
  WaitDisconnected();
 }
 protected void WaitDisconnected()
 {
  if(!BCMM_FluidLineSession.ForPatient(m_Target))
  {
   m_Suite.Record(m_PatientInventory.Contains(m_Kit) && BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()>0 && !BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.FLUID),"Actual owner line removal retains original partial bag in patient inventory without creating a medic supply");
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLineTest(4,m_Target);
   GetGame().GetCallqueue().CallLater(Finish,8000);
   return;
  }
  if(m_LineWait++<80)
  {
   GetGame().GetCallqueue().CallLater(WaitDisconnected,250);
   return;
  }
  m_Suite.Record(false,"Actual empty-handed owner disconnects through native menu/RPC");
  Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMLineOpened;
 protected bool m_BCMMLineSent;
 protected bool m_BCMMLineLeft;
 protected bool m_BCMMLineReturned;
 protected bool m_BCMMLineRemovalSent;
 protected int m_BCMMLineObserved;
 protected float m_BCMMLineReadyAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  string observer;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmLineObserver",observer))
  {
   return;
  }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player || !player.GetControlledEntity() || !mode.BCMM_HasSettingsSnapshot())
  {
   return;
  }
  RplComponent ownership=RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent));
  if(player.GetPlayerId()==1 && (!ownership || !ownership.IsOwner()))
  {
   return;
  }
  IEntity target=mode.BCMM_LinePatient();
  int phase=mode.BCMM_LinePhase();
  if(!target || phase < 1)
  {
   return;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical)
  {
   return;
  }
  IEntity actor=player.GetControlledEntity();
  if(player.GetPlayerId()==1)
  {
   if(phase==1 && !m_BCMMLineOpened)
   {
    vector initialPose[4];
    actor.GetWorldTransform(initialPose);
    initialPose[3]=target.GetOrigin()+"-1 0.1 0";
    ChimeraCharacter.Cast(actor).Teleport(initialPose);
    BCMM_MedicalMenu.Open(target);
    m_BCMMLineOpened=true;
    m_BCMMLineReadyAt=world.GetWorldTime()+2000;
   }
   if(phase==1 && !m_BCMMLineSent && world.GetWorldTime()>m_BCMMLineReadyAt)
   {
    BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
    if(menu && menu.BCMM_TestLineConnectAvailable())
    {
     menu.UseEquipment(BCMM_EquipmentProcedure.CONNECT_WHOLE_BLOOD_LINE);
     menu.Close();
     m_BCMMLineSent=true;
     Print("BCMM_LINE_OWNER_CONNECT_SENT");
    }
   }
   if(phase==2 && !m_BCMMLineLeft)
   {
    vector pose[4];
    actor.GetWorldTransform(pose);
    pose[3]=target.GetOrigin()+"4 0.1 0";
    ChimeraCharacter.Cast(actor).Teleport(pose);
    m_BCMMLineLeft=true;
    Print("BCMM_LINE_OWNER_LEFT_RANGE");
   }
   if(phase==3 && !m_BCMMLineReturned)
   {
    vector pose[4];
    actor.GetWorldTransform(pose);
    pose[3]=target.GetOrigin()+"-1 0.1 0";
    ChimeraCharacter.Cast(actor).Teleport(pose);
    BCMM_MedicalMenu.Open(target);
    m_BCMMLineReadyAt=world.GetWorldTime()+1500;
    m_BCMMLineReturned=true;
   }
   if(phase==3 && !m_BCMMLineRemovalSent && world.GetWorldTime()>m_BCMMLineReadyAt)
   {
    BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
    if(menu && menu.BCMM_TestLineRemovalAvailable() && BCMM_TreatmentAccess.CanReach(actor,target))
    {
     menu.UseEquipment(BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE);
     menu.Close();
     m_BCMMLineRemovalSent=true;
     Print("BCMM_LINE_OWNER_REMOVAL_SENT zeroBags=1");
    }
   }
  }
  bool ready=(phase==2 && medical.BCMM_FluidLineActive() && medical.BCMM_FluidLineRemainingMl()>0 && medical.BCMM_FluidLineRemainingMl()<490 && medical.GetBloodHitZone().GetHealthScaled()>0.701) || (phase==4 && !medical.BCMM_FluidLineActive());
  if(phase!=m_BCMMLineObserved && ready)
  {
   m_BCMMLineObserved=phase;
   PrintFormat("BCMM_LINE_OBSERVED %1 player=%2 remaining=%3",phase,player.GetPlayerId(),medical.BCMM_FluidLineRemainingMl());
  }
 }
}
