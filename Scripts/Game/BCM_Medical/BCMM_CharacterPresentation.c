// Original BCM procedural arm animation. Native locomotion and medical authority remain unchanged.
class BCMM_GripLiveBone
{
 string Name;string Parent;int Index;int ParentIndex;
 vector Matrix[4];vector Local[4];vector Goal[4];vector Final[4];
 bool Captured;bool Planned;bool HasGoal;
 void BCMM_GripLiveBone(string name,int index) { Name=name;Index=index;Parent=BCMM_GripPoseData.Parent(name); }
}
class BCMM_GripMath
{
 static vector Cross(vector a,vector b) { return Vector(a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]); }
 static vector Direction(vector m[4],vector v) { return m[0]*v[0]+m[1]*v[1]+m[2]*v[2]; }
 static vector Point(vector m[4],vector v) { return m[3]+Direction(m,v); }
 static vector InverseDirection(vector m[4],vector v) { return Vector(vector.Dot(v,m[0]),vector.Dot(v,m[1]),vector.Dot(v,m[2])); }
 static vector InversePoint(vector m[4],vector v) { return InverseDirection(m,v-m[3]); }
 static vector Unit(vector v) { float n=v.Length(); if(n<0.00001) { return "0 1 0"; } return v/n; }
 static vector Rotate(vector value,vector from,vector to)
 {
  from=Unit(from);to=Unit(to);float c=Math.Clamp(vector.Dot(from,to),-1,1);
  vector cross=BCMM_GripMath.Cross(from,to);float sine=cross.Length();
  if(sine<0.00001)
  {
   if(c>0) { return value; }
   vector axis=BCMM_GripMath.Cross(from,"0 1 0");if(axis.LengthSq()<0.0001) { axis=BCMM_GripMath.Cross(from,"1 0 0"); }
   axis=Unit(axis);return axis*(2*vector.Dot(axis,value))-value;
  }
  vector axis=cross/sine;
  return value*c+BCMM_GripMath.Cross(axis,value)*sine+axis*vector.Dot(axis,value)*(1-c);
 }
 static bool Invert(vector m[4],out vector result[4])
 {
  vector a=Cross(m[1],m[2]);float determinant=vector.Dot(m[0],a);
  if(!(Math.AbsFloat(determinant)>0.000001 && Math.AbsFloat(determinant)<1000)) { return false; }
  vector b=Cross(m[2],m[0])/determinant;vector c=Cross(m[0],m[1])/determinant;a=a/determinant;
  result[0]=Vector(a[0],b[0],c[0]);result[1]=Vector(a[1],b[1],c[1]);result[2]=Vector(a[2],b[2],c[2]);
  result[3]=-Direction(result,m[3]);return true;
 }
 static void Compose(vector a[4],vector b[4],out vector result[4])
 {
  for(int i=0;i<3;i++) { result[i]=Direction(a,b[i]); }
  result[3]=Point(a,b[3]);
 }
 static void Blend(vector a[4],vector b[4],float weight,out vector result[4])
 {
  result[0]=Unit(a[0]*(1-weight)+b[0]*weight);
  vector up=a[1]*(1-weight)+b[1]*weight;
  result[2]=Unit(BCMM_GripMath.Cross(result[0],up));result[1]=Unit(BCMM_GripMath.Cross(result[2],result[0]));
  result[3]=a[3]*(1-weight)+b[3]*weight;
 }
}
[ComponentEditorProps(category:"GameScripted/BCM Medical")]
class BCMM_CharacterPresentationComponentClass:ScriptComponentClass {}
class BCMM_CharacterPresentationComponent:ScriptComponent
{
 protected static ref array<BCMM_CharacterPresentationComponent> s_Active={};
 protected ref array<ref BCMM_GripLiveBone> m_Bones={};
 protected ref array<ref BCMM_GripPoseBone> m_LeftFingers={};
 protected ref array<ref BCMM_GripPoseBone> m_RightFingers={};
 protected vector m_LeftTemplate[4];protected vector m_RightTemplate[4];
 protected vector m_LeftPalm;protected vector m_RightPalm;
 protected vector m_LeftTarget;protected vector m_RightTarget;
 protected vector m_StretcherFrame[4];
 protected float m_GripWeight;
 protected bool m_Initialised;
 protected ref BCMM_AppliedVisuals m_EquipmentVisuals;
 protected bool m_LoggedRawPose;
 protected bool m_ModifiersApplied;
 protected float m_LastPoseTime;
 protected vector m_FirstMovingFoot;
 protected bool m_TrackedFoot;
 protected float m_NativeFootTravel;
 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);s_Active.Insert(this);m_EquipmentVisuals=new BCMM_AppliedVisuals(owner);
 }
 override protected void OnDelete(IEntity owner)
 {
  if(m_EquipmentVisuals) { m_EquipmentVisuals.Clear(); }
  s_Active.RemoveItem(this);super.OnDelete(owner);
 }
 static void UpdateAll(float timeSlice)
 {
  foreach(BCMM_CharacterPresentationComponent component:s_Active) { if(component && component.GetOwner()) { component.UpdateFrame(timeSlice); } }
 }
 protected bool Initialise()
 {
  Animation animation=GetOwner().GetAnimation();if(!animation) { return false; }
  array<string> names={};animation.GetBoneNames(names);
  foreach(string name:names)
  {
   if(name=="LeftShoulder" || name=="RightShoulder" || name.StartsWith("LeftArm") || name.StartsWith("LeftForeArm") || name.StartsWith("LeftHand") || name.StartsWith("RightArm") || name.StartsWith("RightForeArm") || name.StartsWith("RightHand"))
   {
    if(name.Contains("Volume") || name.EndsWith("4") || name.EndsWith("5") || name.Contains("Prop") || name.Contains("Inbetween")) { continue; }
    BCMM_GripLiveBone bone=new BCMM_GripLiveBone(name,animation.GetBoneIndex(name));bone.ParentIndex=animation.GetBoneIndex(bone.Parent);m_Bones.Insert(bone);
   }
  }
  BCMM_GripPoseData.Read(true,m_LeftTemplate,m_LeftPalm,m_LeftFingers);
  BCMM_GripPoseData.Read(false,m_RightTemplate,m_RightPalm,m_RightFingers);
  m_Initialised=true;return true;
 }
 protected BCMM_GripLiveBone Bone(string name)
 {
  foreach(BCMM_GripLiveBone bone:m_Bones) { if(bone.Name==name) { return bone; } }
  return null;
 }
 void UpdateFrame(float timeSlice)
 {
  IEntity owner=GetOwner();
  if(m_EquipmentVisuals) { m_EquipmentVisuals.Update(); }
  string developingGrip;
  if(!System.GetCLIParam("bcmmGripPresentationCheck",developingGrip)) { return; }
  if(!m_Initialised && !Initialise()) { return; }
  BCMM_StretcherComponent stretcher=BCMM_StretcherComponent.ForCarrier(owner);
  bool active=stretcher && stretcher.IsCarried() && BCMM_TreatmentAccess.Setting("stretcherGripAnimationEnabled")>0.5;
  float blend=Math.Max(0.05,BCMM_TreatmentAccess.Setting("stretcherGripBlendSeconds"));
  float step=Math.Clamp(timeSlice,0,0.1)/blend;
  if(active)
  {
   m_GripWeight=Math.Min(1,m_GripWeight+step);
   stretcher.GetOwner().GetWorldTransform(m_StretcherFrame);
   vector carrier[4];owner.GetWorldTransform(carrier);
   Animation animation=owner.GetAnimation();vector shoulder[4];
   if(!animation.GetBoneMatrix(animation.GetBoneIndex("LeftArm"),shoulder)) { return; }
   float leftSide=1;
   if(vector.Dot(BCMM_GripMath.Direction(carrier,shoulder[3]),m_StretcherFrame[0])<0) { leftSide=-1; }
   float end=-1.13;
   if(vector.Dot(owner.GetOrigin()-m_StretcherFrame[3],m_StretcherFrame[2])>0) { end=1.13; }
   m_LeftTarget=BCMM_GripMath.Point(m_StretcherFrame,Vector(leftSide*0.30,0.37,end));
   m_RightTarget=BCMM_GripMath.Point(m_StretcherFrame,Vector(-leftSide*0.30,0.37,end));
  }
  else { m_GripWeight=Math.Max(0,m_GripWeight-step); }
  if(m_GripWeight<=0)
  {
   if(m_ModifiersApplied) { ClearModifiers(); }
   return;
  }
  Animation skeleton=owner.GetAnimation();
  foreach(BCMM_GripLiveBone captured:m_Bones)
  {
   captured.Captured=false;captured.Planned=false;captured.HasGoal=false;
   skeleton.GetBoneLocalMatrix(captured.Index,captured.Local);
  }
  foreach(BCMM_GripLiveBone nativeBone:m_Bones) { Capture(nativeBone); }
  ChimeraCharacter character=ChimeraCharacter.Cast(owner);
  if(active && character && character.GetCharacterController().GetVelocity().Length()>0.15)
  {
   vector foot[4];
   if(skeleton.GetBoneMatrix(skeleton.GetBoneIndex("LeftFoot"),foot))
   {
    if(!m_TrackedFoot) { m_FirstMovingFoot=foot[3];m_TrackedFoot=true; }
    m_NativeFootTravel=Math.Max(m_NativeFootTravel,vector.Distance(m_FirstMovingFoot,foot[3]));
   }
  }
  ApplyArm(true,m_LeftTarget,m_LeftTemplate,m_LeftPalm,m_LeftFingers);
  ApplyArm(false,m_RightTarget,m_RightTemplate,m_RightPalm,m_RightFingers);
  ApplyModifiers();
  m_LastPoseTime=owner.GetWorld().GetWorldTime();
 }
 protected void Capture(BCMM_GripLiveBone bone)
 {
  if(bone.Captured) { return; }
  vector parent[4];BCMM_GripLiveBone ancestor=Bone(bone.Parent);
  if(ancestor) { Capture(ancestor);for(int i=0;i<4;i++) { parent[i]=ancestor.Matrix[i]; } }
  else { GetOwner().GetAnimation().GetBoneMatrix(bone.ParentIndex,parent); }
  BCMM_GripMath.Compose(parent,bone.Local,bone.Matrix);bone.Captured=true;
 }
 protected void Plan(BCMM_GripLiveBone bone)
 {
  if(bone.Planned) { return; }
  vector parent[4];BCMM_GripLiveBone ancestor=Bone(bone.Parent);
  if(ancestor) { Plan(ancestor);for(int i=0;i<4;i++) { parent[i]=ancestor.Final[i]; } }
  else { GetOwner().GetAnimation().GetBoneMatrix(bone.ParentIndex,parent); }
  if(bone.HasGoal) { for(int j=0;j<4;j++) { bone.Final[j]=bone.Goal[j]; } }
  else { BCMM_GripMath.Compose(parent,bone.Local,bone.Final); }
  bone.Planned=true;
 }
 protected void ClearModifiers()
 {
  vector identity[4];Math3D.MatrixIdentity4(identity);
  foreach(BCMM_GripLiveBone bone:m_Bones) { GetOwner().GetAnimation().SetBoneMatrix(GetOwner(),bone.Index,identity); }
  m_ModifiersApplied=false;
 }
 protected void ApplyModifiers()
 {
  Animation skeleton=GetOwner().GetAnimation();vector identity[4];Math3D.MatrixIdentity4(identity);
  foreach(BCMM_GripLiveBone bone:m_Bones)
  {
   if(!bone.HasGoal) { skeleton.SetBoneMatrix(GetOwner(),bone.Index,identity);continue; }
   Plan(bone);vector parent[4];BCMM_GripLiveBone ancestor=Bone(bone.Parent);
   if(ancestor) { Plan(ancestor);for(int i=0;i<4;i++) { parent[i]=ancestor.Final[i]; } }
   else { skeleton.GetBoneMatrix(bone.ParentIndex,parent); }
   vector inverseParent[4];vector inverseLocal[4];vector desiredLocal[4];vector modifier[4];
   if(!BCMM_GripMath.Invert(parent,inverseParent) || !BCMM_GripMath.Invert(bone.Local,inverseLocal)) { skeleton.SetBoneMatrix(GetOwner(),bone.Index,identity);continue; }
   BCMM_GripMath.Compose(inverseParent,bone.Final,desiredLocal);BCMM_GripMath.Compose(inverseLocal,desiredLocal,modifier);
   float distance=modifier[3].LengthSq();
   if(!(distance>=0 && distance<4)) { skeleton.SetBoneMatrix(GetOwner(),bone.Index,identity);continue; }
   skeleton.SetBoneMatrix(GetOwner(),bone.Index,modifier);
  }
  m_ModifiersApplied=true;
 }
 protected void Write(BCMM_GripLiveBone bone,vector target[4])
 {
  if(!bone) { return; }
  BCMM_GripMath.Blend(bone.Matrix,target,m_GripWeight,bone.Goal);bone.HasGoal=true;
 }
 protected void ApplyArm(bool left,vector palmTarget,vector template[4],vector palmOffset,notnull array<ref BCMM_GripPoseBone> fingers)
 {
  string prefix="Right";if(left) { prefix="Left"; }
  BCMM_GripLiveBone upper=Bone(prefix+"Arm");BCMM_GripLiveBone fore=Bone(prefix+"ForeArm");BCMM_GripLiveBone hand=Bone(prefix+"Hand");
  if(!upper || !fore || !hand) { return; }
  vector carrier[4];GetOwner().GetWorldTransform(carrier);
  vector handGoal[4];
  float side=vector.Dot(palmTarget-m_StretcherFrame[3],m_StretcherFrame[0]);
  bool flip=(left && side<0) || (!left && side>0);
  for(int i=0;i<3;i++)
  {
   vector axis=BCMM_GripMath.Direction(m_StretcherFrame,template[i]);
   if(flip) { axis=m_StretcherFrame[1]*(2*vector.Dot(axis,m_StretcherFrame[1]))-axis; }
   handGoal[i]=BCMM_GripMath.InverseDirection(carrier,axis);
  }
  if(!m_LoggedRawPose)
  {
   PrintFormat("BCMM_GRIP_RAW origin=%1 upper=%2 fore=%3 hand=%4 target=%5",GetOwner().GetOrigin(),upper.Matrix[3],fore.Matrix[3],hand.Matrix[3],palmTarget);
   m_LoggedRawPose=true;
  }
  vector wanted=BCMM_GripMath.InversePoint(carrier,palmTarget)-BCMM_GripMath.Direction(handGoal,palmOffset);
  vector oldShoulder=upper.Matrix[3];vector shoulder=oldShoulder;vector oldElbow=fore.Matrix[3];vector oldWrist=hand.Matrix[3];
  float a=vector.Distance(shoulder,oldElbow);float b=vector.Distance(oldElbow,oldWrist);
  if(a<0.05 || b<0.05) { return; }
  // Bounded clavicle protraction follows a loaded reach without stretching the arm bones.
  vector shift=BCMM_GripMath.Unit(wanted-shoulder)*Math.Clamp(vector.Distance(wanted,shoulder)-(a+b)+0.015,0,0.06);
  shoulder=shoulder+shift;
  BCMM_GripLiveBone clavicle=Bone(prefix+"Shoulder");
  if(clavicle)
  {
   vector clavicleGoal[4];for(int c=0;c<4;c++) { clavicleGoal[c]=clavicle.Matrix[c]; }
   clavicleGoal[3]=clavicleGoal[3]+shift;Write(clavicle,clavicleGoal);
  }
  vector direction=BCMM_GripMath.Unit(wanted-shoulder);
  float distance=Math.Clamp(vector.Distance(wanted,shoulder),Math.AbsFloat(a-b)+0.005,a+b-0.005);
  vector wrist=shoulder+direction*distance;
  float x=(a*a-b*b+distance*distance)/(2*distance);float y=Math.Sqrt(Math.Max(0,a*a-x*x));
  vector pole=oldElbow-oldShoulder;pole=pole-direction*vector.Dot(pole,direction);
  if(pole.LengthSq()<0.0001) { pole=BCMM_GripMath.Cross(direction,"0 0 1"); }
  vector elbow=shoulder+direction*x+BCMM_GripMath.Unit(pole)*y;
  foreach(BCMM_GripLiveBone bone:m_Bones)
  {
   bool upperBone=bone.Name.StartsWith(prefix+"Arm");bool foreBone=bone.Name.StartsWith(prefix+"ForeArm");
   if(!upperBone && !foreBone) { continue; }
   vector origin=oldShoulder;vector anchor=shoulder;vector before=oldElbow-oldShoulder;vector after=elbow-shoulder;
   if(foreBone) { origin=oldElbow;anchor=elbow;before=oldWrist-oldElbow;after=wrist-elbow; }
   vector goal[4];
   for(int row=0;row<3;row++) { goal[row]=BCMM_GripMath.Rotate(bone.Matrix[row],before,after); }
   goal[3]=anchor+BCMM_GripMath.Rotate(bone.Matrix[3]-origin,before,after);Write(bone,goal);
  }
  handGoal[3]=wrist;Write(hand,handGoal);
  foreach(BCMM_GripPoseBone pose:fingers)
  {
   BCMM_GripLiveBone finger=Bone(pose.Name);if(!finger) { continue; }
   vector fingerGoal[4];BCMM_GripMath.Compose(handGoal,pose.Matrix,fingerGoal);Write(finger,fingerGoal);
  }
 }
 bool AppliedVisualsSafe() { return !m_EquipmentVisuals || m_EquipmentVisuals.CosmeticsOnly(); }
 int AppliedVisualCount() { if(!m_EquipmentVisuals) { return 0; } return m_EquipmentVisuals.Count(); }
 bool AppliedVisualSlot(int slot) { return m_EquipmentVisuals && m_EquipmentVisuals.SlotActive(slot); }
 float NativeFootTravel() { return m_NativeFootTravel; }
 float GripWeight() { return m_GripWeight; }
 float LastPoseTime() { return m_LastPoseTime; }
 float GripError()
 {
  Animation animation=GetOwner().GetAnimation();if(!animation) { return 100; }
  vector carrier[4];GetOwner().GetWorldTransform(carrier);vector left[4];vector right[4];
  if(!animation.GetBoneMatrix(animation.GetBoneIndex("LeftHand"),left) || !animation.GetBoneMatrix(animation.GetBoneIndex("RightHand"),right)) { return 100; }
  vector l=BCMM_GripMath.Point(carrier,BCMM_GripMath.Point(left,m_LeftPalm));
  vector r=BCMM_GripMath.Point(carrier,BCMM_GripMath.Point(right,m_RightPalm));
  float error=Math.Max(vector.Distance(l,m_LeftTarget),vector.Distance(r,m_RightTarget));
  if(!(error>=0 && error<100)) { return 100; }
  return error;
 }
}

modded class ArmaReforgerScripted
{
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  if(!world.IsEditMode()) { BCMM_CharacterPresentationComponent.UpdateAll(timeslice); }
 }
}
