// Session-scoped authority records survive physical corpse/bag disposal.
enum BCMM_RemainsDisposition { AWAITING_DISPOSITION, BURIED, SENT_HOME, TIMED_OUT, ADMIN_RELEASED, VEHICLE_LOST }
class BCMM_RemainsRecord
{
 int Token;
 string PlayerIdentity;
 int Life;
 string DeceasedName;
 string DeathFactionKey;
 float DeathSeconds;
 IEntity Physical;
 IEntity OriginalCorpse;
 RplId OriginalId=RplId.Invalid();
 IEntity StoredVehicle;
 bool DeletionPending;
 BCMM_RemainsDisposition Disposition;
}
modded class SCR_BaseGameMode
{
 protected int m_BCMMNextRemainsToken;
 protected ref map<int,ref BCMM_RemainsRecord> m_BCMMRemainsRecords=new map<int,ref BCMM_RemainsRecord>();
 BCMM_RemainsRecord BCMM_RemainsByToken(int token) { return m_BCMMRemainsRecords.Get(token); }
 BCMM_RemainsRecord BCMM_RemainsForEntity(IEntity entity)
 {
  if(!entity) return null;
  foreach(int token,BCMM_RemainsRecord record:m_BCMMRemainsRecords)
   if(record.Physical==entity) return record;
  return null;
 }
 int BCMM_RegisterRemains(IEntity corpse)
 {
  if(!Replication.IsServer() || !corpse) return 0;
  ChimeraCharacter character=ChimeraCharacter.Cast(corpse);
  if(!character || !character.GetCharacterController() || character.GetCharacterController().GetLifeState()!=ECharacterLifeState.DEAD) return 0;
  foreach(int previousToken,BCMM_RemainsRecord existing:m_BCMMRemainsRecords)
   if(existing.OriginalCorpse==corpse)
   {
    if(existing.OriginalId==RplId.Invalid())existing.OriginalId=BCMM_BodyBagComponent.Id(corpse);
    return previousToken;
   }
  BCMM_RemainsRecord record=new BCMM_RemainsRecord();
  record.Token=++m_BCMMNextRemainsToken;
  record.Physical=corpse;
  record.OriginalCorpse=corpse;
  record.OriginalId=BCMM_BodyBagComponent.Id(corpse);
  record.DeathSeconds=corpse.GetWorld().GetWorldTime()*0.001;
  SCR_ChimeraCharacter nativeCharacter=SCR_ChimeraCharacter.Cast(corpse);
  Faction faction;if(nativeCharacter)faction=nativeCharacter.GetFaction();
  if(faction)record.DeathFactionKey=faction.GetFactionKey();
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(corpse.FindComponent(SCR_CharacterDamageManagerComponent));
  if(medical) record.DeceasedName=medical.BCMM_DeceasedName();
  m_BCMMRemainsRecords.Set(record.Token,record);
  return record.Token;
 }
 bool BCMM_TransferRemains(IEntity corpse,IEntity bag)
 {
  if(!Replication.IsServer() || !corpse || !bag) return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if(!state || state.RemainsToken()>0) return false;
  int token=BCMM_RegisterRemains(corpse);
  BCMM_RemainsRecord record=BCMM_RemainsByToken(token);
  if(!state || !record || record.Physical!=corpse || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) return false;
  record.Physical=bag;
  record.DeceasedName=state.DeceasedName();
  state.SetRemainsToken(token);
  record.StoredVehicle=BCMM_BodyBagComponent.Resolve(state.VehicleId());
  PrintFormat("BCMM_REMAINS transfer token=%1 bagToken=%2 physical=%3",token,state.RemainsToken(),record.Physical);
  return true;
 }
 bool BCMM_CommitRemains(int token,IEntity expected,BCMM_RemainsDisposition disposition)
 {
  if(!Replication.IsServer() || disposition<BCMM_RemainsDisposition.BURIED || disposition>BCMM_RemainsDisposition.VEHICLE_LOST) return false;
  BCMM_RemainsRecord record=BCMM_RemainsByToken(token);
  if(!record || !expected || record.Physical!=expected || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) return false;
  record.Disposition=disposition;
  record.DeletionPending=false;
  PrintFormat("BCMM_REMAINS disposition token=%1 state=%2 name=%3",token,disposition,record.DeceasedName);
  return true;
 }
 void BCMM_RemainsDeleted(int token,IEntity expected)
 {
  if(!Replication.IsServer() || token<=0) return;
  BCMM_RemainsRecord record=BCMM_RemainsByToken(token);
  if(record && expected && record.Physical==expected && record.StoredVehicle && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION)
  {
   // Native deletion is child-first. Retain the immutable record until the
   // parent manager has had its own synchronous deletion callback.
   record.DeletionPending=true;
   GetGame().GetCallqueue().CallLater(BCMM_FinalizeDeletedRemains,100,false,token);
   return;
  }
  BCMM_CommitRemains(token,expected,BCMM_RemainsDisposition.ADMIN_RELEASED);
 }
 protected void BCMM_FinalizeDeletedRemains(int token)
 {
  BCMM_RemainsRecord record=BCMM_RemainsByToken(token);
  if(!record || !record.DeletionPending || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) return;
  record.DeletionPending=false;
  record.Disposition=BCMM_RemainsDisposition.ADMIN_RELEASED;
  PrintFormat("BCMM_REMAINS deleted stored bag token=%1 state=%2",token,record.Disposition);
 }
 void BCMM_UpdateStoredRemains(int token,IEntity bag,IEntity vehicle)
 {
  if(!Replication.IsServer()) return;
  BCMM_RemainsRecord record=BCMM_RemainsByToken(token);
  if(record && record.Physical==bag && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION) record.StoredVehicle=vehicle;
 }
 void BCMM_RemainsVehicleDeleted(IEntity vehicle)
 {
  if(!Replication.IsServer() || !vehicle) return;
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(vehicle.FindComponent(InventoryStorageManagerComponent));
  foreach(int token,BCMM_RemainsRecord record:m_BCMMRemainsRecords)
  {
   if(record.StoredVehicle!=vehicle || record.Disposition!=BCMM_RemainsDisposition.AWAITING_DISPOSITION) continue;
   if(!record.DeletionPending && (!record.Physical || !inventory || !inventory.Contains(record.Physical))) continue;
   record.DeletionPending=false;
   record.Disposition=BCMM_RemainsDisposition.VEHICLE_LOST;
   PrintFormat("BCMM_REMAINS vehicle deletion token=%1 state=%2",token,record.Disposition);
  }
 }
 void BCMM_DiscardRejectedDeployment(IEntity entity)
 {
  if(!Replication.IsServer() || !entity) return;
  GetGame().GetCallqueue().CallLater(BCMM_DeleteRejectedDeployment,100,false,entity);
 }
 protected void BCMM_DeleteRejectedDeployment(IEntity entity)
 {
  if(!Replication.IsServer() || !entity || entity.IsDeleted()) return;
  foreach(string identity,BCMM_LifeAccount account:m_BCMMLifeAccounts)
   if(account.ActiveEntity==entity) return;
  BCMM_RemainsRecord remains=BCMM_RemainsForEntity(entity);
  if(remains && !remains.PlayerIdentity.IsEmpty()) return;
  SCR_EntityHelper.DeleteEntityAndChildren(entity);
 }
 void ~SCR_BaseGameMode()
 {
  if(!GetGame() || !GetGame().GetCallqueue()) return;
  GetGame().GetCallqueue().Remove(BCMM_FinalizeDeletedRemains);
  GetGame().GetCallqueue().Remove(BCMM_DeleteRejectedDeployment);
 }
}
