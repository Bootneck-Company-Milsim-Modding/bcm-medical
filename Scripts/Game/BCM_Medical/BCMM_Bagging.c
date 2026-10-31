// Original BCM body-bag item marker and recovery-only container.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_EmptyBagComponentClass : ScriptComponentClass {}
class BCMM_EmptyBagComponent : ScriptComponent {}
class BCMM_BagStorageComponentClass : SCR_UniversalInventoryStorageComponentClass {}
class BCMM_BagStorageComponent : SCR_UniversalInventoryStorageComponent
{
 protected bool m_Transferring;
 void SetTransferring(bool value) { m_Transferring = Replication.IsServer() && value; }
 bool IsTransferring() { return m_Transferring; }
 override bool OnOverrideCanStoreItem() { return true; }
 override bool CanStoreItem(IEntity item, int slotID) { return m_Transferring && super.CanStoreItem(item, slotID); }
 override bool OnOverrideCanStoreResource() { return true; }
 override bool CanStoreResource(ResourceName resourceName, int slotID) { return false; }
}
class BCMM_BagTransferEntry
{
 IEntity Item;
 BaseInventoryStorageComponent Storage;
 int Slot;
 int Depth;
 bool FromWorld;
 vector WorldPosition;
 vector WorldAngles;
}
enum BCMM_BagTransferPhase { WAITING, TRANSFER, CONSUME, ROLLBACK, COMMITTING, FINISHED }
class BCMM_BaggingOperation : ScriptedInventoryOperationCallback
{
 protected static ref array<ref BCMM_BaggingOperation> s_Active = {};
 protected IEntity m_Actor;
 protected IEntity m_Corpse;
 protected IEntity m_Empty;
 protected int m_PlayerId;
 protected PlayerController m_PlayerController;
 protected IEntity m_Bag;
 protected SCR_InventoryStorageManagerComponent m_Source;
 protected SCR_InventoryStorageManagerComponent m_ActorInventory;
 protected BCMM_BagStorageComponent m_Target;
 protected InventoryStorageManagerComponent m_BagManager;
 protected BCMM_BodyBagComponent m_Identity;
 protected ref array<ref BCMM_BagTransferEntry> m_Items = {};
 protected int m_Index;
 protected int m_RestoreIndex;
 protected BCMM_BagTransferPhase m_Phase;
 protected bool m_RestoreFailed;
 static bool IsBusy(IEntity corpse)
 {
  foreach (BCMM_BaggingOperation operation : s_Active)
   if (operation.m_Corpse == corpse && operation.m_Phase != BCMM_BagTransferPhase.FINISHED) return true;
  return false;
 }
 static float Setting(string key)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) return mode.BCMM_ReadSetting(key);
  BCMM_Settings defaults = new BCMM_Settings();
  return defaults.Get(key);
 }
 static IEntity FindEmpty(IEntity actor)
 {
  if (!actor) return null;
  SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(actor.FindComponent(SCR_InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
   if (item && item.FindComponent(BCMM_EmptyBagComponent)) return item;
  return null;
 }
 static bool CanBegin(IEntity actor, IEntity corpse)
 {
  if (!actor || !corpse || actor == corpse || IsBusy(corpse)) return false;
  ChimeraCharacter user = ChimeraCharacter.Cast(actor);
  ChimeraCharacter dead = ChimeraCharacter.Cast(corpse);
  if (!user || !dead || !user.GetCharacterController() || !dead.GetCharacterController()) return false;
  if (user.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || user.GetCharacterController().IsUnconscious() || SCR_CompartmentAccessComponent.GetVehicleIn(user)) return false;
  if (dead.GetCharacterController().GetLifeState() != ECharacterLifeState.DEAD || SCR_CompartmentAccessComponent.GetVehicleIn(dead) || corpse.GetParent()) return false;
  return vector.Distance(actor.GetOrigin(), corpse.GetOrigin()) <= Setting("baggingRangeMetres") && FindEmpty(actor);
 }
 static bool Begin(IEntity actor, IEntity corpse)
 {
  if (!Replication.IsServer() || !CanBegin(actor, corpse)) return false;
  BCMM_BaggingOperation operation = new BCMM_BaggingOperation();
  operation.m_Actor = actor;
  operation.m_Corpse = corpse;
  operation.m_Empty = FindEmpty(actor);
  operation.m_PlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  if (operation.m_PlayerId > 0)
  {
   operation.m_PlayerController = GetGame().GetPlayerManager().GetPlayerController(operation.m_PlayerId);
   if (!operation.m_PlayerController || operation.m_PlayerController.GetControlledEntity() != actor) return false;
  }
  operation.m_Source = SCR_InventoryStorageManagerComponent.Cast(corpse.FindComponent(SCR_InventoryStorageManagerComponent));
  operation.m_ActorInventory = SCR_InventoryStorageManagerComponent.Cast(actor.FindComponent(SCR_InventoryStorageManagerComponent));
  if (!operation.m_Source || !operation.m_ActorInventory) return false;
  s_Active.Insert(operation);
  BCMM_RemainsProtection.ObserveCorpse(corpse);
  SCR_GarbageSystem garbage = SCR_GarbageSystem.GetByEntityWorld(corpse);
  if (garbage) garbage.UpdateBlacklist(corpse, true);
  GetGame().GetCallqueue().CallLater(operation.StartTransfer, Setting("baggingDurationSeconds") * 1000);
  return true;
 }
 protected bool StillValid()
 {
  if (!m_Actor || !m_Corpse || !m_Empty || !m_ActorInventory || !m_ActorInventory.Contains(m_Empty)) return false;
  // Bind delayed work to the original connection and controlled character.
  // A disconnect or possession change must not let the abandoned body finish it.
  PlayerManager players = GetGame().GetPlayerManager();
  if (m_PlayerId > 0)
  {
   if (!m_PlayerController || players.GetPlayerController(m_PlayerId) != m_PlayerController || m_PlayerController.GetControlledEntity() != m_Actor) return false;
  }
  else if (players.GetPlayerIdFromControlledEntity(m_Actor) > 0) return false;
  ChimeraCharacter user = ChimeraCharacter.Cast(m_Actor);
  ChimeraCharacter dead = ChimeraCharacter.Cast(m_Corpse);
  return user && dead && user.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE && !user.GetCharacterController().IsUnconscious() && !SCR_CompartmentAccessComponent.GetVehicleIn(user) && dead.GetCharacterController().GetLifeState() == ECharacterLifeState.DEAD && !SCR_CompartmentAccessComponent.GetVehicleIn(dead) && !m_Corpse.GetParent() && vector.Distance(m_Actor.GetOrigin(), m_Corpse.GetOrigin()) <= Setting("baggingRangeMetres");
 }
 protected void StartTransfer()
 {
  if (!StillValid())
  {
   Print("BCMM bagging cancelled: casualty, performer, kit or range changed before completion");
   Release();
   return;
  }
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  m_Corpse.GetWorldTransform(spawn.Transform);
  spawn.Transform[3] = m_Corpse.GetOrigin() + "0 0.15 0";
  m_Bag = GetGame().SpawnEntityPrefab(Resource.Load("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et"), GetGame().GetWorld(), spawn);
  if (!m_Bag) { Release(); return; }
  m_Target = BCMM_BagStorageComponent.Cast(m_Bag.FindComponent(BCMM_BagStorageComponent));
  m_Identity = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_BagManager = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  if (!m_Target || !m_Identity) { SCR_EntityHelper.DeleteEntityAndChildren(m_Bag); Release(); return; }
  m_Identity.SetReady(false);
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent));
  string name;
  if (medical) name = medical.BCMM_DeceasedName();
  if (name.IsEmpty())
  {
   CharacterIdentityComponent identity = CharacterIdentityComponent.Cast(m_Corpse.FindComponent(CharacterIdentityComponent));
   if (identity && identity.GetIdentity()) name = identity.GetIdentity().GetFullName();
  }
  if (name.IsEmpty()) name = "Unknown casualty";
  m_Identity.SetDeceasedName(name);
  SnapshotItems();
  m_Target.SetTransferring(true);
  m_Phase = BCMM_BagTransferPhase.TRANSFER;
  MoveNext();
 }
 protected void SnapshotItems()
 {
  array<IEntity> items = {};
  m_Source.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   bool duplicate;
   foreach (BCMM_BagTransferEntry prior : m_Items) if (prior.Item == item) duplicate = true;
   if (duplicate) continue;
   InventoryItemComponent component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
   if (!component || !component.GetParentSlot() || !component.GetParentSlot().GetStorage()) continue;
   BaseInventoryStorageComponent storage = component.GetParentSlot().GetStorage();
   // Preserve sights, magazines and special weapon slots on their root weapon.
   if (storage.GetOwner() != m_Corpse && !SCR_UniversalInventoryStorageComponent.Cast(storage)) continue;
   BCMM_BagTransferEntry entry = new BCMM_BagTransferEntry();
   entry.Item = item;
   entry.Storage = storage;
   entry.Slot = component.GetParentSlot().GetID();
   IEntity current = item;
   for (int guard = 0; current && current != m_Corpse && guard < 32; guard++)
   {
    InventoryItemComponent currentItem = InventoryItemComponent.Cast(current.FindComponent(InventoryItemComponent));
    if (!currentItem || !currentItem.GetParentSlot() || !currentItem.GetParentSlot().GetStorage()) break;
    current = currentItem.GetParentSlot().GetStorage().GetOwner();
    entry.Depth++;
   }
   m_Items.Insert(entry);
  }
  // Native death can drop the held weapon before corpse inventory is searched.
  // Only exact pre-death item references are eligible, never nearby strangers'
  // equipment, and items now in any inventory are left with their current owner.
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent));
  array<IEntity> recorded = {};
  if (medical) medical.BCMM_CopyDeathEquipment(recorded);
  foreach (IEntity dropped : recorded)
  {
   if (!dropped) continue;
   bool queued;
   foreach (BCMM_BagTransferEntry previous : m_Items) if (previous.Item == dropped) queued = true;
   if (queued) continue;
   InventoryItemComponent component = InventoryItemComponent.Cast(dropped.FindComponent(InventoryItemComponent));
   if (!component || component.GetParentSlot() || dropped.GetParent()) continue;
   if (vector.Distance(dropped.GetOrigin(), m_Corpse.GetOrigin()) > Setting("deathEquipmentRecoveryRangeMetres")) continue;
   BCMM_BagTransferEntry entry = new BCMM_BagTransferEntry();
   entry.Item = dropped;
   entry.FromWorld = true;
   entry.WorldPosition = dropped.GetOrigin();
   entry.WorldAngles = dropped.GetAngles();
   m_Items.Insert(entry);
  }
  for (int i = 0; i < m_Items.Count(); i++)
   for (int j = i + 1; j < m_Items.Count(); j++)
    if (m_Items[j].Depth > m_Items[i].Depth)
    {
     BCMM_BagTransferEntry swap = m_Items[i];
     m_Items[i] = m_Items[j];
     m_Items[j] = swap;
    }
 }
 protected void MoveNext()
 {
  if (!StillValid() || !m_Bag || !m_Target) { Rollback(); return; }
  if (m_Index >= m_Items.Count())
  {
   m_Target.SetTransferring(false);
   m_Phase = BCMM_BagTransferPhase.CONSUME;
   if (!m_ActorInventory.TryDeleteItem(m_Empty, this) && m_Phase == BCMM_BagTransferPhase.CONSUME) Rollback();
   return;
  }
  BCMM_BagTransferEntry entry = m_Items[m_Index];
  bool accepted;
  if (entry.FromWorld)
  {
   // Recheck at the actual move, not only the snapshot. Another player may
   // have picked up the recorded weapon while earlier items were moving.
   InventoryItemComponent currentItem;
   if (entry.Item) currentItem = InventoryItemComponent.Cast(entry.Item.FindComponent(InventoryItemComponent));
   if (!currentItem || currentItem.GetParentSlot() || entry.Item.GetParent() || vector.Distance(entry.Item.GetOrigin(), m_Corpse.GetOrigin()) > Setting("deathEquipmentRecoveryRangeMetres"))
   {
    m_Index++;
    GetGame().GetCallqueue().CallLater(MoveNext, 1);
    return;
   }
   accepted = m_BagManager.TryInsertItemInStorage(entry.Item, m_Target, -1, this);
  }
  else accepted = m_Source.TryMoveItemToStorage(entry.Item, m_Target, -1, this);
  if (!accepted && m_Phase == BCMM_BagTransferPhase.TRANSFER) Rollback();
 }
 override protected void OnComplete()
 {
  if (m_Phase == BCMM_BagTransferPhase.FINISHED) return;
  if (m_Phase == BCMM_BagTransferPhase.TRANSFER) { m_Index++; GetGame().GetCallqueue().CallLater(MoveNext, 1); return; }
  if (m_Phase == BCMM_BagTransferPhase.ROLLBACK) { GetGame().GetCallqueue().CallLater(FinishRestoreStep, 1); return; }
  if (m_Phase != BCMM_BagTransferPhase.CONSUME) return;
  m_Phase = BCMM_BagTransferPhase.COMMITTING;
  GetGame().GetCallqueue().CallLater(Commit, 1);
 }
 protected void Commit()
 {
  if (m_Phase != BCMM_BagTransferPhase.COMMITTING) return;
  // Consumption is the commit point. Original item entities have moved; none
  // were cloned. Only now may the original dead character be removed.
  Print("BCMM bagging commit inventory complete");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode && !mode.BCMM_TransferRemains(m_Corpse,m_Bag))
  {
   if(m_Identity) m_Identity.SetReady(true);
   Print("BCMM bagging retained original remains: death record transfer failed",LogLevel.ERROR);
   Release();
   return;
  }
  if (m_Identity) m_Identity.SetReady(true);
  if (m_Corpse) SCR_EntityHelper.DeleteEntityAndChildren(m_Corpse);
  Print("BCMM bagging corpse removal complete");
  Release();
 }
 override protected void OnFailed()
 {
  if (m_Phase == BCMM_BagTransferPhase.FINISHED) return;
  if (m_Phase == BCMM_BagTransferPhase.ROLLBACK)
  {
   m_RestoreFailed = true;
   m_RestoreIndex--;
   RestoreNext();
   return;
  }
  Rollback();
 }
 protected void Rollback()
 {
  if (m_Phase == BCMM_BagTransferPhase.ROLLBACK || m_Phase == BCMM_BagTransferPhase.FINISHED) return;
  m_Phase = BCMM_BagTransferPhase.ROLLBACK;
  if (m_Target) m_Target.SetTransferring(false);
  m_RestoreIndex = m_Items.Count() - 1;
  RestoreNext();
 }
 protected void RestoreNext()
 {
  if (m_RestoreIndex < 0)
  {
   if (m_RestoreFailed)
   {
    if (m_Identity) m_Identity.SetReady(true);
    Print("BCMM bagging rollback incomplete: recovery bag and original items preserved", LogLevel.ERROR);
   }
   else if (m_Bag) SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);
   Release();
   return;
  }
  BCMM_BagTransferEntry entry = m_Items[m_RestoreIndex];
  InventoryItemComponent component;
  if (entry.Item) component = InventoryItemComponent.Cast(entry.Item.FindComponent(InventoryItemComponent));
  if (!component || !component.GetParentSlot() || component.GetParentSlot().GetStorage() != m_Target)
  {
   m_RestoreIndex--;
   RestoreNext();
   return;
  }
  bool accepted;
  if (entry.FromWorld) accepted = m_BagManager.TryRemoveItemFromStorage(entry.Item, m_Target, this);
  else if (m_Source && entry.Storage) accepted = m_Source.TryMoveItemToStorage(entry.Item, entry.Storage, entry.Slot, this);
  if (!accepted && m_Phase == BCMM_BagTransferPhase.ROLLBACK) OnFailed();
 }
 protected void FinishRestoreStep()
 {
  if (m_Phase != BCMM_BagTransferPhase.ROLLBACK || m_RestoreIndex < 0) return;
  BCMM_BagTransferEntry entry = m_Items[m_RestoreIndex];
  if (entry.FromWorld && entry.Item)
  {
   entry.Item.SetOrigin(entry.WorldPosition);
   entry.Item.SetAngles(entry.WorldAngles);
  }
  m_RestoreIndex--;
  RestoreNext();
 }
 protected void Release()
 {
  if (m_Phase == BCMM_BagTransferPhase.FINISHED) return;
  m_Phase = BCMM_BagTransferPhase.FINISHED;
  if (m_Corpse)
  {
   SCR_GarbageSystem garbage = SCR_GarbageSystem.GetByEntityWorld(m_Corpse);
   if (garbage && !BCMM_RemainsProtection.ShouldProtect(m_Corpse)) garbage.UpdateBlacklist(m_Corpse, false);
  }
  s_Active.RemoveItem(this);
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestBagging(IEntity corpse)
 {
  if (GetGame().GetPlayerController() == this && corpse) Rpc(BCMM_BaggingRequest, BCMM_BodyBagComponent.Id(corpse));
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_BaggingRequest(RplId corpse)
 {
  BCMM_BaggingOperation.Begin(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(corpse));
 }
}
class BCMM_BagCorpseAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user) { return BCMM_BaggingOperation.CanBegin(user, GetOwner()); }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestBagging(pOwnerEntity);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
