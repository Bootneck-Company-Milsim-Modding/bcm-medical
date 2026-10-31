// Original BCM patient-state save codecs and native character adapter.
class BCMM_PatientSaveValidation
{
 static bool StateShapeValid(notnull LoadContext context, string key)
 {
  if (!context.CanSeekMembers() || !context.DoesKeyExist(key)) return true;
  // JSON treats a primitive as an existing object cursor. Probe leaf types before
  // its native class reader, which cannot safely look up fields on a primitive.
  int integerValue;
  if (context.ReadValue(key, integerValue)) return false;
  float floatValue;
  if (context.ReadValue(key, floatValue)) return false;
  bool boolValue;
  if (context.ReadValue(key, boolValue)) return false;
  string stringValue;
  if (context.ReadValue(key, stringValue)) return false;
  return true;
 }
 static bool HasState(notnull LoadContext context, string key)
 {
  // Native binary contexts can seek named objects but report every primitive key as present.
  if (context.CanSeekMembers())
  {
   return context.DoesKeyExist(key);
  }
  return context.DoesObjectExist(key);
 }
 static bool Range(float value,float minimum,float maximum){return value>=minimum && value<=maximum;}
}
modded class BCMM_Wound
{
 bool SerializationSave(SaveContext context)
 {
  return context.WriteValue("id",Id) && context.WriteValue("region",BodyZone) && context.WriteValue("flow",UncontrolledFlowMlPerSecond) && context.WriteValue("compression",CompressionFraction) && context.WriteValue("packed",Packed) && context.WriteValue("bandaged",Bandaged) && context.WriteValue("cause",Cause) && context.WriteValue("projectile",RetainedProjectile) && context.WriteValue("sutured",Sutured) && context.WriteValue("reopened",Reopened) && context.WriteValue("wearRemaining",m_DressingWearRemaining);
 }
 bool SerializationLoad(LoadContext context)
 {
  if(!context.ReadValue("id",Id) || !context.ReadValue("region",BodyZone) || !context.ReadValue("flow",UncontrolledFlowMlPerSecond) || !context.ReadValue("compression",CompressionFraction) || !context.ReadValue("packed",Packed) || !context.ReadValue("bandaged",Bandaged) || !context.ReadValue("cause",Cause) || !context.ReadValue("projectile",RetainedProjectile) || !context.ReadValue("sutured",Sutured) || !context.ReadValue("reopened",Reopened) || !context.ReadValue("wearRemaining",m_DressingWearRemaining))return false;
  return Id>0 && BodyZone>=0 && BodyZone<=6 && Cause>=0 && Cause<=4 && BCMM_PatientSaveValidation.Range(UncontrolledFlowMlPerSecond,0,1000000) && BCMM_PatientSaveValidation.Range(CompressionFraction,0,1) && BCMM_PatientSaveValidation.Range(m_DressingWearRemaining,0,1000000000);
 }
}
modded class BCMM_Pharmacology
{
 bool SerializationSave(SaveContext context)
 {
  array<float> effects={Pain,OpioidEffect,BreathingScale,PressureScale,StimulantEffect,TXAEffect};
  return context.WriteValue("time",m_Time) && context.WriteValue("lastDose",m_LastDose) && context.WriteValue("doses",m_Doses) && context.WriteValue("effects",effects);
 }
 bool SerializationLoad(LoadContext context)
 {
  array<float> effects;
  if(!context.ReadValue("time",m_Time) || !context.ReadValue("lastDose",m_LastDose) || !context.ReadValue("doses",m_Doses) || !context.ReadValue("effects",effects))return false;
  if(!BCMM_PatientSaveValidation.Range(m_Time,0,1000000000) || !m_LastDose || m_LastDose.Count()!=6 || !m_Doses || m_Doses.Count()>4096 || !effects || effects.Count()!=6)return false;
  foreach(float last:m_LastDose)if(!BCMM_PatientSaveValidation.Range(last,-100000,m_Time))return false;
  foreach(BCMM_MedicationDose dose:m_Doses)if(!dose || dose.Kind<0 || dose.Kind>5 || !BCMM_PatientSaveValidation.Range(dose.Age,0,m_Time+0.001))return false;
  if(!BCMM_PatientSaveValidation.Range(effects[0],0,100) || !BCMM_PatientSaveValidation.Range(effects[1],0,4096) || !BCMM_PatientSaveValidation.Range(effects[2],0,1) || !BCMM_PatientSaveValidation.Range(effects[3],0,1) || !BCMM_PatientSaveValidation.Range(effects[4],0,4096) || !BCMM_PatientSaveValidation.Range(effects[5],0,1))return false;
  Pain=effects[0];OpioidEffect=effects[1];BreathingScale=effects[2];PressureScale=effects[3];StimulantEffect=effects[4];TXAEffect=effects[5];return true;
 }
}
modded class BCMM_Patient
{
 bool SerializationSave(SaveContext context)
 {
  array<float> vitals={OxygenSaturation,HeartRate,SystolicPressure,RespiratoryRate,PerfusionFraction,OxygenDeliveryFraction,HypoxicDebtSeconds,m_CriticalExposure,m_UnstableVitalExposure,m_CorrectedExposure,m_VentilationRemaining,m_TotalBloodLostMl};
  array<int> counters={m_NextWoundId,m_NextInternalId,m_ShocksDelivered};
  return context.WriteValue("schema",1) && context.WriteValue("wounds",m_Wounds) && context.WriteValue("internalBleeds",m_InternalBleeds) && context.WriteValue("leftChest",LeftChest) && context.WriteValue("rightChest",RightChest) && context.WriteValue("leftLeg",LeftLeg) && context.WriteValue("rightLeg",RightLeg) && context.WriteValue("fluids",Fluids) && context.WriteValue("medication",Medication) && context.WriteValue("equipment",Equipment) && context.WriteValue("rhythm",Rhythm) && context.WriteValue("airway",Airway) && context.WriteValue("circulation",HasSpontaneousCirculation) && context.WriteValue("terminal",Terminal) && context.WriteValue("vitals",vitals) && context.WriteValue("counters",counters);
 }
 bool SerializationLoad(LoadContext context)
 {
  int schema;array<float> vitals;array<int> counters;
  if(!context.ReadValue("schema",schema) || schema!=1 || !context.ReadValue("wounds",m_Wounds) || !context.ReadValue("internalBleeds",m_InternalBleeds) || !context.ReadValue("leftChest",LeftChest) || !context.ReadValue("rightChest",RightChest) || !context.ReadValue("leftLeg",LeftLeg) || !context.ReadValue("rightLeg",RightLeg) || !context.ReadValue("fluids",Fluids) || !context.ReadValue("medication",Medication) || !context.ReadValue("equipment",Equipment) || !context.ReadValue("rhythm",Rhythm) || !context.ReadValue("airway",Airway) || !context.ReadValue("circulation",HasSpontaneousCirculation) || !context.ReadValue("terminal",Terminal) || !context.ReadValue("vitals",vitals) || !context.ReadValue("counters",counters))return false;
  if(!m_Wounds || m_Wounds.Count()>65536 || !m_InternalBleeds || m_InternalBleeds.Count()>65536 || !LeftChest || !RightChest || !LeftLeg || !RightLeg || !Fluids || !Medication || !Equipment || !vitals || vitals.Count()!=12 || !counters || counters.Count()!=3)return false;
  if(Rhythm<0 || Rhythm>4 || Airway<0 || Airway>2)return false;
  foreach(int counter:counters)if(counter<0 || counter>1000000000)return false;
  set<int> woundIds=new set<int>();
  foreach(BCMM_Wound wound:m_Wounds)
  {
   if(!wound || wound.Id>counters[0] || woundIds.Contains(wound.Id))return false;
   woundIds.Insert(wound.Id);
  }
  set<int> internalIds=new set<int>();
  foreach(BCMM_InternalBleeding source:m_InternalBleeds)
  {
   if(!source || source.Id<=0 || source.Id>counters[1] || internalIds.Contains(source.Id) || source.Region<0 || source.Region>6 || source.Compartment<0 || source.Compartment>4 || !BCMM_PatientSaveValidation.Range(source.RateMlPerSecond,0,1000000) || !BCMM_PatientSaveValidation.Range(source.PooledMl,0,1000000000))return false;
   internalIds.Insert(source.Id);
  }
  array<BCMM_ChestSide> chests={LeftChest,RightChest};
  foreach(BCMM_ChestSide chest:chests)if(!BCMM_PatientSaveValidation.Range(chest.CollapseFraction,0,1) || !BCMM_PatientSaveValidation.Range(chest.PleuralBloodMl,0,1000000000))return false;
  if((LeftLeg.Splinted && !LeftLeg.Broken) || (RightLeg.Splinted && !RightLeg.Broken))return false;
  if(!BCMM_PatientSaveValidation.Range(Fluids.RedCellReserve,0,1) || !BCMM_PatientSaveValidation.Range(Fluids.ClottingReserve,0,1) || !BCMM_PatientSaveValidation.Range(Fluids.LastVolume,0,1) || !BCMM_PatientSaveValidation.Range(Fluids.TransientWater,0,10))return false;
  if(Equipment.VascularAccessRegion<0 || Equipment.VascularAccessRegion>6 || Equipment.MonitorChecks<0 || Equipment.MonitorChecks>1000000000 || !BCMM_PatientSaveValidation.Range(Equipment.AnalysisAge,0,1000000))return false;
  foreach(int index,float value:vitals)
  {
   float maximum=1000000000;if(index<=5)maximum=1000;if(index==0 || index==4 || index==5)maximum=1;
   if(!BCMM_PatientSaveValidation.Range(value,0,maximum))return false;
  }
  OxygenSaturation=vitals[0];HeartRate=vitals[1];SystolicPressure=vitals[2];RespiratoryRate=vitals[3];PerfusionFraction=vitals[4];OxygenDeliveryFraction=vitals[5];HypoxicDebtSeconds=vitals[6];m_CriticalExposure=vitals[7];m_UnstableVitalExposure=vitals[8];m_CorrectedExposure=vitals[9];m_VentilationRemaining=vitals[10];m_TotalBloodLostMl=vitals[11];
  m_NextWoundId=counters[0];m_NextInternalId=counters[1];m_ShocksDelivered=counters[2];
  // Do not resume an old actor/session or reuse a per-process AED analysis id.
  m_CompressionActor=0;m_CompressionSession=0;Equipment.AnalysisDevice=0;Equipment.AnalysisAge=1000000;Equipment.LastAnalysisAdvised=false;Equipment.OxygenActive=false;
  return true;
 }
}
modded class BCMM_DepletionPolicy
{
 array<float> BCMM_CaptureDepletion(float now)
 {
  float active;float expired;float remaining;if(m_Active){active=1;remaining=Math.Max(0,m_DeadlineSeconds-now);}if(m_Expired)expired=1;
  return {active,expired,remaining};
 }
 bool BCMM_RestoreDepletion(notnull array<float> state,float now)
 {
  if(state.Count()!=3 || !(now>=0) || (state[0]!=0 && state[0]!=1) || (state[1]!=0 && state[1]!=1) || !BCMM_PatientSaveValidation.Range(state[2],0,86400))return false;
  if((state[0]==0 && (state[1]!=0 || state[2]!=0)) || (state[1]==1 && state[2]!=0))return false;
  m_Active=state[0]==1;m_Expired=state[1]==1;m_DeadlineSeconds=0;if(m_Active)m_DeadlineSeconds=now+state[2];m_LastTimeSeconds=now;return true;
 }
}class BCMM_SavedClinicalState
{
 int Schema=1;
 ref BCMM_Patient Patient;
 ref array<int> WoundIds={};
 ref array<string> ZoneNames={};
 ref array<float> Depletion;
 ref array<string> OxygenRefs={};
 bool DeathPermitted;
 bool Depleted;
 bool Critical;
 bool Medic;
 string DeceasedName;
}
modded class BCMM_OxygenSession
{
 static array<string> BCMM_CaptureOxygenRefs(IEntity patient,PersistenceSystem persistence)
 {
  foreach(BCMM_OxygenSession session:s_Active)
  {
   if(session.m_Patient!=patient || !session.m_Actor || !session.m_Device || session.m_Model!=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient())continue;
   UUID actor=persistence.GetId(session.m_Actor);UUID device=persistence.GetId(session.m_Device);
   if(actor.IsNull() || device.IsNull())return {};
   return {actor,device};
  }
  return {};
 }
}
class BCMM_SavedOxygenBinding
{
 // Native availability callbacks can finish before the delayed inventory check.
 // Retain the binding explicitly until it starts or its bounded wait expires.
 protected static ref array<ref BCMM_SavedOxygenBinding> s_Pending={};
 IEntity PatientEntity;
 BCMM_Patient Model;
 IEntity ActorEntity;
 IEntity DeviceEntity;
 protected bool m_Started;
 protected static void OnActor(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedOxygenBinding binding=BCMM_SavedOxygenBinding.Cast(context);if(!binding || expired)return;
  binding.ActorEntity=IEntity.Cast(instance);binding.TryStart();
 }
 protected static void OnDevice(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedOxygenBinding binding=BCMM_SavedOxygenBinding.Cast(context);if(!binding || expired)return;
  binding.DeviceEntity=IEntity.Cast(instance);binding.TryStart();
 }
 protected void TryStart()
 {
  if(m_Started || !PatientEntity || !ActorEntity || !DeviceEntity)return;
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(PatientEntity.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical || medical.BCMM_GetPatient()!=Model)return;
  m_Started=true;GetGame().GetCallqueue().CallLater(StartValidated,100);
 }
 protected void StartValidated()
 {
  s_Pending.RemoveItem(this);GetGame().GetCallqueue().Remove(Expire);
  if(!PatientEntity || !ActorEntity || !DeviceEntity)return;
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(PatientEntity.FindComponent(SCR_CharacterDamageManagerComponent));
  if(medical && medical.BCMM_GetPatient()==Model)
  {
   BCMM_OxygenSession.Start(ActorEntity,PatientEntity,DeviceEntity);
  }
 }
 protected void Expire(){s_Pending.RemoveItem(this);}
 void Await(PersistenceSystem persistence,notnull array<string> refs)
 {
  s_Pending.Insert(this);GetGame().GetCallqueue().CallLater(Expire,31000);
  persistence.WhenAvailable(refs[0],new PersistenceWhenAvailableTask(OnActor,this),30);
  persistence.WhenAvailable(refs[1],new PersistenceWhenAvailableTask(OnDevice,this),30);
 }
}
modded class SCR_CharacterDamageManagerComponent
{
 BCMM_SavedClinicalState BCMM_CaptureClinicalState(PersistenceSystem persistence)
 {
  if(!Replication.IsServer() || !persistence || !m_BCMMPatient)return null;
  BCMM_SavedClinicalState state=new BCMM_SavedClinicalState();state.Patient=m_BCMMPatient;state.Depletion=m_BCMMDepletion.BCMM_CaptureDepletion(BCMM_Now());state.DeathPermitted=m_BCMMDeathPermitted;state.Depleted=m_BCMMDepleted;state.Critical=m_BCMMCritical;state.Medic=m_BCMMMedic;state.DeceasedName=m_BCMMDeceasedName;
  foreach(BCMM_NativeWound link:m_BCMMNativeWounds)
  {
   if(!link || !link.Zone || !m_BCMMPatient.FindWound(link.WoundId))return null;
   state.WoundIds.Insert(link.WoundId);state.ZoneNames.Insert(link.Zone.GetName());
  }
  if(m_BCMMPatient.Equipment.OxygenActive)state.OxygenRefs=BCMM_OxygenSession.BCMM_CaptureOxygenRefs(GetOwner(),persistence);
  if(m_BCMMPatient.Equipment.OxygenActive && state.OxygenRefs.Count()!=2)return null;
  return state;
 }
 bool BCMM_RestoreClinicalState(PersistenceSystem persistence,BCMM_SavedClinicalState state)
 {
  if(!Replication.IsServer() || !persistence || !state || state.Schema!=1 || !state.Patient || !state.Depletion || !state.WoundIds || !state.ZoneNames || state.WoundIds.Count()!=state.ZoneNames.Count() || state.WoundIds.Count()>state.Patient.WoundCount() || !state.OxygenRefs || (state.OxygenRefs.Count()!=0 && state.OxygenRefs.Count()!=2) || state.DeceasedName.Length()>1024 || BCMM_SplintTreatment.IsBusy(GetOwner(),GetOwner()))return false;
  foreach(string refId:state.OxygenRefs)if(!BCMM_LifeIdentityFormat.IsCanonical(refId))return false;
  BCMM_DepletionPolicy depletion=new BCMM_DepletionPolicy();if(!depletion.BCMM_RestoreDepletion(state.Depletion,BCMM_Now()))return false;
  array<ref BCMM_NativeWound> links={};set<int> seen=new set<int>();
  for(int index=0;index<state.WoundIds.Count();index++)
  {
   int id=state.WoundIds[index];if(seen.Contains(id) || !state.Patient.FindWound(id))return false;
   SCR_CharacterHitZone zone=SCR_CharacterHitZone.Cast(GetHitZoneByName(state.ZoneNames[index]));if(!zone)return false;
   BCMM_NativeWound link=new BCMM_NativeWound();link.WoundId=id;link.Zone=zone;links.Insert(link);seen.Insert(id);
  }
  BCMM_OxygenSession.StopForPatient(GetOwner());
  m_BCMMPatient=state.Patient;m_BCMMDepletion=depletion;m_BCMMNativeWounds=links;m_BCMMDeathPermitted=state.DeathPermitted;m_BCMMDepleted=state.Depleted;m_BCMMCritical=state.Critical;m_BCMMMedic=state.Medic;m_BCMMDeceasedName=state.DeceasedName;
  m_BCMMLastPhysiologyTime=BCMM_Now();
  m_BCMMOxygen=m_BCMMPatient.OxygenSaturation;m_BCMMPulse=m_BCMMPatient.HeartRate;m_BCMMPressure=m_BCMMPatient.SystolicPressure;m_BCMMRespiratoryRate=m_BCMMPatient.RespiratoryRate;m_BCMMArrest=!m_BCMMPatient.HasSpontaneousCirculation;
  m_BCMMHypoxic=m_BCMMPatient.OxygenSaturation<m_BCMMClinicalSettings.Get("unconsciousOxygenFraction") || m_BCMMPatient.OxygenDeliveryFraction<m_BCMMClinicalSettings.Get("recoveryOxygenDeliveryFraction");
  m_BCMMPainAim=m_BCMMPatient.Medication.Pain*0.01*m_BCMMClinicalSettings.Get("painAimingPenalty");
  if(state.OxygenRefs.Count()==2)
  {
   BCMM_SavedOxygenBinding binding=new BCMM_SavedOxygenBinding();binding.PatientEntity=GetOwner();binding.Model=m_BCMMPatient;binding.Await(persistence,state.OxygenRefs);
  }
  Replication.BumpMe();GetGame().GetCallqueue().CallLater(BCMM_PublishRestoredClinicalState,100);return true;
 }
 protected void BCMM_PublishRestoredClinicalState()
 {
  if(!GetOwner() || !BCMM_IsAuthority())return;
  BCMM_PublishFractures();BCMM_RefreshConsciousness();BCMM_ApplyPainHandling();BCMM_VisualStatusBridge.Publish(this);
 }
}
// Replace only the native Character.conf serializer entry with this subclass;
// ordinary vehicle and prop damage serializers keep their existing policy.
class BCMM_CharacterClinicalSerializer : HitZoneContainerComponentSerializer
{
 override static typename GetTargetType(){return SCR_CharacterDamageManagerComponent;}
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(component);
  BCMM_SavedClinicalState state=medical.BCMM_CaptureClinicalState(GetSystem());
  if(!state || !context.WriteValue("bcmmClinical",state))return ESerializeResult.ERROR;
  if(!context.StartObject("base"))return ESerializeResult.ERROR;
  ESerializeResult nativeResult=super.Serialize(owner,component,context);
  if(!context.EndObject())return ESerializeResult.ERROR;
  if(nativeResult==ESerializeResult.ERROR)return nativeResult;
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmClinical")) return false;
  if(BCMM_PatientSaveValidation.HasState(context, "bcmmClinical"))
  {
   BCMM_SavedClinicalState state;
   if(!context.ReadValue("bcmmClinical",state)){Print("BCMM_CLINICAL_LOAD codec rejected",LogLevel.ERROR);return false;}
   if(!SCR_CharacterDamageManagerComponent.Cast(component).BCMM_RestoreClinicalState(GetSystem(),state)){Print("BCMM_CLINICAL_LOAD binding rejected",LogLevel.ERROR);return false;}

  }
  if(context.DoesObjectExist("base"))
  {
   if(!context.StartObject("base"))return false;
   bool healthLoaded=super.Deserialize(owner,component,context);
   if(!healthLoaded || !context.EndObject())return false;
   return true;
  }
  // Native DEFAULT health omits the empty base object. Keep prefab health.
  if(BCMM_PatientSaveValidation.HasState(context, "bcmmClinical"))return true;
  return super.Deserialize(owner,component,context);
 }
}



