// Staged native original-stock source/effect world continuity. Never a refill adapter.
class BCMM_StockSalineWorldManifest : JsonApiStruct
{
 string SaveId;
 string PatientId;
 string SourceId;
 float Volume;
 float Cells;
 float EffectRemaining;
 float LifeRemaining;
 bool Active = true;
 void BCMM_StockSalineWorldManifest()
 {
  RegV("SaveId");
  RegV("PatientId");
  RegV("SourceId");
  RegV("Volume");
  RegV("Cells");
  RegV("EffectRemaining");
  RegV("LifeRemaining");
  RegV("Active");
 }
}
class BCMM_NativeSalineWorldChecks : BCMM_NativeSalineRuntimeChecks
{
 protected ref BCMM_StockSalineWorldManifest m_Save = new BCMM_StockSalineWorldManifest();
 override protected void CheckFlow()
 {
  string inactive;
  if (System.GetCLIParam("bcmmNativeSalineInactive", inactive))
  {
   m_Save.Active = false;
   m_Medical.BCMM_GetPatient().Equipment.VascularAccess = false;
   BCMM_VisualStatusBridge.Publish(m_Medical);
   GetGame().GetCallqueue().CallLater(SaveCurrentWorld, 700);
   return;
  }
  SaveCurrentWorld();
 }
 protected void SaveCurrentWorld()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  SCR_SalineDamageEffect effect = SCR_SalineDamageEffect.Cast(m_Medical.FindDamageEffectOfType(SCR_SalineDamageEffect));
  m_Suite.Record(persistence && persistence.IsTracked(m_Target) && m_Kit && persistence.IsTracked(m_Kit) && ((m_Save.Active && effect && effect.IsActive()) || (!m_Save.Active && !effect)), "Native world tracks original patient, attached original stock saline and expected active or stopped delivery state");
  if (!persistence || !m_Kit || (m_Save.Active && !effect))
  {
   Finish();
   return;
  }
  m_Save.PatientId = persistence.GetId(m_Target);
  m_Save.SourceId = persistence.GetId(m_Kit);
  m_Save.Volume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Save.Cells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  BCMM_SavedNativeSaline captured = BCMM_NativeSalineBinding.Capture(m_Target, persistence);
  m_Suite.Record(captured && captured.Valid(), "Actual original stock source exposes a valid remaining native lifetime before save");
  if (!captured)
  {
   Finish();
   return;
  }
  m_Save.LifeRemaining = captured.LifeSeconds;
  if (effect)
  {
   m_Save.EffectRemaining = effect.GetMaxDuration() - effect.GetCurrentDuration();
  }
  PrintFormat("BCMM_STOCK_SALINE_SAVE volume=%1 cells=%2 remaining=%3", m_Save.Volume, m_Save.Cells, m_Save.EffectRemaining);
  SaveGameManager manager = GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED, "BCMM native stock saline", ESaveGameRequestFlags.BLOCKING, new SaveGameOperationCallback(OnSaved));
 }
 protected void OnSaved(bool success, Managed context = null)
 {
  m_Suite.Record(success, "Actual SaveGameManager commits native stock saline world");
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(), new SaveGameObtainCallback(OnSaves));
 }
 protected void OnSaves(bool success, array<SaveGame> saves, Managed context = null)
 {
  SaveGame selected;
  if (success && saves)
  {
   foreach (SaveGame save : saves)
   {
    if (save.GetSavePointName() == "BCMM native stock saline" && (!selected || save.GetSavePointCreatedUnix() >= selected.GetSavePointCreatedUnix()))
    {
     selected = save;
    }
   }
  }
  m_Suite.Record(selected != null, "Original native stock saline world save is discoverable");
  if (selected)
  {
   m_Save.SaveId = selected.GetId();
   m_Suite.Record(m_Save.PackToFile("$profile:BCMM_StockSalineWorld.json"), "Native stock saline manifest preserves original UUIDs, volume and actual remaining effect time");
  }
  m_Suite.Complete();
 }
}
class BCMM_NativeSalineWorldRestartChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_StockSalineWorldManifest m_Save = new BCMM_StockSalineWorldManifest();
 protected float m_ResumedVolume;
 protected float m_ResumedCells;
 override protected void Prepare()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_StockSalineWorld.json") && persistence && persistence.WasDataLoaded(), "Separate native process loads actual stock saline world save");
  if (!persistence || !persistence.WasDataLoaded())
  {
   Finish();
   return;
  }
  m_Target = IEntity.Cast(persistence.FindById(m_Save.PatientId));
  m_Kit = IEntity.Cast(persistence.FindById(m_Save.SourceId));
  if (m_Target)
  {
   m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  SCR_SalineStorageComponent storage;
  array<IEntity> attached = {};
  if (m_Target)
  {
   storage = SCR_SalineStorageComponent.Cast(m_Target.FindComponent(SCR_SalineStorageComponent));
  }
  if (storage)
  {
   storage.GetAll(attached);
  }
  SCR_SalineDamageEffect effect;
  if (m_Medical)
  {
   effect = SCR_SalineDamageEffect.Cast(m_Medical.FindDamageEffectOfType(SCR_SalineDamageEffect));
  }
  PrintFormat("BCMM_STOCK_SALINE_RESTORE patient=%1 source=%2 attached=%3 effect=%4", m_Target != null, m_Kit != null, attached.Contains(m_Kit), effect != null);
  m_Suite.Record(m_Target && m_Kit && attached.Contains(m_Kit), "Restored native patient slot contains the exact original stock saline source");
  m_Suite.Record((m_Save.Active && effect && effect.IsActive()) || (!m_Save.Active && !effect), "Saved active/inactive native saline delivery state is preserved without restart");
  if (!m_Medical || (m_Save.Active && !effect))
  {
   m_Suite.Complete();
   return;
  }
  float remaining;
  if (effect)
  {
   remaining = effect.GetMaxDuration() - effect.GetCurrentDuration();
  }
  float runtime = GetGame().GetWorld().GetWorldTime() * 0.001;
  m_Suite.Record((!m_Save.Active && remaining == 0) || (m_Save.Active && remaining <= m_Save.EffectRemaining + 0.1 && remaining >= m_Save.EffectRemaining - runtime - 1), "Native restart neither refills effect time nor consumes offline duration");
  m_ResumedVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_ResumedCells = m_Medical.BCMM_GetPatient().Fluids.RedCellReserve;
  m_Suite.Record(Math.AbsFloat(m_ResumedCells - m_Save.Cells) < 0.001 && m_ResumedVolume >= m_Save.Volume - 0.001, "Saved native stock saline retains clinical red cells and native volume");
  GetGame().GetCallqueue().CallLater(CheckResumedFlow, 3000);
 }
 protected void CheckResumedFlow()
 {
  m_Suite.Record(((m_Save.Active && m_Medical.GetBloodHitZone().GetHealthScaled() > m_ResumedVolume) || (!m_Save.Active && m_Medical.GetBloodHitZone().GetHealthScaled() <= m_ResumedVolume + 0.0001 && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect))) && Math.AbsFloat(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve - m_ResumedCells) < 0.001, "Restored actual stock saline continues typed native delivery without creating red cells");
  PrintFormat("BCMM_STOCK_SALINE_WAIT_EXPIRY remaining=%1", m_Save.EffectRemaining);
  GetGame().GetCallqueue().CallLater(CheckExpiry, (m_Save.LifeRemaining + 3) * 1000);
 }
 protected void CheckExpiry()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  IEntity original = IEntity.Cast(persistence.FindById(m_Save.SourceId));
  PrintFormat("BCMM_STOCK_SALINE_EXPIRED source=%1 held=%2 effect=%3", original != null, m_Kit != null, m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect));
  m_Suite.Record(!original && !m_Kit && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), "Actual restored stock lifetime expires, removes exact original source and leaves no native delivery effect");
  m_ResumedVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  GetGame().GetCallqueue().CallLater(CheckAfterExpiry, 1200);
 }
 protected void CheckAfterExpiry()
 {
  m_Suite.Record(m_Medical.GetBloodHitZone().GetHealthScaled() <= m_ResumedVolume + 0.0001 && Math.AbsFloat(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve - m_ResumedCells) < 0.001, "Expired original stock source cannot continue delivery or invent red cells");
  m_Suite.Complete();
 }
}

