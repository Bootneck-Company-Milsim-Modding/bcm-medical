class BCMM_MedicationRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected bool m_CancelledOnce;
 protected int m_RestartWait;
 override protected void Prepare()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Medication fixture equips native backpack");
  m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_LEG, 1);
  m_Medical.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Kit = Spawn("{6AB4B3F3FD216012}Prefabs/Items/BCMM_Morphine.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Original placeholder morphine enters inventory");
  GetGame().GetCallqueue().CallLater(BeginMedication, 1200);
 }
 protected void BeginMedication()
 {
  m_Suite.Record(!BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, 1), "Medication rejects absent naloxone supply");
  m_Suite.Record(m_Medical.GetAimingDamage() > 0.2, "Untreated pain affects actual native aiming damage");
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, 0), "Authority reserves actual morphine and begins timed animation");
  m_Suite.Record(!BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, 0), "Repeated request cannot start a duplicate procedure");
  GetGame().GetCallqueue().CallLater(CheckAnimation, 2500);
  if (m_CancelledOnce) GetGame().GetCallqueue().CallLater(CheckDelivered, 18000);
 }
 protected void CheckAnimation()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), "Medication actually plays native item-use animation");
  m_Suite.Record(m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "Animation has no premature dose or kit consumption");
  if (!m_CancelledOnce)
  {
   m_CancelledOnce = true;
   BCMM_SplintTreatment.CancelActor(m_Actor);
   GetGame().GetCallqueue().CallLater(CheckCancelledMedication, 500);
  }
 }
 protected void CheckCancelledMedication()
 {
  if (ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && m_RestartWait++ < 30)
  { GetGame().GetCallqueue().CallLater(CheckCancelledMedication, 500); return; }
  m_Suite.Record(m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "Mid-animation cancellation retains original medication without administering");
  m_Suite.Record(!ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), "Cancelling procedure releases native item-use animation");
  BeginMedication();
 }
 protected void CheckDelivered()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 1, "Successful animation consumes exact medication and administers one dose");
  m_Suite.Record(m_Medical.BCMM_GetPatient().Medication.Pain < 63 && m_Medical.BCMM_GetPatient().LeftLeg.Broken, "Native patient receives analgesia while fracture remains");
  m_Suite.Record(!BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Target, 0), "Completed dose cannot be repeated without supply or interval");
  m_Suite.Record(m_Medical.GetAimingDamage() < 0.252, "Medication reduces native pain handling penalty over its onset");
  m_Medical.FullHeal();
  m_Suite.Record(m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0 && m_Medical.BCMM_GetPatient().Medication.BreathingScale == 1, "Native full heal resets medication and respiratory modifiers");
  GetGame().GetCallqueue().CallLater(BeginSelfMedication, 1200);
 }
 protected void BeginSelfMedication()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Kit = Spawn("{6AB4B3F3FD216014}Prefabs/Items/BCMM_Epinephrine.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Self-medication receives separate finite epinephrine");
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor, m_Actor, 2), "Native self-injection starts through the same authority procedure");
  GetGame().GetCallqueue().CallLater(CheckSelfAnimation, 2500);
  GetGame().GetCallqueue().CallLater(CheckSelfDelivered, 18000);
 }
 protected void CheckSelfAnimation()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 0, "Self-injection occupies the actor without an instant dose");
 }
 protected void CheckSelfDelivered()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.ActiveDoses() == 1 && m_Medical.BCMM_Pulse() > 75, "Native self-injection consumes exact item and changes the correct patient's pulse");
  Finish();
 }
}