// A timed, authority-only removal action. No replacement supply or native item
// animation is needed to remove equipment that is already connected.
class BCMM_OxygenRemovalTreatment : BCMM_SplintTreatment
{
 protected ref BCMM_OxygenSession m_Connection;
 protected ref BCMM_Patient m_ConnectedModel;
 static bool BeginRemoval(IEntity actor,IEntity target,SCR_PlayerController player=null)
 {
  if(!Replication.IsServer() || (player && player.GetControlledEntity()!=actor) || !BCMM_TreatmentAccess.CanReach(actor,target) || !BCMM_MedicRole.EquipmentAllowed(actor,BCMM_EquipmentProcedure.DISCONNECT_OXYGEN))return false;
  SCR_CharacterDamageManagerComponent medical;if(target)medical=SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
  BCMM_OxygenSession connection=BCMM_OxygenSession.ConnectionForPatient(target);if(!medical || medical.IsDestroyed() || !connection || !medical.BCMM_GetPatient().Equipment.OxygenActive)return false;
  BCMM_OxygenRemovalTreatment operation=new BCMM_OxygenRemovalTreatment();operation.m_Connection=connection;operation.m_ConnectedModel=medical.BCMM_GetPatient();
  return StartOperation(operation,actor,target,null,BCMM_TreatmentAccess.Setting("oxygenRemovalSeconds"),player);
 }
 override protected bool RequiresInventorySupply(){return false;}
 override protected bool RequiresNativeAnimation(){return false;}
 override protected bool ConsumesKit(){return false;}
 override protected int ProcedureRegion(){return BCMM_BodyRegion.HEAD;}
 override protected string ProcedureName(){return BCMM_EquipmentPolicy.Name(BCMM_EquipmentProcedure.DISCONNECT_OXYGEN);}
 override protected bool StillEligible()
 {
  SCR_CharacterDamageManagerComponent medical;if(m_Target)medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  return medical && !medical.IsDestroyed() && medical.BCMM_GetPatient()==m_ConnectedModel && !m_ConnectedModel.Terminal && m_ConnectedModel.Equipment.OxygenActive && BCMM_OxygenSession.ConnectionForPatient(m_Target)==m_Connection && BCMM_MedicRole.EquipmentAllowed(m_Actor,BCMM_EquipmentProcedure.DISCONNECT_OXYGEN);
 }
 override protected void ApplyProcedure()
 {
  if(!StillEligible())return;
  BCMM_OxygenSession.StopForPatient(m_Target);
  if(m_Player)m_Player.BCMM_TreatmentFeedback("Oxygen disconnected.");
 }
}
