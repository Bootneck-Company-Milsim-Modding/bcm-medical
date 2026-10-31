// Independent multiplayer medical presentation and unsafe owner-RPC acceptance.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMAppliedPhase;
 [RplProp()] protected RplId m_BCMMAppliedPatient=RplId.Invalid();
 void BCMM_PublishAppliedTest(int phase,IEntity patient)
 {
  m_BCMMAppliedPhase=phase;m_BCMMAppliedPatient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();PrintFormat("BCMM_APPLIED_PHASE %1",phase);
 }
 int BCMM_AppliedPhase() { return m_BCMMAppliedPhase; }
 IEntity BCMM_AppliedPatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMAppliedPatient); }
}
class BCMM_AppliedEquipmentNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Stage;
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity())
  {
   if(m_OwnerWait++<100) { GetGame().GetCallqueue().CallLater(Prepare,250);return; }
   m_Suite.Record(false,"Applied equipment fixture receives an owning client");Finish();return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=player.GetControlledEntity();
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Target.SetOrigin("101 2.2 0");m_Actor.SetOrigin("100 2.2 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Unsafe owner-RPC fixture equips actual inventory");
  m_Kit=Spawn("{6AB4B3F3FD216012}Prefabs/Items/BCMM_Morphine.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Owner holds exact finite morphine item");
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();patient.Equipment.PadsAttached=true;patient.Equipment.VascularAccess=true;patient.Equipment.VascularAccessRegion=BCMM_BodyRegion.RIGHT_ARM;
  patient.LeftChest.OpenWound=true;patient.RightChest.OpenWound=true;patient.LeftChest.ApplySeal();patient.RightChest.ApplySeal();
  patient.LeftLeg.Broken=true;patient.RightLeg.Broken=true;patient.LeftLeg.Splinted=true;patient.RightLeg.Splinted=true;
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.5);
  m_Medical.BCMM_SetPulseMonitor(true);BCMM_VisualStatusBridge.Publish(m_Medical);
  GetGame().GetCallqueue().CallLater(PublishReady,2000);
 }
 protected void PublishReady() { Publish(1); }
 protected void Publish(int phase)
 {
  m_Stage=phase;SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishAppliedTest(phase,m_Target);
  int delay=5000;if(phase==1) { delay=18000; }
  GetGame().GetCallqueue().CallLater(Next,delay);
 }
 protected void Next()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(m_Stage==1)
  {
   SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
   m_Suite.Record(owner && owner.BCMM_TestPressureDenial(),"Actual owner RPC is rejected specifically by the pressure safeguard");
   m_Suite.Record(BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),"Unsafe owner request is tested within real treatment reach");
   m_Suite.Record(m_Medical.BCMM_SystolicPressure()<90 && m_Kit && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_GetPatient().Medication.ActiveDoses()==0,"Unsafe actual owner medication RPC retains exact kit and cannot add a dose");
   mode.BCMM_TestRoleSetting("equipmentVisualChestDevicesEnabled",0);Publish(2);return;
  }
  if(m_Stage==2) { mode.BCMM_TestRoleSetting("equipmentVisualsEnabled",0);Publish(3);return; }
  if(m_Stage==3) { mode.BCMM_TestRoleSetting("equipmentVisualsEnabled",1);mode.BCMM_TestRoleSetting("equipmentVisualChestDevicesEnabled",1);Publish(4);return; }
  if(m_Stage==4) { m_Medical.FullHeal();m_Medical.BCMM_SetPulseMonitor(false);Publish(5);return; }
  m_Suite.Record(m_Medical.BCMM_AppliedEquipment()==0 && !m_Medical.BCMM_PulseMonitorActive(),"Authority FullHeal and monitor removal clear replicated applied flags");Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMAppliedObserved;
 protected bool m_BCMMAppliedPositioned;
 protected bool m_BCMMUnsafeMedicationSent;
 protected float m_BCMMAppliedLogAt;
 protected float m_BCMMAppliedRequestAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmAppliedObserver",observer)) { return; }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player || !player.GetControlledEntity()) { return; }
  IEntity target=mode.BCMM_AppliedPatient();if(!target || mode.BCMM_AppliedPhase()<=0) { return; }
  if(!m_BCMMAppliedPositioned)
  {
   if(player.GetPlayerId()==1) { vector pose[4];player.GetControlledEntity().GetWorldTransform(pose);pose[3]=target.GetOrigin()+"0 0.2 -1.2";ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(pose); }
   m_BCMMAppliedPositioned=true;m_BCMMAppliedRequestAt=world.GetWorldTime()+2000;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_CharacterPresentationComponent presentation=BCMM_CharacterPresentationComponent.Cast(target.FindComponent(BCMM_CharacterPresentationComponent));
  int phase=mode.BCMM_AppliedPhase();if(!medical || !presentation || phase<=m_BCMMAppliedObserved) { return; }
  if(phase==1 && player.GetPlayerId()==1 && !m_BCMMUnsafeMedicationSent && (world.GetWorldTime()<m_BCMMAppliedRequestAt || !BCMM_TreatmentAccess.CanReach(player.GetControlledEntity(),target))) { return; }
  if(phase==1 && player.GetPlayerId()==1 && !m_BCMMUnsafeMedicationSent && medical.BCMM_SystolicPressure()<90 && BCMM_TreatmentAccess.CanReach(player.GetControlledEntity(),target))
  {
   player.BCMM_RequestMedication(target,BCMM_MedicationKind.MORPHINE);m_BCMMUnsafeMedicationSent=true;Print("BCMM_UNSAFE_OWNER_MEDICATION_SENT");
  }
  int expected=8;if(phase==2) { expected=4; } if(phase==3 || phase==5) { expected=0; }
  if(world.GetWorldTime()>m_BCMMAppliedLogAt) { m_BCMMAppliedLogAt=world.GetWorldTime()+1000;PrintFormat("BCMM_APPLIED_NETWORK_STATE phase=%1 count=%2 flags=%3 pressure=%4",phase,presentation.AppliedVisualCount(),medical.BCMM_AppliedEquipment(),medical.BCMM_SystolicPressure()); }
  if(presentation.AppliedVisualCount()!=expected || !presentation.AppliedVisualsSafe()) { return; }
  if(phase!=5 && (medical.BCMM_AppliedEquipment()!=63 || medical.BCMM_IVRegion()!=BCMM_BodyRegion.RIGHT_ARM || !medical.BCMM_PulseMonitorActive())) { return; }
  if(phase==5 && (medical.BCMM_AppliedEquipment()!=0 || medical.BCMM_PulseMonitorActive())) { return; }
  m_BCMMAppliedObserved=phase;PrintFormat("BCMM_APPLIED_OBSERVED %1 count=%2",phase,expected);
 }
}
modded class SCR_PlayerController
{
 protected bool m_BCMMObservedPressureDenial;
 bool BCMM_TestPressureDenial() { return m_BCMMObservedPressureDenial; }
 override void BCMM_TreatmentFeedback(string message)
 {
  super.BCMM_TreatmentFeedback(message);string diagnostic;
  if(System.GetCLIParam("bcmmAppliedEquipmentNetwork",diagnostic) && message.Contains("Morphine withheld: systolic pressure too low"))
  {
   m_BCMMObservedPressureDenial=true;Print("BCMM_UNSAFE_OWNER_PRESSURE_REJECTED");
  }
 }
}
