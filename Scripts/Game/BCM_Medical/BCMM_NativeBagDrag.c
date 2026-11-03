// Original server-owned body-bag following. No character attachment, seats or
// carrying code. Native physics owns motion/collision; sweeps release on walls.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_DragRopeComponentClass : ScriptComponentClass {}
class BCMM_DragRopeComponent : ScriptComponent {}
class BCMM_NativeBagDrag
{
 static void ExcludeHierarchy(IEntity root, array<IEntity> exclusions)
 {
  if (!root || exclusions.Contains(root)) return;
  exclusions.Insert(root);
  IEntity child = root.GetChildren();
  while (child) { ExcludeHierarchy(child, exclusions); child = child.GetSibling(); }
 }
 protected static ref array<ref BCMM_NativeBagDrag> s_Active = {};
 protected static int s_NextToken;
 protected static float s_NextUpdate;
 protected IEntity m_Actor;
 protected IEntity m_Bag;
 protected BCMM_BodyBagComponent m_State;
 protected int m_PlayerId;
 protected int m_Token;
 protected bool m_Rope;
 protected float m_AirborneSince;
 protected float m_OverSpeedSince;
 protected string m_StopReason;
 protected bool Fail(string reason) { m_StopReason = reason; return false; }
 protected ref BCMM_BodyBagDragSession m_Policy = new BCMM_BodyBagDragSession();
 protected ref BCMM_Settings m_Settings = new BCMM_Settings();
 static bool ManualDragging(IEntity actor)
 {
  if (!actor) return false;
  RplId actorId = BCMM_BodyBagComponent.Id(actor);
  foreach (BCMM_BodyBagComponent bag : BCMM_BodyBagComponent.Instances)
   if (bag && bag.DragMode() == BCMM_TransportMode.BODY_BAG_MANUAL_DRAG && bag.DraggingActor() == actorId) return true;
  return false;
 }
 static bool HasActor(IEntity actor)
 {
  foreach(BCMM_NativeBagDrag session:s_Active)if(session.m_Actor==actor)return true;
  return false;
 }
 static bool HasRope(IEntity actor)
 {
  if (!actor) return false;
  SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(actor.FindComponent(SCR_InventoryStorageManagerComponent));
  if (!inventory) return false;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items) if (item && item.FindComponent(BCMM_DragRopeComponent)) return true;
  return false;
 }
 static BCMM_NativeBagDrag FindBag(IEntity bag)
 {
  foreach (BCMM_NativeBagDrag session : s_Active) if (session.m_Bag == bag) return session;
  return null;
 }
 protected static bool ActorValid(IEntity actor, bool allowAirborne = false)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character || character.IsInVehicle()) return false;
  CharacterControllerComponent controller = character.GetCharacterController();
  return controller && controller.GetLifeState() == ECharacterLifeState.ALIVE && !controller.IsUnconscious() && !controller.IsSwimming() && (allowAirborne || !controller.IsFalling());
 }
 static bool Begin(IEntity actor, IEntity bag, bool rope, int revision)
 {
  if (!Replication.IsServer() || !bag || !ActorValid(actor) || FindBag(bag)) return false;
  if(BCMM_LivingTransport.ForParticipant(actor))return false;
  if(BCMM_BagSearchSessions.IsSearched(bag))return false;
  foreach (BCMM_NativeBagDrag other : s_Active) if (other.m_Actor == actor) return false;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  InventoryItemComponent item = InventoryItemComponent.Cast(bag.FindComponent(InventoryItemComponent));
  if (!state || !state.Ready() || state.State() != BCMM_BagCargoState.WORLD || state.Revision() != revision || !item || item.GetParentSlot()) return false;
  if (vector.Distance(actor.GetOrigin(), bag.GetOrigin()) > BCMM_BaggingOperation.Setting("baggingRangeMetres")) return false;
  if (rope && !HasRope(actor)) return false;
  BCMM_NativeBagDrag session = new BCMM_NativeBagDrag();
  session.m_Actor = actor;
  session.m_Bag = bag;
  session.m_State = state;
  session.m_Rope = rope;
  session.m_Token = ++s_NextToken;
  session.m_PlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(actor);
  // Domain ownership uses an issued positive capability, never a client ID.
  if (!session.m_Policy.Begin(session.m_Token, session.m_Token, rope, !rope || HasRope(actor))) return false;
  state.SetDragging(BCMM_BodyBagComponent.Id(actor), session.m_Policy.Mode());
  Physics physics = bag.GetPhysics();
  if (physics) physics.SetActive(ActiveState.ACTIVE);
  s_Active.Insert(session);
  return true;
 }
 static bool Release(IEntity actor, IEntity bag)
 {
  if (!Replication.IsServer()) return false;
  BCMM_NativeBagDrag session = FindBag(bag);
  if (!session || session.m_Actor != actor) return false;
  session.End();
  return true;
 }
 protected void End()
 {
  if (!m_StopReason.IsEmpty()) Print("BCMM drag release: " + m_StopReason, LogLevel.NORMAL);
  m_Policy.ForceRelease();
  if (m_State) m_State.SetDragging(RplId.Invalid(), BCMM_TransportMode.NONE);
  if (m_Bag)
  {
   InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
   if (item && !item.GetParentSlot() && m_Bag.GetPhysics()) m_Bag.GetPhysics().SetVelocity(vector.Zero);
  }
  s_Active.RemoveItem(this);
 }
 protected bool Tick()
 {
  if (!m_Bag || !m_State || m_State.State() != BCMM_BagCargoState.WORLD || !m_State.Ready()) return Fail("bag unavailable");
  if (!ActorValid(m_Actor, true)) return Fail("actor no longer able to drag");
  InventoryItemComponent item = InventoryItemComponent.Cast(m_Bag.FindComponent(InventoryItemComponent));
  if (!item || item.GetParentSlot()) return Fail("bag entered inventory");
  if (m_PlayerId > 0 && GetGame().GetPlayerManager().GetPlayerControlledEntity(m_PlayerId) != m_Actor) return Fail("controller changed");
  if (m_Rope && !HasRope(m_Actor)) return Fail("rope no longer in kit");
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(m_Settings);
  CharacterControllerComponent controller = ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (controller.IsFalling())
  {
   if (m_AirborneSince <= 0) m_AirborneSince = now;
   if (now - m_AirborneSince > 0.5) return Fail("performer left supporting ground");
  }
  else m_AirborneSince = 0;
  vector movement = controller.GetVelocity(); movement[1] = 0;
  if (!m_Rope && controller.IsWeaponRaised()) return Fail("weapon raised while manually dragging");
  if (!m_Rope && controller.IsSprinting()) return Fail("sprinting while manually dragging");
  if (!m_Rope && movement.Length() > m_Settings.Get("manualBagDragMaxSpeedMetresPerSecond"))
  {
   // The owner's gait cap arrives through replication. Brief acceleration or
   // position-correction spikes must not cancel the first movement frame.
   if (m_OverSpeedSince <= 0) m_OverSpeedSince = now;
   if (now - m_OverSpeedSince > 0.35) return Fail("sustained movement exceeds manual drag speed limit");
  }
  else m_OverSpeedSince = 0;
  vector desired;
  if (!m_Policy.DesiredBagPosition(m_Actor.GetOrigin(), m_Bag.GetOrigin(), true, true, m_Settings, desired)) return Fail("rope safety distance exceeded");
  TraceParam floor = new TraceParam();
  floor.Start = Vector(desired[0], m_Actor.GetOrigin()[1] + 1, desired[2]);
  floor.End = floor.Start - "0 5 0";
  floor.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  floor.LayerMask = EPhysicsLayerPresets.Projectile;
  array<IEntity> exclusions = {};
  ExcludeHierarchy(m_Bag, exclusions); ExcludeHierarchy(m_Actor, exclusions);
  floor.ExcludeArray = exclusions;
  float ground = m_Bag.GetWorld().TraceMove(floor);
  if (ground >= 1) return Fail("no supporting surface");
  vector boundsMin, boundsMax;
  // Local model clearance must not grow when a loaded hierarchy rolls or
  // contains clothing with its own displaced world bounds.
  m_Bag.GetBounds(boundsMin, boundsMax);
  float originClearance = Math.Max(0.02, -boundsMin[1]);
  desired[1] = floor.Start[1] - 5 * ground + originClearance + 0.03;
  TraceBox path = new TraceBox();
  path.Mins = "-0.15 -0.1 -0.15";
  path.Maxs = "0.15 0.1 0.15";
  path.Start = m_Bag.GetOrigin() + "0 0.2 0";
  path.End = desired + "0 0.2 0";
  path.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  // Obstructions include physical geometry without a projectile layer.
  path.TargetLayers = -1;
  path.ExcludeArray = exclusions;
  float pathFraction=m_Bag.GetWorld().TraceMove(path);
  if (pathFraction < 0.99) return Fail("movement blocked");
  vector velocity = (desired - m_Bag.GetOrigin()) * m_Settings.Get("bagDragResponsiveness");
  float speed = velocity.Length();
  float maximum = m_Settings.Get("bagDragMaxSpeedMetresPerSecond");
  if (speed > maximum) velocity *= maximum / speed;
  Physics physics = m_Bag.GetPhysics();
  if (!physics) return Fail("no bag physics");
  vector normal = floor.TraceNorm;
  if (normal.LengthSq() > 0.01 && normal[1] > 0.3)
  {
   normal.Normalize();
   vector frame[4]; m_Bag.GetWorldTransform(frame);
   vector forward = frame[2];
   float projection = forward[0]*normal[0] + forward[1]*normal[1] + forward[2]*normal[2];
   forward -= normal * projection;
   if (forward.LengthSq() < 0.01) forward = "0 0 1";
   forward.Normalize();
   vector right = BCMM_GripMath.Cross(normal, forward); right.Normalize();
   frame[0] = right; frame[1] = normal; frame[2] = BCMM_GripMath.Cross(right, normal);
   m_Bag.SetWorldTransform(frame);
   physics.SetAngularVelocity(vector.Zero);
  }
  physics.SetActive(ActiveState.ACTIVE);
  physics.SetVelocity(velocity);
  return true;
 }
 static void Update()
 {
  if (!Replication.IsServer()) return;
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  if (now < s_NextUpdate) return;
  s_NextUpdate = now + 0.05;
  for (int i = s_Active.Count() - 1; i >= 0; i--)
  {
   BCMM_NativeBagDrag session = s_Active[i];
   if (!session.Tick()) session.End();
  }
 }
 static void Reset() { s_Active.Clear(); s_NextToken = 0; s_NextUpdate = 0; }
}
modded class SCR_PlayerController
{
 void BCMM_RequestBagDrag(IEntity bag, bool rope, bool release)
 {
  if (GetGame().GetPlayerController() != this || !bag) return;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));
  if (state) Rpc(BCMM_BagDragRequest, BCMM_BodyBagComponent.Id(bag), rope, release, state.Revision());
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_BagDragRequest(RplId bag, bool rope, bool release, int revision)
 {
  IEntity entity = BCMM_BodyBagComponent.Resolve(bag);
  if (release)
  {
   BCMM_BodyBagComponent state;
   if (entity) state = BCMM_BodyBagComponent.Cast(entity.FindComponent(BCMM_BodyBagComponent));
   if (state && state.Revision() == revision) BCMM_NativeBagDrag.Release(GetControlledEntity(), entity);
  }
  else BCMM_NativeBagDrag.Begin(GetControlledEntity(), entity, rope, revision);
 }
}
class BCMM_DragBagAction : ScriptedUserAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(GetOwner().FindComponent(BCMM_BodyBagComponent));
  return bag && bag.Ready() && !BCMM_BagSearchSessions.IsSearched(GetOwner()) && bag.State() == BCMM_BagCargoState.WORLD && bag.DraggingActor() == RplId.Invalid();
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestBagDrag(pOwnerEntity, false, false);
 }
 override bool HasLocalEffectOnlyScript() { return true; }
}
class BCMM_RopeDragBagAction : BCMM_DragBagAction
{
 override bool CanBeShownScript(IEntity user) { return BCMM_NativeBagDrag.HasRope(user) && super.CanBeShownScript(user); }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestBagDrag(pOwnerEntity, true, false);
 }
}
class BCMM_ReleaseBagAction : BCMM_DragBagAction
{
 override bool CanBeShownScript(IEntity user)
 {
  BCMM_BodyBagComponent bag = BCMM_BodyBagComponent.Cast(GetOwner().FindComponent(BCMM_BodyBagComponent));
  return bag && bag.DraggingActor() == BCMM_BodyBagComponent.Id(user);
 }
 override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player) player.BCMM_RequestBagDrag(pOwnerEntity, false, true);
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (!world.IsEditMode()) BCMM_NativeBagDrag.Update();
 }
}
