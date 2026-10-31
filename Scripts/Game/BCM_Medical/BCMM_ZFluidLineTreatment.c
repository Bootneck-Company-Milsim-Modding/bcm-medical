// Original timed native inventory connection and guarded line removal.
class BCMM_FluidLineConnection : BCMM_SplintTreatment
{
 protected int m_FluidAction;
 protected int m_AccessRegion;
 protected ref BCMM_Patient m_Model;
 static bool BeginLine(IEntity actor,IEntity patient,int fluidAction,int region,SCR_PlayerController player=null)
 {
  if(!Replication.IsServer() || (player && player.GetControlledEntity()!=actor) || !BCMM_EquipmentPolicy.IsInfusion(fluidAction) || !BCMM_TreatmentAccess.CanReach(actor,patient) || !BCMM_MedicRole.Allows(actor,"medicOnlyFluids") || BCMM_FluidLineSession.ForPatient(patient) || BCMM_FluidLineTransfer.PatientPending(patient))
  {
   return false;
  }
  IEntity source=BCMM_EquipmentTreatment.FindEquipment(actor,BCMM_EquipmentPolicy.Kind(fluidAction));
  if(!source || BCMM_SplintTreatment.IsReservedSupply(source) || BCMM_FluidLineSession.UsesSource(source) || BCMM_FluidLineTransfer.SourcePending(source))
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical)
  {
   return false;
  }
  BCMM_FluidLineConnection operation=new BCMM_FluidLineConnection();
  operation.m_Actor=actor;
  operation.m_Target=patient;
  operation.m_Kit=source;
  operation.m_FluidAction=fluidAction;
  operation.m_AccessRegion=region;
  operation.m_Model=medical.BCMM_GetPatient();
  if(!operation.StillEligible())
  {
   return false;
  }
  return StartOperation(operation,actor,patient,source,BCMM_TreatmentAccess.Setting("fluidLineConnectionSeconds"),player);
 }
 override protected bool ConsumesKit()
 {
  return false;
 }
 override protected int ProcedureRegion()
 {
  return m_AccessRegion;
 }
 override protected string ProcedureName()
 {
  return "Connect fluid line";
 }
 override protected bool StillEligible()
 {
  if(BCMM_TreatmentAccess.Setting("continuousFluidLinesEnabled")<0.5 || !BCMM_MedicRole.Allows(m_Actor,"medicOnlyFluids") || BCMM_FluidLineSession.ForPatient(m_Target) || BCMM_FluidLineTransfer.PatientPending(m_Target) || BCMM_FluidLineSession.UsesSource(m_Kit) || BCMM_FluidLineTransfer.SourcePending(m_Kit))
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical;
  if(m_Target)
  {
   medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  InventoryStorageManagerComponent inventory;
  if(m_Target)
  {
   inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  }
  BCMM_EquipmentItemComponent source;
  if(m_Kit)
  {
   source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  }
  if(!medical || medical.IsDestroyed() || !medical.GetBloodHitZone() || medical.BCMM_GetPatient()!=m_Model || !m_Model || !m_Model.Equipment.VascularAccess || m_Model.Equipment.VascularAccessRegion!=m_AccessRegion || m_AccessRegion<BCMM_BodyRegion.LEFT_ARM || m_AccessRegion>BCMM_BodyRegion.RIGHT_LEG || !inventory || !source || !source.IsFluidBag() || source.FluidRemainingMl()<=0.05)
  {
   return false;
  }
  if(m_Actor!=m_Target && !inventory.FindStorageForItem(m_Kit,EStoragePurpose.PURPOSE_ANY))
  {
   return false;
  }
  BCMM_Settings settings=new BCMM_Settings();
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode)
  {
   mode.BCMM_CopySettings(settings);
  }
  return BCMM_ClinicalSafety.FluidDenial(m_Model,medical.GetBloodHitZone().GetHealthScaled(),settings,BCMM_EquipmentPolicy.FluidType(m_FluidAction)).IsEmpty();
 }
 override protected void ApplyProcedure()
 {
  if(!StillEligible())
  {
   return;
  }
  BCMM_ProcedureAnimation.End(m_Player,m_Actor,m_Kit,m_ProcedureToken);
  BCMM_FluidLineTransfer.Move(m_Actor,m_Target,m_Kit,m_Model,m_AccessRegion,m_Player);
 }
}
class BCMM_FluidLineTransfer : ScriptedInventoryOperationCallback
{
 protected static ref array<ref BCMM_FluidLineTransfer> s_Pending=
 {
 };
 protected IEntity m_Patient;
 protected IEntity m_Source;
 protected ref BCMM_Patient m_Model;
 protected int m_Region;
 protected SCR_PlayerController m_Player;
 protected bool m_Completed;
 static bool PatientPending(IEntity patient)
 {
  foreach(BCMM_FluidLineTransfer pending:s_Pending)
  {
   if(pending.m_Patient==patient)
   {
    return true;
   }
  }
  return false;
 }
 static bool SourcePending(IEntity source)
 {
  foreach(BCMM_FluidLineTransfer pending:s_Pending)
  {
   if(pending.m_Source==source)
   {
    return true;
   }
  }
  return false;
 }
 static bool Move(IEntity actor,IEntity patient,IEntity source,BCMM_Patient model,int region,SCR_PlayerController player)
 {
  if(!Replication.IsServer() || !actor || !patient || !source || PatientPending(patient) || SourcePending(source))
  {
   return false;
  }
  InventoryStorageManagerComponent from=InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  InventoryStorageManagerComponent to=InventoryStorageManagerComponent.Cast(patient.FindComponent(InventoryStorageManagerComponent));
  if(!from || !to || !from.Contains(source))
  {
   return false;
  }
  if(actor==patient)
  {
   return BCMM_FluidLineSession.Start(patient,source,region);
  }
  BaseInventoryStorageComponent storage=to.FindStorageForItem(source,EStoragePurpose.PURPOSE_ANY);
  if(!storage)
  {
   return false;
  }
  BCMM_FluidLineTransfer transfer=new BCMM_FluidLineTransfer();
  transfer.m_Patient=patient;
  transfer.m_Source=source;
  transfer.m_Model=model;
  transfer.m_Region=region;
  transfer.m_Player=player;
  s_Pending.Insert(transfer);
  if(!from.TryMoveItemToStorage(source,storage,-1,transfer))
  {
   transfer.OnFailed();
   return false;
  }
  return true;
 }
 override protected void OnComplete()
 {
  if(m_Completed)
  {
   return;
  }
  m_Completed=true;
  SCR_CharacterDamageManagerComponent medical;
  if(m_Patient)
  {
   medical=SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  bool connected=medical && medical.BCMM_GetPatient()==m_Model && BCMM_FluidLineSession.Start(m_Patient,m_Source,m_Region);
  if(m_Player)
  {
   if(connected)
   {
    m_Player.BCMM_TreatmentFeedback("Fluid line connected. The original bag is in the patient's inventory; your hands are free.");
   }
   else m_Player.BCMM_TreatmentFeedback("Fluid line could not start. Check the original bag in the patient's inventory.");
  }
  s_Pending.RemoveItem(this);
 }
 override protected void OnFailed()
 {
  if(m_Completed)
  {
   return;
  }
  m_Completed=true;
  if(m_Player)
  {
   m_Player.BCMM_TreatmentFeedback("Fluid line transfer failed; no replacement bag was created.");
  }
  s_Pending.RemoveItem(this);
 }
}
class BCMM_FluidLineRemoval : BCMM_SplintTreatment
{
 protected ref BCMM_FluidLineSession m_Line;
 protected int m_Region;
 static bool BeginRemoval(IEntity actor,IEntity patient,int region,SCR_PlayerController player=null)
 {
  if(!Replication.IsServer() || (player && player.GetControlledEntity()!=actor) || !BCMM_TreatmentAccess.CanReach(actor,patient) || !BCMM_MedicRole.Allows(actor,"medicOnlyFluids"))
  {
   return false;
  }
  BCMM_FluidLineSession line=BCMM_FluidLineSession.ForPatient(patient);
  if(!line || line.AccessRegion()!=region)
  {
   return false;
  }
  BCMM_FluidLineRemoval operation=new BCMM_FluidLineRemoval();
  operation.m_Line=line;
  operation.m_Region=region;
  return StartOperation(operation,actor,patient,null,BCMM_TreatmentAccess.Setting("fluidLineRemovalSeconds"),player);
 }
 override protected bool RequiresInventorySupply()
 {
  return false;
 }
 override protected bool RequiresNativeAnimation()
 {
  return false;
 }
 override protected bool ConsumesKit()
 {
  return false;
 }
 override protected int ProcedureRegion()
 {
  return m_Region;
 }
 override protected string ProcedureName()
 {
  return "Disconnect fluid line";
 }
 override protected bool StillEligible()
 {
  return BCMM_FluidLineSession.ForPatient(m_Target)==m_Line && BCMM_MedicRole.Allows(m_Actor,"medicOnlyFluids");
 }
 override protected void ApplyProcedure()
 {
  if(StillEligible())
  {
   BCMM_FluidLineSession.StopForPatient(m_Target);
  }
 }
}
