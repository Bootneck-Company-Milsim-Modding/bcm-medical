#ifdef WORKBENCH
[WorkbenchPluginAttribute(name: "Build stretcher test deck", wbModules: {"ResourceManager"})]
class BCMM_BuildStretcherFixturePlugin : WorkbenchPlugin {
 override void RunCommandline() { ResourceManager manager=Workbench.GetModule(ResourceManager); array<string> paths={"Assets/BCMM_TestDeck.fbx"}; manager.RebuildResourceFiles(paths,"PC"); }
}
#endif
