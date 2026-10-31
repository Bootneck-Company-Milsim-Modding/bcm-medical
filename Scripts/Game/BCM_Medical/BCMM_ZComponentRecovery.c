// Original opt-in game recovery. Never restores missing native blood volume.
modded class BCMM_FluidBalance
{
 void BCMM_RecoverComposition(float seconds,float volume,float redSeconds,float clotSeconds)
 {
  if(!(seconds>0 && seconds<=3600) || !(volume>=0 && volume<=1) || !(redSeconds>0 && redSeconds<=86400) || !(clotSeconds>0 && clotSeconds<=86400))return;
  if(!(RedCellReserve>=0 && RedCellReserve<=1) || !(ClottingReserve>=0 && ClottingReserve<=1) || !(LastVolume>=0 && LastVolume<=1) || !(TransientWater>=0 && TransientWater<=1))return;
  ObserveBlood(volume);
  if(RedCellReserve<volume)RedCellReserve=Math.Min(volume,RedCellReserve+(volume-RedCellReserve)*(1-Math.Pow(2.718281828,-seconds/redSeconds)));
  if(ClottingReserve<volume)ClottingReserve=Math.Min(volume,ClottingReserve+(volume-ClottingReserve)*(1-Math.Pow(2.718281828,-seconds/clotSeconds)));
 }
}
modded class BCMM_Patient
{
 bool BCMM_CanRecoverComponents(float volume,bool facility,notnull BCMM_Settings settings)
 {
  if(settings.Get("componentRecoveryEnabled")<0.5 || settings.Get("fluidCompositionEnabled")<0.5 || Terminal || !HasSpontaneousCirculation)return false;
  if(!(volume>=settings.Get("componentRecoveryMinimumBloodFraction") && volume<=1) || !(RespiratoryRate>0 && RespiratoryRate<=200) || !(OxygenSaturation>=settings.Get("unconsciousOxygenFraction") && OxygenSaturation<=1))return false;
  if(!(SystolicPressure>=settings.Get("componentRecoveryMinimumSystolicPressure") && SystolicPressure<=1000) || LeftChest.Tension || RightChest.Tension)return false;
  if(settings.Get("componentRecoveryRequiresFacility")>0.5 && !facility)return false;
  if(TotalExternalFlowMlPerSecond()>0.00001)return false;
  for(int index=0;index<InternalBleedCount();index++)
  {
   BCMM_InternalBleeding source=InternalBleedAt(index);
   if(!source.Repaired && source.RateMlPerSecond>0.00001)return false;
  }
  return true;
 }
 void BCMM_AdvanceComponentRecovery(float seconds,float volume,bool facility,notnull BCMM_Settings settings)
 {
  if(!BCMM_CanRecoverComponents(volume,facility,settings))return;
  Fluids.BCMM_RecoverComposition(seconds,volume,settings.Get("redCellRecoverySeconds"),settings.Get("clottingRecoverySeconds"));
 }
}
