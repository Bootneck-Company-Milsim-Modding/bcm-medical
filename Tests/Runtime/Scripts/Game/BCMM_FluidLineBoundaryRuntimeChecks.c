// Independent patient/source pairs, typed delivery, stale removal and real source movement.
class BCMM_FluidLineBoundaryRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_OtherDoctor;
 protected IEntity m_OtherPatient;
 protected IEntity m_OtherBag;
 protected InventoryStorageManagerComponent m_PatientInventory;
 protected InventoryStorageManagerComponent m_OtherInventory;
 protected SCR_CharacterDamageManagerComponent m_OtherMedical;
 protected ref BCMM_FluidLineSession m_NewLine;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.25 -2");
  m_Target.SetOrigin("101 2.25 -2");
  m_OtherDoctor=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","100 2.25 2");
  m_OtherPatient=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","101 2.25 2");
  array<IEntity> bodies=
  {
   m_Actor,m_Target,m_OtherDoctor,m_OtherPatient
  };
  foreach(IEntity body:bodies)
  {
   AIControlComponent ai=AIControlComponent.Cast(body.FindComponent(AIControlComponent));
   if(ai)
   {
    ai.DeactivateAI();
   }
  }
  m_Medical=PreparePatient(m_Target);
  m_OtherMedical=PreparePatient(m_OtherPatient);
  m_PatientInventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_OtherInventory=InventoryStorageManagerComponent.Cast(m_OtherPatient.FindComponent(InventoryStorageManagerComponent));
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY),"Boundary medic owns real native capacity for a later cross-character source move");
  m_Kit=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Target.GetOrigin());
  m_OtherBag=Spawn("{B6D83F6853C68D73}Prefabs/Items/BCMM_Plasma.et",m_OtherPatient.GetOrigin());
  m_Suite.Record(m_Kit && m_PatientInventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY) && m_OtherBag && m_OtherInventory.TryInsertItem(m_OtherBag,EStoragePurpose.PURPOSE_ANY),"Distinct native patients each own an original finite typed source");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  mode.BCMM_TestRoleSetting("fluidInfusionSeconds",30);
  mode.BCMM_TestRoleSetting("fluidLineRemovalSeconds",2);
  GetGame().GetCallqueue().CallLater(StartIndependentPairs,1500);
 }
 protected SCR_CharacterDamageManagerComponent PreparePatient(IEntity patient)
 {
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  medical.FullHeal();
  medical.GetBloodHitZone().SetHealth(medical.GetBloodHitZone().GetMaxHealth()*0.6);
  medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(patient.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",patient.GetOrigin());
  m_Suite.Record(pack && inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY),"Each native patient has actual capacity for its original finite typed bag");
  return medical;
 }
 protected void StartIndependentPairs()
 {
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM) && BCMM_FluidLineSession.Start(m_OtherPatient,m_OtherBag,BCMM_BodyRegion.LEFT_ARM),"Distinct real patient/source pairs can deliver simultaneously");
  m_Suite.Record(!BCMM_FluidLineSession.Start(m_OtherPatient,m_Kit,BCMM_BodyRegion.LEFT_ARM) && !BCMM_FluidLineSession.Start(m_Target,m_OtherBag,BCMM_BodyRegion.LEFT_ARM),"One native patient or physical source cannot be shared by a second line");
  GetGame().GetCallqueue().CallLater(CheckTypedLines,3000);
 }
 protected void CheckTypedLines()
 {
  BCMM_Patient saline=m_Medical.BCMM_GetPatient();
  BCMM_Patient plasma=m_OtherMedical.BCMM_GetPatient();
  BCMM_EquipmentItemComponent salineBag=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  BCMM_EquipmentItemComponent plasmaBag=BCMM_EquipmentItemComponent.Cast(m_OtherBag.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(salineBag.FluidRemainingMl()>400 && salineBag.FluidRemainingMl()<480 && plasmaBag.FluidRemainingMl()>400 && plasmaBag.FluidRemainingMl()<480,"Both original finite typed sources genuinely deplete during continuous native flow");
  m_Suite.Record(Math.AbsFloat(saline.Fluids.RedCellReserve-0.6)<0.0001 && Math.AbsFloat(plasma.Fluids.RedCellReserve-0.6)<0.0001 && Math.AbsFloat(saline.Fluids.ClottingReserve-0.6)<0.0001 && plasma.Fluids.ClottingReserve>0.605,"Native independent saline adds no cells/clotting while plasma adds clotting without red cells");
  m_Suite.Record(BCMM_FluidLineRemoval.BeginRemoval(m_Actor,m_Target,BCMM_BodyRegion.LEFT_ARM) && BCMM_FluidLineRemoval.BeginRemoval(m_OtherDoctor,m_OtherPatient,BCMM_BodyRegion.LEFT_ARM),"Two distinct actual actor/patient pairs may begin inventory-free timed removal together");
  GetGame().GetCallqueue().CallLater(CheckParallelRemoved,3000);
 }
 protected void CheckParallelRemoved()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && !m_OtherMedical.BCMM_FluidLineActive() && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !BCMM_SplintTreatment.IsBusy(m_OtherDoctor,m_OtherPatient) && m_PatientInventory.Contains(m_Kit) && m_OtherInventory.Contains(m_OtherBag),"Both independent native removals complete and preserve only their own original partial bags");
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Stale-removal case reconnects its real original partial source");
  BCMM_FluidLineSession old=BCMM_FluidLineSession.ForPatient(m_Target);
  m_Suite.Record(BCMM_FluidLineRemoval.BeginRemoval(m_Actor,m_Target,BCMM_BodyRegion.LEFT_ARM),"Stale line case starts actual timed removal");
  BCMM_FluidLineSession.StopForPatient(m_Target);
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Same original partial source reconnects during a previous pending removal");
  m_NewLine=BCMM_FluidLineSession.ForPatient(m_Target);
  m_Suite.Record(m_NewLine && m_NewLine!=old,"The replacement connection is a genuinely different native session");
  GetGame().GetCallqueue().CallLater(CheckStaleLine,3000);
 }
 protected void CheckStaleLine()
 {
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && BCMM_FluidLineSession.ForPatient(m_Target)==m_NewLine && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Old timed removal cannot stop a new line on the same source and patient");
  BaseInventoryStorageComponent storage=m_Inventory.FindStorageForItem(m_Kit,EStoragePurpose.PURPOSE_ANY);
  m_Suite.Record(storage && m_PatientInventory.TryMoveItemToStorage(m_Kit,storage),"Actual native inventory moves the original active source out of patient ownership");
  GetGame().GetCallqueue().CallLater(CheckMovedSourceStops,700);
 }
 protected void CheckMovedSourceStops()
 {
  m_Suite.Record(m_Inventory.Contains(m_Kit) && !m_PatientInventory.Contains(m_Kit) && !m_Medical.BCMM_FluidLineActive() && !BCMM_FluidLineSession.ForPatient(m_Target),"Real source movement stops the line and leaves the original partial bag in its actual new inventory");
  Finish();
 }
}
