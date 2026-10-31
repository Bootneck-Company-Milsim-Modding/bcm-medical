// Original native passenger/finite-line world regression; scripted relocation, not driving acceptance.
class BCMM_TransportLineManifest : JsonApiStruct
{
 string VehicleId;
 void BCMM_TransportLineManifest()
 {
  RegV("VehicleId");
 }
}
class BCMM_TransportFluidLineChecks : BCMM_FluidLinePersistenceRuntimeChecks
{
 protected IEntity m_TransportVehicle;
 protected float m_TransportRemaining;
 protected float m_TransportVolume;
 protected ref BCMM_TransportLineManifest m_TransportSave = new BCMM_TransportLineManifest();
 override protected void SaveFlowingWorld()
 {
  m_TransportVehicle = Spawn("{6AB4B3F3FD216005}Prefabs/Vehicles/BCMMedical/BCMM_CargoJeep.et", "103 1 0");
  if (!m_TransportVehicle)
  {
   m_Suite.Record(false, "Native transport vehicle spawns");
   Finish();
   return;
  }
  m_TransportVehicle.GetPhysics().SetVelocity(vector.Zero);
  m_TransportVehicle.GetPhysics().SetActive(ActiveState.INACTIVE);
  BaseCompartmentManagerComponent manager = BaseCompartmentManagerComponent.Cast(m_TransportVehicle.FindComponent(BaseCompartmentManagerComponent));
  array<BaseCompartmentSlot> slots = {};
  if (manager)
  {
   manager.GetCompartments(slots);
  }
  if (slots.Count() < 2)
  {
   m_Suite.Record(false, "Native transport has a passenger seat");
   Finish();
   return;
  }
  CompartmentAccessComponent access = ChimeraCharacter.Cast(m_Target).GetCompartmentAccessComponent();
  m_Suite.Record(access.GetInVehicle(m_TransportVehicle, slots[1], true, -1, ECloseDoorAfterActions.INVALID, false), "Patient with active original fluid bag enters native passenger compartment");
  GetGame().GetCallqueue().CallLater(CheckTransportSeat, 1000);
 }
 protected void CheckTransportSeat()
 {
  BCMM_EquipmentItemComponent source = BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(SCR_CompartmentAccessComponent.GetVehicleIn(m_Target) == m_TransportVehicle && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_FluidLineActive(), "Actual seated patient retains active original finite source in patient inventory");
  m_TransportRemaining = source.FluidRemainingMl();
  m_TransportVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  m_TransportVehicle.SetOrigin("130 1 0");
  GetGame().GetCallqueue().CallLater(CheckTransportFlow, 3000);
 }
 protected void CheckTransportFlow()
 {
  BCMM_EquipmentItemComponent source = BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  float used = m_TransportRemaining - source.FluidRemainingMl();
  float delivered = (m_Medical.GetBloodHitZone().GetHealthScaled() - m_TransportVolume) * BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  m_Suite.Record(SCR_CompartmentAccessComponent.GetVehicleIn(m_Target) == m_TransportVehicle && vector.Distance(m_Target.GetOrigin(), m_Actor.GetOrigin()) > 20, "Native compartment carries the patient away from the original medic during vehicle relocation");
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && m_Inventory.Contains(m_Kit) && used > 10 && used < 35 && Math.AbsFloat(delivered - used) < 0.1, "Patient-owned connected bag continues exact finite native flow in relocated vehicle");
  m_Suite.Record(!BCMM_TreatmentAccess.CanReach(m_Actor, m_Target) && !BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Target, BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE, BCMM_BodyRegion.LEFT_ARM), "Existing transport flow does not grant distant medics permission for new vehicle procedures");
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(persistence && persistence.IsTracked(m_TransportVehicle), "Native transport vehicle is tracked before saved-world acceptance");
  if (persistence)
  {
   m_TransportSave.VehicleId = persistence.GetId(m_TransportVehicle);
   m_Suite.Record(m_TransportSave.PackToFile("$profile:BCMM_TransportLine.json"), "Transport manifest preserves exact original vehicle UUID");
  }
  super.SaveFlowingWorld();
 }
}
class BCMM_TransportFluidLineRestartChecks : BCMM_FluidLineRestartRuntimeChecks
{
 override protected void Prepare()
 {
  super.Prepare();
  BCMM_TransportLineManifest transport = new BCMM_TransportLineManifest();
  PersistenceSystem persistence = PersistenceSystem.GetInstance();
  m_Suite.Record(transport.LoadFromFile("$profile:BCMM_TransportLine.json") && persistence, "Separate process reads original transport UUID");
  IEntity vehicle;
  if (persistence)
  {
   vehicle = IEntity.Cast(persistence.FindById(transport.VehicleId));
  }
  m_Suite.Record(vehicle && m_Target && SCR_CompartmentAccessComponent.GetVehicleIn(m_Target) == vehicle, "Native saved-world restores the same patient in the exact original passenger vehicle");
 }
}

