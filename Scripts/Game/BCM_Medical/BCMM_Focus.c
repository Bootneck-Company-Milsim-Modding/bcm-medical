// Extra player focus only. Weapon optics, binocular magnification and ADS
// alignment remain native. Timing belongs to the persistent player controller.
class BCMM_FocusBudget
{
 protected bool m_Active;
 protected bool m_RequireRelease;
 protected float m_Started;
 protected float m_LastEnded = -1000000;
 float Apply(float requested, float now, float duration, float cooldown)
 {
  if (requested <= 0)
  {
   if (m_Active)
   {
    float ended = now;
    if (duration > 0 && now - m_Started >= duration) ended = m_Started + duration;
    m_LastEnded = ended;
   }
   m_Active = false; m_RequireRelease = false;
   return 0;
  }
  if (!m_Active)
  {
   if (m_RequireRelease) return 0;
   if (now < m_LastEnded + cooldown) { m_RequireRelease = true; return 0; }
   m_Active = true; m_Started = now;
  }
  if (duration > 0 && now - m_Started >= duration)
  {
   m_Active = false; m_RequireRelease = true; m_LastEnded = m_Started + duration;
   return 0;
  }
  return requested;
 }
}
modded class SCR_PlayerController
{
 protected ref BCMM_FocusBudget m_BCMMFocusBudget = new BCMM_FocusBudget();
 void BCMM_UpdateFocusBudget()
 {
  ChimeraCharacter body = ChimeraCharacter.Cast(GetControlledEntity());
  if (body && body.GetCharacterController().GetLifeState() == ECharacterLifeState.ALIVE && !body.GetCharacterController().IsUnconscious()) return;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) m_BCMMFocusBudget.Apply(0, GetGame().GetWorld().GetWorldTime() * 0.001, mode.BCMM_ReadSetting("focusDurationSeconds"), mode.BCMM_ReadSetting("focusCooldownSeconds"));
 }
 override float GetFocusValue(float adsProgress, float dt)
 {
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (!mode || !(mode.BCMM_ReadSetting("focusEnabled") > 0.5)) return super.GetFocusValue(adsProgress, dt);
  // Calling native once with no automatic progress resolves all native manual
  // sources (hold, toggle, controller analogue, vehicle), including their resets.
  float manual = super.GetFocusValue(0, dt);
  float automatic;
  if (adsProgress > 0)
  {
   automatic = s_fADSFocus;
   if (SCR_2DPIPSightsComponent.IsPIPActive()) automatic = Math.Lerp(s_fADSFocus, 1, s_fPIPFocus);
   automatic *= Math.Min(adsProgress, 1);
   if (m_CharacterController && m_CharacterController.IsFreeLookEnabled())
   {
    CharacterHeadAimingComponent aiming = m_CharacterController.GetHeadAimingComponent();
    if (aiming) automatic *= Math.Clamp(1 - Math.InverseLerp(1, 6, aiming.GetAimingRotation().Length()), 0, 1);
   }
  }
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  ChimeraCharacter body = ChimeraCharacter.Cast(GetControlledEntity());
  if (!body || body.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || body.GetCharacterController().IsUnconscious()) manual = 0;
  float allowed = m_BCMMFocusBudget.Apply(manual, now, mode.BCMM_ReadSetting("focusDurationSeconds"), mode.BCMM_ReadSetting("focusCooldownSeconds"));
  return Math.Clamp(automatic + Math.Max(0, allowed - automatic) * mode.BCMM_ReadSetting("focusStrengthPercent") * 0.01, 0, 1);
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  SCR_PlayerController pc = SCR_PlayerController.Cast(GetPlayerController());
  if (pc && !world.IsEditMode()) pc.BCMM_UpdateFocusBudget();
 }
}
