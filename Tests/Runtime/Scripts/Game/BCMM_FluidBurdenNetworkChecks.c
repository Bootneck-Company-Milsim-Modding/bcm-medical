// Staged real owner policy rejection and live native UI/proxy/JIP gates.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMBurdenPhase;
 [RplProp()] protected RplId m_BCMMBurdenPatient=RplId.Invalid();
 void BCMM_PublishBurdenTest(int phase,IEntity patient)
 {
  m_BCMMBurdenPhase=phase;
  m_BCMMBurdenPatient=BCMM_BodyBagComponent.Id(patient);
  Replication.BumpMe();
  PrintFormat("BCMM_BURDEN_PHASE %1",phase);
 }
 int BCMM_BurdenPhase()
 {
  return m_BCMMBurdenPhase;
 }
 IEntity BCMM_BurdenPatient()
 {
  return BCMM_BodyBagComponent.Resolve(m_BCMMBurdenPatient);
 }
}
modded class SCR_PlayerController
{
 protected bool m_BCMMBurdenRequestDenied;
 bool BCMM_TestBurdenRequestDenied()
 {
  return m_BCMMBurdenRequestDenied;
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)] override protected void BCMM_EquipmentRequest(RplId target,int action,int region,int view)
 {
  string diagnostic;
  if(System.GetCLIParam("bcmmFluidBurdenNetwork",diagnostic) && action==BCMM_EquipmentProcedure.INFUSE_SALINE)
  {
   IEntity patient=BCMM_BodyBagComponent.Resolve(target);
   SCR_CharacterDamageManagerComponent medical;
   if(patient)
   {
    medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
   }
   if(medical)
   {
    BCMM_Settings settings=new BCMM_Settings();
    SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_CopySettings(settings);
    m_BCMMBurdenRequestDenied=medical.BCMM_GetPatient().BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,settings) && BCMM_TreatmentAccess.CanReach(GetControlledEntity(),patient) && BCMM_EquipmentTreatment.FindEquipment(GetControlledEntity(),BCMM_EquipmentKind.SALINE)!=null;
   }
  }
  super.BCMM_EquipmentRequest(target,action,region,view);
 }
}
modded class BCMM_MedicalMenu
{
 void BCMM_TestPrepareBurdenUI()
 {
  SelectRegion(BCMM_BodyRegion.LEFT_ARM);
  SelectTab(6);
 }
 void BCMM_TraceBurdenUI()
 {
  Widget row=GetRootWidget().FindAnyWidget("Action3");
  PrintFormat("BCMM_BURDEN_UI_TRACE count=%1 procedure=%2 reach=%3 row=%4 enabled=%5 region=%6 tab=%7 actor=%8 target=%9",m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+BCMM_EquipmentKind.SALINE],Player().BCMM_HasProcedure(),BCMM_TreatmentAccess.CanReach(Player().GetControlledEntity(),Target()),row!=null,row && row.IsEnabled(),m_Region,m_Tab,Player().GetControlledEntity().GetOrigin(),Target().GetOrigin());
 }
 bool BCMM_TestWaterButton(bool expectedEnabled)
 {
  Widget row=GetRootWidget().FindAnyWidget("Action3");
  return m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+BCMM_EquipmentKind.SALINE]>0 && Player() && !Player().BCMM_HasProcedure() && BCMM_TreatmentAccess.CanReach(Player().GetControlledEntity(),Target()) && row && row.IsEnabled()==expectedEnabled;
 }
}
class BCMM_FluidBurdenNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_InducingBag;
 protected InventoryStorageManagerComponent m_PatientInventory;
 protected SCR_BaseGameMode m_Mode;
 protected int m_Phase;
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
   m_Suite.Record(false,"Burden network receives an actual owning client");
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
  m_Mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Suite.Record(m_Mode.BCMM_TestRoleSetting("fluidOverloadEnabled",1) && m_Mode.BCMM_TestRoleSetting("fluidOverloadThresholdFraction",0.01) && m_Mode.BCMM_TestRoleSetting("fluidOverloadSevereFraction",0.02) && m_Mode.BCMM_TestRoleSetting("fluidOverloadClearanceSeconds",60) && m_Mode.BCMM_TestRoleSetting("fluidRedistributionSeconds",30) && m_Mode.BCMM_TestRoleSetting("fluidInfusionSeconds",3) && m_Mode.BCMM_TestRoleSetting("oxygenChangePerSecond",0.05) && m_Mode.BCMM_TestRoleSetting("automaticUnconsciousAirwayObstruction",0),"Connected burden fixture uses valid live gameplay settings");
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  BCMM_VisualStatusBridge.Publish(m_Medical);
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_PatientInventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  IEntity patientPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
  m_Kit=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Actor.GetOrigin());
  m_InducingBag=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Target.GetOrigin());
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && patientPack && m_PatientInventory.TryInsertItem(patientPack,EStoragePurpose.PURPOSE_ANY),"Owner and actual patient receive real inventory capacity");
  GetGame().GetCallqueue().CallLater(StartNativeBurden,1500);
 }
 protected void StartNativeBurden()
 {
  m_Suite.Record(m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY) && m_PatientInventory.TryInsertItem(m_InducingBag,EStoragePurpose.PURPOSE_ANY) && BCMM_FluidLineSession.Start(m_Target,m_InducingBag,BCMM_BodyRegion.LEFT_ARM),"Connected fixture genuinely delivers one original finite saline source while preserving a separate owning-player source");
  GetGame().GetCallqueue().CallLater(PublishInitialBurden,18000);
 }
 protected void PublishInitialBurden()
 {
  Publish(1,18000);
 }
 protected void Publish(int phase,int delay)
 {
  m_Phase=phase;
  m_Mode.BCMM_PublishBurdenTest(phase,m_Target);
  GetGame().GetCallqueue().CallLater(NextBurdenPhase,delay);
 }
 protected void NextBurdenPhase()
 {
  if(m_Phase==1)
  {
   SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
   PrintFormat("BCMM_BURDEN_SERVER_SOURCE contains=%1 remaining=%2 found=%3 reach=%4",m_Inventory.Contains(m_Kit),BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl(),BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.SALINE)==m_Kit,BCMM_TreatmentAccess.CanReach(m_Actor,m_Target));
   m_Suite.Record(owner && owner.BCMM_TestBurdenRequestDenied() && m_Inventory.Contains(m_Kit) && BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500 && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Actual in-range owner RPC cannot bypass burden policy, debit its real source or start a procedure");
   m_Mode.BCMM_TestRoleSetting("fluidOverloadEnabled",0);
   Publish(2,15000);
   return;
  }
  if(m_Phase==2)
  {
   m_Suite.Record(m_Medical.BCMM_GetPatient().BCMM_FluidBurdenFraction()>0 && !m_Medical.BCMM_WaterLoadingWithheld() && m_Medical.BCMM_OxygenSaturation()>0.9,"Live disable suppresses effects and water gate without silently erasing stored burden");
   m_Mode.BCMM_TestRoleSetting("fluidOverloadEnabled",1);
   Publish(3,12000);
   return;
  }
  m_Suite.Record(m_Medical.BCMM_WaterLoadingWithheld() && m_Medical.BCMM_GetPatient().BCMM_FluidBurdenFraction()>0 && m_Inventory.Contains(m_Kit) && BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500,"Re-enabled policy restores the gate from retained actual state without consuming a blocked source");
  Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMBurdenOpened;
 protected bool m_BCMMBurdenSent;
 protected int m_BCMMBurdenObserved;
 protected int m_BCMMBurdenUIObserved;
 protected float m_BCMMBurdenReadyAt;
 protected float m_BCMMBurdenTraceAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  string observer;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmBurdenObserver",observer))
  {
   return;
  }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player || !player.GetControlledEntity() || !mode.BCMM_HasSettingsSnapshot())
  {
   return;
  }
  IEntity target=mode.BCMM_BurdenPatient();
  int phase=mode.BCMM_BurdenPhase();
  if(!target || phase<1)
  {
   return;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical)
  {
   return;
  }
  if(player.GetPlayerId()==1)
  {
   RplComponent ownership=RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent));
   if(!ownership || !ownership.IsOwner())
   {
    return;
   }
   if(!m_BCMMBurdenOpened)
   {
    vector pose[4];
    player.GetControlledEntity().GetWorldTransform(pose);
    pose[3]=target.GetOrigin()+"-1 0.1 0";
    ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(pose);
    BCMM_MedicalMenu.Open(target);
    BCMM_MedicalMenu.Current().BCMM_TestPrepareBurdenUI();
    m_BCMMBurdenReadyAt=world.GetWorldTime()+2000;
    m_BCMMBurdenOpened=true;
   }
   BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
   bool expectEnabled=phase==2;
   if(menu && world.GetWorldTime()>m_BCMMBurdenTraceAt)
   {
    m_BCMMBurdenTraceAt=world.GetWorldTime()+1000;
    menu.BCMM_TraceBurdenUI();
   }
   if(menu && phase!=m_BCMMBurdenUIObserved && world.GetWorldTime()>m_BCMMBurdenReadyAt && menu.BCMM_TestWaterButton(expectEnabled) && medical.BCMM_WaterLoadingWithheld()!=expectEnabled)
   {
    m_BCMMBurdenUIObserved=phase;
    PrintFormat("BCMM_BURDEN_OWNER_UI %1",phase);
    if(phase==1 && !m_BCMMBurdenSent)
    {
     menu.UseEquipment(BCMM_EquipmentProcedure.INFUSE_SALINE);
     m_BCMMBurdenSent=true;
     Print("BCMM_BURDEN_OWNER_BYPASS_SENT");
    }
   }
  }
  bool ready=(phase==2 && !medical.BCMM_WaterLoadingWithheld() && mode.BCMM_ReadSetting("fluidOverloadEnabled")==0 && medical.BCMM_OxygenSaturation()>0.9) || (phase!=2 && medical.BCMM_WaterLoadingWithheld() && mode.BCMM_ReadSetting("fluidOverloadEnabled")==1 && medical.BCMM_OxygenSaturation()<0.9);
  if(phase!=m_BCMMBurdenObserved && ready)
  {
   m_BCMMBurdenObserved=phase;
   PrintFormat("BCMM_BURDEN_OBSERVED %1 player=%2 oxygen=%3",phase,player.GetPlayerId(),medical.BCMM_OxygenSaturation());
  }
 }
}
