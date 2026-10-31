// Real finite inventory, full default-duration typed-fluid integration.
class BCMM_FluidsRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Trial;
 protected float m_Before;
 protected float m_RedBefore;
 protected float m_PartialRemaining;
 protected float m_CancelReserve;
 protected ref array<int> m_Actions={16,17,13};
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.25 0");m_Target.SetOrigin("101 2.25 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Typed-fluid fixture equips native backpack");
  PrepareTrial();
 }
 protected void PrepareTrial()
 {
  m_Medical.FullHeal();m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.5);
  array<ResourceName> bags={"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et","{B6D83F6853C68D73}Prefabs/Items/BCMM_Plasma.et","{6AB4B3F3FD216029}Prefabs/Items/BCMM_FluidBag.et"};
  m_Kit=Spawn(bags[m_Trial],m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Distinct typed-fluid bag enters actual inventory");
  GetGame().GetCallqueue().CallLater(BeginTrial,2000);
 }
 protected void BeginTrial()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();int action=m_Actions[m_Trial];
  m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,action,BCMM_BodyRegion.LEFT_ARM),"Typed-fluid bag cannot be used without vascular access");
  patient.Equipment.VascularAccess=true;
  int wrong=BCMM_EquipmentProcedure.INFUSE_SALINE;if(m_Trial==0) { wrong=BCMM_EquipmentProcedure.INFUSE_PLASMA; }
  m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,wrong,BCMM_BodyRegion.LEFT_ARM),"A differently typed bag cannot silently substitute for the requested fluid");
  m_Before=m_Medical.GetBloodHitZone().GetHealthScaled();m_RedBefore=patient.Fluids.RedCellReserve;
  m_Suite.Record(m_Medical.BCMM_DeliverReserveMl(500,99)==0 && m_Medical.GetBloodHitZone().GetHealthScaled()==m_Before,"Native delivery rejects unknown fluid type without changing reserve");
  array<IEntity> debugItems={};m_Inventory.GetItems(debugItems,EStoragePurpose.PURPOSE_ANY);PrintFormat("BCMM_FLUID_INVENTORY contains=%1 count=%2 expected=%3 max=%4",m_Inventory.Contains(m_Kit),debugItems.Count(),BCMM_EquipmentPolicy.Kind(action),BCMM_SupplyCatalog.EQUIPMENT_COUNT);foreach(IEntity debugItem:debugItems) { BCMM_EquipmentItemComponent marker=BCMM_EquipmentItemComponent.Cast(debugItem.FindComponent(BCMM_EquipmentItemComponent));if(marker) { PrintFormat("BCMM_FLUID_ITEM kind=%1 same=%2",marker.Kind,debugItem==m_Kit); } }
  PrintFormat("BCMM_FLUID_READY kind=%1 found=%2 reach=%3 role=%4 eligible=%5 denial=%6 using=%7 busy=%8",BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent)).Kind,BCMM_EquipmentTreatment.FindEquipment(m_Actor,BCMM_EquipmentPolicy.Kind(action))!=null,BCMM_TreatmentAccess.CanReach(m_Actor,m_Target),BCMM_MedicRole.EquipmentAllowed(m_Actor,action),BCMM_EquipmentPolicy.Eligible(patient,action,0,new BCMM_Settings()),BCMM_ClinicalSafety.FluidDenial(patient,m_Before,new BCMM_Settings()),ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),BCMM_SplintTreatment.IsBusy(m_Actor,m_Target));
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,action,BCMM_BodyRegion.LEFT_ARM),"Full default-time typed infusion starts with exact item and access");
  Print("BCMM_FLUID_BEGIN "+BCMM_EquipmentPolicy.Name(action));
  float duration=BCMM_TreatmentAccess.Setting("fluidInfusionSeconds");
  m_Suite.Record(duration==60,"Acceptance uses full default sixty-second infusion duration");
  GetGame().GetCallqueue().CallLater(CheckMidpoint,duration*500);
  GetGame().GetCallqueue().CallLater(CheckDelivered,duration*1000+8000);
 }
 protected void CheckMidpoint()
 {
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem() && BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Fluid remains reserved in native animation at midpoint");
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  float reserve=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(source && source.FluidCapacityMl()==500 && source.FluidRemainingMl()>200 && source.FluidRemainingMl()<300,"Native midpoint retains the exact bag with partially depleted finite contents");
  m_Suite.Record(reserve>m_Before+0.035 && reserve<m_Before+0.06,"Typed fluid reaches native circulating reserve progressively before completion");
  PrintFormat("BCMM_FLUID_PROGRESS type=%1 remainingMl=%2 reserve=%3",m_Actions[m_Trial],source.FluidRemainingMl(),reserve);
 }
 protected void CheckDelivered()
 {
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();float reserve=m_Medical.GetBloodHitZone().GetHealthScaled();
  PrintFormat("BCMM_FLUID_DELIVERED type=%1 volume=%2 red=%3 clot=%4 oxygen=%5 delivery=%6",m_Actions[m_Trial],reserve,patient.Fluids.RedCellReserve,patient.Fluids.ClottingReserve,patient.OxygenSaturation,patient.OxygenDeliveryFraction);
  m_Suite.Record(!m_Kit && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Successful typed infusion consumes the reserved bag exactly once");
  m_Suite.Record(reserve>m_Before+0.085 && reserve<=m_Before+0.101,"Finite five-hundred-millilitre bag changes native reserve with bounded water redistribution");
  if(m_Trial<2) { m_Suite.Record(Math.AbsFloat(patient.Fluids.RedCellReserve-m_RedBefore)<0.001,"Saline and plasma do not add native oxygen-carrying reserve"); }
  else { m_Suite.Record(patient.Fluids.RedCellReserve>m_RedBefore+0.099,"Whole blood supplies oxygen-carrying reserve through actual item completion"); }
  if(m_Trial==0) { m_Suite.Record(patient.Fluids.ClottingReserve<0.501 && patient.Fluids.TransientWater>0.06,"Saline dilution and redistribution are recorded by production native adapter"); }
  else { m_Suite.Record(patient.Fluids.ClottingReserve>0.599,"Plasma and whole blood supply clotting reserve through native adapter"); }
  m_Suite.Record(patient.OxygenSaturation>0.97 && patient.HasSpontaneousCirculation,"Appropriate replacement preserves ventilation and circulation without false saturation loss");
  if(m_Trial==0)
  {
   m_Before=reserve;m_RedBefore=patient.Fluids.RedCellReserve;
   GetGame().GetCallqueue().CallLater(CheckRedistribution,10000);return;
  }
  NextTrial();
 }
 protected void CheckRedistribution()
 {
  m_Suite.Record(m_Medical.GetBloodHitZone().GetHealthScaled()<m_Before-0.001 && Math.AbsFloat(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve-m_RedBefore)<0.001,"Native saline water redistributes without phantom haemorrhage or red-cell loss");
  NextTrial();
 }
 protected void NextTrial()
 {
  m_Trial++;if(m_Trial<3) { PrepareTrial();return; }
  m_Medical.FullHeal();BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  m_Suite.Record(patient.Fluids.RedCellReserve==1 && patient.Fluids.ClottingReserve==1 && patient.Fluids.TransientWater==0,"Native FullHeal resets fluid composition and transient load");
  BeginInterruption();
 }
 protected void BeginInterruption()
 {
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.5);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Kit=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Progressive interruption fixture owns one exact fresh saline bag");
  GetGame().GetCallqueue().CallLater(StartInterruption,1500);
 }
 protected void StartInterruption()
 {
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.INFUSE_SALINE,BCMM_BodyRegion.LEFT_ARM),"Native partial infusion begins before authority cancellation");
  GetGame().GetCallqueue().CallLater(CancelPartial,8000);
 }
 protected void CancelPartial()
 {
  BCMM_SplintTreatment.CancelActor(m_Actor);
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_PartialRemaining=source.FluidRemainingMl();m_CancelReserve=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(m_PartialRemaining>350 && m_PartialRemaining<480 && m_CancelReserve>0.5,"Cancellation occurs after finite partial delivery, not before native use begins");
  GetGame().GetCallqueue().CallLater(ResumePartial,1500);
 }
 protected void ResumePartial()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(m_Inventory.Contains(m_Kit) && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && source.FluidRemainingMl()==m_PartialRemaining,"Cancellation preserves the same partial bag without refilling its contents");
  m_Suite.Record(m_Medical.GetBloodHitZone().GetHealthScaled()<=m_CancelReserve+0.0001,"Cancelled infusion cannot continue delivering in the background");
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.INFUSE_SALINE,BCMM_BodyRegion.LEFT_ARM),"The exact partial bag can resume a new validated native infusion");
  GetGame().GetCallqueue().CallLater(CheckResumed,m_PartialRemaining/500*60000+8000);
 }
 protected void CheckResumed()
 {
  float reserve=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(!m_Kit && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Resuming consumes the original bag exactly once when its remaining volume is exhausted");
  m_Suite.Record(reserve>0.58 && reserve<0.602 && Math.AbsFloat(m_Medical.BCMM_GetPatient().Fluids.RedCellReserve-0.5)<0.001,"Interrupted and resumed saline delivers at most its original capacity and never invents red cells");
  PrintFormat("BCMM_FLUID_RESUMED reserve=%1 partialMl=%2",reserve,m_PartialRemaining);
  Finish();
 }
}
