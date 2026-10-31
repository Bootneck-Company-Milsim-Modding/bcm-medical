// Test-only replicated medical observer, including JIP while unconscious.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMMedicalTestPhase;
 [RplProp()] protected RplId m_BCMMMedicalTestPatient = RplId.Invalid();
 void BCMM_PublishMedicalTest(int phase, IEntity patient)
 {
  if (!Replication.IsServer()) return;
  m_BCMMMedicalTestPatient = BCMM_BodyBagComponent.Id(patient);
  m_BCMMMedicalTestPhase = phase;
  Replication.BumpMe();
  PrintFormat("BCMM_MED_NET_PHASE %1", phase);
 }
 int BCMM_MedicalTestPhase() { return m_BCMMMedicalTestPhase; }
 RplId BCMM_MedicalTestPatient() { return m_BCMMMedicalTestPatient; }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMLoggedCPRRequest;
 protected bool m_BCMMRequestedCPR;
 protected float m_BCMMNextCPRRequest;
 protected bool m_BCMMExamLogged;
 protected int m_BCMMObservedMedicalPhase;
 protected float m_BCMMNextMedicalObservation;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice);
  string enabled;
  if (Replication.IsServer() || !System.GetCLIParam("bcmmMedicalObserver", enabled) || world.GetWorldTime() < m_BCMMNextMedicalObservation) return;
  m_BCMMNextMedicalObservation = world.GetWorldTime() + 250;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGameMode());
  SCR_PlayerController player = SCR_PlayerController.Cast(GetPlayerController());
  if (player && !m_BCMMExamLogged && !player.BCMM_LastExamination().IsEmpty())
  {
   m_BCMMExamLogged = true;
   Print("BCMM_MED_EXAM_OWNER_RECEIVED");
  }
  if (!mode) return;
  if (mode.BCMM_MedicalTestPhase() == 1 && world.GetWorldTime() >= m_BCMMNextCPRRequest && player && player.GetPlayerId() == 1 && player.GetControlledEntity())
  {
   IEntity patient = BCMM_BodyBagComponent.Resolve(mode.BCMM_MedicalTestPatient());
   ChimeraCharacter medic = ChimeraCharacter.Cast(player.GetControlledEntity());
   if (!m_BCMMLoggedCPRRequest && patient)
   {
    m_BCMMLoggedCPRRequest = true;

    PrintFormat("BCMM_MED_CPR_ACCESS player=%1 actor=%2 patient=%3 life=%4 falling=%5 velocity=%6 reach=%7", player.GetPlayerId(), medic.GetOrigin(), patient.GetOrigin(), medic.GetCharacterController().GetLifeState(), medic.GetCharacterController().IsFalling(), medic.GetCharacterController().GetVelocity(), BCMM_TreatmentAccess.CanReach(medic, patient));
   }
   if (patient && !m_BCMMRequestedCPR && !player.BCMM_Compressing(patient) && medic && medic.GetCharacterController().GetMovementInput().Length() <= 0.1 && !medic.GetCharacterController().IsWeaponRaised() && BCMM_TreatmentAccess.CanReach(player.GetControlledEntity(), patient))
   {
    m_BCMMNextCPRRequest = world.GetWorldTime() + 1000;
    player.BCMM_RequestCPR(patient);
    if (!m_BCMMRequestedCPR)
    {
     m_BCMMRequestedCPR = true;
     player.BCMM_RequestExamination(patient);
     Print("BCMM_MED_CPR_OWNER_REQUESTED");
    }
   }
  }
  if (mode.BCMM_MedicalTestPhase() <= m_BCMMObservedMedicalPhase) return;
  IEntity entity = BCMM_BodyBagComponent.Resolve(mode.BCMM_MedicalTestPatient());
  if (!entity) return;
  ChimeraCharacter character = ChimeraCharacter.Cast(entity);
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!character || !medical) return;
  bool injured = mode.BCMM_MedicalTestPhase() == 1;
  if (injured)
  {
   if (!medical.BCMM_InCardiacArrest() || medical.BCMM_Pulse() != 0 || medical.BCMM_LegsAllowJump() || medical.BCMM_MovementCap() >= 1 || !character.GetCharacterController().IsUnconscious() || medical.BCMM_OxygenSaturation() >= 0.9) return;
  }
  else
  {
   if (medical.BCMM_InCardiacArrest() || medical.BCMM_Pulse() <= 0 || !medical.BCMM_LegsAllowJump() || medical.BCMM_MovementCap() != 1 || character.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || medical.BCMM_OxygenSaturation() < 0.97) return;
  }
  m_BCMMObservedMedicalPhase = mode.BCMM_MedicalTestPhase();
  PrintFormat("BCMM_MED_NET_OBSERVED phase=%1 arrest=%2 pulse=%3 oxygen=%4 jump=%5 cap=%6", m_BCMMObservedMedicalPhase, medical.BCMM_InCardiacArrest(), medical.BCMM_Pulse(), medical.BCMM_OxygenSaturation(), medical.BCMM_LegsAllowJump(), medical.BCMM_MovementCap());
 }
}