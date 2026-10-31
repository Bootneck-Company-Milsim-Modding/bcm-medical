// Failure injection exists only in the unpublished diagnostic addon.
modded class BCMM_BaggingOperation
{
 protected static int s_BCMMFailBaggingAfter = -1;
 static bool TestPickupBeforeWorldMove;
 static IEntity TestPickedUpWeapon;
 static void BCMM_TestFailBaggingAfter(int items) { s_BCMMFailBaggingAfter = items; }
 override protected void MoveNext()
 {
  if (TestPickupBeforeWorldMove && m_Index < m_Items.Count() && m_Items[m_Index].FromWorld)
  {
   TestPickupBeforeWorldMove = false;
   IEntity weapon = m_Items[m_Index].Item;
   if (m_ActorInventory.TryInsertItem(weapon, EStoragePurpose.PURPOSE_ANY)) TestPickedUpWeapon = weapon;
  }
  if ((s_BCMMFailBaggingAfter >= 0 && m_Index >= s_BCMMFailBaggingAfter) || (s_BCMMFailBaggingAfter == -2 && m_Index >= m_Items.Count()))
  {
   s_BCMMFailBaggingAfter = -1;
   Rollback();
   return;
  }
  super.MoveNext();
 }
}
class BCMM_BaggingRollbackChecks : BCMM_BaggingRuntimeChecks
{
 protected ref array<IEntity> m_OriginalRoots = {};
 protected IEntity RootOf(IEntity item)
 {
  IEntity root = item;
  for (int guard = 0; root && root.GetParent() && guard < 32; guard++) root = root.GetParent();
  return root;
 }
 override protected void Bag()
 {
  // Native death may have already dropped the held rifle; restore that location.
  foreach (IEntity item : m_OriginalItems) m_OriginalRoots.Insert(RootOf(item));
  super.Bag();
 }
 override void Start(BCMM_DomainChecks suite)
 {
  BCMM_BaggingOperation.BCMM_TestFailBaggingAfter(-2);
  super.Start(suite);
 }
 override protected void Verify()
 {
  m_Suite.Record(m_Corpse && m_Empty, "Failed bagging preserves original corpse and unconsumed kit");
  SCR_InventoryStorageManagerComponent inventory;
  if (m_Corpse) inventory = SCR_InventoryStorageManagerComponent.Cast(m_Corpse.FindComponent(SCR_InventoryStorageManagerComponent));
  foreach (int index, IEntity item : m_OriginalItems)
   m_Suite.Record(item && RootOf(item) == m_OriginalRoots[index], "Failed bagging restores exact original equipment hierarchy: " + m_OriginalNames[index]);
  bool orphan;
  foreach (BCMM_BodyBagComponent bag : BCMM_BodyBagComponent.Instances)
   if (bag && bag.DeceasedName() == "BCM bagging test casualty") orphan = true;
  m_Suite.Record(!orphan && !BCMM_BaggingOperation.IsBusy(m_Corpse), "Successful rollback removes temporary bag and releases casualty reservation");
  BCMM_RemainsRecord remains=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_RemainsToken);
  m_Suite.Record(remains && remains.Physical==m_Corpse && remains.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION,"Rollback preserves original death token on exact exposed corpse");
  SCR_GarbageSystem garbage;
  if (m_Corpse) garbage = SCR_GarbageSystem.GetByEntityWorld(m_Corpse);
  m_Suite.Record(garbage && !garbage.Insert(m_Corpse, 1) && !garbage.IsInserted(m_Corpse), "Rollback preserves native corpse minimum protection after reservation release");
  GetGame().GetCallqueue().CallLater(VerifyProtectedRollback, 5000);
 }
 protected void VerifyProtectedRollback()
 {
  m_Suite.Record(m_Corpse && m_Empty && !BCMM_BaggingOperation.IsBusy(m_Corpse), "Original corpse and unused kit survive native garbage deadline after rollback");
  Finish();
 }
}
modded class BCMM_CargoOperation
{
 protected static int s_BCMMFailCargoAfter = -1;
 static void BCMM_TestFailCargoAfter(int items) { s_BCMMFailCargoAfter = items; }
 override protected void MoveNextGear()
 {
  if (s_BCMMFailCargoAfter >= 0 && m_GearIndex >= s_BCMMFailCargoAfter)
  {
   s_BCMMFailCargoAfter = -1;
   Rollback();
   return;
  }
  super.MoveNextGear();
 }
}
class BCMM_CargoRollbackChecks : BCMM_CargoRuntimeChecks
{
 override void Start(BCMM_DomainChecks suite)
 {
  m_Fixture = 4;
  BCMM_CargoOperation.BCMM_TestFailCargoAfter(1);
  super.Start(suite);
 }
 override protected void VerifyLoaded()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (state.State() == BCMM_BagCargoState.LOADING && m_WaitCount++ < 40)
  {
   GetGame().GetCallqueue().CallLater(VerifyLoaded, 250);
   return;
  }
  InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  SCR_VehicleInventoryStorageManagerComponent vehicleInventory = SCR_VehicleInventoryStorageManagerComponent.Cast(m_Vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
  BCMM_VehicleCargoComponent cargo = BCMM_VehicleCargoComponent.Cast(m_Vehicle.FindComponent(BCMM_VehicleCargoComponent));
  m_Suite.Record(state.State() == BCMM_BagCargoState.WORLD && !vehicleInventory.Contains(m_Bag), "Failed cargo load rolls bag back to world state");
  m_Suite.Record(m_Gear && bagInventory.Contains(m_Gear) && !vehicleInventory.Contains(m_Gear), "Failed cargo load restores exact original equipment to bag");
  m_Suite.Record(cargo.Count() == 0 && state.VehicleId() == RplId.Invalid(), "Failed cargo load leaves no stale manifest or owning vehicle");
  FinishFixture();
 }
}
class BCMM_BaggingPickupChecks : BCMM_BaggingRuntimeChecks
{
 override void Start(BCMM_DomainChecks suite)
 {
  BCMM_BaggingOperation.TestPickupBeforeWorldMove = true;
  BCMM_BaggingOperation.TestPickedUpWeapon = null;
  super.Start(suite);
 }
 override protected void Verify()
 {
  foreach (BCMM_BodyBagComponent bag : BCMM_BodyBagComponent.Instances)
   if (bag && bag.DeceasedName() == "BCM bagging test casualty") m_Bag = bag.GetOwner();
  IEntity weapon = BCMM_BaggingOperation.TestPickedUpWeapon;
  InventoryStorageManagerComponent actorInventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(weapon && BelongsTo(actorInventory, m_Actor, weapon), "Weapon picked up after snapshot remains with current holder");
  m_Suite.Record(m_Bag && !m_Corpse && !m_Empty, "Concurrent weapon pickup permits remaining bagging to commit");
  if (m_Bag)
  {
   InventoryStorageManagerComponent bagInventory = InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
   foreach (int index, IEntity item : m_OriginalItems)
    m_Suite.Record(BelongsTo(actorInventory, m_Actor, item) || BelongsTo(bagInventory, m_Bag, item), "Concurrent pickup preserves original item without duplication: " + m_OriginalNames[index]);
   m_Suite.Record(!BelongsTo(bagInventory, m_Bag, weapon), "Bag does not take the picked-up rifle back from current holder");
  }
  BCMM_BaggingOperation.TestPickedUpWeapon = null;
  Finish();
 }
}
modded class BCMM_SplintTreatment
{
 override protected void Finish()
 {
  bool retained = !m_Consuming && m_Player && m_Kit && m_Inventory && m_Inventory.Contains(m_Kit);
  if(retained)
  {
   CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
   PrintFormat("BCMM_PROCEDURE_CANCEL_STATE token=%1 observed=%2 complete=%3 using=%4 held=%5 drift=%6 reach=%7 eligible=%8 raised=%9",m_ProcedureToken,m_AnimationObserved,m_AnimationComplete,controller.IsUsingItem(),controller.GetAttachedGadgetAtLeftHandSlot()==m_Kit,vector.Distance(m_Actor.GetOrigin(),m_StartPosition),BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),StillEligible(),controller.IsWeaponRaised());
   PrintFormat("BCMM_PROCEDURE_CANCEL_TIME now=%1 deadline=%2 ended=%3",GetGame().GetWorld().GetWorldTime()*0.001,m_Deadline,m_AnimationEndedAt);
   TraceParam trace=new TraceParam();trace.Start=ChimeraCharacter.Cast(m_Actor).EyePosition();trace.End=m_Target.GetOrigin()+"0 0.7 0";trace.Flags=TraceFlags.WORLD|TraceFlags.ENTS;trace.LayerMask=EPhysicsLayerPresets.Projectile;array<IEntity> exclusions={m_Actor,m_Target};trace.ExcludeArray=exclusions;
   float fraction=m_Actor.GetWorld().TraceMove(trace);
   PrintFormat("BCMM_PROCEDURE_CANCEL_REACH distance=%1 fraction=%2 hit=%3 falling=%4 swimming=%5 unconscious=%6 life=%7 targetLife=%8",vector.Distance(m_Actor.GetOrigin(),m_Target.GetOrigin()),fraction,trace.TraceEnt,controller.IsFalling(),controller.IsSwimming(),controller.IsUnconscious(),controller.GetLifeState(),ChimeraCharacter.Cast(m_Target).GetCharacterController().GetLifeState());
  }
  super.Finish();
  if (retained) Print("BCMM_SPLINT_SERVER_CANCEL_RETAINED_KIT");
 }
}

