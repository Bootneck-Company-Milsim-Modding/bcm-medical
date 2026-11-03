#ifdef WORKBENCH
[WorkbenchPluginAttribute(name: "Build BCM beta 01 repairs", wbModules: {"ResourceManager"})]
class BCMM_BuildBeta01ResourcesPlugin : WorkbenchPlugin
{
 override void Run() { Build(); }
 override void RunCommandline() { Build(); }
 protected void Build()
 {
  ResourceManager manager = Workbench.GetModule(ResourceManager);
  array<string> files = {
   "Assets/Medical/CompactDragRope/RopeCoil_BCR.png", "Assets/Medical/CompactDragRope/RopeCoil.emat", "Assets/Medical/CompactDragRope/BCMM_CompactDragRope.fbx",
   "Assets/Medical/Suction/BCMM_Suction.fbx",
   "Assets/Medical/FluidBag/Data/BloodBody_BCR.png", "Assets/Medical/FluidBag/Data/PlasmaBody_BCR.png",
   "Assets/Medical/FluidBag/Data/BloodBody.emat", "Assets/Medical/FluidBag/Data/PlasmaBody.emat",
   "Assets/Medical/Epinephrine/BCMM_Epinephrine.fbx", "Assets/Medical/Injector/BCMM_Injector.fbx",
   "Assets/Medical/Morphine/BCMM_Morphine.fbx", "Assets/Medical/GenericInjector/BCMM_GenericInjector.fbx",
   "Assets/Medical/Naloxone/BCMM_Naloxone.fbx", "Assets/Medical/NCD/BCMM_NCD.fbx", "Assets/Medical/TXA/BCMM_TXA.fbx",
   "Assets/Medical/Splint/BCMM_Splint.fbx", "Assets/Medical/SurgeryKit/BCMM_SurgeryKit.fbx", "Assets/Medical/SutureKit/BCMM_SutureKit.fbx",
   "Assets/Medical/BVM/BCMM_BVM.fbx", "Assets/Medical/FluidBag/BCMM_FluidBag.fbx", "Assets/Medical/PlasmaBag/BCMM_PlasmaBag.fbx",
   "Assets/Medical/WeaponSling/BCMM_WeaponSling.fbx", "Assets/Medical/DragRopeLine/BCMM_DragRopeLine.fbx"
  };
  manager.RebuildResourceFiles(files, "PC");
  PrintFormat("BCMM_BETA01_RESOURCE_BUILD_QUEUED count=%1", files.Count());
 }
}
#endif
