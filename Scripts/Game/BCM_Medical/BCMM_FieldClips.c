// Original BCM clip selection. Only these owned graphs may enter the shared native playback adapter.
class BCMM_FieldClips
{
 static const ResourceName SURGERY_GRAPH = "{6AB4B3F3FD216302}Source/Animations/Workspaces/BCMM_Surgery.agr";
 static const ResourceName SURGERY_INSTANCE = "{6AB4B3F3FD216303}Source/Animations/Workspaces/BCMM_Surgery.asi";
 static const ResourceName SUTURE_GRAPH = "{6AB4B3F3FD216306}Source/Animations/Workspaces/BCMM_Suture.agr";
 static const ResourceName SUTURE_INSTANCE = "{6AB4B3F3FD216307}Source/Animations/Workspaces/BCMM_Suture.asi";
 static const ResourceName SPLINT_GRAPH = "{6AB4B3F3FD21630A}Source/Animations/Workspaces/BCMM_Splint.agr";
 static const ResourceName SPLINT_INSTANCE = "{6AB4B3F3FD21630B}Source/Animations/Workspaces/BCMM_Splint.asi";
 static const ResourceName TABLETSELF_GRAPH = "{61AF379CC39987F7}Source/Animations/Workspaces/BCMM_TabletSelf.agr";
 static const ResourceName TABLETSELF_INSTANCE = "{05439FC1A1BFB2DF}Source/Animations/Workspaces/BCMM_TabletSelf.asi";
 static const ResourceName TABLETOTHER_GRAPH = "{E5ADD4A02A3F37F6}Source/Animations/Workspaces/BCMM_TabletOther.agr";
 static const ResourceName TABLETOTHER_INSTANCE = "{9A0E6AEE867D5A34}Source/Animations/Workspaces/BCMM_TabletOther.asi";
 static bool OwnedGraph(ResourceName graph) { return graph == BCMM_CPRAnimation.GRAPH || graph == SURGERY_GRAPH || graph == SUTURE_GRAPH || graph == SPLINT_GRAPH || graph == TABLETSELF_GRAPH || graph == TABLETOTHER_GRAPH; }
 static ResourceName Graph(string kind)
 {
  if (kind == "Surgery") return SURGERY_GRAPH;
  if (kind == "Suture") return SUTURE_GRAPH;
  if (kind == "Splint") return SPLINT_GRAPH;
  if(kind=="TabletSelf") return TABLETSELF_GRAPH;
  if(kind=="TabletOther") return TABLETOTHER_GRAPH;
  return string.Empty;
 }
 static ResourceName Instance(string kind)
 {
  if (kind == "Surgery") return SURGERY_INSTANCE;
  if (kind == "Suture") return SUTURE_INSTANCE;
  if (kind == "Splint") return SPLINT_INSTANCE;
  if(kind=="TabletSelf") return TABLETSELF_INSTANCE;
  if(kind=="TabletOther") return TABLETOTHER_INSTANCE;
  return string.Empty;
 }
 static string Select(IEntity actor, IEntity target, IEntity item)
 {
  ChimeraCharacter patient = ChimeraCharacter.Cast(target);
  if (!actor || !patient || patient.IsInVehicle() || !item) return string.Empty;
  BCMM_MedicationItemComponent medication=BCMM_MedicationItemComponent.Cast(item.FindComponent(BCMM_MedicationItemComponent));
  if(medication && medication.Kind==BCMM_MedicationKind.PARACETAMOL && BCMM_TreatmentAccess.Setting("tabletAnimationEnabled")>0.5)
  {
   if(actor==target && Supported("TabletSelf",actor,target)) return "TabletSelf";
   if(actor!=target && Supported("TabletOther",actor,target)) return "TabletOther";
  }
  if(actor==target) return string.Empty;
  CharacterControllerComponent control = patient.GetCharacterController();
  if (!control.IsUnconscious() && control.GetStance() != ECharacterStance.PRONE) return string.Empty;
  if (item.FindComponent(BCMM_SplintItemComponent)) return "Splint";
  if (item.FindComponent(BCMM_SutureKitComponent)) return "Suture";
  if (item.FindComponent(BCMM_SurgeryKitComponent)) return "Surgery";
  return string.Empty;
 }
 static bool Supported(string kind,IEntity actor,IEntity target)
 {
  ChimeraCharacter user=ChimeraCharacter.Cast(actor);ChimeraCharacter patient=ChimeraCharacter.Cast(target);
  if(kind=="TabletSelf" || kind=="TabletOther")
  {
   if(!user || !patient || user.IsInVehicle() || patient.IsInVehicle() || user.GetCharacterController().GetStance()!=ECharacterStance.STAND || patient.GetCharacterController().GetStance()!=ECharacterStance.STAND || patient.GetCharacterController().IsUnconscious()) return false;
   return (kind=="TabletSelf" && actor==target) || (kind=="TabletOther" && actor!=target);
  }
  return IsGroundPatient(target);
 }
 static bool IsGroundPatient(IEntity target)
 {
  ChimeraCharacter patient = ChimeraCharacter.Cast(target);
  return patient && !patient.IsInVehicle() && (patient.GetCharacterController().IsUnconscious() || patient.GetCharacterController().GetStance() == ECharacterStance.PRONE);
 }
 static void Stop(IEntity actor)
 {
  BCMM_CPRAnimation.Stop(actor, SURGERY_GRAPH);
  BCMM_CPRAnimation.Stop(actor, SUTURE_GRAPH);
  BCMM_CPRAnimation.Stop(actor, SPLINT_GRAPH);
  BCMM_CPRAnimation.Stop(actor,TABLETSELF_GRAPH);
  BCMM_CPRAnimation.Stop(actor,TABLETOTHER_GRAPH);
 }
}
