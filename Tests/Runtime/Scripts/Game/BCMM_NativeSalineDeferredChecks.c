// Native persistence callback boundaries; no cloned sources or simulated item animations.
class BCMM_NativeSalineDeferredChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_SavedNativeSaline m_Missing = new BCMM_SavedNativeSaline();
 protected PersistenceSystem m_Persistence;
 protected float m_HeldVolume;
 override protected void Prepare()
 {
  m_Persistence = PersistenceSystem.GetInstance();
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Missing.SourceId = "01234567-89ab-cdef-0123-456789abcdef";
  m_Missing.LifeSeconds = 30;
  m_Missing.EffectSeconds = 20;
  m_Missing.DPS = -1;
  m_Suite.Record(m_Persistence && m_Persistence.IsTracked(m_Target) && !m_Persistence.FindById(m_Missing.SourceId), "Deferred stock fixture owns a real tracked patient and a genuinely missing canonical source");
  if (!m_Persistence)
  {
   Finish();
   return;
  }
  BCMM_NativeSalineBinding.Await(m_Target, m_Medical.BCMM_GetPatient(), m_Missing, m_Persistence);
  BCMM_SavedNativeSaline pending = BCMM_NativeSalineBinding.Capture(m_Target, m_Persistence);
  m_Suite.Record(pending && pending.SourceId == m_Missing.SourceId && pending.EffectSeconds == 20 && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), "Unresolved original stock reference survives a capture without creating delivery or a replacement source");
  SCR_CharacterDamageManagerComponent actorMedical = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_NativeSalineBinding.Await(m_Actor, actorMedical.BCMM_GetPatient(), m_Missing, m_Persistence);
  actorMedical.FullHeal();
  m_Suite.Record(!BCMM_NativeSalineBinding.Capture(m_Actor, m_Persistence), "FullHeal cannot serialize a pending stock reference from the previous clinical model");
  m_HeldVolume = m_Medical.GetBloodHitZone().GetHealthScaled();
  GetGame().GetCallqueue().CallLater(CheckTimeout, 31500);
 }
 protected void CheckTimeout()
 {
  m_Suite.Record(!BCMM_NativeSalineBinding.Capture(m_Target, m_Persistence) && !m_Persistence.FindById(m_Missing.SourceId) && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && m_Medical.GetBloodHitZone().GetHealthScaled() <= m_HeldVolume + 0.0001, "Actual deferred stock lookup times out without source cloning or fluid delivery");
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_Kit = Spawn("{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Rejected old-model callback uses a real tracked original stock source");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth() * 0.6);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess = true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion = BCMM_BodyRegion.RIGHT_ARM;
  BCMM_NativeSalineBinding.ObserveOriginal(m_Target, m_Kit, 30);
  m_Suite.Record(!BCMM_NativeSalineBinding.Capture(m_Target, m_Persistence), "A failed unattached native application cannot create a saved stock binding");
  BCMM_SavedNativeSaline known = new BCMM_SavedNativeSaline();
  known.SourceId = m_Persistence.GetId(m_Kit);
  known.LifeSeconds = 30;
  known.EffectSeconds = 20;
  known.DPS = -1;
  m_Suite.Record(m_Persistence.IsTracked(m_Kit) && known.Valid(), "Old-model callback names a real native registered source UUID and a complete valid stock state");
  BCMM_Patient oldModel = m_Medical.BCMM_GetPatient();
  m_Medical.FullHeal();
  BCMM_NativeSalineBinding.Await(m_Target, oldModel, known, m_Persistence);
  GetGame().GetCallqueue().CallLater(CheckStaleCallback, 700);
 }
 protected void CheckStaleCallback()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !BCMM_NativeSalineBinding.Capture(m_Target, m_Persistence) && !m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect), "Actual WhenAvailable callback for the original source cannot restore an old model or seize an unattached bag");
  Finish();
 }
}

