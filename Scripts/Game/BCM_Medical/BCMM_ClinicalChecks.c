// Original targeted examination. No findings are sent merely for opening a panel.
enum BCMM_ClinicalCheck { SUPPLIES, BLEEDING, AIRWAY, BREATHING, PULSE, BLOOD_LOSS, PAIN, MEDICATION, MONITOR, INJURY }
class BCMM_ClinicalChecks
{
 static string RegionName(int region)
 {
  array<string> names = {"Head", "Chest", "Abdomen", "Left arm", "Right arm", "Left leg", "Right leg"};
  if (region < 0 || region >= names.Count()) return "Select body region";
  return names[region];
 }
 static ECharacterHitZoneGroup NativeGroup(int region)
 {
  switch (region)
  {
   case BCMM_BodyRegion.HEAD: return ECharacterHitZoneGroup.HEAD;
   case BCMM_BodyRegion.CHEST: return ECharacterHitZoneGroup.UPPERTORSO;
   case BCMM_BodyRegion.ABDOMEN: return ECharacterHitZoneGroup.LOWERTORSO;
   case BCMM_BodyRegion.LEFT_ARM: return ECharacterHitZoneGroup.LEFTARM;
   case BCMM_BodyRegion.RIGHT_ARM: return ECharacterHitZoneGroup.RIGHTARM;
   case BCMM_BodyRegion.LEFT_LEG: return ECharacterHitZoneGroup.LEFTLEG;
   case BCMM_BodyRegion.RIGHT_LEG: return ECharacterHitZoneGroup.RIGHTLEG;
  }
  return ECharacterHitZoneGroup.VIRTUAL;
 }
 static bool Supports(int region, int check)
 {
  if (region < 0 || region > 6 || check < 0 || check > 9) return false;
  if (check == BCMM_ClinicalCheck.MONITOR) return false;
  if (check == BCMM_ClinicalCheck.AIRWAY) return region == BCMM_BodyRegion.HEAD;
  if (check == BCMM_ClinicalCheck.BREATHING) return region == BCMM_BodyRegion.CHEST;
  if (check == BCMM_ClinicalCheck.PULSE) return region == BCMM_BodyRegion.HEAD || region == BCMM_BodyRegion.CHEST || region == BCMM_BodyRegion.LEFT_ARM || region == BCMM_BodyRegion.RIGHT_ARM;
  return true;
 }
 static string Report(BCMM_Patient patient, int region, int check)
 {
  if (check == BCMM_ClinicalCheck.PAIN)
  {
   if (patient.Medication.Pain >= 65) return "Overall pain: severe";
   if (patient.Medication.Pain >= 30) return "Overall pain: moderate";
   if (patient.Medication.Pain > 0) return "Overall pain: mild";
   return "No marked pain detected";
  }
  if (check == BCMM_ClinicalCheck.MEDICATION) return patient.Medication.History();
  if (check == BCMM_ClinicalCheck.BLEEDING || check == BCMM_ClinicalCheck.INJURY)
  {
   int active;
   int dressed;
   int closed;
   for (int i = 0; i < patient.WoundCount(); i++)
   {
    BCMM_Wound wound = patient.WoundAt(i);
    if (!wound || wound.BodyZone != region) continue;
    if (wound.Sutured) closed++;
    else if (wound.Bandaged) dressed++;
    else if (wound.FlowMlPerSecond() > 0) active++;
   }
   string result = string.Format("Bleeding points: %1\n%2 dressed / %3 closed", active, dressed, closed);
   if (region == BCMM_BodyRegion.LEFT_LEG && patient.LeftLeg.Broken)
   {
    if (patient.LeftLeg.Splinted) result += "\nFracture supported by splint";
    else result += "\nSigns of unsupported fracture";
   }
   if (region == BCMM_BodyRegion.RIGHT_LEG && patient.RightLeg.Broken)
   {
    if (patient.RightLeg.Splinted) result += "\nFracture supported by splint";
    else result += "\nSigns of unsupported fracture";
   }
   return result;
  }
  if (check == BCMM_ClinicalCheck.AIRWAY)
  {
   if (patient.Airway == BCMM_AirwayCondition.PATENT) return "Airway appears clear";
   return "Signs of airway obstruction";
  }
  if (check == BCMM_ClinicalCheck.BREATHING)
  {
   string breathing = "Respiratory rate: " + Math.Round(patient.RespiratoryRate).ToString() + " /min";
   if (!patient.HasSpontaneousCirculation || patient.Airway != BCMM_AirwayCondition.PATENT || patient.RespiratoryRate < 4) return breathing + "\nBreathing absent or ineffective";
   if (patient.LeftChest.Tension || patient.RightChest.Tension || patient.LeftChest.CollapseFraction > 0.25 || patient.RightChest.CollapseFraction > 0.25) return breathing + "\nBreathing impaired; chest injury suspected";
   if (patient.Medication.BreathingScale < 0.8) return breathing + "\nBreathing slowed; reassess medication and support ventilation";
   return breathing + "\nSpontaneous breathing present";
  }
  if (check == BCMM_ClinicalCheck.PULSE)
  {
   int pulse;
   if (patient.HasSpontaneousCirculation) pulse = Math.Round(patient.HeartRate);
   string circulation = "Pulse: " + pulse.ToString() + " bpm";
   if (!patient.HasSpontaneousCirculation) return circulation + "\nPulse absent";
   if (patient.PerfusionFraction < 0.5) return circulation + "\nPulse weak";
   if (patient.HeartRate > 110) return circulation + "\nPulse rapid";
   return circulation + "\nPulse present";
  }
  if (check == BCMM_ClinicalCheck.BLOOD_LOSS)
  {
   string estimate=string.Format("Game reserve estimate: %1 percent\nOxygen-carrying reserve: %2 percent baseline",Math.Round(patient.Fluids.LastVolume*100),Math.Round(patient.Fluids.RedCellReserve*100));
   BCMM_Settings burdenSettings=new BCMM_Settings();SCR_BaseGameMode burdenMode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(burdenMode){burdenMode.BCMM_CopySettings(burdenSettings);}
   if(patient.BCMM_FluidBurdenSeverity(burdenSettings)>0){estimate+="\nElevated redistributed-fluid game burden; reassess breathing and additional water loading.";}
   if(patient.OxygenDeliveryFraction<BCMM_TreatmentAccess.Setting("recoveryOxygenDeliveryFraction")) { return estimate+"\nPoor tissue oxygen delivery; normal SpO2 does not rule out blood loss."; }
   if (patient.PerfusionFraction < 0.5 || patient.SystolicPressure<90) return estimate+"\nSigns of poor perfusion / blood loss";
   return estimate+"\nNo marked perfusion deficit detected by this check";
  }
  return "";
 }
 static void Supplies(IEntity actor, out int bandages, out int splints, out int surgery, out int sutures)
 {
  bandages = 0; splints = 0; surgery = 0; sutures = 0;
  if (!actor) return;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
   if (consumable && SCR_ConsumableBandage.Cast(consumable.GetConsumableEffect())) bandages++;
   if (item.FindComponent(BCMM_SplintItemComponent)) splints++;
   if (item.FindComponent(BCMM_SurgeryKitComponent)) surgery++;
   if (item.FindComponent(BCMM_SutureKitComponent)) sutures++;
  }
 }
}
class BCMM_ClinicalCheckSession
{
 protected static ref array<ref BCMM_ClinicalCheckSession> s_Active = {};
 static void Reset()
 {
  foreach (BCMM_ClinicalCheckSession session : s_Active) session.Player = null;
  s_Active.Clear();
 }
 SCR_PlayerController Player;
 IEntity Actor;
 IEntity Patient;
 int Region;
 int Check;
 int View;
 float Deadline;
 static void Begin(SCR_PlayerController player, IEntity target, int region, int check, int view)
 {
  if (!Replication.IsServer() || !player || !BCMM_ClinicalChecks.Supports(region, check) || !BCMM_TreatmentAccess.CanExamine(player.GetControlledEntity(), target)) return;
  BCMM_ClinicalCheckSession session = new BCMM_ClinicalCheckSession();
  session.Player = player;
  session.Actor = player.GetControlledEntity();
  session.Patient = target;
  session.Region = region;
  session.Check = check;
  session.View = view;
  if (check == BCMM_ClinicalCheck.SUPPLIES) { session.Reply(true, ""); return; }
  foreach (BCMM_ClinicalCheckSession pending : s_Active) if (pending.Player == player) { session.Reply(false, "Finish the current check first."); return; }
  float duration = BCMM_MedicRole.Duration(session.Actor, BCMM_TreatmentAccess.Setting("examinationDurationSeconds"));
  session.Deadline = GetGame().GetWorld().GetWorldTime() * 0.001 + duration;
  player.BCMM_SendCheckProgress(BCMM_BodyBagComponent.Id(target), view, region, check, duration);
  s_Active.Insert(session);
  GetGame().GetCallqueue().CallLater(session.Update, 100);
 }
 protected void Update()
 {
  if (!Player || Player.GetControlledEntity() != Actor || !BCMM_TreatmentAccess.CanExamine(Actor, Patient))
  {
   Reply(false, "Check interrupted: casualty unavailable.");
   s_Active.RemoveItem(this);
   return;
  }
  if (GetGame().GetWorld().GetWorldTime() * 0.001 < Deadline) { GetGame().GetCallqueue().CallLater(Update, 100); return; }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if (medical)
  {
   string finding = BCMM_ClinicalChecks.Report(medical.BCMM_GetPatient(), Region, Check);
   if(Check==BCMM_ClinicalCheck.BREATHING) { finding+=BCMM_OxygenSession.Status(Patient); }
   if (Check == BCMM_ClinicalCheck.PAIN && ChimeraCharacter.Cast(Patient).GetCharacterController().IsUnconscious()) finding = "Patient unresponsive; verbal pain assessment unavailable";
   Reply(true, finding);
  }
  s_Active.RemoveItem(this);
 }
 protected void Reply(bool completed, string result)
 {
  if (!Player) return;
  int bandages, splints, surgery, sutures;
  BCMM_ClinicalChecks.Supplies(Actor, bandages, splints, surgery, sutures);
  array<int> counts = {bandages, splints, surgery, sutures};
  BCMM_MedicationTreatment.AppendSupplies(Actor, counts);
  BCMM_EquipmentTreatment.AppendSupplies(Actor, counts);
  if (completed && (Check == BCMM_ClinicalCheck.BLEEDING || Check == BCMM_ClinicalCheck.INJURY)) counts.Insert(BCMM_ClinicalTreatmentFlags.ForPatient(Actor, Patient, Region));
  if (completed && Check != BCMM_ClinicalCheck.SUPPLIES) Player.BCMM_RememberCheck(Actor, Patient, Region, Check, result);
  Player.BCMM_SendClinicalCheck(BCMM_BodyBagComponent.Id(Patient), View, Region, Check, completed, result, counts);
 }
}
// Per-controller knowledge, bound to both exact characters and the patient
// model for this life. Revalidation reveals no new unexamined findings.
class BCMM_ClinicalKnowledge
{
 IEntity Actor;
 IEntity Patient;
 ref BCMM_Patient Life;
 int Region;
 int Check;
 string Signature;
 string Finding;
 bool Matches(IEntity actor, IEntity patient, BCMM_Patient life)
 {
  return Actor == actor && Patient == patient && Life == life && life && !life.Terminal && Signature == State(life, Region, Check);
 }
 static string State(BCMM_Patient life, int region, int check)
 {
  string signature = BCMM_ClinicalChecks.Report(life, region, check);
  if (check == BCMM_ClinicalCheck.AIRWAY) signature += ":" + life.Airway.ToString() + ":" + life.Equipment.AirwayAdjunct.ToString();
  if (check == BCMM_ClinicalCheck.BREATHING) signature += ":" + life.Equipment.OxygenActive.ToString();
  if (check == BCMM_ClinicalCheck.BLEEDING || check == BCMM_ClinicalCheck.INJURY)
  {
   for (int i = 0; i < life.WoundCount(); i++)
   {
    BCMM_Wound wound = life.WoundAt(i);
    if (wound && wound.BodyZone == region)
     signature += string.Format(":%1/%2/%3/%4/%5", wound.Id, wound.RetainedProjectile, wound.Bandaged, wound.Sutured, wound.Reopened);
   }
   for (int n = 0; n < life.InternalBleedCount(); n++)
   {
    BCMM_InternalBleeding bleed = life.InternalBleedAt(n);
    if (bleed && bleed.Region == region) signature += string.Format(":internal/%1/%2/%3", bleed.Id, bleed.Compartment, bleed.Repaired);
   }
  }
  return signature;
 }
}
modded class SCR_PlayerController
{
 protected ref array<ref BCMM_ClinicalKnowledge> m_BCMMKnowledge = {};
 void BCMM_RememberCheck(IEntity actor, IEntity patient, int region, int check, string finding)
 {
  if (!Replication.IsServer() || !actor || !patient) return;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed()) return;
  for (int i = m_BCMMKnowledge.Count() - 1; i >= 0; i--)
  {
   BCMM_ClinicalKnowledge old = m_BCMMKnowledge[i];
   if (!old.Actor || !old.Patient || old.Actor != actor || (old.Patient == patient && old.Region == region && old.Check == check)) m_BCMMKnowledge.Remove(i);
  }
  // Bound memory even on a long session with many different casualties.
  if (m_BCMMKnowledge.Count() >= 512) m_BCMMKnowledge.Remove(0);
  BCMM_ClinicalKnowledge known = new BCMM_ClinicalKnowledge();
  known.Actor = actor; known.Patient = patient; known.Life = medical.BCMM_GetPatient();
  known.Region = region; known.Check = check; known.Finding = finding;
  known.Signature = BCMM_ClinicalKnowledge.State(known.Life, region, check);
  m_BCMMKnowledge.Insert(known);
 }
 void BCMM_RequestKnownChecks(IEntity patient, int view)
 {
  if (GetGame().GetPlayerController() == this && patient) Rpc(BCMM_KnownChecksRequest, BCMM_BodyBagComponent.Id(patient), view);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_KnownChecksRequest(RplId target, int view)
 {
  IEntity actor = GetControlledEntity();
  IEntity patient = BCMM_BodyBagComponent.Resolve(target);
  if (!BCMM_TreatmentAccess.CanExamine(actor, patient)) return;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed()) return;
  for (int i = m_BCMMKnowledge.Count() - 1; i >= 0; i--)
  {
   BCMM_ClinicalKnowledge known = m_BCMMKnowledge[i];
   if (!known.Actor || !known.Patient || known.Actor != actor) { m_BCMMKnowledge.Remove(i); continue; }
   if (known.Patient != patient) continue;
   if (!known.Matches(actor, patient, medical.BCMM_GetPatient()))
   { m_BCMMKnowledge.Remove(i); continue; }
   int bandages, splints, surgery, sutures;
   BCMM_ClinicalChecks.Supplies(actor, bandages, splints, surgery, sutures);
   array<int> counts = {bandages, splints, surgery, sutures};
   BCMM_MedicationTreatment.AppendSupplies(actor, counts);
   BCMM_EquipmentTreatment.AppendSupplies(actor, counts);
   if (known.Check == BCMM_ClinicalCheck.BLEEDING || known.Check == BCMM_ClinicalCheck.INJURY)
   {
    counts.Insert(BCMM_ClinicalTreatmentFlags.ForPatient(actor, patient, known.Region));
   }
   BCMM_SendClinicalCheck(target, view, known.Region, known.Check, true, known.Finding, counts);
  }
 }
 void BCMM_SendCheckProgress(RplId patient, int view, int region, int check, float seconds)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveCheckProgress(patient, view, region, check, seconds);
  else Rpc(BCMM_ReceiveCheckProgress, patient, view, region, check, seconds);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveCheckProgress(RplId patient, int view, int region, int check, float seconds)
 {
  BCMM_MedicalMenu.BeginCheckProgress(patient, view, region, check, seconds);
 }
 void BCMM_RequestClinicalCheck(IEntity target, int region, int check, int view)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_ClinicalCheckRequest, BCMM_BodyBagComponent.Id(target), region, check, view);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_ClinicalCheckRequest(RplId target, int region, int check, int view)
 {
  BCMM_ClinicalCheckSession.Begin(this, BCMM_BodyBagComponent.Resolve(target), region, check, view);
 }
 void BCMM_SendClinicalCheck(RplId target, int view, int region, int check, bool completed, string result, array<int> counts)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveClinicalCheck(target, view, region, check, completed, result, counts);
  else Rpc(BCMM_ReceiveClinicalCheck, target, view, region, check, completed, result, counts);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveClinicalCheck(RplId target, int view, int region, int check, bool completed, string result, array<int> counts)
 {
  BCMM_MedicalMenu.ReceiveClinicalCheck(target, view, region, check, completed, result, counts);
 }
}
