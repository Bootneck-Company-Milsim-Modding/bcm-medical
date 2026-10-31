// Draft diagnostic only: owner/proxy/JIP observations after a native saved-world load.
modded class SCR_BaseGameMode
{
 [RplProp()] int m_BCMMWorldNetPhase;
 [RplProp()] RplId m_BCMMWorldNetPatient=RplId.Invalid();
 [RplProp()] RplId m_BCMMWorldNetOxygen=RplId.Invalid();
 [RplProp()] RplId m_BCMMWorldNetBag=RplId.Invalid();
 [RplProp()] RplId m_BCMMWorldNetCargoBag=RplId.Invalid();
 [RplProp()] RplId m_BCMMWorldNetVehicle=RplId.Invalid();
 void BCMM_PublishWorldNet(int phase,IEntity patient,IEntity oxygen,IEntity bag,IEntity cargoBag,IEntity vehicle)
 {
  m_BCMMWorldNetPatient=BCMM_BodyBagComponent.Id(patient);m_BCMMWorldNetOxygen=BCMM_BodyBagComponent.Id(oxygen);m_BCMMWorldNetBag=BCMM_BodyBagComponent.Id(bag);m_BCMMWorldNetCargoBag=BCMM_BodyBagComponent.Id(cargoBag);m_BCMMWorldNetVehicle=BCMM_BodyBagComponent.Id(vehicle);m_BCMMWorldNetPhase=phase;Replication.BumpMe();PrintFormat("BCMM_WORLD_NETWORK_PHASE %1",phase);
 }
}
class BCMM_WorldPersistenceNetworkChecks : BCMM_FinitePersistenceRuntimeChecks
{
 protected IEntity m_NetPatient;
 protected IEntity m_NetOxygen;
 protected IEntity m_NetWorldBag;
 protected IEntity m_NetCargoBag;
 protected IEntity m_NetVehicle;
 protected int m_NetOwnerWait;
 protected IEntity m_NetCargoFloor;
 override void Start(BCMM_DomainChecks suite)
 {
  // Plain diagnostic world has no terrain outside the small character floor.
  m_NetCargoFloor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","110 0 0");
  super.Start(suite);
 }
 override protected void Prepare()
 {
  m_Persistence=PersistenceSystem.GetInstance();
  if(!m_Persistence || !m_Persistence.WasDataLoaded()){m_Suite.Record(false,"Saved-world network fixture actually loaded native persistence");Finish();return;}
  if(!m_FiniteManifest.LoadFromFile("$profile:BCMM_FinitePersistence.json")){m_Suite.Record(false,"Saved-world network fixture loads original manifest");Finish();return;}
  SCR_PlayerController owner=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!owner || !owner.GetControlledEntity())
  {
   if(m_NetOwnerWait++<120){GetGame().GetCallqueue().CallLater(Prepare,250);return;}
   m_Suite.Record(false,"Saved-world network fixture receives actual native owner");Finish();return;
  }
  m_NetPatient=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalIds[0]));m_NetOxygen=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalOxygenId));m_NetWorldBag=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.WorldIds[0]));m_NetVehicle=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.CargoIds[0]));m_NetCargoBag=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.CargoIds[1]));
  m_Suite.Record(m_NetPatient && m_NetOxygen && m_NetWorldBag && m_NetVehicle && m_NetCargoBag,"Network fixture resolves original saved clinical and recovery UUIDs");
  if(!m_NetPatient || !m_NetOxygen || !m_NetWorldBag || !m_NetVehicle || !m_NetCargoBag){Finish();return;}
  // Local restart already verifies saved transforms; move only this observation fixture into native cargo access range.
  vector viewing=m_NetVehicle.GetOrigin()+Vector(-2,1.2,1);m_NetPatient.SetOrigin(viewing);
  RplComponent.Cast(m_NetPatient.FindComponent(RplComponent)).GiveExt(owner.GetRplIdentity(),false);
  IEntity previous=owner.GetControlledEntity();m_Suite.Record(owner.SetControlledEntity(m_NetPatient),"Actual owner takes native possession of the original restored patient");if(previous && previous!=m_NetPatient)SCR_EntityHelper.DeleteEntityAndChildren(previous);
  AIControlComponent ai=AIControlComponent.Cast(m_NetPatient.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_GetPatient().MarkTerminal();
  PublishNetwork(1);GetGame().GetCallqueue().CallLater(PublishLatePhase,18000);
 }
 protected void PublishNetwork(int phase)
 {
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishWorldNet(phase,m_NetPatient,m_NetOxygen,m_NetWorldBag,m_NetCargoBag,m_NetVehicle);
 }
 protected void PublishLatePhase(){PublishNetwork(2);GetGame().GetCallqueue().CallLater(CheckRestoredExpiry,8000);GetGame().GetCallqueue().CallLater(Finish,22000);}
 protected void CheckRestoredExpiry()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Target).GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD,"Terminal physiology also reaches native death without re-entering consciousness callbacks");
  IEntity expired=IEntity.Cast(m_Persistence.FindById(m_FiniteManifest.ClinicalIds[1]));
  SCR_CharacterDamageManagerComponent medical;if(expired)medical=SCR_CharacterDamageManagerComponent.Cast(expired.FindComponent(SCR_CharacterDamageManagerComponent));
  ChimeraCharacter character=ChimeraCharacter.Cast(expired);
  m_Suite.Record(character && character.GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD && medical && medical.BCMM_GetPatient().Terminal,"Restored depletion grace actually expires into one native death without recursive Kill callbacks");
  if(medical){medical.UpdateConsciousness();medical.UpdateConsciousness();}
  m_Suite.Record(character && character.GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD,"Repeated consciousness refresh cannot re-enter native death or resurrect a depleted patient");
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMWorldNetObserved;
 protected int m_BCMMWorldNetOpened;
 protected float m_BCMMWorldNetNext;
 protected float m_BCMMWorldNetTrace;
 protected bool m_BCMMWorldNetPositioned;
 protected IEntity m_BCMMWorldNetFloor;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string enabled;
  if(Replication.IsServer() || !System.GetCLIParam("bcmmWorldPersistenceObserver",enabled) || world.GetWorldTime()<m_BCMMWorldNetNext)return;
  m_BCMMWorldNetNext=world.GetWorldTime()+250;
  if(!m_BCMMWorldNetFloor)
  {
   EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;spawn.Transform[3]="100 0 0";
   m_BCMMWorldNetFloor=GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et"),world,spawn);
  }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());
  if(!mode || !player || !player.GetControlledEntity() || mode.m_BCMMWorldNetPhase<=m_BCMMWorldNetObserved)return;
  IEntity patient=BCMM_BodyBagComponent.Resolve(mode.m_BCMMWorldNetPatient);IEntity vehicle=BCMM_BodyBagComponent.Resolve(mode.m_BCMMWorldNetVehicle);IEntity bag=BCMM_BodyBagComponent.Resolve(mode.m_BCMMWorldNetBag);IEntity cargoBag=BCMM_BodyBagComponent.Resolve(mode.m_BCMMWorldNetCargoBag);
  if(world.GetWorldTime()>=m_BCMMWorldNetTrace){m_BCMMWorldNetTrace=world.GetWorldTime()+5000;PrintFormat("BCMM_WORLD_NETWORK_WAIT phase=%1 player=%2 patient=%3 vehicle=%4 bag=%5 cargoBag=%6",mode.m_BCMMWorldNetPhase,player.GetPlayerId(),patient,vehicle,bag,cargoBag);}
  if(!patient || !vehicle || !bag)return;
  if(player.GetPlayerId()==1 && player.GetControlledEntity()==patient && !m_BCMMWorldNetPositioned)
  {
   RplComponent ownership=RplComponent.Cast(patient.FindComponent(RplComponent));
   if(!ownership || !ownership.IsOwner())return;
   vector transform[4];patient.GetWorldTransform(transform);transform[3]=vehicle.GetOrigin()+Vector(-2,1.2,1);ChimeraCharacter.Cast(patient).Teleport(transform);m_BCMMWorldNetPositioned=true;
   Print("BCMM_WORLD_NETWORK_POSITIONED actualOwner=1");return;
  }
  SCR_InventoryStorageManagerComponent personal=SCR_InventoryStorageManagerComponent.Cast(player.GetControlledEntity().FindComponent(SCR_InventoryStorageManagerComponent));
  if(!personal)return;
  if(m_BCMMWorldNetOpened!=mode.m_BCMMWorldNetPhase){personal.SetStorageToOpen(vehicle);personal.OpenInventory();m_BCMMWorldNetOpened=mode.m_BCMMWorldNetPhase;}
  if(!cargoBag)return;
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));BCMM_BodyBagComponent worldState=BCMM_BodyBagComponent.Cast(bag.FindComponent(BCMM_BodyBagComponent));BCMM_BodyBagComponent cargoState=BCMM_BodyBagComponent.Cast(cargoBag.FindComponent(BCMM_BodyBagComponent));
  if(!medical || medical.BCMM_AppliedEquipment()!=107 || medical.BCMM_OxygenSaturation()<0.5 || medical.BCMM_Pulse()<=0 || !worldState || worldState.DeceasedName()!="BCM saved recovery casualty" || !cargoState || cargoState.DeceasedName()!="BCM saved vehicle casualty" || cargoState.State()!=BCMM_BagCargoState.STORED || cargoState.VehicleId()!=BCMM_BodyBagComponent.Id(vehicle))return;
  if(player.GetPlayerId()==1)
  {
   IEntity oxygen=BCMM_BodyBagComponent.Resolve(mode.m_BCMMWorldNetOxygen);BCMM_EquipmentItemComponent source;if(oxygen)source=BCMM_EquipmentItemComponent.Cast(oxygen.FindComponent(BCMM_EquipmentItemComponent));
   RplComponent patientRpl=RplComponent.Cast(patient.FindComponent(RplComponent));
   if(player.GetControlledEntity()!=patient || !patientRpl.IsOwner() || !source || source.OxygenLitres()<=0 || source.OxygenLitres()>=300)return;
   PrintFormat("BCMM_WORLD_NETWORK_OWNER phase=%1 gas=%2",mode.m_BCMMWorldNetPhase,source.OxygenLitres());
  }

  InventoryStorageManagerComponent cargo=InventoryStorageManagerComponent.Cast(vehicle.FindComponent(InventoryStorageManagerComponent));
  if(!personal || !cargo)return;
  if(m_BCMMWorldNetOpened!=mode.m_BCMMWorldNetPhase){personal.SetStorageToOpen(vehicle);personal.OpenInventory();m_BCMMWorldNetOpened=mode.m_BCMMWorldNetPhase;}
  int bags=cargo.GetDepositItemCountByResource(player.GetControlledEntity(),"{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et");
  int devices=cargo.GetDepositItemCountByResource(player.GetControlledEntity(),"{6AB4B3F3FD216025}Prefabs/Items/BCMM_AED.et");
  if(bags!=1 || devices!=1)return;
  m_BCMMWorldNetObserved=mode.m_BCMMWorldNetPhase;personal.CloseInventory();PrintFormat("BCMM_WORLD_NETWORK_OBSERVED phase=%1 player=%2 equipment=%3",m_BCMMWorldNetObserved,player.GetPlayerId(),medical.BCMM_AppliedEquipment());
 }
}







