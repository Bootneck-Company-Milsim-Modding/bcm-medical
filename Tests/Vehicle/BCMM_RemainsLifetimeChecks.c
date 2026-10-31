// Native garbage behavior against Medical entities; no shortened minimum timer.
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_RemainsTestKill() { m_BCMMDeathPermitted = true; Kill(Instigator.CreateInstigator(GetOwner())); }
}
modded class ArmaReforgerScripted
{
 protected ref BCMM_RemainsLifetimeChecks m_BCMMRemainsChecks;
 override void OnUpdate(BaseWorld world, float timeslice)
 {
  super.OnUpdate(world,timeslice);
  if(!Replication.IsServer() || world.IsEditMode() || !GetGame().GetGameMode() || m_BCMMRemainsChecks) return;
  m_BCMMRemainsChecks=new BCMM_RemainsLifetimeChecks();
  GetGame().GetCallqueue().CallLater(m_BCMMRemainsChecks.Start,2000,false);
 }
}
class BCMM_RemainsLifetimeChecks
{
 protected IEntity m_Corpse;
 protected IEntity m_Bag;
 protected IEntity m_Control;
 protected int m_Checks;
 protected int m_Failures;
 protected bool m_Full;
 protected float m_Started;
 protected void Check(bool valid,string label) { m_Checks++;if(!valid)m_Failures++;PrintFormat("BCMM_PROTECTION_CHECK check=%1 pass=%2 %3",m_Checks,valid,label); }
 protected IEntity Spawn(ResourceName prefab,vector position)
 {
  EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;spawn.Transform[3]=position;
  return GetGame().SpawnEntityPrefab(Resource.Load(prefab),GetGame().GetWorld(),spawn);
 }
 void Start()
 {
  string full;m_Full=System.GetCLIParam("bcmmFullLifetime",full);
  EntitySpawnParams floor=new EntitySpawnParams();floor.TransformMode=ETransformMode.WORLD;
  floor.Transform[0]="20 0 0";floor.Transform[1]="0 20 0";floor.Transform[2]="0 0 20";floor.Transform[3]="100 0 0";
  GetGame().SpawnEntityPrefab(Resource.Load("{AE73676ABA995907}Prefabs/Structures/BuildingParts/Entry/Entry_Block_Platform_01_3x12.et"),GetGame().GetWorld(),floor);
  m_Corpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et","100 1 0");
  m_Bag=Spawn("{6AB4B3F3FD216001}Prefabs/BodyBags/BCMM_FilledBodyBag.et","105 1 0");
  m_Control=Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et","110 1 0");
  Check(m_Corpse && m_Bag && m_Control,"native Medical corpse, filled bag and ordinary-item control spawn");
  if(!m_Corpse || !m_Bag || !m_Control){Finish();return;}
  SCR_CharacterDamageManagerComponent.Cast(m_Corpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_RemainsTestKill();
  m_Started=GetGame().GetWorld().GetWorldTime();
  GetGame().GetCallqueue().CallLater(Initial,1000,false);
 }
 protected void Initial()
 {
  Check(SCR_ChimeraCharacter.Cast(m_Corpse).GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD,"corpse fixture is actually native DEAD");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_Corpse);
  Check(garbage && !garbage.Insert(m_Corpse,1) && !garbage.IsInserted(m_Corpse),"native collector rejects immediate insertion of protected corpse");
  Check(garbage && !garbage.Insert(m_Bag,1) && !garbage.IsInserted(m_Bag),"native collector rejects independent filled-bag collection");
  Check(garbage && garbage.Insert(m_Control,1),"same native collector accepts ordinary control item");
  GetGame().GetCallqueue().CallLater(ControlCollected,5000,false);
 }
 protected void ControlCollected()
 {
  Check(!m_Control || m_Control.IsDeleted(),"ordinary control is physically collected after native deadline");
  Check(m_Corpse && m_Bag,"protected exact corpse and filled bag survive same collector");
  if(m_Failures || !m_Full){Finish();return;}
  GetGame().GetCallqueue().CallLater(BeforeMinimum,1792500,false);
  GetGame().GetCallqueue().CallLater(AfterMinimum,1797000,false);
  GetGame().GetCallqueue().CallLater(Heartbeat,60000,true);
 }
 protected void Heartbeat() { PrintFormat("BCMM_PROTECTION_WAIT elapsedSeconds=%1",(GetGame().GetWorld().GetWorldTime()-m_Started)*0.001); }
 protected void BeforeMinimum()
 {
  float elapsed=(GetGame().GetWorld().GetWorldTime()-m_Started)*0.001;
  PrintFormat("BCMM_PROTECTION_BEFORE elapsedSeconds=%1",elapsed);
  Check(elapsed>=1798 && elapsed<1800,"pre-expiry uses actual unshortened native world time");
  Check(m_Corpse && m_Bag,"exact corpse and filled bag survive the entire minimum window");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_Bag);
  Check(garbage && m_Corpse && !garbage.Insert(m_Corpse,1),"native collector still rejects corpse just before actual thirty-minute expiry");
 }
 protected void AfterMinimum()
 {
  float elapsed=(GetGame().GetWorld().GetWorldTime()-m_Started)*0.001;
  PrintFormat("BCMM_PROTECTION_AFTER elapsedSeconds=%1",elapsed);
  Check(elapsed>=1800,"actual thirty-minute native minimum has elapsed");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_Bag);
  Check(m_Bag && !garbage.Insert(m_Bag,1) && !garbage.IsInserted(m_Bag),"filled bag remains protected after corpse minimum expires");
  if(m_Corpse) Check(garbage.Insert(m_Corpse,1) || garbage.IsInserted(m_Corpse),"idle exposed corpse returns to native collection eligibility");
  else Check(true,"idle exposed corpse was already collected after minimum expiry");
  GetGame().GetCallqueue().CallLater(CollectedAfterMinimum,5000,false);
 }
 protected void CollectedAfterMinimum()
 {
  Check(!m_Corpse || m_Corpse.IsDeleted(),"native collector physically removes eligible idle corpse");
  Check(m_Bag && !m_Bag.IsDeleted(),"same native collector preserves original filled bag");
  Finish();
 }
 protected void Finish() { GetGame().GetCallqueue().Remove(Heartbeat);PrintFormat("BCMM_PROTECTION_COMPLETE checks=%1 failures=%2 fullDuration=%3",m_Checks,m_Failures,m_Full); }
}
