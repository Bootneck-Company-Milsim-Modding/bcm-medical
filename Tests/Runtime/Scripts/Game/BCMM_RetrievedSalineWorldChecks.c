// Private reproduction of a used original stock bag retrieved before native world save.
class BCMM_RetrievedSalineActors : JsonApiStruct
{
 string ActorId;
 string FreshSourceId;
 void BCMM_RetrievedSalineActors()
 {
  RegV("ActorId");
  RegV("FreshSourceId");
 }
}
class BCMM_RetrievedSalineWorldChecks : BCMM_NativeSalineWorldChecks
{
 protected ref BCMM_RetrievedSalineActors m_Actors = new BCMM_RetrievedSalineActors();
 override protected void CheckFlow()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  BCMM_SavedNativeSaline captured = BCMM_NativeSalineBinding.Capture(m_Target, persistence);
  m_Suite.Record(captured && captured.Valid(), "Retrieved native bag starts with its real original attached lifetime");
  if (!captured)
  {
   Finish();
   return;
  }
  BCMM_StockSourceLifeState ownState = BCMM_StockSourceLifetime.Capture(m_Kit);
  m_Suite.Record(ownState && ownState.Remaining > 0, "Actual native application registers a physical source lifetime before retrieval");
  Resource itemConfig = Resource.Load("{59A157EBABAC7AEC}Configs/Systems/Persistence/Configuration/Item.conf");
  bool hasSerializer;
  if (itemConfig && itemConfig.IsValid())
  {
   BaseContainer config = itemConfig.GetResource().ToBaseContainer();
   BaseContainerList serializers = config.GetObjectArray("ComponentSerializers");
   if (serializers)
   {
    for (int serializerIndex = 0; serializerIndex < serializers.Count(); serializerIndex++)
    {
     string serializerClass = serializers.Get(serializerIndex).GetClassName();
     PrintFormat("BCMM_ITEM_SERIALIZER class=%1", serializerClass);
     if (serializerClass == "BCMM_NativeStockLifeSerializer")
     {
      hasSerializer = true;
     }
    }
   }
  }
  m_Suite.Record(hasSerializer, "Actual loaded native item configuration contains the isolated source-lifetime serializer");
  m_Save.LifeRemaining = captured.LifeSeconds;
  m_Save.Active = false;
  m_Save.PatientId = persistence.GetId(m_Target);
  m_Save.SourceId = persistence.GetId(m_Kit);
  m_Actors.ActorId = persistence.GetId(m_Actor);
  IEntity pack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack, EStoragePurpose.PURPOSE_ANY), "Retrieved-source controls have actual native inventory capacity");
  IEntity freshSource = Spawn("{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(freshSource && m_Inventory.TryInsertItem(freshSource, EStoragePurpose.PURPOSE_ANY) && persistence.IsTracked(freshSource), "A separate original unused native bag enters the saved actor inventory");
  m_Actors.FreshSourceId = persistence.GetId(freshSource);
  m_Suite.Record(persistence.IsTracked(m_Actor) && BCMM_LifeIdentityFormat.IsCanonical(m_Actors.ActorId), "Retrieved-bag actor is genuinely tracked under its original canonical native UUID");
  m_Suite.Record(m_Actors.PackToFile("$profile:BCMM_RetrievedSalineActor.json"), "Retrieved-bag fixture records its real native actor UUID");
  BaseInventoryStorageComponent destination = m_Inventory.FindStorageForItem(m_Kit, EStoragePurpose.PURPOSE_ANY);
  InventoryStorageManagerComponent patientInventory = InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(destination && patientInventory.TryMoveItemToStorage(m_Kit, destination), "Actual original used saline moves from the patient slot back into native actor inventory");
  GetGame().GetCallqueue().CallLater(SaveCurrentWorld, 700);
 }
 override protected void SaveCurrentWorld()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !BCMM_NativeSalineBinding.Attached(m_Target, m_Kit) && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), "Retrieved original native bag is physically owned by the actor and no longer delivering to the patient");
  m_Save.Volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  string fullHeal;
  if (System.GetCLIParam("bcmmRetrievedFullHeal", fullHeal))
  {
   m_Medical.FullHeal();
   m_Save.Volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  }
  m_Save.Cells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  SaveGameManager manager = GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED, "BCMM native stock saline", ESaveGameRequestFlags.BLOCKING, new SaveGameOperationCallback(OnSaved));
 }
}
class BCMM_RetrievedSalineRestartChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_StockSalineWorldManifest m_Save = new BCMM_StockSalineWorldManifest();
 protected ref BCMM_RetrievedSalineActors m_Actors = new BCMM_RetrievedSalineActors();
 protected float m_HeldVolume;
 protected IEntity m_FreshSource;
 override protected void Prepare()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_StockSalineWorld.json") && m_Actors.LoadFromFile("$profile:BCMM_RetrievedSalineActor.json") && persistence && persistence.WasDataLoaded(), "Separate native process loads the actual retrieved-source world and original UUID manifests");
  if (!persistence || !persistence.WasDataLoaded())
  {
   Finish();
   return;
  }
  m_Target = IEntity.Cast(persistence.FindById(m_Save.PatientId));
  m_Actor = IEntity.Cast(persistence.FindById(m_Actors.ActorId));
  m_Kit = IEntity.Cast(persistence.FindById(m_Save.SourceId));
  m_FreshSource = IEntity.Cast(persistence.FindById(m_Actors.FreshSourceId));
  if (m_Actor)
  {
   m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  }
  if (m_Target)
  {
   m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  PrintFormat("BCMM_RETRIEVED_RESTORE actor=%1 patient=%2 source=%3 inventory=%4", m_Actor, m_Target, m_Kit, m_Inventory);
  m_Suite.Record(m_Actor && m_Kit && m_Inventory && m_Inventory.Contains(m_Kit) && m_Medical && !BCMM_NativeSalineBinding.Attached(m_Target, m_Kit) && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), "Exact retrieved original source remains in the original actor inventory without phantom patient flow");
  if (!m_Medical || !m_Kit)
  {
   m_Suite.Complete();
   return;
  }
  m_HeldVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(m_FreshSource && m_Inventory.Contains(m_FreshSource) && !BCMM_StockSourceLifetime.Capture(m_FreshSource), "Unused original stock source restores without acquiring a used-source timer");
  GetGame().GetCallqueue().CallLater(CheckOriginalExpiry, (m_Save.LifeRemaining + 3) * 1000);
 }
 protected void CheckOriginalExpiry()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(!m_Kit && !persistence.FindById(m_Save.SourceId), "Retrieved original stock source retains its already-started native expiry across the actual world restart");
  m_Suite.Record(m_FreshSource && m_Inventory.Contains(m_FreshSource) && !BCMM_StockSourceLifetime.Capture(m_FreshSource), "Expiry of the exact used source leaves the original unused bag intact and fresh");
  m_Suite.Record(!m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && m_Medical.GetBloodHitZone().GetHealthScaled() <= m_HeldVolume + 0.0001, "Retrieved stock source never restarts patient delivery after save or expiry");
  m_Suite.Complete();
 }
}



