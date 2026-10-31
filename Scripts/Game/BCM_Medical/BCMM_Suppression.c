// BCM visual-only suppression. Native muzzle/projectile hooks; no medical state changes.
class BCMM_SuppressionEnvelope
{
 float Strength;
 protected float m_Peak;
 protected float m_Age;
 void Pulse(float amount)
 {
  Strength = Math.Clamp(Strength + Math.Max(0, amount), 0, 1);
  m_Peak = Strength;
  m_Age = 0;
 }
 void Advance(float dt, float release)
 {
  m_Age += Math.Max(0, dt);
  Strength = m_Peak * Math.Max(0, 1 - m_Age / Math.Max(0.25, release));
 }
 void Reset() { Strength = 0; m_Peak = 0; m_Age = 0; }
 static vector ClosestPoint(vector start, vector end, vector point)
 {
  vector delta = end - start;
  float lengthSq = delta.LengthSq();
  if (lengthSq < 0.000001) return start;
  return start + delta * Math.Clamp(vector.Dot(point - start, delta) / lengthSq, 0, 1);
 }
}

class BCMM_SuppressionRound
{
 IEntity Entity;
 vector Previous;
 float Age;
}

class BCMM_SuppressionClient
{
 // Bound only by our own material. Medical blur uses a separate slot.
 static bool BlurEnabled;
 static float BlurAmount;
 protected static ref BCMM_SuppressionEnvelope s_Envelope = new BCMM_SuppressionEnvelope();
 protected static ref array<ref BCMM_SuppressionRound> s_Rounds = {};
 protected static ChimeraCharacter s_Character;
 protected static Widget s_Overlay;
 protected static int s_Camera = -1;
 static float Strength() { return s_Envelope.Strength; }
 static void Reset(bool newWorld = false)
 {
  s_Envelope.Reset();
  s_Rounds.Clear();
  BlurEnabled = false;
  BlurAmount = 0;
  if (s_Overlay) { s_Overlay.RemoveFromHierarchy(); s_Overlay = null; }
  s_Character = null;
  if (newWorld) s_Camera = -1;
 }
 protected static ChimeraCharacter LocalCharacter()
 {
  PlayerController pc = GetGame().GetPlayerController();
  if (!pc || GetGame().GetMenuManager().IsAnyMenuOpen()) return null;
  if (SCR_EditorManagerEntity.IsOpenedInstance()) return null;
  ChimeraCharacter character = ChimeraCharacter.Cast(pc.GetControlledEntity());
  if (!character || character.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || character.GetCharacterController().IsUnconscious()) return null;
  return character;
 }
 static void Register(IEntity projectile, BaseMuzzleComponent muzzle)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (!mode || !(mode.BCMM_ReadSetting("suppressionEnabled") > 0.5) || !projectile || !muzzle) return;
  ChimeraCharacter player = LocalCharacter();
  if (!player || !projectile.FindComponent(ProjectileMoveComponent)) return;
  IEntity shooter = muzzle.GetOwner().GetRootParent();
  if (shooter == player || vector.Distance(player.EyePosition(), muzzle.GetOwner().GetOrigin()) < 3) return;
  if (s_Rounds.Count() >= 128) s_Rounds.Remove(0);
  foreach (BCMM_SuppressionRound existing : s_Rounds) if (existing.Entity == projectile) return;
  BCMM_SuppressionRound round = new BCMM_SuppressionRound();
  round.Entity = projectile;
  round.Previous = projectile.GetOrigin();
  s_Rounds.Insert(round);
 }
 static void Update(float dt)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  ChimeraCharacter player = LocalCharacter();
  if (!mode || !(mode.BCMM_ReadSetting("suppressionEnabled") > 0.5) || !player)
  {
   if (s_Character || s_Overlay || s_Rounds.Count() > 0 || BlurEnabled) Reset();
   return;
  }
  if (s_Character && s_Character != player) Reset();
  s_Character = player;
  s_Envelope.Advance(dt, mode.BCMM_ReadSetting("suppressionReleaseSeconds"));
  vector eye = player.EyePosition();
  for (int i = s_Rounds.Count() - 1; i >= 0; i--)
  {
   BCMM_SuppressionRound round = s_Rounds[i];
   round.Age += dt;
   if (!round.Entity || round.Entity.IsDeleted() || round.Age > 4) { s_Rounds.Remove(i); continue; }
   vector position = round.Entity.GetOrigin();
   vector nearest = BCMM_SuppressionEnvelope.ClosestPoint(round.Previous, position, eye);
   float distance = vector.Distance(nearest, eye);
   round.Previous = position;
   if (distance > 6) continue;
   TraceParam trace = new TraceParam();
   trace.Start = eye;
   trace.End = nearest;
   trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  trace.LayerMask = EPhysicsLayerPresets.Projectile;
   array<IEntity> exclusions = {player, round.Entity};
   trace.ExcludeArray = exclusions;
   if (GetGame().GetWorld().TraceMove(trace) >= 0.99)
    s_Envelope.Pulse(Math.Lerp(0.5, 0.18, distance / 6));
   s_Rounds.Remove(i); // Each round can contribute only once, including blocked passes.
  }
  Apply(mode);
 }
 protected static void Apply(SCR_BaseGameMode mode)
 {
  BlurAmount = Math.Clamp(s_Envelope.Strength * mode.BCMM_ReadSetting("suppressionBlurPercent") * mode.BCMM_ReadSetting("suppressionBlurMultiplier") / 100, 0, 1);
  BlurEnabled = BlurAmount > 0.0001;
  int camera = GetGame().GetWorld().GetCurrentCameraId();
  if (BlurEnabled && camera != s_Camera)
  {
   GetGame().GetWorld().SetCameraPostProcessEffect(camera, 18, PostProcessEffectType.GaussFilter, "{6AB4B3F3FD215001}UI/Materials/BCMM_SuppressionBlur.emat");
   s_Camera = camera;
  }
  float opacity = Math.Clamp(s_Envelope.Strength * mode.BCMM_ReadSetting("suppressionVignettePercent") * mode.BCMM_ReadSetting("suppressionVignetteMultiplier") / 100, 0, 1);
  if (opacity > 0 && !s_Overlay)
   s_Overlay = GetGame().GetWorkspace().CreateWidgets("{6AB4B3F3FD215002}UI/layouts/BCMM/Suppression.layout");
  if (s_Overlay) s_Overlay.SetOpacity(opacity);
 }
}

modded class SCR_MuzzleEffectComponent
{
 override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
 {
  super.OnFired(effectEntity, muzzle, projectileEntity);
  BCMM_SuppressionClient.Register(projectileEntity, muzzle);
 }
}

modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (!world.IsEditMode()) BCMM_SuppressionClient.Update(timeslice);
 }
}
