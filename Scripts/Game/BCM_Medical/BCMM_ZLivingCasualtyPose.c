// Original BCM lifecycle for native compartment-based casualty pose holders.
class BCMM_PoseAnchor
{
 static IEntity Spawn(IEntity actor,int mode)
 {
  ResourceName resource="{8BE8AAB5BD594ADB}Prefabs/Medical/Transport/BCMM_CarriedAnchor.et";
  if(mode==2)resource="{5F3E8CA2F93A4D06}Prefabs/Medical/Transport/BCMM_DraggedAnchor.et";
  EntitySpawnParams parameters=new EntitySpawnParams();parameters.TransformMode=ETransformMode.WORLD;actor.GetWorldTransform(parameters.Transform);
  return GetGame().SpawnEntityPrefab(Resource.Load(resource),actor.GetWorld(),parameters);
 }
 static bool Contains(IEntity anchor,IEntity patient)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(patient);if(!character || !anchor)return false;
  CompartmentAccessComponent access=character.GetCompartmentAccessComponent();
  return access && access.IsInCompartment() && access.GetVehicleCompartmentManagerOwner()==anchor;
 }
 static bool Enter(IEntity anchor,IEntity patient)
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(patient);if(!character || !anchor)return false;
  CompartmentAccessComponent access=character.GetCompartmentAccessComponent();
  if(!access || access.IsInCompartment())return Contains(anchor,patient);
  if(access.IsGettingIn())return false;
  BaseCompartmentManagerComponent manager=BaseCompartmentManagerComponent.Cast(anchor.FindComponent(BaseCompartmentManagerComponent));
  array<BaseCompartmentSlot> slots={};if(manager)manager.GetCompartments(slots);
  if(slots.IsEmpty())return false;
  return access.GetInVehicle(anchor,slots[0],true,-1,ECloseDoorAfterActions.INVALID,false);
 }
 static void Exit(IEntity anchor,IEntity patient,vector position,vector angles)
 {
  if(!Contains(anchor,patient))return;
  ChimeraCharacter character=ChimeraCharacter.Cast(patient);
  if(!character.GetRplComponent().IsOwner())return;
  vector transform[4];Math3D.AnglesToMatrix(angles,transform);transform[3]=position;
  CompartmentAccessComponent access=character.GetCompartmentAccessComponent();
  if(access.IsGettingOut())return;
  if(access.IsGettingIn())access.InterruptVehicleActionQueue(true,true,true);
  access.GetOutVehicle_NoDoor(transform,false,true,true);
 }
 static void Cleanup(IEntity anchor,IEntity patient,int attempts=0)
 {
  if(!Replication.IsServer() || !anchor)return;
  if(Contains(anchor,patient))
  {
   if(attempts<20)GetGame().GetCallqueue().CallLater(Cleanup,250,false,anchor,patient,attempts+1);
   return;
  }
  RplComponent.DeleteRplEntity(anchor,false);
 }
}
