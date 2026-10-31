// Original authority-owned finite flow from the exact patient-owned source.
class BCMM_FluidLineSession
{
 protected static ref array<ref BCMM_FluidLineSession> s_Active=
 {
 };
 protected IEntity m_Patient;
 protected IEntity m_Source;
 protected ref BCMM_Patient m_Model;
 protected int m_AccessRegion;
 float RemainingMl()
 {
  BCMM_EquipmentItemComponent source;
  if(m_Source)
  {
   source=BCMM_EquipmentItemComponent.Cast(m_Source.FindComponent(BCMM_EquipmentItemComponent));
  }
  if(source)
  {
   return source.FluidRemainingMl();
  }
  return 0;
 }
 int AccessRegion()
 {
  return m_AccessRegion;
 }
 protected float m_LastTick;
 static BCMM_FluidLineSession ForPatient(IEntity patient)
 {
  if(!patient)
  {
   return null;
  }
  foreach(BCMM_FluidLineSession line:s_Active)
  {
   if(line.m_Patient==patient)
   {
    return line;
   }
  }
  return null;
 }
 static bool UsesSource(IEntity source)
 {
  if(!source)
  {
   return false;
  }
  foreach(BCMM_FluidLineSession line:s_Active)
  {
   if(line.m_Source==source)
   {
    return true;
   }
  }
  return false;
 }
 static bool Start(IEntity patient,IEntity source,int region)
 {
  if(!Replication.IsServer() || !patient || !source || ForPatient(patient) || UsesSource(source))
  {
   return false;
  }
  BCMM_FluidLineSession line=new BCMM_FluidLineSession();
  line.m_Patient=patient;
  line.m_Source=source;
  line.m_AccessRegion=region;
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if(!medical)
  {
   return false;
  }
  line.m_Model=medical.BCMM_GetPatient();
  if(!line.Valid())
  {
   return false;
  }
  line.m_LastTick=GetGame().GetWorld().GetWorldTime()*0.001;
  s_Active.Insert(line);
  line.Publish();
  GetGame().GetCallqueue().CallLater(line.Update,250);
  return true;
 }
 protected bool Valid()
 {
  if(!m_Patient || !m_Source || BCMM_TreatmentAccess.Setting("continuousFluidLinesEnabled")<0.5)
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Patient.FindComponent(InventoryStorageManagerComponent));
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Source.FindComponent(BCMM_EquipmentItemComponent));
  if(!medical || medical.IsDestroyed() || !medical.GetBloodHitZone() || medical.BCMM_GetPatient()!=m_Model || !m_Model || m_Model.Terminal || !inventory || !inventory.Contains(m_Source) || !source || !source.IsFluidBag() || source.FluidRemainingMl()<=0.05 || !m_Model.Equipment.VascularAccess || m_AccessRegion<BCMM_BodyRegion.LEFT_ARM || m_AccessRegion>BCMM_BodyRegion.RIGHT_LEG || m_Model.Equipment.VascularAccessRegion!=m_AccessRegion)
  {
   return false;
  }
  BCMM_Settings settings=new BCMM_Settings();
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode)
  {
   mode.BCMM_CopySettings(settings);
  }
  return BCMM_ClinicalSafety.FluidDenial(m_Model,medical.GetBloodHitZone().GetHealthScaled(),settings,source.FluidKind()).IsEmpty();
 }
 protected void Update()
 {
  if(!Valid())
  {
   Stop();
   return;
  }
  float now=GetGame().GetWorld().GetWorldTime()*0.001;
  float elapsed=Math.Clamp(now-m_LastTick,0,1);
  m_LastTick=now;
  BCMM_EquipmentItemComponent source=BCMM_EquipmentItemComponent.Cast(m_Source.FindComponent(BCMM_EquipmentItemComponent));
  float duration=BCMM_TreatmentAccess.Setting("fluidInfusionSeconds");
  if(!(duration>0) || source.FluidCapacityMl()<=0)
  {
   Stop();
   return;
  }
  float requested=Math.Min(source.FluidRemainingMl(),source.FluidCapacityMl()*elapsed/duration);
  int kind=source.FluidKind();
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  float delivered=medical.BCMM_DeliverFluidVolumeMl(requested,kind);
  source.ConsumeFluidMl(Math.Min(requested,delivered));
  Publish();
  if(!Valid())
  {
   Stop();
   return;
  }
  GetGame().GetCallqueue().CallLater(Update,250);
 }
 static void StopForPatient(IEntity patient)
 {
  if(!Replication.IsServer())
  {
   return;
  }
  for(int index=s_Active.Count()-1;index>=0;index--)
  {
   if(s_Active[index].m_Patient==patient)
   {
    s_Active[index].Stop();
   }
  }
 }
 protected void Stop()
 {
  GetGame().GetCallqueue().Remove(Update);
  s_Active.RemoveItem(this);
  Publish();
 }
 protected void Publish()
 {
  SCR_CharacterDamageManagerComponent medical;
  if(m_Patient)
  {
   medical=SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  if(medical)
  {
   medical.BCMM_PublishFluidLine();
  }
 }
}
