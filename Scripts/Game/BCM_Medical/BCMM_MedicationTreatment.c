// Original finite-supply medication administration using the shared authority timer.
[ComponentEditorProps(category: "GameScripted/BCM Medical")]
class BCMM_MedicationItemComponentClass : ScriptComponentClass {}
class BCMM_MedicationItemComponent : ScriptComponent
{
 [Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(BCMM_MedicationKind))] int Kind;
}
class BCMM_MedicationTreatment : BCMM_SplintTreatment
{
 protected int m_Kind;
 static IEntity FindMedication(IEntity actor, int kind)
 {
  if (!actor || kind < 0 || kind > BCMM_MedicationKind.FENTANYL) return null;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return null;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   BCMM_MedicationItemComponent marker = BCMM_MedicationItemComponent.Cast(item.FindComponent(BCMM_MedicationItemComponent));
   if (marker && marker.Kind == kind) return item;
  }
  return null;
 }
 static bool MedicationEligible(IEntity target, int kind)
 {
  if (!target || kind < 0 || kind > BCMM_MedicationKind.FENTANYL) return false;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical || medical.IsDestroyed() || medical.BCMM_GetPatient().Terminal) return false;
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(settings);
  BCMM_Patient patient = medical.BCMM_GetPatient();
  if(kind==BCMM_MedicationKind.PARACETAMOL && ChimeraCharacter.Cast(target).GetCharacterController().IsUnconscious()) { return false; }
  return BCMM_ClinicalSafety.MedicationDenial(patient,kind,settings).IsEmpty();
 }
 static bool BeginMedication(IEntity actor, IEntity target, int kind, SCR_PlayerController player = null)
 {
  if (!Replication.IsServer() || (player && player.GetControlledEntity() != actor)) return false;
  if (!BCMM_TreatmentAccess.CanReach(actor, target) || !MedicationEligible(target, kind)) return false;
  if (!BCMM_MedicRole.Allows(actor, "medicOnlyMedication")) return false;
  if (kind == BCMM_MedicationKind.TXA && !BCMM_MedicRole.Allows(actor, "txaMedicOnly")) return false;
  if(kind==BCMM_MedicationKind.FENTANYL && !BCMM_MedicRole.Allows(actor,"fentanylMedicOnly")) { return false; }
  IEntity kit = FindMedication(actor, kind);
  if (!kit) return false;
  BCMM_MedicationTreatment operation = new BCMM_MedicationTreatment();
  operation.m_Kind = kind;
  string duration = "medicationApplicationSeconds";
  if (kind == BCMM_MedicationKind.TXA) duration = "txaApplicationSeconds";
  if(kind==BCMM_MedicationKind.PARACETAMOL || kind==BCMM_MedicationKind.FENTANYL) { duration=BCMM_Pharmacology.Prefix(kind)+"ApplicationSeconds"; }
  return StartOperation(operation, actor, target, kit, BCMM_TreatmentAccess.Setting(duration), player);
 }
 protected override int ProcedureRegion() { return -1; }
 protected override string ProcedureName() { return BCMM_Pharmacology.Name(m_Kind) + " administration"; }
 protected override bool StillEligible() { return (m_Kind!=BCMM_MedicationKind.FENTANYL || BCMM_MedicRole.Allows(m_Actor,"fentanylMedicOnly")) && BCMM_MedicRole.Allows(m_Actor, "medicOnlyMedication") && (m_Kind != BCMM_MedicationKind.TXA || BCMM_MedicRole.Allows(m_Actor, "txaMedicOnly")) && MedicationEligible(m_Target, m_Kind); }
 protected override void ApplyProcedure()
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_Settings settings = new BCMM_Settings();
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(settings);
  medical.BCMM_GetPatient().Medication.Administer(m_Kind, settings);
  if (m_Player) m_Player.BCMM_TreatmentFeedback(BCMM_Pharmacology.Name(m_Kind) + " administered. Monitor breathing and circulation.");
 }
 static void AppendSupplies(IEntity actor, array<int> counts)
 {
  for (int kind = 0; kind < BCMM_SupplyCatalog.MEDICATION_COUNT; kind++) counts.Insert(0);
  if (!actor) return;
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  if (!inventory) return;
  array<IEntity> items = {};
  inventory.GetItems(items, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : items)
  {
   if (!item) continue;
   BCMM_MedicationItemComponent marker = BCMM_MedicationItemComponent.Cast(item.FindComponent(BCMM_MedicationItemComponent));
   if (marker && marker.Kind >= 0 && marker.Kind <= BCMM_MedicationKind.FENTANYL) counts[4 + marker.Kind] = counts[4 + marker.Kind] + 1;
  }
 }
}
modded class SCR_PlayerController
{
 void BCMM_RequestMedication(IEntity target, int kind)
 {
  if (GetGame().GetPlayerController() == this && target) Rpc(BCMM_MedicationRequest, BCMM_BodyBagComponent.Id(target), kind);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Server)]
 protected void BCMM_MedicationRequest(RplId target, int kind)
 {
  IEntity patientEntity=BCMM_BodyBagComponent.Resolve(target);
  if (!BCMM_MedicationTreatment.BeginMedication(GetControlledEntity(),patientEntity,kind,this))
  {
   string reason="Medication unavailable: check supplies, range, role and free hands.";
   SCR_CharacterDamageManagerComponent medical;
   if(patientEntity) { medical=SCR_CharacterDamageManagerComponent.Cast(patientEntity.FindComponent(SCR_CharacterDamageManagerComponent)); }
   if(medical && BCMM_TreatmentAccess.CanExamine(GetControlledEntity(),patientEntity))
   {
    BCMM_Settings settings=new BCMM_Settings();SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());if(mode) { mode.BCMM_CopySettings(settings); }
    string denied=BCMM_ClinicalSafety.MedicationDenial(medical.BCMM_GetPatient(),kind,settings);if(!denied.IsEmpty()) { reason=denied; }
   }
   BCMM_BandageFeedback.Send(this,reason);
  }
 }
}
