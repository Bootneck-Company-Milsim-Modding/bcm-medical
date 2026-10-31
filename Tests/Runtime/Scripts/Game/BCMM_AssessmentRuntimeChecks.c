class BCMM_AssessmentRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Vehicle;
 protected IEntity m_Outside;
 override protected void Prepare()
 {
  m_Suite.Record(BCMM_TreatmentAccess.CanExamine(m_Actor, m_Actor), "Conscious player can examine self");
  m_Suite.Record(BCMM_TreatmentAccess.CanExamine(m_Actor, m_Target), "Conscious nearby casualty can be examined");
  SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient().LeftLeg.Broken = true;
  array<IEntity> nearby = {};
  BCMM_AssessmentTargets.Collect(m_Actor, nearby);
  m_Suite.Record(!nearby.Contains(m_Target), "Sidebar excludes a nearby un-aimed character outside the shared vehicle");
  m_Suite.Record((BCMM_ClinicalTreatmentFlags.ForPatient(m_Actor, m_Target, BCMM_BodyRegion.LEFT_LEG) & 2) != 0, "Examined broken leg advertises an applicable splint");
  m_Suite.Record(BCMM_ClinicalTreatmentFlags.ForPatient(m_Actor, m_Target, BCMM_BodyRegion.LEFT_ARM) == 0, "Healthy arm has no actionable wound or fracture treatments");
  m_Vehicle = Spawn("{6AB4B3F3FD216005}Prefabs/Vehicles/BCMMedical/BCMM_CargoJeep.et", "103 1 0");
  m_Outside = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", "120 2 0");
  if (!m_Vehicle) { m_Suite.Record(false, "Assessment jeep spawns"); Finish(); return; }
  m_Vehicle.GetPhysics().SetVelocity(vector.Zero);
  m_Vehicle.GetPhysics().SetActive(ActiveState.INACTIVE);
  BaseCompartmentManagerComponent manager = BaseCompartmentManagerComponent.Cast(m_Vehicle.FindComponent(BaseCompartmentManagerComponent));
  array<BaseCompartmentSlot> slots = {};
  if (manager) manager.GetCompartments(slots);
  if (slots.Count() < 2) { m_Suite.Record(false, "Assessment vehicle has two real seats"); Finish(); return; }
  CompartmentAccessComponent first = ChimeraCharacter.Cast(m_Actor).GetCompartmentAccessComponent();
  CompartmentAccessComponent second = ChimeraCharacter.Cast(m_Target).GetCompartmentAccessComponent();
  m_Suite.Record(first.GetInVehicle(m_Vehicle, slots[0], true, -1, ECloseDoorAfterActions.INVALID, false), "Examiner enters native vehicle seat");
  m_Suite.Record(second.GetInVehicle(m_Vehicle, slots[1], true, -1, ECloseDoorAfterActions.INVALID, false), "Other conscious character enters same vehicle");
  GetGame().GetCallqueue().CallLater(CheckSeated, 1000);
 }
 protected void CheckSeated()
 {
  m_Suite.Record(SCR_CompartmentAccessComponent.GetVehicleIn(m_Actor) == m_Vehicle && SCR_CompartmentAccessComponent.GetVehicleIn(m_Target) == m_Vehicle, "Both assessment subjects occupy actual same vehicle");
  m_Suite.Record(BCMM_TreatmentAccess.CanExamine(m_Actor, m_Target) && BCMM_TreatmentAccess.CanExamine(m_Actor, m_Actor), "Server permits own and same-vehicle occupant assessments");
  m_Suite.Record(!BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), "Viewing in vehicle does not bypass treatment restrictions");
  m_Suite.Record(!BCMM_TreatmentAccess.CanExamine(m_Actor, m_Outside), "Vehicle examination cannot reveal unrelated distant character");
  array<IEntity> candidates = {};
  BCMM_AssessmentTargets.Collect(m_Actor, candidates);
  m_Suite.Record(candidates.Contains(m_Actor) && candidates.Contains(m_Target) && !candidates.Contains(m_Outside), "Self/occupant switch lists real vehicle occupants without outsiders");
  if (m_Vehicle) SCR_EntityHelper.DeleteEntityAndChildren(m_Vehicle);
  if (m_Outside) SCR_EntityHelper.DeleteEntityAndChildren(m_Outside);
  Finish();
 }
}