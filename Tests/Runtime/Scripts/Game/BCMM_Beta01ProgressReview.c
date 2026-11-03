// Actual local owner/server procedure with menu and closed-menu captures.
class BCMM_Beta01ProgressReview
{
 protected static ref BCMM_Beta01ProgressReview s_Review;
 protected IEntity m_Actor;
 protected IEntity m_Oxygen;
 static void Start(IEntity actor)
 {
  s_Review = new BCMM_Beta01ProgressReview(); s_Review.m_Actor = actor;
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = actor.GetOrigin();
  s_Review.m_Oxygen = GetGame().SpawnEntityPrefab(Resource.Load("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et"), GetGame().GetWorld(), spawn);
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  inventory.TryInsertItem(s_Review.m_Oxygen, EStoragePurpose.PURPOSE_ANY);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("oxygenApplicationSeconds", 15);
  GetGame().GetCallqueue().CallLater(s_Review.Begin, 2500);
 }
 protected void Begin()
 {
  BCMM_MedicalMenu.Open(m_Actor);
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  bool accepted = BCMM_EquipmentTreatment.BeginEquipment(m_Actor, m_Actor, BCMM_EquipmentProcedure.CONNECT_OXYGEN, BCMM_BodyRegion.HEAD, player);
  PrintFormat("BCMM_PROGRESS_REVIEW started=%1", accepted);
  GetGame().GetCallqueue().CallLater(CaptureMenu, 4000);
  GetGame().GetCallqueue().CallLater(CloseMenu, 6000);
  GetGame().GetCallqueue().CallLater(CaptureHUD, 9000);
  GetGame().GetCallqueue().CallLater(Verify, 18000);
 }
 protected void CaptureMenu()
 {
  FileIO.MakeDirectory("$profile:screenshots");
  System.MakeScreenshot("$profile:screenshots/active-procedure-menu");
 }
 protected void CloseMenu()
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (menu) GetGame().GetMenuManager().CloseMenu(menu);
 }
 protected void CaptureHUD() { System.MakeScreenshot("$profile:screenshots/active-procedure-closed-menu-hud"); }
 protected void Verify()
 {
  BCMM_Patient patient = SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient();
  PrintFormat("BCMM_PROGRESS_REVIEW completed=%1 ownerProcedureActive=%2 originalSupplyRetained=%3", patient.Equipment.OxygenActive, SCR_PlayerController.Cast(GetGame().GetPlayerController()).BCMM_HasProcedure(), m_Oxygen != null);
 }
}
