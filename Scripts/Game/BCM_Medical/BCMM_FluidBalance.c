// Original BCM normalised fluid composition. Fractions describe game reserves, not lab values.
enum BCMM_FluidKind { SALINE, PLASMA, WHOLE_BLOOD }
class BCMM_FluidBalance
{
 float RedCellReserve=1;
 float ClottingReserve=1;
 float LastVolume=1;
 float TransientWater;
 void ObserveBlood(float volume)
 {
  volume=Math.Clamp(volume,0,1);
  if(volume<LastVolume)
  {
   float ratio=0;if(LastVolume>0.00001) { ratio=volume/LastVolume; }
   RedCellReserve*=ratio;ClottingReserve*=ratio;TransientWater*=ratio;
  }
  else if(volume>LastVolume)
  {
   // Explicit typed deliveries update LastVolume themselves. Other increases are native recovery.
   RedCellReserve=Math.Min(1,RedCellReserve+volume-LastVolume);ClottingReserve=Math.Min(1,ClottingReserve+volume-LastVolume);
  }
  LastVolume=volume;
 }
 void RecordDelivery(int kind,float before,float after,notnull BCMM_Settings settings)
 {
  ObserveBlood(before);float delivered=Math.Max(0,after-before);
  if(settings.Get("fluidCompositionEnabled")<0.5) { kind=BCMM_FluidKind.WHOLE_BLOOD; }
  if(kind==BCMM_FluidKind.WHOLE_BLOOD) { RedCellReserve=Math.Min(1,RedCellReserve+delivered); }
  if(kind==BCMM_FluidKind.WHOLE_BLOOD || kind==BCMM_FluidKind.PLASMA) { ClottingReserve=Math.Min(1,ClottingReserve+delivered); }
  if(kind==BCMM_FluidKind.SALINE) { TransientWater+=delivered*(1-settings.Get("salineRetainedFraction")); }
  if(kind==BCMM_FluidKind.PLASMA) { TransientWater+=delivered*(1-settings.Get("plasmaRetainedFraction")); }
  LastVolume=after;
 }
 float RedCellConcentration(notnull BCMM_Settings settings)
 {
  if(settings.Get("fluidCompositionEnabled")<0.5) { return 1; }
  return Math.Clamp(RedCellReserve/Math.Max(0.00001,LastVolume),0,1);
 }
 float ClottingConcentration(notnull BCMM_Settings settings)
 {
  if(settings.Get("fluidCompositionEnabled")<0.5) { return 1; }
  return Math.Clamp(ClottingReserve/Math.Max(0.00001,LastVolume),0,1);
 }
 float Redistribute(float seconds,float volume,notnull BCMM_Settings settings)
 {
  if(settings.Get("fluidCompositionEnabled")<0.5 || !(seconds>0)) { return 0; }
  float removed=Math.Min(volume,TransientWater*Math.Clamp(seconds/settings.Get("fluidRedistributionSeconds"),0,1));
  TransientWater=Math.Max(0,TransientWater-removed);LastVolume=Math.Max(0,volume-removed);
  // Water redistribution does not discard red cells or invent a haemorrhage.
  return removed;
 }
 float BleedingMultiplier(notnull BCMM_Settings settings) { return 1+(1-ClottingConcentration(settings))*settings.Get("fluidDilutionBleedMultiplier"); }
 float DressingWearMultiplier(notnull BCMM_Settings settings) { return 1+(1-ClottingConcentration(settings))*settings.Get("fluidDilutionWearMultiplier"); }
}