// Native finite equipment and real medication supplies; unpublished diagnostic addon.
class BCMM_OxygenMedicationRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected bool m_OralApproached;
 protected IEntity m_Oxygen;protected float m_BeforeGas;protected int m_WakeWait;protected int m_StanceWait;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");m_Actor.SetOrigin("100 2.25 0");m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  RplComponent ownership=RplComponent.Cast(m_Actor.FindComponent(RplComponent));if(ownership) ownership.GiveExt(GetGame().GetPlayerController().GetRplIdentity(),false);
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Oxygen/medication fixture has native inventory");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_TestRoleSetting("oxygenCapacityLitres",10);mode.BCMM_TestRoleSetting("oxygenFlowLitresPerMinute",20);
  m_Oxygen=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());m_Suite.Record(m_Oxygen && m_Inventory.TryInsertItem(m_Oxygen,EStoragePurpose.PURPOSE_ANY),"Supplied oxygen model becomes an actual finite inventory device");
  m_Medical.BCMM_GetPatient().LeftChest.CollapseFraction=0.8;GetGame().GetCallqueue().CallLater(StartOxygen,2000);
 }
 protected float Gas() { return BCMM_EquipmentItemComponent.Cast(m_Oxygen.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres(); }
 protected void StartOxygen()
 {
  m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,18,BCMM_BodyRegion.LEFT_ARM),"Oxygen connection rejects a wrong body region");
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,18,BCMM_BodyRegion.HEAD),"Default ten-second oxygen connection starts with actual device");
  GetGame().GetCallqueue().CallLater(CheckOxygenMid,5000);GetGame().GetCallqueue().CallLater(CheckConnected,18000);
 }
 protected void CheckOxygenMid() { m_Suite.Record(m_Oxygen && Gas()==10 && !m_Medical.BCMM_GetPatient().Equipment.OxygenActive && ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Connecting oxygen animates before supply or depletion begins"); }
 protected void CheckConnected()
 {
  PrintFormat("BCMM_OXYGEN_NATIVE gas=%1 active=%2 flags=%3 inventory=%4",Gas(),m_Medical.BCMM_GetPatient().Equipment.OxygenActive,m_Medical.BCMM_AppliedEquipment(),m_Inventory.Contains(m_Oxygen));
  m_Suite.Record(m_Oxygen && m_Inventory.Contains(m_Oxygen) && Gas()<10 && Gas()>5 && m_Medical.BCMM_GetPatient().Equipment.OxygenActive && (m_Medical.BCMM_AppliedEquipment() & 64),"Completed connection retains finite kit, consumes gas and publishes active mask");
  m_BeforeGas=Gas();m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,19,BCMM_BodyRegion.HEAD),"Timed oxygen disconnection uses retained device");GetGame().GetCallqueue().CallLater(CheckDisconnected,11000);
 }
 protected void CheckDisconnected()
 {
  m_Suite.Record(!m_Medical.BCMM_GetPatient().Equipment.OxygenActive && Gas()<m_BeforeGas && m_Oxygen,"Disconnection stops active state without resetting remaining capacity");m_BeforeGas=Gas();GetGame().GetCallqueue().CallLater(Reconnect,1500);
 }
 protected void Reconnect()
 {
  m_Suite.Record(Math.AbsFloat(Gas()-m_BeforeGas)<0.001,"Disconnected oxygen does not continue draining");
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,18,BCMM_BodyRegion.HEAD),"The same partially used kit reconnects");GetGame().GetCallqueue().CallLater(CheckDepleted,35000);
 }
 protected void CheckDepleted()
 {
  m_Suite.Record(Gas()==0 && m_Oxygen && !m_Medical.BCMM_GetPatient().Equipment.OxygenActive && !(m_Medical.BCMM_AppliedEquipment() & 64),"Empty oxygen stops supply and replicated mask while preserving its exact device");
  m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,18,BCMM_BodyRegion.HEAD),"An empty device cannot create a new oxygen session");
  IEntity replacement=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());
  m_Suite.Record(replacement && m_Inventory.TryInsertItem(replacement,EStoragePurpose.PURPOSE_ANY) && BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN)==replacement && Gas()==0,"A usable spare is selected ahead of the retained empty oxygen device without refilling the old source");
  m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,2);m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Kit=Spawn("{D0F8A8C2097AA101}Prefabs/Items/BCMM_Fentanyl.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Distinct fentanyl supply enters native inventory");
  GetGame().GetCallqueue().CallLater(StartFentanyl,1500);
 }
 protected void StartFentanyl()
 {
  m_Suite.Record(!BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,5),"Stronger opioid rejects an ordinary non-medic by default");SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(true);
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,5),"Qualified medic starts finite stronger-opioid injection");GetGame().GetCallqueue().CallLater(CheckFentanyl,20000);
 }
 protected void CheckFentanyl()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.HasActiveKind(5) && m_Medical.BCMM_RespiratoryRate()<16 && m_Medical.BCMM_SystolicPressure()<120,"Completed fentanyl consumes exactly one kit and changes native breathing/pressure");
  m_Kit=Spawn("{6AB4B3F3FD216013}Prefabs/Items/BCMM_Naloxone.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Native reversal fixture has finite naloxone");
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,1),"Naloxone is permitted for fentanyl-only exposure");GetGame().GetCallqueue().CallLater(CheckReversed,20000);
 }
 protected void CheckReversed()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.BreathingScale>0.9,"Native naloxone administration reverses fentanyl respiratory effects");
  m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,2);m_Kit=Spawn("{D0F8A8C2097AA102}Prefabs/Items/BCMM_Paracetamol.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Distinct oral tablet placeholder enters native inventory");
  m_Medical.BCMM_FieldFixtureUnconscious(true);GetGame().GetCallqueue().CallLater(CheckOralConsciousness,1500);
 }
 protected void CheckOralConsciousness()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious() && !BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,4) && m_Kit,"Actually unresponsive native casualty cannot receive tablets even with stable model vitals");
  m_Medical.BCMM_FieldFixtureUnconscious(false);m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,2);GetGame().GetCallqueue().CallLater(StartTablets,2000);
 }
 protected void StartTablets()
 {
  if(ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious() && m_WakeWait++<24) { GetGame().GetCallqueue().CallLater(StartTablets,500);return; }
  CharacterControllerComponent actorPose=ChimeraCharacter.Cast(m_Actor).GetCharacterController();CharacterControllerComponent targetPose=ChimeraCharacter.Cast(m_Target).GetCharacterController();
  if((actorPose.GetStance()!=ECharacterStance.STAND || targetPose.GetStance()!=ECharacterStance.STAND) && m_StanceWait++<24) { actorPose.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);targetPose.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);GetGame().GetCallqueue().CallLater(StartTablets,500);return; }
  if(!m_OralApproached)
  {
   // Native recovery has moved the casualty since the previous procedure.
   // Approach before this new request; keep all runtime range checks active.
   BCMM_TestAccessDiagnostics.Trace(m_Actor,m_Target);
   vector approach[4];m_Actor.GetWorldTransform(approach);approach[3]=m_Target.GetOrigin()+Vector(0,0.1,-1);ChimeraCharacter.Cast(m_Actor).Teleport(approach);
   m_OralApproached=true;GetGame().GetCallqueue().CallLater(StartTablets,1000);return;
  }
  PrintFormat("BCMM_ORAL_READY unconscious=%1 kit=%2 found=%3 reach=%4 eligible=%5 rr=%6",ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious(),m_Kit!=null,BCMM_MedicationTreatment.FindMedication(m_Actor,4)!=null,BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),BCMM_MedicationTreatment.MedicationEligible(m_Target,4),m_Medical.BCMM_RespiratoryRate());
  BCMM_TestAccessDiagnostics.Trace(m_Actor,m_Target);
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,4),"Awake casualty can receive the finite oral supply");GetGame().GetCallqueue().CallLater(CheckTabletsAnimating,3000);GetGame().GetCallqueue().CallLater(CheckTablets,18000); }
 protected void CheckTabletsAnimating()
 {
  m_Suite.Record(BCMM_CPRAnimation.IsActive(m_Actor,BCMM_FieldClips.TABLETOTHER_GRAPH) && m_Kit,"Other-patient tablets play the owned native handover graph before consuming supply");
 }
 protected void CheckTablets()
 {
  m_Suite.Record(!m_Kit && m_Medical.BCMM_GetPatient().Medication.HasActiveKind(4) && m_Medical.BCMM_GetPatient().Medication.BreathingScale==1,"Tablets consume one supply and enter non-opioid history without breathing depression");
  GetGame().GetCallqueue().CallLater(StartSelfTablets,2500);
 }
 protected void StartSelfTablets()
 {
  SCR_CharacterDamageManagerComponent wearer=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));wearer.FullHeal();wearer.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);
  m_Kit=Spawn("{D0F8A8C2097AA102}Prefabs/Items/BCMM_Paracetamol.et",m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY) && BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Actor,4),"Self tablets start with a separate finite supply");
  GetGame().GetCallqueue().CallLater(CheckSelfTabletsAnimating,3000);GetGame().GetCallqueue().CallLater(CheckSelfTabletsDone,16000);
 }
 protected void CheckSelfTabletsAnimating() { m_Suite.Record(BCMM_CPRAnimation.IsActive(m_Actor,BCMM_FieldClips.TABLETSELF_GRAPH) && m_Kit,"Taking tablets plays the original self hand-to-mouth native graph before consumption"); }
 protected void CheckSelfTabletsDone()
 {
  SCR_CharacterDamageManagerComponent wearer=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(!m_Kit && wearer.BCMM_GetPatient().Medication.HasActiveKind(4),"Self tablet graph completes through the authoritative timer with exact consumption");
  IEntity source=BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.OXYGEN);
  BCMM_OxygenSession.Start(m_Actor,m_Actor,source);wearer.BCMM_FieldFixtureUnconscious(true);GetGame().GetCallqueue().CallLater(CheckUnconsciousWearer,2500);
 }
 protected void CheckUnconsciousWearer()
 {
  SCR_CharacterDamageManagerComponent wearer=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  PrintFormat("BCMM_OXYGEN_WEARER unconscious=%1 life=%2 active=%3 flags=%4",ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUnconscious(),ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetLifeState(),wearer.BCMM_GetPatient().Equipment.OxygenActive,wearer.BCMM_AppliedEquipment());
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUnconscious() && wearer.BCMM_GetPatient().Equipment.OxygenActive && (wearer.BCMM_AppliedEquipment() & 64),"Connected self oxygen survives actual native unconsciousness with its retained source");
  wearer.BCMM_FieldFixtureUnconscious(false);wearer.FullHeal();GetGame().GetCallqueue().CallLater(CheckSelfReset,1000);
 }
 protected void CheckSelfReset()
 {
  SCR_CharacterDamageManagerComponent wearer=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(!wearer.BCMM_GetPatient().Equipment.OxygenActive && !(wearer.BCMM_AppliedEquipment() & 64),"Patient reset clears the retained self oxygen session and mask");Finish();
 }
}

