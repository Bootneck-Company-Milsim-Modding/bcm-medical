// Actual saved-world fixture produced before the optional burden serializer existed.
class BCMM_FluidBurdenLegacyWorldChecks : BCMM_FluidLineRestartRuntimeChecks
{
 override protected void Prepare()
 {
  super.Prepare();
  if(!m_Medical){return;}
  m_Suite.Record(m_Medical.BCMM_GetPatient().BCMM_FluidBurdenFraction()==0 && !m_Medical.BCMM_WaterLoadingWithheld() && BCMM_TreatmentAccess.Setting("fluidOverloadEnabled")==0,"An actual pre-overload native world restores its original patient/line with zero optional burden and conservative defaults");
 }
}
