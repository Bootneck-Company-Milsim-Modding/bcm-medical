// Original BCM handheld pulse monitoring. No inventory duplication or permanent
// hidden sensor is created: the operator must keep the exact device nearby.
class BCMM_PulseMonitorSession
{
 protected static ref array<ref BCMM_PulseMonitorSession> s_Active = {};
 IEntity Actor;
 IEntity Patient;
 IEntity Device;
 static void Start(IEntity actor, IEntity patient, IEntity device)
 {
  if (!Replication.IsServer() || !actor || !patient || !device) return;
  for (int i = s_Active.Count() - 1; i >= 0; i--)
   if (s_Active[i].Actor == actor || s_Active[i].Patient == patient || s_Active[i].Device == device) s_Active[i].Stop();
  BCMM_PulseMonitorSession session = new BCMM_PulseMonitorSession();
  session.Actor = actor; session.Patient = patient; session.Device = device;
  s_Active.Insert(session); session.Update();
 }
 protected bool Valid()
 {
  if (!Actor || !Patient || !Device || !BCMM_TreatmentAccess.CanExamine(Actor, Patient)) return false;
  if (vector.Distance(Actor.GetOrigin(), Patient.GetOrigin()) > BCMM_TreatmentAccess.Setting("pulseOximeterRangeMetres")) return false;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(Actor.FindComponent(InventoryStorageManagerComponent));
  BCMM_EquipmentItemComponent marker = BCMM_EquipmentItemComponent.Cast(Device.FindComponent(BCMM_EquipmentItemComponent));
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  return inventory && inventory.Contains(Device) && marker && marker.Kind == BCMM_EquipmentKind.PULSE_OXIMETER && medical && !medical.IsDestroyed() && !medical.BCMM_GetPatient().Terminal;
 }
 protected void Update()
 {
  if (!Valid()) { Stop(); return; }
  SCR_CharacterDamageManagerComponent.Cast(Patient.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetPulseMonitor(true);
  GetGame().GetCallqueue().CallLater(Update, 250);
 }
 protected void Stop()
 {
  if (Patient)
  {
   SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Patient.FindComponent(SCR_CharacterDamageManagerComponent));
   if (medical) medical.BCMM_SetPulseMonitor(false);
  }
  GetGame().GetCallqueue().Remove(Update);
  s_Active.RemoveItem(this);
 }
}
modded class SCR_CharacterDamageManagerComponent
{
 [RplProp(onRplName: "BCMM_PulseMonitorChanged")] protected bool m_BCMMPulseMonitor;
 [RplProp()] protected ref array<int> m_BCMMRegionVisuals = {0, 0, 0, 0, 0, 0, 0};
 bool BCMM_PulseMonitorActive() { return m_BCMMPulseMonitor; }
 void BCMM_SetPulseMonitor(bool active)
 {
  if (!Replication.IsServer() || active == m_BCMMPulseMonitor) return;
  m_BCMMPulseMonitor = active;
  BCMM_PulseMonitorChanged(); Replication.BumpMe();
 }
 protected void BCMM_PulseMonitorChanged()
 {
  GetGame().GetCallqueue().Remove(BCMM_PulseAudioTick);
  if (m_BCMMPulseMonitor) GetGame().GetCallqueue().CallLater(BCMM_PulseAudioTick, 50);
 }
 protected void BCMM_PulseAudioTick()
 {
  if (!m_BCMMPulseMonitor || !GetOwner()) return;
  float pulse = BCMM_Pulse();
  PlayerController player = GetGame().GetPlayerController();
  if (pulse > 0 && player && player.GetControlledEntity() && BCMM_TreatmentAccess.Setting("pulseOximeterSoundEnabled") > 0.5 && vector.Distance(player.GetControlledEntity().GetOrigin(), GetOwner().GetOrigin()) <= BCMM_TreatmentAccess.Setting("pulseOximeterAudibleMetres"))
   AudioSystem.PlaySound("{6AB4B3F3FD219301}Sounds/Medical/BCMM_Pulse.wav");
  float interval = 250;
  if (pulse > 0) interval = 60000 / Math.Clamp(pulse, 20, 300);
  GetGame().GetCallqueue().CallLater(BCMM_PulseAudioTick, interval);
 }
 int BCMM_RegionVisual(int region)
 {
  if (!m_BCMMRegionVisuals.IsIndexValid(region)) return 0;
  return m_BCMMRegionVisuals[region];
 }
 void BCMM_PublishRegionVisuals()
 {
  if (!Replication.IsServer() || !m_BCMMPatient) return;
  bool changed;
  for (int region = 0; region < 7; region++)
  {
   int flags = 0;
   if (region == BCMM_BodyRegion.LEFT_LEG && m_BCMMPatient.LeftLeg.Broken) { flags = 1; if (m_BCMMPatient.LeftLeg.Splinted) flags |= 2; }
   if (region == BCMM_BodyRegion.RIGHT_LEG && m_BCMMPatient.RightLeg.Broken) { flags = 1; if (m_BCMMPatient.RightLeg.Splinted) flags |= 2; }
   if (GetGroupTourniquetted(BCMM_ClinicalChecks.NativeGroup(region))) flags |= 16;
   for (int index = 0; index < m_BCMMPatient.WoundCount(); index++)
   {
    BCMM_Wound wound = m_BCMMPatient.WoundAt(index);
    if (!wound || wound.BodyZone != region) continue;
    if (wound.Sutured) flags |= 8;
    else if (wound.Bandaged) flags |= 4;
   }
   if (m_BCMMRegionVisuals[region] != flags) { m_BCMMRegionVisuals[region] = flags; changed = true; }
  }
  if (changed) Replication.BumpMe();
 }
}

class BCMM_VisualStatusBridge
{
 static void Publish(SCR_CharacterDamageManagerComponent medical) { if (medical) { medical.BCMM_PublishRegionVisuals(); medical.BCMM_PublishAppliedEquipment(); } }
}

// Original BCM replicated clinical presentation. Cosmetics never contain usable items.
modded class SCR_CharacterDamageManagerComponent
{
 [RplProp()] protected int m_BCMMAppliedEquipment;
 [RplProp()] protected int m_BCMMIVRegion = BCMM_BodyRegion.LEFT_ARM;
 int BCMM_AppliedEquipment() { return m_BCMMAppliedEquipment; }
 int BCMM_IVRegion() { return m_BCMMIVRegion; }
 void BCMM_PublishAppliedEquipment()
 {
  if(!Replication.IsServer()) { return; }
  BCMM_Patient patient=BCMM_GetPatient();if(!patient) { return; }
  int flags;
  if(patient.Equipment.PadsAttached) { flags |= 1; }
  if(patient.LeftChest.Sealed) { flags |= 2; }
  if(patient.RightChest.Sealed) { flags |= 4; }
  if(patient.LeftLeg.Broken && patient.LeftLeg.Splinted) { flags |= 8; }
  if(patient.RightLeg.Broken && patient.RightLeg.Splinted) { flags |= 16; }
  if(patient.Equipment.VascularAccess) { flags |= 32; }
  if(patient.Equipment.OxygenActive) { flags |= 64; }
  int region=patient.Equipment.VascularAccessRegion;
  if(flags!=m_BCMMAppliedEquipment || region!=m_BCMMIVRegion)
  {
   m_BCMMAppliedEquipment=flags;m_BCMMIVRegion=region;Replication.BumpMe();
  }
 }
}
