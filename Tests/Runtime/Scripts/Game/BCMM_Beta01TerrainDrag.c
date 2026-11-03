// Six native terrain cases: manual and rope movement at three real grass sites.
class BCMM_Beta01TerrainDrag : BCMM_DragRuntimeChecks
{
 protected int m_Case;
 protected vector m_Site;
 protected vector m_Normal;
 override void Start(BCMM_DomainChecks suite) { m_Suite=suite; NextSite(); }
 protected void NextSite()
 {
  if(m_Case==6){Print("BCMM_TERRAIN_DRAG_COMPLETE cases=6");m_Suite.Complete();return;}
  BaseWorld world=GetGame().GetWorld();bool found;
  int site=m_Case%3;
  for(int x=400+site*600;x<1000+site*600 && !found;x+=80)
   for(int z=400;z<2800 && !found;z+=80)
   {
    float height=world.GetSurfaceY(x,z);if(height<2)continue;
    TraceParam probe=new TraceParam();probe.Start=Vector(x,height+1,z);probe.End=Vector(x,height-1,z);probe.Flags=TraceFlags.WORLD;
    if(world.TraceMove(probe)>=1 || probe.TraceNorm[1]<0.97)continue;
    string material=probe.TraceMaterial;material.ToLower();if(material.IndexOf("grass")<0)continue;
    if(Math.AbsFloat(world.GetSurfaceY(x,z+4)-height)>0.7)continue;
    m_Site=Vector(x,height,z);m_Normal=probe.TraceNorm;found=true;
   }
  m_Suite.Record(found,"Terrain drag locates actual grass collision support without a synthetic deck");
  if(!found){m_Suite.Complete();return;}
  m_Bag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et",m_Site+"0 0.3 0");
  if(m_Case==0)
  {
   array<int> layers={};SCR_Enum.GetEnumValues(EPhysicsLayerDefs,layers);
   foreach(int layer:layers)PrintFormat("BCMM_COLLISION_LAYER name=%1 value=%2",SCR_Enum.GetEnumName(EPhysicsLayerDefs,layer),layer);
   Physics physics=m_Bag.GetPhysics();
   PrintFormat("BCMM_BAG_COLLISION body=%1 response=%2 geoms=%3",physics.GetInteractionLayer(),physics.GetResponseIndex(),physics.GetNumGeoms());
   for(int geom=0;geom<physics.GetNumGeoms();geom++)PrintFormat("BCMM_BAG_COLLISION geom=%1 layer=%2",geom,physics.GetGeomInteractionLayer(geom));
  }
  vector actorPosition=m_Site+"0 0.3 1.2";actorPosition[1]=world.GetSurfaceY(actorPosition[0],actorPosition[2])+0.2;
  m_Actor=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",actorPosition);
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",actorPosition);
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Terrain actor owns a native backpack");
  GetGame().GetCallqueue().CallLater(AddTerrainRope,600);
 }
 protected void AddTerrainRope()
 {
  m_Rope=Spawn("{6AB4B3F3FD216007}Prefabs/Items/BCMM_DragRope.et",m_Actor.GetOrigin());
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(inventory.TryInsertItem(m_Rope,EStoragePurpose.PURPOSE_ANY),"Terrain actor owns the original rope supply");
  GetGame().GetCallqueue().CallLater(BeginTerrainDrag,1400);
 }
 protected void BeginTerrainDrag()
 {
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));m_Start=m_Bag.GetOrigin();
  m_Suite.Record(BCMM_NativeBagDrag.Begin(m_Actor,m_Bag,m_Case>=3,state.Revision()),"Native terrain drag acquires its reservation");
  vector position=m_Site+"0 0 3";position[1]=GetGame().GetWorld().GetSurfaceY(position[0],position[2])+0.1;
  m_Actor.SetOrigin(position);
  GetGame().GetCallqueue().CallLater(CheckTerrainDrag,2500);
 }
 protected void CheckTerrainDrag()
 {
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  vector frame[4];m_Bag.GetWorldTransform(frame);
  float moved=vector.DistanceXZ(m_Start,m_Bag.GetOrigin());float alignment=vector.Dot(frame[1],m_Normal);
  PrintFormat("BCMM_TERRAIN_DRAG case=%1 site=%2 moved=%3 alignment=%4 mode=%5",m_Case,m_Site,moved,alignment,state.DragMode());
  m_Suite.Record(moved>0.25 && state.DragMode()!=BCMM_TransportMode.NONE,"Real terrain permits physical movement and retains the correct drag session");
  m_Suite.Record(alignment>0.95,"Bag follows the real terrain normal");
  m_Suite.Record(BCMM_NativeBagDrag.Release(m_Actor,m_Bag) && !BCMM_NativeBagDrag.FindBag(m_Bag),"Terrain release removes the server reservation");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Bag);SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Case++;GetGame().GetCallqueue().CallLater(NextSite,300);
 }
}