class BCMM_TestAccessDiagnostics
{
 static void Trace(IEntity actor,IEntity target)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(actor);CharacterControllerComponent controller=character.GetCharacterController();
  TraceParam trace=new TraceParam();trace.Start=character.EyePosition();trace.End=target.GetOrigin()+"0 0.7 0";trace.Flags=TraceFlags.WORLD|TraceFlags.ENTS;trace.LayerMask=EPhysicsLayerPresets.Projectile;array<IEntity> exclusions={actor,target};trace.ExcludeArray=exclusions;
  float fraction=actor.GetWorld().TraceMove(trace);
  PrintFormat("BCMM_ACCESS_TRACE actor=%1 target=%2 eye=%3 distance=%4 fraction=%5 hit=%6",actor.GetOrigin(),target.GetOrigin(),character.EyePosition(),vector.Distance(actor.GetOrigin(),target.GetOrigin()),fraction,trace.TraceEnt);
  PrintFormat("BCMM_ACCESS_STATE falling=%1 swimming=%2 unconscious=%3 life=%4 targetLife=%5 vehicle=%6 held=%7",controller.IsFalling(),controller.IsSwimming(),controller.IsUnconscious(),controller.GetLifeState(),ChimeraCharacter.Cast(target).GetCharacterController().GetLifeState(),character.IsInVehicle(),controller.GetAttachedGadgetAtLeftHandSlot());
 }
}