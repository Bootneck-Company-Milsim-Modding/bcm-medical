// Full default-time native item integration. Diagnostic addon only.
class BCMM_EquipmentRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Step;
 protected IEntity m_AED;
 protected int m_InternalSource;
 protected float m_ReserveBefore;
 protected ref array<int> m_Steps = {0, 2, 7, 8, 9, 4, 5, 6, 14, 10, 12, 13};
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor = Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et", "100 2 0");
  m_Actor.SetOrigin("100 2.25 0"); m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent)); if(ai)ai.DeactivateAI();
  ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent)); if(ai)ai.DeactivateAI();
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Advanced equipment fixture equips native backpack");
  m_Medical.BCMM_GetPatient().LeftChest.OpenWound = true;
  m_Medical.BCMM_GetPatient().LeftChest.Tension = true;
  m_Medical.BCMM_GetPatient().LeftChest.CollapseFraction = 0.3;
  NextStep();
 }
 protected void NextStep()
 {
  int action = m_Steps[m_Step];
  BCMM_Patient patient = m_Medical.BCMM_GetPatient();
  if (m_Step == 2)
  {
   patient.HasSpontaneousCirculation = false;
   patient.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
   patient.OxygenSaturation = 0.98;
  }
  if (m_Step == 5) patient.Airway = BCMM_AirwayCondition.SOFT_TISSUE_OBSTRUCTION;
  if (m_Step == 6) patient.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  if (m_Step == 9)
  {
   m_Medical.FullHeal();
   patient = m_Medical.BCMM_GetPatient();
   m_InternalSource = patient.AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 1);
   patient.AdvanceInternalTrauma(200, BCMM_Activity.RESTING, new BCMM_Settings());
  }
  if (m_Step == 10) m_Medical.FullHeal();
  if (m_Step == 11)
  {
   m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth() * 0.5);
   m_ReserveBefore = m_Medical.GetBloodHitZone().GetHealthScaled();
  }
  int kind = BCMM_EquipmentPolicy.Kind(action);
  if (kind == BCMM_EquipmentKind.AED && m_AED) m_Kit = m_AED;
  else
  {
   array<ResourceName> resources = {
    "{6AB4B3F3FD216020}Prefabs/Items/BCMM_ChestSeal.et",
    "{6AB4B3F3FD216021}Prefabs/Items/BCMM_NCD.et",
    "{6AB4B3F3FD216022}Prefabs/Items/BCMM_AirwayAdjunct.et",
    "{6AB4B3F3FD216023}Prefabs/Items/BCMM_Suction.et",
    "{6AB4B3F3FD216024}Prefabs/Items/BCMM_BVM.et",
    "{6AB4B3F3FD216025}Prefabs/Items/BCMM_AED.et",
    "{6AB4B3F3FD216026}Prefabs/Items/BCMM_AEDPads.et",
    "{6AB4B3F3FD216027}Prefabs/Items/BCMM_ChestDrain.et",
    "{6AB4B3F3FD216028}Prefabs/Items/BCMM_VascularAccess.et",
    "{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",
    "{6AB4B3F3FD216030}Prefabs/Items/BCMM_Monitor.et"
   };
   m_Kit = Spawn(resources[kind], m_Actor.GetOrigin());
   m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Actual advanced equipment enters inventory: " + BCMM_EquipmentPolicy.Name(action));
   if (kind == BCMM_EquipmentKind.AED) m_AED = m_Kit;
  }
  GetGame().GetCallqueue().CallLater(BeginStep, 2000);
 }
 protected int Region()
 {
  if (m_Step == 5 || m_Step == 6) return BCMM_BodyRegion.HEAD;
  if (m_Step >= 10) return BCMM_BodyRegion.LEFT_ARM;
  return BCMM_BodyRegion.CHEST;
 }
 protected void BeginStep()
 {
  Print("BCMM_EQUIPMENT_STEP "+BCMM_EquipmentPolicy.Name(m_Steps[m_Step]));
  int action = m_Steps[m_Step];
  if (m_Step == 1) m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Target, BCMM_EquipmentProcedure.NCD_RIGHT, BCMM_BodyRegion.CHEST), "Wrong-side decompression retains available needle");
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Target, action, Region()), "Native timed equipment procedure starts: " + BCMM_EquipmentPolicy.Name(action));
  float seconds = BCMM_TreatmentAccess.Setting(BCMM_EquipmentPolicy.DurationKey(action));
  if (seconds >= 10) GetGame().GetCallqueue().CallLater(CheckWorking, seconds * 500);
  GetGame().GetCallqueue().CallLater(CheckStep, seconds * 1000 + 8000);
 }
 protected void CheckWorking()
 {
  PrintFormat("BCMM_EQUIPMENT_POSITION actor=%1 patient=%2", m_Actor.GetOrigin(),m_Target.GetOrigin());
  PrintFormat("BCMM_EQUIPMENT_MID action=%1 kit=%2 busy=%3 using=%4 destroyed=%5 terminal=%6 reach=%7", m_Steps[m_Step], m_Kit != null, BCMM_SplintTreatment.IsBusy(m_Actor,m_Target), ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), m_Medical.IsDestroyed(), m_Medical.BCMM_GetPatient().Terminal, BCMM_TreatmentAccess.CanReach(m_Actor,m_Target));
  // Native injection playback can finish before the configurable procedure
  // timer. The exact supply and authority lease must remain until commit.
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && BCMM_SplintTreatment.IsBusy(m_Actor,m_Target), "Advanced procedure retains exact original supply and authority lease at midpoint");
 }
 protected void CheckStep()
 {
  int action = m_Steps[m_Step];
  BCMM_Patient patient = m_Medical.BCMM_GetPatient();
  bool reusable = BCMM_EquipmentPolicy.Reusable(BCMM_EquipmentPolicy.Kind(action));
  m_Suite.Record((reusable && m_Kit) || (!reusable && !m_Kit), "Equipment retains durable device or consumes original supply: " + BCMM_EquipmentPolicy.Name(action));
  switch (m_Step)
  {
   case 0: m_Suite.Record(patient.LeftChest.Sealed && patient.LeftChest.Tension, "Seal closes external chest wound while trapped pressure remains"); break;
   case 1: m_Suite.Record(!patient.LeftChest.Tension && patient.Equipment.LeftDecompressed && patient.LeftChest.CollapseFraction < 0.3, "Decompression removes pressure and enables gradual lung recovery"); break;
   case 2: m_Suite.Record(patient.Equipment.PadsAttached, "Consumed AED pads become applied patient equipment"); break;
   case 3:
    m_Suite.Record(patient.Equipment.LastAnalysisAdvised && patient.Equipment.AnalysisDevice == BCMM_EquipmentItemComponent.Cast(m_AED.FindComponent(BCMM_EquipmentItemComponent)).Device(), "AED records fresh analysis against the exact retained device");
    break;
   case 4:
    m_Suite.Record(patient.ShocksDelivered() == 1 && !patient.HasSpontaneousCirculation && BCMM_EquipmentItemComponent.Cast(m_AED.FindComponent(BCMM_EquipmentItemComponent)).ShocksRemaining() == 19, "One native AED action uses one discharge without automatic ROSC");
    m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Target, BCMM_EquipmentProcedure.SHOCK, BCMM_BodyRegion.CHEST), "Second shock requires a new valid shockable analysis");
    break;
   case 5: m_Suite.Record(patient.Equipment.AirwayAdjunct && patient.Airway == BCMM_AirwayCondition.PATENT, "Actual adjunct clears soft tissue obstruction on unresponsive casualty"); break;
   case 6: m_Suite.Record(patient.Airway == BCMM_AirwayCondition.PATENT, "Retained suction device clears foreign-material obstruction"); break;
   case 7: m_Suite.Record(!patient.HasSpontaneousCirculation && patient.CanVentilate(), "Assisted ventilation does not manufacture circulation"); break;
   case 9: m_Suite.Record(patient.Equipment.LeftDrain && patient.LeftChest.PleuralBloodMl < 200 && !patient.FindInternalBleed(m_InternalSource).Repaired, "Installed drain removes collection while active bleeding source remains"); break;
   case 10: m_Suite.Record(patient.Equipment.VascularAccess, "Separate consumed access kit enables infusion"); break;
   case 11:
    m_Suite.Record(m_Medical.GetBloodHitZone().GetHealthScaled() >= m_ReserveBefore + 0.09 && m_Medical.GetBloodHitZone().GetHealthScaled() <= 1, "Finite bag delivers measured native reserve without exceeding capacity");
    m_Medical.FullHeal();
    m_Suite.Record(!m_Medical.BCMM_GetPatient().Equipment.VascularAccess && !m_Medical.BCMM_GetPatient().Equipment.PadsAttached, "Native FullHeal clears installed equipment state");
    break;
  }
  m_Step++;
  if (m_Step >= m_Steps.Count()) { Finish(); return; }
  NextStep();
 }
}
