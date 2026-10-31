// Unpublished acceptance of the exact reusable device, live UI and safe release.
class BCMM_PulseOximeterRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected BCMM_MedicalMenu m_Menu;
 protected AudioHandle m_BeepProbe;
 protected AudioHandle m_ControlAudio;
 protected int m_AudioProbeCount;
 override protected void Prepare()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  AIControlComponent ai = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  ai = AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Pulse-oximeter fixture equips real inventory storage");
  m_Kit = Spawn("{6AB4B3F3FD216031}Prefabs/Items/BCMM_PulseOximeter.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Kit.GetVObject() && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Supplied pulse-oximeter mesh is a real usable inventory item");
  string audioProbe;
  if (System.GetCLIParam("bcmmPulseAudio", audioProbe))
  {
   SCR_PlayerController cameraOwner = SCR_PlayerController.Cast(GetGame().GetPlayerController());
   if (cameraOwner.GetPlayerCamera()) GetGame().GetCameraManager().SetCamera(cameraOwner.GetPlayerCamera());
   AudioSystem.Resume((1 << AudioSystem.SFX) | (1 << AudioSystem.UI));
   m_ControlAudio = AudioSystem.PlaySound("{E89D9A1F4BA63CDC}Sounds/Props/Furniture/Piano/Samples/Props_Piano_Jingle_1.wav");
   m_BeepProbe = AudioSystem.PlaySound("{6AB4B3F3FD219301}Sounds/Medical/BCMM_Pulse.wav");
   PrintFormat("BCMM_PULSE_AUDIO handle=%1 immediate=%2", m_BeepProbe, AudioSystem.IsSoundPlayed(m_BeepProbe));
   GetGame().GetCallqueue().CallLater(CheckAudioPlayback, 20);
  }
  m_Suite.Record(!m_Medical.BCMM_PulseMonitorActive(), "Owning an oximeter does not automatically reveal live measurements");
  GetGame().GetCallqueue().CallLater(BeginMonitoring, 1500);
 }
 protected void CheckAudioPlayback()
 {
  bool played = AudioSystem.IsSoundPlayed(m_BeepProbe);
  if (!played && m_AudioProbeCount++ < 3) { GetGame().GetCallqueue().CallLater(CheckAudioPlayback, 20); return; }
  PrintFormat("BCMM_AUDIO_COMPARE own=%1 native=%2 controlHandle=%3", played, AudioSystem.IsSoundPlayed(m_ControlAudio), m_ControlAudio);
  AudioSystem.TerminateSound(m_ControlAudio);
  m_Suite.Record(played, "Original pulse beep is accepted by native audio playback after the audio thread starts");
 }
 protected void BeginMonitoring()
 {
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Target, BCMM_EquipmentProcedure.PULSE_OXIMETER, BCMM_BodyRegion.LEFT_ARM), "Pulse oximetry starts through the production equipment policy");
  GetGame().GetCallqueue().CallLater(CheckPreparing, 1800);
  GetGame().GetCallqueue().CallLater(CheckStarted, 14000);
 }
 protected void CheckPreparing()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), "Real oximeter setup plays native treatment item animation");
  m_Suite.Record(!m_Medical.BCMM_PulseMonitorActive() && m_Inventory.Contains(m_Kit), "No live readings appear before timed setup completes");
 }
 protected void CheckStarted()
 {
  m_Suite.Record(m_Medical.BCMM_PulseMonitorActive() && m_Kit && m_Inventory.Contains(m_Kit), "Completed setup activates monitoring and retains the original reusable device");
  BCMM_MedicalMenu.Open(m_Target);
  m_Menu = BCMM_MedicalMenu.Current();
  m_Suite.Record(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("normalHeartRate", 95), "Live-reading fixture changes the baseline within its allowed GM range");
  GetGame().GetCallqueue().CallLater(CheckLiveReading, 2200);
 }
 protected void CheckLiveReading()
 {
  TextWidget status = TextWidget.Cast(m_Menu.GetRootWidget().FindAnyWidget("Assessment"));
  PrintFormat("BCMM_PULSE_LIVE active=%1 pulse=%2 status=%3", m_Medical.BCMM_PulseMonitorActive(), m_Medical.BCMM_Pulse(), status.GetText());
  m_Suite.Record(status.GetText().Contains("LIVE PULSE OXIMETER") && status.GetText().Contains("95 bpm"), "Circulation shows changing numerical monitor readings without a manual pulse check");
  m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Medical.BCMM_PublishFractures(); m_Menu.RefreshDiagram();
  TextWidget marker = TextWidget.Cast(m_Menu.GetRootWidget().FindAnyWidget("RegionMarker5"));
  m_Suite.Record(marker.GetText() == "F", "Replicated fracture state produces a marker on the correct body leg");
  m_Medical.BCMM_GetPatient().LeftLeg.ApplySplint();
  m_Medical.BCMM_PublishFractures(); m_Menu.RefreshDiagram();
  m_Suite.Record(marker.GetText() == "F S", "Splint marker preserves the underlying fracture instead of claiming a healed leg");
  m_Suite.Record(TextWidget.Cast(m_Menu.GetRootWidget().FindAnyWidget("RegionMarker6")).GetText().IsEmpty(), "An injury on the left leg does not leak a fracture marker onto the healthy right leg");
  m_Start = m_Actor.GetOrigin(); m_Actor.SetOrigin(m_Start + "5 0 0");
  GetGame().GetCallqueue().CallLater(CheckOutOfRange, 800);
 }
 protected void CheckOutOfRange()
 {
  m_Suite.Record(!m_Medical.BCMM_PulseMonitorActive(), "Moving the operator away stops live monitoring");
  m_Actor.SetOrigin(m_Start);
  GetGame().GetCallqueue().CallLater(RestartMonitoring, 800);
 }
 protected void RestartMonitoring()
 {
  BCMM_PulseMonitorSession.Start(m_Actor, m_Target, m_Kit);
  m_Suite.Record(m_Medical.BCMM_PulseMonitorActive(), "Returning with the same device permits a new monitoring session");
  m_Inventory.TryDeleteItem(m_Kit);
  GetGame().GetCallqueue().CallLater(CheckDeviceLost, 600);
 }
 protected void CheckDeviceLost()
 {
  m_Suite.Record(!m_Medical.BCMM_PulseMonitorActive(), "Losing the exact device stops live monitoring without a permanent hidden sensor");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("normalHeartRate", 75);
  m_Menu.Close();
  Finish();
 }
}
