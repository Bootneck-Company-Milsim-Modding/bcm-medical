// Actual local native controller, held-tool item-use and full thirty-second dig.
// This complements, but does not replace, dedicated owner/observer replication.
modded class SCR_PlayerController
{
 int BCMM_TestDiggingToken() { return m_BCMMBurialAnimationToken; }
}
class BCMM_BurialOwnerChecks:BCMM_BurialRuntimeChecks
{
 override void Start(BCMM_DomainChecks suite)
 {
  super.Start(suite);
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  suite.Record(player!=null,"native burial animation fixture has actual local player controller");
  if(player) player.SetControlledEntity(m_Actor);
 }
 override protected void BeginFullBurial()
 {
  super.BeginFullBurial();
  GetGame().GetCallqueue().CallLater(VerifyNativeDigging,2000,false);
 }
 protected void VerifyNativeDigging()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  CharacterControllerComponent controller=ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  m_Suite.Record(player && player.GetControlledEntity()==m_Actor && player.BCMM_TestDiggingToken()==m_RemainsToken,"original local owner retains exact active burial animation token");
  m_Suite.Record(controller.IsUsingItem(),"native entrenching item-use command actually runs during timed burial");
 }
 override protected void VerifyBuried()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  m_Suite.Record(player && player.BCMM_TestDiggingToken()==0,"successful burial clears original owner digging animation");
  super.VerifyBuried();
 }
}
