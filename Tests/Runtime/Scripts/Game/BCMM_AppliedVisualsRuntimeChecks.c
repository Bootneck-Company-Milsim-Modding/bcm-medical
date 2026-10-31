// Native applied-equipment presentation acceptance, independent of stretcher carrying.
class BCMM_AppliedVisualsRuntimeChecks : BCMM_SplintRuntimeChecks
{
 protected BCMM_CharacterPresentationComponent m_Presentation;
 protected int m_Retry;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 2 0");
  m_Actor.SetOrigin("100 2.2 0");m_Target.SetOrigin("101 2.2 0");m_Target.SetAngles("180 0 0");
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  ai=AIControlComponent.Cast(m_Target.FindComponent(AIControlComponent));if(ai) { ai.DeactivateAI(); }
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Presentation=BCMM_CharacterPresentationComponent.Cast(m_Target.FindComponent(BCMM_CharacterPresentationComponent));
  BCMM_Patient patient=m_Medical.BCMM_GetPatient();
  patient.Equipment.PadsAttached=true;patient.Equipment.VascularAccess=true;patient.Equipment.VascularAccessRegion=BCMM_BodyRegion.RIGHT_ARM;
  patient.LeftChest.OpenWound=true;patient.RightChest.OpenWound=true;patient.LeftChest.ApplySeal();patient.RightChest.ApplySeal();
  patient.LeftLeg.Broken=true;patient.LeftLeg.Splinted=true;patient.RightLeg.Broken=true;patient.RightLeg.Splinted=true;
  m_Medical.BCMM_SetPulseMonitor(true);BCMM_VisualStatusBridge.Publish(m_Medical);
  GetGame().GetCallqueue().CallLater(InspectApplied,2000);
 }
 protected void InspectApplied()
 {
  PrintFormat("BCMM_APPLIED_DEBUG flags=%1 iv=%2 count=%3",m_Medical.BCMM_AppliedEquipment(),m_Medical.BCMM_IVRegion(),m_Presentation.AppliedVisualCount());
  for(int slot=0;slot<12;slot++) { PrintFormat("BCMM_APPLIED_SLOT %1 active=%2",slot,m_Presentation.AppliedVisualSlot(slot)); }
  if(m_Presentation.AppliedVisualCount()!=8 && m_Retry++<4) { GetGame().GetCallqueue().CallLater(InspectApplied,1000);return; }
  array<string> probes={"RightKnee","RightFoot","RightForeArm","RightHand","LeftHandIndex1","LeftHandIndex3"};
  foreach(string probe:probes) { PrintFormat("BCMM_BONE_PROBE %1 index=%2",probe,m_Target.GetAnimation().GetBoneIndex(probe)); }
  m_Suite.Record(m_Medical.BCMM_AppliedEquipment()==63 && m_Medical.BCMM_IVRegion()==BCMM_BodyRegion.RIGHT_ARM,"Applied equipment flags preserve exact IV site");
  m_Suite.Record(m_Presentation.AppliedVisualCount()==8,"Native character displays two pads, two seals, two splints, cannula and pulse clip");
  m_Suite.Record(m_Presentation.AppliedVisualsSafe(),"Applied visuals have no inventory, replication or physics components");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("equipmentVisualChestDevicesEnabled",0);
  GetGame().GetCallqueue().CallLater(CheckFiltered,500);
 }
 protected void CheckFiltered()
 {
  m_Suite.Record(m_Presentation.AppliedVisualCount()==4 && !m_Presentation.AppliedVisualSlot(0) && m_Presentation.AppliedVisualSlot(6),"GM chest visual toggle preserves other applied equipment");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("equipmentVisualsEnabled",0);
  GetGame().GetCallqueue().CallLater(CheckDisabled,500);
 }
 protected void CheckDisabled()
 {
  m_Suite.Record(m_Presentation.AppliedVisualCount()==0,"Global GM visual toggle removes all local props");
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("equipmentVisualsEnabled",1);
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("equipmentVisualChestDevicesEnabled",1);
  m_Actor.SetOrigin("160 2 0");GetGame().GetCallqueue().CallLater(CheckRange,500);
 }
 protected void CheckRange()
 {
  m_Suite.Record(m_Presentation.AppliedVisualCount()==0,"Out-of-range patient clears applied visuals");
  m_Actor.SetOrigin("100 2 0");GetGame().GetCallqueue().CallLater(CheckRestored,500);
 }
 protected void CheckRestored()
 {
  m_Suite.Record(m_Presentation.AppliedVisualCount()==8,"Returning to range rebuilds equipment without another treatment");
  m_Medical.FullHeal();m_Medical.BCMM_SetPulseMonitor(false);
  GetGame().GetCallqueue().CallLater(CheckHealed,500);
 }
 protected void CheckHealed()
 {
  m_Suite.Record(m_Medical.BCMM_AppliedEquipment()==0 && m_Presentation.AppliedVisualCount()==0,"Native FullHeal clears clinical equipment and its cosmetics");
  Finish();
 }
}