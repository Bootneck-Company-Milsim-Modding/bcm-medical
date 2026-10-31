// Original BCM finite-supply save state. Units are selected by the saved kind:
// fluid=mL, oxygen=L, AED=whole charges. Capacity belongs to the item.
modded class BCMM_EquipmentItemComponent
{
 bool BCMM_IsPersistentFiniteSupply()
 {
  return IsFluidBag() || Kind==BCMM_EquipmentKind.OXYGEN || Kind==BCMM_EquipmentKind.AED;
 }
 array<float> BCMM_CaptureFiniteSupply()
 {
  if(!BCMM_IsPersistentFiniteSupply())return {};
  float capacity=m_FluidCapacityMl;float remaining=m_FluidRemainingMl;
  if(Kind==BCMM_EquipmentKind.OXYGEN){capacity=m_OxygenCapacityLitres;remaining=m_OxygenLitres;}
  if(Kind==BCMM_EquipmentKind.AED){capacity=m_ShockCapacity;remaining=m_Shocks;}
  return {1,Kind,capacity,remaining};
 }
 bool BCMM_RestoreFiniteSupply(notnull array<float> state)
 {
  if(!Replication.IsServer() || BCMM_SplintTreatment.IsReservedSupply(GetOwner()) || !BCMM_IsPersistentFiniteSupply())return false;
  if(state.Count()!=4 || state[0]!=1 || state[1]!=Kind)return false;
  string capacityKey="fluidBagVolumeMl";
  if(Kind==BCMM_EquipmentKind.OXYGEN)capacityKey="oxygenCapacityLitres";
  if(Kind==BCMM_EquipmentKind.AED)capacityKey="aedShocksPerDevice";
  BCMM_Settings schema=new BCMM_Settings();
  int index=schema.IndexOf(capacityKey);
  if(index<0 || !schema.Definition(index).Accepts(state[2]))return false;
  if(!(state[3]>=0 && state[3]<=state[2]))return false;
  if(Kind==BCMM_EquipmentKind.AED && state[3]!=Math.Round(state[3]))return false;
  if(IsFluidBag()){m_FluidCapacityMl=state[2];m_FluidRemainingMl=state[3];}
  if(Kind==BCMM_EquipmentKind.OXYGEN){m_OxygenCapacityLitres=state[2];m_OxygenLitres=state[3];}
  if(Kind==BCMM_EquipmentKind.AED){m_ShockCapacity=state[2];m_Shocks=state[3];}
  Replication.BumpMe();return true;
 }
}

class BCMM_EquipmentItemSerializer : ScriptedComponentSerializer
{
 override static typename GetTargetType(){return BCMM_EquipmentItemComponent;}
 // A corrupt saved finite item must not survive as a newly refilled device.
 override static EDeserializeFailHandling GetDeserializeFailHandling(){return EDeserializeFailHandling.ERROR;}
 override protected ESerializeResult Serialize(notnull IEntity owner,notnull GenericComponent component,notnull SaveContext context)
 {
  BCMM_EquipmentItemComponent supply=BCMM_EquipmentItemComponent.Cast(component);
  if(!supply.BCMM_IsPersistentFiniteSupply())return ESerializeResult.DEFAULT;
  array<float> state=supply.BCMM_CaptureFiniteSupply();
  if(!context.WriteValue("bcmmFiniteSupply",state))return ESerializeResult.ERROR;
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner,notnull GenericComponent component,notnull LoadContext context)
 {
  array<float> state;
  if(!context.ReadValue("bcmmFiniteSupply",state) || !state)return false;
  return BCMM_EquipmentItemComponent.Cast(component).BCMM_RestoreFiniteSupply(state);
 }
}