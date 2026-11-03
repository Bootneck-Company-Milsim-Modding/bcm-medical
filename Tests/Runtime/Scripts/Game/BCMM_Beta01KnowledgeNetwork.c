// Dedicated two-owner privacy acceptance. Findings remain owner RPCs;
// replicated state here identifies only the patient and diagnostic phase.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMKnowledgePhase;
 [RplProp()] protected RplId m_BCMMKnowledgePatient = RplId.Invalid();
 protected IEntity m_BCMMKnowledgeSubject;
 protected int m_BCMMKnowledgeWait;
 int BCMM_BetaKnowledgePhase() { return m_BCMMKnowledgePhase; }
 IEntity BCMM_BetaKnowledgePatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMKnowledgePatient); }
 override void OnGameStart()
 {
  super.OnGameStart(); string enabled;
  if (Replication.IsServer() && System.GetCLIParam("bcmmBeta01KnowledgeServer", enabled)) GetGame().GetCallqueue().CallLater(BCMM_KnowledgePrepare, 500);
 }
 protected void BCMM_KnowledgePrepare()
 {
  PlayerManager players = GetGame().GetPlayerManager();
  SCR_PlayerController first = SCR_PlayerController.Cast(players.GetPlayerController(1));
  SCR_PlayerController second = SCR_PlayerController.Cast(players.GetPlayerController(2));
  if (!first || !second || !first.GetControlledEntity() || !second.GetControlledEntity())
  {
   if (m_BCMMKnowledgeWait++ < 160) GetGame().GetCallqueue().CallLater(BCMM_KnowledgePrepare, 250);
   else Print("BCMM_KNOWLEDGE_NET_FAILED two controlled owners unavailable");
   return;
  }
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = "101 1.2 0";
  m_BCMMKnowledgeSubject = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  first.GetControlledEntity().SetOrigin("100 1.2 0"); second.GetControlledEntity().SetOrigin("102 1.2 0");
  m_BCMMKnowledgePatient = BCMM_BodyBagComponent.Id(m_BCMMKnowledgeSubject); m_BCMMKnowledgePhase = 1; Replication.BumpMe();
  Print("BCMM_KNOWLEDGE_NET_PHASE 1");
  GetGame().GetCallqueue().CallLater(BCMM_KnowledgeSwitches, 10000);
 }
 protected void BCMM_KnowledgeSwitches()
 {
  m_BCMMKnowledgePhase = 2; Replication.BumpMe(); Print("BCMM_KNOWLEDGE_NET_PHASE 2");
  GetGame().GetCallqueue().CallLater(BCMM_KnowledgeInvalidate, 110000);
 }
 protected void BCMM_KnowledgeInvalidate()
 {
  SCR_CharacterDamageManagerComponent.Cast(m_BCMMKnowledgeSubject.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient().Equipment.AirwayAdjunct = true;
  m_BCMMKnowledgePhase = 3; Replication.BumpMe(); Print("BCMM_KNOWLEDGE_NET_PHASE 3");
 }
}
class BCMM_Beta01KnowledgeOwner
{
 protected static ref BCMM_Beta01KnowledgeOwner s_Owner;
 protected SCR_PlayerController m_Player;
 protected IEntity m_Patient;
 protected int m_Cycles;
 protected int m_Failures;
 static void Start(SCR_PlayerController player, IEntity patient)
 {
  s_Owner = new BCMM_Beta01KnowledgeOwner(); s_Owner.m_Player = player; s_Owner.m_Patient = patient;
  BCMM_MedicalMenu.Open(patient); GetGame().GetCallqueue().CallLater(s_Owner.Examine, 750);
 }
 protected void Examine()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current(); if (!menu) { Print("BCMM_KNOWLEDGE_NET_FAILED owner menu missing"); return; }
  menu.BCMM_BetaSelectPatient(m_Patient);
  if (m_Player.GetPlayerId() == 1) m_Player.BCMM_RequestClinicalCheck(m_Patient, 0, BCMM_ClinicalCheck.AIRWAY, menu.BCMM_BetaView());
  GetGame().GetCallqueue().CallLater(WaitSwitchPhase, 500);
 }
 protected void WaitSwitchPhase()
 {
  if (SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BetaKnowledgePhase() < 2) { GetGame().GetCallqueue().CallLater(WaitSwitchPhase, 250); return; }
  SwitchB();
 }
 protected void SwitchB() { BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Player.GetControlledEntity()); GetGame().GetCallqueue().CallLater(CheckB, 250); }
 protected void CheckB()
 {
  if (BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) m_Failures++;
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Patient); GetGame().GetCallqueue().CallLater(CheckA, 250);
 }
 protected void CheckA()
 {
  bool expected = m_Player.GetPlayerId() == 1;
  if (BCMM_MedicalMenu.Current().BCMM_BetaHasAirway() != expected) m_Failures++;
  if (++m_Cycles < 200) { SwitchB(); return; }
  PrintFormat("BCMM_KNOWLEDGE_NET_OWNER_COMPLETE id=%1 cycles=%2 failures=%3 expectedFinding=%4", m_Player.GetPlayerId(), m_Cycles, m_Failures, expected);
  GetGame().GetCallqueue().CallLater(WaitInvalidation, 250);
 }
 protected void WaitInvalidation()
 {
  if (SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_BetaKnowledgePhase() < 3) { GetGame().GetCallqueue().CallLater(WaitInvalidation, 250); return; }
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Patient); GetGame().GetCallqueue().CallLater(CheckInvalidation, 500);
 }
 protected void CheckInvalidation()
 {
  if (BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) m_Failures++;
  PrintFormat("BCMM_KNOWLEDGE_NET_COMPLETE id=%1 cycles=%2 failures=%3 invalidated=%4", m_Player.GetPlayerId(), m_Cycles, m_Failures, !BCMM_MedicalMenu.Current().BCMM_BetaHasAirway());
 }
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMKnowledgeOwnerStarted;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world, timeslice); string enabled;
  if (m_BCMMKnowledgeOwnerStarted || Replication.IsServer() || !System.GetCLIParam("bcmmBeta01KnowledgeOwner", enabled)) return;
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode()); SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (!mode || mode.BCMM_BetaKnowledgePhase() < 1 || !mode.BCMM_BetaKnowledgePatient() || !player || !player.GetControlledEntity()) return;
  vector matrix[4]; player.GetControlledEntity().GetWorldTransform(matrix); matrix[3] = "100 1.2 0"; if (player.GetPlayerId() == 2) matrix[3] = "102 1.2 0";
  ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(matrix);
  m_BCMMKnowledgeOwnerStarted = true; BCMM_Beta01KnowledgeOwner.Start(player, mode.BCMM_BetaKnowledgePatient());
 }
}
