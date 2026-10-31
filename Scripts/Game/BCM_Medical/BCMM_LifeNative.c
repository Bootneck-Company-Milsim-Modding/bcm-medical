// Original BCM authority adapter. Native preflight queries are read-only;
// preparation reserves, finalization commits, and failed responses cancel.
class BCMM_LifeAccount
{
 ref BCMM_LifeLedger Ledger = new BCMM_LifeLedger();
 IEntity ActiveEntity;
 IEntity PendingEntity;
 int Reservation;
}
modded class SCR_BaseGameMode
{
 protected ref map<string, ref BCMM_LifeAccount> m_BCMMLifeAccounts = new map<string, ref BCMM_LifeAccount>();
 protected ref map<int, string> m_BCMMLifeIdentity = new map<int, string>();
 bool BCMM_LifeEntityMatches(BCMM_LifeAccount account,IEntity entity)
 {
  if(!account || !entity)return false;
  if(account.ActiveEntity==entity)return true;
  if(account.PersistentActiveEntityId.IsNull())return false;
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  return persistence && persistence.GetId(entity)==account.PersistentActiveEntityId;
 }
 BCMM_LifeAccount BCMM_AccountFor(int playerId)
 {
  if (!Replication.IsServer() || !m_BCMMLifeIdentity.Contains(playerId)) return null;
  string identity = m_BCMMLifeIdentity.Get(playerId);
  if (identity.IsEmpty()) return null;
  BCMM_LifeAccount account = m_BCMMLifeAccounts.Get(identity);
  if (!account)
  {
   account = new BCMM_LifeAccount();
   m_BCMMLifeAccounts.Set(identity, account);
  }
  return account;
 }
 bool BCMM_LifeCanDeploy(int playerId, IEntity resume = null)
 {
  if (!Replication.IsServer()) return false;
  bool limited=BCMM_ReadSetting("livesEnabled")>0.5;
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  if (!account) return !limited;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
  if (account.Ledger.IsAlive()) return !limited || (resume && BCMM_LifeEntityMatches(account,resume));
  return account.Ledger.CanReserveDeployment(BCMM_ReadSetting("totalLives"),limited);
 }
 bool BCMM_ReserveLife(int playerId, IEntity entity)
 {
  if (!Replication.IsServer() || !entity) return false;
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  if (!account) return BCMM_ReadSetting("livesEnabled")<0.5;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
  if (account.Ledger.IsAlive())
  {
   if (BCMM_LifeEntityMatches(account,entity)) return true;
   return BCMM_ReadSetting("livesEnabled") < 0.5;
  }
  if (account.Reservation > 0) return account.PendingEntity == entity;
  int reservation = account.Ledger.ReserveDeployment(BCMM_ReadSetting("totalLives"), BCMM_ReadSetting("livesEnabled") > 0.5);
  if (reservation <= 0) return false;
  account.Reservation = reservation;
  account.PendingEntity = entity;
  return true;
 }
 bool BCMM_CommitLife(int playerId, IEntity entity)
 {
  if (!Replication.IsServer() || !entity) return false;
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  if (!account) return BCMM_ReadSetting("livesEnabled")<0.5;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
  if (account.Ledger.IsAlive())
  {
   if (BCMM_LifeEntityMatches(account,entity)) { account.ActiveEntity=entity;BCMM_SendLifeStatus(playerId);return true; }
   if (BCMM_ReadSetting("livesEnabled") >= 0.5) return false;
   account.ActiveEntity = entity;
  account.PersistentActiveEntityId=UUID.NULL_UUID;
   BCMM_SendLifeStatus(playerId);
   return true;
  }
  if (account.PendingEntity != entity || !account.Ledger.CommitDeployment(account.Reservation)) return false;
  account.ActiveEntity = entity;
  account.PersistentActiveEntityId=UUID.NULL_UUID;
  account.PendingEntity = null;
  account.Reservation = 0;
  BCMM_SendLifeStatus(playerId);
  return true;
 }
 void BCMM_CancelLife(int playerId)
 {
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  if (!account || account.Reservation <= 0) return;
  account.Ledger.CancelDeployment(account.Reservation);
  account.Reservation = 0;
  account.PendingEntity = null;
 }
 bool BCMM_ConfirmLifeDeath(int playerId, IEntity corpse)
 {
  if (!Replication.IsServer() || !corpse) return false;
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  ChimeraCharacter character = ChimeraCharacter.Cast(corpse);
  if (!account || !BCMM_LifeEntityMatches(account,corpse) || !character || character.GetCharacterController().GetLifeState() != ECharacterLifeState.DEAD) return false;
  bool accepted = account.Ledger.ConfirmDeath(account.Ledger.CurrentLife());
  if(accepted)
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
   BCMM_RemainsRecord record=mode.BCMM_RemainsByToken(mode.BCMM_RegisterRemains(corpse));
   if(record)
   {
    record.PlayerIdentity=m_BCMMLifeIdentity.Get(playerId);
    record.Life=account.Ledger.CurrentLife();
   }
  }
  if (accepted) BCMM_SendLifeStatus(playerId);
  return accepted;
 }
 void BCMM_SendLifeStatus(int playerId)
 {
  if (!Replication.IsServer() || !GetGame().GetPlayerManager()) return;
  BCMM_LifeAccount account = BCMM_AccountFor(playerId);
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
  if (!account || !player) return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
  player.BCMM_SendLifeStatus(account.Ledger.RemainingDeployments(BCMM_ReadSetting("totalLives")), account.Ledger.IsAlive(), BCMM_ReadSetting("livesEnabled") > 0.5);
 }
 protected void BCMM_PublishAllLifeStates()
 {
  foreach (int playerId, string identity : m_BCMMLifeIdentity) BCMM_SendLifeStatus(playerId);
 }
 override void OnPlayerAuditSuccess(int iPlayerID)
 {
  if (Replication.IsServer())
  {
   UUID identity = SCR_PlayerIdentityUtils.GetPlayerIdentityId(iPlayerID);
   if (!identity.IsNull()) { string accountKey=identity;accountKey.ToLower();m_BCMMLifeIdentity.Set(iPlayerID,accountKey); }
  }
  // Native persistence may issue a possession spawn during this call.
  super.OnPlayerAuditSuccess(iPlayerID);
  BCMM_SendLifeStatus(iPlayerID);
 }
 override bool CanPlayerSpawn_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, out SCR_ESpawnResult result = SCR_ESpawnResult.SPAWN_NOT_ALLOWED)
 {
  if (!super.CanPlayerSpawn_S(requestComponent, handlerComponent, data, result)) return false;
  if (!requestComponent) return false;
  IEntity resume;
  SCR_PossessSpawnData possession = SCR_PossessSpawnData.Cast(data);
  if (possession) resume = BCMM_BodyBagComponent.Resolve(possession.GetRplId());
  if (BCMM_LifeCanDeploy(requestComponent.GetPlayerId(), resume)) return true;
  result = SCR_ESpawnResult.SPAWN_NOT_ALLOWED;
  BCMM_SendLifeStatus(requestComponent.GetPlayerId());
  return false;
 }
 override bool PreparePlayerEntity_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
 {
  if (!super.PreparePlayerEntity_S(requestComponent, handlerComponent, data, entity) || !requestComponent) return false;
  // This also covers native possession handlers configured to skip preflight.
  return BCMM_ReserveLife(requestComponent.GetPlayerId(), entity);
 }
 override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
 {
  if (Replication.IsServer() && requestComponent && !BCMM_CommitLife(requestComponent.GetPlayerId(), entity))
  {
   BCMM_CancelLife(requestComponent.GetPlayerId());
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
   // Possession targets predate this request and must never be destroyed by a
   // failed attempt. A newly created unauthorized body is cleaned up deferred.
   if(!SCR_PossessSpawnData.Cast(data)) mode.BCMM_DiscardRejectedDeployment(entity);
   Print("BCMM LIFE rejected finalized body without a matching permitted reservation",LogLevel.WARNING);
  }
  super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
 }
 override void OnSpawnPlayerEntityFailure_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, IEntity entity, SCR_SpawnData data, SCR_ESpawnResult reason)
 {
  if (Replication.IsServer() && requestComponent) BCMM_CancelLife(requestComponent.GetPlayerId());
  super.OnSpawnPlayerEntityFailure_S(requestComponent, handlerComponent, entity, data, reason);
 }
 protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
 {
  BCMM_ConfirmLifeDeath(playerId, playerEntity);
  super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
 }
 protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
 {
  if (Replication.IsServer()) BCMM_CancelLife(playerId);
  super.OnPlayerDisconnected(playerId, cause, timeout);
  m_BCMMLifeIdentity.Remove(playerId);
 }
 override void OnControllableDeleted(IEntity entity)
 {
  if (Replication.IsServer())
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
   BCMM_RemainsRecord remains=mode.BCMM_RemainsForEntity(entity);
   if(remains) mode.BCMM_RemainsDeleted(remains.Token,entity);
   foreach (string identity, BCMM_LifeAccount account : m_BCMMLifeAccounts)
    if (account.ActiveEntity == entity && account.Ledger.IsAlive())
    {
     account.Ledger.RetireDeletedDeployment(account.Ledger.CurrentLife());
     account.ActiveEntity = null;
    }
   BCMM_PublishAllLifeStates();
  }
  super.OnControllableDeleted(entity);
 }
 override bool BCMM_ChangeSetting(string key, float value, SCR_AttributesManagerEditorComponent manager)
 {
  bool saved = super.BCMM_ChangeSetting(key, value, manager);
  if (saved && (key == "livesEnabled" || key == "totalLives")) BCMM_PublishAllLifeStates();
  return saved;
 }
 protected void BCMM_ResetAccountAllowance(BCMM_LifeAccount account)
 {
  if(!Replication.IsServer() || !account)return;
  account.Ledger.ResetAllowance();account.PendingEntity=null;account.Reservation=0;
 }
 bool BCMM_AdjustLives(SCR_AttributesManagerEditorComponent manager, int targetPlayerId, int change, bool reset)
 {
  if (!BCMM_IsSettingsEditor(manager)) return false;
  BCMM_LifeAccount account = BCMM_AccountFor(targetPlayerId);
  if (!account) return false;
  if (reset) BCMM_ResetAccountAllowance(account);
  else if (!account.Ledger.AdjustAllowance(change)) return false;
  BCMM_SendLifeStatus(targetPlayerId);
  return true;
 }
 bool BCMM_ResetAllLives(SCR_AttributesManagerEditorComponent manager)
 {
  if (!BCMM_IsSettingsEditor(manager)) return false;
  foreach (string identity, BCMM_LifeAccount account : m_BCMMLifeAccounts) BCMM_ResetAccountAllowance(account);
  BCMM_PublishAllLifeStates();
  return true;
 }
}
modded class SCR_SpawnRequestComponent
{
 override protected void SendResponse_S(SCR_ESpawnResult response, SCR_SpawnData data)
 {
  if (Replication.IsServer() && response != SCR_ESpawnResult.OK)
  {
   SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   if (mode) mode.BCMM_CancelLife(GetPlayerId());
  }
  super.SendResponse_S(response, data);
 }
}
modded class SCR_PlayerController
{
 protected int m_BCMMLivesRemaining;
 protected bool m_BCMMLifeAlive;
 protected bool m_BCMMLimitEnabled;
 int BCMM_LivesRemaining() { return m_BCMMLivesRemaining; }
 bool BCMM_LifeSpawnBlocked() { return m_BCMMLimitEnabled && m_BCMMLivesRemaining <= 0 && !m_BCMMLifeAlive; }
 void BCMM_SendLifeStatus(int remaining, bool alive, bool enabled)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveLifeStatus(remaining, alive, enabled);
  else Rpc(BCMM_ReceiveLifeStatus, remaining, alive, enabled);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveLifeStatus(int remaining, bool alive, bool enabled)
 {
  m_BCMMLivesRemaining = remaining;
  m_BCMMLifeAlive = alive;
  m_BCMMLimitEnabled = enabled;
 }
}


