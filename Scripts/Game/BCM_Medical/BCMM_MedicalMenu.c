modded enum ChimeraMenuPreset { BCMM_MedicalPanel = 684001 }
class BCMM_AnatomyRegion : SCR_ScriptedWidgetComponent
{
 [Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(BCMM_BodyRegion))] protected int m_Region;
 override bool OnMouseEnter(Widget w, int x, int y)
 {
  if (BCMM_MedicalMenu.Current()) BCMM_MedicalMenu.Current().HoverRegion(m_Region, true);
  return true;
 }
 override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
 {
  if (BCMM_MedicalMenu.Current()) BCMM_MedicalMenu.Current().HoverRegion(m_Region, false);
  return true;
 }
 override bool OnClick(Widget w, int x, int y, int button)
 {
  if (button != 0 || !BCMM_MedicalMenu.Current()) return false;
  BCMM_MedicalMenu.Current().SelectRegion(m_Region);
  return true;
 }
}
class BCMM_MedicalMenu : ChimeraMenuBase
{
 protected static BCMM_MedicalMenu s_Open;
 protected static RplId s_RequestedTarget = RplId.Invalid();
 protected static int s_NextView;
 protected RplId m_Target = RplId.Invalid();
 protected int m_View;
 protected int m_Region = -1;
 protected int m_Tab = 3;
 protected RplId m_SwapTarget = RplId.Invalid();
 protected ref array<ref BCMM_PatientRow> m_RosterRows = {};
 protected bool m_CheckActive;
 protected float m_CheckStart;
 protected float m_CheckSeconds;
 protected int m_CheckRegion;
 protected int m_CheckKind;
 protected float m_PatientRefreshAt;
 protected int m_HoverRegion = -1;
 protected ref map<int, int> m_TreatmentFlags = new map<int, int>();
 protected ref array<IEntity> m_Candidates = {};
 protected ref array<int> m_Actions = {};
 protected ref array<string> m_ActionLabels = {};
 protected ref array<bool> m_ActionEnabled = {};
 protected int m_ActionPage;
 protected ref array<int> m_Supplies = {};
 protected ref map<string, string> m_Findings = new map<string, string>();
 static BCMM_MedicalMenu Current() { return s_Open; }
 int SelectedRegion() { return m_Region; }
 static void Open(IEntity target)
 {
  if (!target) return;
  s_RequestedTarget = BCMM_BodyBagComponent.Id(target);
  if (s_Open) s_Open.Close();
  GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.BCMM_MedicalPanel);
 }
 // Legacy hands-only reply cannot populate this check-driven interface.
 static bool ReceiveAssessment(RplId target, string summary) { return s_Open != null; }
 static void ReceiveClinicalCheck(RplId target, int view, int region, int check, bool completed, string result, array<int> counts)
 {
  if (!s_Open || s_Open.m_Target != target || s_Open.m_View != view) return;
  if (counts && counts.Count() >= BCMM_SupplyCatalog.COUNT)
  {
   for (int supply = 0; supply < BCMM_SupplyCatalog.COUNT; supply++) s_Open.m_Supplies[supply] = counts[supply];
   if (completed && (check == BCMM_ClinicalCheck.BLEEDING || check == BCMM_ClinicalCheck.INJURY) && counts.Count() == BCMM_SupplyCatalog.COUNT + 1) s_Open.m_TreatmentFlags.Set(region, counts[BCMM_SupplyCatalog.COUNT]);
  }
  if (check != BCMM_ClinicalCheck.SUPPLIES)
  {
   if (s_Open.m_CheckActive && s_Open.m_CheckRegion == region && s_Open.m_CheckKind == check) s_Open.m_CheckActive = false;
   if (completed)
   {
    s_Open.m_Findings.Set(region.ToString() + ":" + check.ToString(), result);
    if (s_Open.m_Region == region) s_Open.SetText("RegionLabel", BCMM_ClinicalChecks.RegionName(region));
   }
   else s_Open.SetText("RegionLabel", result);
  }
  s_Open.RefreshFindings();
  s_Open.RefreshActions();
 }
 protected SCR_PlayerController Player() { return SCR_PlayerController.Cast(GetGame().GetPlayerController()); }
 protected IEntity Target() { return BCMM_BodyBagComponent.Resolve(m_Target); }
 protected void SetText(string name, string value)
 {
  TextWidget text = TextWidget.Cast(GetRootWidget().FindAnyWidget(name));
  if (text) text.SetText(value);
 }
 protected SCR_ButtonTextComponent Bind(string name, string label)
 {
  SCR_ButtonTextComponent button = SCR_ButtonTextComponent.GetButtonText(name, GetRootWidget());
  if (button) button.SetText(label);
  Widget root = GetRootWidget().FindAnyWidget(name);
  if (root)
  {
   TextWidget text = TextWidget.Cast(root.FindAnyWidget("Text"));
   if (text)
   {
    text.SetMinFontSize(14);text.SetDesiredFontSize(16);
    if(name.StartsWith("Transport")){text.SetMinFontSize(10);text.SetDesiredFontSize(12);}
   }
  }
  return button;
 }
 override void OnMenuOpen()
 {
  super.OnMenuOpen();
  s_Open = this;
  m_Supplies.Resize(BCMM_SupplyCatalog.COUNT);
  m_Target = s_RequestedTarget;
  if (Player() && Target() != Player().GetControlledEntity()) m_SwapTarget = m_Target;
  m_View = ++s_NextView;
  BindAll();
  TextWidget anatomyHint = TextWidget.Cast(GetRootWidget().FindAnyWidget("AnatomyHint"));
  if (anatomyHint) { anatomyHint.SetTextWrapping(true); anatomyHint.SetMinFontSize(10); anatomyHint.SetDesiredFontSize(12); }
  TextWidget rosterStatus = TextWidget.Cast(GetRootWidget().FindAnyWidget("RosterStatus"));
  if (rosterStatus) { rosterStatus.SetTextWrapping(true); rosterStatus.SetMinFontSize(11); rosterStatus.SetDesiredFontSize(13); }
  GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, ClosePanel);
  TextWidget assessmentText = TextWidget.Cast(GetRootWidget().FindAnyWidget("Assessment"));
  if (assessmentText) { assessmentText.SetMinFontSize(12); assessmentText.SetDesiredFontSize(16); assessmentText.SetTextWrapping(true); }
  TextWidget transportHint=TextWidget.Cast(GetRootWidget().FindAnyWidget("TransportStatus"));
  if(transportHint){transportHint.SetTextWrapping(true);transportHint.SetMinFontSize(10);transportHint.SetDesiredFontSize(12);}
  SetText("Assessment", "");
  SetText("RegionLabel", "Select body region");
  for (int i = 0; i < 7; i++)
  {
   Widget highlight = GetRootWidget().FindAnyWidget("RegionHighlight" + i.ToString());
   if (highlight) highlight.SetVisible(false);
  }
  FillPatients();
  RefreshActions();
  if (Player() && Target()) Player().BCMM_RequestKnownChecks(Target(), m_View);
 }
 override void OnMenuClose()
 {
  GetGame().GetInputManager().RemoveActionListener("MenuBack", EActionTrigger.DOWN, ClosePanel);
  foreach (BCMM_PatientRow row : m_RosterRows) row.Release();
  m_RosterRows.Clear();
  if (s_Open == this) s_Open = null;
  super.OnMenuClose();
 }
 override void OnMenuUpdate(float tDelta)
 {
  super.OnMenuUpdate(tDelta);
  // The menu preset owns a context containing the rebindable medical toggle.
  if (GetGame().GetWorld().GetWorldTime() >= m_PatientRefreshAt)
  {
   FillPatients();
   RefreshFindings();
   RefreshDiagram();
   RefreshSupplies();
   m_PatientRefreshAt = GetGame().GetWorld().GetWorldTime() + 1000;
  }
  if (Player())
  {
   bool active = Player().BCMM_HasProcedure();
   if (active) SetText("TreatmentStatus", Player().BCMM_ProcedureStatus());
   else
   {
    SCR_CharacterDamageManagerComponent dripStatus;if(Target())dripStatus=SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
    if(dripStatus && dripStatus.BCMM_FluidLineActive())SetText("TreatmentStatus",string.Format("Drip flowing / %1 mL remaining",Math.Ceil(dripStatus.BCMM_FluidLineRemainingMl())));
    else SetText("TreatmentStatus", "Ready");
   }
   Widget cancel = GetRootWidget().FindAnyWidget("CancelProcedure");
   if (cancel) { cancel.SetVisible(active); cancel.SetEnabled(active); }
   Widget fill = GetRootWidget().FindAnyWidget("ProcedureFill");
   Widget panel = GetRootWidget().FindAnyWidget("Panel");
   if (fill && panel)
   {
    float width, height;
    panel.GetScreenSize(width, height);
    FrameSlot.SetSize(fill, GetGame().GetWorkspace().DPIUnscale(width * 0.665 * Player().BCMM_ProcedureProgress()), 8);
   }
  }
  for (int row = 0; row < 6; row++)
  {
   int actionIndex = m_ActionPage * 6 + row;
   Widget button = GetRootWidget().FindAnyWidget("Action" + row.ToString());
   if (!button || !m_Actions.IsIndexValid(actionIndex)) continue;
   bool enabled = ActionAllowed(actionIndex);
   if (button.IsEnabled() != enabled) button.SetEnabled(enabled);
  }
  FitAnatomy();
  RefreshTransport();
  UpdateCheckProgress();
 }
 protected void BindAll()
 {
  SCR_ButtonTextComponent b;
  b = Bind("Close", "CLOSE MENU"); if (b) b.m_OnClicked.Insert(ClosePanel);
  b = Bind("SwapPatient", "SELF / SWAP"); if (b) b.m_OnClicked.Insert(SwapPatient);
  b = Bind("MoreActions", "MORE TREATMENTS"); if (b) b.m_OnClicked.Insert(MoreActions);
  b = Bind("CancelProcedure", "CANCEL TREATMENT"); if (b) b.m_OnClicked.Insert(CancelProcedure);
  b = Bind("TransportCarry", "CARRY"); if (b) b.m_OnClicked.Insert(TransportCarry);
  b = Bind("TransportDrag", "DRAG"); if (b) b.m_OnClicked.Insert(TransportDrag);
  b = Bind("TabCirculation", "CIRCULATION"); if (b) b.m_OnClicked.Insert(TabCirculation);
  b = Bind("TabAirway", "AIRWAY"); if (b) b.m_OnClicked.Insert(TabAirway);
  b = Bind("TabSurgery", "SURGERY"); if (b) b.m_OnClicked.Insert(TabSurgery);
  b = Bind("TabMedication", "MEDICATION"); if (b) b.m_OnClicked.Insert(TabMedication);
  b = Bind("TabBlood", "FLUIDS"); if (b) b.m_OnClicked.Insert(TabBlood);
  b = Bind("Action0", ""); if (b) b.m_OnClicked.Insert(Action0);
  b = Bind("Action1", ""); if (b) b.m_OnClicked.Insert(Action1);
  b = Bind("Action2", ""); if (b) b.m_OnClicked.Insert(Action2);
  b = Bind("Action3", ""); if (b) b.m_OnClicked.Insert(Action3);
  b = Bind("Action4", ""); if (b) b.m_OnClicked.Insert(Action4);
  b = Bind("Action5", ""); if (b) b.m_OnClicked.Insert(Action5);
  b = Bind("Action6", ""); if (b) b.m_OnClicked.Insert(Action6);
  b = Bind("Action7", ""); if (b) b.m_OnClicked.Insert(Action7);
  b = Bind("Action8", ""); if (b) b.m_OnClicked.Insert(Action8);
  b = Bind("Action9", ""); if (b) b.m_OnClicked.Insert(Action9);
 }
 void SelectRegion(int region)
 {
  if (region < 0 || region > 6) return;
  m_Region = region;
  m_ActionPage = 0;
  SetText("AnatomyHint", "SELECTED: " + BCMM_ClinicalChecks.RegionName(region));
  SetText("RegionLabel", BCMM_ClinicalChecks.RegionName(region));
  for (int i = 0; i < 7; i++)
  {
   Widget highlight = GetRootWidget().FindAnyWidget("RegionHighlight" + i.ToString());
   if (highlight) { highlight.SetVisible(true); highlight.SetOpacity(0.08); if (i == region) highlight.SetOpacity(0.65); }
  }
  RefreshFindings();
  RefreshActions();
  if (Player()) Player().BCMM_RequestClinicalCheck(Target(), region, BCMM_ClinicalCheck.SUPPLIES, m_View);
 }
 protected void RefreshTransport()
 {
  if(!Player())return;
  bool incoming=Player().BCMM_TransportIncoming();int token=Player().BCMM_TransportOfferToken();
  bool active=BCMM_LivingTransport.ForParticipant(Player().GetControlledEntity())!=null;
  string carry="CARRY";string drag="DRAG";string status="Look at a nearby casualty to request transport.";
  bool canCarry=BCMM_LivingTransport.Eligible(Player().GetControlledEntity(),Target(),1);
  bool canDrag=BCMM_LivingTransport.Eligible(Player().GetControlledEntity(),Target(),2);
  string transportName="carry";if(Player().BCMM_TransportOfferMode()==2)transportName="drag";
  if(incoming){carry="ACCEPT";drag="DECLINE";canCarry=true;canDrag=true;status="Incoming "+transportName+" request. Accept or decline.";}
  else if(token>0){carry="CANCEL";canCarry=true;canDrag=false;status="Waiting for the other player to accept.";}
  else if(active){carry="RELEASE";canCarry=true;canDrag=false;status="Transport active. Either participant can release.";}
  else if(canCarry || canDrag)status="Conscious players must accept the request.";
  Bind("TransportCarry",carry);Bind("TransportDrag",drag);SetText("TransportStatus",status);
  Widget c=GetRootWidget().FindAnyWidget("TransportCarry");if(c)c.SetEnabled(canCarry);
  Widget d=GetRootWidget().FindAnyWidget("TransportDrag");if(d)d.SetEnabled(canDrag);
  Widget decline=GetRootWidget().FindAnyWidget("TransportDecline");if(decline)decline.SetVisible(false);
 }
 protected void TransportCarry()
 {
  if(!Player())return;
  if(Player().BCMM_TransportIncoming())Player().BCMM_RequestLivingTransport(null,3,Player().BCMM_TransportOfferToken());
  else if(Player().BCMM_TransportOfferToken()>0)Player().BCMM_RequestLivingTransport(null,5,Player().BCMM_TransportOfferToken());
  else if(BCMM_LivingTransport.ForParticipant(Player().GetControlledEntity()))Player().BCMM_RequestLivingTransport(null,6);
  else Player().BCMM_RequestLivingTransport(Target(),1);
 }
 protected void TransportDrag()
 {
  if(!Player())return;
  if(Player().BCMM_TransportIncoming())Player().BCMM_RequestLivingTransport(null,4,Player().BCMM_TransportOfferToken());
  else Player().BCMM_RequestLivingTransport(Target(),2);
 }
 protected void TransportDecline(){TransportDrag();}
 protected bool FindingVisible(int check)
 {
  if (m_Tab == 3) return check == 1 || check == 4 || check == 8;
  if (m_Tab == 1) return check == 2 || check == 3;
  if (m_Tab == 7) return check == 9;
  if (m_Tab == 5) return check == 6 || check == 7;
  return m_Tab == 6 && check == 5;
 }
 protected void RefreshFindings()
 {
  string text;
  if (m_Region >= 0) for (int check = 1; check <= 9; check++)
  {
   if (!FindingVisible(check)) continue;
   string finding;
   if (m_Findings.Find(m_Region.ToString() + ":" + check.ToString(), finding))
   {
    if (!text.IsEmpty()) text += "\n";
    text += finding;
   }
  }
  SCR_CharacterDamageManagerComponent monitor = null;
  if (Target()) monitor = SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
  if (m_Tab == 3 && monitor && monitor.BCMM_PulseMonitorActive())
  {
   if (!text.IsEmpty()) text += "\n";
   int pulse = Math.Round(monitor.BCMM_Pulse());
   int saturation = Math.Round(monitor.BCMM_OxygenSaturation() * 100);
   text += string.Format("LIVE PULSE OXIMETER\nPulse %1 bpm\nSpO2 %2 percent", pulse, saturation);
  }
  SetText("Assessment", text);
 }
 protected void AddAction(int action, string label, bool enabled = true)
 {
  m_Actions.Insert(action);
  m_ActionLabels.Insert(label);
  m_ActionEnabled.Insert(enabled);
 }
 protected void DrawActions()
 {
  for (int row = 0; row < 10; row++)
  {
   Widget button = GetRootWidget().FindAnyWidget("Action" + row.ToString());
   if (!button) continue;
   int index = m_ActionPage * 6 + row;
   bool visible = row < 6 && m_Actions.IsIndexValid(index);
   button.SetVisible(visible);
   if (!visible) continue;
   Bind("Action" + row.ToString(), m_ActionLabels[index]);
   button.SetEnabled(ActionAllowed(index));
  }
  int pages = Math.Max(1, Math.Ceil(m_Actions.Count() / 6.0));
  Widget more = GetRootWidget().FindAnyWidget("MoreActions");
  if (more) { more.SetVisible(pages > 1); more.SetEnabled(pages > 1); }
  if (pages > 1) SetText("ActionPage", string.Format("Page %1 / %2", m_ActionPage + 1, pages));
  else SetText("ActionPage", "");
  array<string> tabs = {"TabCirculation", "TabAirway", "TabSurgery", "TabMedication", "TabBlood"};
  array<int> tabIDs = {3, 1, 7, 5, 6};
  for (int t = 0; t < tabs.Count(); t++)
  {
   SCR_ButtonTextComponent tab = SCR_ButtonTextComponent.GetButtonText(tabs[t], GetRootWidget());
   if (tab) tab.SetToggled(m_Tab == tabIDs[t], false, false, true);
  }
 }
 void RefreshDiagram()
 {
  SCR_CharacterDamageManagerComponent medical = null;
  if (Target()) medical = SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
  for (int region = 0; region < 7; region++)
  {
   int flags = 0;
   if (medical) flags = medical.BCMM_RegionVisual(region);
   string label = "";
   if (flags & 1) label = "F";
   if (flags & 2) label += " S";
   if (flags & 16) label += " TQ";
   if ((flags & 4) && label.IsEmpty()) label = "B";
   if ((flags & 8) && label.IsEmpty()) label = "ST";
   SetText("RegionMarker" + region.ToString(), label);
   TextWidget badge = TextWidget.Cast(GetRootWidget().FindAnyWidget("RegionMarker" + region.ToString()));
   if (badge)
   {
    badge.SetColor(Color.FromInt(0xFF337FBB));
    if (flags & 1) badge.SetColor(Color.FromInt(0xFFCF4949));
    if (flags & 2) badge.SetColor(Color.FromInt(0xFFB98420));
    if (flags & 16) badge.SetColor(Color.FromInt(0xFF884EA0));
   }
  }
 }
 protected string DiagramCaption(int region)
 {
  string caption = BCMM_ClinicalChecks.RegionName(region);
  if (!Target()) return caption;
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical) return caption;
  int flags = medical.BCMM_RegionVisual(region);
  string status;
  if (flags & 1) status = "Fracture";
  if (flags & 2) status += " / Splinted";
  if (flags & 16) status += " / Tourniquet";
  if (status.IsEmpty() && (flags & 4)) status = "Dressed";
  if (status.IsEmpty() && (flags & 8)) status = "Sutured";
  if (!status.IsEmpty()) caption += "\n" + status;
  return caption;
 }
 void HoverRegion(int region, bool hovering)
 {
  if (hovering) m_HoverRegion = region;
  else if (m_HoverRegion == region) m_HoverRegion = -1;
  for (int i = 0; i < 7; i++)
  {
   Widget highlight = GetRootWidget().FindAnyWidget("RegionHighlight" + i.ToString());
   if (!highlight) continue;
   highlight.SetVisible(true);
   highlight.SetOpacity(0.08);
   if (i == m_Region) highlight.SetOpacity(0.65);
   if (i == m_HoverRegion) highlight.SetOpacity(0.9);
  }
  if (m_HoverRegion >= 0) SetText("AnatomyHint", DiagramCaption(m_HoverRegion));
  else if (m_Region >= 0) SetText("AnatomyHint", DiagramCaption(m_Region));
  else SetText("AnatomyHint", "CLICK A BODY PART");
 }
 protected bool KnownTreatment(int action)
 {
  int flags;
  if (!m_TreatmentFlags.Find(m_Region, flags)) return false;
  int bit = 1 << (action - 10);
  return (flags & bit) != 0;
 }
 protected bool ActionAllowed(int index)
 {
  if (!m_Actions.IsIndexValid(index)) return false;
  int availabilityAction=m_Actions[index];bool available=m_ActionEnabled[index];
  SCR_CharacterDamageManagerComponent lineStatus;if(Target())lineStatus=SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
  if(availabilityAction==40+BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE)available=lineStatus && lineStatus.BCMM_FluidLineActive() && lineStatus.BCMM_FluidLineRegion()==m_Region;
  if(availabilityAction>=40 && BCMM_EquipmentPolicy.IsLineConnection(availabilityAction-40))available=available && lineStatus && !lineStatus.BCMM_FluidLineActive() && (lineStatus.BCMM_AppliedEquipment()&32) && lineStatus.BCMM_IVRegion()==m_Region && BCMM_TreatmentAccess.Setting("continuousFluidLinesEnabled")>0.5;
  if(availabilityAction>=40 && BCMM_EquipmentPolicy.IsInfusion(availabilityAction-40) && lineStatus && lineStatus.BCMM_FluidLineActive())available=false;
  if(availabilityAction>=40 && lineStatus && lineStatus.BCMM_WaterLoadingWithheld())
  {
   int sourceKind=BCMM_EquipmentPolicy.Kind(availabilityAction-40);
   if(sourceKind==BCMM_EquipmentKind.SALINE || sourceKind==BCMM_EquipmentKind.PLASMA){available=false;}
  }
  if(!available)return false;
  if (!ActionInTab(m_Actions[index])) return false;
  if (m_Actions[index] == 17) return true;
  if (m_CheckActive) return false;
  if (m_Actions[index] < 10) return Player() && BCMM_TreatmentAccess.CanExamine(Player().GetControlledEntity(), Target());
  if (m_Actions[index] >= 10 && m_Actions[index] <= 15 && !KnownTreatment(m_Actions[index])) return false;
  if (!Player() || Player().BCMM_HasProcedure()) return false;
  ChimeraCharacter actor = ChimeraCharacter.Cast(Player().GetControlledEntity());
  if (!actor || actor.GetCharacterController().IsUsingItem()) return false;
  int action = m_Actions[index];
  if (action >= 12 && action <= 15 && !BCMM_MedicRole.Allows(actor, "medicOnlySurgery")) return false;
  if (action >= 20 && action <= 25 && !BCMM_MedicRole.Allows(actor, "medicOnlyMedication")) return false;
  if (action==25 && !BCMM_MedicRole.Allows(actor,"fentanylMedicOnly")) return false;
  if (action == 23 && !BCMM_MedicRole.Allows(actor, "txaMedicOnly")) return false;
  if (action >= 40 && !BCMM_MedicRole.EquipmentAllowed(actor, action - 40)) return false;
  return BCMM_TreatmentAccess.CanReach(Player().GetControlledEntity(), Target());
 }
 protected void MoreActions()
 {
  int pages = Math.Max(1, Math.Ceil(m_Actions.Count() / 6.0));
  m_ActionPage = (m_ActionPage + 1) % pages;
  DrawActions();
 }
 protected void SelectTab(int tab)
 {
  if (tab != 1 && tab != 3 && tab != 5 && tab != 6 && tab != 7) return;
  m_Tab = tab;
  RefreshFindings();
  m_ActionPage = 0;
  RefreshActions();
 }
 protected void FitAnatomy()
 {
  Widget panel = GetRootWidget().FindAnyWidget("Panel");
  Widget anatomy = GetRootWidget().FindAnyWidget("AnatomyPanel");
  if (!panel || !anatomy) return;
  float width, height;
  panel.GetScreenSize(width, height);
  float bodyWidth = Math.Min(width * 0.26, height * 0.52 * 0.6);
  FrameSlot.SetSize(anatomy, GetGame().GetWorkspace().DPIUnscale(bodyWidth), GetGame().GetWorkspace().DPIUnscale(bodyWidth / 0.6));
  FrameSlot.SetAlignment(anatomy, 0.5, 0.5);
 }
 protected bool ActionInTab(int action)
 {
  if (action == 1 || action == 4 || action == 10 || action == 16 || action == 17) return m_Tab == 3 && (action != 4 || BCMM_ClinicalChecks.Supports(m_Region, 4)) && ((action != 16 && action != 17) || m_Region == BCMM_BodyRegion.CHEST);
  if (action == 2) return m_Tab == 1 && m_Region == BCMM_BodyRegion.HEAD;
  if (action == 3) return m_Tab == 1 && m_Region == BCMM_BodyRegion.CHEST;
  if (action == 9 || (action >= 11 && action <= 15)) return m_Tab == 7;
  if (action == 6 || action == 7 || (action >= 20 && action <= 25)) return m_Tab == 5;
  if (action == 5) return m_Tab == 6;
  if (action >= 40 && action <= 40 + BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE)
  {
   int equipment = action - 40;
   if (equipment == BCMM_EquipmentProcedure.PULSE_OXIMETER) return m_Tab == 3 && (m_Region == BCMM_BodyRegion.LEFT_ARM || m_Region == BCMM_BodyRegion.RIGHT_ARM);
   if (equipment==BCMM_EquipmentProcedure.CONNECT_OXYGEN || equipment==BCMM_EquipmentProcedure.DISCONNECT_OXYGEN) return m_Tab==1 && m_Region==BCMM_BodyRegion.HEAD;
   if (equipment == BCMM_EquipmentProcedure.INSERT_AIRWAY || equipment == BCMM_EquipmentProcedure.SUCTION) return m_Tab == 1 && m_Region == BCMM_BodyRegion.HEAD;
   if (BCMM_EquipmentPolicy.IsLineConnection(equipment) || equipment==BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE || equipment == BCMM_EquipmentProcedure.INSERT_IV || BCMM_EquipmentPolicy.IsInfusion(equipment)) return m_Tab == 6 && m_Region >= BCMM_BodyRegion.LEFT_ARM;
   if (equipment == BCMM_EquipmentProcedure.APPLY_PADS || equipment == BCMM_EquipmentProcedure.ANALYSE || equipment == BCMM_EquipmentProcedure.SHOCK || equipment == BCMM_EquipmentProcedure.MONITOR) return m_Tab == 3 && m_Region == BCMM_BodyRegion.CHEST;
   return m_Tab == 1 && m_Region == BCMM_BodyRegion.CHEST;
  }
  return false;
 }
 static void BeginCheckProgress(RplId patient, int view, int region, int check, float seconds)
 {
  if (!s_Open || s_Open.m_Target != patient || s_Open.m_View != view || seconds <= 0) return;
  s_Open.m_CheckActive = true; s_Open.m_CheckRegion = region; s_Open.m_CheckKind = check;
  s_Open.m_CheckStart = GetGame().GetWorld().GetWorldTime() * 0.001; s_Open.m_CheckSeconds = seconds;
 }
 protected void UpdateCheckProgress()
 {
  Widget fill = GetRootWidget().FindAnyWidget("CheckProgressFill");
  Widget panel = GetRootWidget().FindAnyWidget("Panel");
  if (!fill || !panel) return;
  fill.SetVisible(m_CheckActive);
  if (!m_CheckActive) return;
  float fraction = Math.Clamp((GetGame().GetWorld().GetWorldTime() * 0.001 - m_CheckStart) / m_CheckSeconds, 0, 1);
  float width, height; panel.GetScreenSize(width, height);
  FrameSlot.SetSize(fill, GetGame().GetWorkspace().DPIUnscale(width * 0.665 * fraction), 2);
 }
 protected void RefreshActions()
 {
  m_Actions.Clear();
  m_ActionLabels.Clear();
  m_ActionEnabled.Clear();
  for (int i = 0; i < 10; i++)
  {
   Widget button = GetRootWidget().FindAnyWidget("Action" + i.ToString());
   if (button) button.SetVisible(false);
  }
  if (m_Region < 0) { DrawActions(); return; }
  if (m_Tab == 3)
  {
   AddAction(1, "Check bleeding");
   if (BCMM_ClinicalChecks.Supports(m_Region, 4)) AddAction(4, "Check pulse / heart rate");
   if (m_Region == BCMM_BodyRegion.LEFT_ARM || m_Region == BCMM_BodyRegion.RIGHT_ARM) EquipmentAction(BCMM_EquipmentProcedure.PULSE_OXIMETER);
   AddAction(10, "Bandage / " + m_Supplies[0].ToString() + " remaining", m_Supplies[0] > 0);
   if (m_Region == BCMM_BodyRegion.CHEST)
   {
    EquipmentAction(BCMM_EquipmentProcedure.APPLY_PADS);
    EquipmentAction(BCMM_EquipmentProcedure.ANALYSE);
    EquipmentAction(BCMM_EquipmentProcedure.SHOCK);
    EquipmentAction(BCMM_EquipmentProcedure.MONITOR);
    if (Player() && Player().BCMM_Compressing(Target())) AddAction(17, "Stop CPR");
    else AddAction(16, "Start CPR");
   }
  }
  if (m_Tab == 1 && m_Region == BCMM_BodyRegion.HEAD)
  {
   AddAction(2, "Check airway");
   EquipmentAction(BCMM_EquipmentProcedure.INSERT_AIRWAY);
   EquipmentAction(BCMM_EquipmentProcedure.SUCTION);
   EquipmentAction(BCMM_EquipmentProcedure.CONNECT_OXYGEN);
   EquipmentAction(BCMM_EquipmentProcedure.DISCONNECT_OXYGEN);
  }
  if (m_Tab == 1 && m_Region == BCMM_BodyRegion.CHEST)
  {
   AddAction(3, "Check breathing / respiratory rate");
   EquipmentAction(BCMM_EquipmentProcedure.SEAL_LEFT);
   EquipmentAction(BCMM_EquipmentProcedure.SEAL_RIGHT);
   EquipmentAction(BCMM_EquipmentProcedure.NCD_LEFT);
   EquipmentAction(BCMM_EquipmentProcedure.NCD_RIGHT);
   EquipmentAction(BCMM_EquipmentProcedure.DRAIN_LEFT);
   EquipmentAction(BCMM_EquipmentProcedure.DRAIN_RIGHT);
   EquipmentAction(BCMM_EquipmentProcedure.VENTILATE);
  }
  if (m_Tab == 7)
  {
   AddAction(9, "Examine injury");
   if (m_Region == BCMM_BodyRegion.LEFT_LEG || m_Region == BCMM_BodyRegion.RIGHT_LEG)
   {
    AddAction(11, "Apply splint / " + m_Supplies[1].ToString(), m_Supplies[1] > 0);
    AddAction(14, "Definitive fracture surgery", m_Supplies[2] > 0);
   }
   AddAction(12, "Explore / remove projectile", m_Supplies[2] > 0);
   AddAction(13, "Suture wound / " + m_Supplies[3].ToString(), m_Supplies[3] > 0);
   if (m_Region <= BCMM_BodyRegion.ABDOMEN) AddAction(15, "Repair internal bleeding", m_Supplies[2] > 0);
  }
  if (m_Tab == 5)
  {
   AddAction(6, "Assess pain");
   AddAction(7, "Check medication record");
   if (m_Region >= BCMM_BodyRegion.LEFT_ARM || m_Region==BCMM_BodyRegion.HEAD)
    for (int medication = 0; medication < BCMM_SupplyCatalog.MEDICATION_COUNT; medication++)
     if((medication==BCMM_MedicationKind.PARACETAMOL && m_Region==BCMM_BodyRegion.HEAD) || (medication!=BCMM_MedicationKind.PARACETAMOL && m_Region>=BCMM_BodyRegion.LEFT_ARM)) AddAction(20 + medication, BCMM_Pharmacology.Name(medication) + " / " + m_Supplies[4 + medication].ToString(), m_Supplies[4 + medication] > 0);
  }
  if (m_Tab == 6)
  {
   AddAction(5, "Assess circulation / fluid loss");
   if (m_Region >= BCMM_BodyRegion.LEFT_ARM)
   {
    EquipmentAction(BCMM_EquipmentProcedure.INSERT_IV);
    EquipmentAction(BCMM_EquipmentProcedure.INFUSE);
    EquipmentAction(BCMM_EquipmentProcedure.INFUSE_SALINE);
    EquipmentAction(BCMM_EquipmentProcedure.INFUSE_PLASMA);
    if(BCMM_TreatmentAccess.Setting("continuousFluidLinesEnabled")>0.5)
    {
     EquipmentAction(BCMM_EquipmentProcedure.CONNECT_WHOLE_BLOOD_LINE);
     EquipmentAction(BCMM_EquipmentProcedure.CONNECT_SALINE_LINE);
     EquipmentAction(BCMM_EquipmentProcedure.CONNECT_PLASMA_LINE);
    }
    EquipmentAction(BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE);
   }
  }
  DrawActions();
 }
 static void RefreshSupplies()
 {
  if (s_Open && s_Open.Player() && s_Open.Target() && s_Open.m_Region >= 0)
   s_Open.Player().BCMM_RequestClinicalCheck(s_Open.Target(), s_Open.m_Region, BCMM_ClinicalCheck.SUPPLIES, s_Open.m_View);
 }
 protected void EquipmentAction(int action)
 {
  int kind = BCMM_EquipmentPolicy.Kind(action);
  if (kind < 0) return;
  if(action==BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE)
  {
   SCR_CharacterDamageManagerComponent lineMedical;if(Target())lineMedical=SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
   AddAction(40+action,BCMM_EquipmentPolicy.Name(action),lineMedical && lineMedical.BCMM_FluidLineActive() && lineMedical.BCMM_FluidLineRegion()==m_Region);return;
  }
  if(action==BCMM_EquipmentProcedure.DISCONNECT_OXYGEN)
  {
   SCR_CharacterDamageManagerComponent medical;if(Target())medical=SCR_CharacterDamageManagerComponent.Cast(Target().FindComponent(SCR_CharacterDamageManagerComponent));
   AddAction(40+action,BCMM_EquipmentPolicy.Name(action),medical && (medical.BCMM_AppliedEquipment() & 64)!=0);return;
  }
  string label=BCMM_EquipmentPolicy.Name(action)+" / "+m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+kind].ToString();
  if(BCMM_EquipmentPolicy.IsInfusion(action) || BCMM_EquipmentPolicy.IsLineConnection(action))
  {
   IEntity actor;if(GetGame().GetPlayerController())actor=GetGame().GetPlayerController().GetControlledEntity();
   IEntity bag=BCMM_EquipmentTreatment.FindEquipment(actor,kind);
   BCMM_EquipmentItemComponent source;if(bag)source=BCMM_EquipmentItemComponent.Cast(bag.FindComponent(BCMM_EquipmentItemComponent));
   if(source)label+=" / "+Math.Ceil(source.FluidRemainingMl()).ToString()+" mL in next bag";
  }
  AddAction(40+action,label,m_Supplies[BCMM_SupplyCatalog.EQUIPMENT_OFFSET+kind]>0);
 }
 void CheckBreathing() { SelectTab(1); RunAction(3); }
 void CheckPulse() { SelectTab(3); RunAction(4); }
 void UseEquipment(int action) { if (action >= 0 && action <= BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE) RunAction(40 + action); }
 void CheckPain() { SelectTab(5); RunAction(6); }
 void CheckMedication() { SelectTab(5); RunAction(7); }
 void AdministerMedication(int kind) { if (kind >= 0 && kind <= BCMM_MedicationKind.FENTANYL) RunAction(20 + kind); }
 void CheckBleeding() { SelectTab(3); RunAction(1); }
 void BandageSelectedRegion() { RunAction(10); }
 protected void RunAction(int action)
 {
  SCR_PlayerController player = Player();
  if (!player || m_Region < 0 || !Target() || !ActionInTab(action)) return;
  if ((action >= 1 && action <= 7) || action == 9)
  {
   SetText("RegionLabel", BCMM_ClinicalChecks.RegionName(m_Region) + " / checking...");
   player.BCMM_RequestClinicalCheck(Target(), m_Region, action, m_View);
   return;
  }
  if (action >= 40 && action <= 40 + BCMM_EquipmentProcedure.DISCONNECT_FLUID_LINE) { player.BCMM_RequestEquipment(Target(), action - 40, m_Region, m_View); return; }
  if (action >= 20 && action <= 25) { player.BCMM_RequestMedication(Target(), action - 20); return; }
  if (action >= 10 && action <= 15 && !KnownTreatment(action)) return;
  if (action == 10) { player.BCMM_RequestBandage(Target(), m_Region); m_TreatmentFlags.Set(m_Region, 0); RefreshActions(); return; }
  if (action == 11) player.BCMM_RequestSplint(Target(), m_Region == BCMM_BodyRegion.LEFT_LEG);
  if (action == 12) player.BCMM_RequestSurgery(Target(), BCMM_SurgicalProcedure.EXTRACT_PROJECTILE, m_Region);
  if (action == 13) player.BCMM_RequestSurgery(Target(), BCMM_SurgicalProcedure.SUTURE, m_Region);
  if (action == 14)
  {
   BCMM_SurgicalProcedure procedure = BCMM_SurgicalProcedure.REPAIR_RIGHT_LEG;
   if (m_Region == BCMM_BodyRegion.LEFT_LEG) procedure = BCMM_SurgicalProcedure.REPAIR_LEFT_LEG;
   player.BCMM_RequestSurgery(Target(), procedure, m_Region);
  }
  if (action == 15) player.BCMM_RequestSurgery(Target(), BCMM_SurgicalProcedure.REPAIR_INTERNAL, m_Region);
  if (action >= 11 && action <= 15) { m_TreatmentFlags.Set(m_Region, 0); RefreshActions(); }
  if (action == 16) player.BCMM_RequestCPR(Target());
  if (action == 17) player.BCMM_StopCPR();
 }
 protected void Choose(int slot) { int index = m_ActionPage * 6 + slot; if (slot < 6 && m_Actions.IsIndexValid(index) && ActionAllowed(index)) RunAction(m_Actions[index]); }
 protected void Action0() { Choose(0); }
 protected void Action1() { Choose(1); }
 protected void Action2() { Choose(2); }
 protected void Action3() { Choose(3); }
 protected void Action4() { Choose(4); }
 protected void Action5() { Choose(5); }
 protected void Action6() { Choose(6); }
 protected void Action7() { Choose(7); }
 protected void Action8() { Choose(8); }
 protected void Action9() { Choose(9); }
 protected void TabBleeding() { SelectTab(3); }
 protected void TabAirway() { SelectTab(1); }
 protected void TabBreathing() { SelectTab(1); }
 protected void TabCirculation() { SelectTab(3); }
 protected void TabSurgery() { SelectTab(7); }
 protected void TabPain() { SelectTab(5); }
 protected void TabMedication() { SelectTab(5); }
 protected void TabBlood() { SelectTab(6); }
 protected string PatientName(IEntity patient)
 {
  if (!patient) return "Unavailable";
  int id = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(patient);
  if (id > 0) return GetGame().GetPlayerManager().GetPlayerName(id);
  if (Player() && patient == Player().GetControlledEntity()) return "Local player";
  return "Casualty";
 }
 protected void FillPatients()
 {
  if (!Player()) return;
  array<IEntity> discovered = {};
  BCMM_AssessmentTargets.Collect(Player().GetControlledEntity(), discovered);
  discovered.RemoveItem(Player().GetControlledEntity());
  bool changed = discovered.Count() != m_Candidates.Count();
  if (!changed) for (int row = 0; row < discovered.Count(); row++) if (!m_Candidates.Contains(discovered[row])) changed = true;
  SetText("PatientName", PatientName(Target()));
  Widget status = GetRootWidget().FindAnyWidget("RosterStatus");
  if (status) status.SetVisible(discovered.IsEmpty());
  if (SCR_CompartmentAccessComponent.GetVehicleIn(Player().GetControlledEntity())) SetText("RosterStatus", "No other occupants.");
  else SetText("RosterStatus", "Look at a patient and press H. Vehicle occupants appear here.");
  if (changed)
  {
   foreach (BCMM_PatientRow old : m_RosterRows) { old.Release(); if (old.Root) old.Root.RemoveFromHierarchy(); }
   m_RosterRows.Clear();
   m_Candidates.Copy(discovered);
   Widget parent = GetRootWidget().FindAnyWidget("PatientList");
   foreach (IEntity patient : m_Candidates)
   {
    Widget root = GetGame().GetWorkspace().CreateWidgets("{6AB4B3F3FD215007}UI/layouts/BCMM/PatientRow.layout", parent);
    if (!root) continue;
    BCMM_PatientRow entry = new BCMM_PatientRow();
    entry.Menu = this; entry.Patient = patient; entry.Root = root;
    entry.Button = SCR_ButtonTextComponent.Cast(root.FindHandler(SCR_ButtonTextComponent));
    if (entry.Button) entry.Button.m_OnClicked.Insert(entry.Click);
    m_RosterRows.Insert(entry);
   }
  }
  foreach (BCMM_PatientRow entry : m_RosterRows)
   if (entry.Button)
   {
    entry.Button.SetText(PatientName(entry.Patient));
    entry.Button.SetToggled(entry.Patient == Target(), false, false, true);
   }
 }
 void ChooseRosterPatient(IEntity patient)
 {
  if (!Player() || !patient) return;
  IEntity vehicle = SCR_CompartmentAccessComponent.GetVehicleIn(Player().GetControlledEntity());
  if (!vehicle || vehicle != SCR_CompartmentAccessComponent.GetVehicleIn(patient)) return;
  SelectTarget(patient);
 }
 protected void SelectTarget(IEntity patient)
 {
  if (!Player() || !BCMM_TreatmentAccess.CanExamine(Player().GetControlledEntity(), patient)) return;
  m_Target = BCMM_BodyBagComponent.Id(patient);
  if (patient != Player().GetControlledEntity()) m_SwapTarget = m_Target;
  m_View = ++s_NextView; m_Region = -1; m_ActionPage = 0; m_CheckActive = false;
  m_Findings.Clear(); m_TreatmentFlags.Clear();
  Player().BCMM_RequestKnownChecks(patient, m_View);
  SetText("Assessment", ""); SetText("RegionLabel", "Select body region");
  for (int i = 0; i < 7; i++)
  {
   Widget highlight = GetRootWidget().FindAnyWidget("RegionHighlight" + i.ToString());
   if (highlight) highlight.SetVisible(false);
  }
  FillPatients(); RefreshActions();
 }
 protected void SwapPatient()
 {
  if (!Player()) return;
  IEntity self = Player().GetControlledEntity();
  if (Target() != self) { m_SwapTarget = m_Target; SelectTarget(self); return; }
  IEntity other = BCMM_BodyBagComponent.Resolve(m_SwapTarget);
  if (!other || other == self || !BCMM_TreatmentAccess.CanExamine(self, other)) other = BCMM_AssessmentTargets.LookingAt(self);
  if (other && other != self) SelectTarget(other);
 }
 protected void CancelProcedure() { if (Player()) Player().BCMM_RequestCancelProcedure(); }
 protected void ClosePanel() { Close(); }
}
class BCMM_PatientRow
{
 BCMM_MedicalMenu Menu;
 IEntity Patient;
 Widget Root;
 SCR_ButtonTextComponent Button;
 void Release() { if (Button) Button.m_OnClicked.Remove(Click); Menu = null; Patient = null; Button = null; }
 void Click() { if (Menu) Menu.ChooseRosterPatient(Patient); }
}
