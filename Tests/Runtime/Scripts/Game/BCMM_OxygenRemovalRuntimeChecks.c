modded class BCMM_MedicalMenu
{
 bool BCMM_TestRemovalAvailable()
 {
  SelectRegion(BCMM_BodyRegion.HEAD);SelectTab(1);
  Widget row=GetRootWidget().FindAnyWidget("Action4");
  return m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+BCMM_EquipmentKind.OXYGEN]==0 && row && row.IsEnabled() && TextWidget.Cast(row.FindAnyWidget("Text")).GetText()=="Disconnect oxygen supply";
 }
}
class BCMM_OxygenRemovalRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Carrier;protected IEntity m_OtherDoctor;protected IEntity m_OtherPatient;
 protected IEntity m_FirstDevice;protected IEntity m_SecondDevice;
 protected InventoryStorageManagerComponent m_CarrierInventory;
 protected ref BCMM_OxygenSession m_Reconnected;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");m_Actor.SetOrigin("100 2.25 -2");m_Target.SetOrigin("101 2.25 -2");
  m_Carrier=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","102 2.25 0");m_OtherDoctor=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","100 2.25 2");m_OtherPatient=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","101 2.25 2");
  array<IEntity> bodies={m_Actor,m_Target,m_Carrier,m_OtherDoctor,m_OtherPatient};foreach(IEntity body:bodies){AIControlComponent ai=AIControlComponent.Cast(body.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();SCR_CharacterDamageManagerComponent.Cast(body.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(false);}
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_CarrierInventory=InventoryStorageManagerComponent.Cast(m_Carrier.FindComponent(InventoryStorageManagerComponent));IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Carrier.GetOrigin());
  m_FirstDevice=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Carrier.GetOrigin());m_SecondDevice=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Carrier.GetOrigin());
  m_Suite.Record(pack && m_CarrierInventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_CarrierInventory.TryInsertItem(m_FirstDevice,EStoragePurpose.PURPOSE_ANY) && m_CarrierInventory.TryInsertItem(m_SecondDevice,EStoragePurpose.PURPOSE_ANY),"A separate actual character carries both finite oxygen devices");
  m_Suite.Record(BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN)==null && BCMM_EquipmentTreatment.FindEquipment(m_OtherDoctor,BCMM_EquipmentKind.OXYGEN)==null,"Both removing medics genuinely have no oxygen cylinder");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("oxygenRemovalSeconds",4);
  BCMM_OxygenSession.Start(m_Carrier,m_Target,m_FirstDevice);GetGame().GetCallqueue().CallLater(OpenPanel,1000);
 }
 protected void OpenPanel(){BCMM_MedicalMenu.Open(m_Target);GetGame().GetCallqueue().CallLater(CheckPanel,300);}
 protected void CheckPanel()
 {
  BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();m_Suite.Record(menu && menu.BCMM_TestRemovalAvailable(),"Actual native medical UI enables connected oxygen removal with a zero-cylinder supply count");
  if(!menu){Finish();return;}menu.UseEquipment(BCMM_EquipmentProcedure.DISCONNECT_OXYGEN);menu.Close();GetGame().GetCallqueue().CallLater(CheckMidpoint,2000);
 }
 protected void CheckMidpoint()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().Equipment.OxygenActive && BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Inventory-free removal respects its four-second authority timer without equipping a fictional supply");GetGame().GetCallqueue().CallLater(CheckRemoved,3000);
 }
 protected void CheckRemoved()
 {
  m_Suite.Record(!m_Medical.BCMM_GetPatient().Equipment.OxygenActive && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && m_CarrierInventory.Contains(m_FirstDevice),"Native menu/RPC removal stops oxygen while retaining the original carrier's physical device");
  BCMM_OxygenSession.Start(m_Carrier,m_Target,m_FirstDevice);m_Suite.Record(Begin(m_Actor,m_Target),"A cylinder-free medic can start another timed removal");m_Target.SetOrigin("104 2.25 -2");GetGame().GetCallqueue().CallLater(CheckRemovalCancelled,700);
 }
 protected bool Begin(IEntity actor,IEntity target){return BCMM_EquipmentTreatment.BeginEquipment(actor,target,BCMM_EquipmentProcedure.DISCONNECT_OXYGEN,BCMM_BodyRegion.HEAD);}
 protected void CheckRemovalCancelled()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().Equipment.OxygenActive && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Real casualty range loss cancels removal without disconnecting an otherwise valid oxygen supply");m_Target.SetOrigin("101 2.25 -2");GetGame().GetCallqueue().CallLater(BeginStale,700);
 }
 protected void BeginStale()
 {
  m_Suite.Record(Begin(m_Actor,m_Target),"Stale-connection fixture starts a real timed removal");BCMM_OxygenSession old=BCMM_OxygenSession.ConnectionForPatient(m_Target);BCMM_OxygenSession.StopForPatient(m_Target);BCMM_OxygenSession.Start(m_Carrier,m_Target,m_FirstDevice);m_Reconnected=BCMM_OxygenSession.ConnectionForPatient(m_Target);
  m_Suite.Record(m_Reconnected && m_Reconnected!=old,"The same finite device genuinely reconnects as a new session");GetGame().GetCallqueue().CallLater(CheckStale,5000);
 }
 protected void CheckStale()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().Equipment.OxygenActive && BCMM_OxygenSession.ConnectionForPatient(m_Target)==m_Reconnected && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Old removal completion cannot stop a new connection using the same source and patient model");
  BCMM_OxygenSession.Start(m_Carrier,m_OtherPatient,m_SecondDevice);m_Suite.Record(Begin(m_Actor,m_Target) && Begin(m_OtherDoctor,m_OtherPatient),"Two distinct actor/patient pairs may concurrently remove oxygen without sharing a null supply lock");GetGame().GetCallqueue().CallLater(CheckConcurrent,5000);
 }
 protected void CheckConcurrent()
 {
  SCR_CharacterDamageManagerComponent other=SCR_CharacterDamageManagerComponent.Cast(m_OtherPatient.FindComponent(SCR_CharacterDamageManagerComponent));m_Suite.Record(!m_Medical.BCMM_GetPatient().Equipment.OxygenActive && !other.BCMM_GetPatient().Equipment.OxygenActive && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !BCMM_SplintTreatment.IsBusy(m_OtherDoctor,m_OtherPatient),"Both independent native timed removals complete and release only their own reservations");
  m_Suite.Record(m_CarrierInventory.Contains(m_FirstDevice) && m_CarrierInventory.Contains(m_SecondDevice) && BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN)==null,"Removals preserve both original devices without creating a replacement supply");Finish();
 }
}
