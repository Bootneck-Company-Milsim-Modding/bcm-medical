class BCMM_FluidLineManifest : JsonApiStruct
{
 string SaveId;
 string PatientId;
 string SourceId;
 float Remaining;
 float Volume;
 void BCMM_FluidLineManifest()
 {
  RegV("SaveId");
  RegV("PatientId");
  RegV("SourceId");
  RegV("Remaining");
  RegV("Volume");
 }
}
class BCMM_FluidLinePersistenceRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_FluidLineManifest m_Save=new BCMM_FluidLineManifest();
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));
  if(ai)
  {
   ai.DeactivateAI();
  }
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
  m_Kit=Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_Target.GetOrigin());
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"World-save fixture has a real patient-owned original finite bag");
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Actual native continuous session starts before the world save");
  GetGame().GetCallqueue().CallLater(SaveFlowingWorld,5000);
 }
 protected void SaveFlowingWorld()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(persistence && persistence.IsTracked(m_Target) && persistence.IsTracked(m_Kit) && m_Medical.BCMM_FluidLineActive() && source.FluidRemainingMl()<500 && source.FluidRemainingMl()>0,"Native persistence tracks exact patient and partially depleted active source");
  if(!persistence)
  {
   Finish();
   return;
  }
  m_Save.PatientId=persistence.GetId(m_Target);
  m_Save.SourceId=persistence.GetId(m_Kit);
  m_Save.Remaining=source.FluidRemainingMl();
  m_Save.Volume=m_Medical.GetBloodHitZone().GetHealthScaled();
  SaveGameManager manager=GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM active fluid line",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnLineSaved));
 }
 protected void OnLineSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Native SaveGameManager commits the active line and original finite bag");
  if(!success)
  {
   Finish();
   return;
  }
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnLineSaves));
 }
 protected void OnLineSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;
  if(success && saves)
  {
   foreach(SaveGame save:saves)
   {
    if(save.GetSavePointName()=="BCMM active fluid line" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))
    {
     selected=save;
    }
   }
  }
  m_Suite.Record(selected!=null,"Actual active-line world save is discoverable");
  if(selected)
  {
   m_Save.SaveId=selected.GetId();
   m_Suite.Record(m_Save.PackToFile("$profile:BCMM_FluidLine.json"),"Line restart manifest retains original patient/source UUIDs and measured native volume");
  }
  Finish();
 }
}
class BCMM_FluidLineRestartRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_FluidLineManifest m_Save=new BCMM_FluidLineManifest();
 protected float m_HeldRemaining;
 protected float m_HeldVolume;
 override protected void Prepare()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_FluidLine.json") && persistence && persistence.WasDataLoaded(),"Separate native process loads the actual active-line world save");
  if(!persistence || !persistence.WasDataLoaded())
  {
   Finish();
   return;
  }
  m_Target=IEntity.Cast(persistence.FindById(m_Save.PatientId));
  m_Kit=IEntity.Cast(persistence.FindById(m_Save.SourceId));
  if(m_Target)
  {
   m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  if(m_Target)
  {
   m_Inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  }
  BCMM_EquipmentItemComponent source;
  if(m_Kit)
  {
   source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  }
  m_Suite.Record(m_Medical && source && m_Inventory && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_FluidLineActive(),"Original patient and exact original finite source resume only after native inventory binding");
  if(!m_Medical || !source)
  {
   Finish();
   return;
  }
  float used=m_Save.Remaining-source.FluidRemainingMl();
  float delivered=(m_Medical.GetBloodHitZone().GetHealthScaled()-m_Save.Volume)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  float rate=source.FluidCapacityMl()/BCMM_TreatmentAccess.Setting("fluidInfusionSeconds");
  float runtime=GetGame().GetWorld().GetWorldTime()*0.001;
  m_Suite.Record(used>=0 && used<=rate*runtime+3 && Math.AbsFloat(delivered-used)<3,"Restart neither refills the bag nor awards offline fluid; resumed native volume matches bounded current-process source depletion");
  m_HeldRemaining=source.FluidRemainingMl();
  m_HeldVolume=m_Medical.GetBloodHitZone().GetHealthScaled();
  GetGame().GetCallqueue().CallLater(CheckResumedLine,3000);
 }
 protected void CheckResumedLine()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  float used=m_HeldRemaining-source.FluidRemainingMl();
  float delivered=(m_Medical.GetBloodHitZone().GetHealthScaled()-m_HeldVolume)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && used>10 && used<35 && Math.AbsFloat(delivered-used)<0.1,"Independent restored session continues measured native flow using the same original partially used bag");
  m_Medical.FullHeal();
  m_HeldRemaining=source.FluidRemainingMl();
  GetGame().GetCallqueue().CallLater(CheckHealStops,700);
 }
 protected void CheckHealStops()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && !BCMM_FluidLineSession.ForPatient(m_Target) && source.FluidRemainingMl()==m_HeldRemaining,"Native FullHeal cancels restored line and leaves original finite contents unchanged");
  Finish();
 }
}
