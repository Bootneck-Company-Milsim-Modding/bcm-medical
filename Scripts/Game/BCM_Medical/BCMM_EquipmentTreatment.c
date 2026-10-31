// Original authority equipment workflows. Native graphs are development presentation only.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_EquipmentItemComponentClass : ScriptComponentClass {}
class BCMM_EquipmentItemComponent : ScriptComponent
{
 [Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(BCMM_EquipmentKind))] int Kind;
 protected static int s_NextDevice;
 protected int m_Device;
 [RplProp()] protected int m_Shocks;
 [RplProp()] protected int m_ShockCapacity;
 [RplProp()] protected float m_OxygenLitres;
 [RplProp()] protected float m_OxygenCapacityLitres;
 [RplProp()] protected float m_FluidCapacityMl;
 [RplProp()] protected float m_FluidRemainingMl;
 int FluidKind()
 {
  if(Kind==BCMM_EquipmentKind.SALINE){return BCMM_FluidKind.SALINE;}
  if(Kind==BCMM_EquipmentKind.PLASMA){return BCMM_FluidKind.PLASMA;}
  return BCMM_FluidKind.WHOLE_BLOOD;
 }
 bool IsFluidBag() { return Kind==BCMM_EquipmentKind.FLUID || Kind==BCMM_EquipmentKind.SALINE || Kind==BCMM_EquipmentKind.PLASMA; }
 float FluidCapacityMl() { return m_FluidCapacityMl; }
 float FluidRemainingMl() { return m_FluidRemainingMl; }
 float ConsumeFluidMl(float requested)
 {
  if(!Replication.IsServer() || !IsFluidBag() || !(requested>0))return 0;
  float used=Math.Min(m_FluidRemainingMl,requested);
  m_FluidRemainingMl=Math.Max(0,m_FluidRemainingMl-used);
  if(used>0)Replication.BumpMe();
  return used;
 }
 float OxygenLitres() { return m_OxygenLitres; }
 float ConsumeOxygen(float seconds) { if(!Replication.IsServer() || Kind!=BCMM_EquipmentKind.OXYGEN || !(seconds>0) || seconds>1) { return 0; }float used=Math.Min(m_OxygenLitres,seconds*BCMM_TreatmentAccess.Setting("oxygenFlowLitresPerMinute")/60);m_OxygenLitres=Math.Max(0,m_OxygenLitres-used);Replication.BumpMe();return used; }
 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  if (Replication.IsServer())
  {
   m_Device = ++s_NextDevice;
   if(IsFluidBag()){m_FluidCapacityMl=BCMM_TreatmentAccess.Setting("fluidBagVolumeMl");m_FluidRemainingMl=m_FluidCapacityMl;}
   if(Kind==BCMM_EquipmentKind.OXYGEN) { m_OxygenCapacityLitres=BCMM_TreatmentAccess.Setting("oxygenCapacityLitres");m_OxygenLitres=m_OxygenCapacityLitres; }
   m_ShockCapacity=BCMM_TreatmentAccess.Setting("aedShocksPerDevice");m_Shocks=m_ShockCapacity;
  }
 }
 float RemainingValueFraction()
 {
  if(IsFluidBag())
  {
   if(m_FluidCapacityMl<=0)return 0;
   return Math.Clamp(m_FluidRemainingMl/m_FluidCapacityMl,0,1);
  }
  if(Kind==BCMM_EquipmentKind.OXYGEN)
  {
   if(m_OxygenCapacityLitres<=0)return 0;
   return Math.Clamp(m_OxygenLitres/m_OxygenCapacityLitres,0,1);
  }
  if(Kind==BCMM_EquipmentKind.AED)
  {
   if(m_ShockCapacity<=0)return 0;
   return Math.Clamp(m_Shocks/(float)m_ShockCapacity,0,1);
  }
  return 1;
 }
 int Device() { return m_Device; }
 int ShocksRemaining() { return m_Shocks; }
 bool Discharge()
 {
  if (!Replication.IsServer() || Kind != BCMM_EquipmentKind.AED || m_Shocks <= 0) return false;
  m_Shocks--;
  Replication.BumpMe();
  return true;
 }
}
class BCMM_EquipmentPolicy
{
 static bool IsLineConnection(int procedure){return procedure>=BCMM_EquipmentProcedure.CONNECT_WHOLE_BLOOD_LINE && procedure<=BCMM_EquipmentProcedure.CONNECT_PLASMA_LINE;}
 static int LineFluidAction(int procedure)
 {
  if(procedure==BCMM_EquipmentProcedure.CONNECT_SALINE_LINE){return BCMM_EquipmentProcedure.INFUSE_SALINE;}
  if(procedure==BCMM_EquipmentProcedure.CONNECT_PLASMA_LINE){return BCMM_EquipmentProcedure.INFUSE_PLASMA;}
  return BCMM_EquipmentProcedure.INFUSE;
 }
 static bool IsInfusion(int procedure) { return procedure==BCMM_EquipmentProcedure.INFUSE || procedure==BCMM_EquipmentProcedure.INFUSE_SALINE || procedure==BCMM_EquipmentProcedure.INFUSE_PLASMA; }
 static int FluidType(int procedure)
 {
  if(procedure==BCMM_EquipmentProcedure.INFUSE_SALINE) { return BCMM_FluidKind.SALINE; }
  if(procedure==BCMM_EquipmentProcedure.INFUSE_PLASMA) { return BCMM_FluidKind.PLASMA; }
  return BCMM_FluidKind.WHOLE_BLOOD;
 }
 static int Kind(int procedure)
 {
  switch (procedure)
  {
   case BCMM_EquipmentProcedure.SEAL_LEFT:
   case BCMM_EquipmentProcedure.SEAL_RIGHT: return BCMM_EquipmentKind.CHEST_SEAL;
   case BCMM_EquipmentProcedure.NCD_LEFT:
   case BCMM_EquipmentProcedure.NCD_RIGHT: return BCMM_EquipmentKind.NCD;
   case BCMM_EquipmentProcedure.INSERT_AIRWAY: return BCMM_EquipmentKind.AIRWAY;
   case BCMM_EquipmentProcedure.SUCTION: return BCMM_EquipmentKind.SUCTION;
   case BCMM_EquipmentProcedure.VENTILATE: return BCMM_EquipmentKind.BVM;
   case BCMM_EquipmentProcedure.APPLY_PADS: return BCMM_EquipmentKind.PADS;
   case BCMM_EquipmentProcedure.ANALYSE:
   case BCMM_EquipmentProcedure.SHOCK: return BCMM_EquipmentKind.AED;
   case BCMM_EquipmentProcedure.DRAIN_LEFT:
   case BCMM_EquipmentProcedure.DRAIN_RIGHT: return BCMM_EquipmentKind.CHEST_DRAIN;
   case BCMM_EquipmentProcedure.INSERT_IV: return BCMM_EquipmentKind.IV_ACCESS;
   case BCMM_EquipmentProcedure.CONNECT_WHOLE_BLOOD_LINE:
   case BCMM_EquipmentProcedure.INFUSE: return BCMM_EquipmentKind.FLUID;
   case BCMM_EquipmentProcedure.CONNECT_SALINE_LINE:
   case BCMM_EquipmentProcedure.INFUSE_SALINE: return BCMM_EquipmentKind.SALINE;
   case BCMM_EquipmentProcedure.CONNECT_PLASMA_LINE:
   case BCMM_EquipmentProcedure.INFUSE_PLASMA: return BCMM_EquipmentKind.PLASMA;
   case BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE: return BCMM_EquipmentKind.IV_ACCESS;
   case BCMM_EquipmentProcedure.CONNECT_OXYGEN:
   case BCMM_EquipmentProcedure.DISCONNECT_OXYGEN: return BCMM_EquipmentKind.OXYGEN;
   case BCMM_EquipmentProcedure.MONITOR: return BCMM_EquipmentKind.MONITOR;
   case BCMM_EquipmentProcedure.PULSE_OXIMETER: return BCMM_EquipmentKind.PULSE_OXIMETER;
  }
  return -1;
 }
 static bool Reusable(int kind)
 {
  return kind==BCMM_EquipmentKind.OXYGEN || kind == BCMM_EquipmentKind.SUCTION || kind == BCMM_EquipmentKind.BVM || kind == BCMM_EquipmentKind.AED || kind == BCMM_EquipmentKind.MONITOR || kind == BCMM_EquipmentKind.PULSE_OXIMETER;
 }
 static string DurationKey(int procedure)
 {
  array<string> keys = {"chestSealApplicationSeconds", "chestSealApplicationSeconds", "decompressionSeconds", "decompressionSeconds", "airwayInsertionSeconds", "suctionSeconds", "ventilationCycleSeconds", "aedPadApplicationSeconds", "aedAnalysisSeconds", "aedShockSeconds", "chestDrainInsertionSeconds", "chestDrainInsertionSeconds", "ivAccessSeconds", "fluidInfusionSeconds", "monitorCheckSeconds", "pulseOximeterApplicationSeconds", "fluidInfusionSeconds", "fluidInfusionSeconds", "oxygenApplicationSeconds", "oxygenRemovalSeconds", "fluidLineConnectionSeconds", "fluidLineConnectionSeconds", "fluidLineConnectionSeconds", "fluidLineRemovalSeconds"};
  if (procedure < 0 || procedure >= keys.Count()) return "";
  return keys[procedure];
 }
 static string Name(int procedure)
 {
  array<string> names = {"Left chest seal", "Right chest seal", "Left decompression", "Right decompression", "Airway adjunct", "Airway suction", "Assisted ventilation", "AED pad application", "AED analysis", "AED discharge - stand clear", "Left chest drain", "Right chest drain", "Vascular access", "Whole blood infusion", "Instrumented vital check", "Start pulse oximetry", "Saline infusion", "Plasma infusion", "Connect oxygen supply", "Disconnect oxygen supply", "Connect whole blood drip", "Connect saline drip", "Connect plasma drip", "Disconnect fluid line"};
  if (procedure < 0 || procedure >= names.Count()) return "Unknown equipment action";
  return names[procedure];
 }
 static bool RegionAllowed(int procedure, int region)
 {
  if (procedure < 0 || procedure > BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE || region < 0 || region > 6) return false;
  if(IsLineConnection(procedure) || procedure==BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE)return region>=BCMM_BodyRegion.LEFT_ARM;
  if(procedure==BCMM_EquipmentProcedure.CONNECT_OXYGEN || procedure==BCMM_EquipmentProcedure.DISCONNECT_OXYGEN) return region==BCMM_BodyRegion.HEAD;
  if (procedure == BCMM_EquipmentProcedure.PULSE_OXIMETER) return region == BCMM_BodyRegion.LEFT_ARM || region == BCMM_BodyRegion.RIGHT_ARM;
  if (procedure == BCMM_EquipmentProcedure.INSERT_AIRWAY || procedure == BCMM_EquipmentProcedure.SUCTION) return region == BCMM_BodyRegion.HEAD;
  if (procedure == BCMM_EquipmentProcedure.INSERT_IV) return region >= BCMM_BodyRegion.LEFT_ARM;
  if (IsInfusion(procedure)) return true;
  return region == BCMM_BodyRegion.CHEST;
 }
 static bool Eligible(BCMM_Patient patient, int procedure, int device, notnull BCMM_Settings settings)
 {
  if (!patient || patient.Terminal || procedure < 0 || procedure > BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE) return false;
  switch (procedure)
  {
   case BCMM_EquipmentProcedure.SEAL_LEFT: return patient.LeftChest.OpenWound && !patient.LeftChest.Sealed;
   case BCMM_EquipmentProcedure.SEAL_RIGHT: return patient.RightChest.OpenWound && !patient.RightChest.Sealed;
   case BCMM_EquipmentProcedure.NCD_LEFT: return patient.LeftChest.Tension;
   case BCMM_EquipmentProcedure.NCD_RIGHT: return patient.RightChest.Tension;
   case BCMM_EquipmentProcedure.INSERT_AIRWAY: return patient.Airway == BCMM_AirwayCondition.SOFT_TISSUE_OBSTRUCTION && !patient.Equipment.AirwayAdjunct;
   case BCMM_EquipmentProcedure.SUCTION: return patient.Airway == BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
   case BCMM_EquipmentProcedure.VENTILATE: return patient.CanVentilate() && (!patient.HasSpontaneousCirculation || patient.OxygenSaturation < 0.95 || patient.Medication.BreathingScale < 0.95);
   case BCMM_EquipmentProcedure.APPLY_PADS: return !patient.Equipment.PadsAttached;
   case BCMM_EquipmentProcedure.ANALYSE: return patient.Equipment.PadsAttached;
   case BCMM_EquipmentProcedure.SHOCK: return patient.Equipment.MayShock(patient, device, settings);
   case BCMM_EquipmentProcedure.DRAIN_LEFT: return !patient.Equipment.LeftDrain && patient.LeftChest.PleuralBloodMl > 0;
   case BCMM_EquipmentProcedure.DRAIN_RIGHT: return !patient.Equipment.RightDrain && patient.RightChest.PleuralBloodMl > 0;
   case BCMM_EquipmentProcedure.INSERT_IV: return !patient.Equipment.VascularAccess;
   case BCMM_EquipmentProcedure.INFUSE:
   case BCMM_EquipmentProcedure.INFUSE_SALINE:
   case BCMM_EquipmentProcedure.INFUSE_PLASMA: return patient.Equipment.VascularAccess && (settings.Get("fluidSafetyChecksEnabled")<0.5 || patient.SystolicPressure<settings.Get("fluidMaximumSystolicPressure"));
   case BCMM_EquipmentProcedure.CONNECT_OXYGEN: return settings.Get("oxygenEnabled")>0.5 && !patient.Equipment.OxygenActive;
   case BCMM_EquipmentProcedure.DISCONNECT_OXYGEN: return patient.Equipment.OxygenActive;
   case BCMM_EquipmentProcedure.MONITOR: return true;
   case BCMM_EquipmentProcedure.PULSE_OXIMETER: return true;
  }
  return false;
 }
}
class BCMM_EquipmentTreatment : BCMM_SplintTreatment
{
 protected int m_Action;
 protected int m_Region;
 protected int m_Device;
 protected int m_View;
 protected float m_FluidBudgetMl;
 protected float m_FluidDeliveredMl;
 static IEntity FindEquipment(IEntity actor, int kind)
 {
  if (!actor || kind < 0 || kind >= BCMM_SupplyCatalog.EQUIPMENT_COUNT) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  IEntity bestOxygen;float bestGas;
  foreach (IEntity item : items)
  {
   if (!item || BCMM_FluidLineSession.UsesSource(item) || BCMM_FluidLineTransfer.SourcePending(item)) continue;
   BCMM_EquipmentItemComponent marker=BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
   if(!marker || marker.Kind!=kind) { continue; }
   if(marker.IsFluidBag() && marker.FluidRemainingMl()<=0.05)continue;
   if(kind!=BCMM_EquipmentKind.OXYGEN) { return item; }
   if(marker.OxygenLitres()>bestGas) { bestGas=marker.OxygenLitres();bestOxygen=item; }
  }
  return bestOxygen;
 }
 static bool BeginEquipment(IEntity actor, IEntity target, int action, int region, SCR_PlayerController player = null, int view = 0)
 {
  if (!Replication.IsServer() || (player && player.GetControlledEntity() != actor)) return false;
  if (!BCMM_EquipmentPolicy.RegionAllowed(action, region) || !BCMM_TreatmentAccess.CanReach(actor, target)) return false;
  if (!BCMM_MedicRole.EquipmentAllowed(actor, action)) return false;
  if(BCMM_EquipmentPolicy.IsLineConnection(action))return BCMM_FluidLineConnection.BeginLine(actor,target,BCMM_EquipmentPolicy.LineFluidAction(action),region,player);
  if(action==BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE)return BCMM_FluidLineRemoval.BeginRemoval(actor,target,region,player);
  if(BCMM_EquipmentPolicy.IsInfusion(action) && (BCMM_FluidLineSession.ForPatient(target) || BCMM_FluidLineTransfer.PatientPending(target)))return false;
  if(action==BCMM_EquipmentProcedure.DISCONNECT_OXYGEN)return BCMM_OxygenRemovalTreatment.BeginRemoval(actor,target,player);
  IEntity kit = FindEquipment(actor, BCMM_EquipmentPolicy.Kind(action));
  if (!kit) return false;
  BCMM_EquipmentTreatment operation = new BCMM_EquipmentTreatment();
  operation.m_Actor = actor; operation.m_Target = target; operation.m_Kit = kit;
  operation.m_Action = action; operation.m_Region = region; operation.m_View = view;
  operation.m_Device = BCMM_EquipmentItemComponent.Cast(kit.FindComponent(BCMM_EquipmentItemComponent)).Device();
  BCMM_EquipmentItemComponent fluid=BCMM_EquipmentItemComponent.Cast(kit.FindComponent(BCMM_EquipmentItemComponent));
  float duration=BCMM_TreatmentAccess.Setting(BCMM_EquipmentPolicy.DurationKey(action));
  if(BCMM_EquipmentPolicy.IsInfusion(action))
  {
   operation.m_FluidBudgetMl=fluid.FluidRemainingMl();
   duration*=operation.m_FluidBudgetMl/Math.Max(0.001,fluid.FluidCapacityMl());
  }
  if (!operation.StillEligible()) return false;
  return StartOperation(operation, actor, target, kit, duration, player);
 }
 protected override int ProcedureRegion() { return m_Region; }
 protected override string ProcedureName() { return BCMM_EquipmentPolicy.Name(m_Action); }
 protected override bool ConsumesKit()
 {
  if(BCMM_EquipmentPolicy.IsInfusion(m_Action))
  {
   BCMM_EquipmentItemComponent source;if(m_Kit)source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
   return source && source.FluidRemainingMl()<=0.05;
  }
  return !BCMM_EquipmentPolicy.Reusable(BCMM_EquipmentPolicy.Kind(m_Action));
 }
 protected override void AdvanceProcedure(float now)
 {
  if(!BCMM_EquipmentPolicy.IsInfusion(m_Action) || !m_Kit || m_FluidBudgetMl<=0 || m_Duration<=0)return;
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!source || !medical || !medical.GetBloodHitZone())return;
  float elapsed=Math.Clamp(now-(m_Deadline-m_Duration),0,m_Duration);
  float requested=Math.Min(source.FluidRemainingMl(),Math.Max(0,m_FluidBudgetMl*elapsed/m_Duration-m_FluidDeliveredMl));
  if(requested<=0)return;
  float actualMl=medical.BCMM_DeliverFluidVolumeMl(requested,BCMM_EquipmentPolicy.FluidType(m_Action));
  m_FluidDeliveredMl+=source.ConsumeFluidMl(Math.Min(requested,actualMl));
 }
 protected override bool StillEligible()
 {
  if (!BCMM_MedicRole.EquipmentAllowed(m_Actor, m_Action)) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed()) return false;
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(settings);
  if (!BCMM_EquipmentPolicy.Eligible(medical.BCMM_GetPatient(), m_Action, m_Device, settings)) return false;
  if(m_Action==BCMM_EquipmentProcedure.CONNECT_OXYGEN && BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres()<=0) return false;
  if (m_Action == BCMM_EquipmentProcedure.INSERT_AIRWAY && !ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious()) return false;
  if(BCMM_EquipmentPolicy.IsInfusion(m_Action))
  {
   if(m_FluidBudgetMl<=0.05)return false;
   if(m_FluidDeliveredMl>=m_FluidBudgetMl-0.05)return true;
   return medical.GetBloodHitZone() && BCMM_ClinicalSafety.FluidDenial(medical.BCMM_GetPatient(),medical.GetBloodHitZone().GetHealthScaled(),settings,BCMM_EquipmentPolicy.FluidType(m_Action)).IsEmpty();
  }
  if (m_Action == BCMM_EquipmentProcedure.SHOCK)
  {
   BCMM_EquipmentItemComponent device = BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
   return device && device.ShocksRemaining() > 0;
  }
  return true;
 }
 protected override void ApplyProcedure()
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_Patient patient = medical.BCMM_GetPatient();
  switch (m_Action)
  {
   case BCMM_EquipmentProcedure.SEAL_LEFT: patient.LeftChest.ApplySeal(); break;
   case BCMM_EquipmentProcedure.SEAL_RIGHT: patient.RightChest.ApplySeal(); break;
   case BCMM_EquipmentProcedure.NCD_LEFT: patient.LeftChest.RelieveTension(); patient.Equipment.LeftDecompressed = true; break;
   case BCMM_EquipmentProcedure.NCD_RIGHT: patient.RightChest.RelieveTension(); patient.Equipment.RightDecompressed = true; break;
   case BCMM_EquipmentProcedure.INSERT_AIRWAY: patient.Airway = BCMM_AirwayCondition.PATENT; patient.Equipment.AirwayAdjunct = true; break;
   case BCMM_EquipmentProcedure.SUCTION: patient.Airway = BCMM_AirwayCondition.PATENT; break;
   case BCMM_EquipmentProcedure.VENTILATE: patient.DeliverAssistedVentilation(BCMM_TreatmentAccess.Setting("ventilationSupportSeconds")); break;
   case BCMM_EquipmentProcedure.APPLY_PADS: patient.Equipment.PadsAttached = true; break;
   case BCMM_EquipmentProcedure.ANALYSE:
    patient.Equipment.Analyse(patient, m_Device);
    if (patient.IsShockAdvised()) Reply("AED: shock advised. Stop compressions and stand clear before discharge.");
    else Reply("AED: no shock advised. Assess breathing, circulation and reversible causes.");
    break;
   case BCMM_EquipmentProcedure.SHOCK:
    BCMM_EquipmentItemComponent device = BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
    if (patient.DeliverShock() && device.Discharge()) Reply("AED discharged. Reassess circulation; a shock does not guarantee a pulse.");
    patient.Equipment.AnalysisAge = 1000000;
    break;
   case BCMM_EquipmentProcedure.DRAIN_LEFT: patient.Equipment.LeftDrain = true; break;
   case BCMM_EquipmentProcedure.DRAIN_RIGHT: patient.Equipment.RightDrain = true; break;
   case BCMM_EquipmentProcedure.INSERT_IV: patient.Equipment.VascularAccess = true; patient.Equipment.VascularAccessRegion = m_Region; break;
   case BCMM_EquipmentProcedure.INFUSE:
   case BCMM_EquipmentProcedure.INFUSE_SALINE:
   case BCMM_EquipmentProcedure.INFUSE_PLASMA: break; // Volume is delivered progressively from the reserved physical bag.
   case BCMM_EquipmentProcedure.CONNECT_OXYGEN: BCMM_OxygenSession.Start(m_Actor,m_Target,m_Kit);break;
   case BCMM_EquipmentProcedure.DISCONNECT_OXYGEN: BCMM_OxygenSession.StopForPatient(m_Target);break;
   case BCMM_EquipmentProcedure.PULSE_OXIMETER: BCMM_PulseMonitorSession.Start(m_Actor, m_Target, m_Kit); break;
   case BCMM_EquipmentProcedure.MONITOR:
    patient.Equipment.MonitorChecks++;
    Reply(string.Format("Monitor snapshot\nPulse %1 /min\nSpO2 %2 percent\nSystolic %3\nRespiratory rate %4 /min", Math.Round(patient.HeartRate), Math.Round(patient.OxygenSaturation * 100), Math.Round(patient.SystolicPressure), Math.Round(patient.RespiratoryRate)));
    break;
  }
  BCMM_VisualStatusBridge.Publish(medical);
  if (m_Player) m_Player.BCMM_TreatmentFeedback(ProcedureName() + " completed.");
 }
 protected void Reply(string text)
 {
  if (!m_Player) return;
  array<int> counts = {};
  int bandages, splints, surgery, sutures;
  BCMM_ClinicalChecks.Supplies(m_Actor, bandages, splints, surgery, sutures);
  counts = {bandages, splints, surgery, sutures};
  BCMM_MedicationTreatment.AppendSupplies(m_Actor, counts);
  AppendSupplies(m_Actor, counts);
  m_Player.BCMM_SendClinicalCheck(BCMM_BodyBagComponent.Id(m_Target), m_View, m_Region, 8, true, text, counts);
 }
 static void AppendSupplies(IEntity actor, array<int> counts)
 {
  for (int i = 0; i < BCMM_SupplyCatalog.EQUIPMENT_COUNT; i++) counts.Insert(0);
  if (!actor) return;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item || BCMM_FluidLineSession.UsesSource(item) || BCMM_FluidLineTransfer.SourcePending(item)) continue;
   BCMM_EquipmentItemComponent marker = BCMM_EquipmentItemComponent.Cast(item.FindComponent(BCMM_EquipmentItemComponent));
   if(marker && marker.IsFluidBag() && marker.FluidRemainingMl()<=0.05)continue;
   if (marker && marker.Kind >= 0 && marker.Kind < BCMM_SupplyCatalog.EQUIPMENT_COUNT) counts[BCMM_SupplyCatalog.EQUIPMENT_OFFSET + marker.Kind] = counts[BCMM_SupplyCatalog.EQUIPMENT_OFFSET + marker.Kind] + 1;
  }
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestEquipment(IEntity target, int action, int region, int view)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_EquipmentRequest, BCMM_BodyBagComponent.Id(target), action, region, view);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_EquipmentRequest(RplId target, int action, int region, int view)
 {
  bool started = BCMM_EquipmentTreatment.BeginEquipment(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(target), action, region, this, view);
  string diagnostic;
  if (System.GetCLIParam("bcmmEquipmentNetwork", diagnostic))
  PrintFormat("BCMM_EQUIPMENT_REQUEST action=%1 region=%2 started=%3 reach=%4 kit=%5", action, region, started, BCMM_TreatmentAccess.CanReach(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(target)), BCMM_EquipmentTreatment.FindEquipment(GetControlledEntity(), BCMM_EquipmentPolicy.Kind(action)) != null);
  if (!started) BCMM_EquipmentDebug.Report(GetControlledEntity(), BCMM_BodyBagComponent.Resolve(target), action);
  if (!started)
   BCMM_BandageFeedback.Send(this, "Equipment action unavailable. Check findings, kit, access, pads/analysis and stand-clear conditions.");
 }
}

