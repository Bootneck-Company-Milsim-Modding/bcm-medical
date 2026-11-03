// Rendered distance evidence, not an assertion that every LOD is visually correct.
class BCMM_Beta01WorldReview : BCMM_SplintRuntimeChecks
{
 protected ref array<ResourceName> m_Items;
 protected int m_Index;
 protected int m_View;
 protected int m_Captures;
 protected vector m_Centre;
 protected SCR_ManualCamera m_Camera;
 override protected void Prepare()
 {
  FileIO.MakeDirectory("$profile:screenshots");
  SCR_EntityHelper.DeleteEntityAndChildren(m_Target);m_Target=null;
  AIControlComponent ai=AIControlComponent.Cast(m_Actor.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  GetGame().GetPlayerController().SetControlledEntity(m_Actor);
  m_Items=BCMM_Beta01ItemResources.All();NextItem();
 }
 protected void NextItem()
 {
  if(m_Index>=m_Items.Count())
  {
   m_Suite.Record(m_Captures==87,"All 29 catalogue items captured at three world camera distances");
   PrintFormat("BCMM_WORLD_REVIEW_COMPLETE items=%1 captures=%2",m_Index,m_Captures);
   if(m_Camera)delete m_Camera;Finish();return;
  }
  m_Kit=Spawn(m_Items[m_Index],"110 3 0");
  m_Suite.Record(m_Kit!=null,"World review spawns exact production item");
  if(!m_Kit){Finish();return;}
  if(m_Kit.GetPhysics()){m_Kit.GetPhysics().SetVelocity(vector.Zero);m_Kit.GetPhysics().SetActive(ActiveState.INACTIVE);}
  GetGame().GetCallqueue().CallLater(PrepareCamera,200);
 }
 protected void PrepareCamera()
 {
  vector low,high;m_Kit.GetWorldBounds(low,high);m_Centre=(low+high)*0.5;
  m_Suite.Record(vector.Distance(low,high)>0.001,"World item has nonempty native bounds");
  PrintFormat("BCMM_WORLD_REVIEW_BOUNDS item=%1 resource=%2 dimensions=%3",m_Index,m_Items[m_Index],high-low);
  m_View=0;ShowView();
 }
 protected void ShowView()
 {
  float distance=0.75;if(m_View==1)distance=3;if(m_View==2)distance=12;
  vector direction="0.4 0.3 1";direction.Normalize();vector offset=direction*distance;
  EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
  Math3D.AnglesToMatrix((-offset).VectorToAngles(),spawn.Transform);spawn.Transform[3]=m_Centre+offset;
  if(!m_Camera)m_Camera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));
  m_Camera.SetWorldTransform(spawn.Transform);m_Camera.SetInputEnabled(false);m_Camera.SetVerticalFOV(45);
  GetGame().GetCameraManager().SetCamera(m_Camera);m_Camera.ApplyTransform(0);
  GetGame().GetCallqueue().CallLater(CaptureView,500);
 }
 protected void CaptureView()
 {
  System.MakeScreenshot(string.Format("$profile:screenshots/world-%1-distance-%2",m_Index,m_View));m_Captures++;
  if(++m_View<3){ShowView();return;}
  SCR_EntityHelper.DeleteEntityAndChildren(m_Kit);m_Kit=null;m_Index++;NextItem();
 }
}
