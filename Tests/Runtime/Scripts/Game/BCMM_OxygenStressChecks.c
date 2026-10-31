// Dedicated focused regression for native oxygen source switching and reach.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMOxygenStressPhase;
 [RplProp()] protected RplId m_BCMMOxygenStressTarget=RplId.Invalid();
 void BCMM_PublishOxygenStress(int phase,IEntity target){m_BCMMOxygenStressPhase=phase;m_BCMMOxygenStressTarget=BCMM_BodyBagComponent.Id(target);Replication.BumpMe();PrintFormat("BCMM_OXYGEN_STRESS_PHASE %1",phase);}
 int BCMM_OxygenStressPhase(){return m_BCMMOxygenStressPhase;}
 IEntity BCMM_OxygenStressTarget(){return BCMM_BodyBagComponent.Resolve(m_BCMMOxygenStressTarget);}
}
class BCMM_OxygenStressChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Trial;
 protected ref array<IEntity> m_Sources={};
 protected ref array<float> m_Remaining={};
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity())
  {
   if(m_OwnerWait++<100){GetGame().GetCallqueue().CallLater(Prepare,250);return;}
   m_Suite.Record(false,"Oxygen stress receives owning native player");Finish();return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=player.GetControlledEntity();
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  vector matrix[4];m_Target.GetWorldTransform(matrix);matrix[3]="101 2.35 0";ChimeraCharacter.Cast(m_Target).Teleport(matrix);
  AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY),"Stress owner equips actual native supply storage");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_TestRoleSetting("oxygenCapacityLitres",10);mode.BCMM_TestRoleSetting("oxygenFlowLitresPerMinute",20);mode.BCMM_TestRoleSetting("oxygenApplicationSeconds",3);
  SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(true);
  GetGame().GetCallqueue().CallLater(NextTrial,2000);
 }
 protected void NextTrial()
 {
  if(m_Trial>=10){SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishOxygenStress(11,m_Target);GetGame().GetCallqueue().CallLater(Finish,2000);return;}
  BCMM_OxygenSession.StopForPatient(m_Target);m_Medical.FullHeal();m_Medical.BCMM_GetPatient().LeftChest.CollapseFraction=0.8;m_Medical.BCMM_GetPatient().OxygenSaturation=0.588;
  m_Remaining.Clear();foreach(IEntity item:m_Sources)m_Remaining.Insert(BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres());
  m_Kit=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Next stress trial receives a fuller physical oxygen source");
  m_Trial++;SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishOxygenStress(m_Trial,m_Target);GetGame().GetCallqueue().CallLater(CheckTrial,6000);
 }
 protected void CheckTrial()
 {
  float gas=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres();
  PrintFormat("BCMM_OXYGEN_STRESS_RESULT trial=%1 active=%2 gas=%3 reach=%4",m_Trial,m_Medical.BCMM_GetPatient().Equipment.OxygenActive,gas,BCMM_TreatmentAccess.CanReach(m_Actor,m_Target));
  m_Suite.Record(m_Medical.BCMM_GetPatient().Equipment.OxygenActive && gas>0 && gas<10,"Repeated native owner connection activates and depletes the fuller physical oxygen source");
  for(int index=0;index<m_Sources.Count();index++)m_Suite.Record(Math.AbsFloat(BCMM_EquipmentItemComponent.Cast(m_Sources[index].FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres()-m_Remaining[index])<0.001,"Source switching leaves every previous disconnected cylinder unchanged");
  m_Sources.Insert(m_Kit);GetGame().GetCallqueue().CallLater(NextTrial,1000);
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMOxygenStressSent;
 protected bool m_BCMMOxygenStressPositioned;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;if(Replication.IsServer() || !System.GetCLIParam("bcmmOxygenStressObserver",observer))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());if(!mode || !player || player.GetPlayerId()!=1 || !player.GetControlledEntity())return;
  int phase=mode.BCMM_OxygenStressPhase();IEntity patient=mode.BCMM_OxygenStressTarget();if(!patient || phase<=m_BCMMOxygenStressSent || phase>10)return;
  IEntity actor=player.GetControlledEntity();RplComponent ownership=RplComponent.Cast(actor.FindComponent(RplComponent));if(!ownership || !ownership.IsOwner())return;
  if(!m_BCMMOxygenStressPositioned)
  {
   vector matrix[4];actor.GetWorldTransform(matrix);matrix[3]=patient.GetOrigin()+"0 0.2 -1.2";ChimeraCharacter.Cast(actor).Teleport(matrix);m_BCMMOxygenStressPositioned=true;return;
  }
  if(mode.BCMM_ReadSetting("oxygenFlowLitresPerMinute")!=20 || !BCMM_TreatmentAccess.CanReach(actor,patient))return;
  player.BCMM_RequestEquipment(patient,BCMM_EquipmentProcedure.CONNECT_OXYGEN,BCMM_BodyRegion.HEAD,0);
  m_BCMMOxygenStressSent=phase;PrintFormat("BCMM_OXYGEN_STRESS_OWNER_SENT %1",phase);
 }
}