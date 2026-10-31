// Dedicated actual menu requests, finite devices and owner/proxy/active-session JIP.
modded class SCR_BaseGameMode
{
 [RplProp()] protected RplId m_BCMMArsenalUS=RplId.Invalid();[RplProp()] protected RplId m_BCMMArsenalUSSR=RplId.Invalid();[RplProp()] protected RplId m_BCMMArsenalFIA=RplId.Invalid();
 void BCMM_PublishArsenalBoxes(IEntity us,IEntity ussr,IEntity fia) { m_BCMMArsenalUS=BCMM_BodyBagComponent.Id(us);m_BCMMArsenalUSSR=BCMM_BodyBagComponent.Id(ussr);m_BCMMArsenalFIA=BCMM_BodyBagComponent.Id(fia);Replication.BumpMe(); }
 IEntity BCMM_NativeUSArsenal() { return BCMM_BodyBagComponent.Resolve(m_BCMMArsenalUS); }
 IEntity BCMM_NativeUSSRArsenal() { return BCMM_BodyBagComponent.Resolve(m_BCMMArsenalUSSR); }
 IEntity BCMM_NativeFIAArsenal() { return BCMM_BodyBagComponent.Resolve(m_BCMMArsenalFIA); }

 [RplProp()] protected int m_BCMMOxygenPhase;
 [RplProp()] protected RplId m_BCMMOxygenPatient=RplId.Invalid();
 void BCMM_PublishOxygenTest(int phase,IEntity patient) { m_BCMMOxygenPhase=phase;m_BCMMOxygenPatient=BCMM_BodyBagComponent.Id(patient);Replication.BumpMe();PrintFormat("BCMM_OXYGEN_PHASE %1",phase); }
 int BCMM_OxygenPhase() { return m_BCMMOxygenPhase; }
 IEntity BCMM_OxygenPatient() { return BCMM_BodyBagComponent.Resolve(m_BCMMOxygenPatient); }
}
class BCMM_OxygenMedicationNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Phase;protected IEntity m_Oxygen;protected float m_GasBefore;protected vector m_SelfStart;
 override protected void Prepare()
 {
  SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));
  if(!player || !player.GetControlledEntity()) { if(m_OwnerWait++<100) { GetGame().GetCallqueue().CallLater(Prepare,250);return; }m_Suite.Record(false,"Oxygen fixture receives actual owner");Finish();return; }
  SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);m_Actor=player.GetControlledEntity();SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Target.SetOrigin("101 2.25 0");AIControlComponent ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));m_Inventory=InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(InventoryStorageManagerComponent));
  IEntity backpack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Actor.GetOrigin());m_Suite.Record(backpack && m_Inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"Oxygen owner receives native inventory");
  IEntity usBox=Spawn("{45C675ED3CFBBEC5}Prefabs/Props/Military/Compositions/US/ArsenalBox_US.et","104 2.25 4");IEntity ussrBox=Spawn("{AD59EDEAA26CE792}Prefabs/Props/Military/Compositions/USSR/ArsenalBox_USSR.et","108 2.25 4");IEntity fiaBox=Spawn("{B53B98CEA2D72735}Prefabs/Props/Military/Compositions/FIA/ArsenalBox_FIA.et","112 2.25 4");
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_PublishArsenalBoxes(usBox,ussrBox,fiaBox);mode.BCMM_TestRoleSetting("oxygenCapacityLitres",10);mode.BCMM_TestRoleSetting("oxygenFlowLitresPerMinute",20);mode.BCMM_TestRoleSetting("oxygenApplicationSeconds",3);mode.BCMM_TestRoleSetting("fentanylApplicationSeconds",3);mode.BCMM_TestRoleSetting("paracetamolApplicationSeconds",12);mode.BCMM_TestRoleSetting("paracetamolOnsetSeconds",10);mode.BCMM_TestRoleSetting("medicationApplicationSeconds",3);
  m_Medical.BCMM_GetPatient().LeftChest.CollapseFraction=0.8;m_Medical.BCMM_GetPatient().OxygenSaturation=0.588;
  SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SetMedic(true);
  m_Oxygen=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());m_Suite.Record(m_Oxygen && m_Inventory.TryInsertItem(m_Oxygen,EStoragePurpose.PURPOSE_ANY),"Owner receives finite native oxygen source");
  GetGame().GetCallqueue().CallLater(Baseline,2000);
 }
 protected void Baseline() { Publish(1,18000); }
 protected void Publish(int phase,int delay) { m_Phase=phase;SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishOxygenTest(phase,m_Target);GetGame().GetCallqueue().CallLater(Next,delay); }
 protected void Supply(ResourceName resource) { m_Kit=Spawn(resource,m_Actor.GetOrigin());m_Suite.Record(m_Kit && m_Inventory.TryInsertItem(m_Kit,EStoragePurpose.PURPOSE_ANY),"Next exact native medication supply enters owning inventory"); }
 protected void Next()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  if(m_Phase==1) { Publish(2,25000);return; }
  if(m_Phase==2)
  {
   float remaining=BCMM_EquipmentItemComponent.Cast(m_Oxygen.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres();
   m_Suite.Record(patient.Equipment.OxygenActive && m_Oxygen && m_Inventory.Contains(m_Oxygen) && remaining>0 && remaining<5 && patient.OxygenSaturation>0.7,"Actual owner oxygen request retains exact source, drains finite gas and improves native oxygenation");
   m_GasBefore=remaining;mode.BCMM_TestRoleSetting("oxygenEnabled",0);mode.BCMM_TestRoleSetting("arsenalUSEnabled",0);Publish(3,8000);return;
  }
  if(m_Phase==3)
  {
   m_Suite.Record(!patient.Equipment.OxygenActive && !(m_Medical.BCMM_AppliedEquipment() & 64) && BCMM_EquipmentItemComponent.Cast(m_Oxygen.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres()>=m_GasBefore-0.2,"Live GM disable stops supply, gas drain and replicated mask without deleting source");
   mode.BCMM_TestRoleSetting("oxygenEnabled",1);mode.BCMM_TestRoleSetting("arsenalUSEnabled",1);mode.BCMM_TestRoleSetting("arsenalCostOxygen",21);m_Medical.FullHeal();patient=m_Medical.BCMM_GetPatient();patient.AddWound(BCMM_BodyRegion.LEFT_ARM,2);patient.Equipment.VascularAccess=true;Supply("{D0F8A8C2097AA101}Prefabs/Items/BCMM_Fentanyl.et");Publish(4,20000);return;
  }
  if(m_Phase==4)
  {
   m_Suite.Record(!m_Kit && patient.Medication.HasActiveKind(5) && patient.RespiratoryRate<12 && patient.SystolicPressure<115,"Owner fentanyl menu request consumes one item and changes linked native vitals");Supply("{6AB4B3F3FD216013}Prefabs/Items/BCMM_Naloxone.et");Publish(5,15000);return;
  }
  if(m_Phase==5)
  {
   m_Suite.Record(!m_Kit && patient.Medication.HasActiveKind(1) && patient.Medication.BreathingScale>0.9,"Owner naloxone request reverses fentanyl-only exposure");m_Medical.FullHeal();m_Medical.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,2);Supply("{D0F8A8C2097AA102}Prefabs/Items/BCMM_Paracetamol.et");Publish(6,22000);return;
  }
  if(m_Phase==6)
  {
   m_Suite.Record(!m_Kit && patient.Medication.HasActiveKind(4) && patient.Medication.Pain<5 && patient.Medication.BreathingScale==1,"Owner oral medicine consumes one supply and relieves pain without opioid physiology");m_Medical.FullHeal();Publish(7,6000);return;
  }
  if(m_Phase==7)
  {
   m_Suite.Record(!patient.Equipment.OxygenActive && patient.Medication.ActiveDoses()==0,"FullHeal clears oxygen and medication clinical state");
   patient.LeftChest.CollapseFraction=0.8;patient.OxygenSaturation=0.588;
   IEntity fresh=Spawn("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et",m_Actor.GetOrigin());m_Suite.Record(fresh && m_Inventory.TryInsertItem(fresh,EStoragePurpose.PURPOSE_ANY),"Owner receives a usable spare alongside its partially used oxygen kit");m_Oxygen=fresh;Publish(8,25000);return;
  }
  if(m_Phase==8)
  {
   m_Suite.Record(patient.Equipment.OxygenActive && BCMM_EquipmentItemComponent.Cast(m_Oxygen.FindComponent(BCMM_EquipmentItemComponent)).OxygenLitres()<5,"Actual second connection selects the fuller source instead of the old partially used kit");
   SCR_EntityHelper.DeleteEntityAndChildren(m_Oxygen);Publish(9,8000);return;
  }
  if(m_Phase==9)
  {
   m_Suite.Record(!patient.Equipment.OxygenActive && !(m_Medical.BCMM_AppliedEquipment() & 64),"Actual source deletion stops supply and publishes mask removal");m_Medical.FullHeal();Publish(10,5000);return;
  }
  if(m_Phase==10)
  {
   SCR_CharacterDamageManagerComponent self=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));self.FullHeal();self.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);Supply("{D0F8A8C2097AA102}Prefabs/Items/BCMM_Paracetamol.et");m_Phase=11;mode.BCMM_PublishOxygenTest(11,m_Actor);GetGame().GetCallqueue().CallLater(Next,22000);return;
  }
  if(m_Phase==11)
  {
   SCR_CharacterDamageManagerComponent self=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));m_Suite.Record(!m_Kit && self.BCMM_GetPatient().Medication.HasActiveKind(4),"Actual owner self/swap menu uses the native self-tablet graph and consumes one supply");
   self.FullHeal();self.BCMM_GetPatient().AddWound(BCMM_BodyRegion.LEFT_ARM,1);Supply("{D0F8A8C2097AA102}Prefabs/Items/BCMM_Paracetamol.et");m_SelfStart=m_Actor.GetOrigin();m_Phase=12;mode.BCMM_PublishOxygenTest(12,m_Actor);GetGame().GetCallqueue().CallLater(Next,12000);return;
  }
  if(m_Phase==12)
  {
   SCR_CharacterDamageManagerComponent self=SCR_CharacterDamageManagerComponent.Cast(m_Actor.FindComponent(SCR_CharacterDamageManagerComponent));m_Suite.Record(m_Kit && m_Inventory.Contains(m_Kit) && !self.BCMM_GetPatient().Medication.HasActiveKind(4) && vector.Distance(m_SelfStart,m_Actor.GetOrigin())>0.5 && !BCMM_CPRAnimation.IsActive(m_Actor,BCMM_FieldClips.TABLETSELF_GRAPH),"Actual owner displacement interrupts self tablets, retains the exact supply and applies no medication");
   self.FullHeal();m_Phase=13;mode.BCMM_PublishOxygenTest(13,m_Actor);GetGame().GetCallqueue().CallLater(Finish,5000);return;
  }
  Finish();
 }
}
modded class ArmaReforgerScripted
{
 protected float m_BCMMOwnerDebugAt;
 protected bool m_BCMMArsenalChecked;protected bool m_BCMMTabletGraphSeen;protected bool m_BCMMSelfTabletSeen;protected bool m_BCMMSelfDisplaced;
 protected int m_BCMMOxygenApproachedPhase;protected int m_BCMMOxygenObserved;protected int m_BCMMOxygenRequested;protected bool m_BCMMOxygenPositioned;protected float m_BCMMOxygenRequestAt;protected int m_BCMMOxygenSeenPhase;protected float m_BCMMOxygenPhaseSince;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string observer;if(Replication.IsServer() || !System.GetCLIParam("bcmmOxygenObserver",observer)) { return; }
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());if(!mode || !player || !player.GetControlledEntity()) { return; }
  IEntity target=mode.BCMM_OxygenPatient();int phase=mode.BCMM_OxygenPhase();if(!target || phase<=0 || !mode.BCMM_HasSettingsSnapshot()) { return; }
  SCR_FactionManager factions=SCR_FactionManager.Cast(GetFactionManager());if(!factions || !factions.GetFactionByKey("US")) return;
  SCR_EntityCatalogManagerComponent catalogs=SCR_EntityCatalogManagerComponent.GetInstance();if(catalogs) catalogs.BCMM_TestArsenalFactionManager(factions);
  bool traceOwner=player.GetPlayerId()==1 && phase==2 && world.GetWorldTime()>m_BCMMOwnerDebugAt;
  if(traceOwner) { m_BCMMOwnerDebugAt=world.GetWorldTime()+2000;PrintFormat("BCMM_OWNER_WAIT reach=%1 menu=%2 owner=%3 actor=%4 target=%5",BCMM_TreatmentAccess.CanReach(player.GetControlledEntity(),target),BCMM_MedicalMenu.Current()!=null,RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent)).IsOwner(),player.GetControlledEntity().GetOrigin(),target.GetOrigin()); }
  IEntity usBoxEntity=mode.BCMM_NativeUSArsenal();IEntity ussrBoxEntity=mode.BCMM_NativeUSSRArsenal();IEntity fiaBoxEntity=mode.BCMM_NativeFIAArsenal();
  if(!usBoxEntity || !ussrBoxEntity || !fiaBoxEntity) { return; }
  array<IEntity> boxes={usBoxEntity,ussrBoxEntity,fiaBoxEntity};
  for(int boxIndex=0;boxIndex<boxes.Count();boxIndex++)
  {
   IEntity box=boxes[boxIndex];if(!box) { return; }
   SCR_ArsenalComponent arsenal=SCR_ArsenalComponent.Cast(box.FindComponent(SCR_ArsenalComponent));
   SCR_ArsenalInventoryStorageManagerComponent stock=SCR_ArsenalInventoryStorageManagerComponent.Cast(box.FindComponent(SCR_ArsenalInventoryStorageManagerComponent));
   if(!arsenal || !stock) { return; }
   array<SCR_ArsenalItem> available={};arsenal.GetFilteredArsenalItems(available);int medicalCount=0;
   foreach(SCR_ArsenalItem entry:available) { if(BCMM_ArsenalItem.Cast(entry)) { medicalCount++; } }
   int expected=29;if(boxIndex==0 && mode.BCMM_ReadSetting("arsenalUSEnabled")<0.5) { expected=0; }
   bool cached=stock.IsPrefabInArsenalStorage("{D0F8A8C2097AA103}Prefabs/Items/BCMM_Oxygen.et");
   if(medicalCount!=expected || cached!=(expected==29)) { if(traceOwner) PrintFormat("BCMM_BOX_WAIT index=%1 count=%2 expected=%3 cached=%4",boxIndex,medicalCount,expected,cached);return; }
  }
  if(!m_BCMMArsenalChecked)
  {
   bool us=BCMM_ArsenalChecks.Inspect("{5F7EC52FC40A03E2}Configs/EntityCatalog/US/InventoryItems_EntityCatalog_US.conf","US");
   bool ussr=BCMM_ArsenalChecks.Inspect("{C53421647C3D0D2E}Configs/EntityCatalog/USSR/InventoryItems_EntityCatalog_USSR.conf","USSR");
   bool fia=BCMM_ArsenalChecks.Inspect("{E908001749419691}Configs/EntityCatalog/FIA/InventoryItems_EntityCatalog_FIA.conf","FIA");
   if(!us || !ussr || !fia) { Print("BCMM_CHECK FAILED: replicated client faction medical catalogs",LogLevel.ERROR);return; }
   m_BCMMArsenalChecked=true;
  }
  if(phase==6 && !m_BCMMTabletGraphSeen)
  {
   IEntity medic=GetPlayerManager().GetPlayerControlledEntity(1);
   if(medic && BCMM_CPRAnimation.IsActive(medic,BCMM_FieldClips.TABLETOTHER_GRAPH)) { m_BCMMTabletGraphSeen=true;PrintFormat("BCMM_TABLET_NETWORK_ACTIVE viewer=%1",player.GetPlayerId()); }
  }
  IEntity own=player.GetControlledEntity();IEntity medic=GetPlayerManager().GetPlayerControlledEntity(1);
  if(phase==11 && !m_BCMMSelfTabletSeen && medic && BCMM_CPRAnimation.IsActive(medic,BCMM_FieldClips.TABLETSELF_GRAPH)) { m_BCMMSelfTabletSeen=true;PrintFormat("BCMM_SELF_TABLET_NETWORK_ACTIVE viewer=%1",player.GetPlayerId()); }
  if(phase==12 && player.GetPlayerId()==1 && !m_BCMMSelfDisplaced && m_BCMMOxygenRequested==12 && BCMM_CPRAnimation.IsActive(own,BCMM_FieldClips.TABLETSELF_GRAPH))
  {
   vector displaced[4];own.GetWorldTransform(displaced);displaced[3]=displaced[3]+Vector(0.8,0,0);ChimeraCharacter.Cast(own).Teleport(displaced);m_BCMMSelfDisplaced=true;Print("BCMM_SELF_TABLET_CANCEL_SENT");
  }
  if(!m_BCMMOxygenPositioned)
  {
   if(player.GetPlayerId()==1) { vector pose[4];player.GetControlledEntity().GetWorldTransform(pose);pose[3]=target.GetOrigin()+"0 0.2 -1.2";ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(pose);BCMM_MedicalMenu.Open(target); }
   m_BCMMOxygenPositioned=true;m_BCMMOxygenRequestAt=world.GetWorldTime()+2000;
  }
  if(traceOwner) { PrintFormat("BCMM_OWNER_VALUES flow=%1 onset=%2 fentanyl=%3 tablets=%4 requested=%5 at=%6 now=%7",mode.BCMM_ReadSetting("oxygenFlowLitresPerMinute"),mode.BCMM_ReadSetting("paracetamolOnsetSeconds"),mode.BCMM_ReadSetting("fentanylApplicationSeconds"),mode.BCMM_ReadSetting("paracetamolApplicationSeconds"),m_BCMMOxygenRequested,m_BCMMOxygenRequestAt,world.GetWorldTime()); }
  bool requestPhase=phase==2 || phase==4 || phase==5 || phase==6 || phase==8 || phase==11 || phase==12;
  // Recovery/ragdoll transitions move the casualty. Approach before each new
  // success-case request; never move the medic to keep an active operation alive.
  if(requestPhase && phase<11 && player.GetPlayerId()==1 && m_BCMMOxygenApproachedPhase!=phase)
  {
   if(BCMM_SplintTreatment.IsBusy(own,target))return;
   vector approach[4];own.GetWorldTransform(approach);approach[3]=target.GetOrigin()+Vector(0,0.1,-1);ChimeraCharacter.Cast(own).Teleport(approach);
   m_BCMMOxygenApproachedPhase=phase;m_BCMMOxygenRequestAt=world.GetWorldTime()+2000;
   PrintFormat("BCMM_OXYGEN_FIXTURE_APPROACH phase=%1 distance=%2",phase,vector.Distance(own.GetOrigin(),target.GetOrigin()));return;
  }
  if(requestPhase && player.GetPlayerId()==1 && m_BCMMOxygenRequested<phase && world.GetWorldTime()>m_BCMMOxygenRequestAt)
  {
   // Wait for the nondefault authoritative GM snapshot before issuing treatment.
   if(mode.BCMM_ReadSetting("oxygenFlowLitresPerMinute")!=20) { return; }
   if(mode.BCMM_ReadSetting("paracetamolOnsetSeconds")!=10) { return; }
   if(mode.BCMM_ReadSetting("fentanylApplicationSeconds")!=3) { return; }
   if(mode.BCMM_ReadSetting("paracetamolApplicationSeconds")!=12) { return; }
   IEntity user=player.GetControlledEntity();
   if(!BCMM_TreatmentAccess.CanReach(user,target)) { return; }
   RplComponent owner=RplComponent.Cast(user.FindComponent(RplComponent));
   if(!owner || !owner.IsOwner()) { return; }
   if(phase==6 || phase==11 || phase==12)
   {
    CharacterControllerComponent stance=ChimeraCharacter.Cast(user).GetCharacterController();
    if(stance.GetStance()!=ECharacterStance.STAND) { stance.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOERECTED);return; }
   }
   BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
   if(!menu) { BCMM_MedicalMenu.Open(target);return; }
   if(phase>=11)
   {
    if(!menu.BCMM_TestSelfSelected()) { menu.BCMM_TestSwapToSelf();return; }
   }
   if(phase==2 || phase==8)
   {
    menu.BCMM_TestOxygenTab();menu.SelectRegion(BCMM_BodyRegion.HEAD);menu.UseEquipment(18);
   }
   else
   {
    menu.BCMM_TestMedicationTab();
    if(phase==6 || phase==11 || phase==12) { menu.SelectRegion(BCMM_BodyRegion.HEAD);menu.AdministerMedication(4); }
    else { menu.SelectRegion(BCMM_BodyRegion.LEFT_ARM);int drug=5;if(phase==5) { drug=1; }menu.AdministerMedication(drug); }
   }
   m_BCMMOxygenRequested=phase;
   PrintFormat("BCMM_OXYGEN_OWNER_SENT %1",phase);
  }
  if(m_BCMMOxygenSeenPhase!=phase) { m_BCMMOxygenSeenPhase=phase;m_BCMMOxygenPhaseSince=world.GetWorldTime(); }
  if(phase<=m_BCMMOxygenObserved || ((phase==6 || phase==11 || phase==12) && world.GetWorldTime()-m_BCMMOxygenPhaseSince<6000)) { return; }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));BCMM_CharacterPresentationComponent presentation=BCMM_CharacterPresentationComponent.Cast(target.FindComponent(BCMM_CharacterPresentationComponent));if(!medical || !presentation) { return; }
  bool ready;
  if(phase==1) { ready=medical.BCMM_OxygenSaturation()<0.65; }
  if(phase==2 || phase==8) { ready=(medical.BCMM_AppliedEquipment() & 64) && presentation.AppliedVisualSlot(12) && presentation.AppliedVisualsSafe() && medical.BCMM_OxygenSaturation()>0.7; }
  if(phase==9) { ready=!(medical.BCMM_AppliedEquipment() & 64) && !presentation.AppliedVisualSlot(12); }
  if(phase==3) { ready=mode.BCMM_ReadSetting("oxygenEnabled")==0 && !(medical.BCMM_AppliedEquipment() & 64) && !presentation.AppliedVisualSlot(12); }
  if(phase==4) { ready=medical.BCMM_RespiratoryRate()<12 && medical.BCMM_SystolicPressure()<115; }
  if(phase==5) { ready=medical.BCMM_RespiratoryRate()>15 && medical.BCMM_SystolicPressure()>115; }
  if(phase==6 || phase==11) { ready=medical.BCMM_RespiratoryRate()>15.9 && medical.BCMM_SystolicPressure()>119 && medical.GetAimingDamage()<0.02; }
  if(phase==12) { ready=medical.GetAimingDamage()>0.02 && medic && !BCMM_CPRAnimation.IsActive(medic,BCMM_FieldClips.TABLETSELF_GRAPH); }
  if(phase==7 || phase==10 || phase==13) { ready=!medical.BCMM_InCardiacArrest() && medical.BCMM_Pulse()==75 && medical.BCMM_SystolicPressure()==120 && !(medical.BCMM_AppliedEquipment() & 64); }
  if(!ready) { return; }m_BCMMOxygenObserved=phase;PrintFormat("BCMM_ARSENAL_BOXES_OBSERVED phase=%1",phase);PrintFormat("BCMM_OXYGEN_OBSERVED %1 oxygen=%2 rr=%3 pressure=%4",phase,medical.BCMM_OxygenSaturation(),medical.BCMM_RespiratoryRate(),medical.BCMM_SystolicPressure());
 }
}
modded class BCMM_MedicalMenu
{
 bool BCMM_TestSelfSelected() { return Target()==Player().GetControlledEntity(); }
 void BCMM_TestSwapToSelf() { SwapPatient(); }
 void BCMM_TestOxygenTab() { SelectTab(1); }
 void BCMM_TestMedicationTab() { SelectTab(5); }
}

