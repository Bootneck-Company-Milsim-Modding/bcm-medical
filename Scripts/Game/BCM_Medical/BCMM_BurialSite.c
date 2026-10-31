// Native tool/material contract, independent of faction, item names and ACE.
class BCMM_BurialSite
{
 static float AuthoredGraveScale()
 {
  Resource resource=Resource.Load("{6AB4B3F3FD21E001}Prefabs/Graves/BCMM_Grave.et");
  if(!resource || !resource.IsValid()) return 0;
  IEntitySource source=resource.GetResource().ToEntitySource();float scale;
  if(source && source.Get("scale",scale)) return scale;
  return 0;
 }
 static IEntity HeldTool(IEntity actor)
 {
  if(!actor) return null;
  SCR_GadgetManagerComponent gadgets=SCR_GadgetManagerComponent.GetGadgetManager(actor);
  if(!gadgets) return null;
  IEntity held=gadgets.GetHeldGadget();
  if(held && held.FindComponent(SCR_CampaignBuildingGadgetToolComponent)) return held;
  return null;
 }
 static bool NaturalMaterial(string material)
 {
  material.ToLower();
  array<string> hard={"asphalt","tarmac","concrete","pavement","sidewalk","gravel","rock","stone","metal","wood","brick","tile"};
  foreach(string blocked:hard) if(material.IndexOf(blocked)>=0) return false;
  array<string> natural={"dirt","grass","soil","earth","mud","clay","forest","meadow","field"};
  foreach(string allowed:natural) if(material.IndexOf(allowed)>=0) return true;
  return false;
 }
 static bool Trace(IEntity bag,IEntity actor,out vector position,out vector normal)
 {
  if(!bag || !bag.GetPhysics() || bag.GetPhysics().GetVelocity().Length()>0.2) return false;
  BaseWorld world=bag.GetWorld();vector origin=bag.GetOrigin();
  float terrain=world.GetSurfaceY(origin[0],origin[2]);
  if(Math.AbsFloat(origin[1]-terrain)>1.5) return false;
  TraceParam support=new TraceParam();support.Start=origin+"0 1 0";support.End=Vector(origin[0],terrain-0.5,origin[2]);support.Flags=TraceFlags.ENTS|TraceFlags.WORLD;
  array<IEntity> exclusions={bag};if(actor)exclusions.Insert(actor);
  IEntity held=HeldTool(actor);if(held)exclusions.Insert(held);
  support.ExcludeArray=exclusions;
  if(world.TraceMove(support)>=1) return false;
  string material=support.TraceMaterial;if(support.SurfaceProps) material+=" "+support.SurfaceProps.GetName();
  if(!NaturalMaterial(material)) return false;
  TraceParam ground=new TraceParam();ground.Start=origin+"0 1 0";ground.End=Vector(origin[0],terrain-0.5,origin[2]);ground.Flags=TraceFlags.WORLD;
  float fraction=world.TraceMove(ground);
  if(fraction>=1 || ground.TraceNorm.LengthSq()<0.0001) return false;
  position=vector.Lerp(ground.Start,ground.End,fraction);normal=ground.TraceNorm;
  if(normal[1]<0)normal*=-1;
  normal.Normalize();
  return true;
 }
 static void AlignGrave(IEntity grave,IEntity bag)
 {
  vector minimum;vector maximum;bag.GetWorldBounds(minimum,maximum);vector bagCenter=(minimum+maximum)*0.5;
  for(int pass=0;pass<2;pass++)
  {
   grave.GetWorldBounds(minimum,maximum);vector graveCenter=(minimum+maximum)*0.5;
   vector transform[4];grave.GetWorldTransform(transform);
   transform[3][0]=transform[3][0]+bagCenter[0]-graveCenter[0];transform[3][2]=transform[3][2]+bagCenter[2]-graveCenter[2];
   transform[3][1]=grave.GetWorld().GetSurfaceY(transform[3][0],transform[3][2])-0.1;
   grave.SetWorldTransform(transform);grave.Update();
  }
 }
}
