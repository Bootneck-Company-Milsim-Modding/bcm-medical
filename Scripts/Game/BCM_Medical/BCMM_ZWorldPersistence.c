// Original BCM native physical remains and stationary-world persistence.
class BCMM_SavedRemainsRecord
{
 int Token;
 string PlayerIdentity;
 int Life;
 string DeceasedName;
 string DeathFactionKey;
 float DeathAge;
 UUID PhysicalId;
 UUID OriginalCorpseId;
 UUID VehicleId;
 int Disposition;
}
class BCMM_SavedRemainsRegistry
{
 int Schema=1;
 int NextToken;
 ref array<ref BCMM_SavedRemainsRecord> Records={};
}
// Preserve unavailable native references without recreating physical recovery.
modded class BCMM_RemainsRecord
{
 UUID PendingPhysicalId;
}
modded class SCR_BaseGameMode
{
 BCMM_SavedRemainsRegistry BCMM_CaptureSavedRemains(PersistenceSystem persistence)
 {
  if(!Replication.IsServer() || !persistence || m_BCMMRemainsRecords.Count()>100000)return null;
  BCMM_SavedRemainsRegistry saved=new BCMM_SavedRemainsRegistry();saved.NextToken=m_BCMMNextRemainsToken;
  float now=GetGame().GetWorld().GetWorldTime()*0.001;
  foreach(int token,BCMM_RemainsRecord record:m_BCMMRemainsRecords)
  {
   if(!record || record.DeletionPending)return null;
   BCMM_SavedRemainsRecord copy=new BCMM_SavedRemainsRecord();copy.Token=token;copy.PlayerIdentity=record.PlayerIdentity;copy.Life=record.Life;copy.DeceasedName=record.DeceasedName;copy.DeathFactionKey=record.DeathFactionKey;copy.DeathAge=Math.Max(0,now-record.DeathSeconds);copy.Disposition=record.Disposition;
   if(record.Physical)copy.PhysicalId=persistence.GetId(record.Physical);
   else if(record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION)copy.PhysicalId=record.PendingPhysicalId;
   if(record.OriginalCorpse)copy.OriginalCorpseId=persistence.GetId(record.OriginalCorpse);
   if(record.StoredVehicle)copy.VehicleId=persistence.GetId(record.StoredVehicle);
   if(record.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION && copy.PhysicalId.IsNull())return null;
   saved.Records.Insert(copy);
  }
  return saved;
 }
 bool BCMM_RestoreSavedRemains(PersistenceSystem persistence,BCMM_SavedRemainsRegistry saved)
 {
  if(!Replication.IsServer() || !persistence || !saved || saved.Schema!=1 || saved.NextToken<0 || saved.NextToken>1000000000 || !saved.Records || saved.Records.Count()>100000)return false;
  map<int,ref BCMM_RemainsRecord> candidate=new map<int,ref BCMM_RemainsRecord>();
  float now=GetGame().GetWorld().GetWorldTime()*0.001;
  foreach(BCMM_SavedRemainsRecord copy:saved.Records)
  {
   if(!copy || copy.Token<=0 || copy.Token>saved.NextToken || candidate.Contains(copy.Token) || copy.Life<0 || copy.Life>1000000000 || copy.DeceasedName.Length()>1024 || copy.DeathFactionKey.Length()>128 || !BCMM_PatientSaveValidation.Range(copy.DeathAge,0,1000000000) || copy.Disposition<0 || copy.Disposition>BCMM_RemainsDisposition.VEHICLE_LOST)return false;
   if(!copy.PlayerIdentity.IsEmpty() && !BCMM_LifeIdentityFormat.IsCanonical(copy.PlayerIdentity))return false;
   if(!copy.PhysicalId.IsNull() && !BCMM_LifeIdentityFormat.IsCanonical(copy.PhysicalId))return false;
   if(!copy.OriginalCorpseId.IsNull() && !BCMM_LifeIdentityFormat.IsCanonical(copy.OriginalCorpseId))return false;
   if(!copy.VehicleId.IsNull() && !BCMM_LifeIdentityFormat.IsCanonical(copy.VehicleId))return false;
   if(copy.Disposition==BCMM_RemainsDisposition.AWAITING_DISPOSITION && copy.PhysicalId.IsNull())return false;
   BCMM_RemainsRecord record=new BCMM_RemainsRecord();record.Token=copy.Token;record.PlayerIdentity=copy.PlayerIdentity;record.Life=copy.Life;record.DeceasedName=copy.DeceasedName;record.DeathFactionKey=copy.DeathFactionKey;record.DeathSeconds=now-copy.DeathAge;record.Disposition=copy.Disposition;
   record.PendingPhysicalId=copy.PhysicalId;
   candidate.Set(copy.Token,record);
  }
  m_BCMMRemainsRecords=candidate;m_BCMMNextRemainsToken=saved.NextToken;
  foreach(BCMM_SavedRemainsRecord savedRecord:saved.Records)
  {
   if(!savedRecord.PhysicalId.IsNull())BCMM_SavedRemainsBinding.Await(this,persistence,savedRecord.Token,0,savedRecord.PhysicalId);
   if(!savedRecord.OriginalCorpseId.IsNull())BCMM_SavedRemainsBinding.Await(this,persistence,savedRecord.Token,1,savedRecord.OriginalCorpseId);
   if(!savedRecord.VehicleId.IsNull())BCMM_SavedRemainsBinding.Await(this,persistence,savedRecord.Token,2,savedRecord.VehicleId);
  }
  return true;
 }
}
class BCMM_SavedRemainsBinding
{
 SCR_BaseGameMode Mode;
 int Token;
 int Kind;
 UUID ExpectedId;
 BCMM_RemainsRecord ExpectedRecord;
 protected static void OnAvailable(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedRemainsBinding binding=BCMM_SavedRemainsBinding.Cast(context);IEntity entity=IEntity.Cast(instance);
  if(!binding || !binding.Mode || expired || !entity || binding.Mode.BCMM_RemainsByToken(binding.Token)!=binding.ExpectedRecord)return;
  PersistenceSystem persistence=PersistenceSystem.GetInstance();if(!persistence || persistence.GetId(entity)!=binding.ExpectedId)return;
  if(binding.Kind==0){binding.ExpectedRecord.Physical=entity;binding.ExpectedRecord.PendingPhysicalId=UUID.NULL_UUID;}
  if(binding.Kind==1){binding.ExpectedRecord.OriginalCorpse=entity;binding.ExpectedRecord.OriginalId=BCMM_BodyBagComponent.Id(entity);}
  if(binding.Kind==2)binding.ExpectedRecord.StoredVehicle=entity;
 }
 static void Await(SCR_BaseGameMode mode,PersistenceSystem persistence,int token,int kind,UUID expected)
 {
  BCMM_SavedRemainsBinding binding=new BCMM_SavedRemainsBinding();binding.Mode=mode;binding.Token=token;binding.Kind=kind;binding.ExpectedId=expected;binding.ExpectedRecord=mode.BCMM_RemainsByToken(token);
  persistence.WhenAvailable(expected,new PersistenceWhenAvailableTask(OnAvailable,binding),30);
 }
}
modded class SCR_GameModeSerializer
{
 override protected ESerializeResult Serialize(notnull IEntity entity,notnull SaveContext context)
 {
  ESerializeResult baseResult=super.Serialize(entity,context);if(baseResult==ESerializeResult.ERROR)return baseResult;
  BCMM_SavedRemainsRegistry records=SCR_BaseGameMode.Cast(entity).BCMM_CaptureSavedRemains(GetSystem());
  if(!records || !context.WriteValue("bcmmPhysicalRemains",records))return ESerializeResult.ERROR;
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity entity,notnull LoadContext context)
 {
  if(!super.Deserialize(entity,context))return false;
  if(!context.DoesObjectExist("bcmmPhysicalRemains"))return true;
  BCMM_SavedRemainsRegistry records;
  return context.ReadValue("bcmmPhysicalRemains",records) && SCR_BaseGameMode.Cast(entity).BCMM_RestoreSavedRemains(GetSystem(),records);
 }
}
class BCMM_SavedBagState
{
 int Schema=1;
 string DeceasedName;
 int RemainsToken;
 int StateRevision;
}
modded class BCMM_BodyBagComponent
{
 BCMM_SavedBagState BCMM_CaptureSavedBag()
 {
  if(!Replication.IsServer() || !m_Ready || m_State==BCMM_BagCargoState.LOADING || m_State==BCMM_BagCargoState.UNLOADING || m_State==BCMM_BagCargoState.VEHICLE_LOST)return null;
  BCMM_SavedBagState state=new BCMM_SavedBagState();state.DeceasedName=m_DeceasedName;state.RemainsToken=m_RemainsToken;state.StateRevision=m_Revision;return state;
 }
 bool BCMM_RestoreSavedBag(BCMM_SavedBagState state)
 {
  if(!Replication.IsServer() || !state || state.Schema!=1 || state.DeceasedName.Length()>1024 || state.RemainsToken<0 || state.RemainsToken>1000000000 || state.StateRevision<0 || state.StateRevision>=1000000000)return false;
  m_DeceasedName=state.DeceasedName;m_RemainsToken=state.RemainsToken;m_Revision=state.StateRevision+1;m_Ready=true;m_Searching=false;m_DraggingActor=RplId.Invalid();m_State=BCMM_BagCargoState.WORLD;m_Vehicle=RplId.Invalid();
  // Native inventory hierarchy is reconciled by EOnFrame after restoration.
  Replication.BumpMe();return true;
 }
}
class BCMM_BodyBagSerializer : ScriptedComponentSerializer
{
 override static typename GetTargetType(){return BCMM_BodyBagComponent;}
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  BCMM_SavedBagState state=BCMM_BodyBagComponent.Cast(component).BCMM_CaptureSavedBag();
  if(!state || !context.WriteValue("bcmmBag",state))return ESerializeResult.ERROR;
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  BCMM_SavedBagState state;return context.ReadValue("bcmmBag",state) && BCMM_BodyBagComponent.Cast(component).BCMM_RestoreSavedBag(state);
 }
}
class BCMM_SavedGraveState
{
 int Schema=1;
 string DeceasedName;
 int Token;
 float DeathAge;
 float BurialAge;
}
modded class BCMM_GraveMarkerComponent
{
 BCMM_SavedGraveState BCMM_CaptureSavedGrave()
 {
  if(!Replication.IsServer() || m_Token<=0)return null;
  float now=GetOwner().GetWorld().GetWorldTime()*0.001;
  BCMM_SavedGraveState state=new BCMM_SavedGraveState();state.DeceasedName=m_Name;state.Token=m_Token;state.DeathAge=Math.Max(0,now-m_DeathSeconds);state.BurialAge=Math.Max(0,now-m_BurialSeconds);return state;
 }
 bool BCMM_RestoreSavedGrave(BCMM_SavedGraveState state)
 {
  if(!Replication.IsServer() || !state || state.Schema!=1 || state.Token<=0 || state.Token>1000000000 || state.DeceasedName.Length()>1024 || !BCMM_PatientSaveValidation.Range(state.DeathAge,0,1000000000) || !BCMM_PatientSaveValidation.Range(state.BurialAge,0,state.DeathAge))return false;
  float now=GetOwner().GetWorld().GetWorldTime()*0.001;m_Name=state.DeceasedName;m_Token=state.Token;m_DeathSeconds=now-state.DeathAge;m_BurialSeconds=now-state.BurialAge;Replication.BumpMe();return true;
 }
}
class BCMM_GraveSerializer : ScriptedComponentSerializer
{
 override static typename GetTargetType(){return BCMM_GraveMarkerComponent;}
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  BCMM_SavedGraveState state=BCMM_GraveMarkerComponent.Cast(component).BCMM_CaptureSavedGrave();
  if(!state || !context.WriteValue("bcmmMemorial",state)){return ESerializeResult.ERROR;}
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  BCMM_SavedGraveState state;return context.ReadValue("bcmmMemorial",state) && BCMM_GraveMarkerComponent.Cast(component).BCMM_RestoreSavedGrave(state);
 }
}
class BCMM_SavedStretcherState
{
 int Schema=1;
 UUID PatientId;
}
modded class BCMM_StretcherComponent
{
 BCMM_SavedStretcherState BCMM_CaptureSavedStretcher(PersistenceSystem persistence)
 {
  if(!Replication.IsServer() || !persistence || HasCarriers() || IsCarried())return null;
  BCMM_SavedStretcherState state=new BCMM_SavedStretcherState();IEntity patient=Patient();
  if(patient){state.PatientId=persistence.GetId(patient);if(state.PatientId.IsNull())return null;}
  return state;
 }
 bool BCMM_RestoreSavedStretcher(BCMM_SavedStretcherState state,PersistenceSystem persistence)
 {
  if(!Replication.IsServer() || !persistence || !state || state.Schema!=1)return false;
  if(!state.PatientId.IsNull() && !BCMM_LifeIdentityFormat.IsCanonical(state.PatientId))return false;
  m_Patient=RplId.Invalid();m_ReleasedPatient=RplId.Invalid();m_PlacementPatient=RplId.Invalid();
  if(!state.PatientId.IsNull())BCMM_SavedStretcherBinding.Await(this,persistence,state.PatientId);
  Replication.BumpMe();return true;
 }
 void BCMM_BindSavedStretcherPatient(UUID expected,IEntity patient)
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();ChimeraCharacter character=ChimeraCharacter.Cast(patient);
  if(!Replication.IsServer() || !persistence || !character || persistence.GetId(patient)!=expected || patient.GetParent() || character.IsInVehicle() || !character.GetCharacterController() || character.GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD || !character.GetCharacterController().IsUnconscious() || vector.Distance(patient.GetOrigin(),GetOwner().GetOrigin())>1.6 || OnAnotherStretcher(patient))return;
  m_Patient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();
 }
}
class BCMM_SavedStretcherBinding
{
 BCMM_StretcherComponent Stretcher;
 UUID ExpectedId;
 IEntity Patient;
 protected static ref array<ref BCMM_SavedStretcherBinding> s_Pending={};
 protected static void OnAvailable(Managed instance,PersistenceDeferredDeserializeTask task,bool expired,Managed context)
 {
  BCMM_SavedStretcherBinding binding=BCMM_SavedStretcherBinding.Cast(context);if(!binding || expired)return;
  binding.Patient=IEntity.Cast(instance);GetGame().GetCallqueue().CallLater(binding.Bind,250);
 }
 protected void Bind()
 {
  if(Stretcher && Stretcher.GetOwner())Stretcher.BCMM_BindSavedStretcherPatient(ExpectedId,Patient);
  GetGame().GetCallqueue().Remove(Expire);s_Pending.RemoveItem(this);
 }
 protected void Expire(){s_Pending.RemoveItem(this);}
 static void Await(BCMM_StretcherComponent stretcher,PersistenceSystem persistence,UUID expected)
 {
  BCMM_SavedStretcherBinding binding=new BCMM_SavedStretcherBinding();binding.Stretcher=stretcher;binding.ExpectedId=expected;s_Pending.Insert(binding);GetGame().GetCallqueue().CallLater(binding.Expire,31000);
  persistence.WhenAvailable(expected,new PersistenceWhenAvailableTask(OnAvailable,binding),30);
 }
}
class BCMM_StretcherSerializer : ScriptedComponentSerializer
{
 override static typename GetTargetType(){return BCMM_StretcherComponent;}
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  BCMM_SavedStretcherState state=BCMM_StretcherComponent.Cast(component).BCMM_CaptureSavedStretcher(GetSystem());
  if(!state || !context.WriteValue("bcmmStationaryPatient",state)){return ESerializeResult.ERROR;}
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  BCMM_SavedStretcherState state;return context.ReadValue("bcmmStationaryPatient",state) && BCMM_StretcherComponent.Cast(component).BCMM_RestoreSavedStretcher(state,GetSystem());
 }
}

// The recovery bag denies arbitrary insertion. Native synchronous load is the
// only extra insertion scope; normal gameplay always keeps the existing guard.
class BCMM_RecoveryStorageSerializer : BaseInventoryStorageComponentSerializer
{
 override static typename GetTargetType(){return BCMM_BagStorageComponent;}
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  BCMM_BagStorageComponent storage=BCMM_BagStorageComponent.Cast(component);
  storage.SetTransferring(true);
  bool restored=super.Deserialize(owner,component,context);
  storage.SetTransferring(false);
  return restored;
 }
}

// Native storage writes its nested slot/entity values without type tags.
// Match that scope on read, including nested scripted medical components,
// then restore the caller's context setting for the next serializer.
class BCMM_NativeVehicleStorageSerializer : BaseInventoryStorageComponentSerializer
{
 override static typename GetTargetType(){return BaseInventoryStorageComponent;}
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  bool previous=context.EnableTypeDiscriminator(false);
  bool restored=super.Deserialize(owner,component,context);
  context.EnableTypeDiscriminator(previous);
  return restored;
 }
}

