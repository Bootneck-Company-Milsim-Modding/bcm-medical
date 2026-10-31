// Connected-player authority acceptance, not a physical input test.
modded class SCR_CharacterDamageManagerComponent
{
 void BCMM_SessionTestKill() { m_BCMMDeathPermitted=true; Kill(Instigator.CreateInstigator(GetOwner())); }
}
modded class SCR_BaseGameMode
{
 bool BCMM_TestDisconnectDuration() { return m_BCMMSettings.Set("baggingDurationSeconds",30); }
}
modded class BCMM_VehicleLifecycleChecks
{
 protected IEntity m_SessionCorpse;
 protected IEntity m_SessionKit;
 protected IEntity m_Replacement;
 protected bool m_SessionDisconnect;
 protected int m_OriginalPlayerId;
 override protected void Loaded()
 {
  Check(m_Controller && m_Controller.GetControlledEntity()==m_Actor,"session fixture has original connected player's actual controlled character");
  m_SessionCorpse=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"1 0 0");
  m_SessionKit=Spawn("{6AB4B3F3FD216006}Prefabs/Items/BCMM_EmptyBodyBag.et",m_Actor.GetOrigin()+"0 1 0");
  IEntity backpack=Spawn("{06B68C58B72EAAC6}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium.et",m_Actor.GetOrigin()+"0 1 0");
  SCR_InventoryStorageManagerComponent inventory=SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  Check(backpack && inventory.TryInsertItem(backpack,EStoragePurpose.PURPOSE_ANY),"native original actor equips session kit backpack");
  Check(m_SessionKit && inventory.TryInsertItem(m_SessionKit,EStoragePurpose.PURPOSE_ANY),"exact folded kit enters original actor inventory");
  Check(m_SessionCorpse!=null,"native session casualty spawns");
  if(m_Failures){Finish();return;}
  SCR_CharacterDamageManagerComponent.Cast(m_SessionCorpse.FindComponent(SCR_CharacterDamageManagerComponent)).BCMM_SessionTestKill();
  GetGame().GetCallqueue().CallLater(BeginSessionBagging,1500,false);
 }
 protected void BeginSessionBagging()
 {
  string disconnect;
  m_SessionDisconnect=System.GetCLIParam("bcmmSessionDisconnect",disconnect);
  m_OriginalPlayerId=m_Controller.GetPlayerId();
  if(m_SessionDisconnect) Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestDisconnectDuration(),"disconnect fixture uses supported thirty-second bagging duration to allow native peer-loss detection");
  Check(BCMM_BaggingOperation.Begin(m_Actor,m_SessionCorpse),"original connected player begins native delayed bagging");
  Check(BCMM_BaggingOperation.IsBusy(m_SessionCorpse),"native bagging reservation exists before possession changes");
  if(m_Failures){Finish();return;}
  if(m_SessionDisconnect)
  {
   PrintFormat("BCMM_BAGGING_SESSION waitingDisconnect player=%1 duration=%2",m_OriginalPlayerId,BCMM_BaggingOperation.Setting("baggingDurationSeconds"));
   GetGame().GetCallqueue().CallLater(VerifyCancelledSession,32000,false);
   return;
  }
  m_Replacement=Spawn("{2F912ED6E399FF47}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Unarmed.et",m_Actor.GetOrigin()+"0 0 2");
  RplComponent.Cast(m_Replacement.FindComponent(RplComponent)).GiveExt(m_Controller.GetRplIdentity(),false);
  AIControlComponent ai=AIControlComponent.Cast(m_Replacement.FindComponent(AIControlComponent));if(ai)ai.DeactivateAI();
  m_Controller.SetInitialMainEntity(m_Replacement);
  bool switched=m_Controller.GetControlledEntity()==m_Replacement && GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Actor)<=0;
  Check(switched,"same real player controller possesses replacement while original actor remains alive nearby");
  PrintFormat("BCMM_BAGGING_SESSION originalPlayer=%1 switched=%2",m_Controller.GetPlayerId(),switched);
  GetGame().GetCallqueue().CallLater(VerifyCancelledSession,BCMM_BaggingOperation.Setting("baggingDurationSeconds")*1000+2000,false);
 }
 protected void VerifyCancelledSession()
 {
  if(m_SessionDisconnect)
  {
   array<int> connected={};GetGame().GetPlayerManager().GetPlayers(connected);
   bool disconnected=!connected.Contains(m_OriginalPlayerId) && (!m_Controller || GetGame().GetPlayerManager().GetPlayerController(m_OriginalPlayerId)!=m_Controller);
   Check(disconnected,"authority observes actual owning-client disconnection before accepting cancellation evidence");
   PrintFormat("BCMM_BAGGING_SESSION disconnected=%1 originalPlayer=%2",disconnected,m_OriginalPlayerId);
  }
  Check(m_SessionCorpse && SCR_ChimeraCharacter.Cast(m_SessionCorpse).GetCharacterController().GetLifeState()==ECharacterLifeState.DEAD,"possession change preserves original dead casualty instead of committing abandoned bagging");
  SCR_InventoryStorageManagerComponent inventory;
  if(m_Actor) inventory=SCR_InventoryStorageManagerComponent.Cast(m_Actor.FindComponent(SCR_InventoryStorageManagerComponent));
  Check(m_SessionKit && inventory && inventory.Contains(m_SessionKit),"session cancellation preserves exact unused folded kit on original actor");
  Check(!BCMM_BaggingOperation.IsBusy(m_SessionCorpse),"possession cancellation releases casualty reservation");
  SCR_GarbageSystem garbage=SCR_GarbageSystem.GetByEntityWorld(m_SessionCorpse);
  Check(garbage && !garbage.Insert(m_SessionCorpse,1),"cancelled connected-player operation preserves minimum corpse garbage protection");
  GetGame().GetCallqueue().CallLater(VerifyCancelledSurvival,5000,false);
 }
 protected void VerifyCancelledSurvival()
 {
  Check(m_SessionCorpse && m_SessionKit,"cancelled exact casualty and kit survive native collector deadline");
  Finish();
 }
}
