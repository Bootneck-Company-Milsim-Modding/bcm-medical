#ifdef WORKBENCH
[WorkbenchPluginAttribute(name:"Build BCM medical presentation and arsenals",wbModules:{"ResourceManager"})]
class BCMM_BuildPresentationPlugin:WorkbenchPlugin
{
 override void RunCommandline()
 {
  ResourceManager manager=Workbench.GetModule(ResourceManager);
  array<string> files={"Source/Animations/BCMM_TabletOther_enter.txa","Source/Animations/BCMM_TabletOther_exit.txa","Source/Animations/BCMM_TabletOther_loop.txa","Source/Animations/BCMM_TabletSelf_enter.txa","Source/Animations/BCMM_TabletSelf_exit.txa","Source/Animations/BCMM_TabletSelf_loop.txa","Source/Animations/Workspaces/BCMM_TabletOther.agf","Source/Animations/Workspaces/BCMM_TabletOther.agr","Source/Animations/Workspaces/BCMM_TabletOther.asi","Source/Animations/Workspaces/BCMM_TabletOther.aw","Source/Animations/Workspaces/BCMM_TabletSelf.agf","Source/Animations/Workspaces/BCMM_TabletSelf.agr","Source/Animations/Workspaces/BCMM_TabletSelf.asi","Source/Animations/Workspaces/BCMM_TabletSelf.aw","Configs/EntityCatalog/FIA/InventoryItems_EntityCatalog_FIA.conf","Configs/EntityCatalog/US/InventoryItems_EntityCatalog_US.conf","Configs/EntityCatalog/USSR/InventoryItems_EntityCatalog_USSR.conf","Configs/Arsenal/ArsenalItemTypeUIConfig.conf"};
  foreach(string source:files) { string path;Workbench.GetAbsolutePath("$MCBMedical:"+source,path);manager.RegisterResourceFile(path,true); }
  manager.RebuildResourceFiles(files,"PC");Print("BCMM_PRESENTATION_BUILD_REQUESTED");
 }
}
#endif