class BCMM_EquipmentDebug
{
 static float TraceTo(ChimeraCharacter actor, IEntity patient, vector point)
 {
  TraceParam trace = new TraceParam();
  trace.Start = actor.EyePosition(); trace.End = point;
  trace.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  trace.LayerMask = EPhysicsLayerPresets.Projectile;
  array<IEntity> excluded = {actor, patient}; trace.ExcludeArray = excluded;
  return actor.GetWorld().TraceMove(trace);
 }
 static void Report(IEntity actor, IEntity patient, int action)
 {
  string fixture;
  if (!System.GetCLIParam("bcmmEquipmentNetwork", fixture)) return;
  ChimeraCharacter medic = ChimeraCharacter.Cast(actor);
  ChimeraCharacter target = ChimeraCharacter.Cast(patient);
  if (!medic || !target) return;
  CharacterControllerComponent controller = medic.GetCharacterController();
  PrintFormat("BCMM_EQUIPMENT_REJECT actor=%1 patient=%2 distance=%3 using=%4 falling=%5 raised=%6 busy=%7", actor.GetOrigin(), patient.GetOrigin(), vector.Distance(actor.GetOrigin(), patient.GetOrigin()), controller.IsUsingItem(), controller.IsFalling(), controller.IsWeaponRaised(), BCMM_SplintTreatment.IsBusy(actor, patient));
  PrintFormat("BCMM_EQUIPMENT_TRACE origin=%1 aim=%2 eye=%3 unconscious=%4 aimpoint=%5", TraceTo(medic, patient, patient.GetOrigin() + "0 0.7 0"), TraceTo(medic, patient, target.AimingPosition()), TraceTo(medic, patient, target.EyePosition()), target.GetCharacterController().IsUnconscious(), target.AimingPosition());
 }
}