// Diagnostic-only registered binary named-object rejection with a healthy control. Never publish.
class BCMM_OptionalFaultWorldManifest : JsonApiStruct
{
 string SaveId;
 string PatientId;
 string ControlId;
 string Field;
 void BCMM_OptionalFaultWorldManifest()
 {
  RegV("SaveId");
  RegV("PatientId");
  RegV("ControlId");
  RegV("Field");
 }
}
class BCMM_CorruptNativeSalineState : BCMM_SavedNativeSaline
{
 override bool SerializationSave(SaveContext context)
 {
  return context.WriteValue("schema", 99);
 }
}
class BCMM_CorruptFluidLineState : BCMM_SavedFluidLine
{
 void BCMM_CorruptFluidLineState()
 {
  Schema = 99;
 }
}
class BCMM_CorruptFluidBurdenState : BCMM_FluidBurdenState
{
 override bool SerializationSave(SaveContext context)
 {
  return context.WriteValue("schema", 99);
 }
}
class BCMM_OptionalFaultWriter
{
 static IEntity Patient;
 static bool Observed;
 static string Field;
}
modded class BCMM_CharacterClinicalSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity owner, notnull GenericComponent component, notnull SaveContext context)
 {
  ESerializeResult result = super.Serialize(owner, component, context);
  if (result == ESerializeResult.ERROR || !BCMM_OptionalFaultWriter.Patient || owner != BCMM_OptionalFaultWriter.Patient)
  {
   return result;
  }
  Managed badState;
  if (BCMM_OptionalFaultWriter.Field == "bcmmNativeSaline")
  {
   badState = new BCMM_CorruptNativeSalineState();
  }
  else if (BCMM_OptionalFaultWriter.Field == "bcmmFluidLine")
  {
   badState = new BCMM_CorruptFluidLineState();
  }
  else
  {
   badState = new BCMM_CorruptFluidBurdenState();
  }
  bool written = context.WriteValue(BCMM_OptionalFaultWriter.Field, badState);
  BCMM_OptionalFaultWriter.Observed = written;
  PrintFormat("BCMM_OPTIONAL_WORLD_WRITE field=%1 written=%2", BCMM_OptionalFaultWriter.Field, written);
  if (!written)
  {
   return ESerializeResult.ERROR;
  }
  return ESerializeResult.OK;
 }

}
class BCMM_OptionalFaultWorldSeed : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_OptionalFaultWorldManifest m_Save = new BCMM_OptionalFaultWorldManifest();
 override protected void Prepare()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.BCMM_SetMedic(true);
  m_Save.Field = "bcmmNativeSaline";
  System.GetCLIParam("bcmmOptionalFaultField", m_Save.Field);
  m_Suite.Record(m_Save.Field == "bcmmNativeSaline" || m_Save.Field == "bcmmFluidLine" || m_Save.Field == "bcmmFluidBurden", "Fault world names an actual optional clinical field");
  m_Suite.Record(persistence && persistence.IsTracked(m_Target), "Fault world uses an actual native tracked fixture patient");
  if (!persistence)
  {
   Finish();
   return;
  }
  m_Save.PatientId = persistence.GetId(m_Target);
  m_Save.ControlId = persistence.GetId(m_Actor);
  BCMM_OptionalFaultWriter.Patient = m_Target;
  BCMM_OptionalFaultWriter.Field = m_Save.Field;
  SaveGameManager manager = GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED, "BCMM optional clinical fault", ESaveGameRequestFlags.BLOCKING, new SaveGameOperationCallback(OnSaved));
 }
 protected void OnSaved(bool success, Managed context = null)
 {
  m_Suite.Record(success && BCMM_OptionalFaultWriter.Observed, "Actual registered clinical serializer writes an invalid-schema named optional object into the genuine native save point");
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(), new SaveGameObtainCallback(OnSaves));
 }
 protected void OnSaves(bool success, array<SaveGame> saves, Managed context = null)
 {
  SaveGame selected;
  if (success && saves)
  {
   foreach (SaveGame save : saves)
   {
    if (save.GetSavePointName() == "BCMM optional clinical fault" && (!selected || save.GetSavePointCreatedUnix() >= selected.GetSavePointCreatedUnix()))
    {
     selected = save;
    }
   }
  }
  m_Suite.Record(selected != null, "Native optional-field fault save point is discoverable");
  if (selected)
  {
   m_Save.SaveId = selected.GetId();
   m_Suite.Record(m_Save.PackToFile("$profile:BCMM_OptionalFaultWorld.json"), "Fault manifest retains the original patient UUID and exact altered optional field");
  }
  m_Suite.Complete();
 }
}
class BCMM_OptionalFaultWorldRestart : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_OptionalFaultWorldManifest m_Save = new BCMM_OptionalFaultWorldManifest();
 override protected void Prepare()
 {
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_OptionalFaultWorld.json") && persistence && persistence.WasDataLoaded() && BCMM_LifeIdentityFormat.IsCanonical(m_Save.PatientId), "Separate native process loads the actual malformed optional-field world under its original patient UUID");
  if (!persistence)
  {
   Finish();
   return;
  }
  IEntity control = IEntity.Cast(persistence.FindById(m_Save.ControlId));
  m_Suite.Record(control != null, "Healthy control character without malformed optional state survives the same native world load");
  IEntity original = IEntity.Cast(persistence.FindById(m_Save.PatientId));
  PrintFormat("BCMM_OPTIONAL_WORLD_RESTORE field=%1 patientRetained=%2", m_Save.Field, original != null);
  m_Suite.Record(!original, "Actual registered world deserialization rejects an invalid-schema named optional object instead of treating it as absent");
  Finish();
 }
}




