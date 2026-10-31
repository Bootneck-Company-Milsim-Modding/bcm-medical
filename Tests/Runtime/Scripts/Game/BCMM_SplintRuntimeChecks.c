class BCMM_SplintRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected IEntity m_Actor;
 protected IEntity m_Target;
 protected IEntity m_Kit;
 protected IEntity m_Floor;
 protected SCR_CharacterDamageManagerComponent m_Medical;
 protected InventoryStorageManagerComponent m_Inventory;
 protected vector m_Start;
 protected int m_OwnerWait;
 protected IEntity m_OwnerBandage;
 protected IEntity m_OwnerSeal;
 protected IEntity m_OwnerMonitor;
 protected IEntity Spawn(ResourceName resource, vector position)
 {
  EntitySpawnParams params = new EntitySpawnParams();
  params.TransformMode = ETransformMode.WORLD;
  params.Transform[3] = position;
  return GetGame().SpawnEntityPrefab(Resource.Load(resource), GetGame().GetWorld(), params);
 }
 void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  m_Floor = Spawn("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et", "100 0 0");
  m_Actor = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", "100 1.2 0");
  m_Target = Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et", "101 1.2 0");
  GetGame().GetCallqueue().CallLater(Prepare, 1500);
 }
 protected void Prepare()
 {
  string skipAI;
  if (System.GetCLIParam("bcmmSkipAISplint", skipAI))
  {
   m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
   m_Start = m_Actor.GetOrigin();
   m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
   m_Medical.BCMM_PublishFractures();
   PrepareOwner();
   return;
  }
  if (!m_Actor || !m_Target) { m_Suite.Record(false, "Splint fixture characters spawn"); Finish(); return; }
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Medical.BCMM_PublishFractures();
  m_Suite.Record(!BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Splint treatment rejects missing inventory kit");
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Splint fixture equips native backpack");
  m_Kit = Spawn("{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Original splint kit enters native inventory");
  m_Suite.Record(!BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, false), "Splint rejects uninjured opposite leg");
  m_Start = m_Actor.GetOrigin();
  m_Suite.Record(BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Authority starts timed splint treatment");
  m_Suite.Record(!BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Concurrent treatment cannot reserve same patient or kit");
  m_Actor.SetOrigin(m_Start + "3 0 0");
  GetGame().GetCallqueue().CallLater(CheckCancelled, 500);
 }
 protected void CheckCancelled()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !m_Medical.BCMM_GetPatient().LeftLeg.Splinted, "Interrupted treatment preserves exact kit and does not splint");
  m_Actor.SetOrigin(m_Start);
  GetGame().GetCallqueue().CallLater(Restart, 500);
 }
 protected void Restart()
 {
  m_Suite.Record(BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Cancellation releases patient and item reservations");
  GetGame().GetCallqueue().CallLater(CheckApplied, BCMM_TreatmentAccess.Setting("splintApplicationSeconds") * 1000 + 8000);
 }
 protected void CheckApplied()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().LeftLeg.Splinted, "Successful timed treatment consumes kit and splints selected leg");
  m_Suite.Record(m_Medical.BCMM_GetPatient().LeftLeg.Broken && !m_Medical.BCMM_LegsAllowJump(), "Splint does not heal fracture or restore jumping");
  m_Suite.Record(m_Medical.BCMM_MovementCap() > 0.5, "Applied splint improves actual native movement cap");
  m_Suite.Record(!BCMM_SplintTreatment.BeginForActor(m_Actor, m_Target, true), "Already splinted leg rejects repeated treatment");
  string network;
  if (System.GetCLIParam("bcmmSplintNetwork", network)) PrepareOwner();
  else Finish();
 }
 protected void PrepareOwner()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!player || !player.GetControlledEntity())
  {
   if (m_OwnerWait++ < 80) { GetGame().GetCallqueue().CallLater(PrepareOwner, 250); return; }
   m_Suite.Record(false, "Connected owner becomes available for splint RPC");
   Finish();
   return;
  }
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Actor = player.GetControlledEntity();
  m_OwnerWait = 0;
  m_Start = m_Actor.GetOrigin();
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Start);
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Owner receives native backpack for splint kit");
  m_Kit = Spawn("{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et", m_Start);
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Owner receives exact splint for RPC test");
  m_Medical.BCMM_GetPatient().LeftLeg.Splinted = false;
  m_Medical.BCMM_PublishFractures();
  string equipmentOnly;
  if (System.GetCLIParam("bcmmEquipmentOnlyNetwork", equipmentOnly))
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(1, m_Target, 1000000);
   GetGame().GetCallqueue().CallLater(PrepareOwnerEquipment, 15000);
   return;
  }
  m_Medical.BCMM_SetMedic(true);
  string pulseOnly;
  if (System.GetCLIParam("bcmmPulseOnlyNetwork", pulseOnly))
  {
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(1, m_Target, 1000000);
   GetGame().GetCallqueue().CallLater(PrepareOwnerPulse, 10000);
   return;
  }
  float requestDelay = 12;
  string fastFixture;
  if (System.GetCLIParam("bcmmSkipAISplint", fastFixture)) requestDelay = 3;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(1, m_Target, GetGame().GetWorld().GetWorldTime() * 0.001 + requestDelay);
  GetGame().GetCallqueue().CallLater(RecordOwnerAccess, 5000);
  GetGame().GetCallqueue().CallLater(RecordOwnerAccess, 15000);
  GetGame().GetCallqueue().CallLater(CheckOwnerApplied, BCMM_TreatmentAccess.Setting("splintApplicationSeconds") * 1000 + 20000);
 }
 protected void RecordOwnerAccess()
 {
  if (!m_Actor || !m_Target) return;
  CharacterControllerComponent controller = ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  PrintFormat("BCMM_OWNER_ACCESS actor=%1 patient=%2 reach=%3 eligible=%4 kit=%5", m_Actor.GetOrigin(), m_Target.GetOrigin(), BCMM_TreatmentAccess.CanReach(m_Actor, m_Target), BCMM_SplintTreatment.Eligible(m_Target, true), BCMM_SplintTreatment.FindKit(m_Actor) != null);
  PrintFormat("BCMM_OWNER_STATE falling=%1 unconscious=%2 using=%3 raised=%4 life=%5", controller.IsFalling(), controller.IsUnconscious(), controller.IsUsingItem(), controller.IsWeaponRaised(), controller.GetLifeState());
 }
 protected void CheckOwnerApplied()
 {
  if ((!m_Medical.BCMM_GetPatient().LeftLeg.Splinted || m_Kit) && m_OwnerWait++ < 40)
  {
   GetGame().GetCallqueue().CallLater(CheckOwnerApplied, 500);
   return;
  }
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().LeftLeg.Splinted && m_Medical.BCMM_MovementCap() > 0.5, "Connected owner's actual splint RPC consumes kit and changes native movement");
  array<HitZone> zones = {};
  m_Medical.GetHitZonesOfGroupFromOwner(ECharacterHitZoneGroup.LEFTLEG, zones);
  if (!zones.IsEmpty())
  {
   SCR_CharacterHitZone zone = SCR_CharacterHitZone.Cast(zones[0]);
   zone.SetHealth(zone.GetMaxHealth() * 0.5);
   m_Medical.AddBleedingEffectOnHitZone(zone);
  }
  m_OwnerBandage = Spawn("{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_OwnerBandage && m_Inventory.TryInsertItem(m_OwnerBandage, EStoragePurpose.PURPOSE_ANY), "Connected owner receives real native dressing for selected-region bandage");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(2, m_Target, 0);
  m_OwnerWait = 0;
  GetGame().GetCallqueue().CallLater(CheckOwnerBandaged, 20000);
 }
 protected void CheckOwnerBandaged()
 {
  bool treated = !m_OwnerBandage && m_Medical.GetGroupDamageOverTime(ECharacterHitZoneGroup.LEFTLEG, EDamageType.BLEEDING) == 0;
  if (!treated && m_OwnerWait++ < 20) { GetGame().GetCallqueue().CallLater(CheckOwnerBandaged, 500); return; }
  m_Suite.Record(treated, "Actual owner panel request equips, animates and consumes native dressing on selected patient leg");
  m_Medical.BCMM_SetMedic(false);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(3, m_Target, 0);
  string pulseNetwork;
  if (System.GetCLIParam("bcmmPulseOximeterNetwork", pulseNetwork)) { GetGame().GetCallqueue().CallLater(PrepareOwnerPulse, 3000); return; }
  string medicationNetwork;
  if (System.GetCLIParam("bcmmMedicationNetwork", medicationNetwork)) GetGame().GetCallqueue().CallLater(PrepareNetworkMedication, 3000);
  else GetGame().GetCallqueue().CallLater(Finish, 3000);
 }
 protected void PrepareOwnerPulse()
 {
  string pulseOnly;
  if (System.GetCLIParam("bcmmPulseOnlyNetwork", pulseOnly))
  {
   m_Medical.BCMM_GetPatient().LeftLeg.ApplySplint();
   m_Medical.BCMM_PublishFractures(); m_Medical.BCMM_SetMedic(false);
  }
  m_Kit = Spawn("{6AB4B3F3FD216031}Prefabs/Items/BCMM_PulseOximeter.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Connected owner receives the exact supplied pulse-oximeter item");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(8, m_Target, 0);
  GetGame().GetCallqueue().CallLater(CheckOwnerPulse, 18000);
 }
 protected void CheckOwnerPulse()
 {
  m_Suite.Record(m_Medical.BCMM_PulseMonitorActive() && m_Kit && m_Inventory.Contains(m_Kit), "Actual owner RPC animates setup and starts monitoring without consuming its reusable device");
  m_Suite.Record(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("normalHeartRate", 95), "Network live monitor changes a valid physiological baseline");
  GetGame().GetCallqueue().CallLater(PublishOwnerPulse, 2500);
 }
 protected void PublishOwnerPulse()
 {
  PrintFormat("BCMM_PULSE_SERVER active=%1 pulse=%2 flags=%3", m_Medical.BCMM_PulseMonitorActive(), m_Medical.BCMM_Pulse(), m_Medical.BCMM_RegionVisual(5));
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(9, m_Target, 0);
  int observationWindow = 6000;
  string activeJoin;
  if (System.GetCLIParam("bcmmPulseActiveJoin", activeJoin)) observationWindow = 30000;
  GetGame().GetCallqueue().CallLater(LoseOwnerPulseDevice, observationWindow);
 }
 protected void LoseOwnerPulseDevice()
 {
  m_Inventory.TryDeleteItem(m_Kit);
  GetGame().GetCallqueue().CallLater(CheckOwnerPulseStopped, 1000);
 }
 protected void CheckOwnerPulseStopped()
 {
  m_Suite.Record(!m_Medical.BCMM_PulseMonitorActive(), "Network device loss releases live monitoring on the authority");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("normalHeartRate", 75);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(10, m_Target, 0);
  GetGame().GetCallqueue().CallLater(Finish, 3500);
 }
 protected void PrepareNetworkMedication()
 {
  m_Kit = Spawn("{6AB4B3F3FD216012}Prefabs/Items/BCMM_Morphine.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Connected owner receives original finite morphine placeholder");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(4, m_Target, 0);
  GetGame().GetCallqueue().CallLater(CheckNetworkMedication, 28000);
 }
 protected void CheckNetworkMedication()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 1, "Actual owning client animates, consumes and administers exactly one medication");
  m_Suite.Record(m_Medical.BCMM_GetPatient().LeftLeg.Broken, "Connected medication application does not heal fracture");
  GetGame().GetCallqueue().CallLater(PublishMedicationEffect, 25000);
 }
 protected void PublishMedicationEffect()
 {
  m_Suite.Record(m_Medical.BCMM_OxygenSaturation() > 0.96 && m_Medical.BCMM_SystolicPressure() < 120 && m_Medical.BCMM_GetPatient().RespiratoryRate<16, "Authority single-dose opioid slows breathing and lowers pressure without automatic severe hypoxia");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(5, m_Target, 0);
  string equipmentNetwork;
  if (System.GetCLIParam("bcmmEquipmentNetwork", equipmentNetwork)) GetGame().GetCallqueue().CallLater(PrepareOwnerEquipment, 4000);
  else GetGame().GetCallqueue().CallLater(Finish, 4000);
 }
 protected void PrepareOwnerEquipment()
 {
  m_Medical.BCMM_GetPatient().LeftChest.OpenWound = true;
  m_Medical.BCMM_GetPatient().LeftChest.CollapseFraction = 0.3;
  m_OwnerSeal = Spawn("{6AB4B3F3FD216020}Prefabs/Items/BCMM_ChestSeal.et", m_Actor.GetOrigin());
  m_OwnerMonitor = Spawn("{6AB4B3F3FD216030}Prefabs/Items/BCMM_Monitor.et", m_Actor.GetOrigin());
  m_Suite.Record(m_OwnerSeal && m_Inventory.TryInsertItem(m_OwnerSeal, EStoragePurpose.PURPOSE_ANY) && m_OwnerMonitor && m_Inventory.TryInsertItem(m_OwnerMonitor, EStoragePurpose.PURPOSE_ANY), "Connected owner receives actual chest seal and reusable monitor");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(6, m_Target, 0);
  GetGame().GetCallqueue().CallLater(CheckOwnerEquipment, 75000);
 }
 protected void CheckOwnerEquipment()
 {
  m_Suite.Record(!m_OwnerSeal && m_Medical.BCMM_GetPatient().LeftChest.Sealed, "Actual owner's equipment RPC animates and consumes a chest seal on correct casualty");
  m_Suite.Record(m_OwnerMonitor && m_Inventory.Contains(m_OwnerMonitor) && m_Medical.BCMM_GetPatient().Equipment.MonitorChecks == 1, "Actual owner completes one instrument check while retaining the original reusable monitor");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishSplintTest(7, m_Target, 0);
  GetGame().GetCallqueue().CallLater(Finish, 4000);
 }
 protected void Finish()
 {
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  if (m_Target) SCR_EntityHelper.DeleteEntityAndChildren(m_Target);
  if (m_Floor) delete m_Floor;
  m_Suite.Complete();
 }
}