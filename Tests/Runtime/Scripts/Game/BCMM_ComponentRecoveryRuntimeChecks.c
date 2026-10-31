// Fixture-only policy persistence through the real production settings store.
modded class SCR_BaseGameMode { bool BCMM_TestPersistComponentRecoveryPolicy(){return m_BCMMSettingsStore && m_BCMMSettingsStore.Save(m_BCMMSettings);} }
class BCMM_ComponentRecoveryManifest : JsonApiStruct
{
 string SaveId;string BodyId;string BagId;float Red;float Clot;float Volume;
 void BCMM_ComponentRecoveryManifest(){RegV("SaveId");RegV("BodyId");RegV("BagId");RegV("Red");RegV("Clot");RegV("Volume");}
}
class BCMM_ComponentRecoveryRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_BaseGameMode m_Mode;
 protected IEntity m_Station;
 protected float m_HeldRed;
 protected float m_HeldClot;
 protected ref BCMM_ComponentRecoveryManifest m_Save=new BCMM_ComponentRecoveryManifest();
 protected bool Setting(string key,float value){return m_Mode.BCMM_TestRoleSetting(key,value);}
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");m_Actor.SetOrigin("100 2.25 0");m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  m_Mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Medical.FullHeal();
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());m_Kit=Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_Actor.GetOrigin());
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Recovery fixture holds an actual unused finite blood bag in native inventory");
  m_Station=Spawn("{6AB4B3F3FD21B100}PrefabsEditable/Auto/Items/E_BCMM_FieldSurgeryStation.et","102 2.25 1");
  m_Suite.Record(m_Station && BCMM_MedicalFacilityComponent.Supports(m_Target,m_Target),"Actual stationary catalogue stretcher supplies native recovery facility support");
  m_Suite.Record(Setting("componentRecoveryEnabled",1) && Setting("redCellRecoverySeconds",60) && Setting("clottingRecoverySeconds",30) && Setting("componentRecoveryRequiresFacility",1) && Setting("componentRecoveryMinimumSystolicPressure",90),"Native authority applies the explicit enabled recovery test policy");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.9);BCMM_Patient patient=m_Medical.BCMM_GetPatient();patient.Fluids.LastVolume=0.9;patient.Fluids.RedCellReserve=0.65;patient.Fluids.ClottingReserve=0.6;patient.Fluids.TransientWater=0;
  GetGame().GetCallqueue().CallLater(CheckRecovery,8000);
 }
 protected void Hold(){BCMM_Patient patient=m_Medical.BCMM_GetPatient();m_HeldRed=patient.Fluids.RedCellReserve;m_HeldClot=patient.Fluids.ClottingReserve;}
 protected bool Held(){BCMM_Patient patient=m_Medical.BCMM_GetPatient();return Math.AbsFloat(patient.Fluids.RedCellReserve-m_HeldRed)<0.00001 && Math.AbsFloat(patient.Fluids.ClottingReserve-m_HeldClot)<0.00001;}
 protected void CheckRecovery()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();m_Suite.Record(patient.Fluids.RedCellReserve>0.65 && patient.Fluids.RedCellReserve<0.9 && patient.Fluids.ClottingReserve>0.6 && patient.Fluids.ClottingReserve<0.9,"Actual native character ticks gradually recover both supported components");
  CheckNoVolumeOrSupplyChange();SCR_EntityHelper.DeleteEntityAndChildren(m_Station);m_Station=null;Hold();GetGame().GetCallqueue().CallLater(CheckOutside,3000);
 }
 protected void CheckOutside()
 {
  m_Suite.Record(!BCMM_MedicalFacilityComponent.Supports(m_Target,m_Target) && Held(),"Removing the actual facility pauses native component recovery");
  m_Suite.Record(Setting("componentRecoveryRequiresFacility",0),"Explicit GM field-recovery policy applies");GetGame().GetCallqueue().CallLater(CheckField,3000);
 }
 protected void CheckField()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve>m_HeldRed && m_Medical.BCMM_GetPatient().Fluids.ClottingReserve>m_HeldClot,"Native recovery resumes under the explicit field policy");
  m_Suite.Record(Setting("componentRecoveryMinimumSystolicPressure",140),"Native pressure stop threshold can be configured");Hold();GetGame().GetCallqueue().CallLater(CheckPressure,3000);
 }
 protected void CheckPressure()
 {
  m_Suite.Record(m_Medical.BCMM_SystolicPressure()<140 && Held(),"Actual native pressure below the GM minimum stops both recovery paths");
  m_Suite.Record(Setting("componentRecoveryMinimumSystolicPressure",90) && Setting("componentRecoveryEnabled",0),"Native recovery can be disabled without changing other medical policy");Hold();GetGame().GetCallqueue().CallLater(CheckDisabled,3000);
 }
 protected void CheckDisabled()
 {
  m_Suite.Record(Held(),"Disabling the feature immediately holds native component reserves");CheckNoVolumeOrSupplyChange();SaveWorld();
 }
 protected void CheckNoVolumeOrSupplyChange()
 {
  BCMM_EquipmentItemComponent bag;if(m_Kit)bag=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(Math.AbsFloat(m_Medical.GetBloodHitZone().GetHealthScaled()-0.9)<0.0001 && !m_Medical.IsDestroyed(),"Native component recovery does not refill missing circulating volume or change life state");
  m_Suite.Record(bag && m_Inventory.Contains(m_Kit) && bag.FluidRemainingMl()==bag.FluidCapacityMl(),"Passive component recovery neither consumes nor duplicates a finite inventory supply");
 }
 protected void SaveWorld()
 {
  m_Suite.Record(m_Mode.BCMM_TestPersistComponentRecoveryPolicy(),"Actual settings store persists the explicit schema thirty-one recovery policy");
  PersistenceSystem persistence=PersistenceSystem.GetInstance();m_Suite.Record(persistence && persistence.IsTracked(m_Target) && persistence.IsTracked(m_Kit),"Recovered native patient and unused finite bag participate in real world persistence");if(!persistence){Finish();return;}
  m_Save.BodyId=persistence.GetId(m_Target);m_Save.BagId=persistence.GetId(m_Kit);m_Save.Red=m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;m_Save.Clot=m_Medical.BCMM_GetPatient().Fluids.ClottingReserve;m_Save.Volume=m_Medical.GetBloodHitZone().GetHealthScaled();
  SaveGameManager manager=GetGame().GetSaveGameManager();manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);manager.SetSavingAllowed(true);manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM component recovery",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnSaved));
 }
 protected void OnSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Native world save commits recovered component state");if(!success){Finish();return;}GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnSaves));
 }
 protected void OnSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;if(success && saves)foreach(SaveGame save:saves)if(save.GetSavePointName()=="BCMM component recovery" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))selected=save;
  m_Suite.Record(selected!=null,"Actual recovered-component world save is discoverable");if(selected){m_Save.SaveId=selected.GetId();m_Suite.Record(m_Save.PackToFile("$profile:BCMM_ComponentRecovery.json"),"Recovery restart manifest records exact native UUIDs and component reserves");}Finish();
 }
}
class BCMM_ComponentRecoveryRestartChecks : BCMM_SplintRuntimeChecks
{
 override protected void Prepare()
 {
  BCMM_ComponentRecoveryManifest saved=new BCMM_ComponentRecoveryManifest();PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(saved.LoadFromFile("$profile:BCMM_ComponentRecovery.json") && persistence && persistence.WasDataLoaded(),"Separate native process loads the actual recovered-patient world and manifest");
  if(!persistence || !persistence.WasDataLoaded()){Finish();return;}
  IEntity body=IEntity.Cast(persistence.FindById(saved.BodyId));IEntity bag=IEntity.Cast(persistence.FindById(saved.BagId));SCR_CharacterDamageManagerComponent medical;if(body)medical=SCR_CharacterDamageManagerComponent.Cast(body.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(medical && !medical.IsDestroyed() && Math.AbsFloat(medical.GetBloodHitZone().GetHealthScaled()-saved.Volume)<0.0001,"Recovered patient retains exact native body identity and unchanged missing blood volume");
  if(medical){BCMM_FluidBalance fluids=medical.BCMM_GetPatient().Fluids;m_Suite.Record(Math.AbsFloat(fluids.RedCellReserve-saved.Red)<0.0001 && Math.AbsFloat(fluids.ClottingReserve-saved.Clot)<0.0001,"Native restart retains exact recovered component reserves without offline catch-up");}
  BCMM_EquipmentItemComponent source;if(bag)source=BCMM_EquipmentItemComponent.Cast(bag.FindComponent(BCMM_EquipmentItemComponent));m_Suite.Record(source && source.FluidRemainingMl()==source.FluidCapacityMl(),"Exact unused finite bag survives native world restart without refill or duplication");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_Suite.Record(mode.BCMM_ReadSetting("componentRecoveryEnabled")==0 && mode.BCMM_ReadSetting("componentRecoveryRequiresFacility")==0 && mode.BCMM_ReadSetting("redCellRecoverySeconds")==60 && mode.BCMM_ReadSetting("clottingRecoverySeconds")==30,"Schema thirty-one native restart retains explicit recovery policy and time constants");Finish();
 }
}
