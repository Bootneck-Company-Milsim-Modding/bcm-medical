#ifdef WORKBENCH
[WorkbenchPluginAttribute(name: "Build BCM medical textures", wbModules: {"ResourceManager"})]
class BCMM_BuildUITexturesPlugin : WorkbenchPlugin
{
 override void RunCommandline()
 {
  ResourceManager manager = Workbench.GetModule(ResourceManager);
  string root;
  Workbench.GetAbsolutePath("$MCBMedical:UI/Textures/BCMM", root);
  bool logo = manager.RegisterResourceFile(root + "/CompanyLogo.png", true);
  bool anatomy = manager.RegisterResourceFile(root + "/Anatomy.png", true);
  manager.RebuildResourceFiles({"UI/Textures/BCMM/CompanyLogo.png", "UI/Textures/BCMM/Anatomy.png"}, "PC");
  PrintFormat("BCMM_UI_TEXTURE_BUILD logo=%1 anatomy=%2", logo, anatomy);

 }
}
#endif