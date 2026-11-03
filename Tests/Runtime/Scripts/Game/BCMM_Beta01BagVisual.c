class BCMM_Beta01BagVisual
{
 protected static SCR_ManualCamera s_Camera;
 protected static string s_Label;
 protected static ref BCMM_BaggingRuntimeChecks s_Operation;
 protected static ref BCMM_DomainChecks s_Suite;
 static void Start() { s_Suite = new BCMM_DomainChecks(); s_Operation = new BCMM_BaggingRuntimeChecks(); s_Operation.Start(s_Suite); }
 static void Capture(IEntity bag, string label)
 {
  string enabled; if (!System.GetCLIParam("bcmmBeta01BagVisual", enabled) || !bag) return;
  vector target = bag.CoordToParent("-0.49 0.15 -0.19");
  vector low, high; bag.GetWorldBounds(low, high);
  TraceParam trace = new TraceParam(); trace.Start = target + "0 2 0"; trace.End = target - "0 3 0"; trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS; trace.LayerMask = EPhysicsLayerPresets.Projectile;
  array<IEntity> exclusions = {}; BCMM_NativeBagDrag.ExcludeHierarchy(bag, exclusions); trace.ExcludeArray = exclusions;
  float fraction = bag.GetWorld().TraceMove(trace);
  PrintFormat("BCMM_BAG_GROUND label=%1 origin=%2 visualMinimumY=%3 surfaceY=%4 velocity=%5", label, bag.GetOrigin(), low[1], trace.Start[1]-5*fraction, bag.GetPhysics().GetVelocity());
  vector origin = target + "-3 2 3.3";
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD;
  Math3D.AnglesToMatrix((target-origin).VectorToAngles(), spawn.Transform); spawn.Transform[3] = origin;
  if (s_Camera) delete s_Camera;
  s_Camera = SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera, GetGame().GetWorld(), spawn));
  if (!s_Camera) return;
  s_Camera.SetInputEnabled(false); s_Camera.SetVerticalFOV(55);
  GetGame().GetCameraManager().SetCamera(s_Camera); s_Camera.ApplyTransform(0); s_Label = label;
  GetGame().GetCallqueue().CallLater(Screenshot, 250);
 }
 protected static void Screenshot() { FileIO.MakeDirectory("$profile:screenshots"); System.MakeScreenshot("$profile:screenshots/" + s_Label); }
}
