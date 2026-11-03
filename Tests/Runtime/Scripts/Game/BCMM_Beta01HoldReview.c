// Ready-to-use manual acceptance scene. No automatic bagging request is sent.
class BCMM_Beta01HoldReview
{
 protected static ref BCMM_Beta01HoldReview s_Review;
 protected IEntity m_Actor;
 protected IEntity m_Corpse;
 protected IEntity m_Kit;
 protected SCR_ManualCamera m_Camera;
 static void Start(IEntity actor)
 {
  s_Review = new BCMM_Beta01HoldReview(); s_Review.m_Actor = actor;
  EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = actor.GetOrigin() + "0 0 1.2";
  s_Review.m_Corpse = GetGame().SpawnEntityPrefab(Resource.Load("{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et"), GetGame().GetWorld(), spawn);
  s_Review.m_Kit = GetGame().SpawnEntityPrefab(Resource.Load("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et"), GetGame().GetWorld(), spawn);
  InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent)).TryInsertItem(s_Review.m_Kit, EStoragePurpose.PURPOSE_ANY);
  GetGame().GetCallqueue().CallLater(s_Review.Kill, 2000);
 }
 protected void Kill()
 {
  SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_TestKill();
  GetGame().GetCallqueue().CallLater(Ready, 3000);
 }
 protected void Ready()
 {
  m_Actor.SetOrigin(m_Corpse.GetOrigin() + "0 0.3 -1.2");
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  CharacterControllerComponent controller = ChimeraCharacter.Cast(m_Actor).GetCharacterController();
  controller.SetHeadingAngle(0, true);
  if (player.GetPlayerCamera()) GetGame().GetCameraManager().SetCamera(player.GetPlayerCamera());
  ScriptedCameraItem camera = controller.GetCameraHandlerComponent().GetCurrentCamera();
  vector angles = "45 0 0"; if (camera) camera.SetBaseAngles(angles);
  PrintFormat("BCMM_HOLD_REVIEW_READY canBegin=%1 kitOwned=%2 defaultSeconds=%3", BCMM_BaggingOperation.CanBegin(m_Actor,m_Corpse), InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent)).Contains(m_Kit), BCMM_BaggingOperation.Setting("baggingDurationSeconds"));
  GetGame().GetCallqueue().CallLater(Capture, 1500);
 }
 protected void Capture() { FileIO.MakeDirectory("$profile:screenshots"); System.MakeScreenshot("$profile:screenshots/hold-F-acceptance-ready"); }
}
