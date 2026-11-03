// Isolated local visual test only; not gameplay content.
class BCMM_UIPreview
{
 protected static ref BCMM_UIPreview s_Preview;
 protected Widget m_HeadingPreview;
 protected ref BCMM_DomainChecks m_FluidUISuite;
 protected IEntity m_PreviewFluid;
 protected int m_FluidUIAttempts;
 protected int m_CheckCompletionPolls;
 static void Start()
 {
  s_Preview = new BCMM_UIPreview();
  GetGame().GetCallqueue().CallLater(s_Preview.Setup, 1500);
 }
 protected void Setup()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (!player) { Print("BCMM_UI_PREVIEW_NO_CONTROLLER", LogLevel.ERROR); return; }
  EntitySpawnParams spawn = new EntitySpawnParams();
  spawn.TransformMode = ETransformMode.WORLD;
  spawn.Transform[3] = "100 0 0";
  GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et"), GetGame().GetWorld(), spawn);
  spawn.Transform[3] = "100 1.2 0";
  IEntity actor = GetGame().SpawnEntityPrefab(Resource.Load("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et"), GetGame().GetWorld(), spawn);
  player.SetControlledEntity(actor);
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(actor.FindComponent(InventoryStorageManagerComponent));
  IEntity pack = GetGame().SpawnEntityPrefab(Resource.Load("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et"), GetGame().GetWorld(), spawn);
  IEntity dressing = GetGame().SpawnEntityPrefab(Resource.Load("{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et"), GetGame().GetWorld(), spawn);
  if (inventory && pack) inventory.TryInsertItem(pack, EStoragePurpose.PURPOSE_ANY);
  if (inventory && dressing) inventory.TryInsertItem(dressing, EStoragePurpose.PURPOSE_ANY);
  Print("BCMM_UI_PREVIEW_READY press H");
  string iconBoard;
  if (System.GetCLIParam("bcmmBeta01IconBoard", iconBoard)) { BCMM_Beta01IconBoard.Start(); return; }
  string progressReview;
  if (System.GetCLIParam("bcmmBeta01Progress", progressReview)) { BCMM_Beta01ProgressReview.Start(actor); return; }
  string knowledgeReview;
  if (System.GetCLIParam("bcmmBeta01Knowledge", knowledgeReview)) { BCMM_Beta01KnowledgeReview.Start(actor); return; }
  string bagReview;
  if (System.GetCLIParam("bcmmBeta01BagVisual", bagReview)) { BCMM_Beta01BagVisual.Start(); return; }
  string holdReview;
  if (System.GetCLIParam("bcmmBeta01HoldReview", holdReview)) { BCMM_Beta01HoldReview.Start(actor); return; }
  if (BCMM_Beta01ItemReview.Requested()) { BCMM_Beta01ItemReview.Start(actor); return; }
  string fluidUI;
  if(System.GetCLIParam("bcmmFluidUI",fluidUI))
  {
   m_FluidUISuite=new BCMM_DomainChecks();
   m_PreviewFluid=GetGame().SpawnEntityPrefab(Resource.Load("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et"),GetGame().GetWorld(),spawn);
   m_FluidUISuite.Record(m_PreviewFluid && inventory.TryInsertItem(m_PreviewFluid,EStoragePurpose.PURPOSE_ANY),"Native fluid UI fixture owns an actual finite saline bag");
   if(!m_PreviewFluid){m_FluidUISuite.Complete();return;}
   BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_PreviewFluid.FindComponent(BCMM_EquipmentItemComponent));
   m_FluidUISuite.Record(source && source.ConsumeFluidMl(125)==125 && source.FluidRemainingMl()==375,"Native fixture retains a physical three-hundred-seventy-five-millilitre bag");
   SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   float savedCapacity=mode.BCMM_ReadSetting("fluidBagVolumeMl");
   m_FluidUISuite.Record(mode.BCMM_TestRoleSetting("fluidBagVolumeMl",1000),"Live bag-capacity setting changes successfully on authority");
   IEntity fresh=GetGame().SpawnEntityPrefab(Resource.Load("{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et"),GetGame().GetWorld(),spawn);
   BCMM_EquipmentItemComponent freshSource;if(fresh)freshSource=BCMM_EquipmentItemComponent.Cast(fresh.FindComponent(BCMM_EquipmentItemComponent));
   m_FluidUISuite.Record(source.FluidCapacityMl()==500 && source.FluidRemainingMl()==375 && freshSource && freshSource.FluidCapacityMl()==1000 && freshSource.FluidRemainingMl()==1000,"Live capacity applies only to fresh physical bags and cannot refill a partial one");
   mode.BCMM_TestRoleSetting("fluidBagVolumeMl",savedCapacity);
   if(fresh)SCR_EntityHelper.DeleteEntityAndChildren(fresh);
   PrintFormat("BCMM_FLUID_UI_SETUP remaining=%1",source.FluidRemainingMl());
   BCMM_MedicalMenu.Open(actor);
   GetGame().GetCallqueue().CallLater(ConfigureFluidUI,250);
   return;
  }
  string review;
  if (System.GetCLIParam("bcmmUIReview", review)) BCMM_MedicalMenu.Open(actor);
  string smoke;
  if (System.GetCLIParam("bcmmUISmoke", smoke)) { BCMM_MedicalMenu.Open(actor); GetGame().GetCallqueue().CallLater(CheckPanel, 500); }
 }
 protected void ConfigureFluidUI()
 {
  BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();
  if(!menu && m_FluidUIAttempts++<20){GetGame().GetCallqueue().CallLater(ConfigureFluidUI,250);return;}
  m_FluidUISuite.Record(menu!=null,"Native medical menu finishes initialization before fluid selection");
  if(!menu){m_FluidUISuite.Complete();return;}
  menu.SelectRegion(BCMM_BodyRegion.LEFT_ARM);menu.BCMM_TestFluidTab();
  m_FluidUIAttempts=0;
  Print("BCMM_FLUID_UI_CONFIGURED");
  GetGame().GetCallqueue().CallLater(CheckFluidUI,1000);
 }
 protected bool ContainsVisibleText(Widget widget,string expected,int depth=0)
 {
  if(!widget || depth>64)
  {
   return false;
  }
  TextWidget text=TextWidget.Cast(widget);
  if(text && text.IsVisible() && text.GetText().Contains(expected))
  {
   return true;
  }
  Widget child=widget.GetChildren();
  int siblings;
  while(child && siblings<256)
  {
   if(ContainsVisibleText(child,expected,depth+1))
   {
    return true;
   }
   child=child.GetSibling();
   siblings++;
  }
  return false;
 }
 protected void CheckFluidUI()
 {
  BCMM_MedicalMenu menu=BCMM_MedicalMenu.Current();Widget root;if(menu)root=menu.GetRootWidget();
  PrintFormat("BCMM_FLUID_UI_CHECK attempt=%1 menu=%2",m_FluidUIAttempts,menu!=null);
  bool present=ContainsVisibleText(root,"375 mL in next bag");
  if(!present && m_FluidUIAttempts++<20){GetGame().GetCallqueue().CallLater(CheckFluidUI,250);return;}
  m_FluidUISuite.Record(menu && root && present,"Actual Fluids-tab action renders the partial bag's remaining volume");
  m_FluidUISuite.Record(!ContainsVisibleText(root,"500 mL in next bag"),"Partial supply UI does not falsely display a fresh full bag");
  m_FluidUISuite.Record(root && VisibleActions(root)==5,"Native fluid action rows remain within the five-action layout");
  System.MakeScreenshot("$profile:fluid-partial-ui");
  m_FluidUISuite.Complete();
 }
 protected int VisibleActions(Widget root)
 {
  int count;
  for (int i = 0; i < 6; i++) if (root.FindAnyWidget("Action" + i.ToString()).IsVisible()) count++;
  return count;
 }
 protected void Click(Widget root, string name)
 {
  SCR_ButtonTextComponent button = SCR_ButtonTextComponent.GetButtonText(name, root);
  if (button) button.m_OnClicked.Invoke(button);
 }
 protected void CheckPanel()
 {
  BCMM_DomainChecks suite = new BCMM_DomainChecks();
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  suite.Record(menu != null, "Native local medical panel opens");
  if (!menu) { suite.Complete(); return; }
  Widget root = menu.GetRootWidget();
  TextWidget rosterStatus=TextWidget.Cast(root.FindAnyWidget("RosterStatus"));
  float rosterWidth,rosterHeight,rosterTextWidth,rosterTextHeight;
  rosterStatus.GetScreenSize(rosterWidth,rosterHeight);rosterStatus.GetTextSize(rosterTextWidth,rosterTextHeight);
  rosterTextWidth=GetGame().GetWorkspace().DPIScale(rosterTextWidth);rosterTextHeight=GetGame().GetWorkspace().DPIScale(rosterTextHeight);
  PrintFormat("BCMM_UI_ROSTER_BOUNDS box=%1,%2 text=%3,%4 lines=%5",rosterWidth,rosterHeight,rosterTextWidth,rosterTextHeight,rosterStatus.GetNumLines());
  suite.Record(rosterTextWidth<=rosterWidth+1 && rosterTextHeight<=rosterHeight+1,"Complete native patient-list guidance fits its rendered text area");
  IEntity actor = GetGame().GetPlayerController().GetControlledEntity();
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(actor.FindComponent(SCR_CharacterDamageManagerComponent));
  suite.Record(!BCMM_MedicRole.IsMedic(actor) && BCMM_MedicRole.Duration(actor, 15) == 15, "Normal character keeps configured treatment time");
  medical.BCMM_SetMedic(true);
  suite.Record(BCMM_MedicRole.IsMedic(actor) && Math.AbsFloat(BCMM_MedicRole.Duration(actor, 15) - 9) < 0.001, "Assigned medic role reduces a fifteen second procedure to nine seconds");
  SCR_BaseGameMode mode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  mode.BCMM_TestRoleSetting("medicOnlyAED", 1);
  mode.BCMM_TestRoleSetting("medicOnlyFluids", 1);
  mode.BCMM_TestRoleSetting("medicOnlySurgery", 1);
  suite.Record(BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.SHOCK) && BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.INSERT_IV) && BCMM_MedicRole.Allows(actor, "medicOnlySurgery"), "Assigned medic retains restricted AED, fluids and surgery access");
  medical.BCMM_SetMedic(false);
  suite.Record(!BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.SHOCK) && !BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.INSERT_IV) && !BCMM_MedicRole.Allows(actor, "medicOnlySurgery") && BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.SEAL_LEFT), "Revoked role loses restricted advanced procedures while retaining basic care");
  mode.BCMM_TestRoleSetting("medicOnlyAED", 0);
  mode.BCMM_TestRoleSetting("medicOnlyFluids", 0);
  mode.BCMM_TestRoleSetting("medicOnlySurgery", 0);
  suite.Record(BCMM_MedicRole.EquipmentAllowed(actor, BCMM_EquipmentProcedure.SHOCK), "Disabling the GM restriction restores ordinary-character AED access");
  suite.Record(!BCMM_MedicRole.IsMedic(actor) && BCMM_MedicRole.Duration(actor, 60) == 60, "Revoking medic role restores standard duration");
  Widget heading = GetGame().GetWorkspace().CreateWidgets("{6AB4B3F3FD219201}UI/layouts/BCMM/SettingsHeading.layout");
  suite.Record(heading && heading.FindAnyWidget("SectionTitle") && heading.FindAnyWidget("SectionRule"), "Inline GM section layout loads with heading and separator");
  m_HeadingPreview = heading;
  Resource headingConfig = Resource.Load("{F3D6C6D25642352C}Configs/Editor/AttributeLists/Edit.conf");
  BaseContainerList attributes = headingConfig.GetResource().ToBaseContainer().GetObjectArray("m_aAttributes");
  for (int headingIndex = 0; headingIndex < attributes.Count(); headingIndex++)
   if (attributes.Get(headingIndex).GetClassName() == "BCMM_SectionHeadingAttribute")
   {
    SCR_BaseEditorAttribute attribute = SCR_BaseEditorAttribute.Cast(BaseContainerTools.CreateInstanceFromContainer(attributes.Get(headingIndex)));
    BCMM_SectionHeadingUI.Cast(heading.FindHandler(BCMM_SectionHeadingUI)).Init(heading, attribute);
    break;
   }
  suite.Record(TextWidget.Cast(root.FindAnyWidget("Assessment")).GetText() == "", "Panel initially reveals no findings");
  array<string> tabs = {"TabCirculation", "TabAirway", "TabSurgery", "TabMedication", "TabBlood", "SwapPatient"};
  float previousRight;
  bool valid = true;
  foreach (string name : tabs)
  {
   Widget tab = root.FindAnyWidget(name);
   if (!tab) { valid = false; continue; }
   TextWidget label = TextWidget.Cast(tab.FindAnyWidget("Text"));
   float x, y, width, height, textWidth, textHeight;
   tab.GetScreenPos(x, y); tab.GetScreenSize(width, height);
   if (!label) { valid = false; continue; }
   label.GetScreenSize(textWidth, textHeight);
   if (width <= 0 || height <= 0 || x < previousRight || label.GetText() == "" || textWidth <= 0 || textHeight <= 0) valid = false;
   previousRight = x + width;
  }
  suite.Record(valid, "Five native category buttons and top self/swap have labelled non-overlapping bounds");
  suite.Record(!root.FindAnyWidget("TabBleeding") && !root.FindAnyWidget("TabPain") && !root.FindAnyWidget("TabBreathing") && !root.FindAnyWidget("MorePatients"), "Obsolete categories and more-patients button are removed");
  suite.Record(TextWidget.Cast(root.FindAnyWidget("Close").FindAnyWidget("Text")).GetText() == "CLOSE MENU", "Close control uses a complete native menu label");
  Widget anatomy = root.FindAnyWidget("AnatomyPanel");
  Widget hint = root.FindAnyWidget("AnatomyHint");
  float bx, by, bw, bh, hx, hy, hw, hh;
  anatomy.GetScreenPos(bx, by); anatomy.GetScreenSize(bw, bh);
  hint.GetScreenPos(hx, hy); hint.GetScreenSize(hw, hh);
  suite.Record(bh > 0 && Math.AbsFloat(bw / bh - 0.6) < 0.02 && hy + hh <= by, "Anatomy keeps its supplied aspect and selection prompt stays above the image");
  menu.SelectRegion(BCMM_BodyRegion.LEFT_ARM);
  Click(root, "TabAirway");
  suite.Record(menu.SelectedRegion() == BCMM_BodyRegion.LEFT_ARM && VisibleActions(root) == 0, "Changing to Airway preserves the arm selection and exposes no inappropriate procedures");
  menu.SelectRegion(BCMM_BodyRegion.HEAD);
  suite.Record(VisibleActions(root) == 5 && TextWidget.Cast(root.FindAnyWidget("Action0").FindAnyWidget("Text")).GetText().Contains("airway") && TextWidget.Cast(root.FindAnyWidget("Action3").FindAnyWidget("Text")).GetText().Contains("oxygen") && TextWidget.Cast(root.FindAnyWidget("Action4").FindAnyWidget("Text")).GetText().Contains("oxygen"), "Head Airway offers its examination and context-specific equipment");
  Click(root, "TabSurgery");
  suite.Record(menu.SelectedRegion() == BCMM_BodyRegion.HEAD && TextWidget.Cast(root.FindAnyWidget("Action0").FindAnyWidget("Text")).GetText().Contains("injury"), "Surgery tab retains the selected head and provides injury examination");
  suite.Record(TextWidget.Cast(root.FindAnyWidget("PatientName")).GetText() != "SELF", "Current patient label displays identity rather than SELF");
  suite.Record(root.FindAnyWidget("RosterHeading") && root.FindAnyWidget("PatientScroll"), "Other vehicle occupants have a heading and scrollable list");
  menu.SelectRegion(BCMM_BodyRegion.LEFT_LEG);
  menu.CheckBleeding();
  GetGame().GetCallqueue().CallLater(CheckExaminationProgress, 600, false, suite);
  GetGame().GetCallqueue().CallLater(CheckTreatmentState, 2700, false, suite);
 }
 protected void CheckExaminationProgress(BCMM_DomainChecks suite)
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { suite.Record(false, "Panel remains open while checking"); return; }
  TextWidget heading = TextWidget.Cast(m_HeadingPreview.FindAnyWidget("SectionTitle"));
  float headingWidth, headingHeight; heading.GetScreenSize(headingWidth, headingHeight);
  suite.Record(headingWidth > 0 && headingHeight > 0 && heading.GetText() == "Bleeding", "GM inline heading has real rendered bounds and its actual section title");
  m_HeadingPreview.RemoveFromHierarchy(); m_HeadingPreview = null;
  Widget fill = menu.GetRootWidget().FindAnyWidget("CheckProgressFill");
  float width, height; fill.GetScreenSize(width, height);
  suite.Record(fill.IsVisible() && width > 0 && height > 0, "Server-acknowledged examination displays a progressing subtle bar");
 }
 protected void CheckTreatmentState(BCMM_DomainChecks suite)
 {
  BCMM_MedicalMenu menu = BCMM_MedicalMenu.Current();
  if (!menu) { suite.Record(false, "Panel remains open after checking"); suite.Complete(); return; }
  Widget root = menu.GetRootWidget();
  menu.BCMM_LogCheckTiming();
  // The check starts on its authority acknowledgement, not on the UI click.
  // Bound the wait by that acknowledged world-time deadline and a finite
  // polling limit; a missing result still fails instead of waiting forever.
  if(root.FindAnyWidget("CheckProgressFill").IsVisible() && menu.BCMM_CheckWithinDeadline() && m_CheckCompletionPolls++<20)
  {
   GetGame().GetCallqueue().CallLater(CheckTreatmentState,100,false,suite);return;
  }
  suite.Record(!root.FindAnyWidget("CheckProgressFill").IsVisible(), "Examination progress clears after the authoritative result");
  suite.Record(menu.BCMM_HasCheckResult(),"Authoritative examination supplies its finding before completion assertions");
  string bandageLabel = TextWidget.Cast(root.FindAnyWidget("Action1").FindAnyWidget("Text")).GetText();
  suite.Record(bandageLabel.Contains("Bandage /") && !bandageLabel.Contains("/ 0 remaining"), "Healthy-leg check has real available bandages");
  suite.Record(!root.FindAnyWidget("Action1").IsEnabled(), "Healthy checked leg disables unnecessary bandaging");
  menu.BandageSelectedRegion();
  suite.Record(BCMM_MedicalMenu.Current() == menu, "Unavailable bandaging does not close the panel");
  menu.HoverRegion(BCMM_BodyRegion.LEFT_LEG, true);
  suite.Record(root.FindAnyWidget("RegionHighlight5").GetOpacity() > 0.8, "Hovered body region remains visibly highlighted");
  menu.HoverRegion(BCMM_BodyRegion.LEFT_LEG, false);
  suite.Record(root.FindAnyWidget("RegionHighlight5").GetOpacity() > 0.6 && root.FindAnyWidget("RegionHighlight5").GetOpacity() < 0.8, "Selected body region keeps a distinct highlight");
  Widget carry=root.FindAnyWidget("TransportCarry");Widget drag=root.FindAnyWidget("TransportDrag");
  suite.Record(carry && drag && carry.IsVisible() && drag.IsVisible(),"Medical panel renders both living-transport controls");
  suite.Record(carry && drag && !carry.IsEnabled() && !drag.IsEnabled(),"Transport request controls are disabled when examining self");
  System.MakeScreenshot("$profile:medical-transport-ui");
  m_FluidUISuite=suite;
  GetGame().GetCallqueue().CallLater(menu.Close,300);
  GetGame().GetCallqueue().CallLater(suite.Complete,500);
 }
}


