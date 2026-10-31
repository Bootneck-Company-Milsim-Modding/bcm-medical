from pathlib import Path
root=Path(__file__).resolve().parents[2]
files=[]
for extension,folder in [('*.png','Assets/Medical'),('*.emat','Assets/Medical'),('*.fbx','Assets/Medical'),('*.wav','Sounds/Medical')]:
 for p in (root/folder).rglob(extension):
  if extension=='*.png' and not p.name.endswith(('_BCR.png','_NMO.png')):continue
  files.append(p.relative_to(root).as_posix())
s='#ifdef WORKBENCH\n[WorkbenchPluginAttribute(name: "Build BCM supplied medical models", wbModules: {"ResourceManager"})]\nclass BCMM_BuildMedicalAssetsPlugin : WorkbenchPlugin\n{\n override void Run() { Build(); }\n override void RunCommandline() { Build(); }\n protected void Build()\n {\n  ResourceManager manager = Workbench.GetModule(ResourceManager);\n'
s+='  array<string> sources = {'+', '.join('"'+p+'"' for p in files)+'};\n'
s+='  string selected; System.GetCLIParam("bcmmBuildAsset",selected);\n  if (!selected.IsEmpty())\n  {\n   array<string> requested = {};\n   foreach (string path : sources) { if (path.Contains("/"+selected+"/") || (selected=="Applied" && path.Contains("/Applied"))) { requested.Insert(path); } }\n   manager.RebuildResourceFiles(requested,"PC");\n  }\n  else\n  {\n   manager.RebuildResourceFiles(sources,"PC");\n  }\n  Print("BCMM_MEDICAL_ASSETS_BUILD_REQUESTED");\n }\n}\n#endif\n'
(root/'Scripts/WorkbenchGame/BCMM_BuildMedicalAssetsPlugin.c').write_text(s,encoding='utf-8')
