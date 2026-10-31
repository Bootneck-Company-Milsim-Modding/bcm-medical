// Adapter acceptance with real native entities and test-only trusted identities.
// Production identity binding remains exclusively in native audit success.
class BCMM_LifeRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected SCR_BaseGameMode m_Mode;
 protected IEntity m_First;
 protected IEntity m_Second;
 protected int m_FirstToken;
 protected IEntity m_Rejected;
 protected IEntity m_ResetCandidate;
 void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  m_Mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Mode.BCMM_TestLifeIdentity(90001, "BCMM_TEST_ACCOUNT_A");
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = "100 2 0";
  m_First = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  spawn.Transform[3] = "103 2 0";
  m_Second = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  GetGame().GetCallqueue().CallLater(Check, 1000);
 }
 protected void Check()
 {
  if (!m_First || !m_Second) { m_Suite.Record(false, "Native life fixture entities spawn"); Finish(); return; }
  BCMM_LifeAccount account = m_Mode.BCMM_AccountFor(90001);
  int total = m_Mode.BCMM_ReadSetting("totalLives");
  m_Suite.Record(!m_Mode.BCMM_LifeCanDeploy(99999), "Enabled limiter rejects unaudited identity");
  bool queries = true;
  for (int i = 0; i < 20; i++) queries = queries && m_Mode.BCMM_LifeCanDeploy(90001);
  m_Suite.Record(queries && account.Ledger.RemainingDeployments(total) == total, "Repeated preflight permission queries do not reserve or charge lives");
  m_Suite.Record(m_Mode.BCMM_ReserveLife(90001, m_First), "Native entity preparation reserves one deployment");
  m_Suite.Record(m_Mode.BCMM_ReserveLife(90001, m_First) && !m_Mode.BCMM_ReserveLife(90001, m_Second), "Preparation is idempotent for exact entity and rejects competing body");
  m_Suite.Record(!m_Mode.BCMM_LifeCanDeploy(90001), "Pending reservation blocks concurrent deployment");
  m_Mode.BCMM_CancelLife(90001);
  m_Suite.Record(account.Ledger.RemainingDeployments(total) == total && m_Mode.BCMM_LifeCanDeploy(90001), "Failed spawn cancels reservation without charging");
  m_Mode.BCMM_ReserveLife(90001, m_First);
  m_Suite.Record(!m_Mode.BCMM_CommitLife(90001, m_Second), "Finalization cannot commit a different entity");
  m_Suite.Record(m_Mode.BCMM_CommitLife(90001, m_First), "Exact successful finalization commits deployment");
  m_Mode.BCMM_CommitLife(90001, m_First);
  m_Suite.Record(account.Ledger.RemainingDeployments(total) == total - 1, "Duplicate native finalization charges only once");
  m_Mode.BCMM_TestLifeIdentity(90002, "BCMM_TEST_ACCOUNT_A");
  m_Suite.Record(m_Mode.BCMM_AccountFor(90002) == account && m_Mode.BCMM_LifeCanDeploy(90002, m_First), "Reconnected identity resumes same live body and balance");
  m_Suite.Record(!m_Mode.BCMM_ReserveLife(90002, m_Second), "Reconnect cannot bypass active body using another casualty");
  m_Suite.Record(!m_Mode.BCMM_ConfirmLifeDeath(90001, m_First), "Life accounting requires actual native dead state");
  SCR_CharacterDamageManagerComponent.Cast(m_First.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_TestKill();
  GetGame().GetCallqueue().CallLater(CheckDead, 250);
 }
 protected void CheckDead()
 {
  BCMM_LifeAccount account=m_Mode.BCMM_AccountFor(90001);
  int total=m_Mode.BCMM_ReadSetting("totalLives");
  m_Suite.Record(!account.Ledger.IsAlive() && !m_Mode.BCMM_ConfirmLifeDeath(90001,m_First),"Native death is accepted exactly once for registered Limited Lives deployment");
  m_FirstToken=m_Mode.BCMM_RegisterRemains(m_First);
  m_Suite.Record(m_FirstToken>0 && m_First && m_Mode.BCMM_LifeCanDeploy(90002),"A recoverable corpse never blocks deployment while Limited Lives allowance remains");
  for(int remaining=0;remaining<total-1;remaining++)account.Ledger.AdjustAllowance(-1);
  m_Suite.Record(!m_Mode.BCMM_LifeCanDeploy(90002) && !m_Mode.BCMM_ReserveLife(90002,m_Second),"Exhausted Limited Lives allowance still blocks preparation");
  m_Suite.Record(!m_Mode.BCMM_AdjustLives(null,90002,1,false) && !m_Mode.BCMM_ResetAllLives(null),"Unauthenticated Limited Lives grants and resets remain rejected");
  account.Ledger.ResetAllowance();
  m_Suite.Record(m_First && m_Mode.BCMM_LifeCanDeploy(90002) && m_Mode.BCMM_ReserveLife(90002,m_Second) && m_Mode.BCMM_CommitLife(90002,m_Second),"Limited Lives reset permits a new deployment while the original corpse still exists");
  SCR_EntityHelper.DeleteEntityAndChildren(m_First);
  GetGame().GetCallqueue().CallLater(CheckReleased,250);
 }
 protected void CheckReleased()
 {
  BCMM_LifeAccount account=m_Mode.BCMM_AccountFor(90002);
  BCMM_RemainsRecord record=m_Mode.BCMM_RemainsByToken(m_FirstToken);
  m_Suite.Record(record && record.Disposition==BCMM_RemainsDisposition.ADMIN_RELEASED && account.Ledger.IsAlive() && account.ActiveEntity==m_Second,"Deleting old physical remains cannot change the current Limited Lives deployment");
  m_Suite.Record(account.Ledger.RemainingDeployments(m_Mode.BCMM_ReadSetting("totalLives"))==m_Mode.BCMM_ReadSetting("totalLives")-1,"Body-bag disposal neither charges nor refunds Limited Lives");
  EntitySpawnParams rejectedSpawn=new EntitySpawnParams();rejectedSpawn.TransformMode=ETransformMode.WORLD;rejectedSpawn.Transform[3]="106 2 0";
  m_Rejected=GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"),GetGame().GetWorld(),rejectedSpawn);
  m_Suite.Record(m_Rejected!=null,"native rejected-fresh-body fixture spawns independently of registered active life");
  m_Mode.BCMM_DiscardRejectedDeployment(m_Rejected);
  m_Mode.BCMM_DiscardRejectedDeployment(m_Second);
  GetGame().GetCallqueue().CallLater(CheckRejected,250);
 }
 protected void CheckRejected()
 {
  BCMM_LifeAccount account=m_Mode.BCMM_AccountFor(90002);
  m_Suite.Record(!m_Rejected && m_Second && account.Ledger.IsAlive(),"deferred rejected-body cleanup removes unauthorized fresh body and preserves registered active deployment");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Second);
  GetGame().GetCallqueue().CallLater(CheckDeleted, 250);
 }
 protected void CheckDeleted()
 {
  BCMM_LifeAccount account=m_Mode.BCMM_AccountFor(90002);
  m_Suite.Record(!account.Ledger.IsAlive(),"Native controllable deletion retires missing active body without inventing death");
  m_Mode.BCMM_TestLifeIdentity(90003,"BCMM_TEST_PENDING_RESET");
  EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;spawn.Transform[3]="107 2 0";
  m_ResetCandidate=GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"),GetGame().GetWorld(),spawn);
  BCMM_LifeAccount pending=m_Mode.BCMM_AccountFor(90003);int total=m_Mode.BCMM_ReadSetting("totalLives");
  m_Suite.Record(m_ResetCandidate && m_Mode.BCMM_ReserveLife(90003,m_ResetCandidate) && pending.Reservation>0 && pending.PendingEntity==m_ResetCandidate,"Native reset case has one prepared body and an uncommitted reservation");
  int oldToken=pending.Reservation;BCMM_RemainsRecord remains=m_Mode.BCMM_RemainsByToken(m_FirstToken);
  m_Mode.BCMM_TestResetAccountCore(90003);
  m_Suite.Record(pending.Reservation==0 && !pending.PendingEntity && !pending.Ledger.IsAlive() && pending.Ledger.RemainingDeployments(total)==total,"Reset clears account preparation fields and ledger token without charging an uncommitted life");
  m_Suite.Record(!m_Mode.BCMM_CommitLife(90003,m_ResetCandidate) && pending.Ledger.RemainingDeployments(total)==total,"Stale finalization after reset cannot commit or spend cancelled preparation");
  m_Suite.Record(m_Mode.BCMM_ReserveLife(90003,m_ResetCandidate) && pending.Reservation!=oldToken && m_Mode.BCMM_CommitLife(90003,m_ResetCandidate),"Fresh native preparation/finalization succeeds after reset with a new reservation");
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_ResetCandidate.FindComponent(SCR_CharacterDamageManagerComponent));medical.GetBloodHitZone().SetHealth(medical.GetBloodHitZone().GetMaxHealth()*0.7);int currentLife=pending.Ledger.CurrentLife();
  m_Mode.BCMM_TestResetAccountCore(90003);
  m_Suite.Record(pending.ActiveEntity==m_ResetCandidate && pending.Ledger.IsAlive() && pending.Ledger.CurrentLife()==currentLife && pending.Ledger.RemainingDeployments(total)==total-1 && Math.AbsFloat(medical.GetBloodHitZone().GetHealthScaled()-0.7)<0.001,"Reset counts current life and preserves exact body identity and native partial blood");
  m_Suite.Record(m_Mode.BCMM_RemainsByToken(m_FirstToken)==remains && remains.Disposition==BCMM_RemainsDisposition.ADMIN_RELEASED,"Allowance administration preserves the independent remains record");
  m_Suite.Record(!m_Mode.BCMM_AdjustSelectedPlayerLives(null,m_ResetCandidate,1,false),"Selected-player administration rejects absent authenticated native editor context");
  Finish();
 }
 protected void Finish()
 {
  if (m_First) SCR_EntityHelper.DeleteEntityAndChildren(m_First);
  if (m_Second) SCR_EntityHelper.DeleteEntityAndChildren(m_Second);
  if (m_Rejected) SCR_EntityHelper.DeleteEntityAndChildren(m_Rejected);
  if (m_ResetCandidate) SCR_EntityHelper.DeleteEntityAndChildren(m_ResetCandidate);
  m_Suite.Complete();
 }
}
modded class SCR_BaseGameMode
{
 // Direct core seam; separate from real native GM permission acceptance.
 void BCMM_TestResetAccountCore(int playerId) { BCMM_ResetAccountAllowance(BCMM_AccountFor(playerId)); }
 void BCMM_TestLifeDisconnect(int playerId) { if(Replication.IsServer())m_BCMMLifeIdentity.Remove(playerId); }
 void BCMM_TestLifeIdentity(int playerId, string identity)
 {
  if (Replication.IsServer()) m_BCMMLifeIdentity.Set(playerId, identity);
 }
}

