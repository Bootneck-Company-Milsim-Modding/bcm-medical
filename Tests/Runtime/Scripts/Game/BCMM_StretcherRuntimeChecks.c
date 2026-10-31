// Native world acceptance: actual unconscious character, collision and exact inventory.
class BCMM_StretcherRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Station;
 protected BCMM_StretcherComponent m_Stretcher;
 protected int m_SafetyRound;
 protected IEntity m_SecondCarrier;
 protected vector m_CarryStart;
 protected IEntity m_ExtraFloorA;
 protected IEntity m_ExtraFloorB;
 override protected void Prepare()
 {
  string mirror; PrintFormat("BCMM_MIRROR_FLAG server=%1 enabled=%2",Replication.IsServer(),System.GetCLIParam("bcmmStretcherSettingsMirror",mirror));
  if(System.GetCLIParam("bcmmStretcherSettingsMirror",mirror))
  {
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   m_Suite.Record(mode.BCMM_TestRoleSetting("stretcherMaximumTurnDegreesPerSecond",150) && mode.BCMM_TestRoleSetting("stretcherMaximumSpanMetres",3.8) && mode.BCMM_TestRoleSetting("stretcherHandleOffsetMetres",1.7),"Server applies nondefault newest stretcher settings");
  }
  PrintFormat("BCMM_MIRROR_SERVER turn=%1 handle=%2 span=%3",BCMM_TreatmentAccess.Setting("stretcherMaximumTurnDegreesPerSecond"),BCMM_TreatmentAccess.Setting("stretcherHandleOffsetMetres"),BCMM_TreatmentAccess.Setting("stretcherMaximumSpanMetres"));
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  // A broad flat native collision deck avoids the stepped Entry prefab's height changes.
  m_Floor = Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et", "100 2 0");
  m_Actor.SetOrigin("99.1 2.25 -0.7"); m_Target.SetOrigin("100.4 2.25 -0.7"); m_Target.SetAngles("90 0 0");
  AIControlComponent ai = AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  ai = AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent)); if (ai) ai.DeactivateAI();
  m_Medical = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory = InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_Kit = Spawn("{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et", m_Target.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit, EStoragePurpose.PURPOSE_ANY), "Stretcher casualty has an exact inventory item");
  m_Station = Spawn("{6AB4B3F3FD216011}Prefabs/Items/BCMM_FieldSurgeryStation.et", "100 2 0.8");
  m_Stretcher = BCMM_StretcherComponent.Cast(m_Station.FindComponent(BCMM_StretcherComponent));
  m_Suite.Record(m_Stretcher && m_Station.GetVObject() && m_Station.GetPhysics(), "FieldSurgeryStation has original stretcher mesh and native collision");
  m_Suite.Record(!m_Stretcher.Place(m_Actor, m_Target), "Conscious casualty cannot be forcibly placed without consent");
  m_Medical.BCMM_FieldFixtureUnconscious(true);
  string network;
  if (System.GetCLIParam("bcmmStretcherNetwork", network)) GetGame().GetCallqueue().CallLater(PrepareNetworkActor, 5000);
  else GetGame().GetCallqueue().CallLater(PlacePatient, 5000);
 }
 protected void PrepareNetworkActor()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if (!player || !player.GetControlledEntity())
  {
   if (m_OwnerWait++ < 100) { GetGame().GetCallqueue().CallLater(PrepareNetworkActor, 250); return; }
   m_Suite.Record(false, "Stretcher fixture receives a connected owning client"); Finish(); return;
  }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor); m_Actor = player.GetControlledEntity();
  m_Actor.SetOrigin("99.1 2.25 -0.7");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(1,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckNetworkPlaced, 10000);
 }
 protected void CheckNetworkPlaced()
 {
  m_Suite.Record(m_Stretcher.Patient()==m_Target, "Actual owning client RPC places casualty on stretcher");
  m_Suite.Record(m_Inventory.Contains(m_Kit), "Network placement preserves exact casualty equipment");
  m_Suite.Record(m_Target.GetOrigin()[1] > m_Station.GetOrigin()[1]+0.2, "Network casualty is supported on stretcher collision");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(2,m_Station,m_Target);
  string carry;
  if (System.GetCLIParam("bcmmStretcherCarryNetwork",carry)) GetGame().GetCallqueue().CallLater(PrepareCarriers,500);
  else GetGame().GetCallqueue().CallLater(RequestNetworkRemoval,15000);
 }
 protected void PrepareCarriers()
 {
  SCR_PlayerController second=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(2));
  if (!second || !second.GetControlledEntity()) { m_Suite.Record(false,"Second distinct connected carrier is available");FinishNetwork();return; }
  m_SecondCarrier=second.GetControlledEntity();
  m_Actor.SetAngles("0 0 0");m_SecondCarrier.SetAngles("0 0 0");
  m_Actor.SetOrigin(m_Stretcher.Handle(true)+"0 0.15 0");m_SecondCarrier.SetOrigin(m_Stretcher.Handle(false)+"0 0.15 0");
  m_Suite.Record(!m_Stretcher.ReleaseGrip(m_SecondCarrier),"Unreserved player cannot release another stretcher");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(5,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckLifted,8000);
 }
 protected void CheckLifted()
 {
  PrintFormat("BCMM_CARRY_LIFT carried=%1 front=%2 rear=%3 station=%4",m_Stretcher.IsCarried(),m_Actor.GetOrigin(),m_SecondCarrier.GetOrigin(),m_Station.GetOrigin());
  m_Suite.Record(m_Stretcher.IsCarried() && m_Stretcher.HasCarrier(m_Actor) && m_Stretcher.HasCarrier(m_SecondCarrier),"Two real owning clients reserve distinct ends and lift together");
  m_Suite.Record(!m_Stretcher.Remove(m_Actor),"Carried casualty cannot be unloaded during transport");
  m_Suite.Record(!BCMM_MedicalFacilityComponent.Supports(m_Actor,m_Target),"A lifted stretcher is not a stationary surgery facility");
  m_Suite.Record(BCMM_StretcherComponent.Stabilises(m_Target),"Supported casualty receives configured movement protection");
  m_CarryStart=m_Station.GetOrigin();
  string appliedVisuals;
  if(System.GetCLIParam("bcmmAppliedEquipmentCheck",appliedVisuals)) { SetupAppliedEquipment(); }
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(6,m_Station,m_Target);
  string patientOwner;
  if(System.GetCLIParam("bcmmStretcherPatientOwner",patientOwner))GetGame().GetCallqueue().CallLater(AssignPatientOwner,500);
  string review;int hold=15000;if(System.GetCLIParam("bcmmStretcherReview",review)) { hold=60000; }
  GetGame().GetCallqueue().CallLater(StartCarryWalk,hold);
 }
 protected void AssignPatientOwner()
 {
  SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(3));
  if(!owner || !owner.GetControlledEntity())
  {
   if(m_OwnerWait++<80){GetGame().GetCallqueue().CallLater(AssignPatientOwner,250);return;}
   m_Suite.Record(false,"Late client is ready to own the stretcher casualty");return;
  }
  IEntity previous=owner.GetControlledEntity();
  ChimeraCharacter.Cast(m_Target).GetRplComponent().GiveExt(owner.GetRplIdentity(),false);
  m_Suite.Record(owner.SetControlledEntity(m_Target),"Late client possesses actual unconscious stretcher casualty");
  if(previous && previous!=m_Target)SCR_EntityHelper.DeleteEntityAndChildren(previous);
  Print("BCMM_CARRY_PATIENT_OWNER_ASSIGNED");
 }
 protected void SetupAppliedEquipment()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  patient.Equipment.PadsAttached=true;patient.Equipment.VascularAccess=true;patient.Equipment.VascularAccessRegion=BCMM_BodyRegion.RIGHT_ARM;
  patient.LeftChest.OpenWound=true;patient.RightChest.OpenWound=true;
  patient.LeftChest.ApplySeal();patient.RightChest.ApplySeal();
  patient.LeftLeg.Broken=true;patient.LeftLeg.Splinted=true;patient.RightLeg.Broken=true;patient.RightLeg.Splinted=true;
  m_Medical.BCMM_SetPulseMonitor(true);BCMM_VisualStatusBridge.Publish(m_Medical);
  m_Suite.Record(m_Medical.BCMM_AppliedEquipment()==63 && m_Medical.BCMM_IVRegion()==BCMM_BodyRegion.RIGHT_ARM,"Authority publishes treated equipment and exact IV limb");
 }
 protected void ClearAppliedEquipment()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  patient.Equipment.PadsAttached=false;patient.Equipment.VascularAccess=false;
  patient.LeftChest.Sealed=false;patient.RightChest.Sealed=false;
  patient.LeftLeg.Splinted=false;patient.RightLeg.Splinted=false;
  m_Medical.BCMM_SetPulseMonitor(false);BCMM_VisualStatusBridge.Publish(m_Medical);
  m_Suite.Record(m_Medical.BCMM_AppliedEquipment()==0,"Authority clears all applied equipment flags");
 }
 protected void StartCarryWalk()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(7,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckCarryWalk,4000);
 }
 protected void CheckCarryWalk()
 {
  string appliedVisuals;
  if(System.GetCLIParam("bcmmAppliedEquipmentCheck",appliedVisuals)) { m_Suite.Record(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("equipmentVisualChestDevicesEnabled",0),"Live GM chest visual toggle changes during transport"); }
  PrintFormat("BCMM_CARRY_MOVED delta=%1 carried=%2 front=%3 rear=%4",vector.Distance(m_CarryStart,m_Station.GetOrigin()),m_Stretcher.IsCarried(),m_Actor.GetOrigin(),m_SecondCarrier.GetOrigin());
  m_Suite.Record(m_Stretcher.IsCarried() && vector.Distance(m_CarryStart,m_Station.GetOrigin())>0.25,"Two owners move the supported stretcher through native character locomotion");
  m_Suite.Record(m_Stretcher.Patient()==m_Target && m_Inventory.Contains(m_Kit),"Moving casualty retains occupancy and exact equipment");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(8,m_Station,m_Target);
  string review;int hold=3000;if(System.GetCLIParam("bcmmStretcherReview",review)) { hold=60000; }
  GetGame().GetCallqueue().CallLater(RequestCarryRelease,hold);
 }
 protected void RequestCarryRelease()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(9,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckCarryRelease,4000);
 }
 protected void CheckCarryRelease()
 {
  string appliedVisuals;
  if(System.GetCLIParam("bcmmAppliedEquipmentCheck",appliedVisuals)) { ClearAppliedEquipment(); }
  PrintFormat("BCMM_CARRY_RELEASE_STATE patient=%1 expected=%2 pos=%3 station=%4 unconscious=%5 kit=%6",m_Stretcher.Patient(),m_Target,m_Target.GetOrigin(),m_Station.GetOrigin(),ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious(),m_Inventory.Contains(m_Kit));
  m_Suite.Record(!m_Stretcher.IsCarried() && !m_Stretcher.HasCarriers(),"One actual owner releases both grips and safely sets down");
  m_Suite.Record(m_Stretcher.Patient()==m_Target && m_Inventory.Contains(m_Kit) && vector.Distance(m_Target.GetOrigin(),m_Station.GetOrigin()+"0 0.43 0")<0.25,"Set-down keeps casualty and exact inventory on the stretcher");
  m_Suite.Record(BCMM_MedicalFacilityComponent.Supports(m_Actor,m_Target),"Set-down restores stationary surgical capability");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(10,m_Station,m_Target);
  string safety;
  if(System.GetCLIParam("bcmmStretcherCarrySafety",safety))GetGame().GetCallqueue().CallLater(SafetyPair,3000);
  else GetGame().GetCallqueue().CallLater(FinishNetwork,5000);
 }
 protected void SafetyPair()
 {
  int phase=11;if(m_SafetyRound>0)phase=15;
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(phase,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckSafetyLift,6000);
 }
 protected void CheckSafetyLift()
 {
  m_Suite.Record(m_Stretcher.IsCarried(),"Both owners can rejoin after release without leaked reservations");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(m_SafetyRound==0)
  {
   mode.BCMM_TestRoleSetting("stretcherCarryEnabled",0);
   GetGame().GetCallqueue().CallLater(CheckGMStop,1000);
  }
  else
  {
   mode.BCMM_PublishStretcher(16,m_Station,m_Target);
   GetGame().GetPlayerManager().KickPlayer(2,0,0);
   GetGame().GetCallqueue().CallLater(CheckDisconnectedCarrier,4000);
  }
 }
 protected void CheckGMStop()
 {
  m_Suite.Record(!m_Stretcher.IsCarried() && !m_Stretcher.HasCarriers(),"Live GM disable safely sets down and releases both players");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  mode.BCMM_TestRoleSetting("stretcherStabilityEnabled",0);
  m_Suite.Record(!BCMM_StretcherComponent.Stabilises(m_Target),"Live GM stability toggle removes the medical protection");
  mode.BCMM_TestRoleSetting("stretcherStabilityEnabled",1);mode.BCMM_TestRoleSetting("stretcherCarryEnabled",1);
  m_SafetyRound=1;mode.BCMM_PublishStretcher(14,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(SafetyPair,3000);
 }
 protected void CheckDisconnectedCarrier()
 {
  m_Suite.Record(!GetGame().GetPlayerManager().GetPlayerControlledEntity(2),"Second carrier actually disconnected from the server");
  m_Suite.Record(!m_Stretcher.IsCarried() && !m_Stretcher.HasCarriers(),"Carrier disconnect safely sets down and clears both reservations");
  m_Suite.Record(m_Stretcher.Patient()==m_Target && m_Inventory.Contains(m_Kit),"Disconnect preserves the casualty and original inventory");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(18,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(RequestOwnedUnload,2500);
 }
 protected void RequestOwnedUnload()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(19,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckOwnedUnload,5000);
 }
 protected void CheckOwnedUnload()
 {
  PrintFormat("BCMM_UNLOAD_RESULT occupied=%1 distance=%2 patient=%3 station=%4",m_Stretcher.Patient()!=null,vector.Distance(m_Target.GetOrigin(),m_Station.GetOrigin()),m_Target.GetOrigin(),m_Station.GetOrigin());
  m_Suite.Record(!m_Stretcher.Patient() && vector.Distance(m_Target.GetOrigin(),m_Station.GetOrigin())>0.8 && vector.Distance(m_Target.GetOrigin(),m_Station.GetOrigin())<1.6,"Actual owner RPC unloads player casualty to a safe adjacent supported position");
  m_Suite.Record(m_Inventory.Contains(m_Kit),"Unloading player casualty preserves exact original inventory");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(20,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(FinishNetwork,3000);
 }
 protected void RequestNetworkRemoval()
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(3,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(CheckNetworkRemoved, 10000);
 }
 protected void CheckNetworkRemoved()
 {
  m_Suite.Record(!m_Stretcher.Patient() && m_Inventory.Contains(m_Kit), "Actual owning client RPC removes casualty without losing gear");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishStretcher(4,m_Station,m_Target);
  GetGame().GetCallqueue().CallLater(FinishNetwork, 5000);
 }
 protected void FinishNetwork()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Station); Finish();
 }
 protected void PlacePatient()
 {
  PrintFormat("BCMM_STRETCHER_BEFORE actor=%1 target=%2 station=%3 reach=%4 eligible=%5", m_Actor.GetOrigin(), m_Target.GetOrigin(), m_Station.GetOrigin(), BCMM_TreatmentAccess.CanReach(m_Actor,m_Target), m_Stretcher.CanPlace(m_Actor,m_Target));
  PrintFormat("BCMM_STRETCHER_STATE canUse=%1 falling=%2 unconscious=%3 parent=%4 inVehicle=%5 life=%6", m_Stretcher.ActorCanUse(m_Actor), ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsFalling(), ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious(), m_Target.GetParent(), ChimeraCharacter.Cast(m_Target).IsInVehicle(), ChimeraCharacter.Cast(m_Target).GetCharacterController().GetLifeState());
  vector low,high;m_Station.GetWorldBounds(low,high);
  ChimeraCharacter actorCharacter = ChimeraCharacter.Cast(m_Actor);
  TraceParam sight = new TraceParam();sight.Start=actorCharacter.EyePosition();sight.End=m_Station.GetOrigin()+"0 0.5 0";
  sight.Flags=TraceFlags.WORLD|TraceFlags.ENTS;sight.LayerMask=EPhysicsLayerPresets.Projectile;
  array<IEntity> exclude={m_Actor,m_Station};sight.ExcludeArray=exclude;
  float sightHit=m_Actor.GetWorld().TraceMove(sight);
  PrintFormat("BCMM_STRETCHER_HIT entity=%1",sight.TraceEnt);
  PrintFormat("BCMM_STRETCHER_SIGHT hit=%1 eye=%2 low=%3 high=%4 actorLife=%5 actorUncon=%6",sightHit,sight.Start,low,high,actorCharacter.GetCharacterController().GetLifeState(),actorCharacter.GetCharacterController().IsUnconscious());
  m_Suite.Record(m_Stretcher.FindCasualty(m_Actor) == m_Target, "Look-at stretcher discovery finds the nearby unconscious casualty");
  m_Suite.Record(m_Stretcher.Place(m_Actor, m_Target), "Authority places actual casualty on stretcher");
  m_Suite.Record(m_Stretcher.Patient() == m_Target && !m_Stretcher.Place(m_Actor, m_Target), "Occupied stretcher rejects concurrent duplicate placement");
  m_Suite.Record(m_Inventory.Contains(m_Kit), "Stretcher placement preserves original casualty inventory");
  m_Suite.Record(BCMM_MedicalFacilityComponent.Supports(m_Actor,m_Target), "Stretcher retains medical-facility capability");
  GetGame().GetCallqueue().CallLater(CheckSettled, 3500);
 }
 protected void CheckSettled()
 {
  PrintFormat("BCMM_STRETCHER_SETTLED target=%1 station=%2", m_Target.GetOrigin(), m_Station.GetOrigin());
  m_Suite.Record(m_Stretcher.Patient() == m_Target && m_Target.GetOrigin()[1] > m_Station.GetOrigin()[1] + 0.2, "Actual unconscious patient remains supported above the canvas collision");
  m_Start = m_Actor.GetOrigin(); m_Actor.SetOrigin(m_Start + "10 0 0");
  m_Suite.Record(!m_Stretcher.Remove(m_Actor), "Remote actor cannot remove stretcher casualty");
  m_Actor.SetOrigin(m_Start);
  GetGame().GetCallqueue().CallLater(RemovePatient, 800);
 }
 protected void RemovePatient()
 {
  m_Suite.Record(m_Stretcher.Remove(m_Actor) && !m_Stretcher.Patient(), "Nearby actor removes casualty to a supported clear side");
  m_Suite.Record(m_Inventory.Contains(m_Kit), "Removing casualty preserves exact inventory item");
  GetGame().GetCallqueue().CallLater(CheckDeletion, 2000);
 }
 protected void CheckDeletion()
 {
  m_Suite.Record(m_Stretcher.Place(m_Actor,m_Target), "Released stretcher can be reused");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Station);
  m_Suite.Record(m_Target && m_Inventory.Contains(m_Kit), "Deleting stationary stretcher does not delete casualty or gear");
  if (m_ExtraFloorA) SCR_EntityHelper.DeleteEntityAndChildren(m_ExtraFloorA);
  if (m_ExtraFloorB) SCR_EntityHelper.DeleteEntityAndChildren(m_ExtraFloorB);
  Finish();
 }
}
