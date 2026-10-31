// Owner-only feedback for server-timed care when the medical menu is closed.
// This widget neither captures controls nor authorizes or completes treatment.
class BCMM_ProcedureHUD
{
 protected static Widget s_Root;
 protected static SCR_PlayerController s_Player;
 static void Reset()
 {
  if (s_Root) s_Root.RemoveFromHierarchy();
  s_Root = null;
  s_Player = null;
 }
 static void Update()
 {
  SCR_PlayerController player = SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if (player != s_Player) Reset();
  s_Player = player;
  ChimeraCharacter actor;
  if (player) actor = ChimeraCharacter.Cast(player.GetControlledEntity());
  if (!actor || !actor.GetCharacterController() || actor.GetCharacterController().IsUnconscious() || actor.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE || !player.BCMM_HasProcedure())
  {
   Reset();
   return;
  }
  if (BCMM_MedicalMenu.Current())
  {
   if (s_Root) s_Root.SetVisible(false);
   return;
  }
  if (!GetGame().GetWorkspace()) return;
  if (!s_Root) s_Root = GetGame().GetWorkspace().CreateWidgets("{7ED896AAC4412D30}UI/layouts/BCMM/ProcedureHUD.layout");
  if (!s_Root) return;
  s_Root.SetVisible(true);
  TextWidget label = TextWidget.Cast(s_Root.FindAnyWidget("ProcedureLabel"));
  if (label) label.SetText(player.BCMM_ProcedureStatus());
  Widget track = s_Root.FindAnyWidget("ProgressTrack");
  Widget fill = s_Root.FindAnyWidget("ProgressFill");
  if (!track || !fill) return;
  float width, height;
  track.GetScreenSize(width, height);
  FrameSlot.SetSize(fill, GetGame().GetWorkspace().DPIUnscale(width * player.BCMM_ProcedureProgress()), 4);
 }
}
