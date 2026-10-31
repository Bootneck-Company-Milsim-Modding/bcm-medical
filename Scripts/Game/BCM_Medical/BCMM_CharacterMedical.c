// Original BCM character integration. Uses native hit zones and consciousness;
// no ACE classes, damage effects or prefab replacements are required.
modded class SCR_CharacterDamageManagerComponent
{
 protected ref array<ref BCMM_NativeWound> m_BCMMNativeWounds = {};
 bool BCMM_MapRegion(ECharacterHitZoneGroup group, out BCMM_BodyRegion region)
 {
  switch (group)
  {
   case ECharacterHitZoneGroup.HEAD: region = BCMM_BodyRegion.HEAD; return true;
   case ECharacterHitZoneGroup.UPPERTORSO: region = BCMM_BodyRegion.CHEST; return true;
   case ECharacterHitZoneGroup.LOWERTORSO: region = BCMM_BodyRegion.ABDOMEN; return true;
   case ECharacterHitZoneGroup.LEFTARM: region = BCMM_BodyRegion.LEFT_ARM; return true;
   case ECharacterHitZoneGroup.RIGHTARM: region = BCMM_BodyRegion.RIGHT_ARM; return true;
   case ECharacterHitZoneGroup.LEFTLEG: region = BCMM_BodyRegion.LEFT_LEG; return true;
   case ECharacterHitZoneGroup.RIGHTLEG: region = BCMM_BodyRegion.RIGHT_LEG; return true;
  }
  return false;
 }
 protected float BCMM_TraumaSample() { return Math.RandomFloat01(); }
 void BCMM_RecordImpact(BaseDamageContext context)
 {
  if (!BCMM_IsAuthority() || DotDamageEffect.Cast(context.damageEffect) || context.damageValue <= 0 || IsDestroyed()) return;
  if (context.damageType != EDamageType.KINETIC && context.damageType != EDamageType.COLLISION) return;
  SCR_CharacterHitZone zone = SCR_CharacterHitZone.Cast(context.struckHitZone);
  BCMM_BodyRegion region;
  if (!zone || zone.GetMaxHealth() <= 0 || !BCMM_MapRegion(zone.GetHitZoneGroup(), region) || !m_pBloodHitZone) return;
  BCMM_InjuryCause cause = BCMM_InjuryCause.LACERATION;
  if (context.damageType == EDamageType.KINETIC) cause = BCMM_InjuryCause.GUNSHOT;
  float severity = Math.Clamp(context.damageValue / zone.GetMaxHealth(), 0, 1);
  float nativeRate = zone.GetMaxBleedingRate() * severity * GetBleedingScale();
  BCMM_AddNativeWound(zone, region, cause, nativeRate);
  if (cause == BCMM_InjuryCause.GUNSHOT)
  {
   vector worldTransform[4];
   GetOwner().GetWorldTransform(worldTransform);
   float sideOffset = vector.Dot(context.hitPosition - worldTransform[3], worldTransform[0]);
   BCMM_TraumaGeneration.Apply(m_BCMMPatient, region, severity, sideOffset < 0, BCMM_TraumaSample(), BCMM_TraumaSample(), BCMM_TraumaSample(), BCMM_TraumaSample(), m_BCMMClinicalSettings);
  }
 }
 protected void BCMM_AddNativeWound(SCR_CharacterHitZone zone, BCMM_BodyRegion region, BCMM_InjuryCause cause, float nativeDPS)
 {
  if (!m_pBloodHitZone || m_pBloodHitZone.GetMaxHealth() <= 0) return;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(m_BCMMClinicalSettings);
  float flow = nativeDPS * m_BCMMClinicalSettings.Get("normalBloodVolumeMl") / m_pBloodHitZone.GetMaxHealth();
  int id = m_BCMMPatient.RegisterInjury(region, cause, flow, Math.RandomFloat01(), m_BCMMClinicalSettings);
  if (!id) return;
  BCMM_NativeWound link = new BCMM_NativeWound();
  link.Zone = zone;
  link.WoundId = id;
  m_BCMMNativeWounds.Insert(link);
 }
 float BCMM_NativeBleedingDPS(SCR_CharacterHitZone zone, float fallbackDPS)
 {
  if (!BCMM_IsAuthority() || !zone || !m_pBloodHitZone) return fallbackDPS;
  bool found;
  float flow;
  foreach (BCMM_NativeWound link : m_BCMMNativeWounds)
  {
   if (link.Zone != zone) continue;
   BCMM_Wound wound = m_BCMMPatient.FindWound(link.WoundId);
   if (!wound) continue;
   found = true;
   flow += wound.FlowMlPerSecond();
  }
  if (!found)
  {
   BCMM_BodyRegion region;
   if (BCMM_MapRegion(zone.GetHitZoneGroup(), region)) BCMM_AddNativeWound(zone, region, BCMM_InjuryCause.LACERATION, fallbackDPS);
   return fallbackDPS;
  }
  return flow * m_pBloodHitZone.GetMaxHealth() / m_BCMMClinicalSettings.Get("normalBloodVolumeMl");
 }
 override void RemoveGroupBleeding(ECharacterHitZoneGroup charHZGroup)
 {
  if (BCMM_IsAuthority())
  {
   foreach (BCMM_NativeWound link : m_BCMMNativeWounds)
   {
    if (link.Zone && link.Zone.GetHitZoneGroup() == charHZGroup) m_BCMMPatient.DressWound(link.WoundId, Math.RandomFloat01(), m_BCMMClinicalSettings);
   }
  }
  super.RemoveGroupBleeding(charHZGroup);
 }
 protected void BCMM_AdvanceNativeTrauma(float seconds)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
  BCMM_Activity activity = BCMM_Activity.RESTING;
  if (character && character.GetCharacterController() && !character.GetParent() && !SCR_CompartmentAccessComponent.GetVehicleIn(character) && !character.GetCharacterController().IsFalling())
  {
   float speed = character.GetCharacterController().GetVelocity().Length();
   if (speed > 0.2) activity = BCMM_Activity.WALKING;
   if (speed > 3) activity = BCMM_Activity.RUNNING;
   if (speed > 0.2 && character.GetCharacterController().GetStance() == ECharacterStance.PRONE) activity = BCMM_Activity.CRAWLING;
  }
  float dressingScale = m_BCMMPatient.Medication.ClotWearScale(m_BCMMClinicalSettings)*m_BCMMPatient.Fluids.DressingWearMultiplier(m_BCMMClinicalSettings);
  if (BCMM_StretcherComponent.Stabilises(GetOwner()))
  { activity = BCMM_Activity.RESTING; dressingScale *= 1 - m_BCMMClinicalSettings.Get("stretcherDressingWearReduction"); }
  foreach (BCMM_NativeWound link : m_BCMMNativeWounds)
  {
   BCMM_Wound wound = m_BCMMPatient.FindWound(link.WoundId);
   if (!wound || !link.Zone) continue;
   bool dressed = wound.Bandaged;
   wound.Advance(seconds, activity, m_BCMMClinicalSettings, dressingScale);
   if (dressed && !wound.Bandaged && wound.FlowMlPerSecond() > 0) AddBleedingEffectOnHitZone(link.Zone);
  }
  float internalLoss = m_BCMMPatient.AdvanceInternalTrauma(seconds, activity, m_BCMMClinicalSettings);
  if (internalLoss > 0 && m_pBloodHitZone)
  {
   float damage = internalLoss * m_pBloodHitZone.GetMaxHealth() / m_BCMMClinicalSettings.Get("normalBloodVolumeMl");
   m_pBloodHitZone.HandleDamage(damage, EDamageType.BLEEDING, null);
  }
 }
 [RplProp(onRplName: "BCMM_ApplyMovement")] protected float m_BCMMMovementCap = 1;
 [RplProp()] protected bool m_BCMMFracturedLeg;
 bool BCMM_LegsAllowJump() { return !m_BCMMFracturedLeg; }
 float BCMM_MovementCap() { return BCMM_LivingTransport.LimitMovement(GetOwner(),m_BCMMMovementCap); }
 protected void BCMM_ApplyMovement()
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
  if (character && character.GetCharacterController()) character.GetCharacterController().OverrideMaxSpeed(BCMM_MovementCap());
 }
 void BCMM_PublishFractures()
 {
  if (!BCMM_IsAuthority()) return;
  BCMM_VisualStatusBridge.Publish(this);
  bool broken = !m_BCMMPatient.LegsAllowJump();
  float cap = m_BCMMPatient.LegMovementScale(m_BCMMClinicalSettings);
  if (broken != m_BCMMFracturedLeg || Math.AbsFloat(cap - m_BCMMMovementCap) > 0.001)
  {
   m_BCMMFracturedLeg = broken;
   m_BCMMMovementCap = cap;
   BCMM_ApplyMovement();
   Replication.BumpMe();
  }
  if (broken) UpdateCharacterGroupDamage(ECharacterHitZoneGroup.LEFTLEG);
 }
 void BCMM_RepairNativeLeg(bool left)
 {
  if (!BCMM_IsAuthority()) return;
  ECharacterHitZoneGroup group = ECharacterHitZoneGroup.RIGHTLEG;
  if (left) group = ECharacterHitZoneGroup.LEFTLEG;
  array<HitZone> zones = {};
  GetHitZonesOfGroupFromOwner(group, zones);
  foreach (HitZone zone : zones) if (zone) zone.SetHealth(zone.GetMaxHealth());
  SetMovementDamage(0);
  UpdateCharacterGroupDamage(ECharacterHitZoneGroup.LEFTLEG);
  UpdateCharacterGroupDamage(ECharacterHitZoneGroup.RIGHTLEG);
  BCMM_PublishFractures();
 }
 void BCMM_RecordFracture(SCR_CharacterHitZone zone)
 {
  if (!BCMM_IsAuthority()) return;
  if (zone.GetHitZoneGroup() == ECharacterHitZoneGroup.LEFTLEG) m_BCMMPatient.LeftLeg.Broken = true;
  if (zone.GetHitZoneGroup() == ECharacterHitZoneGroup.RIGHTLEG) m_BCMMPatient.RightLeg.Broken = true;
  BCMM_PublishFractures();
 }
 override void UpdateCharacterGroupDamage(ECharacterHitZoneGroup hitZoneGroup)
 {
  super.UpdateCharacterGroupDamage(hitZoneGroup);
  if (hitZoneGroup == ECharacterHitZoneGroup.LEFTARM || hitZoneGroup == ECharacterHitZoneGroup.RIGHTARM) BCMM_ApplyPainHandling();
  if (!BCMM_IsAuthority() || !m_BCMMPatient) return;
  if (hitZoneGroup != ECharacterHitZoneGroup.LEFTLEG && hitZoneGroup != ECharacterHitZoneGroup.RIGHTLEG) return;
  bool anyBroken = m_BCMMPatient.LeftLeg.Broken || m_BCMMPatient.RightLeg.Broken;
  bool allSupported = (!m_BCMMPatient.LeftLeg.Broken || m_BCMMPatient.LeftLeg.Splinted) && (!m_BCMMPatient.RightLeg.Broken || m_BCMMPatient.RightLeg.Splinted);
  if (anyBroken && allSupported) SetMovementDamage(0);
 }
 protected ref array<IEntity> m_BCMMRecordedEquipment = {};
 protected bool m_BCMMEquipmentFrozen;
 protected bool m_BCMMWasUnconscious;
 void BCMM_CopyDeathEquipment(notnull array<IEntity> output)
 {
  if (Replication.IsServer()) output.Copy(m_BCMMRecordedEquipment);
 }
 protected void BCMM_RememberEquipment(bool preserveUnconsciousDrops)
 {
  if (!BCMM_IsAuthority() || m_BCMMEquipmentFrozen) return;
  SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_InventoryStorageManagerComponent));
  if (!inventory) return;
  array<IEntity> current = {};
  array<IEntity> roots = {};
  inventory.GetItems(current, EStoragePurpose.PURPOSE_ANY);
  foreach (IEntity item : current)
  {
   if (!item || roots.Contains(item)) continue;
   InventoryItemComponent component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
   if (component && component.GetParentSlot() && component.GetParentSlot().GetStorage() && component.GetParentSlot().GetStorage().GetOwner() == GetOwner()) roots.Insert(item);
  }
  if (preserveUnconsciousDrops)
  {
   foreach (IEntity previous : m_BCMMRecordedEquipment)
   {
    if (!previous || roots.Contains(previous)) continue;
    InventoryItemComponent component = InventoryItemComponent.Cast(previous.FindComponent(InventoryItemComponent));
    if (component && !component.GetParentSlot() && vector.Distance(previous.GetOrigin(), GetOwner().GetOrigin()) <= BCMM_BaggingOperation.Setting("deathEquipmentRecoveryRangeMetres")) roots.Insert(previous);
   }
  }
  m_BCMMRecordedEquipment.Copy(roots);
 }
 protected void BCMM_FreezeEquipment()
 {
  if (!BCMM_IsAuthority() || m_BCMMEquipmentFrozen) return;
  ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
  bool unconscious = m_BCMMWasUnconscious || (character && character.GetCharacterController() && character.GetCharacterController().IsUnconscious());
  BCMM_RememberEquipment(unconscious);
  m_BCMMEquipmentFrozen = true;
 }
 protected ref BCMM_DepletionPolicy m_BCMMDepletion = new BCMM_DepletionPolicy();
 protected ref BCMM_Patient m_BCMMPatient = new BCMM_Patient();
 [RplProp(onRplName: "BCMM_RefreshConsciousness")] protected bool m_BCMMDepleted;
 [RplProp(onRplName: "BCMM_RefreshConsciousness")] protected bool m_BCMMCritical;
 [RplProp()] protected bool m_BCMMDeathPermitted;
 protected bool m_BCMMMonitorQueued;
 protected float m_BCMMLastPhysiologyTime;
 protected bool m_BCMMUpdating;
 protected bool m_BCMMFullHealing;
 protected ref BCMM_Settings m_BCMMClinicalSettings = new BCMM_Settings();
 [RplProp(onRplName: "BCMM_ApplyPainHandling")] protected float m_BCMMPainAim;
 [RplProp()] protected float m_BCMMOxygen = 0.98;
 [RplProp()] protected float m_BCMMRespiratoryRate=16;
 [RplProp()] protected float m_BCMMPulse = 75;
 [RplProp()] protected float m_BCMMPressure = 120;
 [RplProp(onRplName: "BCMM_RefreshConsciousness")] protected bool m_BCMMArrest;
 [RplProp(onRplName: "BCMM_RefreshConsciousness")] protected bool m_BCMMHypoxic;
 [RplProp()] protected string m_BCMMDeceasedName;
 static const float BCMM_HEALTH_FLOOR = 1;

 BCMM_Patient BCMM_GetPatient() { return m_BCMMPatient; }
 bool BCMM_IsAuthority()
 {
  HitZone health = GetDefaultHitZone();
  return health && !health.IsProxy();
 }
 bool BCMM_CanDie() { return m_BCMMDeathPermitted; }
 bool BCMM_IsDepleted() { return m_BCMMDepleted; }
 float BCMM_Now() { return GetGame().GetWorld().GetWorldTime() * 0.001; }
 override bool GetPermitUnconsciousness() { return true; }

 string BCMM_DeceasedName() { return m_BCMMDeceasedName; }
 void BCMM_CaptureDeceasedName(string displayName)
 {
  if (!BCMM_IsAuthority() || !m_BCMMDeceasedName.IsEmpty()) return;
  m_BCMMDeceasedName = displayName;
  Replication.BumpMe();
 }
 override void FullHeal(bool ignoreHealingDOT = true)
 {
  if (!BCMM_IsAuthority() || IsDestroyed()) return;
  BCMM_NativeSalineBinding.ResetPatient(GetOwner());
  m_BCMMFullHealing = true;
  m_BCMMLastPhysiologyTime = BCMM_Now();
  m_BCMMDepletion.ResetByFullHeal();
  m_BCMMDepleted = false;
  m_BCMMCritical = false;
  m_BCMMDeathPermitted = false;
  m_BCMMPatient = new BCMM_Patient();
  m_BCMMNativeWounds.Clear();
  m_BCMMRecordedEquipment.Clear();
  m_BCMMEquipmentFrozen = false;
  m_BCMMWasUnconscious = false;
  m_BCMMPainAim = 0;
  m_BCMMOxygen = 0.98;
  m_BCMMRespiratoryRate=m_BCMMClinicalSettings.Get("normalRespiratoryRate");
  m_BCMMPulse = m_BCMMClinicalSettings.Get("normalHeartRate");
  m_BCMMPressure = m_BCMMClinicalSettings.Get("normalSystolicPressure");
  m_BCMMArrest = false;
  m_BCMMHypoxic = false;
  BCMM_PublishFractures();
  super.FullHeal(ignoreHealingDOT);
  m_BCMMFullHealing = false;
  Replication.BumpMe();
  UpdateConsciousness();
 }
 protected void BCMM_RefreshConsciousness() { UpdateConsciousness(); }
 override void UpdateConsciousness()
 {
  if (m_BCMMUpdating || m_BCMMFullHealing) return;
  ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
  if (!character || IsDestroyed()) return;
  CharacterControllerComponent controller = character.GetCharacterController();
  if (!controller || controller.GetLifeState()==ECharacterLifeState.DEAD) return;
  m_BCMMUpdating = true;
  if (BCMM_IsAuthority())
  {
   bool critical = GetDefaultHitZone().GetHealth() <= BCMM_HEALTH_FLOOR;
   critical = critical || (m_pHeadHitZone && m_pHeadHitZone.GetDamageState() == EDamageState.DESTROYED);
   bool changed = critical != m_BCMMCritical;
   m_BCMMCritical = critical;
   if (m_pBloodHitZone)
   {
    SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
    float grace = 60;
    if (mode) grace = mode.BCMM_ReadSetting("bloodDepletionGraceSeconds");
    bool expired = m_BCMMDepletion.Observe(BCMM_Now(), Math.Max(0, m_pBloodHitZone.GetHealth()), grace);
    bool depleted = m_BCMMDepletion.IsActive();
    changed = changed || depleted != m_BCMMDepleted;
    m_BCMMDepleted = depleted;
    if (expired)
    {
     m_BCMMDeathPermitted = true;
     m_BCMMPatient.MarkTerminal();
     Replication.BumpMe();
     // Native Kill synchronously calls OnDamage/OnHealthSet before its dead state is final.
     // Keep the consciousness guard held until that callback chain has returned.
     BCMM_KillNative(GetInstigator());
     m_BCMMUpdating = false;
     return;
    }
   }
   if (changed) Replication.BumpMe();
   if (!m_BCMMMonitorQueued && (m_BCMMDepleted || (m_pBloodHitZone && m_pBloodHitZone.GetHealthScaled() <= m_pBloodHitZone.GetDamageStateThreshold(ECharacterBloodState.UNCONSCIOUS))))
   {
    m_BCMMMonitorQueued = true;
    GetGame().GetCallqueue().CallLater(BCMM_Monitor, 100);
   }
  }
  bool unconscious = m_BCMMDepleted || m_BCMMCritical || m_BCMMArrest || m_BCMMHypoxic || ShouldBeUnconscious();
  if (BCMM_IsAuthority() && unconscious && !m_BCMMWasUnconscious)
  {
   BCMM_RememberEquipment(false);
   BCMM_WeaponRetention.BeforeUnconscious(GetOwner());
  }
  m_BCMMWasUnconscious = unconscious;
  controller.SetUnconscious(unconscious);
  m_BCMMUpdating = false;
 }
 float BCMM_PainHandlingPenalty() { return m_BCMMPainAim; }
 protected void BCMM_ApplyPainHandling()
 {
  // Keep native arm damage as a floor: analgesia never repairs an injured arm.
  float left = 1 - GetGroupHealthScaled(ECharacterHitZoneGroup.LEFTARM);
  float right = 1 - GetGroupHealthScaled(ECharacterHitZoneGroup.RIGHTARM);
  if (GetGroupTourniquetted(ECharacterHitZoneGroup.LEFTARM)) left = 0.7;
  if (GetGroupTourniquetted(ECharacterHitZoneGroup.RIGHTARM)) right = 0.7;
  float structural = (left + right) * 0.5 * m_fAimingDamageMultiplier;
  SetAimingDamage(Math.Max(structural, m_BCMMPainAim));
 }

 float BCMM_OxygenSaturation() { return m_BCMMOxygen; }
 float BCMM_RespiratoryRate() { return m_BCMMRespiratoryRate; }
 float BCMM_Pulse() { return m_BCMMPulse; }
 float BCMM_SystolicPressure() { return m_BCMMPressure; }
 bool BCMM_InCardiacArrest() { return m_BCMMArrest; }
 protected void BCMM_UpdateAirwayFromConsciousness()
 {
  ChimeraCharacter body = ChimeraCharacter.Cast(GetOwner());
  if (!body) return;
  bool unconscious = body.GetCharacterController().IsUnconscious();
  if (!unconscious)
  {
   if (m_BCMMPatient.Airway == BCMM_AirwayCondition.SOFT_TISSUE_OBSTRUCTION) m_BCMMPatient.Airway = BCMM_AirwayCondition.PATENT;
   if (m_BCMMPatient.Equipment.AirwayAdjunct) m_BCMMPatient.Equipment.AirwayAdjunct = false;
   return;
  }
  if (m_BCMMClinicalSettings.Get("automaticUnconsciousAirwayObstruction") > 0.5 && !m_BCMMPatient.Equipment.AirwayAdjunct && m_BCMMPatient.Airway == BCMM_AirwayCondition.PATENT)
   m_BCMMPatient.Airway = BCMM_AirwayCondition.SOFT_TISSUE_OBSTRUCTION;
 }
 protected void BCMM_PhysiologyTick()
 {
  if (!GetOwner() || !BCMM_IsAuthority() || IsDestroyed() || m_BCMMDeathPermitted) return;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if (mode) mode.BCMM_CopySettings(m_BCMMClinicalSettings);
  float now = BCMM_Now();
  float elapsed = Math.Max(0, now - m_BCMMLastPhysiologyTime);
  m_BCMMLastPhysiologyTime = now;
  ChimeraCharacter equipmentOwner = ChimeraCharacter.Cast(GetOwner());
  if (equipmentOwner && !m_BCMMWasUnconscious && !equipmentOwner.GetCharacterController().IsUnconscious()) BCMM_RememberEquipment(false);
  BCMM_PublishFractures();
  BCMM_UpdateAirwayFromConsciousness();
  BCMM_AdvanceNativeTrauma(elapsed);
  if (m_pBloodHitZone)
  {
   float bloodFraction=Math.Clamp(m_pBloodHitZone.GetHealthScaled(),0,1);
   if(m_BCMMClinicalSettings.Get("componentRecoveryEnabled")>0.5)
   {
    bool facility=m_BCMMClinicalSettings.Get("componentRecoveryRequiresFacility")<0.5 || BCMM_MedicalFacilityComponent.Supports(GetOwner(),GetOwner());
    m_BCMMPatient.BCMM_AdvanceComponentRecovery(elapsed,bloodFraction,facility,m_BCMMClinicalSettings);
   }
   float redistributed=m_BCMMPatient.AdvancePhysiology(elapsed,bloodFraction,m_BCMMClinicalSettings);
   if(redistributed>0) { m_pBloodHitZone.SetHealth(Math.Max(0,m_pBloodHitZone.GetHealth()-redistributed*m_pBloodHitZone.GetMaxHealth())); }
  }
  if (m_BCMMPatient.Terminal)
  {
   m_BCMMDeathPermitted = true;
   Replication.BumpMe();
   BCMM_KillNative(GetInstigator());
   return;
  }
  BCMM_SetWaterLoadingWithheld(m_BCMMPatient.BCMM_WithholdAdditionalWater(BCMM_FluidKind.SALINE,m_BCMMClinicalSettings));
  bool arrest = !m_BCMMPatient.HasSpontaneousCirculation;
  bool hypoxic = m_BCMMPatient.OxygenSaturation < m_BCMMClinicalSettings.Get("unconsciousOxygenFraction") || m_BCMMPatient.OxygenDeliveryFraction<m_BCMMClinicalSettings.Get("recoveryOxygenDeliveryFraction");
  float painAim = m_BCMMPatient.Medication.Pain * 0.01 * m_BCMMClinicalSettings.Get("painAimingPenalty");
  bool changed = Math.AbsFloat(m_BCMMRespiratoryRate-m_BCMMPatient.RespiratoryRate)>0.1 || Math.AbsFloat(m_BCMMPainAim - painAim) > 0.001 || hypoxic != m_BCMMHypoxic || arrest != m_BCMMArrest || Math.AbsFloat(m_BCMMOxygen - m_BCMMPatient.OxygenSaturation) > 0.001 || Math.AbsFloat(m_BCMMPulse - m_BCMMPatient.HeartRate) > 0.1 || Math.AbsFloat(m_BCMMPressure - m_BCMMPatient.SystolicPressure) > 0.1;
  m_BCMMPainAim = painAim;
  BCMM_ApplyPainHandling();
  m_BCMMRespiratoryRate=m_BCMMPatient.RespiratoryRate;
  m_BCMMOxygen = m_BCMMPatient.OxygenSaturation;
  m_BCMMPulse = m_BCMMPatient.HeartRate;
  m_BCMMPressure = m_BCMMPatient.SystolicPressure;
  m_BCMMArrest = arrest;
  m_BCMMHypoxic = hypoxic;
  if (changed) Replication.BumpMe();
  UpdateConsciousness();
  GetGame().GetCallqueue().CallLater(BCMM_PhysiologyTick, 1000);
 }
 protected void BCMM_Monitor()
 {
  m_BCMMMonitorQueued = false;
  if (!GetOwner() || !BCMM_IsAuthority() || IsDestroyed()) return;
  UpdateConsciousness();
 }
 // Call only after the native saline effect has actually increased blood.
 void BCMM_RecordFluidDelivery(float deliveredReserve)
 {
  if (!BCMM_IsAuthority() || IsDestroyed() || m_BCMMDeathPermitted || !m_pBloodHitZone) return;
  if (m_BCMMDepletion.RecordDeliveredRescueFluid(BCMM_Now(), deliveredReserve, m_pBloodHitZone.GetHealth()))
  {
   m_BCMMDepleted = false;
   Replication.BumpMe();
   UpdateConsciousness();
  }
 }
 void BCMM_RecordTypedFluid(float deliveredNative,int fluidKind,float beforeNative)
 {
  if(!BCMM_IsAuthority() || !m_pBloodHitZone || deliveredNative<=0) { return; }
  float maximum=m_pBloodHitZone.GetMaxHealth();
  if(maximum<=0) { return; }
  m_BCMMPatient.Fluids.RecordDelivery(fluidKind,beforeNative/maximum,m_pBloodHitZone.GetHealthScaled(),m_BCMMClinicalSettings);
  BCMM_RecordFluidDelivery(deliveredNative);
 }
 // Original finite fluid adapter records only the native reserve delta actually delivered.
 float BCMM_DeliverReserveMl(float volume,int fluidKind=BCMM_FluidKind.WHOLE_BLOOD)
 {
  if (!BCMM_IsAuthority() || IsDestroyed() || m_BCMMDeathPermitted || !m_pBloodHitZone || !(volume > 0)) return 0;
  if(!BCMM_ClinicalSafety.FluidDenial(m_BCMMPatient,m_pBloodHitZone.GetHealthScaled(),m_BCMMClinicalSettings,fluidKind).IsEmpty()) { return 0; }
  if(fluidKind<BCMM_FluidKind.SALINE || fluidKind>BCMM_FluidKind.WHOLE_BLOOD) { return 0; }
  float before = m_pBloodHitZone.GetHealth();
  float requested = Math.Min(volume, m_BCMMClinicalSettings.Get("normalBloodVolumeMl"));
  float delta = requested / m_BCMMClinicalSettings.Get("normalBloodVolumeMl") * m_pBloodHitZone.GetMaxHealth();
  m_pBloodHitZone.SetHealth(Math.Min(m_pBloodHitZone.GetMaxHealth(), before + delta));
  float actual = Math.Max(0, m_pBloodHitZone.GetHealth() - before);
  BCMM_RecordTypedFluid(actual,fluidKind,before);
  return actual;
 }
 // Millilitre result for finite physical supplies; conversion uses the exact
 // clinical settings used by delivery, including during live GM changes.
 float BCMM_DeliverFluidVolumeMl(float volume,int fluidKind)
 {
  if(!BCMM_IsAuthority() || !m_pBloodHitZone)return 0;
  float maximum=m_pBloodHitZone.GetMaxHealth();
  if(maximum<=0)return 0;
  return BCMM_DeliverReserveMl(volume,fluidKind)*m_BCMMClinicalSettings.Get("normalBloodVolumeMl")/maximum;
 }
 bool BCMM_PauseBloodRegeneration()
 {
  return BCMM_TreatmentAccess.Setting("naturalBloodRegenerationEnabled")<0.5 || m_BCMMDepleted || (m_pBloodHitZone && m_pBloodHitZone.GetHealth() <= 0);
 }
 protected override void OnDamage(notnull BaseDamageContext damageContext)
 {
  super.OnDamage(damageContext);
  if (BCMM_IsAuthority())
  {
   BCMM_RecordImpact(damageContext);
   UpdateConsciousness();
  }
 }
 protected override void OnHealthSet()
 {
  super.OnHealthSet();
  UpdateConsciousness();
 }
 // Native death can synchronously deliver damage and health callbacks.
 protected void BCMM_KillNative(Instigator instigator)
 {
  bool wasUpdating=m_BCMMUpdating;m_BCMMUpdating=true;
  BCMM_FreezeEquipment();super.Kill(instigator);
  m_BCMMUpdating=wasUpdating;
 }
 override void Kill(notnull Instigator instigator)
 {
  if (!BCMM_IsAuthority()) return;
  if (m_BCMMDeathPermitted)
  {
   BCMM_KillNative(instigator);
   return;
  }
  ForceUnconsciousness();
 }
 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  if (BCMM_IsAuthority())
  {
   GetGame().GetCallqueue().CallLater(UpdateConsciousness);
   m_BCMMLastPhysiologyTime = BCMM_Now();
   GetGame().GetCallqueue().CallLater(BCMM_PhysiologyTick, 1000);
  }
 }
 void ~SCR_CharacterDamageManagerComponent()
 {
  if (!GetGame() || !GetGame().GetCallqueue()) return;
  GetGame().GetCallqueue().Remove(BCMM_PhysiologyTick);
  GetGame().GetCallqueue().Remove(BCMM_Monitor);
  GetGame().GetCallqueue().Remove(UpdateConsciousness);
 }
}
modded class SCR_CharacterHealthHitZone
{
 override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
 {
  float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
  SCR_CharacterDamageManagerComponent manager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
  if (!manager || damage <= 0 || manager.BCMM_CanDie()) return damage;
  return Math.Min(damage, Math.Max(0, GetHealth() - SCR_CharacterDamageManagerComponent.BCMM_HEALTH_FLOOR));
 }
}
modded class SCR_CharacterHitZone
{
 protected override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
 {
  super.OnDamageStateChanged(newState, previousDamageState, isJIP);
  if (IsProxy() || isJIP || newState != EDamageState.DESTROYED) return;
  SCR_CharacterDamageManagerComponent manager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
  if (!manager) return;
  manager.BCMM_RecordFracture(this);
  manager.AddBleedingEffectOnHitZone(this);
  manager.UpdateConsciousness();
 }
}
modded class SCR_CharacterBloodHitZone
{
 protected override void OnHealthSet()
 {
  super.OnHealthSet();
  SCR_CharacterDamageManagerComponent manager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
  if (manager) manager.UpdateConsciousness();
 }
}
modded class SCR_PassiveHitZoneRegenDamageEffect
{
 protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
 {
  SCR_CharacterDamageManagerComponent character = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
  if (character && GetAffectedHitZone() == character.GetBloodHitZone() && character.BCMM_PauseBloodRegeneration())
  {
   m_fDelayTimer += timeSlice;
   return;
  }
  super.EOnFrame(timeSlice, dmgManager);
 }
}
modded class SCR_SalineDamageEffect
{
 protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
 {
  SCR_CharacterDamageManagerComponent character = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
  if(Replication.IsServer() && character && (!BCMM_NativeTreatmentSafety.FluidAllowed(character.GetOwner()) || !BCMM_NativeSalineBinding.SourceAllowsFlow(character.GetOwner()))) { Terminate();return; }
  float before;
  if (character && character.GetBloodHitZone()) before = character.GetBloodHitZone().GetHealth();
  super.EOnFrame(timeSlice, dmgManager);
  if (character && character.GetBloodHitZone()) character.BCMM_RecordTypedFluid(character.GetBloodHitZone().GetHealth() - before,BCMM_FluidKind.SALINE,before);
 }
}
modded class SCR_BaseGameMode
{
 protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
 {
  // Preserve the server-known player name before native respawn/controller cleanup.
  if (Replication.IsServer() && playerEntity && GetGame().GetPlayerManager())
  {
   SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(playerEntity.FindComponent(SCR_CharacterDamageManagerComponent));
   if (medical) medical.BCMM_CaptureDeceasedName(GetGame().GetPlayerManager().GetPlayerName(playerId));
  }
  super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
 }
}
modded class SCR_CharacterControllerComponent
{
 override bool CanJumpClimb()
 {
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(GetCharacter().GetDamageManager());
  if (medical && !medical.BCMM_LegsAllowJump()) return false;
  return super.CanJumpClimb();
 }
}



