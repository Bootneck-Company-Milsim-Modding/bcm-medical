modded class SCR_BaseGameMode
{
 override void OnGameStart()
 {
  super.OnGameStart();
  BCMM_SuppressionClient.Reset(true);
  BCMM_CPRSession.Reset();
  BCMM_CPRAnimation.Reset();
  BCMM_SplintTreatment.Reset();
  BCMM_MedicalInput.Reset();
  BCMM_ClinicalCheckSession.Reset();
  BCMM_BandageAnimationSession.Reset();
  BCMM_ProcedureAnimation.Reset();
  BCMM_NativeBagDrag.Reset();
  BCMM_BagRopeVisual.Reset();
  BCMM_LivingTransport.Reset();
  BCMM_WeaponRetention.Reset();
 }
}
