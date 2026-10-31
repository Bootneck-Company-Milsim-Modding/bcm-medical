#ifdef WORKBENCH
[WorkbenchPluginAttribute(name: "Build BCM medical animations", wbModules: {"ResourceManager"})]
class BCMM_BuildAnimationsPlugin : WorkbenchPlugin
{
 override void Run() { BuildResources(); }
 override void RunCommandline() { BuildResources(); }
 protected void BuildResources()
 {
  ResourceManager manager = Workbench.GetModule(ResourceManager);
  array<string> sources = {"Source/Animations/BCMM_CPR_enter.txa", "Source/Animations/BCMM_CPR_exit.txa", "Source/Animations/BCMM_CPR_loop.txa", "Source/Animations/BCMM_Splint_enter.txa", "Source/Animations/BCMM_Splint_exit.txa", "Source/Animations/BCMM_Splint_loop.txa", "Source/Animations/BCMM_Surgery_enter.txa", "Source/Animations/BCMM_Surgery_exit.txa", "Source/Animations/BCMM_Surgery_loop.txa", "Source/Animations/BCMM_Suture_enter.txa", "Source/Animations/BCMM_Suture_exit.txa", "Source/Animations/BCMM_Suture_loop.txa", "Source/Animations/BCMM_TabletOther_enter.txa", "Source/Animations/BCMM_TabletOther_exit.txa", "Source/Animations/BCMM_TabletOther_loop.txa", "Source/Animations/BCMM_TabletSelf_enter.txa", "Source/Animations/BCMM_TabletSelf_exit.txa", "Source/Animations/BCMM_TabletSelf_loop.txa"};
  foreach (string source : sources)
  {
   string absolute;
   Workbench.GetAbsolutePath("$MCBMedical:" + source, absolute);
   PrintFormat("BCMM_ANIMATION_REGISTER source=%1 registered=%2", source, manager.RegisterResourceFile(absolute, true));
  }
  manager.RebuildResourceFiles(sources, "PC");
  array<string> graphSources = {"Source/Animations/Workspaces/BCMM_CPR.agf", "Source/Animations/Workspaces/BCMM_CPR.agr", "Source/Animations/Workspaces/BCMM_CPR.asi", "Source/Animations/Workspaces/BCMM_CPR.aw", "Source/Animations/Workspaces/BCMM_Splint.agf", "Source/Animations/Workspaces/BCMM_Splint.agr", "Source/Animations/Workspaces/BCMM_Splint.asi", "Source/Animations/Workspaces/BCMM_Splint.aw", "Source/Animations/Workspaces/BCMM_Surgery.agf", "Source/Animations/Workspaces/BCMM_Surgery.agr", "Source/Animations/Workspaces/BCMM_Surgery.asi", "Source/Animations/Workspaces/BCMM_Surgery.aw", "Source/Animations/Workspaces/BCMM_Suture.agf", "Source/Animations/Workspaces/BCMM_Suture.agr", "Source/Animations/Workspaces/BCMM_Suture.asi", "Source/Animations/Workspaces/BCMM_Suture.aw", "Source/Animations/Workspaces/BCMM_TabletOther.agf", "Source/Animations/Workspaces/BCMM_TabletOther.agr", "Source/Animations/Workspaces/BCMM_TabletOther.asi", "Source/Animations/Workspaces/BCMM_TabletOther.aw", "Source/Animations/Workspaces/BCMM_TabletSelf.agf", "Source/Animations/Workspaces/BCMM_TabletSelf.agr", "Source/Animations/Workspaces/BCMM_TabletSelf.asi", "Source/Animations/Workspaces/BCMM_TabletSelf.aw"};
  foreach (string graph : graphSources)
  {
   string full;
   Workbench.GetAbsolutePath("$MCBMedical:" + graph, full);
   PrintFormat("BCMM_GRAPH_REGISTER source=%1 registered=%2", graph, manager.RegisterResourceFile(full, true));
  }
  manager.RebuildResourceFiles(graphSources, "PC");
  Print("BCMM_ANIMATION_BUILD_REQUESTED");
 }
}
#endif