// Separate native saved-world boundaries while a physical reference stays unavailable.
class BCMM_MissingPhysicalManifest : JsonApiStruct
{
 string SaveId;
 int Token;
 string MissingId="11111111-2222-3333-4444-555555555555";
 string AccountKey="11111111-1111-4111-8111-111111111111";
 string PaidBodyId;
 ref array<int> PaidLedger={};
 void BCMM_MissingPhysicalManifest(){RegV("SaveId");RegV("Token");RegV("MissingId");RegV("AccountKey");RegV("PaidBodyId");RegV("PaidLedger");}
}
class BCMM_MissingPhysicalRestartChecks : BCMM_FinitePersistenceRuntimeChecks
{
 protected ref BCMM_MissingPhysicalManifest m_Missing=new BCMM_MissingPhysicalManifest();
 protected IEntity m_MissingCargoFloor;
 override void Start(BCMM_DomainChecks suite)
 {
  m_MissingCargoFloor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","110 0 0");super.Start(suite);
 }
 override protected void Prepare()
 {
  m_Persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(m_Persistence && m_Persistence.WasDataLoaded(),"Missing-reference save boundary starts from a real native restored world");
  if(!m_Persistence || !m_Persistence.WasDataLoaded()){Finish();return;}
  string phase;System.GetCLIParam("bcmmWorldFaultPhase",phase);
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(phase=="verify"){VerifyMissingBoundary(mode);return;}
  BCMM_WorldPersistenceFaultChecks.Run(m_Suite,m_Persistence,mode);
  array<ref BCMM_SavedLifeAccount> accounts;mode.BCMM_CaptureLifeAccounts(m_Persistence,accounts);
  foreach(BCMM_SavedLifeAccount account:accounts)if(account.AccountKey==m_Missing.AccountKey){m_Missing.PaidBodyId=account.ActiveEntityId;m_Missing.PaidLedger=account.Ledger;}
  m_Suite.Record(m_Missing.PaidLedger.Count()==6,"Missing-reference fixture retains the original independent account ledger");
  BCMM_SavedRemainsRegistry saved=mode.BCMM_CaptureSavedRemains(m_Persistence);
  BCMM_SavedRemainsRecord missing=new BCMM_SavedRemainsRecord();missing.Token=++saved.NextToken;missing.PhysicalId=m_Missing.MissingId;missing.DeceasedName="BCM unresolved physical reference";missing.Disposition=BCMM_RemainsDisposition.AWAITING_DISPOSITION;saved.Records.Insert(missing);m_Missing.Token=missing.Token;
  m_Suite.Record(mode.BCMM_RestoreSavedRemains(m_Persistence,saved),"Native registry accepts a validated unavailable reference without inventing its entity");
  GetGame().GetCallqueue().CallLater(SaveAfterReferenceTimeout,32000);
 }
 protected void SaveAfterReferenceTimeout()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(m_Missing.Token);
  m_Suite.Record(record && !record.Physical && record.PendingPhysicalId==m_Missing.MissingId && mode.BCMM_CaptureSavedRemains(m_Persistence),"Unavailable physical UUID remains saveable after native availability timeout");
  SaveGameManager manager=GetGame().GetSaveGameManager();manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM missing physical reference",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnMissingSaved));
 }
 protected void OnMissingSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Actual native world save commits with an expired unavailable physical reference");if(!success){Finish();return;}
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnMissingSaves));
 }
 protected void OnMissingSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;
  if(success && saves)foreach(SaveGame save:saves)if(save.GetSavePointName()=="BCMM missing physical reference" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))selected=save;
  m_Suite.Record(selected!=null,"Committed unavailable-reference native save point is discoverable");
  if(selected){m_Missing.SaveId=selected.GetId();m_Suite.Record(m_Missing.PackToFile("$profile:BCMM_MissingPhysicalRestart.json"),"Separate-process missing-reference manifest retains the native save UUID");}
  Finish();
 }
 protected void VerifyMissingBoundary(SCR_BaseGameMode mode)
 {
  m_Suite.Record(m_Missing.LoadFromFile("$profile:BCMM_MissingPhysicalRestart.json"),"Third process reads original unavailable-reference manifest");
  BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(m_Missing.Token);
  m_Suite.Record(record && !record.Physical && record.PendingPhysicalId==m_Missing.MissingId && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"Real native restart retains the unavailable physical UUID and disposition without recreating it");
  m_Suite.Record(!m_Persistence.FindById(m_Missing.MissingId) && mode.BCMM_CaptureSavedRemains(m_Persistence),"World after unavailable-reference restart remains saveable without resurrecting missing gear");
  array<ref BCMM_SavedLifeAccount> accounts;bool unchanged=mode.BCMM_CaptureLifeAccounts(m_Persistence,accounts);BCMM_SavedLifeAccount found;
  foreach(BCMM_SavedLifeAccount account:accounts)if(account.AccountKey==m_Missing.AccountKey)found=account;
  unchanged=unchanged && found && found.ActiveEntityId==m_Missing.PaidBodyId && found.Ledger.Count()==m_Missing.PaidLedger.Count();
  if(unchanged)for(int index=0;index<found.Ledger.Count();index++)if(found.Ledger[index]!=m_Missing.PaidLedger[index])unchanged=false;
  m_Suite.Record(unchanged,"Unavailable physical-reference save and restart never alter original paid-life identity or any ledger value");
  Finish();
 }
}
