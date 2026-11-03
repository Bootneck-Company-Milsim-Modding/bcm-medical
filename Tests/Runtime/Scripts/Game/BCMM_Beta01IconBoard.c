// Native item-preview rendering with a labelled cell for every resource.
class BCMM_Beta01IconBoard
{
 protected static ref BCMM_Beta01IconBoard s_Board;
 protected Widget m_Root;
 protected ref array<IEntity> m_Items = {};
 static void Start()
 {
  s_Board = new BCMM_Beta01IconBoard();
  s_Board.Show();
 }
 protected void Show()
 {
  m_Root = GetGame().GetWorkspace().CreateWidgets("{B6C4ED1CA126B002}UI/BCMM_Beta01IconBoard.layout");
  ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
  array<ResourceName> resources = BCMM_Beta01ItemResources.All();
  foreach (int index, ResourceName resource : resources)
  {
   EntitySpawnParams spawn = new EntitySpawnParams(); spawn.TransformMode = ETransformMode.WORLD; spawn.Transform[3] = "100 3 0";
   IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(resource), GetGame().GetWorld(), spawn);
   m_Items.Insert(item);
   ItemPreviewWidget cell = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("Item" + index.ToString()));
   TextWidget label = TextWidget.Cast(m_Root.FindAnyWidget("Label" + index.ToString()));
   if (label) label.SetText(FilePath.StripExtension(FilePath.StripPath(resource)));
   if (item && cell) world.GetItemPreviewManager().SetPreviewItem(cell, item, null, true);
   PrintFormat("BCMM_ICON_BOARD index=%1 resource=%2 item=%3 widget=%4", index, resource, item != null, cell != null);
  }
  GetGame().GetCallqueue().CallLater(Capture, 5000);
 }
 protected void Capture()
 {
  FileIO.MakeDirectory("$profile:screenshots");
  System.MakeScreenshot("$profile:screenshots/labelled-native-icon-board");
  Print("BCMM_ICON_BOARD_CAPTURED");
 }
}
