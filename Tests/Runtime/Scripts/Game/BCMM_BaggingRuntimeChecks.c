// Full native corpse -> bag -> vehicle inventory acceptance fixture.
class BCMM_BaggingRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected IEntity m_Actor;
 protected IEntity m_Floor;
 protected IEntity m_FloorNorth;
 protected IEntity m_FloorSouth;
 protected IEntity m_Corpse;
 protected IEntity m_Empty;
 protected IEntity m_Bag;
 protected IEntity m_Vehicle;
 protected int m_RemainsToken;
 protected ref array<IEntity> m_OriginalItems = {};
 protected ref array<string> m_OriginalNames = {};
 protected ref array<IEntity> m_RequestedExtras = {};
 protected int m_CargoWait;
 void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  // One wide, flat deck above the autotest world's separate floor. Narrow
  // doorstep strips created an accidental half-metre step under the bag's
  // offset collider, turning a flat-follow test into a wall-release test.
  float y = GetGame().GetWorld().GetSurfaceY(100, 0) + 2;
  m_Floor = Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et", Vector(101, y, 0));
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", Vector(100, y + 0.2, 0));
  m_Corpse = Spawn("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et", Vector(101, y + 0.2, 0));
  m_Empty = Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et", Vector(100, y + 0.5, 0));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", Vector(100, y + 0.5, 0));
  if (m_Actor && backpack)
  {
   SCR_InventoryStorageManagerComponent actorInventory = SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
   m_Suite.Record(actorInventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Native backpack equips to carry folded body bag");
  }
  m_Suite.Record(m_Actor && m_Corpse && m_Empty, "Bagging fixture entities spawn");
  GetGame().GetCallqueue().CallLater(Prepare, 1500);
 }
 protected IEntity Spawn(ResourceName path, vector position)
 {
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(Resource.Load(path), GetGame().GetWorld(), spawn);
 }
 protected void Prepare()
 {
  if (!m_Actor || !m_Corpse || !m_Empty) { Finish(); return; }
  SCR_InventoryStorageManagerComponent actorInventory = SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  BaseInventoryStorageComponent storage = actorInventory.FindStorageForItem(m_Empty, EStoragePurpose.PURPOSE_ANY);
  m_Suite.Record(storage && actorInventory.TryInsertItemInStorage(m_Empty, storage), "Actor receives original empty body bag item");
  SCR_InventoryStorageManagerComponent corpseStorage = SCR_InventoryStorageManagerComponent.Cast(m_Corpse.FindComponent(SCR_InventoryStorageManagerComponent));
  IEntity casualtyPack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Corpse.GetOrigin());
  m_Suite.Record(casualtyPack && corpseStorage.TryInsertItem(casualtyPack, EStoragePurpose.PURPOSE_ANY), "Casualty receives a real nested-equipment backpack");
  GetGame().GetCallqueue().CallLater(AddRequestedEquipment, 1000);
 }
 protected void AddRequestedEquipment()
 {
  SCR_InventoryStorageManagerComponent corpseStorage = SCR_InventoryStorageManagerComponent.Cast(m_Corpse.FindComponent(SCR_InventoryStorageManagerComponent));
  array<ResourceName> extras = {"{D41D22DD1B8E921E}Prefabs/Weapons/Grenades/M18/Smoke_M18_Green.et", "{9BBDEE253A16CC66}Prefabs/Weapons/Grenades/M18/Smoke_M18_Yellow.et", "{33CBDE73AB48172A}Prefabs/Weapons/Explosives/DemoBlock_M112/DemoBlock_M112.et", "{33CBDE73AB48172A}Prefabs/Weapons/Explosives/DemoBlock_M112/DemoBlock_M112.et"};
  foreach (ResourceName path : extras)
  {
   IEntity item = Spawn(path, m_Corpse.GetOrigin());
   m_RequestedExtras.Insert(item);
   m_Suite.Record(item && corpseStorage.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY), "Requested original smoke/demolition equipment insertion accepted: " + path);
  }
  GetGame().GetCallqueue().CallLater(SnapshotCasualty, 1000);
 }
 protected void SnapshotCasualty()
 {
  SCR_InventoryStorageManagerComponent corpseInventory = SCR_InventoryStorageManagerComponent.Cast(m_Corpse.FindComponent(SCR_InventoryStorageManagerComponent));
  foreach (IEntity extra : m_RequestedExtras) m_Suite.Record(extra && corpseInventory.Contains(extra), "Requested smoke/demolition entity actually belongs to casualty before death");
  corpseInventory.GetItems(m_OriginalItems, EStoragePurpose.PURPOSE_ANY);
  BCMM_BaggingOperation.CollectEquipment(m_Corpse, m_OriginalItems);
  foreach (IEntity original : m_OriginalItems) m_OriginalNames.Insert(original.GetPrefabData().GetPrefabName());
  m_Suite.Record(m_OriginalItems.Count() > 0, "Casualty has real original equipment to recover");
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent));
  medical.BCMM_CaptureDeceasedName("BCM bagging test casualty");
  medical.BCMM_TestKill();

  GetGame().GetCallqueue().CallLater(ApproachCasualty, 3000);
 }
 protected void ApproachCasualty()
 {
  if (!m_Actor || !m_Corpse) { Finish(); return; }
  // Follow the settled ragdoll rather than assuming it stays at its spawn pivot.
  m_Actor.SetOrigin(m_Corpse.GetOrigin() + "1 0.5 0");
  GetGame().GetCallqueue().CallLater(Bag, 500);
 }
 protected void Bag()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_RemainsToken=mode.BCMM_RegisterRemains(m_Corpse);
  m_Suite.Record(m_RemainsToken>0,"Actual native corpse receives an authority death token");
  m_Suite.Record(mode.BCMM_RegisterRemains(m_Corpse)==m_RemainsToken,"Repeated observation retains original exact death token");
  m_Suite.Record(!mode.BCMM_CommitRemains(m_RemainsToken,m_Actor,BCMM_RemainsDisposition.ADMIN_RELEASED),"Another physical entity cannot dispose exact casualty token");
  m_Suite.Record(!mode.BCMM_CommitRemains(m_RemainsToken,m_Corpse,999),"Undefined terminal disposition cannot mutate death record");
  bool accepted = BCMM_BaggingOperation.Begin(m_Actor, m_Corpse);
  m_Suite.Record(accepted, "Authority accepts bagging dead casualty with real kit");
  m_Suite.Record(!BCMM_BaggingOperation.Begin(m_Actor, m_Corpse), "Competing duplicate bagging reservation rejected");
  GetGame().GetCallqueue().CallLater(Verify, BCMM_BaggingOperation.Setting("baggingDurationSeconds") * 1000 + 4000);
 }
 protected void Verify()
 {
  Print("BCMM_BAGGING verify starts");
  foreach (BCMM_BodyBagComponent bag : BCMM_BodyBagComponent.Instances)
   if (bag && bag.DeceasedName() == "BCM bagging test casualty") m_Bag = bag.GetOwner();
  PrintFormat("BCMM_BAGGING verify bag=%1 corpse=%2 empty=%3", m_Bag, m_Corpse, m_Empty);
  m_Suite.Record(m_Bag != null, "Completed bag preserves deceased identity");
  if (m_Bag)
  {
   BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
   InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
   m_Suite.Record(state.Ready(), "Completed bag is ready for search and transport");
   m_Suite.Record(m_Bag.FindComponent(SCR_EditableEntityComponent)!=null,"completed native Medical bag exposes Game Master selection/deletion component");
   BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_RemainsToken);
   PrintFormat("BCMM_REMAINS_TEST expectedToken=%1 bagToken=%2 record=%3 physical=%4 bag=%5",m_RemainsToken,state.RemainsToken(),record,record.Physical,m_Bag);
   m_Suite.Record(state.RemainsToken()==m_RemainsToken && record && record.Physical==m_Bag,"Completed bag inherits exact original authority death record");
   m_Suite.Record(record && record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"Committed corpse removal does not misclassify bagged remains as admin disposal");
   foreach (int index, IEntity item : m_OriginalItems)
    m_Suite.Record(BelongsTo(inventory, m_Bag, item), "Original casualty equipment survives transfer without cloning: " + m_OriginalNames[index]);
  }
  m_Suite.Record(!m_Corpse, "Original corpse removed only after bagging commits");
  m_Suite.Record(!m_Empty, "Successful bagging consumes the original empty bag");
  if (m_Bag) { BCMM_Beta01BagVisual.Capture(m_Bag, "recovered-loaded-bag-ground"); DragRecovered(); }
  else Finish();
 }
 protected vector m_LoadedDragStart;
 protected int m_DragOriginalCollision;
 protected int m_DragGroundWait;
 protected void DragRecovered()
 {
  m_LoadedDragStart = m_Bag.GetOrigin();
  m_Actor.SetOrigin(m_LoadedDragStart + "0 0.2 1");
  string airborne;
  m_DragGroundWait=0;
  if(System.GetCLIParam("bcmmBeta01DragAirborneSetup",airborne))
  {
   m_Actor.SetOrigin(m_LoadedDragStart+"0 3 1");
   GetGame().GetCallqueue().CallLater(StartLoadedDrag,100);return;
  }
  GetGame().GetCallqueue().CallLater(StartLoadedDrag, 700);
 }
 protected bool WaitForDragGround(bool rope)
 {
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  if(!controller.IsFalling())return false;
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if(m_DragGroundWait++==0)
  {
   m_Suite.Record(!BCMM_NativeBagDrag.Begin(m_Actor,m_Bag,rope,state.Revision()),"Native falling actor cannot acquire a drag reservation");
   PrintFormat("BCMM_DRAG_SETUP_WAIT actor=%1 bag=%2 rope=%3 falling=1",m_Actor.GetOrigin(),m_Bag.GetOrigin(),rope);
  }
  if(m_DragGroundWait>30){m_Suite.Record(false,"Repositioned drag actor reaches supporting ground within three seconds");Finish();return true;}
  if(rope)GetGame().GetCallqueue().CallLater(StartLoadedRope,100);
  else GetGame().GetCallqueue().CallLater(StartLoadedDrag,100);
  return true;
 }
 protected void StartLoadedDrag()
 {
  if(WaitForDragGround(false))return;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  CharacterControllerComponent dragController = ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  PrintFormat("BCMM_DRAG_BEGIN actor=%1 bag=%2 falling=%3 swimming=%4 life=%5 unconscious=%6", m_Actor.GetOrigin(), m_Bag.GetOrigin(), dragController.IsFalling(), dragController.IsSwimming(), dragController.GetLifeState(), dragController.IsUnconscious());
  PrintFormat("BCMM_DRAG_BEGIN ready=%1 state=%2 revision=%3 existing=%4 actorBusy=%5 searched=%6", state.Ready(), state.State(), state.Revision(), BCMM_NativeBagDrag.FindBag(m_Bag), BCMM_NativeBagDrag.HasActor(m_Actor), BCMM_BagSearchSessions.IsSearched(m_Bag));
  m_DragOriginalCollision=m_Bag.GetPhysics().GetInteractionLayer();
  m_Suite.Record(BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, false, state.Revision()), "Manual drag starts on an actually recovered fully equipped casualty");
  m_Suite.Record(m_Bag.GetPhysics().GetInteractionLayer()==EPhysicsLayerPresets.ItemFireView,"Dragged bag uses native non-character item collision");
  m_Suite.Record(!ChimeraCharacter.Cast(m_Actor).GetCharacterController().CanJumpClimb(), "Manual bag drag blocks native jumping and vaulting while hands are occupied");
  m_Actor.SetOrigin(m_Bag.GetOrigin() + "0 0.1 3");
  GetGame().GetCallqueue().CallLater(CheckLoadedDrag, 1800);
 }
 protected void CheckLoadedDrag()
 {
  PrintFormat("BCMM_LOADED_DRAG start=%1 end=%2 actor=%3 angles=%4", m_LoadedDragStart, m_Bag.GetOrigin(), m_Actor.GetOrigin(), m_Bag.GetAngles());
  m_Suite.Record(m_Bag.GetOrigin()[2] > m_LoadedDragStart[2] + 0.25, "Fully equipped recovery bag physically moves during manual dragging");
  BCMM_NativeBagDrag.Release(m_Actor, m_Bag);
  m_Suite.Record(m_Bag.GetPhysics().GetInteractionLayer()==m_DragOriginalCollision,"Bag release restores this bag's exact original collision mask");
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().CanJumpClimb(), "Releasing manual bag drag restores native jump and vault eligibility");
  BCMM_Beta01BagVisual.Capture(m_Bag, "loaded-bag-after-manual-drag");
  string visual;
  if (System.GetCLIParam("bcmmBeta01BagVisual", visual) || System.GetCLIParam("bcmmBeta01LoadedRope", visual)) GetGame().GetCallqueue().CallLater(PrepareLoadedRope, 500);
  else GetGame().GetCallqueue().CallLater(LoadCargo, 500);
 }
 protected void PrepareLoadedRope()
 {
  IEntity rope = Spawn("{6AB4B3F3FD216007}Prefabs/Items/BCMM_DragRope.et", m_Actor.GetOrigin());
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(rope && inventory.TryInsertItem(rope, EStoragePurpose.PURPOSE_ANY), "Loaded casualty tow fixture receives original compact rope");
  m_Actor.SetOrigin(m_Bag.GetOrigin() + "0 0.1 1.2");
  m_DragGroundWait=0;
  GetGame().GetCallqueue().CallLater(StartLoadedRope, 700);
 }
 protected void StartLoadedRope()
 {
  if(WaitForDragGround(true))return;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, true, state.Revision()), "Rope tow starts on exact recovered loaded bag");
  m_LoadedDragStart = m_Bag.GetOrigin();
  m_Actor.SetOrigin(m_LoadedDragStart + "0 0.1 3");
  GetGame().GetCallqueue().CallLater(CheckLoadedRope, 1800);
 }
 protected void CheckLoadedRope()
 {
  m_Suite.Record(m_Bag.GetOrigin()[2] > m_LoadedDragStart[2] + 0.25, "Exact recovered loaded casualty bag moves during rope towing");
  m_Suite.Record(BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).DragMode() == BCMM_TransportMode.BODY_BAG_ROPE_DRAG, "Loaded rope session remains active at native follow distance");
  BCMM_Beta01BagVisual.Capture(m_Bag, "loaded-bag-rope-tow");
  GetGame().GetCallqueue().CallLater(FinishLoadedRope, 750);
 }
 protected void FinishLoadedRope() { BCMM_NativeBagDrag.Release(m_Actor, m_Bag); LoadCargo(); }
 protected bool BelongsTo(InventoryStorageManagerComponent inventory, IEntity container, IEntity item)
 {
  if (!inventory || !container || !item) return false;
  if (inventory.Contains(item)) return true;
  // Native Contains omits some fixed weapon parts that GetItems includes.
  // Require the exact original entity in the container's actual hierarchy.
  IEntity parent = item.GetParent();
  for (int guard = 0; parent && guard < 32; guard++)
  {
   if (parent == container) return true;
   parent = parent.GetParent();
  }
  return false;
 }
 protected void LoadCargo()
 {
  m_Vehicle = Spawn("{6AB4B3F3FD216004}Prefabs/Vehicles/BCMMedical/BCMM_CargoTruck.et", m_Actor.GetOrigin() + "3 0.2 0");
  if (!m_Vehicle) { m_Suite.Record(false, "Bagging-chain truck spawns"); Finish(); return; }
  m_Vehicle.GetPhysics().SetVelocity(vector.Zero);
  m_Vehicle.GetPhysics().SetActive(ActiveState.INACTIVE);
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, true, state.Revision()), "Real bagged casualty and equipment accepted into truck cargo");
  m_CargoWait = 0;
  GetGame().GetCallqueue().CallLater(VerifyCargo, 1500);
 }
 protected void VerifyCargo()
 {
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.State() == BCMM_BagCargoState.LOADING && m_CargoWait++ < 100) { GetGame().GetCallqueue().CallLater(VerifyCargo, 100); return; }
  m_Suite.Record(state.State() == BCMM_BagCargoState.STORED && inventory.Contains(m_Bag), "Full corpse recovery chain reaches native truck cargo");
  foreach (IEntity item : m_OriginalItems)
   m_Suite.Record(BelongsTo(inventory, m_Vehicle, item) && !BelongsTo(bagInventory, m_Bag, item), "Casualty original equipment is directly available in vehicle after loading");
  m_Suite.Record(BCMM_CargoOperation.Begin(m_Actor, m_Vehicle, m_Bag, false, state.Revision()), "Recovered casualty bag can be unloaded separately from gear");
  GetGame().GetCallqueue().CallLater(VerifyCargoUnloaded, 500);
 }
 protected void VerifyCargoUnloaded()
 {
  SCR_VehicleInventoryStorageManagerComponent inventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(state.State() == BCMM_BagCargoState.WORLD && !inventory.Contains(m_Bag), "Full recovery chain unloads named bag to world");
  foreach (IEntity item : m_OriginalItems)
   m_Suite.Record(BelongsTo(inventory, m_Vehicle, item), "Unloading leaves each original casualty item in vehicle");
  Finish();
 }
 protected void Finish()
 {

  Print("BCMM_BAGGING cleanup bag");
  if (m_Bag) SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);
  Print("BCMM_BAGGING cleanup corpse");
  if (m_Corpse) SCR_EntityHelper.DeleteEntityAndChildren(m_Corpse);
  if (m_Empty) SCR_EntityHelper.DeleteEntityAndChildren(m_Empty);
  Print("BCMM_BAGGING cleanup actor");
  if (m_Vehicle) SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  Print("BCMM_BAGGING cleanup floor");
  if (m_Floor) SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  if (m_FloorNorth) delete m_FloorNorth;
  if (m_FloorSouth) delete m_FloorSouth;
  Print("BCMM_BAGGING cleanup after floor");
  Print("BCMM_BAGGING cleanup complete");
  m_Suite.Complete();
 }
}
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_TestKill()
 {
  m_BCMMDeathPermitted = true;
  Kill(Instigator.CreateInstigator(GetOwner()));
 }
}
