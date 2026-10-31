class BCMM_AppliedVisuals
{
 protected IEntity m_Owner;
 protected ref array<IEntity> m_Objects={null,null,null,null,null,null,null,null,null,null,null,null,null};
 protected vector m_Right;protected vector m_Up;protected vector m_Front;
 void BCMM_AppliedVisuals(IEntity owner) { m_Owner=owner; }
 protected bool Flag(int flags,int bit) { return (flags & bit)!=0; }
 void Clear()
 {
  for(int i=0;i<m_Objects.Count();i++) { if(m_Objects[i]) { SCR_EntityHelper.DeleteEntityAndChildren(m_Objects[i]);m_Objects[i]=null; } }
 }
 bool CosmeticsOnly()
 {
  foreach(IEntity object:m_Objects)
  {
   if(object && (object.FindComponent(RplComponent) || object.FindComponent(InventoryItemComponent) || object.GetPhysics())) { return false; }
  }
  return true;
 }
 int Count() { int result;foreach(IEntity object:m_Objects) { if(object) { result++; } } return result; }
 bool SlotActive(int slot) { return m_Objects.IsIndexValid(slot) && m_Objects[slot]!=null; }
 protected bool Bone(string name,out vector position)
 {
  Animation animation=m_Owner.GetAnimation();if(!animation) { return false; }
  int index=animation.GetBoneIndex(name);vector bone[4];vector owner[4];
  if(!animation.GetBoneMatrix(index,bone)) { return false; }
  m_Owner.GetWorldTransform(owner);position=BCMM_GripMath.Point(owner,bone[3]);return true;
 }
 protected bool HeadFrame(out vector frame[4])
 {
  vector centre;vector left;vector right;vector lip;
  if(!Bone("Head",centre)) return false;
  if(Bone("EyeLeft",left) && Bone("EyeRight",right) && Bone("FacialLipUpperMiddle",lip))
  {
   frame[0]=BCMM_GripMath.Unit(left-right);vector up=(left+right)*0.5-lip;
   frame[1]=BCMM_GripMath.Unit(up-frame[0]*vector.Dot(up,frame[0]));frame[2]=BCMM_GripMath.Unit(BCMM_GripMath.Cross(frame[0],frame[1]));frame[3]=centre;return true;
  }
  vector local[4];vector owner[4];
  if(!m_Owner.GetAnimation().GetBoneMatrix(m_Owner.GetAnimation().GetBoneIndex("Head"),local)) return false;
  m_Owner.GetWorldTransform(owner);BCMM_GripMath.Compose(owner,local,frame);frame[0]=-frame[0];frame[2]=-frame[2];return true;
 }
 protected void Display(int slot,bool visible,ResourceName prefab,vector centre,vector right,vector up,vector forward,float scale=1)
 {
  if(!visible)
  {
   if(m_Objects[slot]) { SCR_EntityHelper.DeleteEntityAndChildren(m_Objects[slot]);m_Objects[slot]=null; }
   return;
  }
  vector frame[4];frame[0]=right*scale;frame[1]=up*scale;frame[2]=forward*scale;frame[3]=centre;
  if(!m_Objects[slot])
  {
   Resource resource=Resource.Load(prefab);if(!resource || !resource.IsValid()) { return; }
   EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;
   for(int row=0;row<4;row++) { spawn.Transform[row]=frame[row]; }
   m_Objects[slot]=GetGame().SpawnEntityPrefab(resource,m_Owner.GetWorld(),spawn);
  }
  if(m_Objects[slot]) { m_Objects[slot].SetWorldTransform(frame);m_Objects[slot].Update(); }
 }
 protected void Patch(int slot,bool visible,ResourceName prefab,vector point)
 {
  // Source patch lies in native X/Z; native Y is the outward surface normal.
  Display(slot,visible,prefab,point,m_Right,m_Front,-m_Up);
 }
 protected void Limb(int slot,bool visible,ResourceName prefab,string proximal,string distal,float fraction,float radius,float scale=1)
 {
  vector a,b;
  if(!Bone(proximal,a) || !Bone(distal,b)) { visible=false; }
  vector axis=BCMM_GripMath.Unit(b-a);
  vector across=m_Right-axis*vector.Dot(m_Right,axis);
  if(across.LengthSq()<0.001) { across=m_Front-axis*vector.Dot(m_Front,axis); }
  across=BCMM_GripMath.Unit(across);vector forward=BCMM_GripMath.Unit(BCMM_GripMath.Cross(across,axis));
  vector centre=a+(b-a)*fraction+m_Front*radius;
  Display(slot,visible,prefab,centre,across,axis,forward,scale);
 }
 void Update()
 {
  PlayerController player=GetGame().GetPlayerController();IEntity viewer;
  if(player) { viewer=player.GetControlledEntity(); }
  if(!m_Owner || !viewer || BCMM_TreatmentAccess.Setting("equipmentVisualsEnabled")<0.5 || vector.Distance(viewer.GetOrigin(),m_Owner.GetOrigin())>BCMM_TreatmentAccess.Setting("equipmentVisualRangeMetres")) { Clear();return; }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
  if(medical && medical.BCMM_AppliedEquipment()==0 && !medical.BCMM_PulseMonitorActive())
  {
   bool tourniquet;
   for(int region=BCMM_BodyRegion.LEFT_ARM;region<=BCMM_BodyRegion.RIGHT_LEG;region++) { if(medical.BCMM_RegionVisual(region) & 16) { tourniquet=true; } }
   if(!tourniquet) { Clear();return; }
  }
  vector left,right,spine,hips;
  if(!medical || !Bone("LeftArm",left) || !Bone("RightArm",right) || !Bone("Spine4",spine) || !Bone("Hips",hips)) { Clear();return; }
  m_Right=BCMM_GripMath.Unit(right-left);m_Up=BCMM_GripMath.Unit(spine-hips);m_Front=BCMM_GripMath.Unit(BCMM_GripMath.Cross(m_Right,m_Up));m_Up=BCMM_GripMath.Unit(BCMM_GripMath.Cross(m_Front,m_Right));
  int flags=medical.BCMM_AppliedEquipment();
  bool chest=BCMM_TreatmentAccess.Setting("equipmentVisualChestDevicesEnabled")>0.5;
  bool splints=BCMM_TreatmentAccess.Setting("equipmentVisualSplintsEnabled")>0.5;
  bool access=BCMM_TreatmentAccess.Setting("equipmentVisualIVEnabled")>0.5;
  bool tq=BCMM_TreatmentAccess.Setting("equipmentVisualTourniquetsEnabled")>0.5;
  Patch(0,chest && Flag(flags,1),"{0A4EA118D9DCCAEE}Prefabs/MedicalVisuals/BCMM_AppliedAEDUpper.et",spine+m_Right*0.10+m_Up*0.035+m_Front*0.16);
  Patch(1,chest && Flag(flags,1),"{24585E309EFFA7BD}Prefabs/MedicalVisuals/BCMM_AppliedAEDLower.et",spine-m_Right*0.14-m_Up*0.13+m_Front*0.17);
  Patch(2,chest && Flag(flags,2),"{D3011D057D01FB32}Prefabs/MedicalVisuals/BCMM_ChestSeal.et",spine-m_Right*0.12-m_Up*0.035+m_Front*0.18);
  Patch(3,chest && Flag(flags,4),"{D3011D057D01FB32}Prefabs/MedicalVisuals/BCMM_ChestSeal.et",spine+m_Right*0.12-m_Up*0.035+m_Front*0.18);
  Limb(4,splints && Flag(flags,8),"{8FE8158B5EF4440E}Prefabs/MedicalVisuals/BCMM_AppliedSplint.et","LeftKnee","LeftFoot",0.48,0.035);
  Limb(5,splints && Flag(flags,16),"{8FE8158B5EF4440E}Prefabs/MedicalVisuals/BCMM_AppliedSplint.et","RightKnee","RightFoot",0.48,0.035);
  string ivA="LeftForeArm";string ivB="LeftHand";int region=medical.BCMM_IVRegion();
  if(region==BCMM_BodyRegion.RIGHT_ARM) { ivA="RightForeArm";ivB="RightHand"; }
  if(region==BCMM_BodyRegion.LEFT_LEG) { ivA="LeftKnee";ivB="LeftFoot"; }
  if(region==BCMM_BodyRegion.RIGHT_LEG) { ivA="RightKnee";ivB="RightFoot"; }
  Limb(6,access && Flag(flags,32),"{7CDCFFC722075668}Prefabs/MedicalVisuals/BCMM_AppliedIVCannula.et",ivA,ivB,0.55,0.055);
  Limb(7,medical.BCMM_PulseMonitorActive(),"{28ED24A2516CDDBF}Prefabs/MedicalVisuals/BCMM_PulseOximeter.et","LeftHandIndex1","LeftHandIndex3",0.55,0.005);
  vector headWorld[4];vector adjustment[4];vector maskFrame[4];bool head=HeadFrame(headWorld);
  Math3D.AnglesToMatrix(Vector(BCMM_TreatmentAccess.Setting("maskYawDegrees"),BCMM_TreatmentAccess.Setting("maskPitchDegrees"),BCMM_TreatmentAccess.Setting("maskRollDegrees")),adjustment);
  adjustment[3]=Vector(BCMM_TreatmentAccess.Setting("maskOffsetRight"),BCMM_TreatmentAccess.Setting("maskOffsetUp"),BCMM_TreatmentAccess.Setting("maskOffsetForward"));
  if(head) { BCMM_GripMath.Compose(headWorld,adjustment,maskFrame); }
  Display(12,head && Flag(flags,64) && BCMM_TreatmentAccess.Setting("equipmentVisualOxygenEnabled")>0.5,"{D0F8A8C2097AA104}Prefabs/MedicalVisuals/BCMM_OxygenMask.et",maskFrame[3],maskFrame[0],maskFrame[1],maskFrame[2],BCMM_TreatmentAccess.Setting("maskScale"));
  Limb(8,tq && Flag(medical.BCMM_RegionVisual(BCMM_BodyRegion.LEFT_ARM),16),"{74C03114DA5E1BFB}Prefabs/MedicalVisuals/BCMM_AppliedTourniquet.et","LeftArm","LeftForeArm",0.35,0);
  Limb(9,tq && Flag(medical.BCMM_RegionVisual(BCMM_BodyRegion.RIGHT_ARM),16),"{74C03114DA5E1BFB}Prefabs/MedicalVisuals/BCMM_AppliedTourniquet.et","RightArm","RightForeArm",0.35,0);
  Limb(10,tq && Flag(medical.BCMM_RegionVisual(BCMM_BodyRegion.LEFT_LEG),16),"{74C03114DA5E1BFB}Prefabs/MedicalVisuals/BCMM_AppliedTourniquet.et","LeftLeg","LeftKnee",0.25,0,1.6);
  Limb(11,tq && Flag(medical.BCMM_RegionVisual(BCMM_BodyRegion.RIGHT_LEG),16),"{74C03114DA5E1BFB}Prefabs/MedicalVisuals/BCMM_AppliedTourniquet.et","RightLeg","RightKnee",0.25,0,1.6);
 }
}