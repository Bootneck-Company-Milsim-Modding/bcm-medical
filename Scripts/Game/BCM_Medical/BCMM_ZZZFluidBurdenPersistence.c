// Original optional burden object; existing clinical schema1 remains compatible.
modded class BCMM_CharacterClinicalSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  ESerializeResult nativeResult=super.Serialize(owner,component,context);
  if(nativeResult==ESerializeResult.ERROR)
  {
   return nativeResult;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(component);
  BCMM_FluidBurdenState state=medical.BCMM_GetPatient().BCMM_CaptureFluidBurden();
  if(!state)
  {
   return ESerializeResult.ERROR;
  }
  if(state.LoadFraction()>0 && !context.WriteValue("bcmmFluidBurden",state))
  {
   return ESerializeResult.ERROR;
  }
  return nativeResult;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmFluidBurden")) return false;
  if(!super.Deserialize(owner,component,context))
  {
   return false;
  }
  BCMM_FluidBurdenState state=new BCMM_FluidBurdenState();
  if(BCMM_PatientSaveValidation.HasState(context, "bcmmFluidBurden") && (!context.ReadValue("bcmmFluidBurden",state) || !state || !state.Valid()))
  {
   return false;
  }
  return SCR_CharacterDamageManagerComponent.Cast(component).BCMM_GetPatient().BCMM_RestoreFluidBurden(state);
 }
}


