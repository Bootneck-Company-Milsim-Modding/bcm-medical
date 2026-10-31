class BCMM_LifeAdminManifest : JsonApiStruct
{
 string SaveId;
 string AccountKey="22222222-2222-4222-8222-222222222222";
 string BodyId;
 int Balance;
 ref array<int> Ledger={};
 void BCMM_LifeAdminManifest(){RegV("SaveId");RegV("AccountKey");RegV("BodyId");RegV("Balance");RegV("Ledger");}
}
// Actual native editor context action/RPC acceptance; test identities are local diagnostics.
modded class SCR_BaseGameMode
{
 [RplProp()] int m_BCMMLifeAdminPhase;
 [RplProp()] int m_BCMMLifeAdminExpected;
 [RplProp()] RplId m_BCMMLifeAdminTarget=RplId.Invalid();
 void BCMM_PublishLifeAdmin(int phase,int expected,IEntity target){m_BCMMLifeAdminPhase=phase;m_BCMMLifeAdminExpected=expected;m_BCMMLifeAdminTarget=BCMM_BodyBagComponent.Id(target);Replication.BumpMe();PrintFormat("BCMM_LIFE_ADMIN_PHASE %1",phase);}
}
class BCMM_LifeAdminNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected SCR_BaseGameMode m_Mode;
 protected IEntity m_PlayerTarget;
 protected BCMM_LifeAccount m_Account;
 protected SCR_AttributesManagerEditorComponent m_Attributes;
 protected int m_Total;
 protected int m_Phase;
 protected ref BCMM_LifeAdminManifest m_AdminSave=new BCMM_LifeAdminManifest();
 override protected void Prepare()
 {
  SCR_PlayerController first=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(1));SCR_PlayerController second=SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(2));
  if(!first || !second || !first.GetControlledEntity() || !second.GetControlledEntity())
  {if(m_OwnerWait++<100){GetGame().GetCallqueue().CallLater(Prepare,250);return;}m_Suite.Record(false,"Native GM allowance fixture receives two connected controllers and bodies");Finish();return;}
  m_Mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());m_Mode.BCMM_TestLifeIdentity(1,"11111111-1111-4111-8111-111111111111");m_Mode.BCMM_TestLifeIdentity(2,"22222222-2222-4222-8222-222222222222");
  m_PlayerTarget=second.GetControlledEntity();m_Account=m_Mode.BCMM_AccountFor(2);m_Total=m_Mode.BCMM_ReadSetting("totalLives");
  m_Suite.Record(m_Mode.BCMM_ReserveLife(2,m_PlayerTarget) && m_Mode.BCMM_CommitLife(2,m_PlayerTarget),"Connected target has an actual native body and one committed local diagnostic life");
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_PlayerTarget.FindComponent(SCR_CharacterDamageManagerComponent));medical.GetBloodHitZone().SetHealth(medical.GetBloodHitZone().GetMaxHealth()*0.7);
  SCR_EditorManagerCore core=SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));SCR_EditorManagerEntity editor;if(core)editor=core.GetEditorManager(1);
  m_Suite.Record(editor && editor.GetPlayerID()==1,"Native editor core owns the real connected GM controller");if(!editor){Finish();return;}
  editor.AddEditorModes(EEditorModeAccess.BASE,EEditorMode.EDIT);
  GetGame().GetCallqueue().CallLater(BeginNativeActions,2000);
 }
 protected void BeginNativeActions()
 {
  SCR_EditorManagerCore core=SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));SCR_EditorManagerEntity editor=core.GetEditorManager(1);SCR_EditorModeEntity editMode=editor.FindModeEntity(EEditorMode.EDIT);
  if(editMode)m_Attributes=SCR_AttributesManagerEditorComponent.Cast(editMode.FindComponent(SCR_AttributesManagerEditorComponent));
  m_Suite.Record(SCR_BaseGameMode.BCMM_IsSettingsEditor(m_Attributes),"Server-granted native edit mode supplies real unlimited editor authority");if(!m_Attributes){Finish();return;}
  int before=m_Account.Ledger.RemainingDeployments(m_Total);
  m_Suite.Record(!m_Mode.BCMM_AdjustSelectedPlayerLives(null,m_PlayerTarget,1,false) && !m_Mode.BCMM_AdjustSelectedPlayerLives(m_Attributes,m_Target,1,false) && !m_Mode.BCMM_AdjustSelectedPlayerLives(m_Attributes,m_PlayerTarget,2,false) && m_Account.Ledger.RemainingDeployments(m_Total)==before,"Native selected-player controls reject missing authority, AI selection and invalid deltas without changing allowance");
  Publish(1,m_Total);
 }
 protected void Publish(int phase,int expected){m_Phase=phase;m_Mode.BCMM_PublishLifeAdmin(phase,expected,m_PlayerTarget);GetGame().GetCallqueue().CallLater(CheckPhase,10000);}
 protected void CheckPhase()
 {
  int expected=m_Total;if(m_Phase==2 || m_Phase==3)expected=m_Total-1;
  m_Suite.Record(m_Account.Ledger.RemainingDeployments(m_Total)==expected,"Actual owning GM context action/RPC updates the selected account exactly once");
  SCR_CharacterDamageManagerComponent medical=SCR_CharacterDamageManagerComponent.Cast(m_PlayerTarget.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(m_Account.Ledger.IsAlive() && m_Account.ActiveEntity==m_PlayerTarget && Math.AbsFloat(medical.GetBloodHitZone().GetHealthScaled()-0.7)<0.01,"Native allowance actions preserve the actual target life/body and partial blood");
  m_Suite.Record(m_Mode.BCMM_AccountFor(1).Ledger.RemainingDeployments(m_Total)==m_Total,"Targeted GM action never changes the acting GM's separate account");
  if(m_Phase==1){Publish(2,m_Total-1);return;}
  if(m_Phase==2){m_Account.Ledger.AdjustAllowance(-1);m_Mode.BCMM_SendLifeStatus(2);Publish(3,m_Total-1);return;}
  if(m_Phase==3){Publish(4,m_Total);return;}
  if(m_Phase==4){SCR_EditorManagerCore core=SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));core.GetEditorManager(1).RemoveEditorModes(EEditorModeAccess.BASE,EEditorMode.EDIT);Publish(5,m_Total);return;}
  m_Suite.Record(!SCR_BaseGameMode.BCMM_IsSettingsEditor(m_Attributes) && !m_Mode.BCMM_AdjustSelectedPlayerLives(m_Attributes,m_PlayerTarget,1,false) && m_Account.Ledger.RemainingDeployments(m_Total)==m_Total,"Actual native edit-mode revocation invalidates held authority and rejects further allowance changes");
  SaveAdminWorld();return;
  Finish();
 }
 protected void SaveAdminWorld()
 {
  PersistenceSystem persistence=PersistenceSystem.GetInstance();m_Suite.Record(persistence!=null,"Native GM allowance world has active persistence");if(!persistence){Finish();return;}
  m_AdminSave.BodyId=persistence.GetId(m_PlayerTarget);m_AdminSave.Ledger=m_Account.Ledger.BCMM_CaptureLedger();m_AdminSave.Balance=m_Account.Ledger.RemainingDeployments(m_Total);
  SaveGameManager manager=GetGame().GetSaveGameManager();manager.SetEnabledSaveTypes(ESaveGameType.SCRIPTED);manager.SetSavingAllowed(true);
  manager.RequestSavePoint(ESaveGameType.SCRIPTED,"BCMM native GM allowance",ESaveGameRequestFlags.BLOCKING,new SaveGameOperationCallback(OnAdminSaved));
 }
 protected void OnAdminSaved(bool success,Managed context=null)
 {
  m_Suite.Record(success,"Actual native world save commits the connected GM allowance change");if(!success){Finish();return;}
  GetGame().GetSaveGameManager().GetSaves(SaveGameManager.GetCurrentMissionResource(),new SaveGameObtainCallback(OnAdminSaves));
 }
 protected void OnAdminSaves(bool success,array<SaveGame> saves,Managed context=null)
 {
  SaveGame selected;if(success && saves)foreach(SaveGame save:saves)if(save.GetSavePointName()=="BCMM native GM allowance" && (!selected || save.GetSavePointCreatedUnix()>=selected.GetSavePointCreatedUnix()))selected=save;
  m_Suite.Record(selected!=null,"Committed connected GM allowance save is discoverable");
  if(selected){m_AdminSave.SaveId=selected.GetId();m_Suite.Record(m_AdminSave.PackToFile("$profile:BCMM_LifeAdminRestart.json"),"Native allowance save manifest retains actual body and ledger identities");}
  Finish();
 }
}
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
modded class BCMM_AdjustPlayerLivesContextAction
{
 int BCMM_TestLifeChange(){return m_Change;}
 bool BCMM_TestLifeReset(){return m_Reset;}
}
modded class SCR_BaseActionsEditorComponent
{
 void BCMM_TestChooseLifeTarget(SCR_EditableEntityComponent selected)
 {
  m_SelectedEntities.Clear();m_SelectedEntities.Insert(selected);m_HoveredEntity=selected;
 }
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMAdminSent;
 protected int m_BCMMAdminObserved;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);string enabled;if(Replication.IsServer() || !System.GetCLIParam("bcmmLifeAdminObserver",enabled))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetPlayerController());if(!mode || !player || mode.m_BCMMLifeAdminPhase<=0)return;
  int phase=mode.m_BCMMLifeAdminPhase;
  if(player.GetPlayerId()==2 && phase>m_BCMMAdminObserved && player.BCMM_LivesRemaining()==mode.m_BCMMLifeAdminExpected)
  {m_BCMMAdminObserved=phase;PrintFormat("BCMM_LIFE_ADMIN_TARGET_OBSERVED phase=%1 remaining=%2",phase,player.BCMM_LivesRemaining());}
  if(player.GetPlayerId()!=1 || phase<=m_BCMMAdminSent)return;
  IEntity target=BCMM_BodyBagComponent.Resolve(mode.m_BCMMLifeAdminTarget);if(!target)return;
  SCR_EditableEntityComponent selected=SCR_EditableEntityComponent.Cast(target.FindComponent(SCR_EditableEntityComponent));
  SCR_EditorManagerEntity editor=SCR_EditorManagerEntity.GetInstance();if(!selected || !editor)return;
  SCR_EditorModeEntity editMode=editor.FindModeEntity(EEditorMode.EDIT);
  if(phase==5 && !editMode){m_BCMMAdminSent=phase;Print("BCMM_LIFE_ADMIN_REVOKED actualModeGone=1");return;}
  if(!editMode)return;
  SCR_BaseActionsEditorComponent actions=SCR_BaseActionsEditorComponent.Cast(editMode.FindComponent(SCR_ContextActionsEditorComponent));if(!actions)return;
  array<SCR_BaseEditorAction> list={};actions.GetActions(list);BCMM_AdjustPlayerLivesContextAction chosen;
  foreach(SCR_BaseEditorAction entry:list)
  {
   BCMM_AdjustPlayerLivesContextAction allowance=BCMM_AdjustPlayerLivesContextAction.Cast(entry);if(!allowance)continue;
   if(((phase==1 || phase==4) && allowance.BCMM_TestLifeChange()==1 && !allowance.BCMM_TestLifeReset()) || (phase==2 && allowance.BCMM_TestLifeChange()==-1 && !allowance.BCMM_TestLifeReset()) || (phase==3 && allowance.BCMM_TestLifeReset()))chosen=allowance;
  }
  if(!chosen || !chosen.CanBePerformed(selected,vector.Zero,0))return;
  actions.BCMM_TestChooseLifeTarget(selected);actions.ActionPerform(chosen,vector.Zero,0);m_BCMMAdminSent=phase;
  PrintFormat("BCMM_LIFE_ADMIN_OWNER_SENT phase=%1 actualEditor=1",phase);
 }
}


