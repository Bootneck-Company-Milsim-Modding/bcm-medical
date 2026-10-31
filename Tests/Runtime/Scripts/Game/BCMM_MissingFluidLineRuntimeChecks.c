// Native missing-reference callbacks and actual pending-binding world save.
class BCMM_MissingFluidLineManifest : JsonApiStruct
{
 string SaveId;
 string PatientId;
 string SourceId="d4d4d4d4-d4d4-4444-8444-d4d4d4d4d4d4";
 float Volume;
 void BCMM_MissingFluidLineManifest()
 {
  RegV("SaveId");
  RegV("PatientId");
  RegV("SourceId");
  RegV("Volume");
 }
}
class BCMM_MissingFluidLineRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_MissingFluidLineManifest m_Save=new BCMM_MissingFluidLineManifest();
 protected IEntity m_CallbackPatient;
 protected IEntity m_CallbackBag;
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
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(persistence && persistence.IsTracked(m_Target) && !persistence.FindById(m_Save.SourceId),"Missing-line fixture uses a real tracked patient and genuinely unresolved canonical source identity");
  if(!persistence)
  {
   Finish();
   return;
  }
  BCMM_SavedFluidLine state=new BCMM_SavedFluidLine();
  state.SourceId=m_Save.SourceId;
  state.AccessRegion=BCMM_BodyRegion.LEFT_ARM;
  BCMM_SavedFluidLineBinding.Await(m_Target,m_Medical.BCMM_GetPatient(),state,persistence);
  m_CallbackPatient=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","103 2.25 2");
  AIControlComponent callbackAI=AIControlComponent.Cast(m_CallbackPatient.FindComponent(AIControlComponent));
  if(callbackAI)
  {
   callbackAI.DeactivateAI();
  }
  SCR_CharacterDamageManagerComponent callbackMedical=SCR_CharacterDamageManagerComponent.Cast(m_CallbackPatient.FindComponent(SCR_CharacterDamageManagerComponent));
  callbackMedical.FullHeal();
  callbackMedical.GetBloodHitZone().SetHealth(callbackMedical.GetBloodHitZone().GetMaxHealth()*0.7);
  callbackMedical.BCMM_GetPatient().Equipment.VascularAccess=true;
  InventoryStorageManagerComponent callbackInventory=InventoryStorageManagerComponent.Cast(m_CallbackPatient.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_CallbackPatient.GetOrigin());
  m_CallbackBag=Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_CallbackPatient.GetOrigin());
  m_Suite.Record(pack && callbackInventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_CallbackBag && callbackInventory.TryInsertItem(m_CallbackBag,EStoragePurpose.PURPOSE_ANY),"Stale callback fixture owns a genuinely available original finite bag");
  GetGame().GetCallqueue().CallLater(BeginAvailableCallbackCase,1500);
  GetGame().GetCallqueue().CallLater(SavePendingLine,1000);
 }
 protected void BeginAvailableCallbackCase()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  SCR_CharacterDamageManagerComponent callbackMedical=SCR_CharacterDamageManagerComponent.Cast(m_CallbackPatient.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(persistence.IsTracked(m_CallbackBag),"Known-source callback uses the actual registered original finite item");
  BCMM_SavedFluidLine available=new BCMM_SavedFluidLine();
  available.SourceId=persistence.GetId(m_CallbackBag);
  available.AccessRegion=BCMM_BodyRegion.LEFT_ARM;
  BCMM_SavedFluidLineBinding.Await(m_CallbackPatient,callbackMedical.BCMM_GetPatient(),available,persistence);
  callbackMedical.FullHeal();
  callbackMedical.GetBloodHitZone().SetHealth(callbackMedical.GetBloodHitZone().GetMaxHealth()*0.7);
  callbackMedical.BCMM_GetPatient().Equipment.VascularAccess=true;
  GetGame().GetCallqueue().CallLater(CheckOldCallbackCancelled,300);
 }
 protected void CheckOldCallbackCancelled()
 {
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_CallbackPatient.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_CallbackBag.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(!BCMM_SavedFluidLineBinding.ForPatient(m_CallbackPatient) && !medical.BCMM_FluidLineActive() && !BCMM_FluidLineSession.ForPatient(m_CallbackPatient) && source.FluidRemainingMl()==500,"A real already-available source callback cannot attach or deliver to a new patient model after FullHeal");
 }
 protected void SavePendingLine()
 {
  BCMM_SavedFluidLine pending=BCMM_SavedFluidLineBinding.ForPatient(m_Target);
  m_Suite.Record(pending && pending.SourceId==m_Save.SourceId && !m_Medical.BCMM_FluidLineActive(),"Actual native unavailable-source wait retains its pending UUID without supplying invisible fluid");
  m_Save.PatientId=PersistenceSystem.GetInstance().GetId(m_Target);
  m_Save.Volume=m_Medical.GetBloodHitZone().GetHealthScaled();
  SaveGameManager manager=GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM unresolved fluid source",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnMissingLineSaved));
 }
 protected void OnMissingLineSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Native world save commits the patient while an original-source reference is still unresolved");
  if(!success)
  {
   Finish();
   return;
  }
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnMissingLineSaves));
 }
 protected void OnMissingLineSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;
  if(success && saves)
  {
   foreach(SaveGame save:saves)
   {
    if(save.GetSavePointName()=="BCMM unresolved fluid source" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))
    {
     selected=save;
    }
   }
  }
  m_Suite.Record(selected!=null,"Actual pending-line native save point is discoverable");
  if(selected)
  {
   m_Save.SaveId=selected.GetId();
   m_Suite.Record(m_Save.PackToFile("$profile:BCMM_MissingFluidLine.json"),"Missing-line restart manifest preserves the real patient and unresolved source UUIDs");
  }
  GetGame().GetCallqueue().CallLater(CheckMissingLineExpired,32000);
 }
 protected void CheckMissingLineExpired()
 {
  m_Suite.Record(!BCMM_SavedFluidLineBinding.ForPatient(m_Target) && !BCMM_FluidLineSession.ForPatient(m_Target) && !m_Medical.BCMM_FluidLineActive() && !PersistenceSystem.GetInstance().FindById(m_Save.SourceId),"Real native source timeout expires its binding without creating a physical or invisible replacement");
  m_Suite.Record(Math.AbsFloat(m_Medical.GetBloodHitZone().GetHealthScaled()-m_Save.Volume)<0.0001,"Unresolved source and timeout award no native blood volume");
  Finish();
 }
}
class BCMM_MissingFluidLineRestartChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_MissingFluidLineManifest m_Save=new BCMM_MissingFluidLineManifest();
 override protected void Prepare()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_MissingFluidLine.json") && persistence && persistence.WasDataLoaded(),"Separate native process loads the actual world saved during missing-source resolution");
  if(!persistence || !persistence.WasDataLoaded())
  {
   Finish();
   return;
  }
  m_Target=IEntity.Cast(persistence.FindById(m_Save.PatientId));
  if(m_Target)
  {
   m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  BCMM_SavedFluidLine pending=BCMM_SavedFluidLineBinding.ForPatient(m_Target);
  m_Suite.Record(m_Medical && pending && pending.SourceId==m_Save.SourceId && pending.AccessRegion==BCMM_BodyRegion.LEFT_ARM && !m_Medical.BCMM_FluidLineActive(),"Actual saved pending UUID survives native restart and never silently resumes with a new bag");
  if(!m_Medical)
  {
   Finish();
   return;
  }
  GetGame().GetCallqueue().CallLater(CheckReloadedTimeout,32000);
 }
 protected void CheckReloadedTimeout()
 {
  m_Suite.Record(!BCMM_SavedFluidLineBinding.ForPatient(m_Target) && !BCMM_FluidLineSession.ForPatient(m_Target) && !m_Medical.BCMM_FluidLineActive() && !PersistenceSystem.GetInstance().FindById(m_Save.SourceId),"Restored unresolved-source wait genuinely times out without source recreation");
  m_Suite.Record(Math.AbsFloat(m_Medical.GetBloodHitZone().GetHealthScaled()-m_Save.Volume)<0.0001,"Restarted missing-source binding delivers no volume or offline catch-up");
  Finish();
 }
}
