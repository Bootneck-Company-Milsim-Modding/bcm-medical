// Native rebindable action. Tap/hold filters are personal engine bindings.
modded enum SCR_EActionPrefixType { BCMM_MEDICAL_OPEN = 684002 }
class BCMM_MedicalInput : Managed
{
 protected static ref BCMM_MedicalInput s_Instance;
 static void Ensure()
 {
  if (s_Instance || !GetGame().GetInputManager() || !GetGame().GetPlayerController()) return;
  s_Instance = new BCMM_MedicalInput();
  GetGame().GetInputManager().AddActionListener("BCMM_OpenMedical", EActionTrigger.DOWN, s_Instance.OpenMedical);
 }
 static void Reset()
 {
  if (s_Instance && GetGame().GetInputManager()) GetGame().GetInputManager().RemoveActionListener("BCMM_OpenMedical", EActionTrigger.DOWN, s_Instance.OpenMedical);
  s_Instance = null;
  BCMM_ProcedureHUD.Reset();
 }
 protected void OpenMedical()
 {
  if (BCMM_MedicalMenu.Current())
  {
   // Leave input dispatch before removing the menu and its input listeners.
   // Bind the callback to this exact panel so a later view cannot be closed.
   GetGame().GetCallqueue().CallLater(CloseCurrentPanel,0,false,BCMM_MedicalMenu.Current());return;
  }
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (!player || !player.GetControlledEntity()) return;
  ChimeraCharacter actor = ChimeraCharacter.Cast(player.GetControlledEntity());
  if (!actor || actor.GetCharacterController().IsUnconscious() || actor.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE) return;
  IEntity target = BCMM_AssessmentTargets.LookingAt(actor);
  if (!target) target = actor;
  BCMM_MedicalMenu.Open(target);
 }
 protected static void CloseCurrentPanel(BCMM_MedicalMenu panel)
 {
  if(panel && BCMM_MedicalMenu.Current()==panel)panel.Close();
 }
}
class BCMM_AssessmentTargets
{
 static IEntity LookingAt(IEntity actor)
 {
  CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
  if (!camera || !actor) return null;
  vector transform[4];
  camera.GetWorldTransform(transform);
  TraceParam trace = new TraceParam();
  trace.Start = transform[3];
  trace.End = trace.Start + transform[2] * (BCMM_TreatmentAccess.Setting("treatmentRangeMetres") + vector.Distance(trace.Start, actor.GetOrigin()));
  trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  trace.LayerMask = EPhysicsLayerPresets.Projectile;
  trace.Exclude = actor;
  actor.GetWorld().TraceMove(trace);
  IEntity hit = trace.TraceEnt;
  for (int guard = 0; hit && guard < 16; guard++)
  {
   if (ChimeraCharacter.Cast(hit) && BCMM_TreatmentAccess.CanExamine(actor, hit)) return hit;
   hit = hit.GetParent();
  }
  return null;
 }
 static void Collect(IEntity actor, notnull array<IEntity> targets)
 {
  targets.Clear();
  if (!actor) return;
  targets.Insert(actor);
  // Sidebar discovery is restricted to occupants of the same vehicle.
  // Direct look-at targeting remains the independent H-key path.
  IEntity vehicle = SCR_CompartmentAccessComponent.GetVehicleIn(actor);
  if (!vehicle) return;
  BaseCompartmentManagerComponent manager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
  if (!manager) return;
  array<BaseCompartmentSlot> slots = {};
  manager.GetCompartments(slots);
  foreach (BaseCompartmentSlot slot : slots)
  {
   if (!slot) continue;
   IEntity occupant = slot.GetOccupant();
   if (ChimeraCharacter.Cast(occupant) && !targets.Contains(occupant) && BCMM_TreatmentAccess.CanExamine(actor, occupant)) targets.Insert(occupant);
  }
 }
}
modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  if (!world.IsEditMode()) { BCMM_MedicalInput.Ensure(); BCMM_ProcedureHUD.Update(); }
 }
}
// A bounded local discovery query; every entry still obeys authoritative examination access.
class BCMM_NearbyPatients
{
 protected IEntity m_Actor;
 protected ref array<IEntity> m_Found = {};
 static void Add(IEntity actor, notnull array<IEntity> targets)
 {
  BCMM_NearbyPatients query = new BCMM_NearbyPatients();
  query.m_Actor = actor;
  actor.GetWorld().QueryEntitiesBySphere(actor.GetOrigin(), BCMM_TreatmentAccess.Setting("treatmentRangeMetres"), query.Collect, null, EQueryEntitiesFlags.DYNAMIC);
  // Nearest first; self and directly aimed patient retain their existing priority.
  while (!query.m_Found.IsEmpty())
  {
   int closest;
   float distance = float.MAX;
   for (int i = 0; i < query.m_Found.Count(); i++)
   {
    float candidate = vector.Distance(actor.GetOrigin(), query.m_Found[i].GetOrigin());
    if (candidate < distance) { closest = i; distance = candidate; }
   }
   IEntity patient = query.m_Found[closest];
   query.m_Found.Remove(closest);
   if (!targets.Contains(patient)) targets.Insert(patient);
  }
 }
 protected bool Collect(IEntity entity)
 {
  ChimeraCharacter patient = ChimeraCharacter.Cast(entity);
  if (patient && !m_Found.Contains(patient) && BCMM_TreatmentAccess.CanExamine(m_Actor, patient)) m_Found.Insert(patient);
  return true;
 }
}
