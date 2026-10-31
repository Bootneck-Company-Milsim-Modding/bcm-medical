// Original severity-scaled game injury generation; all samples are supplied by authority.
class BCMM_TraumaGeneration
{
 static bool Apply(BCMM_Patient patient, int region, float severity, bool left, float chestSample, float tensionSample, float internalSample, float airwaySample, notnull BCMM_Settings settings)
 {
  if (!patient || patient.Terminal || region < 0 || region > 6 || !(severity > 0 && severity <= 1)) return false;
  if (!(chestSample >= 0 && chestSample <= 1) || !(tensionSample >= 0 && tensionSample <= 1) || !(internalSample >= 0 && internalSample <= 1) || !(airwaySample >= 0 && airwaySample <= 1)) return false;
  if (region == BCMM_BodyRegion.CHEST && chestSample < settings.Get("penetratingChestInjuryChance") * severity)
  {
   BCMM_ChestSide side = patient.RightChest;
   if (left) side = patient.LeftChest;
   side.OpenWound = true;
   // A new penetration defeats a previously applied seal.
   side.Sealed = false;
   side.CollapseFraction = Math.Clamp(side.CollapseFraction + severity * settings.Get("initialLungCollapseFraction"), 0, 1);
   if (tensionSample < settings.Get("initialTensionChance")) side.Tension = true;
  }
  if (internalSample < settings.Get("penetratingInternalBleedChance") * severity && settings.Get("internalBleedMaxMlPerSecond") > 0)
  {
   BCMM_BleedCompartment compartment = BCMM_BleedCompartment.SOFT_TISSUE;
   if (region == BCMM_BodyRegion.HEAD) compartment = BCMM_BleedCompartment.CRANIAL;
   if (region == BCMM_BodyRegion.ABDOMEN) compartment = BCMM_BleedCompartment.ABDOMEN;
   if (region == BCMM_BodyRegion.CHEST)
   {
    compartment = BCMM_BleedCompartment.RIGHT_PLEURAL;
    if (left) compartment = BCMM_BleedCompartment.LEFT_PLEURAL;
   }
   patient.AddInternalBleeding(region, compartment, severity * settings.Get("internalBleedMaxMlPerSecond"));
  }
  if (region == BCMM_BodyRegion.HEAD && airwaySample < settings.Get("headAirwayOcclusionChance") * severity)
   patient.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  return true;
 }
}
