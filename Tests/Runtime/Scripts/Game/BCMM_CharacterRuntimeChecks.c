// Native character damage fixture. Domain-only tests cannot prove these hooks.
class BCMM_CharacterRuntimeChecks
{
 protected BCMM_DomainChecks m_Suite;
 protected IEntity m_Actor;
 protected SCR_CharacterDamageManagerComponent m_Damage;
 protected float m_BloodBefore;
 protected int m_CPRWait;
 void Start(BCMM_DomainChecks suite)
 {
  m_Suite = suite;
  m_Suite.Record(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("naturalBloodRegenerationEnabled",1),"Native regeneration rescue fixture explicitly enables its GM policy");
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = Vector(100, GetGame().GetWorld().GetSurfaceY(100, 0) + 0.2, 0);
  m_Actor = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  m_Suite.Record(m_Actor != null, "Native medical character spawns");
  GetGame().GetCallqueue().CallLater(Deplete, 1500);
 }
 protected void Deplete()
 {
  if (!m_Actor) { m_Suite.Complete(); return; }
  m_Damage = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(m_Damage && m_Damage.BCMM_GetPatient(), "Native damage manager owns standalone patient");
  if (!m_Damage || !m_Damage.GetBloodHitZone()) { Finish(); return; }
  m_BloodBefore = m_Damage.GetBloodHitZone().GetHealth();
  m_Damage.GetBloodHitZone().SetHealth(0);
  m_Damage.UpdateConsciousness();
  m_Suite.Record(m_Damage.BCMM_IsDepleted(), "Native blood zero starts BCM depletion policy");
  m_Suite.Record(!m_Damage.IsDestroyed() && !m_Damage.BCMM_CanDie(), "Native depletion keeps casualty alive during grace");
  GetGame().GetCallqueue().CallLater(CheckUnconscious, 250);
 }
 protected void CheckUnconscious()
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(m_Actor);
  m_Suite.Record(character.GetCharacterController().IsUnconscious(), "Native casualty actually becomes unconscious");
  m_Suite.Record(m_Damage.BCMM_PauseBloodRegeneration(), "Native passive blood regeneration is paused at depletion");
  m_Damage.GetBloodHitZone().SetHealth(m_BloodBefore);
  m_Damage.UpdateConsciousness();
  m_Suite.Record(m_Damage.BCMM_IsDepleted(), "Unrelated blood setter cannot bypass rescue policy");
  m_Damage.BCMM_RecordFluidDelivery(0);
  m_Suite.Record(m_Damage.BCMM_IsDepleted(), "Zero fluid delivery cannot rescue native casualty");
  m_Damage.BCMM_RecordFluidDelivery(m_BloodBefore);
  m_Suite.Record(!m_Damage.BCMM_IsDepleted(), "Confirmed positive native fluid delivery rescues during grace");
  m_Suite.Record(!m_Damage.BCMM_PauseBloodRegeneration(), "Rescued native casualty can regenerate blood again");
  m_Damage.GetBloodHitZone().SetHealth(0);
  m_Damage.UpdateConsciousness();
  m_Suite.Record(m_Damage.BCMM_IsDepleted(), "A second depletion starts after real rescue");
  m_Damage.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Damage.FullHeal();
  m_Suite.Record(!m_Damage.BCMM_IsDepleted() && !m_Damage.BCMM_CanDie(), "Explicit native full heal resets depletion and death latch");
  m_Suite.Record(!m_Damage.BCMM_GetPatient().LeftLeg.Broken, "Explicit full heal resets standalone patient injuries");
  m_Suite.Record(m_Damage.GetBloodHitZone().GetHealthScaled() > 0.99, "Native full heal restores blood hit zone");
  m_Damage.BCMM_CaptureDeceasedName("BCM medical fixture");
  m_Damage.BCMM_CaptureDeceasedName("changed identity");
  m_Suite.Record(m_Damage.BCMM_DeceasedName() == "BCM medical fixture", "Captured corpse identity cannot be overwritten");
  BCMM_Settings settings = new BCMM_Settings();
  m_Damage.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Damage.BCMM_PublishFractures();
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(ChimeraCharacter.Cast(m_Actor).GetCharacterController());
  m_Suite.Record(!controller.CanJumpClimb(), "Fractured native character cannot jump or climb");
  float unsplinted = m_Damage.BCMM_MovementCap();
  m_Damage.BCMM_GetPatient().LeftLeg.ApplySplint();
  m_Damage.BCMM_PublishFractures();
  m_Suite.Record(m_Damage.BCMM_MovementCap() > unsplinted && !controller.CanJumpClimb(), "Splint improves native movement cap without restoring jumping");
  m_Damage.FullHeal();
  m_Suite.Record(controller.CanJumpClimb() && m_Damage.BCMM_MovementCap() == 1, "Full heal clears native fracture movement restrictions");
  float bloodBeforeInternal = m_Damage.GetBloodHitZone().GetHealth();
  m_Damage.BCMM_GetPatient().AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 100);
  m_Damage.BCMM_TestAdvanceTrauma(1);
  float expectedLoss = 100 * m_Damage.GetBloodHitZone().GetMaxHealth() / settings.Get("normalBloodVolumeMl");
  m_Suite.Record(Math.AbsFloat(bloodBeforeInternal - m_Damage.GetBloodHitZone().GetHealth() - expectedLoss) < 0.01, "Internal bleeding drains actual native circulating reserve exactly once");
  m_Suite.Record(m_Damage.BCMM_GetPatient().LeftChest.PleuralBloodMl == 100, "Thoracic bleeding accumulates in matching pleural space");
  m_Damage.FullHeal();
  m_Damage.AddParticularBleeding("Chest", ECharacterDamageState.WOUNDED, 0.5);
  m_BloodBefore = m_Damage.GetBloodHitZone().GetHealth();
  GetGame().GetCallqueue().CallLater(CheckBleeding, 1250);
 }
 protected void CheckBleeding()
 {
  m_Suite.Record(m_Damage.BCMM_GetPatient().WoundCount() > 0, "Native bleeding effect creates persistent BCM wound record");
  m_Suite.Record(m_Damage.GetBloodHitZone().GetHealth() < m_BloodBefore, "External blood loss reaches actual native blood reserve");
  m_Damage.RemoveGroupBleeding(ECharacterHitZoneGroup.UPPERTORSO);
  m_Suite.Record(m_Damage.BCMM_GetPatient().TotalExternalFlowMlPerSecond() == 0, "Native bandage dresses linked BCM wounds");
  m_BloodBefore = m_Damage.GetBloodHitZone().GetHealth();
  GetGame().GetCallqueue().CallLater(CheckDressing, 1250);
 }
 protected void CheckDressing()
 {
  m_Suite.Record(!m_Damage.IsBleeding(), "Native bleeding effect ends after bandage");
  m_Suite.Record(m_Damage.GetBloodHitZone().GetHealth() >= m_BloodBefore - 0.01, "Dressed wound does not independently drain blood a second time");
  m_Damage.BCMM_TestAdvanceTrauma(601);
  m_Suite.Record(m_Damage.BCMM_GetPatient().TotalExternalFlowMlPerSecond() > 0, "Unsutured native dressing reopens after wear budget");
  GetGame().GetCallqueue().CallLater(CheckReopening, 500);
 }
 protected void CheckReopening()
 {
  m_Suite.Record(m_Damage.IsBleeding(), "Reopened BCM wound reinstates actual native bleeding effect");
  m_Damage.FullHeal();
  m_Suite.Record(m_Damage.BCMM_GetPatient().WoundCount() == 0, "Explicit full heal clears native wound links and records");
  string network;
  if (System.GetCLIParam("bcmmMedicalNetwork", network)) StartMedicalNetwork();
  else { CheckNativePenetratingImpact(); Finish(); }
 }
 protected void CheckNativePenetratingImpact()
 {
  m_Damage.FullHeal();
  m_Damage.BCMM_TestSetClinical("penetratingChestInjuryChance", 1);
  m_Damage.BCMM_TestSetClinical("penetratingInternalBleedChance", 1);
  m_Damage.BCMM_TestSetClinical("initialTensionChance", 1);
  SCR_CharacterHitZone zone = SCR_CharacterHitZone.Cast(m_Damage.GetHitZoneByName("Chest"));
  m_Suite.Record(zone != null, "Native impact fixture has actual chest hit zone");
  if (!zone) return;
  vector hit[3];
  vector worldTransform[4];
  m_Actor.GetWorldTransform(worldTransform);
  hit[0] = worldTransform[3] - worldTransform[0] * 0.2 + worldTransform[1] * 1.3;
  hit[1] = "0 0 1";
  SCR_DamageContext context = new SCR_DamageContext(EDamageType.KINETIC, zone.GetMaxHealth(), hit, m_Actor, zone, Instigator.CreateInstigator(m_Actor), null, -1, -1);
  m_Damage.BCMM_TestDeterministicImpact(true);
  m_Damage.HandleDamage(context);
  m_Damage.BCMM_TestDeterministicImpact(false);
  BCMM_Patient patient = m_Damage.BCMM_GetPatient();
  m_Suite.Record(patient.WoundCount() > 0 && patient.LeftChest.OpenWound && patient.LeftChest.Tension && !patient.RightChest.OpenWound, "Real native damage event generates BCM chest trauma on impacted side");
  m_Suite.Record(patient.InternalBleedCount() > 0 && patient.InternalBleedAt(0).Compartment == BCMM_BleedCompartment.LEFT_PLEURAL, "Real native penetrating event creates matching internal bleeding source");
  m_Damage.FullHeal();
 }
 protected void StartMedicalNetwork()
 {
  m_Damage.BCMM_GetPatient().LeftLeg.Broken = true;
  m_Damage.BCMM_GetPatient().HasSpontaneousCirculation = false;
  m_Damage.BCMM_GetPatient().Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  m_Damage.BCMM_GetPatient().OxygenSaturation = 0.8;
  m_Damage.BCMM_PublishFractures();
  GetGame().GetCallqueue().CallLater(PublishMedicalNetwork, 1500);
 }
 protected void PublishMedicalNetwork()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishMedicalTest(1, m_Actor);
  GetGame().GetCallqueue().CallLater(CheckNetworkCPR, 5000);
  GetGame().GetCallqueue().CallLater(RecoverMedicalNetwork, 25000);
 }
 protected void CheckNetworkCPR()
 {
  SCR_PlayerController first = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  BCMM_CPRSession session = BCMM_CPRSession.Find(first);
  if ((!session || session.Preparing) && m_CPRWait++ < 60)
  {
   GetGame().GetCallqueue().CallLater(CheckNetworkCPR, 250);
   return;
  }
  m_Suite.Record(session && m_Damage.BCMM_GetPatient().HasCompressions(), "Authenticated owner RPC starts native CPR session");
  if (!session) return;
  SCR_PlayerController second = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(2));
  m_Suite.Record(BCMM_CPRSession.Begin(second, m_Actor) == 0, "Second client cannot acquire another medic's CPR patient");
  m_Suite.Record(!BCMM_CPRSession.Renew(first, session.Token + 1), "Stale CPR token cannot renew another session");
  BCMM_CPRSession.Stop(second, session.Token);
  m_Suite.Record(m_Damage.BCMM_GetPatient().HasCompressions(), "Another client cannot stop active CPR with copied token");
  IEntity medic = first.GetControlledEntity();
  // Ownership belongs to the client. Revoke the exact authenticated session;
  // the separate animation-network fixture tests real owner displacement.
  BCMM_CPRSession.Stop(first, session.Token);
  GetGame().GetCallqueue().CallLater(CheckNetworkCPRReleased, 500);
 }
 protected void CheckNetworkCPRReleased()
 {
  m_Suite.Record(!m_Damage.BCMM_GetPatient().HasCompressions(), "Revoking the exact owner session releases native CPR");
 }
 protected void RecoverMedicalNetwork()
 {
  m_Damage.FullHeal();
  GetGame().GetCallqueue().CallLater(PublishRecoveredNetwork, 3000);
 }
 protected void PublishRecoveredNetwork()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishMedicalTest(2, m_Actor);
  GetGame().GetCallqueue().CallLater(Finish, 10000);
 }
 protected void Finish()
 {
  if (m_Actor) SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  m_Suite.Complete();
 }
}
// Diagnostic seam is confined to the unpublished test addon.
modded class SCR_CharacterDamageManagerComponent
{
 override protected void OnDamage(notnull BaseDamageContext damageContext)
 {
  if (damageContext.damageType == EDamageType.KINETIC) PrintFormat("BCMM_IMPACT_TEST before value=%1 effect=%2 destroyed=%3 zone=%4", damageContext.damageValue, damageContext.damageEffect, IsDestroyed(), damageContext.struckHitZone);
  super.OnDamage(damageContext);
  if (damageContext.damageType == EDamageType.KINETIC) PrintFormat("BCMM_IMPACT_TEST after wounds=%1 internal=%2 chance=%3", m_BCMMPatient.WoundCount(), m_BCMMPatient.InternalBleedCount(), m_BCMMClinicalSettings.Get("penetratingChestInjuryChance"));
 }
 protected bool m_BCMMZeroTraumaSamples;
 void BCMM_TestDeterministicImpact(bool enabled) { m_BCMMZeroTraumaSamples = enabled; }
 protected override float BCMM_TraumaSample()
 {
  if (m_BCMMZeroTraumaSamples) return 0;
  return super.BCMM_TraumaSample();
 }
 void BCMM_TestSetClinical(string key, float value) { m_BCMMClinicalSettings.Set(key, value); }
 void BCMM_TestAdvanceTrauma(float seconds) { BCMM_AdvanceNativeTrauma(seconds); }
}