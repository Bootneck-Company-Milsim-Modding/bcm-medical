// Isolated native modifier-space probe; never part of gameplay.
class BCMM_GripCoordinateChecks:BCMM_SplintRuntimeChecks
{
 protected int m_Bone;protected int m_ProbeStep;protected vector m_OriginalModel[4];protected vector m_OriginalLocal[4];
 override protected void Prepare()
 {
  m_Suite.Record(vector.Distance(BCMM_GripMath.Unit("3 0 0"),"1 0 0")<0.0001,"Grip vector normalisation preserves direction");
  m_Suite.Record(vector.Distance(BCMM_GripMath.Rotate("1 0 0","1 0 0","0 1 0"),"0 1 0")<0.0001,"Grip rotation maps independent axes correctly");
  vector parent[4];Math3D.MatrixIdentity4(parent);parent[3]="10 2 3";
  vector inverse[4];vector identity[4];BCMM_GripMath.Invert(parent,inverse);BCMM_GripMath.Compose(parent,inverse,identity);
  m_Suite.Record(identity[3].Length()<0.0001 && vector.Distance(identity[0],"1 0 0")<0.0001,"Grip matrix inversion round-trips translated frames");
  Animation skeleton=m_Actor.GetAnimation();m_Bone=skeleton.GetBoneIndex("LeftHand");
  skeleton.GetBoneMatrix(m_Bone,m_OriginalModel);skeleton.GetBoneLocalMatrix(m_Bone,m_OriginalLocal);
  vector parentBone[4];vector composed[4];skeleton.GetBoneMatrix(skeleton.GetBoneIndex("LeftForeArmTwist"),parentBone);BCMM_GripMath.Compose(parentBone,m_OriginalLocal,composed);
  PrintFormat("BCMM_NATIVE_LOCAL_COMPOSE error=%1 basisError=%2 parent=%3",vector.Distance(composed[3],m_OriginalModel[3]),vector.Distance(composed[0],m_OriginalModel[0]),parentBone[3]);
  PrintFormat("BCMM_BONE_BEFORE root=%1 model=%2 local=%3",m_Actor.GetOrigin(),m_OriginalModel[3],m_OriginalLocal[3]);
  ApplyStep();
 }
 protected void ApplyStep()
 {
  vector input[4];Math3D.MatrixIdentity4(input);
  if(m_ProbeStep==1) { input[3]="0.1 0 0"; }
  if(m_ProbeStep==2) { for(int i=0;i<4;i++) { input[i]=m_OriginalLocal[i]; } }
  if(m_ProbeStep==3) { for(int j=0;j<4;j++) { input[j]=m_OriginalModel[j]; } }
  bool result=m_Actor.GetAnimation().SetBoneMatrix(m_Actor,m_Bone,input);
  PrintFormat("BCMM_BONE_INPUT step=%1 accepted=%2 position=%3",m_ProbeStep,result,input[3]);
  GetGame().GetCallqueue().CallLater(CheckStep,250);
 }
 protected void CheckStep()
 {
  vector model[4];vector local[4];Animation skeleton=m_Actor.GetAnimation();skeleton.GetBoneMatrix(m_Bone,model);skeleton.GetBoneLocalMatrix(m_Bone,local);
  PrintFormat("BCMM_BONE_AFTER step=%1 root=%2 model=%3 local=%4",m_ProbeStep,m_Actor.GetOrigin(),model[3],local[3]);
  m_ProbeStep++;
  if(m_ProbeStep<4) { ApplyStep();return; }
  Finish();
 }
}
