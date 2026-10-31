// Unpublished network acceptance observers.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMSplintPhase;
 [RplProp()] protected RplId m_BCMMSplintPatient = RplId.Invalid();
 [RplProp()] protected float m_BCMMSplintRequestAt;
 void BCMM_PublishSplintTest(int phase, IEntity patient, float requestAt)
 {
  m_BCMMSplintPatient = BCMM_BodyBagComponent.Id(patient);
  m_BCMMSplintPhase = phase;
  m_BCMMSplintRequestAt = requestAt;
  Replication.BumpMe();
  PrintFormat("BCMM_SPLINT_NET_PHASE %1", phase);
 }
 int BCMM_SplintPhase() { return m_BCMMSplintPhase; }
 RplId BCMM_SplintPatient() { return m_BCMMSplintPatient; }
 float BCMM_SplintRequestAt() { return m_BCMMSplintRequestAt; }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMObservedSplint;
 protected bool m_BCMMPulseRequested;
 protected float m_BCMMPulseReportAt;
 protected bool m_BCMMSplintRequested;
 protected bool m_BCMMSplintCancelledOnce;
 protected bool m_BCMMSplintCancelValidated;
 protected float m_BCMMSplintRetryAt;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string observer;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmSplintObserver", observer)) return;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  if (!mode || mode.BCMM_SplintPhase() <= 0) return;
  IEntity target = BCMM_BodyBagComponent.Resolve(mode.BCMM_SplintPatient());
  if (!target) return;
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  if (mode.BCMM_SplintPhase() == 1 && player && player.GetPlayerId() == 1)
  {
   if (!m_BCMMSplintCancelledOnce && player.BCMM_ProcedureStatus().Contains("Splint"))
   {
    m_BCMMSplintCancelledOnce = true;
    m_BCMMSplintRequested = false;
    m_BCMMSplintRetryAt = world.GetWorldTime() + 1000;
    player.BCMM_RequestCancelProcedure();
    Print("BCMM_SPLINT_OWNER_CANCEL_REQUESTED");
   }
   if (m_BCMMSplintCancelledOnce && !m_BCMMSplintCancelValidated && world.GetWorldTime() >= m_BCMMSplintRetryAt && player.BCMM_ProcedureStatus() == "No active procedure")
   {

    m_BCMMSplintCancelValidated = true;
    Print("BCMM_SPLINT_CANCEL_RETAINED_KIT");
   }
  }
  if (!m_BCMMSplintRequested && (!m_BCMMSplintCancelledOnce || m_BCMMSplintCancelValidated) && world.GetWorldTime() >= m_BCMMSplintRetryAt && mode.BCMM_SplintPhase() == 1 && player && player.GetPlayerId() == 1 && world.GetWorldTime() * 0.001 >= mode.BCMM_SplintRequestAt())
  {
   m_BCMMSplintRequested = true;
   ChimeraCharacter actor = ChimeraCharacter.Cast(player.GetControlledEntity());
   RplComponent actorRpl = RplComponent.Cast(actor.FindComponent(RplComponent));
   PrintFormat("BCMM_OWNER_CLIENT actor=%1 patient=%2 reach=%3 owner=%4", actor.GetOrigin(), target.GetOrigin(), BCMM_TreatmentAccess.CanReach(actor, target), actorRpl.IsOwner());
   player.BCMM_RequestSplint(target, true);
   Print("BCMM_SPLINT_OWNER_REQUESTED");
  }
  if (mode.BCMM_SplintPhase() == 8 && player && player.GetPlayerId() == 1 && !m_BCMMPulseRequested)
  {
   IEntity device = BCMM_EquipmentTreatment.FindEquipment(player.GetControlledEntity(), BCMM_EquipmentKind.PULSE_OXIMETER);
   if (!device) return;
   BCMM_MedicalMenu.Open(target);
   BCMM_MedicalMenu.Current().SelectRegion(BCMM_BodyRegion.LEFT_ARM);
   BCMM_MedicalMenu.Current().UseEquipment(BCMM_EquipmentProcedure.PULSE_OXIMETER);
   m_BCMMPulseRequested = true;
   Print("BCMM_PULSE_OWNER_REQUESTED");
  }
  if (mode.BCMM_SplintPhase() <= m_BCMMObservedSplint) return;
  if (mode.BCMM_SplintPhase() == 5)
  {
   SCR_CharacterDamageManagerComponent medicated = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
   if (!medicated || medicated.BCMM_OxygenSaturation() <= 0.96 || medicated.BCMM_SystolicPressure() >= 120) return;
  }
  if (mode.BCMM_SplintPhase() == 6)
  {
   BCMM_MedicalMenu.Open(target);
   GetGame().GetCallqueue().CallLater(BCMM_EquipmentPanelCheck.Run, 500);
  }
  if (mode.BCMM_SplintPhase() == 4)
  {
   BCMM_MedicalMenu.Open(target);
   GetGame().GetCallqueue().CallLater(BCMM_MedicationPanelCheck.Run, 500);
  }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.BCMM_LegsAllowJump()) return;
  if (mode.BCMM_SplintPhase() == 9)
  {
   if (world.GetWorldTime() >= m_BCMMPulseReportAt)
   {
    m_BCMMPulseReportAt = world.GetWorldTime() + 1000;
    PrintFormat("BCMM_PULSE_CLIENT active=%1 pulse=%2 flags=%3", medical.BCMM_PulseMonitorActive(), medical.BCMM_Pulse(), medical.BCMM_RegionVisual(5));
   }
   if (!medical.BCMM_PulseMonitorActive() || medical.BCMM_Pulse() < 90 || (medical.BCMM_RegionVisual(5) & 3) != 3) return;
   BCMM_MedicalMenu.Open(target);
   GetGame().GetCallqueue().CallLater(BCMM_PulseNetworkPanelCheck.Live, 1200);
  }
  if (mode.BCMM_SplintPhase() == 10)
  {
   if (medical.BCMM_PulseMonitorActive()) return;
   GetGame().GetCallqueue().CallLater(BCMM_PulseNetworkPanelCheck.Stopped, 1200);
  }
  int rolePhase = mode.BCMM_SplintPhase();
  string equipmentOnly;
  if (!System.GetCLIParam("bcmmEquipmentOnlyNetwork", equipmentOnly))
  {
   bool expectedMedic = rolePhase < 3;
   if (medical.BCMM_IsMedic() != expectedMedic) return;
   PrintFormat("BCMM_MEDIC_ROLE_OBSERVED phase=%1 medic=%2", rolePhase, medical.BCMM_IsMedic());
  }
  if (mode.BCMM_SplintPhase() == 1 && medical.BCMM_MovementCap() >= 0.5) return;
  if (mode.BCMM_SplintPhase() == 2 && medical.BCMM_MovementCap() <= 0.5) return;
  if (mode.BCMM_SplintPhase() == 2)
  {
   BCMM_MedicalMenu.Open(target);
   GetGame().GetCallqueue().CallLater(BCMM_PanelCheck.Run, 500);
  }
  m_BCMMObservedSplint = mode.BCMM_SplintPhase();
  PrintFormat("BCMM_SPLINT_NET_OBSERVED phase=%1 cap=%2 jump=%3", m_BCMMObservedSplint, medical.BCMM_MovementCap(), medical.BCMM_LegsAllowJump());
 }
}
class BCMM_PanelCheck
{
 static void Run()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu || !menu.GetRootWidget() || !menu.GetRootWidget().FindAnyWidget("Assessment") || !menu.GetRootWidget().FindAnyWidget("Region5") || !menu.GetRootWidget().FindAnyWidget("Close"))
  {
   Print("BCMM_CHECK FAILED: Native medical panel instantiates with assessment and treatment controls", LogLevel.ERROR);
   return;
  }
  TextWidget initial = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (!initial || initial.GetText() != "") { Print("BCMM_CHECK FAILED: Opening medical panel reveals no findings", LogLevel.ERROR); return; }
  for (int region = 0; region < 7; region++)
  {
   Widget zone = menu.GetRootWidget().FindAnyWidget("Region" + region.ToString());
   BCMM_AnatomyRegion handler;
   if (zone) handler = BCMM_AnatomyRegion.Cast(zone.FindHandler(BCMM_AnatomyRegion));
   if (!handler || !handler.OnClick(zone, 0, 0, 0) || menu.SelectedRegion() != region)
   {
    Print("BCMM_CHECK FAILED: Every anatomy hit region maps its own clinical body zone", LogLevel.ERROR);
    return;
   }
  }
  menu.SelectRegion(BCMM_BodyRegion.LEFT_LEG);
  if (initial.GetText() != "") { Print("BCMM_CHECK FAILED: Selecting anatomy alone reveals no findings", LogLevel.ERROR); return; }
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player && player.GetPlayerId() == 1) menu.CheckBleeding();
  GetGame().GetCallqueue().CallLater(CheckAssessment, 2500);
 }
 static void CheckAssessment()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (!menu || !player)
  {
   Print("BCMM_CHECK FAILED: Medical panel remains available for examination reply", LogLevel.ERROR);
   return;
  }
  TextWidget assessment = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (player.GetPlayerId() == 1 && (!assessment || !assessment.GetText().Contains("Bleeding points:") || !assessment.GetText().Contains("Fracture supported")))
  {
   Print("BCMM_CHECK FAILED: Owner medical panel receives real server body-region assessment", LogLevel.ERROR);
   return;
  }
  if (player.GetPlayerId() != 1 && assessment && assessment.GetText() != "")
  {
   Print("BCMM_CHECK FAILED: Region examination findings remain private to the checking player", LogLevel.ERROR);
   return;
  }
  string before = assessment.GetText();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  array<int> staleCounts = {0, 0, 0, 0};
  BCMM_MedicalMenu.ReceiveClinicalCheck(mode.BCMM_SplintPatient(), -1, BCMM_BodyRegion.LEFT_LEG, BCMM_ClinicalCheck.BLEEDING, true, "stale reply", staleCounts);
  if (assessment.GetText() != before) { Print("BCMM_CHECK FAILED: Stale panel generation cannot replace findings", LogLevel.ERROR); return; }
  BCMM_InputAcceptance.Run();
  Print("BCMM_MEDICAL_PANEL_OPENED");
  if (player.GetPlayerId() == 1) menu.BandageSelectedRegion();
  else menu.Close();
 }
}class BCMM_InputAcceptance
{
 static void Run()
 {
  InputBinding binding = GetGame().GetInputManager().CreateUserBinding();
  array<string> keys = {};
  binding.GetBindings("BCMM_OpenMedical", keys, EInputDeviceType.KEYBOARD, "click", false);
  if (keys.Count() != 1 || !keys[0].Contains("KC_H")) { Fail("Default medical key is H"); return; }
  binding.CreateUserBinding("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click");
  for (int i = binding.GetBindingsCount("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click") - 1; i >= 0; i--) binding.RemoveBinding("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click", i);
  binding.AddBinding("BCMM_OpenMedical", "click", "keyboard:KC_J", "InputFilterDown");
  binding.Save();
  keys.Clear();
  binding.GetBindings("BCMM_OpenMedical", keys, EInputDeviceType.KEYBOARD, "click", false);
  if (keys.Count() != 1 || !keys[0].Contains("KC_J")) { Fail("Medical key can change from H to J"); return; }
  if (!binding.SetFilter("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click", 0, "InputFilterHoldOnce")) { Fail("Medical binding accepts hold-to-open filter"); return; }
  binding.Save();
  if (binding.GetFilter("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click", 0) != "InputFilterHoldOnce") { Fail("Medical hold mode reads back correctly"); return; }
  binding.ResetDefault("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click");
  binding.Save();
  keys.Clear();
  binding.GetBindings("BCMM_OpenMedical", keys, EInputDeviceType.KEYBOARD, "click", false);
  if (keys.Count() != 1 || !keys[0].Contains("KC_H") || binding.GetFilter("BCMM_OpenMedical", EInputDeviceType.KEYBOARD, "click", 0) != "InputFilterDown") { Fail("Medical binding returns to H and tap mode"); return; }
  Print("BCMM_MEDICAL_BINDING_REBIND_HOLD_RESET_PASSED");
 }
 static void Fail(string reason) { Print("BCMM_CHECK FAILED: " + reason, LogLevel.ERROR); }
}
class BCMM_MedicationPanelCheck
{
 static void Run()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { Fail("Medication panel opens on each connected client"); return; }
  Widget pain = menu.GetRootWidget().FindAnyWidget("TabMedication");
  Widget medication = menu.GetRootWidget().FindAnyWidget("TabMedication");
  if (!pain || !medication || !pain.IsEnabled() || !medication.IsEnabled()) { Fail("Pain and medication tabs enabled"); return; }
  menu.SelectRegion(BCMM_BodyRegion.LEFT_ARM);
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player.GetPlayerId() == 1) menu.CheckPain();
  GetGame().GetCallqueue().CallLater(CheckPain, 2600);
 }
 static void CheckPain()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  TextWidget text = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (player.GetPlayerId() == 1)
  {
   if (!text.GetText().Contains("pain")) { Fail("Private checked pain report reaches owner"); return; }
   menu.CheckMedication();
  }
  else if (text.GetText() != "") { Fail("Pain report remains private"); return; }
  GetGame().GetCallqueue().CallLater(CheckRecord, 2600);
 }
 static void CheckRecord()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  TextWidget text = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (player.GetPlayerId() == 1)
  {
   if (!text.GetText().Contains("Recorded BCM")) { Fail("Medication record arrives only after explicit check"); return; }
   menu.AdministerMedication(BCMM_MedicationKind.MORPHINE);
  }
  else if (text.GetText() != "") { Fail("Medication record remains private"); return; }
  Print("BCMM_MEDICATION_PANEL_PRIVATE_CHECKS_PASSED");
  if (player.GetPlayerId() != 1) menu.Close();
 }
 static void Fail(string reason) { Print("BCMM_CHECK FAILED: " + reason, LogLevel.ERROR); }
}
class BCMM_EquipmentPanelCheck
{
 protected static int s_MonitorWait;
 static void Run()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { Fail("Advanced equipment panel opens"); return; }
  menu.SelectRegion(BCMM_BodyRegion.CHEST);
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player.GetPlayerId() == 1) menu.CheckBreathing();
  GetGame().GetCallqueue().CallLater(UseSeal, 2600);
 }
 static void UseSeal()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player.GetPlayerId() == 1) menu.UseEquipment(BCMM_EquipmentProcedure.SEAL_LEFT);
  GetGame().GetCallqueue().CallLater(CheckPulse, 22000);
 }
 static void CheckPulse()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player.GetPlayerId() == 1 && player.BCMM_HasProcedure()) { GetGame().GetCallqueue().CallLater(CheckPulse, 500); return; }
  if (player.GetPlayerId() == 1) menu.CheckPulse();
  GetGame().GetCallqueue().CallLater(UseMonitor, 2600);
 }
 static void UseMonitor()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player.GetPlayerId() == 1) menu.UseEquipment(BCMM_EquipmentProcedure.MONITOR);
  GetGame().GetCallqueue().CallLater(CheckMonitor, 11000);
 }
 static void CheckMonitor()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (!menu) { Fail("Instrument panel remains open"); return; }
  TextWidget text = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (player.GetPlayerId() == 1 && (!text || !text.GetText().Contains("Monitor snapshot")))
  {
   if (s_MonitorWait++ < 60) { GetGame().GetCallqueue().CallLater(CheckMonitor, 500); return; }
   Fail("Instrument result arrives through actual private equipment RPC"); return;
  }
  if (player.GetPlayerId() != 1 && text && text.GetText() != "") { Fail("Instrument readings remain private"); return; }
  Print("BCMM_EQUIPMENT_PANEL_PRIVATE_CHECKS_PASSED");
  menu.Close();
 }
 static void Fail(string reason) { Print("BCMM_CHECK FAILED: " + reason, LogLevel.ERROR); }
}
class BCMM_PulseNetworkPanelCheck
{
 static void Live()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { Print("BCMM_CHECK FAILED: connected pulse panel absent", LogLevel.ERROR); return; }
  TextWidget reading = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  TextWidget leg = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("RegionMarker5"));
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  IEntity patient = BCMM_BodyBagComponent.Resolve(mode.BCMM_SplintPatient());
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  int pulse = Math.Round(medical.BCMM_Pulse());
  bool numeric = reading.GetText().Contains(pulse.ToString() + " bpm") || reading.GetText().Contains((pulse - 1).ToString() + " bpm") || reading.GetText().Contains((pulse + 1).ToString() + " bpm");
  if (numeric && pulse >= 90 && reading.GetText().Contains("LIVE PULSE OXIMETER") && leg.GetText().Contains("F") && leg.GetText().Contains("S")) Print("BCMM_PULSE_PANEL_LIVE_PASSED");
  else Print("BCMM_CHECK FAILED: connected live pulse or fracture/splint diagram mismatch", LogLevel.ERROR);
 }
 static void Stopped()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { Print("BCMM_CHECK FAILED: connected pulse panel absent on stop", LogLevel.ERROR); return; }
  TextWidget reading = TextWidget.Cast(menu.GetRootWidget().FindAnyWidget("Assessment"));
  if (!reading.GetText().Contains("LIVE PULSE OXIMETER")) Print("BCMM_PULSE_PANEL_STOPPED_PASSED");
  else Print("BCMM_CHECK FAILED: disconnected pulse monitor still advertised live", LogLevel.ERROR);
 }
}
