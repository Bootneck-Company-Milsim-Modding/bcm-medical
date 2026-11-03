// Real rigid-body movement on six independent flat/sloped collision decks.
// Actor positioning is scripted; this complements the owner-input network test.
class BCMM_Beta01DragMatrix : BCMM_DragRuntimeChecks
{
 protected int m_Case;
 protected vector m_SurfaceNormal;
 protected IEntity m_Obstacle;
 override void Start(BCMM_DomainChecks suite) { m_Suite = suite; NextCase(); }
 protected void NextCase()
 {
  if (m_Case >= 8) { m_Suite.Complete(); return; }

  int slopeIndex = m_Case % 3;
  float slope = (slopeIndex - 1)*10;
  if (m_Case >= 6) slope = 0;
  vector frame[4]; Math3D.AnglesToMatrix(Vector(0, slope, 0), frame); frame[3] = "100 3 0";
  EntitySpawnParams floorSpawn=new EntitySpawnParams();floorSpawn.TransformMode=ETransformMode.WORLD;
  for(int axis=0;axis<4;axis++)floorSpawn.Transform[axis]=frame[axis];
  m_Floor=GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et"),GetGame().GetWorld(),floorSpawn);m_SurfaceNormal=frame[1];
  m_Bag = Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et", m_Floor.CoordToParent("0 0.2 -2"));
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", m_Floor.CoordToParent("0 0.3 -0.8"));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Slope test equips native backpack");
  GetGame().GetCallqueue().CallLater(InsertRope, 700);
 }
 protected void InsertRope()
 {
  m_Rope = Spawn("{6AB4B3F3FD216007}Prefabs/Items/BCMM_DragRope.et", m_Actor.GetOrigin());
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(inventory.TryInsertItem(m_Rope, EStoragePurpose.PURPOSE_ANY), "Slope test inserts original rope");
  GetGame().GetCallqueue().CallLater(StartFollow, 1300);
 }
 protected void StartFollow()
 {
  if (m_Case >= 6)
  {

   vector wall[4]; Math3D.AnglesToMatrix("0 90 0", wall); wall[3] = "100 3 -1.3";
   EntitySpawnParams wallSpawn=new EntitySpawnParams();wallSpawn.TransformMode=ETransformMode.WORLD;
   for(int axis=0;axis<4;axis++)wallSpawn.Transform[axis]=wall[axis];
   m_Obstacle=GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et"),GetGame().GetWorld(),wallSpawn);
   vector wallMin,wallMax;m_Obstacle.GetWorldBounds(wallMin,wallMax);PrintFormat("BCMM_WALL_BOUNDS min=%1 max=%2 mask=%3 geoms=%4",wallMin,wallMax,m_Obstacle.GetPhysics().GetInteractionLayer(),m_Obstacle.GetPhysics().GetNumGeoms());
   m_Suite.Record(wallMax[1]-wallMin[1]>10 && wallMax[2]-wallMin[2]<0.3,"Obstacle collision geometry is vertical before drag begins");
  }
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Start = m_Bag.GetOrigin();
  if(m_Case>=6)PrintFormat("BCMM_WALL_START case=%1 bag=%2 actor=%3",m_Case,m_Start,m_Actor.GetOrigin());
  bool rope = m_Case >= 3 && m_Case != 6;
  m_Suite.Record(BCMM_NativeBagDrag.Begin(m_Actor, m_Bag, rope, state.Revision()), "Surface test begins real drag session");
  m_Actor.SetOrigin(m_Floor.CoordToParent("0 0.1 1"));
  GetGame().GetCallqueue().CallLater(CheckFollow, 2000);
 }
 protected void CheckFollow()
 {
  BCMM_BodyBagComponent state = BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  vector frame[4]; m_Bag.GetWorldTransform(frame);
  float alignment = frame[1][0]*m_SurfaceNormal[0] + frame[1][1]*m_SurfaceNormal[1] + frame[1][2]*m_SurfaceNormal[2];
  float moved = vector.Distance(m_Bag.GetOrigin(), m_Start);
  PrintFormat("BCMM_DRAG_MATRIX case=%1 rope=%2 moved=%3 alignment=%4 mode=%5", m_Case, m_Case >= 3, moved, alignment, state.DragMode());
  if(m_Case>=6)PrintFormat("BCMM_WALL_END bag=%1 actor=%2",m_Bag.GetOrigin(),m_Actor.GetOrigin());
  if (m_Case < 6)
  {
   m_Suite.Record(moved > 0.25 && state.DragMode() != BCMM_TransportMode.NONE, "Bag moves and remains attached on supporting slope");
   m_Suite.Record(alignment > 0.95, "Bag orientation follows supporting slope");
   m_Suite.Record(BCMM_NativeBagDrag.Release(m_Actor, m_Bag), "Slope drag explicitly releases");
  }
  else
  {
   m_Suite.Record(state.DragMode()==BCMM_TransportMode.NONE,"Actual solid wall cancels the drag reservation");
   m_Suite.Record(m_Bag.GetOrigin()[2] < -1.3, "Released bag remains on original side of solid obstacle");
   if (state.DragMode() != BCMM_TransportMode.NONE) m_Suite.Record(BCMM_NativeBagDrag.Release(m_Actor, m_Bag), "Owner can release a physically blocked bag");
   m_Suite.Record(!BCMM_NativeBagDrag.FindBag(m_Bag), "Blocked bag has no stale reservation after release");
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Bag); SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  if (m_Rope) SCR_EntityHelper.DeleteEntityAndChildren(m_Rope);
  if (m_Obstacle) delete m_Obstacle;
  delete m_Floor; m_Case++;
  GetGame().GetCallqueue().CallLater(NextCase, 300);
 }
}