modded class SCR_BaseGameMode
{
 // Fixture-only configuration: no disk writes or production authorization bypass.
 bool BCMM_TestRoleSetting(string key, float value)
 {
  bool changed=m_BCMMSettings.Set(key,value);
  if(!changed){return false;}
  PrintFormat("BCMM_FIXTURE_SETTINGS before=%1 key=%2",m_BCMMSettingsRevision,key);
  BCMM_PublishSettings();
  PrintFormat("BCMM_FIXTURE_SETTINGS after=%1 key=%2",m_BCMMSettingsRevision,key);
  return true;
 }

}
modded class BCMM_MedicalMenu
{
 bool BCMM_CheckWithinDeadline(){return GetGame().GetWorld().GetWorldTime()*0.001-m_CheckStart<m_CheckSeconds+1;}
 bool BCMM_HasCheckResult(){int region=BCMM_BodyRegion.LEFT_LEG;int check=BCMM_ClinicalCheck.BLEEDING;return !m_CheckActive && m_Findings.Contains(region.ToString()+":"+check.ToString());}
 void BCMM_LogCheckTiming()
 {
  PrintFormat("BCMM_UI_CHECK_TIMING active=%1 elapsed=%2 duration=%3 fill=%4 findings=%5",m_CheckActive,GetGame().GetWorld().GetWorldTime()*0.001-m_CheckStart,m_CheckSeconds,GetRootWidget().FindAnyWidget("CheckProgressFill").IsVisible(),m_Findings.Count());
 }
}
