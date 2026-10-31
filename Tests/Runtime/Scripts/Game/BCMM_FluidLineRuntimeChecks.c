// Actual native inventory, connection, finite flow and cancellation acceptance.
class BCMM_FluidLineRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected InventoryStorageManagerComponent m_PatientInventory;
 protected SCR_BaseGameMode m_Mode;
 protected ref array<IEntity> m_Fillers=
 {
 };
 protected float m_BeforeVolume;
 protected float m_HeldContents;
 protected ref BCMM_FluidLineSession m_OldLine;
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
  m_Mode.BCMM_TestRoleSetting("fluidLineConnectionSeconds",2);
  m_Mode.BCMM_TestRoleSetting("fluidLineRemovalSeconds",2);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.FullHeal();
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion=BCMM_BodyRegion.LEFT_ARM;
  m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  m_PatientInventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
  IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Kit=NewBag();
  m_Suite.Record(pack && m_Inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY) && m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Line fixture owns one exact finite physical source in the medic's native backpack");
  for(int index=0;index<128 && m_PatientInventory.FindStorageForItem(m_Kit,EStoragePurpose.PURPOSE_ANY);index++)
  {
   IEntity filler=NewBag();
   if(!m_PatientInventory.TryInsertItem(filler,EStoragePurpose.PURPOSE_ANY))
   {
    SCR_EntityHelper.DeleteEntityAndChildren(filler);
    break;
   }
   m_Fillers.Insert(filler);
  }
  m_Suite.Record(!m_PatientInventory.FindStorageForItem(m_Kit,EStoragePurpose.PURPOSE_ANY) && !BeginLine(BCMM_BodyRegion.LEFT_ARM) && m_Inventory.Contains(m_Kit) && Source().FluidRemainingMl()==500,"Full actual patient storage rejects connection without consuming or relocating the original finite bag");
  foreach(IEntity filler:m_Fillers)
  {
   SCR_EntityHelper.DeleteEntityAndChildren(filler);
  }
  m_Fillers.Clear();
  pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
  m_Suite.Record(pack && m_PatientInventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY),"Patient receives actual native capacity for its own connected bag");
  m_Suite.Record(!BeginLine(BCMM_BodyRegion.RIGHT_ARM),"A drip cannot attach at a different limb from the actual vascular access");
  GetGame().GetCallqueue().CallLater(BeginPreparedLine,1000);
 }
 protected void BeginPreparedLine()
 {
  PrintFormat("BCMM_LINE_READY reach=%1 source=%2 storage=%3 access=%4 region=%5 pressure=%6 using=%7 busy=%8",BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentKind.FLUID)!=null,m_PatientInventory.FindStorageForItem(m_Kit,EStoragePurpose.PURPOSE_ANY)!=null,m_Medical.BCMM_GetPatient().Equipment.VascularAccess,m_Medical.BCMM_GetPatient().Equipment.VascularAccessRegion,m_Medical.BCMM_GetPatient().SystolicPressure,ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),BCMM_SplintTreatment.IsBusy(m_Actor,m_Target));
  m_Suite.Record(BeginLine(BCMM_BodyRegion.LEFT_ARM),"Actual native timed connection starts with the exact source");
  m_Actor.SetOrigin("104 2.25 0");
  GetGame().GetCallqueue().CallLater(CheckLineCancelled,700);
 }
 protected IEntity NewBag()
 {
  return Spawn("{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et",m_Actor.GetOrigin());
 }
 protected BCMM_EquipmentItemComponent Source()
 {
  return BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
 }
 protected bool BeginLine(int region)
 {
  return BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.CONNECT_WHOLE_BLOOD_LINE,region);
 }
 protected void CheckLineCancelled()
 {
  m_Suite.Record(m_Inventory.Contains(m_Kit) && Source().FluidRemainingMl()==500 && !BCMM_FluidLineSession.ForPatient(m_Target) && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Movement before connection preserves exact bag, contents and patient reserve");
  m_Actor.SetOrigin("100 2.25 0");
  GetGame().GetCallqueue().CallLater(ConnectAgain,700);
 }
 protected void ConnectAgain()
 {
  m_BeforeVolume=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(BeginLine(BCMM_BodyRegion.LEFT_ARM),"Cancelled line releases native source and patient reservations");
  m_Suite.Record(!BeginLine(BCMM_BodyRegion.LEFT_ARM),"Concurrent connection cannot reserve the same source and patient twice");
  GetGame().GetCallqueue().CallLater(CheckFlow,6000);
 }
 protected void CheckFlow()
 {
  m_Suite.Record(m_PatientInventory.Contains(m_Kit) && !m_Inventory.Contains(m_Kit) && m_Medical.BCMM_FluidLineActive() && Source().FluidRemainingMl()>400 && Source().FluidRemainingMl()<500,"Native connection moves the same physical bag into actual patient inventory and begins finite progressive flow");
  float delivered=(m_Medical.GetBloodHitZone().GetHealthScaled()-m_BeforeVolume)*BCMM_TreatmentAccess.Setting("normalBloodVolumeMl");
  m_Suite.Record(Math.AbsFloat(delivered-(500-Source().FluidRemainingMl()))<0.1,"Actual native volume gain equals the debit from the connected original source");
  m_Suite.Record(!BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Connected independent line releases the medic's procedure lock and hands");
  m_Suite.Record(BCMM_SplintTreatment.IsReservedSupply(m_Kit) && BCMM_SplintTreatment.ContainsReservedSupply(m_Target) && !BCMM_EquipmentTreatment.FindEquipment(m_Target,BCMM_EquipmentKind.FLUID),"Connected source cannot be reused or hidden from recursive refund reservation checks");
  m_Suite.Record(!BCMM_NativeTreatmentSafety.FluidAllowed(m_Target,m_Actor),"Native inventory fluid routes cannot bypass an already connected independent line");
  m_HeldContents=Source().FluidRemainingMl();
  m_Actor.SetOrigin("105 2.25 0");
  GetGame().GetCallqueue().CallLater(CheckMedicFree,2000);
 }
 protected void CheckMedicFree()
 {
  m_Suite.Record(m_Medical.BCMM_FluidLineActive() && Source().FluidRemainingMl()<m_HeldContents && !BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),"Patient-owned original bag continues delivering after the medic genuinely leaves treatment reach");
  m_Actor.SetOrigin("100 2.25 0");
  GetGame().GetCallqueue().CallLater(BeginLineRemoval,700);
 }
 protected void BeginLineRemoval()
 {
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE,BCMM_BodyRegion.LEFT_ARM),"Empty-handed medic begins timed disconnection at the actual access limb");
  GetGame().GetCallqueue().CallLater(CheckLineRemoved,3000);
 }
 protected void CheckLineRemoved()
 {
  m_HeldContents=Source().FluidRemainingMl();
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && m_PatientInventory.Contains(m_Kit) && m_HeldContents>0 && m_HeldContents<500 && !BCMM_SplintTreatment.IsReservedSupply(m_Kit),"Disconnection leaves exact partially used bag in patient inventory and releases continuous-use reservation");
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Same original partially used patient-owned bag can resume without refill");
  m_OldLine=BCMM_FluidLineSession.ForPatient(m_Target);
  m_Mode.BCMM_TestRoleSetting("continuousFluidLinesEnabled",0);
  GetGame().GetCallqueue().CallLater(CheckPolicyStop,700);
 }
 protected void CheckPolicyStop()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && Source().FluidRemainingMl()<=m_HeldContents && !BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Live GM disable stops independent flow and rejects a new connection without resetting finite contents");
  m_Mode.BCMM_TestRoleSetting("continuousFluidLinesEnabled",1);
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Re-enabled line resumes only from retained actual contents");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=false;
  GetGame().GetCallqueue().CallLater(CheckAccessStop,700);
 }
 protected void CheckAccessStop()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && m_PatientInventory.Contains(m_Kit),"Actual vascular access loss stops flow while retaining original physical bag");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Valid access permits another bounded continuous session");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.8);
  GetGame().GetCallqueue().CallLater(ChangePressureLimit,1500);
 }
 protected void ChangePressureLimit()
 {
  m_Suite.Record(m_Medical.BCMM_GetPatient().SystolicPressure>=90 && m_Mode.BCMM_TestRoleSetting("fluidMaximumSystolicPressure",90),"Native line pressure stop uses a valid saved limit below the actual native pressure");
  GetGame().GetCallqueue().CallLater(CheckPressureStops,700);
 }
 protected void CheckPressureStops()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && m_PatientInventory.Contains(m_Kit),"A live native systolic stop-limit change ends flow without deleting or refilling its source");
  m_Mode.BCMM_TestRoleSetting("fluidMaximumSystolicPressure",140);
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"Restored pressure policy allows the original partial source to resume");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth());
  GetGame().GetCallqueue().CallLater(CheckFullReserveStops,700);
 }
 protected void CheckFullReserveStops()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && !BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"A genuinely full native blood reserve stops the line and rejects unnecessary continued infusion");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.7);
  m_HeldContents=Source().FluidRemainingMl();
  m_Suite.Record(BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM),"A remaining real deficit can resume the same original bag");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Kit);
  GetGame().GetCallqueue().CallLater(CheckSourceStop,700);
 }
 protected void CheckSourceStop()
 {
  m_Suite.Record(!m_Medical.BCMM_FluidLineActive() && !BCMM_FluidLineSession.ForPatient(m_Target),"Deleting the actual source ends flow without a replacement bag or invisible continuing infusion");
  m_Kit=NewBag();
  m_Suite.Record(m_Kit && m_PatientInventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Depletion case owns a separate actual finite source");
  Source().ConsumeFluidMl(Source().FluidRemainingMl());
  m_Suite.Record(Source().FluidRemainingMl()==0 && !BCMM_FluidLineSession.Start(m_Target,m_Kit,BCMM_BodyRegion.LEFT_ARM) && m_PatientInventory.Contains(m_Kit),"An actually exhausted original bag cannot start a drip or refill itself");
  Finish();
 }
}
