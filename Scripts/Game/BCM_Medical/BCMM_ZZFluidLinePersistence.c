// Original optional fluid-line binding; existing clinical schema1 remains compatible.
class BCMM_SavedFluidLine
{
 int Schema=1;
 string SourceId;
 int AccessRegion;
 bool Valid()
 {
  return Schema==1 && BCMM_LifeIdentityFormat.IsCanonical(SourceId) && AccessRegion>=BCMM_BodyRegion.LEFT_ARM && AccessRegion<=BCMM_BodyRegion.RIGHT_LEG;
 }
}
modded class BCMM_FluidLineSession
{
 BCMM_SavedFluidLine SaveBinding(PersistenceSystem persistence)
 {
  if(!m_Source || !m_Patient || !persistence)
  {
   return null;
  }
  UUID sourceId=persistence.GetId(m_Source);
  if(sourceId.IsNull())
  {
   return null;
  }
  BCMM_SavedFluidLine state=new BCMM_SavedFluidLine();
  state.SourceId=sourceId;
  state.AccessRegion=m_AccessRegion;
  return state;
 }
}
class BCMM_SavedFluidLineBinding
{
 protected static ref array<ref BCMM_SavedFluidLineBinding> s_Pending=
 {
 };
 IEntity PatientEntity;
 ref BCMM_Patient Model;
 ref BCMM_SavedFluidLine State;
 protected IEntity m_Source;
 protected bool m_Resolved;
 static BCMM_SavedFluidLine ForPatient(IEntity patient)
 {
  SCR_CharacterDamageManagerComponent medical;
  if(patient)
  {
   medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  if(!medical)
  {
   return null;
  }
  foreach(BCMM_SavedFluidLineBinding pending:s_Pending)
  {
   if(pending.PatientEntity==patient && pending.Model==medical.BCMM_GetPatient())
   {
    return pending.State;
   }
  }
  return null;
 }
 static void Await(IEntity patient,BCMM_Patient model,BCMM_SavedFluidLine state,PersistenceSystem persistence)
 {
  if(!Replication.IsServer() || !patient || !model || !state || !state.Valid() || !persistence)
  {
   return;
  }
  BCMM_SavedFluidLineBinding binding=new BCMM_SavedFluidLineBinding();
  binding.PatientEntity=patient;
  binding.Model=model;
  binding.State=state;
  s_Pending.Insert(binding);
  GetGame().GetCallqueue().CallLater(binding.Expire,31000);
  persistence.WhenAvailable(state.SourceId,new PersistenceWhenAvailableTask(OnSource,binding),30);
 }
 protected static void OnSource(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedFluidLineBinding binding=BCMM_SavedFluidLineBinding.Cast(context);
  if(!binding || binding.m_Resolved)
  {
   return;
  }
  if(expired)
  {
   binding.Expire();
   return;
  }
  binding.m_Source=IEntity.Cast(instance);
  binding.m_Resolved=true;
  GetGame().GetCallqueue().CallLater(binding.StartValidated,100);
 }
 protected void StartValidated()
 {
  SCR_CharacterDamageManagerComponent medical;
  if(PatientEntity)
  {
   medical=SCR_CharacterDamageManagerComponent.Cast(PatientEntity.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  if(medical && medical.BCMM_GetPatient()==Model && !BCMM_FluidLineSession.ForPatient(PatientEntity))
  {
   BCMM_FluidLineSession.Start(PatientEntity,m_Source,State.AccessRegion);
  }
  Expire();
 }
 static void CancelForPatient(IEntity patient)
 {
  if(!Replication.IsServer())
  {
   return;
  }
  for(int index=s_Pending.Count()-1;index>=0;index--)
  {
   if(s_Pending[index].PatientEntity==patient)
   {
    s_Pending[index].Expire();
   }
  }
 }
 protected void Expire()
 {
  m_Resolved=true;
  GetGame().GetCallqueue().Remove(Expire);
  GetGame().GetCallqueue().Remove(StartValidated);
  s_Pending.RemoveItem(this);
 }
}
modded class BCMM_CharacterClinicalSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  ESerializeResult clinicalResult=super.Serialize(owner,component,context);
  if(clinicalResult==ESerializeResult.ERROR)
  {
   return clinicalResult;
  }
  BCMM_FluidLineSession line=BCMM_FluidLineSession.ForPatient(owner);
  BCMM_SavedFluidLine state;
  if(line)
  {
   state=line.SaveBinding(GetSystem());
   if(!state)
   {
    return ESerializeResult.ERROR;
   }
  }
  else state=BCMM_SavedFluidLineBinding.ForPatient(owner);
  if(state && (!state.Valid() || !context.WriteValue("bcmmFluidLine",state)))
  {
   return ESerializeResult.ERROR;
  }
  return clinicalResult;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmFluidLine")) return false;
  if(!super.Deserialize(owner,component,context))
  {
   return false;
  }
  BCMM_FluidLineSession.StopForPatient(owner);
  if(!BCMM_PatientSaveValidation.HasState(context, "bcmmFluidLine"))
  {
   return true;
  }
  BCMM_SavedFluidLine state;
  if(!context.ReadValue("bcmmFluidLine",state) || !state || !state.Valid())
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(component);
  BCMM_SavedFluidLineBinding.Await(owner,medical.BCMM_GetPatient(),state,GetSystem());
  return true;
 }
}


