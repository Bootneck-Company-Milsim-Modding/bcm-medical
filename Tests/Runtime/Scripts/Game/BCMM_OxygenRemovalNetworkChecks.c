modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMRemovalPhase;
 [RplProp()] protected RplId m_BCMMRemovalPatient=RplId.Invalid();
 void BCMM_PublishRemovalTest(int phase,IEntity patient){m_BCMMRemovalPhase=phase;m_BCMMRemovalPatient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();PrintFormat("BCMM_REMOVAL_PHASE %1",phase);}
 int BCMM_RemovalPhase(){return m_BCMMRemovalPhase;}
 IEntity BCMM_RemovalPatient(){return BCMM_BodyBagComponent.Resolve(m_BCMMRemovalPatient);}
}
class BCMM_OxygenRemovalNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Carrier;
 protected InventoryStorageManagerComponent m_CarrierInventory;
 protected int m_Wait;
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity()){if(m_OwnerWait++<100){GetGame().GetCallqueue().CallLater(Prepare,250);return;}m_Suite.Record(false,"Removal network fixture receives an actual owning client");Finish();return;}
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=player.GetControlledEntity();SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");m_Actor.SetOrigin("100 2.25 0");m_Target.SetOrigin("101 2.25 0");
  m_Carrier=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","102 2.25 0");array<IEntity> bodies={m_Target,m_Carrier};foreach(IEntity body:bodies){AIControlComponent ai=AIControlComponent.Cast(body.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();}
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_CarrierInventory=InventoryStorageManagerComponent.Cast(m_Carrier.FindComponent(InventoryStorageManagerComponent));IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Carrier.GetOrigin());m_Kit=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Carrier.GetOrigin());
  m_Suite.Record(pack && m_CarrierInventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_Kit && m_CarrierInventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Different actual character owns the finite oxygen source");m_Suite.Record(!BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN),"Actual connected removing owner has no cylinder");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("oxygenRemovalSeconds",12);BCMM_OxygenSession.Start(m_Carrier,m_Target,m_Kit);GetGame().GetCallqueue().CallLater(PublishConnection,1000);
 }
 protected void PublishConnection(){SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishRemovalTest(1,m_Target);WaitForOwner();}
 protected void WaitForOwner()
 {
  if(BCMM_SplintTreatment.IsBusy(m_Actor,m_Target)){SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishRemovalTest(2,m_Target);GetGame().GetCallqueue().CallLater(CheckRemovalMidpoint,4000);return;}
  if(m_Wait++<100){GetGame().GetCallqueue().CallLater(WaitForOwner,250);return;}m_Suite.Record(false,"Actual owner menu/RPC starts inventory-free removal");Finish();
 }
 protected void CheckRemovalMidpoint()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().Equipment.OxygenActive && BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && !BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN),"Dedicated authority enforces saved removal duration for actual cylinder-free owner without fictitious item playback");GetGame().GetCallqueue().CallLater(CheckRemovalFinished,10000);
 }
 protected void CheckRemovalFinished()
 {
  m_Suite.Record(!m_Medical.BCMM_GetPatient().Equipment.OxygenActive && !(m_Medical.BCMM_AppliedEquipment()&64) && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && m_CarrierInventory.Contains(m_Kit),"Actual owning-client removal stops original connection and retains original carrier device");SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishRemovalTest(3,m_Target);GetGame().GetCallqueue().CallLater(Finish,8000);
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMRemovalOpened;
 protected bool m_BCMMRemovalSent;
 protected int m_BCMMRemovalObserved;
 protected float m_BCMMRemovalReadyAt;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;if(Replication.IsServer() || !System.GetCLIParam("bcmmRemovalObserver",observer))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());if(!mode || !player || !player.GetControlledEntity() || !mode.BCMM_HasSettingsSnapshot())return;
  RplComponent ownership=RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent));if(player.GetPlayerId()==1 && (!ownership || !ownership.IsOwner()))return;
  IEntity target=mode.BCMM_RemovalPatient();
  int phase=mode.BCMM_RemovalPhase();
  if(!target || phase < 1){return;}
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical){return;}
  if(player.GetPlayerId()==1 && phase==1 && !m_BCMMRemovalOpened)
  {
   vector pose[4];player.GetControlledEntity().GetWorldTransform(pose);pose[3]=target.GetOrigin()+"-1 0.1 0";ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(pose);
   BCMM_MedicalMenu.Open(target);m_BCMMRemovalOpened=true;m_BCMMRemovalReadyAt=world.GetWorldTime()+2000;
  }
  if(player.GetPlayerId()==1 && phase==1 && m_BCMMRemovalOpened && !m_BCMMRemovalSent && world.GetWorldTime()>m_BCMMRemovalReadyAt)
  {
   BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();if(!menu || !menu.BCMM_TestRemovalAvailable())return;
   Print("BCMM_REMOVAL_OWNER_ENABLED zeroCylinders=1");menu.UseEquipment(BCMM_EquipmentProcedure.DISCONNECT_OXYGEN);menu.Close();m_BCMMRemovalSent=true;Print("BCMM_REMOVAL_OWNER_SENT");
  }
  BCMM_CharacterPresentationComponent presentation=BCMM_CharacterPresentationComponent.Cast(target.FindComponent(BCMM_CharacterPresentationComponent));if(!presentation || !presentation.AppliedVisualsSafe())return;
  bool active=(medical.BCMM_AppliedEquipment()&64)!=0;bool mask=presentation.AppliedVisualSlot(12);
  if(phase!=m_BCMMRemovalObserved && ((phase<=2 && active && mask) || (phase==3 && !active && !mask)))
  {
   m_BCMMRemovalObserved=phase;PrintFormat("BCMM_REMOVAL_OBSERVED %1 player=%2",phase,player.GetPlayerId());
  }
 }
}
