// Original normalized gameplay burden, source-independent clearance and native water gate.
class BCMM_FluidBurdenState
{
 protected float m_Load;
 bool Valid()
 {
  return m_Load>=0 && m_Load<=10;
 }
 float LoadFraction()
 {
  return m_Load;
 }
 bool Restore(float value)
 {
  if(!(value>=0 && value<=10))
  {
   return false;
  }
  m_Load=value;
  return true;
 }
 bool Advance(float seconds,float redistributed,float perfusion,notnull BCMM_Settings settings)
 {
  if(!Valid() || !(seconds>0 && seconds<=3600) || !(redistributed>=0 && redistributed<=10) || !(perfusion>=0 && perfusion<=1))
  {
   return false;
  }
  float timescale=settings.Get("fluidOverloadClearanceSeconds");
  if(!(timescale>0 && timescale<=86400))
  {
   return false;
  }
  if(m_Load==0 && redistributed==0)
  {
   return true;
  }
  float exposure=seconds*perfusion/timescale;
  float decay=Math.Pow(2.718281828,-exposure);
  float additionScale=1;
  if(exposure>0.001)
  {
   additionScale=(1-decay)/exposure;
  }
  else
  {
   // Stable small-exposure limit avoids cancellation in (1-exp(-x))/x.
   additionScale=1-exposure*0.5+exposure*exposure/6;
  }
  m_Load=Math.Clamp(m_Load*decay+redistributed*additionScale,0,10);
  return true;
 }
 float Severity(notnull BCMM_Settings settings)
 {
  if(!Valid() || settings.Get("fluidOverloadEnabled")<0.5 || settings.Get("fluidCompositionEnabled")<0.5)
  {
   return 0;
  }
  float onset=settings.Get("fluidOverloadThresholdFraction");
  float severe=settings.Get("fluidOverloadSevereFraction");
  if(!(onset>=0 && severe>onset))
  {
   return 0;
  }
  return Math.Clamp((m_Load-onset)/(severe-onset),0,1);
 }
 bool SerializationSave(SaveContext context)
 {
  return Valid() && context.WriteValue("schema",1) && context.WriteValue("load",m_Load);
 }
 bool SerializationLoad(LoadContext context)
 {
  int schema;
  float value;
  if(!context.ReadValue("schema",schema) || schema!=1 || !context.ReadValue("load",value))
  {
   return false;
  }
  return Restore(value);
 }
}
modded class BCMM_Patient
{
 protected ref BCMM_FluidBurdenState m_BCMMFluidBurden=new BCMM_FluidBurdenState();
 void BCMM_AdvanceFluidBurden(float seconds,float redistributed,float perfusion,notnull BCMM_Settings settings)
 {
  float newLoad=0;
  if(settings.Get("fluidOverloadEnabled")>0.5 && settings.Get("fluidCompositionEnabled")>0.5)
  {
   newLoad=redistributed;
  }
  if(newLoad==0 && m_BCMMFluidBurden.LoadFraction()==0)
  {
   return;
  }
  m_BCMMFluidBurden.Advance(seconds,newLoad,perfusion,settings);
 }
 float BCMM_FluidBurdenSeverity(notnull BCMM_Settings settings)
 {
  return m_BCMMFluidBurden.Severity(settings);
 }
 float BCMM_FluidBurdenFraction()
 {
  return m_BCMMFluidBurden.LoadFraction();
 }
 BCMM_FluidBurdenState BCMM_CaptureFluidBurden()
 {
  if(!m_BCMMFluidBurden.Valid())
  {
   return null;
  }
  BCMM_FluidBurdenState state=new BCMM_FluidBurdenState();
  state.Restore(m_BCMMFluidBurden.LoadFraction());
  return state;
 }
 bool BCMM_RestoreFluidBurden(BCMM_FluidBurdenState state)
 {
  if(!state || !state.Valid())
  {
   return false;
  }
  return m_BCMMFluidBurden.Restore(state.LoadFraction());
 }
 bool BCMM_WithholdAdditionalWater(int kind,notnull BCMM_Settings settings)
 {
  if(kind==BCMM_FluidKind.WHOLE_BLOOD || settings.Get("fluidOverloadEnabled")<0.5 || settings.Get("fluidCompositionEnabled")<0.5 || settings.Get("fluidSafetyChecksEnabled")<0.5 || settings.Get("fluidOverloadStopAdditionalWater")<0.5)
  {
   return false;
  }
  float severity=BCMM_FluidBurdenSeverity(settings);
  return severity>0 && severity>=settings.Get("fluidOverloadStopSeverity");
 }
}
// Early declaration keeps the replicated native-consumable gate visible to other extensions.
modded class SCR_CharacterDamageManagerComponent
{
 [RplProp()] protected bool m_BCMMWaterLoadingWithheld;
 bool BCMM_WaterLoadingWithheld()
 {
  return m_BCMMWaterLoadingWithheld;
 }
 void BCMM_SetWaterLoadingWithheld(bool withheld)
 {
  if(!Replication.IsServer() || m_BCMMWaterLoadingWithheld==withheld)
  {
   return;
  }
  m_BCMMWaterLoadingWithheld=withheld;
  Replication.BumpMe();
 }
 override void FullHeal(bool ignoreHealingDOT=true)
 {
  BCMM_SetWaterLoadingWithheld(false);
  super.FullHeal(ignoreHealingDOT);
 }
}
