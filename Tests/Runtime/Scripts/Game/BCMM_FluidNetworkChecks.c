// Diagnostic-only actual owner requests and owner/proxy/late-join physiology.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMFluidPhase;
 [RplProp()] protected RplId m_BCMMFluidBagId=RplId.Invalid();
 [RplProp()] protected RplId m_BCMMFluidPatient=RplId.Invalid();
 void BCMM_PublishFluidTest(int phase,IEntity patient,IEntity bag=null) { m_BCMMFluidBagId=BCMM_BodyBagComponent.Id(bag);m_BCMMFluidPhase=phase;m_BCMMFluidPatient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();PrintFormat("BCMM_FLUID_PHASE %1",phase); }
 IEntity BCMM_FluidBag() { return BCMM_BodyBagComponent.Resolve(m_BCMMFluidBagId); }
 int BCMM_FluidPhase() { return m_BCMMFluidPhase; }
 IEntity BCMM_FluidPatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMFluidPatient); }
}
class BCMM_FluidNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Phase;
 protected float m_LastNativeVolume=0.5;
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity()) { if(m_OwnerWait++<100) { GetGame().GetCallqueue().CallLater(Prepare,250);return; }m_Suite.Record(false,"Fluid network fixture receives an owner");Finish();return; }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=player.GetControlledEntity();
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  vector targetMatrix[4];m_Target.GetWorldTransform(targetMatrix);targetMatrix[3]="101 2.35 0";ChimeraCharacter.Cast(m_Target).Teleport(targetMatrix);
  vector actorMatrix[4];m_Actor.GetWorldTransform(actorMatrix);actorMatrix[3]="100 2.35 0";ChimeraCharacter.Cast(m_Actor).Teleport(actorMatrix);
  AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Fluid owner receives real inventory");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_TestRoleSetting("fluidInfusionSeconds",3);mode.BCMM_TestRoleSetting("salineRetainedFraction",0.3);mode.BCMM_TestRoleSetting("plasmaRetainedFraction",0.9);mode.BCMM_TestRoleSetting("fluidRedistributionSeconds",450);
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.5);m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;BCMM_VisualStatusBridge.Publish(m_Medical);
  GetGame().GetCallqueue().CallLater(PublishBaseline,2000);
 }
 protected void PublishBaseline() { m_Phase=1;SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishFluidTest(1,m_Target);GetGame().GetCallqueue().CallLater(Next,18000); }
 protected void Next()
 {
  if(m_Phase>=2 && m_Phase<=4)
  {
   BCMM_Patient patient=m_Medical.BCMM_GetPatient();
   PrintFormat("BCMM_FLUID_SERVER_CHECK phase=%1 kit=%2 contained=%3 reach=%4 busy=%5 volume=%6",m_Phase,m_Kit!=null,m_Kit && m_Inventory.Contains(m_Kit),BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),m_Medical.GetBloodHitZone().GetHealthScaled());
   m_Suite.Record(!m_Kit && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Actual owner typed-fluid RPC consumes exactly its reserved bag");
   float currentVolume=m_Medical.GetBloodHitZone().GetHealthScaled();
   m_Suite.Record(currentVolume>m_LastNativeVolume+0.09 && currentVolume<=m_LastNativeVolume+0.101 && Math.AbsFloat(currentVolume-patient.Fluids.LastVolume)<0.001,"Owner fluid RPC changes finite native volume once while accounting for ongoing water redistribution");m_LastNativeVolume=currentVolume;
   float expectedRed=0.5;if(m_Phase==4) { expectedRed=0.6; }
   float expectedClot=0.5;if(m_Phase>=3) { expectedClot=0.5+(m_Phase-2)*0.1; }
   m_Suite.Record(Math.AbsFloat(patient.Fluids.RedCellReserve-expectedRed)<0.001 && Math.AbsFloat(patient.Fluids.ClottingReserve-expectedClot)<0.001,"Owner-selected fluid preserves correct component accounting on authority");
   m_Suite.Record(patient.OxygenSaturation>0.97 && patient.HasSpontaneousCirculation,"Owner treatment keeps stable ventilation and circulation");
  }
  m_Phase++;
  if(m_Phase==5) { m_Medical.FullHeal();SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishFluidTest(5,m_Target);GetGame().GetCallqueue().CallLater(Finish,6000);return; }
  array<ResourceName> bags={"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et","{B6D83F6853C68D73}Prefabs/Items/BCMM_Plasma.et","{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et"};
  m_Kit=Spawn(bags[m_Phase-2],m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Authority inserts distinct replicated bag for owner request");
  PrintFormat("BCMM_FLUID_SERVER_BAG actor=%1 id=%2 kind=%3",BCMM_BodyBagComponent.Id(m_Actor),BCMM_BodyBagComponent.Id(m_Kit),BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).Kind);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishFluidTest(m_Phase,m_Target,m_Kit);GetGame().GetCallqueue().CallLater(Next,25000);
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMFluidObserved;
 protected int m_BCMMFluidContentsObserved;
 protected int m_BCMMFluidProgressObserved;
 protected int m_BCMMFluidRequested;
 protected bool m_BCMMFluidPositioned;
 protected float m_BCMMFluidTraceAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;if(Replication.IsServer() || !System.GetCLIParam("bcmmFluidObserver",observer)) { return; }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());if(!mode || !player || !player.GetControlledEntity()) { return; }
  IEntity target=mode.BCMM_FluidPatient();int phase=mode.BCMM_FluidPhase();if(!target || phase<=0) { return; }
  if(!m_BCMMFluidPositioned)
  {
   if(player.GetPlayerId()==1) { vector clientMatrix[4];player.GetControlledEntity().GetWorldTransform(clientMatrix);clientMatrix[3]=target.GetOrigin()+"0 0.1 -1.2";ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(clientMatrix); }m_BCMMFluidPositioned=true;
   if(player.GetPlayerId()==1) { BCMM_MedicalMenu.Open(target);BCMM_MedicalMenu.Current().SelectRegion(BCMM_BodyRegion.LEFT_ARM);BCMM_MedicalMenu.Current().BCMM_TestFluidTab(); }
  }
  bool settings=mode.BCMM_HasSettingsSnapshot() && mode.BCMM_ReadSetting("fluidInfusionSeconds")==3 && Math.AbsFloat(mode.BCMM_ReadSetting("salineRetainedFraction")-0.3)<0.001 && Math.AbsFloat(mode.BCMM_ReadSetting("plasmaRetainedFraction")-0.9)<0.001 && mode.BCMM_ReadSetting("fluidRedistributionSeconds")==450;
  if(!settings) { return; }
  if(phase>=2 && phase<=4 && player.GetPlayerId()==1 && m_BCMMFluidRequested<phase && BCMM_TreatmentAccess.CanReach(player.GetControlledEntity(),target))
  {
   RplComponent nativeOwner=RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent));if(!nativeOwner || !nativeOwner.IsOwner()) { return; }
   int action=BCMM_EquipmentProcedure.INFUSE_SALINE;if(phase==3) { action=BCMM_EquipmentProcedure.INFUSE_PLASMA; }if(phase==4) { action=BCMM_EquipmentProcedure.INFUSE; }
   if(world.GetWorldTime()>m_BCMMFluidTraceAt)
   {
    m_BCMMFluidTraceAt=world.GetWorldTime()+1000;
    InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(player.GetControlledEntity().FindComponent(InventoryStorageManagerComponent));array<IEntity> items={};inventory.GetItems(items,EStoragePurpose.PURPOSE_ANY);
    PrintFormat("BCMM_FLUID_OWNER_INVENTORY phase=%1 actor=%2 count=%3 kind=%4",phase,BCMM_BodyBagComponent.Id(player.GetControlledEntity()),items.Count(),BCMM_EquipmentPolicy.Kind(action));
    foreach(IEntity item:items) { BCMM_EquipmentItemComponent marker=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));if(marker) { PrintFormat("BCMM_FLUID_OWNER_ITEM kind=%1 id=%2",marker.Kind,BCMM_BodyBagComponent.Id(item)); } }
   }

   BCMM_MedicalMenu.Current().UseEquipment(action);m_BCMMFluidRequested=phase;Print("BCMM_FLUID_NATIVE_OWNER");PrintFormat("BCMM_FLUID_OWNER_SENT %1",phase);
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));if(!medical || !medical.GetBloodHitZone()) { return; }
  if(phase>=2 && phase<=4)
  {
   float currentVolume=medical.GetBloodHitZone().GetHealthScaled();
   float baseline=0.5+(phase-2)*0.1;
   if(m_BCMMFluidProgressObserved<phase && currentVolume>baseline+0.01 && currentVolume<baseline+0.085)
   {
    m_BCMMFluidProgressObserved=phase;PrintFormat("BCMM_FLUID_PROGRESS_OBSERVED %1 reserve=%2",phase,currentVolume);
   }
   if(player.GetPlayerId()==1 && m_BCMMFluidContentsObserved<phase)
   {
    IEntity bag=mode.BCMM_FluidBag();BCMM_EquipmentItemComponent source;
    if(bag)source=BCMM_EquipmentItemComponent.Cast(bag.FindComponent(BCMM_EquipmentItemComponent));
    if(source && source.FluidCapacityMl()==500 && source.FluidRemainingMl()>50 && source.FluidRemainingMl()<450)
    {
     m_BCMMFluidContentsObserved=phase;PrintFormat("BCMM_FLUID_CONTENTS_OBSERVED %1 remainingMl=%2",phase,source.FluidRemainingMl());
    }
   }
  }
  if(phase<=m_BCMMFluidObserved) { return; }
  float expected=0.5+(phase-1)*0.1;float expectedPulse=115;if(phase==4) { expectedPulse=107; }if(phase==5) { expected=1;expectedPulse=75; }
  if(Math.AbsFloat(medical.GetBloodHitZone().GetHealthScaled()-expected)>0.01 || Math.AbsFloat(medical.BCMM_SystolicPressure()-expected*120)>2 || Math.AbsFloat(medical.BCMM_Pulse()-expectedPulse)>2 || medical.BCMM_OxygenSaturation()<0.97 || Math.AbsFloat(medical.BCMM_RespiratoryRate()-16)>0.5 || medical.BCMM_InCardiacArrest()) { return; }
  m_BCMMFluidObserved=phase;PrintFormat("BCMM_FLUID_OBSERVED %1 volume=%2 pulse=%3 pressure=%4",phase,medical.GetBloodHitZone().GetHealthScaled(),medical.BCMM_Pulse(),medical.BCMM_SystolicPressure());
 }
}


modded class BCMM_MedicalMenu
{
 void BCMM_TestFluidTab() { SelectTab(6); }
}