class BCMM_LifeAdminRestartChecks : BCMM_FinitePersistenceRuntimeChecks
{
 override protected void Prepare()
 {
  m_Persistence=PersistenceSystem.GetInstance();BCMM_LifeAdminManifest saved=new BCMM_LifeAdminManifest();
  m_Suite.Record(m_Persistence && m_Persistence.WasDataLoaded() && saved.LoadFromFile("$profile:BCMM_LifeAdminRestart.json"),"Independent native process loads the GM allowance world and its original manifest");
  if(!m_Persistence || !m_Persistence.WasDataLoaded()){Finish();return;}
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());mode.BCMM_TestLifeIdentity(90002,saved.AccountKey);BCMM_LifeAccount account=mode.BCMM_AccountFor(90002);
  bool same=account!=null;array<int> actual;if(account)actual=account.Ledger.BCMM_CaptureLedger();same=same && actual && actual.Count()==saved.Ledger.Count();
  if(same)for(int index=0;index<actual.Count();index++)if(actual[index]!=saved.Ledger[index])same=false;
  m_Suite.Record(same && account.Ledger.RemainingDeployments(mode.BCMM_ReadSetting("totalLives"))==saved.Balance,"Actual GM grant and every exact ledger value survive a real native world restart");
  IEntity body=IEntity.Cast(m_Persistence.FindById(saved.BodyId));SCR_CharacterDamageManagerComponent medical;if(body)medical=SCR_CharacterDamageManagerComponent.Cast(body.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Suite.Record(account && account.Ledger.IsAlive() && body && account.ActiveEntity==body && medical && Math.AbsFloat(medical.GetBloodHitZone().GetHealthScaled()-0.7)<0.01,"GM allowance restart preserves the same paid native body and partial blood without healing or recreating it");
  Finish();
 }
}
