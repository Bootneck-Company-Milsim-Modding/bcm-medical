// Uses an owned native pilot, actual collective inputs and native gravity.
// No aircraft transform/velocity/force overrides are used in this fixture.
class BCMM_PilotInput
{
 static IEntity Aircraft;
 static float Started;
 static float GroundHeight;
 static bool Active;
 static bool RotorReady;
 static float ProgramStarted;
 static float NextLog;
}
modded class SCR_HelicopterControllerComponent
{
 override protected event void OnPrepareControls()
 {
  super.OnPrepareControls();
  if (!BCMM_PilotInput.Active || GetOwner() != BCMM_PilotInput.Aircraft) return;
  PlayerController controller = GetGame().GetPlayerController();
  if (!controller || !GetPilotCompartmentSlot() || GetPilotCompartmentSlot().GetOccupant() != controller.GetControlledEntity()) return;
  VehicleHelicopterSimulation simulation = GetSimulation();
  float now = GetGame().GetWorld().GetWorldTime() * 0.001;
  float targetRpm = simulation.RotorGetRPMTarget(0);
  if (!BCMM_PilotInput.RotorReady && simulation.EngineIsOn() && targetRpm > 0 && simulation.RotorGetRPM(0) >= targetRpm * 0.9)
  {
   BCMM_PilotInput.RotorReady = true;
   BCMM_PilotInput.ProgramStarted = now;
   BCMM_PilotInput.GroundHeight = GetOwner().GetOrigin()[1];
   PrintFormat("BCMM_PILOT_CLIENT rotor ready rpm=%1 target=%2 spoolSeconds=%3", simulation.RotorGetRPM(0), targetRpm, now - BCMM_PilotInput.Started);
  }
  float elapsed = -1;
  if (BCMM_PilotInput.RotorReady) elapsed = now - BCMM_PilotInput.ProgramStarted;
  float target = BCMM_PilotInput.GroundHeight;
  if (BCMM_PilotInput.RotorReady && elapsed <= 35) target += 8;
  // Spawn clearance can be above wheel/skid contact. Continue a gentle native
  // descent below that reference until the real ground-contact test succeeds.
  if (BCMM_PilotInput.RotorReady && elapsed > 35) target -= 1;
  float desiredVelocity = Math.Clamp((target - GetOwner().GetOrigin()[1]) * 0.8, -2, 3);
  float adjustment = Math.Clamp((desiredVelocity - simulation.GetVelocity()[1]) * 0.25, -1, 1);
  InputManager input = GetGame().GetInputManager();
  input.ActivateAction("HelicopterCollectiveIncrease", 1);
  input.ActivateAction("HelicopterCollectiveDecrease", 1);
  input.SetActionValue("HelicopterCollectiveIncrease", Math.Max(0, adjustment));
  input.SetActionValue("HelicopterCollectiveDecrease", Math.Max(0, -adjustment));
  input.ActivateAction("HelicopterCyclicForward", 1);
  float cyclic;
  if (elapsed > 22 && elapsed < 26) cyclic = 0.1;
  input.SetActionValue("HelicopterCyclicForward", cyclic);
  if (GetGame().GetWorld().GetWorldTime() * 0.001 > BCMM_PilotInput.NextLog)
  {
   BCMM_PilotInput.NextLog = GetGame().GetWorld().GetWorldTime() * 0.001 + 5;
   RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
   PrintFormat("BCMM_PILOT_CLIENT controls elapsed=%1 adjustment=%2 height=%3 velocity=%4 rpm=%5 engine=%6 owner=%7", elapsed, adjustment, GetOwner().GetOrigin()[1], simulation.GetVelocity(), simulation.RotorGetRPM(0), simulation.EngineIsOn(), rpl && rpl.IsOwner());
  }
 }
}
modded class SCR_PlayerController
{
 void BCMM_TestPilotStart(RplId aircraft) { Rpc(RpcDo_BCMM_TestPilotStart, aircraft); }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void RpcDo_BCMM_TestPilotStart(RplId aircraft)
 {
  RplComponent rpl = RplComponent.Cast(Replication.FindItem(aircraft));
  if (!rpl || !GetControlledEntity()) return;
  IEntity vehicle = rpl.GetEntity();
  HelicopterControllerComponent controller = HelicopterControllerComponent.Cast(vehicle.FindComponent(HelicopterControllerComponent));
  CompartmentAccessComponent access = CompartmentAccessComponent.Cast(GetControlledEntity().FindComponent(CompartmentAccessComponent));
  bool entered = controller && access && access.GetInVehicle(vehicle, controller.GetPilotCompartmentSlot(), true, -1, ECloseDoorAfterActions.INVALID, true);
  PrintFormat("BCMM_PILOT_CLIENT native pilot entry requested=%1", entered);
  BCMM_PilotInput.Aircraft = vehicle;
  GetGame().GetCallqueue().CallLater(BCMM_TestPilotEngine, 2000, false);
 }
 protected void BCMM_TestPilotEngine()
 {
  HelicopterControllerComponent controller = HelicopterControllerComponent.Cast(BCMM_PilotInput.Aircraft.FindComponent(HelicopterControllerComponent));
  bool seated = controller.GetPilotCompartmentSlot() && controller.GetPilotCompartmentSlot().GetOccupant() == GetControlledEntity();
  PrintFormat("BCMM_PILOT_CLIENT native pilot seated=%1 startEngine=%2", seated, controller.StartEngine());
  controller.GetSimulation().SetThrottle(1);
  controller.SetAutohoverEnabled(true);
  BCMM_PilotInput.GroundHeight = BCMM_PilotInput.Aircraft.GetOrigin()[1];
  BCMM_PilotInput.Started = GetGame().GetWorld().GetWorldTime() * 0.001;
  BCMM_PilotInput.RotorReady = false;
  BCMM_PilotInput.Active = true;
 }
 void BCMM_TestPilotExit(vector position) { Rpc(RpcDo_BCMM_TestPilotExit, position); }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void RpcDo_BCMM_TestPilotExit(vector position)
 {
  BCMM_PilotInput.Active = false;
  HelicopterControllerComponent controller = HelicopterControllerComponent.Cast(BCMM_PilotInput.Aircraft.FindComponent(HelicopterControllerComponent));
  controller.SetPersistentWheelBrake(true);
  controller.StopEngine();
  vector transform[4];
  GetControlledEntity().GetWorldTransform(transform);
  position[1] = Math.Max(position[1], GetGame().GetWorld().GetSurfaceY(position[0], position[2])) + 0.15;
  transform[3] = position;
  CompartmentAccessComponent access = CompartmentAccessComponent.Cast(GetControlledEntity().FindComponent(CompartmentAccessComponent));
  PrintFormat("BCMM_PILOT_CLIENT landed exit=%1", access.GetOutVehicle_NoDoor(transform, false, true));
 }
}
