class BCMM_BandageRuntimeChecks : BCMM_SplintRuntimeChecks
{
 override protected void Prepare()
 {
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack = Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et", m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack, EStoragePurpose.PURPOSE_ANY), "Bandaging actor equips native backpack");
  m_Kit = Spawn("{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Real native field dressing enters inventory");
  m_Actor.SetAngles("0 90 0");
  Bleed(m_Medical, ECharacterHitZoneGroup.LEFTLEG);
  Bleed(m_Medical, ECharacterHitZoneGroup.RIGHTLEG);
  GetGame().GetCallqueue().CallLater(BeginBandage, 1200);
 }
 protected void Bleed(SCR_CharacterDamageManagerComponent medical, ECharacterHitZoneGroup group)
 {
  array<HitZone> zones = {};
  medical.GetHitZonesOfGroupFromOwner(group, zones);
  if (zones.IsEmpty()) { m_Suite.Record(false, "Native bandaging limb hit zone exists"); return; }
  SCR_CharacterHitZone zone = SCR_CharacterHitZone.Cast(zones[0]);
  zone.SetHealth(zone.GetMaxHealth() * 0.5);
  medical.AddBleedingEffectOnHitZone(zone);
 }
 protected void BeginBandage()
 {
  m_Suite.Record(BCMM_Bandaging.FindBandage(m_Actor) == m_Kit, "Bandage selection finds original native supply");
  m_Suite.Record(BCMM_Bandaging.Animate(m_Actor, m_Target, m_Kit, BCMM_BodyRegion.LEFT_LEG), "Selected leg bandaging starts native item animation");
  GetGame().GetCallqueue().CallLater(CheckAnimating, 2500);
  GetGame().GetCallqueue().CallLater(CheckOtherTreated, BCMM_TreatmentAccess.Setting("bandageApplicationSeconds") * 1000 + 7000);
 }
 protected void CheckAnimating()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(), "Native bandaging actually occupies actor with item-use animation");
  m_Suite.Record(m_Kit && m_Medical.GetGroupDamageOverTime(ECharacterHitZoneGroup.LEFTLEG, EDamageType.BLEEDING) > 0, "Bandaging does not instantly consume dressing or stop bleeding");
 }
 protected void CheckOtherTreated()
 {
  m_Suite.Record(!m_Kit && m_Medical.GetGroupDamageOverTime(ECharacterHitZoneGroup.LEFTLEG, EDamageType.BLEEDING) == 0, "Native completion consumes original field dressing and treats selected leg");
  m_Suite.Record(m_Medical.GetGroupDamageOverTime(ECharacterHitZoneGroup.RIGHTLEG, EDamageType.BLEEDING) > 0, "Selected-leg bandage does not silently treat opposite leg");
  m_Medical.FullHeal();
  SCR_CharacterDamageManagerComponent actorMedical = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  Bleed(actorMedical, ECharacterHitZoneGroup.LEFTLEG);
  m_Kit = Spawn("{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et", m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Self-treatment receives separate native dressing");
  GetGame().GetCallqueue().CallLater(BeginSelf, 1000);
 }
 protected void BeginSelf()
 {
  m_Suite.Record(BCMM_Bandaging.Animate(m_Actor, m_Actor, m_Kit, BCMM_BodyRegion.LEFT_LEG), "Selected-region self-bandage starts native self animation");
  GetGame().GetCallqueue().CallLater(CheckSelf, BCMM_TreatmentAccess.Setting("bandageApplicationSeconds") * 1000 + 7000);
 }
 protected void CheckSelf()
 {
  SCR_CharacterDamageManagerComponent actorMedical = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(!m_Kit && actorMedical.GetGroupDamageOverTime(ECharacterHitZoneGroup.LEFTLEG, EDamageType.BLEEDING) == 0, "Native self-bandage consumes supply and treats selected limb");
  Finish();
 }
}