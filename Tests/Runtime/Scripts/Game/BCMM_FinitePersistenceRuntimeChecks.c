class BCMM_FinitePersistenceManifest : JsonApiStruct
{
 int Schema=1;
 string SaveId;
 ref array<string> ClinicalIds={};
 ref array<string> WorldIds={};
 float ClinicalWear;
 string ClinicalOxygenId;
 float ClinicalOxygenGas;
 string GraveId;
 string StationId;
 ref array<string> CargoIds={};
 int CargoRemainsToken;
 ref array<string> Ids={};
 ref array<float> Kinds={};
 ref array<float> Capacities={};
 ref array<float> Remaining={};
 void BCMM_FinitePersistenceManifest(){RegV("CargoRemainsToken");RegV("CargoIds");RegV("GraveId");RegV("StationId");RegV("WorldIds");RegV("ClinicalOxygenId");RegV("ClinicalOxygenGas");RegV("ClinicalIds");RegV("ClinicalWear");RegV("Schema");RegV("SaveId");RegV("Ids");RegV("Kinds");RegV("Capacities");RegV("Remaining");}
}
// Actual native PersistenceSystem config/serializer/respawn round trip.
class BCMM_FinitePersistenceRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected PersistenceSystem m_Persistence;
 protected string m_FinitePhase;
 protected ref BCMM_FinitePersistenceManifest m_FiniteManifest=new BCMM_FinitePersistenceManifest();
 protected int m_FiniteIndex;
 protected string m_FiniteJson;
 protected IEntity m_FiniteItem;
 protected UUID m_FiniteId;
 protected string m_FiniteCapacityKey;
 protected float m_FiniteCapacityDefault;
 protected ref array<float> m_FiniteBefore;
 override protected void Prepare()
 {
  m_Persistence=PersistenceSystem.GetInstance();
  if(!m_Persistence){m_Suite.Record(false,"Native persistence system exists");Finish();return;}
  PrintFormat("BCMM_FINITE_PERSISTENCE state=%1",m_Persistence.GetState());
  m_Suite.Record(m_Persistence.GetState()==EPersistenceSystemState.ACTIVE,"Native persistence system is active in the actual diagnostic world");
  System.GetCLIParam("bcmmFinitePersistencePhase",m_FinitePhase);
  if(m_FinitePhase=="verify-loss"){VerifyDisposedWorld();return;}
  if(m_FinitePhase=="verify"){VerifySavedWorld();return;}
  if(m_FinitePhase=="seed")PrepareLifeAccounts();
  SaveNext();
 }
 protected void SaveNext()
 {
  if(m_FiniteIndex>=3){if(m_FinitePhase=="seed")SaveNativeWorld();else Finish();return;}
  array<ResourceName> prefabs={"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et","{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et","{6AB4B3F3FD216025}Prefabs/Items/BCMM_AED.et"};
  IEntity item=Spawn(prefabs[m_FiniteIndex],"100 2 2");BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
  if(m_FiniteIndex==0)source.ConsumeFluidMl(125);if(m_FiniteIndex==1)source.ConsumeOxygen(1);if(m_FiniteIndex==2)source.Discharge();
  m_FiniteBefore=source.BCMM_CaptureFiniteSupply();
  m_Suite.Record(m_FiniteBefore.Count()==4 && m_FiniteBefore[3]<m_FiniteBefore[2],"Native source is partially depleted before persistence");
  array<float> invalid={2,m_FiniteBefore[1],m_FiniteBefore[2],m_FiniteBefore[3]};
  m_Suite.Record(!source.BCMM_RestoreFiniteSupply(invalid),"Unsupported finite-state versions are rejected before mutation");
  invalid={1,m_FiniteBefore[1]+1,m_FiniteBefore[2],m_FiniteBefore[3]};
  m_Suite.Record(!source.BCMM_RestoreFiniteSupply(invalid),"Saved contents cannot cross finite-supply kinds");
  invalid={1,m_FiniteBefore[1],m_FiniteBefore[2],m_FiniteBefore[2]+1};
  m_Suite.Record(!source.BCMM_RestoreFiniteSupply(invalid),"Over-capacity saved contents are rejected atomically");
  invalid={1,m_FiniteBefore[1],m_FiniteBefore[2],-1};
  m_Suite.Record(!source.BCMM_RestoreFiniteSupply(invalid),"Negative saved contents are rejected atomically");
  if(m_FiniteIndex==2)
  {
   invalid={1,m_FiniteBefore[1],m_FiniteBefore[2],0.5};
   m_Suite.Record(!source.BCMM_RestoreFiniteSupply(invalid),"AED persistence rejects fractional charges");
  }
  array<float> unchanged=source.BCMM_CaptureFiniteSupply();
  m_Suite.Record(unchanged[2]==m_FiniteBefore[2] && unchanged[3]==m_FiniteBefore[3],"Rejected persistence data never changes real device contents or capacity");
  bool tracked=m_Persistence.IsTracked(item);if(!tracked)tracked=m_Persistence.StartTracking(item,false);
  m_Suite.Record(tracked && m_Persistence.GetConfig(item),"Native persistence resolves the item configuration and collection");
  SCR_PersistenceJsonSaveContext save=new SCR_PersistenceJsonSaveContext();
  ESerializeResult result=m_Persistence.Serialize(item,save);m_FiniteJson=save.SaveToString();
  PrintFormat("BCMM_FINITE_SAVE kind=%1 result=%2 json=%3",source.Kind,result,m_FiniteJson);
  m_Suite.Record(result!=ESerializeResult.ERROR && m_FiniteJson.Contains("bcmmFiniteSupply"),"Native item serializer includes finite-supply state");
  m_Suite.Record(m_Persistence.Save(item),"Native persistence stores the actual finite item record");
  m_FiniteId=m_Persistence.GetId(item);
  m_FiniteCapacityKey="fluidBagVolumeMl";if(m_FiniteIndex==1)m_FiniteCapacityKey="oxygenCapacityLitres";if(m_FiniteIndex==2)m_FiniteCapacityKey="aedShocksPerDevice";
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_FiniteCapacityDefault=mode.BCMM_ReadSetting(m_FiniteCapacityKey);
  m_Suite.Record(mode.BCMM_TestRoleSetting(m_FiniteCapacityKey,m_FiniteCapacityDefault*2),"GM new-device capacity changes before loading the saved original item");
  m_Persistence.StopTracking(item,false);SCR_EntityHelper.DeleteEntityAndChildren(item);
  GetGame().GetCallqueue().CallLater(RestoreNext,500);
 }
 protected void RestoreNext()
 {
  PersistenceSpawnRequest request=new PersistenceSpawnRequest();
  request.Collection=m_Persistence.FindCollection("Item");request.Include={m_FiniteId};
  m_Persistence.RequestSpawn(request,new PersistenceResultCallback(OnFiniteLoaded));
 }
 protected void OnFiniteLoaded(EPersistenceStatusCode statusCode,Managed result,bool isLast,Managed context=null)
 {
  IEntity restored=IEntity.Cast(result);BCMM_EquipmentItemComponent contents;
  if(restored)contents=BCMM_EquipmentItemComponent.Cast(restored.FindComponent(BCMM_EquipmentItemComponent));
  array<float> after={};if(contents)after=contents.BCMM_CaptureFiniteSupply();
  bool same=after.Count()==m_FiniteBefore.Count();if(same){for(int i=0;i<after.Count();i++){if(Math.AbsFloat(after[i]-m_FiniteBefore[i])>0.001)same=false;}}
  PrintFormat("BCMM_FINITE_LOADED status=%1 item=%2 same=%3",statusCode,restored,same);
  m_Suite.Record(statusCode==EPersistenceStatusCode.OK && restored && contents && same,"Native saved-record load restores exact finite contents and original capacity");
  m_Suite.Record(restored && m_Persistence.GetId(restored)==m_FiniteId,"Native saved-record spawn preserves the persistent UUID of the original supply");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting(m_FiniteCapacityKey,m_FiniteCapacityDefault);
  if(m_FinitePhase=="seed" && restored && same)
  {
   m_FiniteManifest.Ids.Insert(m_FiniteId);m_FiniteManifest.Kinds.Insert(after[1]);m_FiniteManifest.Capacities.Insert(after[2]);m_FiniteManifest.Remaining.Insert(after[3]);
  }
  else if(restored){m_Persistence.StopTracking(restored,false);SCR_EntityHelper.DeleteEntityAndChildren(restored);}
  m_FiniteItem=null;m_FiniteIndex++;GetGame().GetCallqueue().CallLater(SaveNext,500);
 }
 protected void SaveNativeWorld()
 {
  CompleteLifeSeed();
  CheckClinicalCodecs();
  SeedClinicalPatients();
  SeedPhysicalBag();
  SeedCargoRecovery();
  GetGame().GetCallqueue().CallLater(SaveStableNativeWorld,500);
 }
 protected void SaveStableNativeWorld()
 {
  BCMM_BodyBagComponent cargoState=BCMM_BodyBagComponent.Cast(m_SavedCargoBag.FindComponent(BCMM_BodyBagComponent));
  if((!cargoState.Ready() || cargoState.State()!=BCMM_BagCargoState.STORED) && m_SavedCargoWait++<40){GetGame().GetCallqueue().CallLater(SaveStableNativeWorld,250);return;}
  InventoryStorageManagerComponent cargoInventory=InventoryStorageManagerComponent.Cast(m_SavedCargoVehicle.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(cargoState.Ready() && cargoState.State()==BCMM_BagCargoState.STORED && cargoInventory.Contains(m_SavedCargoSupply),"Native vehicle cargo deposit completes before the actual saved-world snapshot");
  m_Suite.Record(m_FiniteManifest.Ids.Count()==3,"Native world save retains all three restored finite supplies");
  SaveGameManager manager=GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM finite-supply restart",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnWorldSaved));
 }
 protected void OnWorldSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Native SaveGameManager commits the complete world save point");
  if(!success){Finish();return;}
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnSavesFound));
 }
 protected void OnSavesFound(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;
  if(success && saves)
  {
   foreach(SaveGame save:saves)
   {
    if(save.GetSavePointName()!="BCMM finite-supply restart")continue;
    if(!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix())selected=save;
   }
  }
  m_Suite.Record(selected!=null,"Native world save point is discoverable from committed storage");
  if(selected)
  {
   m_FiniteManifest.SaveId=selected.GetId();
   m_Suite.Record(m_FiniteManifest.PackToFile("$profile:BCMM_FinitePersistence.json"),"Restart manifest records native save UUID and original finite supply UUIDs");
   PrintFormat("BCMM_FINITE_SAVE_POINT %1",m_FiniteManifest.SaveId);
  }
  Finish();
 }
 protected void VerifyDisposedWorld()
 {
  m_Suite.Record(m_FiniteManifest.LoadFromFile("$profile:BCMM_FinitePersistence.json") && m_FiniteManifest.CargoIds.Count()==3,"Third native process loads the post-disposition world manifest");
  SaveGame active=GetGame().GetSaveGameManager().GetActiveSave();m_Suite.Record(m_Persistence.WasDataLoaded() && active && active.GetId()==m_FiniteManifest.SaveId,"Third process actually loads the new post-wreck save point");
  foreach(string id:m_FiniteManifest.CargoIds)m_Suite.Record(!m_Persistence.FindById(id),"New saved-world restart never resurrects the disposed vehicle, its physical bag or its original equipment");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(m_FiniteManifest.CargoRemainsToken);
  m_Suite.Record(record && record.Disposition==BCMM_RemainsDisposition.VEHICLE_LOST && !record.Physical && !record.StoredVehicle,"Vehicle-loss tombstone survives the next real world save and restart without stale physical references");
  IEntity independent=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.WorldIds[0]));BCMM_BodyBagComponent state;if(independent)state=BCMM_BodyBagComponent.Cast(independent.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(state && state.DeceasedName()=="BCM saved recovery casualty" && state.Ready(),"Unrelated original world remains survives the post-wreck save boundary");
  IEntity grave=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.GraveId));m_Suite.Record(grave && !m_Persistence.FindById(m_FiniteManifest.StationId),"Native deleted-stretcher state and independent grave survive the next saved-world restart");
  BindLifeTestIdentities();BCMM_LifeAccount account=mode.BCMM_AccountFor(90001);
  m_Suite.Record(account && account.Ledger.IsAlive() && account.ActiveEntity && account.Ledger.RemainingDeployments(mode.BCMM_ReadSetting("totalLives"))==mode.BCMM_ReadSetting("totalLives")-1,"Original surviving paid life and remaining balance persist independently of destroyed physical recovery cargo");
  Finish();
 }
 protected void VerifySavedWorld()
 {
  m_Suite.Record(m_FiniteManifest.LoadFromFile("$profile:BCMM_FinitePersistence.json") && m_FiniteManifest.Schema==1 && m_FiniteManifest.Ids.Count()==3,"Separate-process restart loads its original native save manifest");
  VerifyLifeAccounts();
  VerifyClinicalPatients();
  VerifyPhysicalBag();
  VerifySavedCargo();
  SaveGame active=GetGame().GetSaveGameManager().GetActiveSave();
  m_Suite.Record(m_Persistence.WasDataLoaded() && active && active.GetId()==m_FiniteManifest.SaveId,"Separate native process actually loaded the requested complete world save UUID");
  for(int index=0;index<m_FiniteManifest.Ids.Count();index++)
  {
   IEntity item=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.Ids[index]));BCMM_EquipmentItemComponent source;
   if(item)source=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
   array<float> state={};if(source)state=source.BCMM_CaptureFiniteSupply();
   m_Suite.Record(source && state.Count()==4 && state[1]==m_FiniteManifest.Kinds[index] && state[2]==m_FiniteManifest.Capacities[index] && Math.AbsFloat(state[3]-m_FiniteManifest.Remaining[index])<0.001,"Real world restart preserves original finite supply UUID, type, capacity and partial contents");
   PrintFormat("BCMM_FINITE_RESTART index=%1 item=%2 state=%3",index,item,state);
  }
  BeginMissingLifeRecord();
 }
 // Test-only injected identities; this proves accounting/native persistence,
 // not authentication against the live backend.
 protected void BindLifeTestIdentities()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  mode.BCMM_TestLifeIdentity(90001,"11111111-1111-4111-8111-111111111111");
  mode.BCMM_TestLifeIdentity(90002,"22222222-2222-4222-8222-222222222222");
  mode.BCMM_TestLifeIdentity(90003,"33333333-3333-4333-8333-333333333333");
 }
 protected void PrepareLifeAccounts()
 {
  BindLifeTestIdentities();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Suite.Record(mode.BCMM_ReserveLife(90001,m_Actor) && mode.BCMM_CommitLife(90001,m_Actor),"Saved account A commits a real surviving native character");
  m_Suite.Record(mode.BCMM_ReserveLife(90002,m_Target) && mode.BCMM_CommitLife(90002,m_Target),"Saved account B commits its own separate native character");
  mode.BCMM_TestLifeDisconnect(90002);
  SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_TestKill();
  m_Suite.Record(mode.BCMM_ReserveLife(90003,m_Floor),"Saved account C has an uncommitted preparation reservation");
 }
 protected void CompleteLifeSeed()
 {
  BindLifeTestIdentities();
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  PrintFormat("BCMM_DEATH_SEED state=%1 ledger=%2",ChimeraCharacter.Cast(m_Target).GetCharacterController().GetLifeState(),mode.BCMM_AccountFor(90002).Ledger.BCMM_CaptureLedger());
  m_Suite.Record(!mode.BCMM_AccountFor(90002).Ledger.IsAlive() && !mode.BCMM_ConfirmLifeDeath(90002,m_Target),"Native controllable death confirms offline test account B exactly once before save");
  BCMM_LifeAccount account=mode.BCMM_AccountFor(90002);int total=mode.BCMM_ReadSetting("totalLives");
  for(int index=1;index<total;index++)account.Ledger.AdjustAllowance(-1);
  m_Suite.Record(!mode.BCMM_LifeCanDeploy(90002) && account.Ledger.RemainingDeployments(total)==0,"Saved account B has an exhausted deployment allowance");
 }
 protected void VerifyLifeAccounts()
 {
  BindLifeTestIdentities();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());int total=mode.BCMM_ReadSetting("totalLives");
  BCMM_LifeAccount first=mode.BCMM_AccountFor(90001);BCMM_LifeAccount second=mode.BCMM_AccountFor(90002);BCMM_LifeAccount pending=mode.BCMM_AccountFor(90003);
  PrintFormat("BCMM_RESTART_LIFE alive=%1 body=%2 id=%3 state=%4",first.Ledger.IsAlive(),first.ActiveEntity,first.PersistentActiveEntityId,first.Ledger.BCMM_CaptureLedger());
  m_Suite.Record(first && first.Ledger.IsAlive() && first.ActiveEntity && first.Ledger.RemainingDeployments(total)==total-1,"Native world restart preserves account A's paid life and original surviving body");
  m_Suite.Record(first && first.ActiveEntity && mode.BCMM_LifeCanDeploy(90001,first.ActiveEntity) && !mode.BCMM_LifeCanDeploy(90001,m_Actor),"Restored active-body resume is allowed while a different character cannot bypass the ledger");
  m_Suite.Record(second && !second.Ledger.IsAlive() && second.Ledger.RemainingDeployments(total)==0 && !mode.BCMM_LifeCanDeploy(90002),"Exhausted dead account remains exhausted after real server restart");
  m_Suite.Record(pending && !pending.Ledger.IsAlive() && pending.Ledger.RemainingDeployments(total)==total && mode.BCMM_LifeCanDeploy(90003),"Uncommitted preparation does not charge or lock a life across restart");
  mode.BCMM_TestLifeIdentity(90001,"22222222-2222-4222-8222-222222222222");
  m_Suite.Record(mode.BCMM_AccountFor(90001)==second && !mode.BCMM_LifeCanDeploy(90001),"Reusing a connection ID cannot transfer another saved account's balance");
  mode.BCMM_TestLifeIdentity(90004,"11111111-1111-4111-8111-111111111111");
  m_Suite.Record(mode.BCMM_AccountFor(90004)==first && mode.BCMM_LifeCanDeploy(90004,first.ActiveEntity),"Same saved account resumes through a new connection ID without a second charge");
  array<int> before=first.Ledger.BCMM_CaptureLedger();array<int> invalid={2,0,0,0,0,0};
  m_Suite.Record(!first.Ledger.BCMM_RestoreLedger(invalid) && first.Ledger.RemainingDeployments(total)==total-1,"Invalid ledger schema cannot reset the saved allowance");
 }
 protected int m_MissingLifeToken;
 protected void BeginMissingLifeRecord()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  mode.BCMM_TestLifeIdentity(90005,"55555555-5555-4555-8555-555555555555");
  m_Suite.Record(mode.BCMM_ReserveLife(90005,m_Actor) && mode.BCMM_CommitLife(90005,m_Actor),"Missing-body fixture starts with one genuinely committed deployment");
  BCMM_LifeAccount account=mode.BCMM_AccountFor(90005);m_MissingLifeToken=account.Ledger.CurrentLife();account.ActiveEntity=null;account.PersistentActiveEntityId="aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaaa";
  mode.BCMM_ResolveSavedLifeBody(90005);
  GetGame().GetCallqueue().CallLater(CheckMissingLifeRecord,1200);
 }
 protected void CheckMissingLifeRecord()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_LifeAccount account=mode.BCMM_AccountFor(90005);int total=mode.BCMM_ReadSetting("totalLives");
  m_Suite.Record(!account.Ledger.IsAlive() && account.Ledger.RemainingDeployments(total)==total-1 && mode.BCMM_LifeCanDeploy(90005),"Actual native missing-record response retires the stale body without refunding or permanently locking the account");
  m_Suite.Record(mode.BCMM_ReserveLife(90005,m_Target) && mode.BCMM_CommitLife(90005,m_Target),"A new permitted paid deployment follows missing-record recovery");
  mode.BCMM_ApplySavedLifeBodyResult("55555555-5555-4555-8555-555555555555",m_MissingLifeToken,"aaaaaaaa-aaaa-4aaa-8aaa-aaaaaaaaaaaa",EPersistenceStatusCode.NOT_FOUND,null);
  m_Suite.Record(account.Ledger.IsAlive() && account.ActiveEntity==m_Target && account.Ledger.RemainingDeployments(total)==total-2,"A delayed old-body lookup cannot retire or refund the newer paid life");
  array<ref BCMM_SavedLifeAccount> records;
  m_Suite.Record(mode.BCMM_CaptureLifeAccounts(m_Persistence,records) && records.Count()==4,"Complete live account snapshot includes all four independent test accounts");
  BCMM_LifeAccount previous=mode.BCMM_AccountFor(90004);
  array<ref BCMM_SavedLifeAccount> duplicate={};foreach(BCMM_SavedLifeAccount record:records)duplicate.Insert(record);duplicate.Insert(records[0]);
  m_Suite.Record(!mode.BCMM_RestoreLifeAccounts(m_Persistence,duplicate) && mode.BCMM_AccountFor(90004)==previous,"Duplicate saved identities reject the whole import without replacing any live account");
  mode.BCMM_LifePersistenceFault();
  m_Suite.Record(!mode.BCMM_LifeCanDeploy(90003) && !mode.BCMM_ResetAllLives(null) && !mode.BCMM_LifeCanDeploy(90003),"Invalid persisted lives fail closed and unauthenticated reset cannot unlock them");
  m_Suite.Record(mode.BCMM_RestoreLifeAccounts(m_Persistence,records) && mode.BCMM_LifeCanDeploy(90003),"A fully validated saved-account snapshot restores normal policy after rejection");
  BeginRestoredCargoCycle();
 }
 protected bool ReadPatientCodec(BCMM_Patient patient,out BCMM_Patient restored)
 {
  SCR_PersistenceJsonSaveContext save=new SCR_PersistenceJsonSaveContext();
  if(!save.WriteValue("patient",patient))return false;
  SCR_PersistenceJsonLoadContext load=new SCR_PersistenceJsonLoadContext();
  return load.LoadFromString(save.SaveToString()) && load.ReadValue("patient",restored);
 }
 protected void CheckClinicalCodecs()
 {
  BCMM_Settings settings=new BCMM_Settings();BCMM_Patient original=new BCMM_Patient();
  int id=original.AddWound(BCMM_BodyRegion.LEFT_ARM,3);original.DressWound(id,0.5,settings);original.WoundAt(0).Advance(17,BCMM_Activity.RESTING,settings);
  original.Medication.Administer(BCMM_MedicationKind.TXA,settings);original.Medication.Advance(11,original,settings);
  BCMM_Patient restored;
  m_Suite.Record(ReadPatientCodec(original,restored) && restored!=original && restored.WoundCount()==1 && Math.AbsFloat(restored.WoundAt(0).BCMM_TestWearRemaining()-original.WoundAt(0).BCMM_TestWearRemaining())<0.001 && !restored.Medication.CanAdminister(BCMM_MedicationKind.TXA,settings),"Native JSON codec preserves protected dressing and drug cooldown state in a separate model");
  original.LeftLeg.Splinted=true;
  m_Suite.Record(!ReadPatientCodec(original,restored),"Invalid saved splint without fracture is rejected by the actual patient codec");original.LeftLeg.Splinted=false;
  original.WoundAt(0).UncontrolledFlowMlPerSecond=-1;
  m_Suite.Record(!ReadPatientCodec(original,restored),"Negative wound flow is rejected by the nested wound codec");original.WoundAt(0).UncontrolledFlowMlPerSecond=3;
  original.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN,BCMM_BleedCompartment.ABDOMEN,1);original.InternalBleedAt(0).PooledMl=-1;
  m_Suite.Record(!ReadPatientCodec(original,restored),"Negative saved pooled bleeding is rejected before live patient replacement");
 }
 protected IEntity m_SavedCargoVehicle;
 protected IEntity m_SavedCargoBag;
 protected IEntity m_SavedCargoSupply;
 protected int m_SavedCargoWait;
 protected int m_SavedCargoToken;
 protected int m_RestoredCargoWait;
 protected int m_RestoredCargoLifeBalance;
 protected void BeginRestoredCargoCycle()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_RestoredCargoLifeBalance=mode.BCMM_AccountFor(90004).Ledger.RemainingDeployments(mode.BCMM_ReadSetting("totalLives"));
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_SavedCargoVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  InventoryItemComponent item=InventoryItemComponent.Cast(m_SavedCargoBag.FindComponent(InventoryItemComponent));
  m_Suite.Record(item.GetParentSlot() && inventory.TryRemoveItemFromStorage(m_SavedCargoBag,item.GetParentSlot().GetStorage(),null),"Actual native removal unloads the restored original cargo bag");
  GetGame().GetCallqueue().CallLater(CheckRestoredCargoUnloaded,500);
 }
 protected void CheckRestoredCargoUnloaded()
 {
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_SavedCargoBag.FindComponent(BCMM_BodyBagComponent));
  if((!state.Ready() || state.State()!=BCMM_BagCargoState.WORLD) && m_RestoredCargoWait++<20){GetGame().GetCallqueue().CallLater(CheckRestoredCargoUnloaded,250);return;}
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_SavedCargoVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  InventoryStorageManagerComponent bagInventory=InventoryStorageManagerComponent.Cast(m_SavedCargoBag.FindComponent(InventoryStorageManagerComponent));BCMM_VehicleCargoComponent cargo=BCMM_VehicleCargoComponent.Cast(m_SavedCargoVehicle.FindComponent(BCMM_VehicleCargoComponent));
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_SavedCargoToken);
  m_Suite.Record(state.Ready() && state.State()==BCMM_BagCargoState.WORLD && !inventory.Contains(m_SavedCargoBag) && inventory.Contains(m_SavedCargoSupply) && !bagInventory.Contains(m_SavedCargoSupply),"Post-restart unloading leaves the exact recovered AED in the vehicle without copying it back into the bag");
  m_Suite.Record(cargo.Count()==0 && state.VehicleId()==RplId.Invalid() && record && !record.StoredVehicle,"Post-restart unloading clears the current replicated cargo manifest and physical vehicle association");
  BaseInventoryStorageComponent destination=BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_SavedCargoBag.GetPrefabData().GetPrefabName());
  m_Suite.Record(destination && inventory.TryInsertItemInStorage(m_SavedCargoBag,destination),"Actual native reinsertion reloads the same original bag after world restart");m_RestoredCargoWait=0;
  GetGame().GetCallqueue().CallLater(CheckRestoredCargoReloaded,500);
 }
 protected void CheckRestoredCargoReloaded()
 {
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_SavedCargoBag.FindComponent(BCMM_BodyBagComponent));
  if((!state.Ready() || state.State()!=BCMM_BagCargoState.STORED) && m_RestoredCargoWait++<20){GetGame().GetCallqueue().CallLater(CheckRestoredCargoReloaded,250);return;}
  BCMM_VehicleCargoComponent cargo=BCMM_VehicleCargoComponent.Cast(m_SavedCargoVehicle.FindComponent(BCMM_VehicleCargoComponent));
  m_Suite.Record(state.Ready() && state.State()==BCMM_BagCargoState.STORED && cargo.Count()==1 && cargo.BagAt(0)==BCMM_BodyBagComponent.Id(m_SavedCargoBag) && BCMM_EquipmentItemComponent.Cast(m_SavedCargoSupply.FindComponent(BCMM_EquipmentItemComponent)).ShocksRemaining()==19,"Post-restart reload publishes exactly one original bag and preserves the already deposited partial AED");
  SCR_DamageManagerComponent.GetDamageManager(m_SavedCargoVehicle).Kill(Instigator.CreateInstigator(m_Actor));
  GetGame().GetCallqueue().CallLater(CheckRestoredCargoWrecked,750);
 }
 protected void CheckRestoredCargoWrecked()
 {
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_SavedCargoToken);
  m_Suite.Record(SCR_DamageManagerComponent.GetDamageManager(m_SavedCargoVehicle).IsDestroyed() && (!m_SavedCargoBag || m_SavedCargoBag.IsDeleted()) && record && record.Disposition==BCMM_RemainsDisposition.VEHICLE_LOST,"Actual native destruction of the restored/reloaded vehicle disposes the exact stored bag and retains its vehicle-loss record");
  SCR_EntityHelper.DeleteEntityAndChildren(m_SavedCargoVehicle);GetGame().GetCallqueue().CallLater(CheckRestoredCargoDeleted,500);
 }
 protected void CheckRestoredCargoDeleted()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(m_SavedCargoToken);
  IEntity independent=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.WorldIds[0]));
  m_Suite.Record((!m_SavedCargoSupply || m_SavedCargoSupply.IsDeleted()) && record && record.Disposition==BCMM_RemainsDisposition.VEHICLE_LOST && independent && !independent.IsDeleted(),"Deleting the restored wreck follows native equipment lifetime, retains the exact disposition and preserves unrelated world remains");
  m_Suite.Record(mode.BCMM_AccountFor(90004).Ledger.IsAlive() && mode.BCMM_AccountFor(90004).Ledger.RemainingDeployments(mode.BCMM_ReadSetting("totalLives"))==m_RestoredCargoLifeBalance,"Saved cargo unload, reload, native destruction and deletion never charge, refund or gate the surviving paid life");
  SaveGameManager manager=GetGame().GetSaveGameManager();manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM finite-supply restart",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnWorldSaved));
 }
 protected void SeedCargoRecovery()
 {
  float surface=GetGame().GetWorld().GetSurfaceY(110,0);
  string selection;System.GetCLIParam("bcmmSavedCargoVehicle",selection);
  ResourceName vehiclePrefab="{6AB4B3F3FD216004}Prefabs/Vehicles/BCMMedical/BCMM_CargoTruck.et";
  if(selection=="Jeep")vehiclePrefab="{6AB4B3F3FD216005}Prefabs/Vehicles/BCMMedical/BCMM_CargoJeep.et";
  if(selection=="Helicopter")vehiclePrefab="{6AB4B3F3FD216003}Prefabs/Vehicles/BCMMedical/BCMM_CargoHelicopter.et";
  if(selection=="Ambulance")vehiclePrefab="{6AB4B3F3FD216002}Prefabs/Vehicles/BCMMedical/BCMM_CargoAmbulance.et";
  m_SavedCargoVehicle=Spawn(vehiclePrefab,Vector(110,surface+0.2,0));
  m_SavedCargoVehicle.GetPhysics().SetVelocity(vector.Zero);m_SavedCargoVehicle.GetPhysics().SetActive(ActiveState.INACTIVE);
  m_SavedCargoBag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et",Vector(109,surface+0.5,0));
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_SavedCargoBag.FindComponent(BCMM_BodyBagComponent));state.SetDeceasedName("BCM saved vehicle casualty");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRegisterSavedCargo(m_SavedCargoBag);
  m_FiniteManifest.CargoRemainsToken=state.RemainsToken();
  m_SavedCargoSupply=Spawn("{6AB4B3F3FD216025}Prefabs/Items/BCMM_AED.et",m_SavedCargoBag.GetOrigin());
  BCMM_EquipmentItemComponent.Cast(m_SavedCargoSupply.FindComponent(BCMM_EquipmentItemComponent)).Discharge();
  BCMM_BagStorageComponent storage=BCMM_BagStorageComponent.Cast(m_SavedCargoBag.FindComponent(BCMM_BagStorageComponent));storage.SetTransferring(true);
  bool inserted=InventoryStorageManagerComponent.Cast(m_SavedCargoBag.FindComponent(InventoryStorageManagerComponent)).TryInsertItem(m_SavedCargoSupply,EStoragePurpose.PURPOSE_ANY);storage.SetTransferring(false);
  m_Suite.Record(inserted,"Saved cargo recovery starts with a partially discharged original AED inside the bag");
  array<IEntity> entities={m_SavedCargoVehicle,m_SavedCargoBag,m_SavedCargoSupply};
  foreach(IEntity entity:entities){m_Suite.Record(m_Persistence.IsTracked(entity),"Native saved cargo entity has a real persistence configuration");m_FiniteManifest.CargoIds.Insert(m_Persistence.GetId(entity));}
  SCR_VehicleInventoryStorageManagerComponent inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(m_SavedCargoVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BaseInventoryStorageComponent destination=BCMM_BagInventoryPolicy.FindVehicleStorage(inventory,m_SavedCargoBag.GetPrefabData().GetPrefabName());
  m_Suite.Record(destination && inventory.TryInsertItemInStorage(m_SavedCargoBag,destination),"Native vehicle storage accepts the physical recovery bag before world save");
 }
 protected void VerifySavedCargo()
 {
  m_Suite.Record(m_FiniteManifest.CargoIds.Count()==3,"Saved-world manifest retains original vehicle, physical bag and AED UUIDs");if(m_FiniteManifest.CargoIds.Count()!=3)return;
  IEntity vehicle=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.CargoIds[0]));IEntity bag=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.CargoIds[1]));IEntity supply=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.CargoIds[2]));
  SCR_VehicleInventoryStorageManagerComponent inventory;if(vehicle)inventory=SCR_VehicleInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_BodyBagComponent state;if(bag)state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  BCMM_EquipmentItemComponent contents;if(supply)contents=BCMM_EquipmentItemComponent.Cast(supply.FindComponent(BCMM_EquipmentItemComponent));
  PrintFormat("BCMM_CARGO_RESTART vehicle=%1 bag=%2 source=%3 inventory=%4 bagContained=%5 sourceContained=%6",vehicle,bag,supply,inventory!=null,inventory && bag && inventory.Contains(bag),inventory && supply && inventory.Contains(supply));
  m_Suite.Record(inventory && state && contents && inventory.Contains(bag) && inventory.Contains(supply),"Native vehicle hierarchy restores the same physical bag and independently deposited original equipment");
  if(!inventory || !state || !contents)return;
  m_Suite.Record(state.Ready() && state.State()==BCMM_BagCargoState.STORED && BCMM_BodyBagComponent.Resolve(state.VehicleId())==vehicle && contents.ShocksRemaining()==19,"Reconstructed cargo state uses the current vehicle RplId and retains partial AED charges");
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(state.RemainsToken());
  m_Suite.Record(record && record.Physical==bag && record.StoredVehicle==vehicle && state.DeceasedName()=="BCM saved vehicle casualty","Restored physical remains registry binds the exact native cargo vehicle");
  InventoryStorageManagerComponent bagInventory=InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(!bagInventory.Contains(supply),"Restart does not duplicate equipment back into a previously emptied cargo bag");
  m_SavedCargoVehicle=vehicle;m_SavedCargoBag=bag;m_SavedCargoSupply=supply;m_SavedCargoToken=state.RemainsToken();
 }
 protected void SeedPhysicalBag()
 {
  IEntity bag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et","101 1.2 2");
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));state.SetDeceasedName("BCM saved recovery casualty");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Suite.Record(mode.BCMM_TransferRemains(m_Target,bag),"World save fixture transfers the exact dead native account body into its physical bag");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Target);m_Target=null;
  IEntity supply=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",bag.GetOrigin());
  BCMM_EquipmentItemComponent.Cast(supply.FindComponent(BCMM_EquipmentItemComponent)).ConsumeFluidMl(150);
  BCMM_BagStorageComponent storage=BCMM_BagStorageComponent.Cast(bag.FindComponent(BCMM_BagStorageComponent));storage.SetTransferring(true);
  bool inserted=InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent)).TryInsertItem(supply,EStoragePurpose.PURPOSE_ANY);storage.SetTransferring(false);
  m_Suite.Record(inserted,"Saved physical bag contains original partially used recovery equipment");
  m_Suite.Record(m_Persistence.IsTracked(bag),"Filled physical bag resolves an actual native persistence configuration");
  m_Suite.Record(m_Persistence.IsTracked(supply),"Recovered partial supply retains a persistent UUID inside its bag");
  m_FiniteManifest.WorldIds={m_Persistence.GetId(bag),m_Persistence.GetId(supply)};
 }
 protected void VerifyPhysicalBag()
 {
  m_Suite.Record(m_FiniteManifest.WorldIds.Count()==2,"Separate restart reads original physical recovery UUIDs");if(m_FiniteManifest.WorldIds.Count()!=2)return;
  IEntity bag=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.WorldIds[0]));IEntity supply=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.WorldIds[1]));
  BCMM_BodyBagComponent state;if(bag)state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(state && state.Ready() && state.DeceasedName()=="BCM saved recovery casualty" && state.State()==BCMM_BagCargoState.WORLD,"Real native world restart retains physical bag identity and stable world state");if(!state)return;
  BCMM_EquipmentItemComponent contents;if(supply)contents=BCMM_EquipmentItemComponent.Cast(supply.FindComponent(BCMM_EquipmentItemComponent));
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(bag.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(contents && inventory.Contains(supply) && Math.AbsFloat(contents.FluidRemainingMl()-350)<0.001,"Recovery-only native bag inventory restores its exact partial supply without refill or duplicate deposit");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(state.RemainsToken());
  m_Suite.Record(record && record.Physical==bag && record.PlayerIdentity=="22222222-2222-4222-8222-222222222222" && record.Life==1 && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"Restored physical remains retains immutable account/life metadata independently of deployment policy");
  IEntity extra=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",bag.GetOrigin());
  m_Suite.Record(!inventory.TryInsertItem(extra,EStoragePurpose.PURPOSE_ANY),"Native world restoration does not leave arbitrary bag insertion unlocked");SCR_EntityHelper.DeleteEntityAndChildren(extra);
 }
 protected void SeedClinicalPatients()
 {
  BCMM_Settings settings=new BCMM_Settings();
  IEntity injured=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","102 1.2 0");
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(injured.FindComponent(SCR_CharacterDamageManagerComponent));
  medical.BCMM_TestSeedSavedInjuries();BCMM_Patient patient=medical.BCMM_GetPatient();
  patient.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN,BCMM_BleedCompartment.ABDOMEN,0.1);
  patient.LeftLeg.Broken=true;patient.LeftLeg.ApplySplint();patient.LeftChest.OpenWound=true;patient.LeftChest.ApplySeal();
  patient.Equipment.VascularAccess=true;patient.Equipment.VascularAccessRegion=BCMM_BodyRegion.RIGHT_ARM;patient.Equipment.PadsAttached=true;patient.Equipment.AnalysisDevice=777;patient.Equipment.LastAnalysisAdvised=true;patient.Equipment.AnalysisAge=0;
  m_Suite.Record(patient.Medication.Administer(BCMM_MedicationKind.MORPHINE,settings),"Saved patient receives an active medication with its repeat lock");
  medical.GetBloodHitZone().SetHealth(medical.GetBloodHitZone().GetMaxHealth()*0.7);
  medical.BCMM_PublishFractures();
  m_FiniteManifest.ClinicalWear=patient.WoundAt(0).BCMM_TestWearRemaining();
  m_Suite.Record(m_Persistence.IsTracked(injured) || m_Persistence.StartTracking(injured,false),"Wounded patient is tracked by the native character collection");
  m_FiniteManifest.ClinicalIds.Insert(m_Persistence.GetId(injured));
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(injured.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",injured.GetOrigin());
  IEntity cylinder=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",injured.GetOrigin());
  m_Suite.Record(inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && inventory.TryInsertItem(cylinder,EStoragePurpose.PURPOSE_ANY),"Saved oxygen source is in the actual patient's native inventory");
  m_Suite.Record(m_Persistence.IsTracked(cylinder) || m_Persistence.StartTracking(cylinder,false),"Saved connected oxygen source receives its own persistent UUID");
  BCMM_EquipmentItemComponent gas=BCMM_EquipmentItemComponent.Cast(cylinder.FindComponent(BCMM_EquipmentItemComponent));gas.ConsumeOxygen(1);
  BCMM_OxygenSession.Start(injured,injured,cylinder);
  m_FiniteManifest.ClinicalOxygenId=m_Persistence.GetId(cylinder);m_FiniteManifest.ClinicalOxygenGas=gas.OxygenLitres();
  m_Suite.Record(patient.Equipment.OxygenActive,"Saved patient has a real active finite oxygen session");
  IEntity depleted=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","103 1.2 0");
  medical=SCR_CharacterDamageManagerComponent.Cast(depleted.FindComponent(SCR_CharacterDamageManagerComponent));medical.BCMM_TestSeedSavedDepletion();
  m_Suite.Record(medical.BCMM_IsDepleted() && !medical.BCMM_CanDie(),"Saved native patient is alive with an already running depletion timer");
  m_Suite.Record(m_Persistence.IsTracked(depleted) || m_Persistence.StartTracking(depleted,false),"Depleted patient is tracked in the native world save");
  m_FiniteManifest.ClinicalIds.Insert(m_Persistence.GetId(depleted));
  IEntity station=Spawn("{6AB4B3F3FD21B100}PrefabsEditable/Auto/Items/E_BCMM_FieldSurgeryStation.et","103 1 0");
  BCMM_StretcherComponent.Cast(station.FindComponent(BCMM_StretcherComponent)).BCMM_TestSeedStationaryPatient(depleted);
  m_Suite.Record(m_Persistence.IsTracked(station),"Stationary medical stretcher resolves native world persistence");m_FiniteManifest.StationId=m_Persistence.GetId(station);
  IEntity grave=Spawn("{6AB4B3F3FD21E001}Prefabs/Graves/BCMM_Grave.et","104 1 0");
  BCMM_RemainsRecord memorial=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestCreateSavedMemorial();
  BCMM_GraveMarkerComponent.Cast(grave.FindComponent(BCMM_GraveMarkerComponent)).SetMemorial(memorial);
  m_Suite.Record(m_Persistence.IsTracked(grave),"Named physical grave resolves native world persistence");m_FiniteManifest.GraveId=m_Persistence.GetId(grave);
 }
 protected void VerifyClinicalPatients()
 {
  m_Suite.Record(m_FiniteManifest.ClinicalIds.Count()==2,"Restart manifest includes both original saved patient UUIDs");
  if(m_FiniteManifest.ClinicalIds.Count()!=2)return;
  IEntity injured=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalIds[0]));
  SCR_CharacterDamageManagerComponent medical;if(injured)medical=SCR_CharacterDamageManagerComponent.Cast(injured.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(medical!=null,"Native restart restores the original wounded patient entity");if(!medical)return;
  BCMM_Patient patient=medical.BCMM_GetPatient();BCMM_Settings settings=new BCMM_Settings();
  m_Suite.Record(patient.WoundCount()==1 && patient.WoundAt(0).Bandaged && medical.BCMM_TestSavedNativeLinks()==1,"Native wound identity and exact hit-zone link survive without duplicate wounds");
  float wear=patient.WoundAt(0).BCMM_TestWearRemaining();
  m_Suite.Record(wear>0 && wear<=m_FiniteManifest.ClinicalWear && wear>m_FiniteManifest.ClinicalWear-15,"Partially worn dressing retains its remaining wear budget across restart");
  m_Suite.Record(patient.InternalBleedCount()==1 && patient.InternalBleedAt(0).Region==BCMM_BodyRegion.ABDOMEN && patient.InternalBleedAt(0).PooledMl>0,"Internal injury and accumulated bleeding continue after load");
  m_Suite.Record(patient.LeftLeg.Broken && patient.LeftLeg.Splinted && !medical.BCMM_LegsAllowJump(),"Saved splinted fracture republishes actual movement restrictions");
  m_Suite.Record(patient.LeftChest.OpenWound && patient.LeftChest.Sealed && patient.Equipment.VascularAccess && patient.Equipment.VascularAccessRegion==BCMM_BodyRegion.RIGHT_ARM && patient.Equipment.PadsAttached,"Applied chest seal, vascular access location and AED pads survive restart");
  m_Suite.Record(patient.Equipment.AnalysisDevice==0 && !patient.Equipment.LastAnalysisAdvised,"Restart requires fresh AED analysis instead of reusing a process-local device id");
  m_Suite.Record(patient.Medication.ActiveDoses()==1 && patient.Medication.HasActiveOpioid() && !patient.Medication.CanAdminister(BCMM_MedicationKind.MORPHINE,settings),"Medication doses and repeat cooldown do not reset after world restart");
  m_Suite.Record(medical.GetBloodHitZone().GetHealthScaled()>0.65 && medical.GetBloodHitZone().GetHealthScaled()<0.71,"Native health serializer retains actual partial circulating blood alongside clinical data");
  IEntity cylinder=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalOxygenId));
  BCMM_EquipmentItemComponent gas;if(cylinder)gas=BCMM_EquipmentItemComponent.Cast(cylinder.FindComponent(BCMM_EquipmentItemComponent));
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(injured.FindComponent(InventoryStorageManagerComponent));
  PrintFormat("BCMM_OXYGEN_RESTART source=%1 gas=%2 contained=%3 active=%4 refs=%5",cylinder,gas!=null,cylinder && inventory.Contains(cylinder),patient.Equipment.OxygenActive,medical.BCMM_CaptureClinicalState(m_Persistence).OxygenRefs);
  m_Suite.Record(gas && inventory.Contains(cylinder) && patient.Equipment.OxygenActive,"Actual saved oxygen source and patient UUIDs reconnect after native inventory restoration");
  m_Suite.Record(gas && gas.OxygenLitres()<=m_FiniteManifest.ClinicalOxygenGas && gas.OxygenLitres()>m_FiniteManifest.ClinicalOxygenGas-2,"Restored oxygen resumes consumption from saved partial gas without refill");
  IEntity grave=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.GraveId));BCMM_GraveMarkerComponent marker;if(grave)marker=BCMM_GraveMarkerComponent.Cast(grave.FindComponent(BCMM_GraveMarkerComponent));
  BCMM_RemainsRecord memorial;if(marker)memorial=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(marker.RemainsToken());
  m_Suite.Record(marker && marker.DeceasedName()=="BCM saved memorial" && memorial && memorial.Disposition==BCMM_RemainsDisposition.BURIED,"Native grave identity and disposed remains tombstone survive a real world restart");
  IEntity depleted=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalIds[1]));medical=null;if(depleted)medical=SCR_CharacterDamageManagerComponent.Cast(depleted.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(medical && medical.BCMM_IsDepleted() && !medical.BCMM_CanDie() && ChimeraCharacter.Cast(depleted).GetCharacterController().IsUnconscious(),"Native depleted patient remains alive and unconscious after restart");
  if(medical)
  {
   array<float> timer=medical.BCMM_CaptureClinicalState(m_Persistence).Depletion;
   m_Suite.Record(timer[0]==1 && timer[1]==0 && timer[2]>10 && timer[2]<24,"Saved remaining depletion grace resumes without receiving a fresh sixty seconds");
   IEntity station=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.StationId));BCMM_StretcherComponent stretcher;if(station)stretcher=BCMM_StretcherComponent.Cast(station.FindComponent(BCMM_StretcherComponent));
   m_Suite.Record(stretcher && stretcher.Patient()==depleted && BCMM_StretcherComponent.Stabilises(depleted) && !stretcher.HasCarriers() && !stretcher.IsCarried(),"Stationary stretcher rebinds the original unconscious patient UUID without restoring carrying");
   if(station)SCR_EntityHelper.DeleteEntityAndChildren(station);
   m_Suite.Record(depleted && !depleted.IsDeleted() && !medical.BCMM_CanDie(),"Deleting a restored stationary stretcher leaves its native patient alive in the world");
  }
 }
}
modded class BCMM_Wound
{
 float BCMM_TestWearRemaining(){return m_DressingWearRemaining;}
}
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_TestSeedSavedInjuries()
 {
  SCR_CharacterHitZone zone=SCR_CharacterHitZone.Cast(GetHitZoneByName("Chest"));
  BCMM_AddNativeWound(zone,BCMM_BodyRegion.CHEST,BCMM_InjuryCause.GUNSHOT,0.2);
  m_BCMMPatient.DressWound(m_BCMMPatient.WoundAt(0).Id,0.5,m_BCMMClinicalSettings);
  m_BCMMPatient.WoundAt(0).Advance(10,BCMM_Activity.RESTING,m_BCMMClinicalSettings);
 }
 void BCMM_TestSeedSavedDepletion()
 {
  GetBloodHitZone().SetHealth(0);UpdateConsciousness();
  array<float> state={1,0,23};m_BCMMDepletion.BCMM_RestoreDepletion(state,BCMM_Now());
 }
 int BCMM_TestSavedNativeLinks(){return m_BCMMNativeWounds.Count();}
}
modded class BCMM_StretcherComponent
{
 void BCMM_TestSeedStationaryPatient(IEntity patient)
 {
  m_Patient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();
 }
}
modded class SCR_BaseGameMode
{
 BCMM_RemainsRecord BCMM_TestCreateSavedMemorial()
 {
  BCMM_RemainsRecord record=new BCMM_RemainsRecord();record.Token=++m_BCMMNextRemainsToken;record.DeceasedName="BCM saved memorial";record.DeathSeconds=GetGame().GetWorld().GetWorldTime()*0.001-12;record.Disposition=BCMM_RemainsDisposition.BURIED;m_BCMMRemainsRecords.Set(record.Token,record);return record;
 }
}
modded class SCR_BaseGameMode
{
 void BCMM_TestRegisterSavedCargo(IEntity bag)
 {
  BCMM_RemainsRecord record=new BCMM_RemainsRecord();record.Token=++m_BCMMNextRemainsToken;record.DeceasedName="BCM saved vehicle casualty";record.DeathSeconds=GetGame().GetWorld().GetWorldTime()*0.001-5;record.Physical=bag;m_BCMMRemainsRecords.Set(record.Token,record);BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent)).SetRemainsToken(record.Token);
 }
}