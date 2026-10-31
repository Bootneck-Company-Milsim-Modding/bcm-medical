// Staged native burden generation, typed policy and three-way clinical persistence.
modded class SCR_BaseGameMode
{
 bool BCMM_TestPersistBurdenPolicy()
 {
  return m_BCMMSettingsStore && m_BCMMSettingsStore.Save(m_BCMMSettings);
 }
}
class BCMM_FluidBurdenManifest : JsonApiStruct
{
 string SaveId;
 string PatientId;
 string WholeBagId;
 string BlockedBagId;
 float BurdenFraction;
 float TransientWaterFraction;
 float Volume;
 float Cells;
 float WholeRemaining;
 void BCMM_FluidBurdenManifest()
 {
  RegV("SaveId");
  RegV("PatientId");
  RegV("WholeBagId");
  RegV("BlockedBagId");
  RegV("BurdenFraction");
  RegV("TransientWaterFraction");
  RegV("Volume");
  RegV("Cells");
  RegV("WholeRemaining");
 }
}
class BCMM_FluidBurdenRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_BaseGameMode m_Mode;
 protected InventoryStorageManagerComponent m_PatientInventory;
 protected IEntity m_Saline;
 protected IEntity m_Whole;
 protected IEntity m_Blocked;
 protected IEntity m_Oxygen;
 protected float m_UnsupportedOxygen;
 protected ref BCMM_FluidBurdenManifest m_Save=new BCMM_FluidBurdenManifest();
 protected bool Setting(string key,float value)
 {
  return m_Mode.BCMM_TestRoleSetting(key,value);
 }
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.25 0");
  m_Target.SetOrigin("101 2.25 0");
  array<IEntity> bodies=
  {
   m_Actor,m_Target
  };
  foreach(IEntity body:bodies)
  {
   AIControlComponent ai=AIControlComponent.Cast(body.FindComponent(AIControlComponent));
   if(ai)
   {
    ai.DeactivateAI();
   }
  }
  m_Mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_PatientInventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  m_Suite.Record(Setting("fluidOverloadEnabled",1) && Setting("fluidOverloadThresholdFraction",0.01) && Setting("fluidOverloadSevereFraction",0.02) && Setting("fluidOverloadClearanceSeconds",60) && Setting("fluidRedistributionSeconds",30) && Setting("fluidInfusionSeconds",3) && Setting("automaticUnconsciousAirwayObstruction",0) && Setting("oxygenChangePerSecond",0.05),"Native overload fixture uses valid saved gameplay controls and isolates the airway from unrelated automatic obstruction");
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  IEntity actorPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  IEntity patientPack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
  m_Saline=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Target.GetOrigin());
  m_Whole=Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_Target.GetOrigin());
  m_Blocked=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Actor.GetOrigin());
  m_Oxygen=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());
  m_Suite.Record(actorPack && m_Inventory.TryInsertItem(actorPack,EStoragePurpose.PURPOSE_ANY) && patientPack && m_PatientInventory.TryInsertItem(patientPack,EStoragePurpose.PURPOSE_ANY),"Native overload actors receive real inventory capacity before source insertion");
  GetGame().GetCallqueue().CallLater(StartActualWaterLoad,1500);
 }
 protected void StartActualWaterLoad()
 {
  m_Suite.Record(m_PatientInventory.TryInsertItem(m_Saline,EStoragePurpose.PURPOSE_ANY) && m_PatientInventory.TryInsertItem(m_Whole,EStoragePurpose.PURPOSE_ANY) && m_Inventory.TryInsertItem(m_Blocked,EStoragePurpose.PURPOSE_ANY) && m_Inventory.TryInsertItem(m_Oxygen,EStoragePurpose.PURPOSE_ANY),"Actual finite sources enter the now-registered original native inventories");
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Saline,BCMM_BodyRegion.LEFT_ARM),"Actual finite saline starts real native progressive delivery before burden develops");
  GetGame().GetCallqueue().CallLater(CheckGeneratedBurden,15000);
 }
 protected void CheckGeneratedBurden()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Saline.FindComponent(BCMM_EquipmentItemComponent));
  PrintFormat("BCMM_BURDEN_GENERATED remaining=%1 load=%2 oxygen=%3 rr=%4 transient=%5 volume=%6 cells=%7 active=%8 enabled=%9 redistribution=%10",source.FluidRemainingMl(),patient.BCMM_FluidBurdenFraction(),patient.OxygenSaturation,patient.RespiratoryRate,patient.Fluids.TransientWater,m_Medical.GetBloodHitZone().GetHealthScaled(),patient.Fluids.RedCellReserve,m_Medical.BCMM_FluidLineActive(),m_Mode.BCMM_ReadSetting("fluidOverloadEnabled"),m_Mode.BCMM_ReadSetting("fluidRedistributionSeconds"));
  m_Suite.Record(source.FluidRemainingMl()<=0.05 && m_PatientInventory.Contains(m_Saline) && patient.BCMM_FluidBurdenFraction()>0.01 && patient.OxygenSaturation<0.9 && patient.RespiratoryRate>16 && !patient.LeftChest.Tension && !patient.RightChest.Tension && patient.LeftChest.PleuralBloodMl==0,"A genuinely delivered finite saline bag produces native redistributed burden and linked breathing changes without inventing a chest injury");
  m_Actor.SetOrigin(m_Target.GetOrigin()+"-1 0.1 0");
  m_UnsupportedOxygen=patient.OxygenSaturation;
  float volume=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(m_Medical.BCMM_WaterLoadingWithheld() && m_Medical.BCMM_DeliverFluidVolumeMl(50,BCMM_FluidKind.SALINE)==0 && m_Medical.GetBloodHitZone().GetHealthScaled()==volume,"Native adapter and replicated water gate withhold further water without adding reserve");
  m_Suite.Record(BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.SALINE)==m_Blocked && !BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.INFUSE_SALINE,BCMM_BodyRegion.LEFT_ARM) && BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500,"Actual held-infusion route rejects the real available saline source without debiting it");
  BCMM_OxygenSession.Start(m_Actor,m_Target,m_Oxygen);
  BaseInventoryStorageComponent storage=m_PatientInventory.FindStorageForItem(m_Blocked,EStoragePurpose.PURPOSE_ANY);
  m_Suite.Record(storage && m_Inventory.TryMoveItemToStorage(m_Blocked,storage),"Original blocked saline moves through native inventory for an independent-line policy check");
  GetGame().GetCallqueue().CallLater(CheckBlockedLine,700);
 }
 protected void CheckBlockedLine()
 {
  m_Suite.Record(m_PatientInventory.Contains(m_Blocked) && !BCMM_FluidLineSession.Start(m_Target,m_Blocked,BCMM_BodyRegion.LEFT_ARM) && BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500,"Independent line rejects the same original water-loading source and retains all finite contents");
  m_Suite.Record(Setting("fluidInfusionSeconds",60) && BCMM_FluidLineSession.Start(m_Target,m_Whole,BCMM_BodyRegion.LEFT_ARM),"Real original whole blood may still flow under ordinary native access/pressure/volume guards");
  GetGame().GetCallqueue().CallLater(CheckWholeProgress,5000);
 }
 protected void CheckWholeProgress()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Whole.FindComponent(BCMM_EquipmentItemComponent));
  float used=500-source.FluidRemainingMl();
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && used>20 && used<80 && Math.AbsFloat((m_Medical.BCMM_GetPatient().Fluids.RedCellReserve-0.7)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl")-used)<0.2,"Unblocked native whole-blood line adds exactly the cells debited from its original finite source");
  GetGame().GetCallqueue().CallLater(SaveSupportedBurden,10000);
 }
 protected void SaveSupportedBurden()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  m_Suite.Record(patient.OxygenSaturation>m_UnsupportedOxygen+0.03 && patient.BCMM_FluidBurdenFraction()>0.01 && patient.Equipment.OxygenActive && m_Medical.BCMM_FluidLineActive(),"Actual retained oxygen improves breathing while systemic burden and original whole-blood line remain active");
  m_Suite.Record(m_Mode.BCMM_TestPersistBurdenPolicy(),"Real settings store persists schema33 overload controls for restart");
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(persistence && persistence.IsTracked(m_Target) && persistence.IsTracked(m_Whole) && persistence.IsTracked(m_Blocked),"Actual original patient and finite sources are tracked by native persistence");
  if(!persistence)
  {
   Finish();
   return;
  }
  m_Save.PatientId=persistence.GetId(m_Target);
  m_Save.WholeBagId=persistence.GetId(m_Whole);
  m_Save.BlockedBagId=persistence.GetId(m_Blocked);
  m_Save.BurdenFraction=patient.BCMM_FluidBurdenFraction();
  m_Save.TransientWaterFraction=patient.Fluids.TransientWater;
  m_Save.Volume=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Save.Cells=patient.Fluids.RedCellReserve;
  m_Save.WholeRemaining=BCMM_EquipmentItemComponent.Cast(m_Whole.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl();
  SaveGameManager manager=GetGame().GetSaveGameManager();
  manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);
  manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM supported fluid burden",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnBurdenSaved));
 }
 protected void OnBurdenSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Native world save commits burden alongside active oxygen and an original finite line");
  if(!success)
  {
   Finish();
   return;
  }
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnBurdenSaves));
 }
 protected void OnBurdenSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;
  if(success && saves)
  {
   foreach(SaveGame save:saves)
   {
    if(save.GetSavePointName()=="BCMM supported fluid burden" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))
    {
     selected=save;
    }
   }
  }
  m_Suite.Record(selected!=null,"Actual burden world save is discoverable");
  if(selected)
  {
   m_Save.SaveId=selected.GetId();
   m_Suite.Record(m_Save.PackToFile("$profile:BCMM_FluidBurden.json"),"Restart manifest captures exact original identities, burden, water and remaining finite contents");
  }
  Finish();
 }
}
class BCMM_FluidBurdenRestartChecks : BCMM_SplintRuntimeChecks
{
 protected ref BCMM_FluidBurdenManifest m_Save=new BCMM_FluidBurdenManifest();
 protected IEntity m_Blocked;
 protected float m_InitialLoad;
 override protected void Prepare()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();
  m_Suite.Record(m_Save.LoadFromFile("$profile:BCMM_FluidBurden.json") && persistence && persistence.WasDataLoaded(),"Separate native process loads the actual burden/oxygen/line world save");
  if(!persistence || !persistence.WasDataLoaded())
  {
   Finish();
   return;
  }
  m_Target=IEntity.Cast(persistence.FindById(m_Save.PatientId));
  m_Kit=IEntity.Cast(persistence.FindById(m_Save.WholeBagId));
  m_Blocked=IEntity.Cast(persistence.FindById(m_Save.BlockedBagId));
  if(m_Target)
  {
   m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  m_Suite.Record(m_Medical && m_Kit && m_Blocked && m_Medical.BCMM_FluidLineActive() && m_Medical.BCMM_GetPatient().Equipment.OxygenActive,"Original patient, original partial line, oxygen binding and separate burden coexist after native restart");
  if(!m_Medical || !m_Kit || !m_Blocked)
  {
   Finish();
   return;
  }
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  float runtime=GetGame().GetWorld().GetWorldTime()*0.001;
  float minimum=m_Save.BurdenFraction*Math.Pow(2.718281828,-runtime/60);
  float maximum=m_Save.BurdenFraction+m_Save.TransientWaterFraction*Math.Clamp(runtime/30,0,1);
  m_Suite.Record(patient.BCMM_FluidBurdenFraction()>=minimum-0.0001 && patient.BCMM_FluidBurdenFraction()<=maximum+0.0001,"Saved nonzero burden is retained and changes only within current-process input/clearance bounds, without offline clearance");
  m_Suite.Record(BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500 && BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()<=m_Save.WholeRemaining,"Restart preserves original blocked saline and never refills the active whole-blood bag");
  m_InitialLoad=patient.BCMM_FluidBurdenFraction();
  GetGame().GetCallqueue().CallLater(CheckNativeClearance,60000);
 }
 protected void CheckNativeClearance()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  BCMM_EquipmentItemComponent whole=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  float used=m_Save.WholeRemaining-whole.FluidRemainingMl();
  float cells=(patient.Fluids.RedCellReserve-m_Save.Cells)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  PrintFormat("BCMM_BURDEN_RESTART remaining=%1 used=%2 cells=%3 blocked=%4 load=%5 initial=%6 volume=%7 water=%8 duration=%9",whole.FluidRemainingMl(),used,cells,BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl(),patient.BCMM_FluidBurdenFraction(),m_InitialLoad,m_Medical.GetBloodHitZone().GetHealthScaled(),patient.Fluids.TransientWater,BCMM_TreatmentAccess.Setting("fluidInfusionSeconds"));
  m_Suite.Record(!m_Medical.IsDestroyed() && patient.BCMM_FluidBurdenFraction()<m_InitialLoad && patient.BCMM_FluidBurdenFraction()>0,"Real native perfusion clears stored burden gradually while supported patient remains alive");
  m_Suite.Record(whole.FluidRemainingMl()<=0.05 && Math.AbsFloat(cells-used)<0.2 && BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500,"Continued native recovery adds no phantom cells or replacement supplies beyond the exact original whole-blood contents");
  float delivered=(m_Medical.GetBloodHitZone().GetHealthScaled()-m_Save.Volume)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  float redistributed=(m_Save.TransientWaterFraction-patient.Fluids.TransientWater)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  m_Suite.Record(Math.AbsFloat(delivered+redistributed-used)<0.5,"Native reserve changes account only for original delivered blood and existing water redistribution, not burden clearance");
  m_Medical.FullHeal();
  GetGame().GetCallqueue().CallLater(CheckBurdenReset,700);
 }
 protected void CheckBurdenReset()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().BCMM_FluidBurdenFraction()==0 && !m_Medical.BCMM_WaterLoadingWithheld() && !m_Medical.BCMM_FluidLineActive() && BCMM_EquipmentItemComponent.Cast(m_Blocked.FindComponent(BCMM_EquipmentItemComponent)).FluidRemainingMl()==500,"Native FullHeal resets burden/gates and stops the old line without refilling any finite source");
  Finish();
 }
}
