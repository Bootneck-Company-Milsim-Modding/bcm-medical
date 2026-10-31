// Diagnostic-only native catalogue/RPC acceptance, layered on the GM fixture.
class BCMM_CatalogueChecks
{
 static ResourceName Station(){return "{6AB4B3F3FD21B100}PrefabsEditable/Auto/Items/E_BCMM_FieldSurgeryStation.et";}
 protected static vector s_CatalogueSite;
 static vector Site()
 {
  if(s_CatalogueSite!=vector.Zero)return s_CatalogueSite;
  BaseWorld world=GetGame().GetWorld();
  for(int x=400;x<=3000;x+=200)for(int z=400;z<=3000;z+=200)
  {
   float height=world.GetSurfaceY(x,z);if(height<2)continue;
   TraceParam ground=new TraceParam();ground.Start=Vector(x,height+1,z);ground.End=Vector(x,height-0.5,z);ground.Flags=TraceFlags.WORLD;
   if(world.TraceMove(ground)>=1 || ground.TraceNorm[1]<0.98)continue;
   s_CatalogueSite=Vector(x,height,z);return s_CatalogueSite;
  }
  return vector.Zero;
 }
 static bool Thumbnail()
 {
  WorkspaceWidget workspace=GetGame().GetWorkspace();if(!workspace)return false;
  ImageWidget image=ImageWidget.Cast(workspace.CreateWidget(WidgetType.ImageWidgetTypeID,WidgetFlags.CENTER,Color.White,0,null));if(!image)return false;
  image.SetVisible(false);bool loaded=image.LoadImageTexture(0,"{6AB4B3F3FD21A101}UI/Textures/EditorPreviews/Auto/Items/E_BCMM_FieldSurgeryStation.edds",false,true);image.RemoveFromHierarchy();return loaded;
 }
 static bool Enabled(){string value;return System.GetCLIParam("bcmmCatalogueNetwork",value);}
 static void Resources(BCMM_DomainChecks suite)
 {
  Resource resource=Resource.Load(Station());IEntitySource source=SCR_BaseContainerTools.FindEntitySource(resource);
  IEntityComponentSource editable=SCR_EditableEntityComponentClass.GetEditableEntitySource(resource);
  suite.Record(source && editable && SCR_EditableEntityComponentClass.HasFlag(editable,EEditableEntityFlag.PLACEABLE),"Stationary Medical catalogue prefab has native placeable editable data");
  suite.Record(source && SCR_BaseContainerTools.FindComponentIndex(source,"BCMM_StretcherComponent")>=0 && SCR_BaseContainerTools.FindComponentIndex(source,"BCMM_MedicalFacilityComponent")>=0 && SCR_BaseContainerTools.FindComponentIndex(source,"Persistence")>=0 && SCR_BaseContainerTools.FindComponentIndex(source,"RplComponent")>=0,"Editable variant inherits stationary treatment, facility, replication and native persistence components");
  SCR_EditableEntityUIInfo info=SCR_EditableEntityComponentClass.GetInfo(editable);array<EEditableEntityLabel> labels={};if(info)info.GetEntityLabels(labels);
  suite.Record(info && labels.Contains(EEditableEntityLabel.TRAIT_MEDICAL) && WidgetManager.Translate(info.GetName())=="BCM stationary treatment stretcher","Catalogue name is localized and its native medical filter label is present");
  string serverWorld;bool headless=System.GetCLIParam("server",serverWorld);suite.Record(info && info.GetImage()=="{6AB4B3F3FD21A101}UI/Textures/EditorPreviews/Auto/Items/E_BCMM_FieldSurgeryStation.edds" && (headless || Thumbnail()),"Actual supplied stretcher model thumbnail is a compiled native texture");
  Resource registry=Resource.Load("{6AB4B3F3FD21B110}Configs/Editor/PlaceableEntities/BCMM_Medical.conf");SCR_PlaceableEntitiesRegistry instance=SCR_PlaceableEntitiesRegistry.Cast(BaseContainerTools.CreateInstanceFromContainer(registry.GetResource().ToBaseContainer()));
  suite.Record(instance && instance.IsExposed() && instance.GetPrefabs().Count()==1 && instance.GetPrefabs()[0]==Station(),"Medical world registry exposes exactly one stationary treatment stretcher");
  Resource editor=Resource.Load("{59EF8ECAE1DCD417}Prefabs/Editor/Modes/EditorModeEdit.et");IEntitySource editorSource=SCR_BaseContainerTools.FindEntitySource(editor);IEntityComponentSource placing=SCR_BaseContainerTools.FindComponentSource(editorSource,SCR_PlacingEditorComponent);BaseContainerList registries;if(placing)registries=placing.GetObjectArray("m_Registries");
  int medical;int nativeEntries;if(registries)for(int index=0;index<registries.Count();index++){string addon;registries.Get(index).Get("m_sAddon",addon);if(addon=="MCBMedical")medical++;else nativeEntries++;}
  PrintFormat("BCMM_CATALOGUE_REGISTRIES medical=%1 nativeEntries=%2",medical,nativeEntries);
  suite.Record(registries && medical==1 && nativeEntries==41,"Medical placement adds one registry while preserving all forty-one native registries");
  suite.Record(editorSource && SCR_BaseContainerTools.FindComponentIndex(editorSource,"SCR_ContentBrowserEditorComponent")>=0 && SCR_BaseContainerTools.FindComponentIndex(editorSource,"SCR_ContextActionsEditorComponent")>=0 && SCR_BaseContainerTools.FindComponentSource(editorSource,SCR_PreviewEntityEditorComponent)!=null,"Native edit-mode browser, context actions and placement preview remain configured");
 }
}
modded class BCMM_LifeAdminManifest
{
 string StationId;
 void BCMM_LifeAdminManifest(){RegV("StationId");}
}
modded class SCR_BaseGameMode
{
 [RplProp()] RplId m_BCMMCatalogueStation=RplId.Invalid();
 void BCMM_PublishCatalogueStation(IEntity entity){m_BCMMCatalogueStation=BCMM_BodyBagComponent.Id(entity);Replication.BumpMe();}
}
modded class BCMM_LifeAdminNetworkChecks
{
 protected IEntity m_CatalogueStation;
 protected int m_CataloguePlacements;
 override protected void BeginNativeActions()
 {
  if(BCMM_CatalogueChecks.Enabled())
  {
   SCR_EditorManagerCore core=SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));SCR_EditorModeEntity editMode=core.GetEditorManager(1).FindModeEntity(EEditorMode.EDIT);
   SCR_PlacingEditorComponent placing;if(editMode)placing=SCR_PlacingEditorComponent.Cast(editMode.FindComponent(SCR_PlacingEditorComponent));
   m_Suite.Record(placing && placing.GetPrefabID(BCMM_CatalogueChecks.Station())>=0,"Server native GM catalogue resolves the Medical prefab index");
   if(placing)placing.GetOnPlaceEntityServer().Insert(BCMM_OnCataloguePlaced);
  }
  super.BeginNativeActions();
 }
 protected void BCMM_OnCataloguePlaced(int prefabID,SCR_EditableEntityComponent entity,int playerID)
 {
  if(!entity || entity.GetOwner().GetPrefabData().GetPrefabName()!=BCMM_CatalogueChecks.Station())return;
  m_CatalogueStation=entity.GetOwner();m_CataloguePlacements++;m_Mode.BCMM_PublishCatalogueStation(m_CatalogueStation);
  m_Suite.Record(playerID==1 && m_CataloguePlacements==1,"Actual owning GM placement RPC produces one server-authoritative Medical station");
  PrintFormat("BCMM_CATALOGUE_SERVER_PLACED index=%1 player=%2 rpl=%3 valid=%4",prefabID,playerID,BCMM_BodyBagComponent.Id(m_CatalogueStation),BCMM_BodyBagComponent.Id(m_CatalogueStation).IsValid());
 }
 override protected void SaveAdminWorld()
 {
  if(BCMM_CatalogueChecks.Enabled())
  {
   PersistenceSystem persistence=PersistenceSystem.GetInstance();
   m_Suite.Record(m_CataloguePlacements==1 && m_CatalogueStation && m_CatalogueStation.FindComponent(BCMM_StretcherComponent) && m_CatalogueStation.FindComponent(BCMM_MedicalFacilityComponent),"Native GM-placed station retains its actual treatment and facility components");
   m_Suite.Record(persistence && m_CatalogueStation && persistence.IsTracked(m_CatalogueStation),"Native GM-placed editable station is automatically tracked for world persistence");
   if(persistence && m_CatalogueStation)m_AdminSave.StationId=persistence.GetId(m_CatalogueStation);
  }
  super.SaveAdminWorld();
 }
}
modded class BCMM_LifeAdminRestartChecks
{
 override protected void Prepare()
 {
  BCMM_LifeAdminManifest saved=new BCMM_LifeAdminManifest();saved.LoadFromFile("$profile:BCMM_LifeAdminRestart.json");
  if(!saved.StationId.IsEmpty())
  {
   PersistenceSystem persistence=PersistenceSystem.GetInstance();IEntity station;if(persistence)station=IEntity.Cast(persistence.FindById(saved.StationId));
   m_Suite.Record(station && station.GetPrefabData().GetPrefabName()==BCMM_CatalogueChecks.Station() && station.FindComponent(BCMM_StretcherComponent) && station.FindComponent(SCR_EditableEntityComponent),"Independent saved-world process restores the exact GM-placed editable treatment stretcher UUID");
  }
  super.Prepare();
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMCatalogueTracePhase;
 protected bool m_BCMMCatalogueSent;
 protected bool m_BCMMCatalogueObserved;
 protected bool m_BCMMCatalogueOpening;
 protected bool m_BCMMCatalogueAttempted;
 protected bool m_BCMMCatalogueThumbnail;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);if(Replication.IsServer() || !BCMM_CatalogueChecks.Enabled())return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());if(!mode || !player || mode.m_BCMMLifeAdminPhase<1)return;
  IEntity station=BCMM_BodyBagComponent.Resolve(mode.m_BCMMCatalogueStation);
  if(mode.m_BCMMLifeAdminPhase>m_BCMMCatalogueTracePhase){m_BCMMCatalogueTracePhase=mode.m_BCMMLifeAdminPhase;PrintFormat("BCMM_CATALOGUE_OBSERVER_TRACE player=%1 phase=%2 id=%3 valid=%4 resolved=%5",player.GetPlayerId(),m_BCMMCatalogueTracePhase,mode.m_BCMMCatalogueStation,mode.m_BCMMCatalogueStation.IsValid(),station!=null);}
  if(!m_BCMMCatalogueObserved && station && station.FindComponent(BCMM_StretcherComponent) && station.FindComponent(SCR_EditableEntityComponent))
  {m_BCMMCatalogueObserved=true;PrintFormat("BCMM_CATALOGUE_OBSERVED player=%1 actualStation=1",player.GetPlayerId());}
  if(player.GetPlayerId()!=1 || m_BCMMCatalogueSent)return;
  SCR_EditorManagerEntity editor=SCR_EditorManagerEntity.GetInstance();if(!editor)return;
  if(!m_BCMMCatalogueOpening){editor.SetCurrentMode(EEditorMode.EDIT);editor.Open(false);m_BCMMCatalogueOpening=true;return;}
  if(!editor.IsOpened())return;
  SCR_EditorModeEntity editMode=editor.FindModeEntity(EEditorMode.EDIT);SCR_PlacingEditorComponent placing;if(editMode)placing=SCR_PlacingEditorComponent.Cast(editMode.FindComponent(SCR_PlacingEditorComponent));if(!placing)return;
  SCR_PlacingEditorComponentClass data=SCR_PlacingEditorComponentClass.Cast(placing.GetEditorComponentData());int index=placing.GetPrefabID(BCMM_CatalogueChecks.Station());
  if(!data || index<0 || data.GetPrefab(index)!=BCMM_CatalogueChecks.Station())return;
  if(!m_BCMMCatalogueThumbnail){m_BCMMCatalogueThumbnail=BCMM_CatalogueChecks.Thumbnail();if(!m_BCMMCatalogueThumbnail)return;}
  SCR_EditorPreviewParams params=new SCR_EditorPreviewParams();Math3D.MatrixIdentity4(params.m_vTransform);params.m_vTransform[3]=BCMM_CatalogueChecks.Site()+"5 0 0";params.m_vTransform[3][1]=GetWorld().GetSurfaceY(params.m_vTransform[3][0],params.m_vTransform[3][2]);
  if(!m_BCMMCatalogueAttempted){ENotification notice=-1;SCR_EPreviewState preview=SCR_EPreviewState.PLACEABLE;bool allowed=placing.CanCreateEntity(notice,preview,params,index);PrintFormat("BCMM_CATALOGUE_ATTEMPT index=%1 allowed=%2 notice=%3 bounds=%4",index,allowed,notice,SCR_Global.IsPositionWithinTerrainBounds(params.m_vTransform[3]));m_BCMMCatalogueAttempted=true;}
  bool sent=placing.CreateEntity(BCMM_CatalogueChecks.Station(),params);
  if(sent){m_BCMMCatalogueSent=true;PrintFormat("BCMM_CATALOGUE_OWNER_SENT index=%1 nativeRPC=1 thumbnail=1",index);}
 }
}
