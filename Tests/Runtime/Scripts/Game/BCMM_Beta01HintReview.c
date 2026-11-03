// Diagnostic-only capture of native hint action identifiers and localization keys.
modded class BCMM_MedicalMenu
{
 override void OnMenuOpen()
 {
  PrintFormat("BCMM_TOGGLE_LIFECYCLE opening this=%1 current=%2",this,Current());
  super.OnMenuOpen();
 }
 override void OnMenuClose()
 {
  PrintFormat("BCMM_TOGGLE_LIFECYCLE closing this=%1 current=%2 medical=%3 back=%4",this,Current(),GetGame().GetInputManager().GetActionValue("BCMM_OpenMedical"),GetGame().GetInputManager().GetActionValue("MenuBack"));
  super.OnMenuClose();
 }
 override protected void ClosePanel()
 {
  Print("BCMM_TOGGLE_LIFECYCLE ClosePanel");super.ClosePanel();
 }
 protected bool m_BCMMToggleObserved;
 bool BCMM_ToggleActivatedDuringUpdate(){return m_BCMMToggleObserved;}
 override void OnMenuUpdate(float tDelta)
 {
  super.OnMenuUpdate(tDelta);
  if(GetGame().GetInputManager().IsActionActive("BCMM_OpenMedical"))m_BCMMToggleObserved=true;
 }
}
modded class SCR_AvailableActionsWidget
{
 protected static ref array<string> s_BCMMHintSeen={};
 override void SetText(string text,string name,EInputDeviceType currentInputDevice=EInputDeviceType.KEYBOARD)
 {
  string enabled;
  if(System.GetCLIParam("bcmmBeta01Hands",enabled) && !s_BCMMHintSeen.Contains(text+name))
  {
   s_BCMMHintSeen.Insert(text+name);PrintFormat("BCMM_NATIVE_HINT action=%1 label=%2",text,name);
  }
  super.SetText(text,name,currentInputDevice);
 }
}
modded class BCMM_MedicalInput
{
 static void BCMM_InvokeOpenHandler()
 {
  string injected;
  if(System.GetCLIParam("bcmmBeta01ToggleAction",injected))
  {
   GetGame().GetInputManager().SetActionValue("BCMM_OpenMedical",1);
   GetGame().GetCallqueue().CallLater(BCMM_ReleaseToggleAction,100,false);
   return;
  }
  Ensure();if(s_Instance)s_Instance.OpenMedical();
 }
 static void BCMM_ReleaseToggleAction(){GetGame().GetInputManager().SetActionValue("BCMM_OpenMedical",0);}
 override protected void OpenMedical()
 {
  bool wasOpen=BCMM_MedicalMenu.Current()!=null;
  Print("BCMM_HINT_OPEN_ACTION_RECEIVED");
  super.OpenMedical();
  PrintFormat("BCMM_HINT_OPEN_RESULT menu=%1", BCMM_MedicalMenu.Current());
  GetGame().GetCallqueue().CallLater(ReportToggleOutcome,300,false,wasOpen);
 }
 protected void ReportToggleOutcome(bool wasOpen){PrintFormat("BCMM_HINT_TOGGLE_SETTLED wasOpen=%1 isOpen=%2",wasOpen,BCMM_MedicalMenu.Current()!=null);}
}
