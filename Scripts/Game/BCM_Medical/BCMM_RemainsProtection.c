// Original native remains lifetime broker. No ACE or standalone-addon dependency.
class BCMM_RemainsProtection
{
 static const float MINIMUM_CORPSE_SECONDS = 1800;
 protected static ref array<ref BCMM_RemainsProtectionEntry> s_Entries = {};
 protected static BCMM_RemainsProtectionEntry Find(IEntity entity)
 {
  foreach(BCMM_RemainsProtectionEntry entry:s_Entries) if(entry.Entity==entity) return entry;
  return null;
 }
 static void ObserveCorpse(IEntity entity)
 {
  if(!Replication.IsServer() || !entity || entity.GetWorld().IsEditMode()) return;
  ChimeraCharacter character=ChimeraCharacter.Cast(entity);
  if(!character || !character.GetCharacterController() || character.GetCharacterController().GetLifeState()!=ECharacterLifeState.DEAD) return;
  BCMM_RemainsProtectionEntry entry=Find(entity);
  if(!entry)
  {
   entry=new BCMM_RemainsProtectionEntry();entry.Entity=entity;
   entry.MinimumUntil=entity.GetWorld().GetWorldTime()*0.001+MINIMUM_CORPSE_SECONDS;
   s_Entries.Insert(entry);
  }
  entry.Refresh();
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(mode) mode.BCMM_RegisterRemains(entity);
 }
 static void ProtectBag(IEntity entity)
 {
  if(!Replication.IsServer() || !entity || entity.GetWorld().IsEditMode()) return;
  BCMM_RemainsProtectionEntry entry=Find(entity);
  if(!entry){entry=new BCMM_RemainsProtectionEntry();entry.Entity=entity;s_Entries.Insert(entry);}
  entry.Bag=true;entry.Refresh();
 }
 static bool ShouldProtect(IEntity entity)
 {
  if(!entity) return false;
  BCMM_RemainsProtectionEntry entry=Find(entity);
  if(entry)
  {
   if(entry.Bag || entity.GetWorld().GetWorldTime()*0.001<entry.MinimumUntil) return true;
   ChimeraCharacter character=ChimeraCharacter.Cast(entity);
   if(character && (character.IsInVehicle() || character.GetParent())) return true;
  }
  return BCMM_BaggingOperation.IsBusy(entity);
 }
 static void Forget(IEntity entity)
 {
  for(int index=s_Entries.Count()-1;index>=0;index--)
  {
   BCMM_RemainsProtectionEntry entry=s_Entries[index];
   if(entry.Entity!=entity && entry.Entity) continue;
   entry.Stop();s_Entries.Remove(index);
  }
 }
}
class BCMM_RemainsProtectionEntry
{
 IEntity Entity;
 float MinimumUntil;
 bool Bag;
 protected bool m_Blacklisted;
 void Refresh()
 {
  if(GetGame()) GetGame().GetCallqueue().Remove(Refresh);
  if(!Entity){BCMM_RemainsProtection.Forget(null);return;}
  if(!BCMM_RemainsProtection.ShouldProtect(Entity)){BCMM_RemainsProtection.Forget(Entity);return;}
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(Entity);
  if(garbage){garbage.UpdateBlacklist(Entity,true);m_Blacklisted=true;}
  if(!GetGame()) return;
  if(!garbage){GetGame().GetCallqueue().CallLater(Refresh,250,false);return;}
  if(!Bag)
  {
   float remaining=Math.Max(1,MinimumUntil-Entity.GetWorld().GetWorldTime()*0.001);
   GetGame().GetCallqueue().CallLater(Refresh,remaining*1000,false);
  }
 }
 void Stop()
 {
  if(GetGame()) GetGame().GetCallqueue().Remove(Refresh);
  if(Entity && m_Blacklisted)
  {
   SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(Entity);
   if(garbage) garbage.UpdateBlacklist(Entity,false);
  }
  m_Blacklisted=false;
 }
}
modded class SCR_CharacterDamageManagerComponent
{
 protected SCR_CharacterControllerComponent m_BCMMRemainsController;
 protected IEntity m_BCMMRemainsOwner;
 override void OnPostInit(IEntity owner)
 {
  super.OnPostInit(owner);
  if(!Replication.IsServer() || owner.GetWorld().IsEditMode()) return;
  m_BCMMRemainsOwner=owner;
  GetGame().GetCallqueue().CallLater(BCMM_BindRemainsController,0,false);
 }
 protected void BCMM_BindRemainsController()
 {
  ChimeraCharacter character=ChimeraCharacter.Cast(m_BCMMRemainsOwner);
  if(!character) return;
  m_BCMMRemainsController=SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
  if(!m_BCMMRemainsController) return;
  m_BCMMRemainsController.m_OnLifeStateChanged.Insert(BCMM_RemainsLifeChanged);
  if(m_BCMMRemainsController.GetLifeState()==ECharacterLifeState.DEAD) BCMM_RemainsProtection.ObserveCorpse(m_BCMMRemainsOwner);
 }
 protected void BCMM_RemainsLifeChanged(ECharacterLifeState previous,ECharacterLifeState current)
 {
  if(current==ECharacterLifeState.DEAD) BCMM_RemainsProtection.ObserveCorpse(m_BCMMRemainsOwner);
  else if(previous==ECharacterLifeState.DEAD) BCMM_RemainsProtection.Forget(m_BCMMRemainsOwner);
 }
 void ~SCR_CharacterDamageManagerComponent()
 {
  if(!GetGame() || !GetGame().GetCallqueue()) return;
  GetGame().GetCallqueue().Remove(BCMM_BindRemainsController);
  if(m_BCMMRemainsController) m_BCMMRemainsController.m_OnLifeStateChanged.Remove(BCMM_RemainsLifeChanged);
  BCMM_RemainsProtection.Forget(m_BCMMRemainsOwner);
 }
}
