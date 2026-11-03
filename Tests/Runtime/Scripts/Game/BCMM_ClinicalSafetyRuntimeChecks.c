// Real finite inventory and native cancellation acceptance for vital-sign safeguards.
class BCMM_ClinicalSafetyRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected IEntity m_Fluid;
 protected IEntity m_RefundContainer;
 protected IEntity m_RefundArsenal;
 protected ref SCR_ConsumableMorphine m_NativeEffect;
 protected ref SCR_SalineDamageEffect m_LegacySaline;
 protected float m_SalineBefore;
 protected vector m_RangeActorStart;
 override protected void Finish()
 {
  // This standalone arsenal belongs to the fixture, not to the actor's
  // inventory hierarchy. Release it between repeated native instances.
  if(m_RefundArsenal)SCR_EntityHelper.DeleteEntityAndChildren(m_RefundArsenal);
  super.Finish();
 }
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.2 0");m_Target.SetOrigin("101 2.2 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());
  m_RefundContainer=backpack;
  m_RefundArsenal=Spawn("{B6C4ED1CA1262410}Prefabs/BCMM_ArsenalTransaction.et","100 2.2 1");
  m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Clinical safeguards fixture equips native inventory");
  m_Kit=Spawn("{6AB4B3F3FD216012}Prefabs/Items/BCMM_Morphine.et",m_Actor.GetOrigin());m_Fluid=Spawn("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Fluid && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY) && m_Inventory.TryInsertItemInStorage(m_Fluid,BaseInventoryStorageComponent.Cast(backpack.FindComponent(BaseInventoryStorageComponent))),"Safeguards use exact finite medication and fluid items");
  IEntity nativeMorphine=Spawn("{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",m_Actor.GetOrigin());
  SCR_ConsumableItemComponent nativeItem=SCR_ConsumableItemComponent.Cast(nativeMorphine.FindComponent(SCR_ConsumableItemComponent));
  m_NativeEffect=SCR_ConsumableMorphine.Cast(nativeItem.GetConsumableEffect());
  SCR_ConsumableItemComponent injection=SCR_ConsumableItemComponent.Cast(m_Kit.FindComponent(SCR_ConsumableItemComponent));
  SCR_ConsumableItemComponent bag=SCR_ConsumableItemComponent.Cast(m_Fluid.FindComponent(SCR_ConsumableItemComponent));
  m_Suite.Record(injection.GetConsumableType()==SCR_EConsumableType.MORPHINE && bag.GetConsumableType()==SCR_EConsumableType.BANDAGE,"Actual supplied injector and equipment carry valid distinct native animation categories");
  m_Suite.Record(m_NativeEffect!=null,"Actual stock morphine prefab resolves its guarded consumable effect through native config reflection");
  SCR_EntityHelper.DeleteEntityAndChildren(nativeMorphine);
  if(!m_NativeEffect) { Finish();return; }
  m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.5);
  GetGame().GetCallqueue().CallLater(CheckUnsafeStart,1600);
 }
 protected void CheckUnsafeStart()
 {
  m_Suite.Record(m_Medical.BCMM_SystolicPressure()<90 && !BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,0),"Native low pressure blocks morphine before animation or consumption");
  m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_GetPatient().Medication.ActiveDoses()==0,"Unsafe medication request preserves the original kit and adds no dose");
  SCR_ConsumableEffectBase nativeEffect=m_NativeEffect;
  m_Suite.Record(!nativeEffect.CanApplyEffect(m_Target,m_Actor),"Legacy native morphine effect predicate also rejects unsafe vitals");
  nativeEffect.ApplyEffect(m_Target,m_Actor,m_Kit,null);
  m_Suite.Record(m_Medical.BCMM_GetPatient().Medication.ActiveDoses()==0,"Direct native morphine effect cannot bypass server safety");
  m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);
  GetGame().GetCallqueue().CallLater(StartSafe,1500);
 }
 protected void StartSafe()
 {
  m_Suite.Record(BCMM_MedicationTreatment.BeginMedication(m_Actor,m_Target,0),"Recovered vitals permit native timed injection with the same item");
  GetGame().GetCallqueue().CallLater(DeteriorateDuringTreatment,2000);
 }
 protected void DeteriorateDuringTreatment()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Safety revalidation fixture reaches actual native injection animation");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.4);
  GetGame().GetCallqueue().CallLater(CheckSafetyCancellation,2500);
 }
 protected void CheckSafetyCancellation()
 {
  m_Suite.Record(!BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && m_Kit && m_Inventory.Contains(m_Kit) && m_Medical.BCMM_GetPatient().Medication.ActiveDoses()==0,"Deterioration during injection cancels before a dose and retains its exact item");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;m_Medical.BCMM_GetPatient().SystolicPressure=200;
  float before=m_Medical.GetBloodHitZone().GetHealthScaled();
  m_Suite.Record(!BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.INFUSE_SALINE,BCMM_BodyRegion.RIGHT_ARM),"High pressure blocks a native fluid procedure despite established access and deficit");
  m_Suite.Record(m_Medical.BCMM_DeliverReserveMl(500)==0 && m_Medical.GetBloodHitZone().GetHealthScaled()==before && m_Fluid && m_Inventory.Contains(m_Fluid),"Final native fluid adapter also enforces the pressure safeguard without losing the bag");
  m_Suite.Record(!BCMM_NativeTreatmentSafety.FluidAllowed(m_Target,m_Actor),"Native saline route shares the high-pressure fluid safeguard");
  m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);
  SCR_ConsumableEffectBase nativeEffect=m_NativeEffect;
  nativeEffect.ApplyEffect(m_Target,m_Actor,m_Kit,null);
  m_Suite.Record(m_Medical.BCMM_GetPatient().Medication.ActiveDoses()==1,"Permitted legacy native morphine effect enters the same BCM dose history and physiology");
  m_Medical.FullHeal();
  m_Suite.Record(m_Medical.BCMM_PauseBloodRegeneration(),"Default GM policy prevents automatic refilling after bleeding control");
  m_Medical.GetBloodHitZone().SetHealth(m_Medical.GetBloodHitZone().GetMaxHealth()*0.4);
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;BCMM_VisualStatusBridge.Publish(m_Medical);
  GetGame().GetCallqueue().CallLater(StartBCMSaline,1200);
 }
 protected void StartBCMSaline()
 {
  m_SalineBefore=m_Medical.GetBloodHitZone().GetHealth();
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.INFUSE_SALINE,BCMM_BodyRegion.LEFT_ARM),"Actual saline procedure begins before testing loss of access");
  GetGame().GetCallqueue().CallLater(RemoveFluidAccess,2000);
 }
 protected void RemoveFluidAccess()
 {
  m_Suite.Record(ChimeraCharacter.Cast(m_Actor).GetCharacterController().IsUsingItem(),"Saline access-loss test reaches a real native item animation");
  m_Suite.Record(BCMM_SplintTreatment.IsReservedSupply(m_Fluid) && BCMM_SplintTreatment.ContainsReservedSupply(m_Fluid),"Active native infusion reserves its exact supply against refund");
  m_Suite.Record(BCMM_SplintTreatment.ContainsReservedSupply(m_RefundContainer),"Enclosing native backpack cannot bypass the active supply refund reservation");
  m_Suite.Record(!BCMM_SplintTreatment.ContainsReservedSupply(m_Kit),"Unrelated medication remains outside the active fluid refund reservation");
  BCMM_EquipmentItemComponent reservedSource=BCMM_EquipmentItemComponent.Cast(m_Fluid.FindComponent(BCMM_EquipmentItemComponent));
  float reservedRemaining=reservedSource.FluidRemainingMl();array<float> refill=reservedSource.BCMM_CaptureFiniteSupply();refill[3]=refill[2];
  m_Suite.Record(!reservedSource.BCMM_RestoreFiniteSupply(refill) && reservedSource.FluidRemainingMl()==reservedRemaining,"Loading saved item state cannot refill a source reserved by a running native procedure");
  SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(GetGame().GetPlayerController().FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
  SCR_ResourceComponent resources=SCR_ResourceComponent.Cast(m_RefundArsenal.FindComponent(SCR_ResourceComponent));
  transactions.RpcAsk_ArsenalRefundItem(Replication.FindItemId(resources),Replication.FindItemId(m_Fluid.FindComponent(InventoryItemComponent)),EResourceType.SUPPLIES);
  transactions.RpcAsk_ArsenalRefundItem(Replication.FindItemId(resources),Replication.FindItemId(m_RefundContainer.FindComponent(InventoryItemComponent)),EResourceType.SUPPLIES);
  GetGame().GetCallqueue().CallLater(CheckReservedRefundDenied,300);
 }
 protected void CheckReservedRefundDenied()
 {
  SCR_ResourceComponent resources=SCR_ResourceComponent.Cast(m_RefundArsenal.FindComponent(SCR_ResourceComponent));
  m_Suite.Record(m_Fluid && m_RefundContainer && m_Inventory.Contains(m_Fluid) && m_Inventory.Contains(m_RefundContainer) && BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),"Real native direct-source and enclosing-backpack refund RPCs preserve the active procedure and both exact items");
  m_Suite.Record(resources && resources.GetContainer(EResourceType.SUPPLIES).GetResourceValue()==1000,"Denied active-treatment refund RPCs cannot credit arsenal resources");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=false;
  m_SalineBefore=m_Medical.GetBloodHitZone().GetHealth();
  GetGame().GetCallqueue().CallLater(CheckFluidCancelled,1200);
 }
 protected void CheckFluidCancelled()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Fluid.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(m_Fluid && m_Inventory.Contains(m_Fluid) && !BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && m_Medical.GetBloodHitZone().GetHealth()<=m_SalineBefore+0.001,"Access loss stops further progressive delivery and retains the exact physical bag");
  m_Suite.Record(source && source.FluidRemainingMl()>0 && source.FluidRemainingMl()<source.FluidCapacityMl(),"Interrupted native infusion preserves its partial contents instead of resetting the bag");
  m_Medical.BCMM_GetPatient().Equipment.VascularAccess=true;
  m_Suite.Record(!BCMM_SplintTreatment.ContainsReservedSupply(m_Fluid) && !BCMM_SplintTreatment.ContainsReservedSupply(m_RefundContainer),"Cancellation releases the partial supply and enclosing backpack refund reservations");
  GetGame().GetCallqueue().CallLater(StartLegacySaline,500);
 }
 protected void StartLegacySaline()
 {
  m_LegacySaline=new SCR_SalineDamageEffect();m_LegacySaline.SetDamageType(m_LegacySaline.GetDefaultDamageType());m_LegacySaline.SetInstigator(Instigator.CreateInstigator(m_Actor));m_LegacySaline.SetDPS(-5);m_LegacySaline.SetMaxDuration(20);m_LegacySaline.SkipRegenDelay(true);
  m_Medical.BCMM_TestHoldPassiveBlood(true);
  m_SalineBefore=m_Medical.GetBloodHitZone().GetHealth();m_Medical.AddDamageEffect(m_LegacySaline);
  m_LegacySaline=SCR_SalineDamageEffect.Cast(m_Medical.FindDamageEffectOfType(SCR_SalineDamageEffect));
  GetGame().GetCallqueue().CallLater(CheckLegacyDelivery,1200);
 }
 protected void CheckLegacyDelivery()
 {
  PrintFormat("BCMM_LEGACY_SALINE before=%1 after=%2 active=%3",m_SalineBefore,m_Medical.GetBloodHitZone().GetHealth(),m_LegacySaline.IsActive());
  m_Suite.Record(m_LegacySaline.IsActive() && m_Medical.GetBloodHitZone().GetHealth()>m_SalineBefore,"Actual native saline damage effect delivers reserve when BCM criteria are satisfied");
  m_SalineBefore=m_Medical.GetBloodHitZone().GetHealth();m_Medical.BCMM_GetPatient().Equipment.VascularAccess=false;
  GetGame().GetCallqueue().CallLater(CheckLegacyStopped,1000);
 }
 protected void CheckLegacyStopped()
 {
  PrintFormat("BCMM_LEGACY_SALINE_STOP before=%1 after=%2 active=%3 allowed=%4 duration=%5 max=%6",m_SalineBefore,m_Medical.GetBloodHitZone().GetHealth(),m_LegacySaline.IsActive(),BCMM_NativeTreatmentSafety.FluidAllowed(m_Target,m_Actor),m_LegacySaline.GetCurrentDuration(),m_LegacySaline.GetMaxDuration());
  m_Suite.Record(!m_Medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && m_Medical.GetBloodHitZone().GetHealth()<=m_SalineBefore+0.001,"Running native saline terminates before further delivery when access is lost");
  BeginCasualtyRangeLoss();
 }
 protected void BeginCasualtyRangeLoss()
 {
  m_Medical.FullHeal();
  vector pose[4];m_Target.GetWorldTransform(pose);pose[3]=m_Actor.GetOrigin()+Vector(1.2,0,0);ChimeraCharacter.Cast(m_Target).Teleport(pose);
  m_Kit=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());
  m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Casualty range-loss case owns an exact unused finite oxygen source");
  GetGame().GetCallqueue().CallLater(StartCasualtyRangeLoss,1000);
 }
 protected void StartCasualtyRangeLoss()
 {
  m_RangeActorStart=m_Actor.GetOrigin();
  m_Suite.Record(BCMM_EquipmentTreatment.BeginEquipment(m_Actor,m_Target,BCMM_EquipmentProcedure.CONNECT_OXYGEN,BCMM_BodyRegion.HEAD),"Casualty range-loss case starts a real native oxygen connection inside treatment range");
  GetGame().GetCallqueue().CallLater(MoveCasualtyOutOfRange,2500);
 }
 protected void MoveCasualtyOutOfRange()
 {
  m_Suite.Record(BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && !m_Medical.BCMM_GetPatient().Equipment.OxygenActive,"Range-loss case has an active server-timed connection before casualty displacement");
  vector pose[4];m_Target.GetWorldTransform(pose);pose[3]=m_Actor.GetOrigin()+Vector(BCMM_TreatmentAccess.Setting("treatmentRangeMetres")+0.2,0,0);ChimeraCharacter.Cast(m_Target).Teleport(pose);
  GetGame().GetCallqueue().CallLater(CheckCasualtyRangeLoss,1000);
 }
 protected void CheckCasualtyRangeLoss()
 {
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Kit.FindComponent(BCMM_EquipmentItemComponent));
  m_Suite.Record(vector.Distance(m_Actor.GetOrigin(),m_Target.GetOrigin())>BCMM_TreatmentAccess.Setting("treatmentRangeMetres") && vector.Distance(m_Actor.GetOrigin(),m_RangeActorStart)<0.1,"Only the casualty crosses the configured range boundary while the medic stays still");
  m_Suite.Record(!BCMM_SplintTreatment.IsBusy(m_Actor,m_Target) && m_Kit && m_Inventory.Contains(m_Kit) && source.OxygenLitres()==source.BCMM_CaptureFiniteSupply()[2] && !m_Medical.BCMM_GetPatient().Equipment.OxygenActive,"Casualty range loss cancels native connection, retains the unchanged original source and applies no oxygen");
  Finish();
 }
}
// Isolate infusion delivery from vanilla passive blood regeneration in this test only.
modded class SCR_CharacterDamageManagerComponent
{
 protected bool m_BCMMHoldPassiveBlood;
 void BCMM_TestHoldPassiveBlood(bool hold) { m_BCMMHoldPassiveBlood=hold; }
 override bool BCMM_PauseBloodRegeneration() { return m_BCMMHoldPassiveBlood || super.BCMM_PauseBloodRegeneration(); }
}


