// Native rigid-body drag fixture; the diagnostic platform supplies collision.
class BCMM_DragRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected IEntity m_Actor;
 protected IEntity m_Bag;
 protected IEntity m_Rope;
 protected IEntity m_Floor;
 protected vector m_Start;
 protected bool m_Network;
 protected int m_OwnerWait;
 void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  string network; m_Network = System.GetCLIParam("bcmmDragNetwork", network);
  m_Floor = Spawn("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et", "100 0.5 0");
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", "100 1.2 0");
  m_Bag = Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et", "100.7 1 0");
  m_Rope = Spawn("{6AB4B3F3FD216007}Prefabs/Items/BCMM_DragRope.et", "100 1.5 0");
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", "100 1.5 0");
  if (backpack && m_Actor)
  {
   SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
   m_Suite.Record(inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Drag actor receives native equipment storage");
  }
  m_Suite.Record(m_Actor && m_Bag && m_Rope && m_Floor, "Native drag fixture entities spawn");
  GetGame().GetCallqueue().CallLater(CheckManual, 1500);
 }
 protected IEntity Spawn(ResourceName path, vector position)
 {
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(Resource.Load(path), GetGame().GetWorld(), spawn);
 }
 protected void CheckManual()
 {
  if (!m_Actor || !m_Bag || !m_Rope) { Finish(); return; }
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(!BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, true, state.Revision()), "Rope towing requires an actual rope item");
  bool started = BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, false, state.Revision());
  m_Suite.Record(started && state.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG, "Native manual drag reserves bag with drag-only mode");
  m_Suite.Record(!BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, false, state.Revision()), "Duplicate native drag session rejected");
  m_Suite.Record(!BCMM_NativeBagDrag.Release(m_Bag, m_Bag), "Another entity cannot release actor's drag reservation");
  BCMM_NativeBagDrag.Release(m_Actor, m_Bag);
  SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  m_Suite.Record(inventory.FindStorageForItem(m_Bag, EStoragePurpose.PURPOSE_ANY) == null, "Filled bag cannot be packed into personal inventory");
  m_Suite.Record(inventory.TryInsertItem(m_Rope, EStoragePurpose.PURPOSE_ANY), "Native rope item enters actor inventory");
  GetGame().GetCallqueue().CallLater(CheckRope, 500);
 }
 protected void CheckRope()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(BCMM_NativeBagDrag.HasRope(m_Actor), "Rope capability comes from authoritative inventory");
  m_Suite.Record(BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, true, state.Revision()), "Rope-equipped native actor can attach drag rope");
  m_Start = m_Bag.GetOrigin();
  m_Actor.SetOrigin(m_Start + "0 0.5 3");
  GetGame().GetCallqueue().CallLater(CheckMovement, 1500);
 }
 protected void CheckMovement()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  PrintFormat("BCMM_DRAG start=%1 now=%2 actor=%3 mode=%4 velocity=%5", m_Start, m_Bag.GetOrigin(), m_Actor.GetOrigin(), state.DragMode(), m_Bag.GetPhysics().GetVelocity());
  m_Suite.Record(state.DragMode() == BCMM_TransportMode.BODY_BAG_ROPE_DRAG, "Rope session remains active during native follow");
  m_Suite.Record(m_Bag.GetOrigin()[2] > m_Start[2] + 0.25, "Native bag actually follows by rigid-body movement");
  m_Suite.Record(vector.Distance(m_Bag.GetOrigin(), m_Actor.GetOrigin()) < 2.75, "Native follow converges towards configured rope length");
  if (m_Network)
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(1, m_Actor, m_Bag);
   GetGame().GetCallqueue().CallLater(ForceSeparation, 20000);
  }
  else ForceSeparation();
 }
 protected void ForceSeparation()
 {
  m_Start = m_Bag.GetOrigin();
  m_Actor.SetOrigin(m_Start + "10 0 0");
  GetGame().GetCallqueue().CallLater(CheckRelease, 300);
 }
 protected void CheckRelease()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(state.DragMode() == BCMM_TransportMode.NONE && state.DraggingActor() == RplId.Invalid(), "Excessive separation releases rope on authority");
  m_Suite.Record(vector.Distance(m_Bag.GetOrigin(), m_Start) < 2, "Safety release does not teleport bag after carrier");
  if (m_Network)
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(2, m_Actor, m_Bag);
   GetGame().GetCallqueue().CallLater(PrepareOwnerDrag, 5000);
  }
  else Finish();
 }
 protected void PrepareOwnerDrag()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!player || !player.GetControlledEntity())
  {
   if (m_OwnerWait++ < 40) { GetGame().GetCallqueue().CallLater(PrepareOwnerDrag, 250); return; }
   m_Suite.Record(false, "Owner-controlled drag actor becomes available");
   Finish();
   return;
  }
  player.GetControlledEntity().SetOrigin(m_Bag.GetOrigin() + "1.2 0 0");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_AllowDragAcquire();
  m_OwnerWait = 0;
  GetGame().GetCallqueue().CallLater(CheckOwnerDrag, 500);
 }
 protected void CheckOwnerDrag()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if ((!player || state.DragMode() != BCMM_TransportMode.BODY_BAG_MANUAL_DRAG || state.DraggingActor() != BCMM_BodyBagComponent.Id(player.GetControlledEntity())) && m_OwnerWait++ < 40)
  {
   GetGame().GetCallqueue().CallLater(CheckOwnerDrag, 250);
   return;
  }
  m_Suite.Record(player && state.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG && state.DraggingActor() == BCMM_BodyBagComponent.Id(player.GetControlledEntity()), "Authenticated player RPC acquires native manual drag");
  if (!player) { Finish(); return; }
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(3, player.GetControlledEntity(), m_Bag);
  m_OwnerWait = 0;
  GetGame().GetCallqueue().CallLater(CheckOwnerRelease, 4000);
 }
 protected void CheckOwnerRelease()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.DragMode() != BCMM_TransportMode.NONE && m_OwnerWait++ < 40)
  {
   GetGame().GetCallqueue().CallLater(CheckOwnerRelease, 250);
   return;
  }
  m_Suite.Record(state.DragMode() == BCMM_TransportMode.NONE && !BCMM_NativeBagDrag.FindBag(m_Bag), "Authenticated player RPC releases native drag reservation");
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(4, player.GetControlledEntity(), m_Bag);
  GetGame().GetCallqueue().CallLater(PrepareOwnerWalk, 4000);
 }
 protected vector m_OwnerWalkStart;
 protected vector m_WalkBagStart;
 protected void PrepareOwnerWalk()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!player || !player.GetControlledEntity()) { m_Suite.Record(false, "Owner available for movement test"); Finish(); return; }
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Actor = player.GetControlledEntity();
  m_Actor.SetOrigin(m_Bag.GetOrigin() + "0 0.2 1.2");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(5, m_Actor, m_Bag);
  GetGame().GetCallqueue().CallLater(AllowOwnerReacquire, 2000);
  m_OwnerWait = 0;
  GetGame().GetCallqueue().CallLater(StartOwnerWalk, 500);
 }
 protected void StartOwnerWalk()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.DraggingActor() != BCMM_BodyBagComponent.Id(m_Actor) && m_OwnerWait++ < 40) { GetGame().GetCallqueue().CallLater(StartOwnerWalk, 250); return; }
  m_Suite.Record(state.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG, "Owner reacquires bag through authenticated action");
  m_OwnerWalkStart = m_Actor.GetOrigin(); m_WalkBagStart = m_Bag.GetOrigin();
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(6, m_Actor, m_Bag);
  GetGame().GetCallqueue().CallLater(CheckOwnerWalk, 4000);
 }
 protected void AllowOwnerReacquire() { SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_AllowDragAcquire(); }
 protected void CheckOwnerWalk()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  PrintFormat("BCMM_DRAG_OWNER_WALK actorDistance=%1 bagDistance=%2 mode=%3", vector.Distance(m_Actor.GetOrigin(), m_OwnerWalkStart), vector.Distance(m_Bag.GetOrigin(), m_WalkBagStart), state.DragMode());
  m_Suite.Record(vector.Distance(m_Actor.GetOrigin(), m_OwnerWalkStart) > 0.3, "Native owning-client movement reaches dedicated authority");
  m_Suite.Record(vector.Distance(m_Bag.GetOrigin(), m_WalkBagStart) > 0.3 && state.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG, "Manual bag follows actual owning-client movement without cancellation");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(7, m_Actor, m_Bag);
  m_OwnerWait = 0;
  GetGame().GetCallqueue().CallLater(CheckOwnerDisconnect, 500);
 }
 protected void CheckOwnerDisconnect()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if ((GetGame().GetPlayerManager().GetPlayerControlledEntity(1) || state.DragMode() != BCMM_TransportMode.NONE) && m_OwnerWait++ < 80) { GetGame().GetCallqueue().CallLater(CheckOwnerDisconnect, 250); return; }
  m_Suite.Record(!GetGame().GetPlayerManager().GetPlayerControlledEntity(1), "Actual drag owner disconnects");
  m_Suite.Record(state.DragMode() == BCMM_TransportMode.NONE && !BCMM_NativeBagDrag.FindBag(m_Bag), "Owner disconnect releases manual drag on authority");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishDragTest(8, null, m_Bag);
  GetGame().GetCallqueue().CallLater(Finish, 5000);
 }
 protected void Finish()
 {
  if (m_Bag) SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  if (m_Rope) SCR_EntityHelper.DeleteEntityAndChildren(m_Rope);
  if (m_Floor) delete m_Floor;
  m_Suite.Complete();
 }
}
