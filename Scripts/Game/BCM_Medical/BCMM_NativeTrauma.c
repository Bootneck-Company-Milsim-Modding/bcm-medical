// Native effects remain the only writer for external blood loss. BCM owns wound
// persistence/reopening; the native DOT receives their current combined rate.
class BCMM_NativeWound
{
 SCR_CharacterHitZone Zone;
 int WoundId;
}
modded class SCR_BleedingDamageEffect
{
 protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
  SCR_CharacterHitZone zone = SCR_CharacterHitZone.Cast(GetAffectedHitZone());
  if (medical && medical.BCMM_IsAuthority() && zone) SetDPS(medical.BCMM_NativeBleedingDPS(zone, GetDPS()));
  super.EOnFrame(timeSlice, dmgManager);
 }
}