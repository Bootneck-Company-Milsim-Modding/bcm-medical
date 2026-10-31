// Original BCM native Limited Lives persistence.
// Native campaign save state for the existing Limited Lives authority.
class BCMM_SavedLifeAccount
{
 string AccountKey;
 ref array<int> Ledger;
 UUID ActiveEntityId;
}
modded class BCMM_LifeLedger
{
 array<int> BCMM_CaptureLedger()
 {
  int alive;if(m_Alive)alive=1;
  return {1,m_Deployments,m_Adjustment,m_Sequence,m_CurrentLife,alive};
 }
 bool BCMM_RestoreLedger(notnull array<int> state)
 {
  if(state.Count()!=6 || state[0]!=1)return false;
  if(state[1]<0 || state[1]>1000000000 || state[2]<-1000000 || state[2]>1000000)return false;
  if(state[3]<state[1] || state[3]>1000000000 || state[4]<0 || state[4]>state[3])return false;
  if(state[5]!=0 && state[5]!=1)return false;
  if(state[1]>0 && state[4]==0)return false;
  if(state[5]==1 && (state[1]<1 || state[4]<1))return false;
  m_Deployments=state[1];m_Adjustment=state[2];m_Sequence=state[3];m_CurrentLife=state[4];m_Alive=state[5]==1;
  // An interrupted spawn reservation is never a committed paid deployment.
  m_Reservation=0;return true;
 }
}
modded class BCMM_LifeAccount
{
 UUID PersistentActiveEntityId=UUID.NULL_UUID;
}
class BCMM_LifeIdentityFormat
{
 static bool IsCanonical(string value)
 {
  if(value.Length()!=36)return false;
  string lower=value;lower.ToLower();if(lower!=value)return false;
  for(int index=0;index<36;index++)
  {
   string character=value.Substring(index,1);
   if(index==8 || index==13 || index==18 || index==23)
   {
    if(character!="-")return false;
   }
   else if("0123456789abcdef".IndexOf(character)<0)return false;
  }
  UUID identity=value;return !identity.IsNull();
 }
}
class BCMM_SavedLifeBodyBinding
{
 SCR_BaseGameMode Mode;
 string AccountKey;
 int Life;
 UUID BodyId;
 protected static void OnAvailable(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedLifeBodyBinding binding=BCMM_SavedLifeBodyBinding.Cast(context);
  if(binding && binding.Mode && !expired)binding.Mode.BCMM_BindSavedLifeBody(binding.AccountKey,binding.Life,binding.BodyId,IEntity.Cast(instance));
 }
 void Await(PersistenceSystem persistence)
 {
  persistence.WhenAvailable(BodyId,new PersistenceWhenAvailableTask(OnAvailable,this));
 }
}
class BCMM_SavedLifeBodyRequest
{
 SCR_BaseGameMode Mode;
 string AccountKey;
 int Life;
 UUID BodyId;
 protected static void OnLoaded(EPersistenceStatusCode statusCode,Managed result,bool isLast,Managed context)
 {
  BCMM_SavedLifeBodyRequest request=BCMM_SavedLifeBodyRequest.Cast(context);
  if(request && request.Mode)request.Mode.BCMM_ApplySavedLifeBodyResult(request.AccountKey,request.Life,request.BodyId,statusCode,IEntity.Cast(result));
 }
 void Request(PersistenceSystem persistence)
 {
  PersistenceSpawnRequest request=new PersistenceSpawnRequest();request.Collection=persistence.FindCollection("Character");request.Include={BodyId};
  persistence.RequestSpawn(request,new PersistenceResultCallback(OnLoaded,this));
 }
}
modded class SCR_BaseGameMode
{
 protected bool m_BCMMLifeLoadFault;
 void BCMM_BindSavedLifeBody(string identity,int life,UUID expected,IEntity entity)
 {
  if(!Replication.IsServer() || !entity)return;
  BCMM_LifeAccount account=m_BCMMLifeAccounts.Get(identity);PersistenceSystem persistence=PersistenceSystem.GetInstance();
  if(!account || !persistence || !account.Ledger.IsAlive() || account.Ledger.CurrentLife()!=life || account.PersistentActiveEntityId!=expected || persistence.GetId(entity)!=expected)return;
  account.ActiveEntity=entity;
  BCMM_ConfirmDetachedLifeDeath(entity);
 }
 // Called only after the native audit has established the account binding.
 override void OnPlayerAuditSuccess(int iPlayerID)
 {
  super.OnPlayerAuditSuccess(iPlayerID);
  BCMM_ResolveSavedLifeBody(iPlayerID);
 }
 void BCMM_ResolveSavedLifeBody(int playerId)
 {
  if(!Replication.IsServer())return;
  BCMM_LifeAccount account=BCMM_AccountFor(playerId);PersistenceSystem persistence=PersistenceSystem.GetInstance();
  if(!account || !persistence || persistence.GetState()!=EPersistenceSystemState.ACTIVE || !account.Ledger.IsAlive() || account.ActiveEntity || account.PersistentActiveEntityId.IsNull())return;
  BCMM_SavedLifeBodyRequest request=new BCMM_SavedLifeBodyRequest();request.Mode=this;request.AccountKey=m_BCMMLifeIdentity.Get(playerId);request.Life=account.Ledger.CurrentLife();request.BodyId=account.PersistentActiveEntityId;request.Request(persistence);
 }
 void BCMM_ApplySavedLifeBodyResult(string identity,int life,UUID expected,EPersistenceStatusCode status,IEntity entity)
 {
  if(!Replication.IsServer())return;
  BCMM_LifeAccount account=m_BCMMLifeAccounts.Get(identity);
  if(!account || !account.Ledger.IsAlive() || account.Ledger.CurrentLife()!=life || account.PersistentActiveEntityId!=expected)return;
  if(status==EPersistenceStatusCode.OK && entity)
  {
   BCMM_BindSavedLifeBody(identity,life,expected,entity);
   ChimeraCharacter character=ChimeraCharacter.Cast(entity);
   if(account.ActiveEntity==entity && character && character.GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD)
   {
    account.Ledger.ConfirmDeath(life);account.PersistentActiveEntityId=UUID.NULL_UUID;
   }
  }
  else if(status==EPersistenceStatusCode.NOT_FOUND && !account.ActiveEntity)
  {
   // Missing native storage is authoritative. Retire the pointer, never refund
   // the committed deployment or invent a body-disposition respawn condition.
   account.Ledger.RetireDeletedDeployment(life);account.PersistentActiveEntityId=UUID.NULL_UUID;
   Print("BCMM Limited Lives retired a missing saved body without refunding its deployment",LogLevel.WARNING);
  }
  // Transient storage errors preserve the ledger; they never reset balances.
  BCMM_PublishAllLifeStates();
 }
 bool BCMM_CaptureLifeAccounts(PersistenceSystem persistence,out array<ref BCMM_SavedLifeAccount> records)
 {
  records={};
  if(!Replication.IsServer() || !persistence || m_BCMMLifeLoadFault || m_BCMMLifeAccounts.Count()>100000)return false;
  foreach(string identity,BCMM_LifeAccount account:m_BCMMLifeAccounts)
  {
   if(!BCMM_LifeIdentityFormat.IsCanonical(identity) || !account || !account.Ledger)return false;
   BCMM_SavedLifeAccount record=new BCMM_SavedLifeAccount();record.AccountKey=identity;record.Ledger=account.Ledger.BCMM_CaptureLedger();record.ActiveEntityId=UUID.NULL_UUID;
   BCMM_LifeLedger validate=new BCMM_LifeLedger();if(!validate.BCMM_RestoreLedger(record.Ledger))return false;
   if(account.Ledger.IsAlive())
   {
    record.ActiveEntityId=account.PersistentActiveEntityId;
    if(account.ActiveEntity)record.ActiveEntityId=persistence.GetId(account.ActiveEntity);
    if(!BCMM_LifeIdentityFormat.IsCanonical(record.ActiveEntityId))return false;
   }
   records.Insert(record);
  }
  return true;
 }
 bool BCMM_RestoreLifeAccounts(PersistenceSystem persistence,notnull array<ref BCMM_SavedLifeAccount> records)
 {
  if(!Replication.IsServer() || !persistence || records.Count()>100000)return false;
  map<string,ref BCMM_LifeAccount> candidate=new map<string,ref BCMM_LifeAccount>();
  foreach(BCMM_SavedLifeAccount record:records)
  {
   if(!record || !BCMM_LifeIdentityFormat.IsCanonical(record.AccountKey) || !record.Ledger || candidate.Contains(record.AccountKey))return false;
   BCMM_LifeAccount account=new BCMM_LifeAccount();
   if(!account.Ledger.BCMM_RestoreLedger(record.Ledger))return false;
   if(account.Ledger.IsAlive())
   {
    if(!BCMM_LifeIdentityFormat.IsCanonical(record.ActiveEntityId))return false;
    account.PersistentActiveEntityId=record.ActiveEntityId;
    account.ActiveEntity=IEntity.Cast(persistence.FindById(record.ActiveEntityId));
   }
   else if(!record.ActiveEntityId.IsNull())return false;
   candidate.Set(record.AccountKey,account);
  }
  // All identities, counters and body links validate before replacing live state.
  m_BCMMLifeAccounts=candidate;m_BCMMLifeLoadFault=false;
  foreach(string identity,BCMM_LifeAccount account:m_BCMMLifeAccounts)
  {
   if(!account.Ledger.IsAlive() || account.ActiveEntity || account.PersistentActiveEntityId.IsNull())continue;
   BCMM_SavedLifeBodyBinding binding=new BCMM_SavedLifeBodyBinding();binding.Mode=this;binding.AccountKey=identity;binding.Life=account.Ledger.CurrentLife();binding.BodyId=account.PersistentActiveEntityId;binding.Await(persistence);
  }
  BCMM_PublishAllLifeStates();return true;
 }
 void BCMM_LifePersistenceFault()
 {
  if(!Replication.IsServer())return;
  m_BCMMLifeLoadFault=true;BCMM_PublishAllLifeStates();
  Print("BCMM Limited Lives save state rejected; deployment remains locked while the limiter is enabled",LogLevel.ERROR);
 }
 override void BCMM_SendLifeStatus(int playerId)
 {
  if(Replication.IsServer() && m_BCMMLifeLoadFault && BCMM_ReadSetting("livesEnabled")>0.5)
  {
   SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
   if(player)player.BCMM_SendLifeStatus(0,false,true);
   return;
  }
  super.BCMM_SendLifeStatus(playerId);
 }
 override bool BCMM_LifeCanDeploy(int playerId,IEntity resume=null)
 {
  if(m_BCMMLifeLoadFault && BCMM_ReadSetting("livesEnabled")>0.5)return false;
  return super.BCMM_LifeCanDeploy(playerId,resume);
 }
 override bool BCMM_ReserveLife(int playerId,IEntity entity)
 {
  if(m_BCMMLifeLoadFault && BCMM_ReadSetting("livesEnabled")>0.5)return false;
  return super.BCMM_ReserveLife(playerId,entity);
 }
 override bool BCMM_CommitLife(int playerId,IEntity entity)
 {
  if(m_BCMMLifeLoadFault && BCMM_ReadSetting("livesEnabled")>0.5)return false;
  return super.BCMM_CommitLife(playerId,entity);
 }
 override bool BCMM_ResetAllLives(SCR_AttributesManagerEditorComponent manager)
 {
  if(!super.BCMM_ResetAllLives(manager))return false;
  m_BCMMLifeLoadFault=false;BCMM_PublishAllLifeStates();return true;
 }
 protected override void OnControllableDestroyed(IEntity entity,IEntity killerEntity,notnull Instigator instigator)
 {
  super.OnControllableDestroyed(entity,killerEntity,instigator);
  if(Replication.IsServer())GetGame().GetCallqueue().CallLater(BCMM_ConfirmDetachedLifeDeath,100,false,entity);
 }
 void BCMM_ConfirmDetachedLifeDeath(IEntity entity)
 {
  if(!Replication.IsServer() || !entity)return;
  ChimeraCharacter character=ChimeraCharacter.Cast(entity);
  if(!character || character.GetCharacterController().GetLifeState()!=ECharacterLifeState.DEAD)return;
  bool changed;
  foreach(string identity,BCMM_LifeAccount account:m_BCMMLifeAccounts)
  {
   if(!account.Ledger.IsAlive() || !BCMM_LifeEntityMatches(account,entity))continue;
   int life=account.Ledger.CurrentLife();
   if(!account.Ledger.ConfirmDeath(life))continue;
   account.PersistentActiveEntityId=UUID.NULL_UUID;changed=true;
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(this);
   BCMM_RemainsRecord remains=mode.BCMM_RemainsByToken(mode.BCMM_RegisterRemains(entity));
   if(remains){remains.PlayerIdentity=identity;remains.Life=life;}
  }
  if(changed)BCMM_PublishAllLifeStates();
 }
 override void OnControllableDeleted(IEntity entity)
 {
  if(Replication.IsServer())
  {
   foreach(string identity,BCMM_LifeAccount account:m_BCMMLifeAccounts)
   {
    if(account.Ledger.IsAlive() && BCMM_LifeEntityMatches(account,entity))
    {
     account.Ledger.RetireDeletedDeployment(account.Ledger.CurrentLife());account.ActiveEntity=null;account.PersistentActiveEntityId=UUID.NULL_UUID;
    }
   }
  }
  super.OnControllableDeleted(entity);
 }
}
modded class SCR_GameModeSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity entity,notnull SaveContext context)
 {
  ESerializeResult baseResult=super.Serialize(entity,context);if(baseResult==ESerializeResult.ERROR)return baseResult;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(entity);array<ref BCMM_SavedLifeAccount> records;
  if(!mode.BCMM_CaptureLifeAccounts(GetSystem(),records))return ESerializeResult.ERROR;
  if(!context.StartObject("bcmmLimitedLives"))return ESerializeResult.ERROR;
  bool saved=context.WriteValue("schema",1) && context.WriteValue("accounts",records);
  if(!context.EndObject() || !saved)return ESerializeResult.ERROR;
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity entity,notnull LoadContext context)
 {
  if(!super.Deserialize(entity,context))return false;
  if(!context.DoesObjectExist("bcmmLimitedLives"))return true;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(entity);
  if(!context.StartObject("bcmmLimitedLives")){mode.BCMM_LifePersistenceFault();return false;}
  int schema;array<ref BCMM_SavedLifeAccount> records;
  bool read=context.ReadValue("schema",schema) && context.ReadValue("accounts",records);
  bool ended=context.EndObject();
  if(!read || !ended || schema!=1 || !records || !mode.BCMM_RestoreLifeAccounts(GetSystem(),records))
  {
   mode.BCMM_LifePersistenceFault();return false;
  }
  return true;
 }
}