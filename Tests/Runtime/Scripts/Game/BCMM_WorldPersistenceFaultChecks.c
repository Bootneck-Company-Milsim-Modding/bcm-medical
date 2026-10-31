// Native-manager malformed and unresolved physical-reference regression.
class BCMM_WorldPersistenceFaultChecks
{
 protected static BCMM_SavedRemainsRegistry CopyRegistry(BCMM_SavedRemainsRegistry source)
 {
  BCMM_SavedRemainsRegistry copy=new BCMM_SavedRemainsRegistry();copy.NextToken=source.NextToken;
  foreach(BCMM_SavedRemainsRecord record:source.Records)copy.Records.Insert(record);
  return copy;
 }
 static void Run(BCMM_DomainChecks suite,PersistenceSystem persistence,SCR_BaseGameMode mode)
 {
  array<ref BCMM_SavedLifeAccount> livesBefore;
  suite.Record(mode.BCMM_CaptureLifeAccounts(persistence,livesBefore),"Physical-reference fault fixture captures the independent paid-life ledger");
  BCMM_SavedRemainsRegistry original=mode.BCMM_CaptureSavedRemains(persistence);
  suite.Record(original!=null,"Native world registry has a valid baseline before malformed-reference checks");if(!original)return;
  BCMM_RemainsRecord expected;if(original.Records.Count()>0)expected=mode.BCMM_RemainsByToken(original.Records[0].Token);
  BCMM_SavedRemainsRegistry bad=CopyRegistry(original);bad.Schema=2;
  suite.Record(!mode.BCMM_RestoreSavedRemains(persistence,bad),"Future physical registry schema rejects before replacing valid records");
  if(original.Records.Count()>0)
  {
   bad=CopyRegistry(original);bad.Records.Insert(original.Records[0]);
   suite.Record(!mode.BCMM_RestoreSavedRemains(persistence,bad) && mode.BCMM_RemainsByToken(original.Records[0].Token)==expected,"Duplicate saved remains token rejects atomically without touching the live registry");
  }
  bad=CopyRegistry(original);BCMM_SavedRemainsRecord missing=new BCMM_SavedRemainsRecord();missing.Token=++bad.NextToken;missing.Disposition=BCMM_RemainsDisposition.AWAITING_DISPOSITION;
  bad.Records.Insert(missing);
  suite.Record(!mode.BCMM_RestoreSavedRemains(persistence,bad),"Awaiting saved remains without a physical persistent UUID rejects atomically");
  missing.DeathAge=-1;missing.Disposition=BCMM_RemainsDisposition.BURIED;
  suite.Record(!mode.BCMM_RestoreSavedRemains(persistence,bad),"Negative persisted death age rejects without replacing the valid registry");
  missing.DeathAge=0;missing.Disposition=BCMM_RemainsDisposition.AWAITING_DISPOSITION;
  missing.PhysicalId="11111111-2222-3333-4444-555555555555";
  suite.Record(!persistence.FindById(missing.PhysicalId),"Fault case uses a genuinely unavailable native physical UUID");
  suite.Record(mode.BCMM_RestoreSavedRemains(persistence,bad),"Canonical unavailable physical UUID may await native restoration without inventing an entity");
  BCMM_SavedRemainsRegistry unresolved=mode.BCMM_CaptureSavedRemains(persistence);
  suite.Record(unresolved!=null,"Unresolved saved physical reference cannot poison the next native registry capture");
  suite.Record(!persistence.FindById(missing.PhysicalId),"Capturing an unresolved reference never recreates missing recovery equipment");
  suite.Record(mode.BCMM_RestoreSavedRemains(persistence,original),"Fault fixture reinstates original native physical registry after its isolated checks");
  array<ref BCMM_SavedLifeAccount> livesAfter;bool unchanged=mode.BCMM_CaptureLifeAccounts(persistence,livesAfter) && livesBefore && livesAfter && livesBefore.Count()==livesAfter.Count();
  if(unchanged)
  {
   foreach(BCMM_SavedLifeAccount before:livesBefore)
   {
    BCMM_SavedLifeAccount match;
    foreach(BCMM_SavedLifeAccount after:livesAfter)if(after.AccountKey==before.AccountKey)match=after;
    if(!match || match.ActiveEntityId!=before.ActiveEntityId || match.Ledger.Count()!=before.Ledger.Count()){unchanged=false;continue;}
    for(int index=0;index<before.Ledger.Count();index++)if(match.Ledger[index]!=before.Ledger[index])unchanged=false;
   }
  }
  suite.Record(unchanged,"Missing/corrupt physical remains never spend, refund, reset or gate any independent paid life");
 }
}

class BCMM_WorldPersistenceFaultRuntimeChecks : BCMM_FinitePersistenceRuntimeChecks
{
 override protected void Prepare()
 {
  m_Persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(m_Persistence && m_Persistence.WasDataLoaded(),"Physical-reference fault fixture uses a real native saved-world restart");
  if(m_Persistence && m_Persistence.WasDataLoaded())BCMM_WorldPersistenceFaultChecks.Run(m_Suite,m_Persistence,SCR_BaseGameMode.Cast(GetGame().GetGameMode()));
  Finish();
 }
}
