// Original BCM graph orchestration. Native loiter replication owns graph playback.
class BCMM_CPRPlayback
{
 ref SCR_LoiterCustomAnimData Data;
}
class BCMM_CPRAnimation
{
 static const ResourceName GRAPH = "{6AB4B3F3FD216103}Source/Animations/Workspaces/BCMM_CPR.agr";
 static const ResourceName INSTANCE = "{6AB4B3F3FD216104}Source/Animations/Workspaces/BCMM_CPR.asi";
 protected static ref map<IEntity, ref BCMM_CPRPlayback> s_Playbacks = new map<IEntity, ref BCMM_CPRPlayback>();
 static void Reset() { s_Playbacks.Clear(); }
 static bool IsPlaying(IEntity actor, ResourceName graph = GRAPH)
 {
  BCMM_CPRPlayback playback;
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character || !s_Playbacks.Find(actor, playback)) return false;
  return playback.Data && playback.Data.m_sGraphName == graph && IsActive(actor, graph);
 }
 // Server and proxies do not own the local playback map. Check the native
 // context as well as the command so another loiter cannot keep CPR alive.
 static bool IsActive(IEntity actor, ResourceName graph = GRAPH)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character) return false;
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
  return controller && controller.IsLoitering() && controller.GetScrInputContext().m_CustomAnimData && controller.GetScrInputContext().m_CustomAnimData.m_sGraphName == graph;
 }
 static bool Start(IEntity actor, IEntity patient = null, ResourceName graph = GRAPH, ResourceName instance = INSTANCE)
 {
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character) return false;
  if (!BCMM_FieldClips.OwnedGraph(graph)) return false;
  BCMM_CPRPlayback existing;
  if (s_Playbacks.Find(actor, existing))
  {
   if (!existing.Data || existing.Data.m_sGraphName != graph) return false;
   if (graph == GRAPH || IsActive(actor, graph)) return true;
   // A previous native command may still be releasing. Retry the field start
   // rather than treating an issued-but-unstarted request as live playback.
   s_Playbacks.Remove(actor);
  }
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
  if (!controller || !controller.CanPlayLoiterAnimation(ELoiteringType.CUSTOM) || controller.IsLoitering()) return false;
  RplComponent replication = character.GetRplComponent();
  if (replication && !replication.IsOwner()) return false;
  CharacterAnimationComponent animation = character.GetAnimationComponent();
  TAnimGraphCommand command = animation.BindCommand("CMD_Gesture_NPC");
  if (command == -1) return false;
  SCR_LoiterCustomAnimData data = SCR_LoiterCustomAnimData.CreateInstance(commandID: command, commandValue: -1, graphName: graph, graphInstanceName: instance);
  animation.SetVariableInt(animation.BindVariableInt("NPC_Speech"), 1);
  BCMM_CPRPlayback playback = new BCMM_CPRPlayback();
  playback.Data = data;
  s_Playbacks.Set(actor, playback);
  vector transform[4];
  actor.GetWorldTransform(transform);
  if (patient && patient != actor)
  {
   vector centre = patient.GetOrigin();
   Animation bones = patient.GetAnimation();
   vector bone[4];
   if (bones && bones.GetBoneMatrix(bones.GetBoneIndex("Spine4"), bone))
   {
    vector parent[4];
    patient.GetWorldTransform(parent);
    vector localPoint = bone[3];
    centre = parent[3] + parent[0] * localPoint[0] + parent[1] * localPoint[1] + parent[2] * localPoint[2];
   }
   vector direction = centre - actor.GetOrigin();
   direction[1] = 0;
   if (direction.LengthSq() > 0.001)
   {
    vector angles = direction.VectorToAngles();
    Math3D.AnglesToMatrix(angles, transform);
    if (graph != GRAPH) controller.GetInputContext().SetHeadingAngle(angles[0] * Math.DEG2RAD);
   }
   transform[3] = actor.GetOrigin();
  }
  controller.StartLoitering(null, ELoiteringType.CUSTOM, true, false, patient != null && graph == GRAPH, transform, false, data);
  return true;
 }
 // A finite native release window also bounds interruption cleanup. The exact
 // command identity prevents a delayed callback ending a later interaction.
 protected static void FinishRelease(ChimeraCharacter character, SCR_CharacterCommandLoiter expected)
 {
  if (!character || !expected) return;
  CharacterAnimationComponent animation = character.GetAnimationComponent();
  if (animation.GetCommandScripted() != expected) return;
  expected.FinishLoiter();
  SCR_CharacterCommandHandlerComponent handler = SCR_CharacterCommandHandlerComponent.Cast(animation.GetCommandHandler());
  if (handler && Replication.IsServer()) handler.BroadCastLoiterFinish();
 }
 static void Stop(IEntity actor, ResourceName graph = GRAPH)
 {
  BCMM_CPRPlayback playback;
  s_Playbacks.Find(actor, playback);
  if (playback && playback.Data && playback.Data.m_sGraphName != graph) return;
  s_Playbacks.Remove(actor);
  ChimeraCharacter character = ChimeraCharacter.Cast(actor);
  if (!character) return;
  SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
  if (controller && !controller.IsLoitering() && controller.GetScrInputContext().m_CustomAnimData && controller.GetScrInputContext().m_CustomAnimData.m_sGraphName == graph)
  {
   controller.GetScrInputContext().m_iLoiteringType = -1;
   controller.GetScrInputContext().m_CustomAnimData = SCR_LoiterCustomAnimData.Default;
  }
  if (controller && controller.IsLoitering() && controller.GetScrInputContext().m_CustomAnimData && controller.GetScrInputContext().m_CustomAnimData.m_sGraphName == graph)
  {
   CharacterAnimationComponent animation = character.GetAnimationComponent();
   animation.SetVariableInt(animation.BindVariableInt("NPC_Speech"), -1);
   SCR_CharacterCommandLoiter current = SCR_CharacterCommandLoiter.Cast(animation.GetCommandScripted());
   if (current) current.StopLoitering(false);
   SCR_CharacterCommandLoiter command = SCR_CharacterCommandLoiter.Cast(animation.GetCommandScripted());
   if (command) GetGame().GetCallqueue().CallLater(FinishRelease, 1200, false, character, command);
  }
 }
}
