// Real native thirty-second burial; both faction shovels and cancellation.
class BCMM_BurialRuntimeChecks:BCMM_BaggingRuntimeChecks
{
 protected IEntity m_Shovel;
 protected IEntity m_Grave;
 protected float m_BurialStarted;
 protected int m_Variant;
 protected IEntity m_FirstShovel;
 protected vector m_BagCenter;
 protected vector m_ExpectedForward;
 protected vector m_ExpectedUp;
 override void Start(BCMM_DomainChecks suite)
 {
  vector origin;bool found;
  BaseWorld world=GetGame().GetWorld();
  for(int x=400;x<=3000 && !found;x+=200)
   for(int z=400;z<=3000 && !found;z+=200)
   {
    float height=world.GetSurfaceY(x,z);if(height<2)continue;
    TraceParam ground=new TraceParam();ground.Start=Vector(x,height+1,z);ground.End=Vector(x,height-0.5,z);ground.Flags=TraceFlags.WORLD;
    if(world.TraceMove(ground)>=1 || ground.TraceNorm[1]<0.98)continue;
    string material=ground.TraceMaterial;material.ToLower();
    if(material.IndexOf("grass")<0 || !BCMM_BurialSite.NaturalMaterial(material))continue;
    origin=Vector(x,height+0.2,z);found=true;
   }
  suite.Record(found,"real native Arland terrain supplies a flat identified grass burial site");
  if(!found){suite.Complete();return;}
  super.Start(suite);
  if(m_Floor)delete m_Floor;if(m_FloorNorth)delete m_FloorNorth;if(m_FloorSouth)delete m_FloorSouth;
  m_Actor.SetOrigin(origin);m_Corpse.SetOrigin(origin+"1 0 0");m_Empty.SetOrigin(origin+"0 0.3 0");
  AIControlComponent actorAI=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(actorAI)actorAI.DeactivateAI();
 }
 override protected void Verify()
 {
  foreach(BCMM_BodyBagComponent bag:BCMM_BodyBagComponent.Instances)
   if(bag && bag.DeceasedName()=="BCM bagging test casualty") m_Bag=bag.GetOwner();
  m_Suite.Record(m_Bag && !m_Corpse && !m_Empty,"burial starts with actual completed named native bagging chain");
  m_Suite.Record(Math.AbsFloat(BCMM_BurialSite.AuthoredGraveScale()-0.59)<0.001,"native prefab source exposes the approved authored grave scale");
  if(!m_Bag){Finish();return;}
  SCR_InventoryStorageManagerComponent performerInventory=SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  array<IEntity> initialItems={};performerInventory.GetItems(initialItems,EStoragePurpose.PURPOSE_ANY);
  foreach(IEntity initialItem:initialItems)
  {
   if(!initialItem || !initialItem.GetPrefabData()) continue;
   if(initialItem.FindComponent(SCR_CampaignBuildingGadgetToolComponent))
    m_Suite.Record(performerInventory.TryDeleteItem(initialItem),"native fixture removes assembled-backpack shovel before missing-tool test");
  }
  m_Suite.Record(!BCMM_BurialOperation.CanBegin(m_Actor,m_Bag),"burial rejects actor without real shovel");
  EquipShovel(false);
  m_FirstShovel=m_Shovel;
  m_Suite.Record(!BCMM_BurialOperation.CanBegin(m_Actor,m_Bag),"stored entrenching gadget alone cannot authorize burial");
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Shovel,EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(BeginRangeCancellation,3000,false);
 }
 protected void BeginRangeCancellation()
 {
  m_Suite.Record(SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).GetHeldGadget()==m_Shovel,"native US entrenching gadget is actually held after equip animation");
  CheckSurfaceControls();
  m_Suite.Record(BCMM_BurialOperation.Begin(m_Actor,m_Bag),"real native US shovel starts timed burial");
  m_Suite.Record(!BCMM_BurialOperation.Begin(m_Actor,m_Bag),"duplicate burial reservation is rejected");
  m_Suite.Record(!BCMM_BurialOperation.CancelForPerformer(m_Actor,m_RemainsToken+1000),"stale owner animation cancellation cannot release another death token");
  m_Suite.Record(!BCMM_BurialOperation.CancelForPerformer(null,m_RemainsToken),"animation cancellation requires its original performer");
  InventoryStorageManagerComponent bagInventory=InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  foreach(IEntity original:m_OriginalItems)
  {
   InventoryItemComponent item=InventoryItemComponent.Cast(original.FindComponent(InventoryItemComponent));
   if(!item || !item.GetParentSlot())continue;
   BaseInventoryStorageComponent storage=item.GetParentSlot().GetStorage();
   m_Suite.Record(!bagInventory.CanRemoveItemFromStorage(original,storage),"native existing inventory session cannot remove original or nested equipment during burial reservation");
   if(storage.GetOwner()==m_Bag)
    m_Suite.Record(!bagInventory.TryRemoveItemFromStorage(original,storage),"actual native removal request cannot steal root equipment from reserved remains");
  }
  m_Actor.SetOrigin(m_Bag.GetOrigin()+"7 0 0");
  GetGame().GetCallqueue().CallLater(VerifyRangeCancellation,1000,false);
 }
 protected void CheckSurfaceControls()
 {
  vector original[4];m_Bag.GetWorldTransform(original);
  vector floorPosition=original[3]+"15 0 0";floorPosition[1]=GetGame().GetWorld().GetSurfaceY(floorPosition[0],floorPosition[2])+0.8;
  IEntity concrete=Spawn("{85C71F17C37D274C}Prefabs/Structures/BuildingParts/Entry/Entry_Block_Platform_01_3x6.et",floorPosition);
  m_Suite.Record(concrete!=null,"native concrete surface control spawns beside real grass");
  if(concrete)
  {
   concrete.Update();vector minimum;vector maximum;concrete.GetWorldBounds(minimum,maximum);
   m_Bag.SetOrigin(Vector((minimum[0]+maximum[0])*0.5,maximum[1]+0.3,(minimum[2]+maximum[2])*0.5));m_Bag.Update();
   TraceParam contact=new TraceParam();contact.Start=m_Bag.GetOrigin()+"0 1 0";contact.End=m_Bag.GetOrigin()-"0 2 0";contact.Flags=TraceFlags.ENTS|TraceFlags.WORLD;contact.Exclude=m_Bag;
   float fraction=GetGame().GetWorld().TraceMove(contact);string material=contact.TraceMaterial;if(contact.SurfaceProps)material+=" "+contact.SurfaceProps.GetName();material.ToLower();
   m_Suite.Record(fraction<1 && material.IndexOf("concrete")>=0,"actual native support trace identifies concrete over terrain");
   vector position;vector normal;
   m_Suite.Record(!BCMM_BurialSite.Trace(m_Bag,m_Actor,position,normal),"production burial site rejects actual concrete overlay");
   m_Bag.SetWorldTransform(original);m_Bag.Update();delete concrete;
  }
  vector oldVelocity=m_Bag.GetPhysics().GetVelocity();m_Bag.GetPhysics().SetVelocity("1 0 0");
  vector movingPosition;vector movingNormal;
  m_Suite.Record(m_Bag.GetPhysics().GetVelocity().Length()>0.2 && !BCMM_BurialSite.Trace(m_Bag,m_Actor,movingPosition,movingNormal),"actual moving native bag is ineligible for digging");
  m_Bag.GetPhysics().SetVelocity(oldVelocity);
 }
 protected void EquipShovel(bool soviet)
 {
  SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  ResourceName resource="{6E35D94130954509}Prefabs/Items/Equipment/Accessories/ETool_ALICE/ETool_ALICE_FreeRoamBuilding_Gadget.et";
  if(soviet) resource="{062E2F1D7F6739D6}Prefabs/Items/Equipment/Accessories/ETool_MPL50/ETool_MPL50_FreeRoamBuilding_Gadget.et";
  m_Shovel=Spawn(resource,m_Actor.GetOrigin()+"0 1 0");
  m_Suite.Record(m_Shovel && inventory.TryInsertItem(m_Shovel,EStoragePurpose.PURPOSE_ANY),"actual faction shovel enters native performer inventory");
 }
 protected void VerifyRangeCancellation()
 {
  BCMM_BodyBagComponent state=BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent));
  m_Suite.Record(state.Ready() && m_Bag,"range change cancels and restores original bag readiness");
  m_Suite.Record(FindGrave()==null,"cancelled digging creates no memorial grave");
  m_Suite.Record(!BCMM_BagInventoryPolicy.RemovalLocked(m_Bag),"cancelled burial releases native removal guard for original bag");
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Bag.FindComponent(InventoryStorageManagerComponent));
  foreach(IEntity original:m_OriginalItems)
  {
   InventoryItemComponent item=InventoryItemComponent.Cast(original.FindComponent(InventoryItemComponent));
   if(item && item.GetParentSlot() && item.GetParentSlot().GetStorage().GetOwner()==m_Bag)
    m_Suite.Record(inventory.CanRemoveItemFromStorage(original,item.GetParentSlot().GetStorage()),"native cancellation restores ordinary removal of exact original root equipment");
  }
  m_Actor.SetOrigin(m_Bag.GetOrigin()+"1 0.1 0");
  GetGame().GetCallqueue().CallLater(BeginShovelLoss,1000,false);
 }
 protected void BeginShovelLoss()
 {
  m_Suite.Record(BCMM_BurialOperation.Begin(m_Actor,m_Bag),"range cancellation releases reservation for a new burial");
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).RemoveHeldGadget();
  GetGame().GetCallqueue().CallLater(VerifyShovelCancellation,3000,false);
 }
 protected void VerifyShovelCancellation()
 {
  m_Suite.Record(BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).Ready() && !FindGrave(),"lost shovel cancels without disposing bag or creating grave");
  m_Suite.Record(m_Shovel && !m_Shovel.IsDeleted(),"stowing the tool cancels digging without consuming it");
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Shovel,EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(BeginFullBurial,3000,false);
 }
 protected IEntity FindGrave()
 {
  foreach(BCMM_GraveMarkerComponent marker:BCMM_GraveMarkerComponent.Instances)
   if(marker && marker.RemainsToken()==m_RemainsToken) return marker.GetOwner();
  return null;
 }
 protected void BeginFullBurial()
 {
  m_BurialStarted=GetGame().GetWorld().GetWorldTime();
  vector minimum;vector maximum;m_Bag.GetWorldBounds(minimum,maximum);m_BagCenter=(minimum+maximum)*0.5;
  vector surface;m_Suite.Record(BCMM_BurialSite.Trace(m_Bag,m_Actor,surface,m_ExpectedUp),"full burial validates real natural support and terrain normal");
  vector bagTransform[4];m_Bag.GetWorldTransform(bagTransform);m_ExpectedForward=bagTransform[0]-m_ExpectedUp*vector.Dot(bagTransform[0],m_ExpectedUp);m_ExpectedForward.Normalize();
  m_Suite.Record(SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).GetHeldGadget()==m_Shovel,"native faction entrenching gadget remains actually held for full burial");
  m_Suite.Record(BCMM_BurialOperation.Begin(m_Actor,m_Bag),"native faction shovel begins full thirty-second burial");
  GetGame().GetCallqueue().CallLater(VerifyBuried,32000,false);
 }
 protected void VerifyBuried()
 {
  m_Grave=FindGrave();
  BCMM_RemainsRecord record=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_RemainsToken);
  m_Suite.Record(GetGame().GetWorld().GetWorldTime()-m_BurialStarted>=30000,"burial uses actual unshortened native thirty-second interval");
  m_Suite.Record(!m_Bag && m_Grave,"successful burial replaces exact bag with one real native grave");
  m_Suite.Record(record && record.Disposition==BCMM_RemainsDisposition.BURIED,"bag deletion preserves committed BURIED death-token outcome");
  m_Suite.Record(m_Shovel && !m_Shovel.IsDeleted(),"completed digging does not consume the actual equipped entrenching tool");
  if(m_Variant>0)m_Suite.Record(m_FirstShovel && !m_FirstShovel.IsDeleted(),"switching native faction tools preserves the original US shovel");
  if(m_Variant==0)
   foreach(IEntity originalItem:m_OriginalItems)
    m_Suite.Record(!originalItem || originalItem.IsDeleted(),"burial disposes exact original casualty equipment still contained by the bag");
  if(!m_Grave){Finish();return;}
  vector graveTransform[4];m_Grave.GetWorldTransform(graveTransform);
  PrintFormat("BCMM_GRAVE_PLACEMENT scaleX=%1 scaleY=%2 scaleZ=%3",graveTransform[0].Length(),graveTransform[1].Length(),graveTransform[2].Length());
  m_Suite.Record(Math.AbsFloat(graveTransform[0].Length()-0.59)<0.005 && Math.AbsFloat(graveTransform[1].Length()-0.59)<0.005 && Math.AbsFloat(graveTransform[2].Length()-0.59)<0.005,"grave retains approved authored uniform scale without inheriting bag physics scale");
  vector forward=graveTransform[2];forward.Normalize();vector up=graveTransform[1];up.Normalize();
  m_Suite.Record(vector.Dot(forward,m_ExpectedForward)>0.98 && vector.Dot(up,m_ExpectedUp)>0.98,"grave long axis follows bag length and native terrain normal instead of rigid-body tilt");
  vector graveMin;vector graveMax;m_Grave.GetWorldBounds(graveMin,graveMax);vector graveCenter=(graveMin+graveMax)*0.5;
  m_Suite.Record(Math.AbsFloat(graveCenter[0]-m_BagCenter[0])<0.05 && Math.AbsFloat(graveCenter[2]-m_BagCenter[2])<0.05,"world mesh-bounds centres of original bag and memorial align on X/Z");
  m_Suite.Record(Math.AbsFloat(m_Grave.GetOrigin()[1]-GetGame().GetWorld().GetSurfaceY(m_Grave.GetOrigin()[0],m_Grave.GetOrigin()[2])+0.1)<0.005,"grave pivot sits at approved ten-centimetre terrain offset");
  BCMM_GraveMarkerComponent marker=BCMM_GraveMarkerComponent.Cast(m_Grave.FindComponent(BCMM_GraveMarkerComponent));
  m_Suite.Record(marker.DeceasedName()==record.DeceasedName && marker.RemainsToken()==m_RemainsToken,"memorial retains name and exact death token after physical remains deletion");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_Grave);
  m_Suite.Record(garbage && !garbage.Insert(m_Grave,1),"native collector refuses protected memorial grave");
  GetGame().GetCallqueue().CallLater(VerifyGraveSurvival,5000,false);
 }
 protected void VerifyGraveSurvival()
 {
  m_Suite.Record(m_Grave && !m_Grave.IsDeleted(),"original grave survives native garbage collection deadline");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Grave);
  BCMM_RemainsRecord previous=SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_RemainsByToken(m_RemainsToken);
  m_Suite.Record(previous && previous.Disposition==BCMM_RemainsDisposition.BURIED,"explicit memorial deletion cannot rewrite completed burial outcome");
  if(m_Variant++>0){Finish();return;}
  m_Bag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et",m_Actor.GetOrigin()+"1 0.1 0");
  m_Corpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"2 0 0");
  SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_TestKill();
  GetGame().GetCallqueue().CallLater(BindSecondDeath,1500,false);
 }
 protected void BindSecondDeath()
 {
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_RemainsToken=mode.BCMM_RegisterRemains(m_Corpse);
  BCMM_BodyBagComponent.Cast(m_Bag.FindComponent(BCMM_BodyBagComponent)).SetDeceasedName("Second native faction burial casualty");
  m_Suite.Record(mode.BCMM_TransferRemains(m_Corpse,m_Bag),"second real native corpse transfers a distinct death token to second bag");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Corpse);
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).RemoveHeldGadget();
  GetGame().GetCallqueue().CallLater(EquipSecondTool,2000,false);
 }
 protected void EquipSecondTool()
 {
  EquipShovel(true);
  SCR_GadgetManagerComponent.GetGadgetManager(m_Actor).SetGadgetMode(m_Shovel,EGadgetMode.IN_HAND);
  GetGame().GetCallqueue().CallLater(BeginFullBurial,3000,false);
 }
}
