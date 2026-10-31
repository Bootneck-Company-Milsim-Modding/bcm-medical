// Original optional stock-saline binding; existing clinical schema1 remains compatible.
modded class BCMM_CharacterClinicalSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity owner, notnull GenericComponent component, notnull SaveContext context)
 {
  ESerializeResult result = super.Serialize(owner, component, context);
  if (result == ESerializeResult.ERROR)
  {
   return result;
  }
  BCMM_SavedNativeSaline state = BCMM_NativeSalineBinding.Capture(owner, GetSystem());
  if (state && (!state.Valid() || !context.WriteValue("bcmmNativeSaline", state)))
  {
   return ESerializeResult.ERROR;
  }
  return result;
 }
 override protected bool Deserialize(notnull IEntity owner, notnull GenericComponent component, notnull LoadContext context)
 {
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmNativeSaline")) return false;
  if (!super.Deserialize(owner, component, context))
  {
   return false;
  }
  if (!BCMM_PatientSaveValidation.HasState(context, "bcmmNativeSaline"))
  {
   return true;
  }
  BCMM_SavedNativeSaline state;
  if (!context.ReadValue("bcmmNativeSaline", state) || !state || !state.Valid())
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(component);
  BCMM_NativeSalineBinding.Await(owner, medical.BCMM_GetPatient(), state, GetSystem());
  return true;
 }
}


