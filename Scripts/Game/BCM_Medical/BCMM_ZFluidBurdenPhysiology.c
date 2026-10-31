// Late extension preserves visibility of the original Enforce physiology methods.
modded class BCMM_Patient
{
 override float LungCapacity(notnull BCMM_Settings settings)
 {
  float originalCapacity=super.LungCapacity(settings);
  float severity=BCMM_FluidBurdenSeverity(settings);
  if(severity<=0)
  {
   return originalCapacity;
  }
  float multiplier=1-severity*(1-settings.Get("fluidOverloadMinimumLungFraction"));
  return originalCapacity*Math.Clamp(multiplier,0,1);
 }
}
