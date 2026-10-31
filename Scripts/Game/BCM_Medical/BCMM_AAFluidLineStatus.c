// Original BCM replicated patient-owned fluid-line state and reset hook.
modded class SCR_CharacterDamageManagerComponent
{
 override void FullHeal(bool ignoreHealingDOT=true)
 {
  if(Replication.IsServer())
  {
   BCMM_FluidLineSession.StopForPatient(GetOwner());
   BCMM_SavedFluidLineBinding.CancelForPatient(GetOwner());
  }
  super.FullHeal(ignoreHealingDOT);
 }
 [RplProp()] protected bool m_BCMMFluidLineActive;
 [RplProp()] protected float m_BCMMFluidLineRemainingMl;
 [RplProp()] protected int m_BCMMFluidLineRegion;
 int BCMM_FluidLineRegion()
 {
  return m_BCMMFluidLineRegion;
 }
 bool BCMM_FluidLineActive()
 {
  return m_BCMMFluidLineActive;
 }
 float BCMM_FluidLineRemainingMl()
 {
  return m_BCMMFluidLineRemainingMl;
 }
 void BCMM_PublishFluidLine()
 {
  if(!Replication.IsServer())
  {
   return;
  }
  BCMM_FluidLineSession line=BCMM_FluidLineSession.ForPatient(GetOwner());
  m_BCMMFluidLineActive=line!=null;
  m_BCMMFluidLineRemainingMl=0;
  m_BCMMFluidLineRegion=-1;
  if(line)
  {
   m_BCMMFluidLineRemainingMl=line.RemainingMl();
   m_BCMMFluidLineRegion=line.AccessRegion();
  }
  Replication.BumpMe();
 }
}
