// Local cosmetic geometry follows replicated drag ownership. No physics,
// inventory, animation or server authority is attached to the visible line.
class BCMM_BagRopeVisual
{
 protected static ref array<ref BCMM_BagRopeVisual> s_Lines = {};
 protected IEntity m_Bag;
 protected ref array<IEntity> m_Segments = {null,null,null,null};
 protected void Clear()
 {
  foreach (IEntity segment : m_Segments) if (segment) SCR_EntityHelper.DeleteEntityAndChildren(segment);
  m_Segments = {null,null,null,null};
 }
 static void Reset()
 {
  foreach (BCMM_BagRopeVisual line : s_Lines) line.Clear();
  s_Lines.Clear();
 }
 protected bool UpdateLine()
 {
  if (!m_Bag) return false;
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  if (!state || state.DragMode() != BCMM_TransportMode.BODY_BAG_ROPE_DRAG) return false;
  IEntity actor = BCMM_BodyBagComponent.Resolve(state.DraggingActor());
  if (!actor) return false;
  vector from = actor.CoordToParent("0.15 0.85 -0.12");
  vector to = m_Bag.CoordToParent("-0.49 0.15 0.95");
  float span = vector.Distance(from, to);
  if (span > BCMM_TreatmentAccess.Setting("ropeBreakDistanceMetres") + 1) return false;
  float sag = Math.Clamp((BCMM_TreatmentAccess.Setting("ropeLengthMetres") - span) * 0.2, 0.02, 0.25);
  for (int i = 0; i < 4; i++)
  {
   float a = i / 4.0; float b = (i + 1) / 4.0;
   vector start = from + (to - from) * a - Vector(0, 4 * sag * a * (1-a), 0);
   vector end = from + (to - from) * b - Vector(0, 4 * sag * b * (1-b), 0);
   vector delta = end - start; float length = delta.Length();
   if (length < 0.001) continue;
   vector forward = delta / length;
   vector right = BCMM_GripMath.Cross("0 1 0", forward);
   if (right.LengthSq() < 0.001) right = "1 0 0";
   right.Normalize();
   vector frame[4]; frame[0] = right; frame[1] = BCMM_GripMath.Cross(forward, right); frame[2] = forward; frame[3] = (start + end) * 0.5;
   if (!m_Segments[i])
   {
    EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD;
    for (int row = 0; row < 4; row++) spawn.Transform[row] = frame[row];
    m_Segments[i] = GetGame().SpawnEntityPrefab(Resource.Load("{87B20356EED46BB5}Prefabs/Medical/Applied/BCMM_DragRopeLine.et"), GetGame().GetWorld(), spawn);
   }
   // Entity transforms support uniform scale. Nonuniform basis scaling made
   // native rendering split the short segments away from their anchors.
   if (m_Segments[i]) { m_Segments[i].SetWorldTransform(frame); m_Segments[i].SetScale(length); m_Segments[i].Update(); }
  }
  return true;
 }
 static void Update()
 {
  if (!GetGame().GetPlayerController() || !GetGame().GetWorkspace()) return;
  foreach (BCMM_BodyBagComponent state : BCMM_BodyBagComponent.Instances)
  {
   if (!state || state.DragMode() != BCMM_TransportMode.BODY_BAG_ROPE_DRAG) continue;
   bool found = false;
   foreach (BCMM_BagRopeVisual line : s_Lines) if (line.m_Bag == state.GetOwner()) found = true;
   if (!found) { BCMM_BagRopeVisual line = new BCMM_BagRopeVisual(); line.m_Bag = state.GetOwner(); s_Lines.Insert(line); }
  }
  for (int i = s_Lines.Count() - 1; i >= 0; i--)
   if (!s_Lines[i].UpdateLine()) { s_Lines[i].Clear(); s_Lines.Remove(i); }
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (!world.IsEditMode()) BCMM_BagRopeVisual.Update();
 }
}
