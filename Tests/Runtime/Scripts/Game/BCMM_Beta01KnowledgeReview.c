modded class BCMM_MedicalMenu
{
 void BCMM_BetaSelectPatient(IEntity patient) { SelectTarget(patient); SelectRegion(BCMM_BodyRegion.HEAD); SelectTab(1); }
 int BCMM_BetaView() { return m_View; }
 bool BCMM_BetaHasAirway() { return m_Findings.Contains("0:2"); }
}
// Real local owner's examination request/reply and menu switching lifecycle.
class BCMM_Beta01KnowledgeReview
{
 protected static ref BCMM_Beta01KnowledgeReview s_Review;
 protected IEntity m_Actor;
 protected IEntity m_Patient;
 protected int m_Cycles;
 protected int m_Failures;
 static void Start(IEntity actor)
 {
  s_Review = new BCMM_Beta01KnowledgeReview(); s_Review.m_Actor = actor;
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = actor.GetOrigin() + "0.8 0 0";
  s_Review.m_Patient = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  GetGame().GetCallqueue().CallLater(s_Review.Open, 1500);
 }
 protected void Open() { BCMM_MedicalMenu.Open(m_Patient); GetGame().GetCallqueue().CallLater(Examine, 500); }
 protected void Examine()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current(); menu.BCMM_BetaSelectPatient(m_Patient);
  SCR_PlayerController.Cast(GetGame().GetPlayerController()).BCMM_RequestClinicalCheck(m_Patient, BCMM_BodyRegion.HEAD, BCMM_ClinicalCheck.AIRWAY, menu.BCMM_BetaView());
  GetGame().GetCallqueue().CallLater(Examined, 3500);
 }
 protected void Examined()
 {
  if (!BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) { Print("BCMM_KNOWLEDGE_UI_FAILED initial owner examination reply absent"); return; }
  FileIO.MakeDirectory("$profile:screenshots"); System.MakeScreenshot("$profile:screenshots/completed-airway-examination");
  SwitchToB();
 }
 protected void SwitchToB()
 {
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Actor);
  GetGame().GetCallqueue().CallLater(CheckB, 200);
 }
 protected void CheckB()
 {
  if (BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) m_Failures++;
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Patient);
  GetGame().GetCallqueue().CallLater(CheckA, 200);
 }
 protected void CheckA()
 {
  if (!BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) m_Failures++;
  m_Cycles++;
  if (m_Cycles % 20 == 0) PrintFormat("BCMM_KNOWLEDGE_UI cycles=%1 failures=%2", m_Cycles, m_Failures);
  if (m_Cycles < 200) { SwitchToB(); return; }
  System.MakeScreenshot("$profile:screenshots/airway-after-200-patient-switches");
  SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient().Equipment.AirwayAdjunct = true;
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Actor);
  GetGame().GetCallqueue().CallLater(RevisitChanged, 200);
 }
 protected void RevisitChanged()
 {
  BCMM_MedicalMenu.Current().BCMM_BetaSelectPatient(m_Patient);
  GetGame().GetCallqueue().CallLater(CheckInvalidation, 200);
 }
 protected void CheckInvalidation()
 {
  if (BCMM_MedicalMenu.Current().BCMM_BetaHasAirway()) m_Failures++;
  PrintFormat("BCMM_KNOWLEDGE_UI_COMPLETE cycles=%1 failures=%2 changedDomainInvalidated=%3 remoteOwnerPrivacyCycles=0", m_Cycles, m_Failures, !BCMM_MedicalMenu.Current().BCMM_BetaHasAirway());
  System.MakeScreenshot("$profile:screenshots/airway-invalidated-after-device-change");
 }
}
