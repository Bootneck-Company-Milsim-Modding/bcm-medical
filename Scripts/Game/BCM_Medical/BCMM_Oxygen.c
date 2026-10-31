// Original finite, authority-owned portable oxygen sessions. No medical doses.
class BCMM_OxygenSession
{
 protected static ref array<ref BCMM_OxygenSession> s_Active={};
 protected IEntity m_Actor;protected IEntity m_Patient;protected IEntity m_Device;
 protected ref BCMM_Patient m_Model;
 static void Start(IEntity actor,IEntity patient,IEntity device)
 {
  if(!Replication.IsServer() || !actor || !patient || !device) { return; }
  for(int i=s_Active.Count()-1;i>=0;i--) { if(s_Active[i].m_Device==device || s_Active[i].m_Patient==patient) { s_Active[i].Stop(); } }
  BCMM_OxygenSession session=new BCMM_OxygenSession();session.m_Actor=actor;session.m_Patient=patient;session.m_Device=device;
  session.m_Model=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient();
  s_Active.Insert(session);session.Update();
 }
 static BCMM_OxygenSession ConnectionForPatient(IEntity patient)
 {
  if(!patient)return null;
  foreach(BCMM_OxygenSession session:s_Active)if(session.m_Patient==patient && session.m_Device)return session;
  return null;
 }
 static string Status(IEntity patient)
 {
  foreach(BCMM_OxygenSession session:s_Active)
  {
   if(session.m_Patient!=patient || !session.m_Device) { continue; }
   BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(session.m_Device.FindComponent(BCMM_EquipmentItemComponent));
   if(source) { return string.Format("\nPortable oxygen: %1 L remaining, %2 L/min",Math.Round(source.OxygenLitres()),BCMM_TreatmentAccess.Setting("oxygenFlowLitresPerMinute")); }
  }
  return "\nNo portable oxygen connected";
 }
 static void StopForPatient(IEntity patient)
 {
  if(!Replication.IsServer()) { return; }
  for(int i=s_Active.Count()-1;i>=0;i--) { if(s_Active[i].m_Patient==patient) { s_Active[i].Stop(); } }
 }
 protected void Update()
 {
  SCR_CharacterDamageManagerComponent medical;InventoryStorageManagerComponent inventory;BCMM_EquipmentItemComponent marker;
  ChimeraCharacter operator=ChimeraCharacter.Cast(m_Actor);
  if(m_Patient) { medical=SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent)); }
  if(m_Actor) { inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent)); }
  if(m_Device) { marker=BCMM_EquipmentItemComponent.Cast(m_Device.FindComponent(BCMM_EquipmentItemComponent)); }
  if(!medical || medical.IsDestroyed() || medical.BCMM_GetPatient()!=m_Model || m_Model.Terminal || !inventory || !m_Device || !inventory.Contains(m_Device) || !marker || marker.Kind!=BCMM_EquipmentKind.OXYGEN || !operator || !operator.GetCharacterController() || operator.GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD || vector.Distance(m_Actor.GetOrigin(),m_Patient.GetOrigin())>BCMM_TreatmentAccess.Setting("oxygenRangeMetres") || BCMM_TreatmentAccess.Setting("oxygenEnabled")<0.5 || marker.OxygenLitres()<=0)
  { Stop();return; }
  marker.ConsumeOxygen(0.25);m_Model.Equipment.OxygenActive=marker.OxygenLitres()>0;BCMM_VisualStatusBridge.Publish(medical);
  GetGame().GetCallqueue().CallLater(Update,250);
 }
 protected void Stop()
 {
  if(m_Model) { m_Model.Equipment.OxygenActive=false; }
  if(m_Patient) { BCMM_VisualStatusBridge.Publish(SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent))); }
  GetGame().GetCallqueue().Remove(Update);s_Active.RemoveItem(this);
 }
}
