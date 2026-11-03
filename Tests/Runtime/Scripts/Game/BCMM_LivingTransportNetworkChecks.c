// Unpublished diagnostics: actual owner RPCs and late-join observations.
modded class BCMM_LivingTransport
{
 static int TestOfferToken()
 {
  if(s_Offers.Count()>0) { return s_Offers[0].Token; }
  return 0;
 }
 static int TestLastOfferToken(){return s_NextToken;}
}
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMLivingTestPhase;
 [RplProp()] protected RplId m_BCMMLivingTestActor=RplId.Invalid();
 [RplProp()] protected RplId m_BCMMLivingTestPatient=RplId.Invalid();
 [RplProp()] protected RplId m_BCMMLivingTestWeapon=RplId.Invalid();
 [RplProp()] protected int m_BCMMLivingCancelledOffer;
 protected ref array<int> m_BCMMLivingObservationAcks={};
 void BCMM_LivingObservationAck(int playerId,int phase)
 {
  int key=phase*10+playerId;
  if(Replication.IsServer() && playerId>0 && playerId<=3 && phase==m_BCMMLivingTestPhase && !m_BCMMLivingObservationAcks.Contains(key))m_BCMMLivingObservationAcks.Insert(key);
 }
 bool BCMM_LivingObservedBy(int playerId,int phase){return m_BCMMLivingObservationAcks.Contains(phase*10+playerId);}
 bool BCMM_LivingObservedByAll(int phase){return BCMM_LivingObservedBy(1,phase) && BCMM_LivingObservedBy(2,phase) && BCMM_LivingObservedBy(3,phase);}
 void BCMM_PublishCancelledOffer(int token){m_BCMMLivingCancelledOffer=token;Replication.BumpMe();}
 int BCMM_LivingCancelledOffer(){return m_BCMMLivingCancelledOffer;}
 void BCMM_PublishLivingTest(int phase,IEntity actor,IEntity patient)
 {
  m_BCMMLivingTestPhase=phase;m_BCMMLivingTestActor=BCMM_BodyBagComponent.Id(actor);m_BCMMLivingTestPatient=BCMM_BodyBagComponent.Id(patient);
  Replication.BumpMe();PrintFormat("BCMM_LIVING_NET_PHASE %1",phase);
 }
 int BCMM_LivingTestPhase(){return m_BCMMLivingTestPhase;}
 RplId BCMM_LivingTestActor(){return m_BCMMLivingTestActor;}
 RplId BCMM_LivingTestPatient(){return m_BCMMLivingTestPatient;}
 RplId BCMM_LivingTestWeapon(){return m_BCMMLivingTestWeapon;}
 void BCMM_PublishLivingWeapon(IEntity weapon){m_BCMMLivingTestWeapon=BCMM_BodyBagComponent.Id(weapon);Replication.BumpMe();}
}
modded class SCR_PlayerController
{
 void BCMM_LivingObservationAck(int phase){Rpc(BCMM_LivingObservationAckServer,phase);}
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_LivingObservationAckServer(int phase){SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_LivingObservationAck(GetPlayerId(),phase);}
}
class BCMM_LivingTransportNetworkChecks : BCMM_SplintRuntimeChecks
{
 protected int m_Phase;
 protected int m_Attempts;
 protected float m_PhaseAt;
 protected bool m_HadOffer;
 protected IEntity m_NetworkWeapon;
 protected IEntity m_NetworkSling;
 protected int m_OfferBaseline;
 protected vector m_WalkStart;
 protected vector m_WalkForward;
 protected float m_WalkDistance;
 protected float m_WalkHeadingDot=1;
 protected IEntity m_CarryHolder;
 protected IEntity m_DragHolder;
 protected float m_ReleaseObservedAt;
 protected float m_NextFinalTrace;
 override protected void Prepare()
 {
  SCR_EntityHelper.DeleteEntityAndChildren(m_Floor);
  m_Floor=Spawn("{B6C4ED1CA1262110}Prefabs/BCMM_StretcherTestPlatform.et","100 1 0");
  if(m_Actor)SCR_EntityHelper.DeleteEntityAndChildren(m_Actor);
  if(m_Target)SCR_EntityHelper.DeleteEntityAndChildren(m_Target);
  m_Actor=null;m_Target=null;
  GetGame().GetCallqueue().CallLater(WaitOwners,250);
 }
 protected void WaitOwners()
 {
  SCR_PlayerController a=BCMM_LivingTransport.Player(1);SCR_PlayerController p=BCMM_LivingTransport.Player(2);
  if(!a || !p || !a.GetControlledEntity() || !p.GetControlledEntity())
  {
   if(m_Attempts++<240){GetGame().GetCallqueue().CallLater(WaitOwners,250);return;}
   m_Suite.Record(false,"Two native transport owners join and possess characters");m_Suite.Complete();return;
  }
  m_Actor=a.GetControlledEntity();m_Target=p.GetControlledEntity();m_Attempts=0;
  m_Medical=SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
  m_Medical.BCMM_GetPatient().LeftLeg.Broken=true;m_Medical.BCMM_PublishFractures();
  Publish(1);GetGame().GetCallqueue().CallLater(TickNetwork,250,true);
 }
 protected void Publish(int phase)
 {
  m_Phase=phase;m_PhaseAt=GetGame().GetWorld().GetWorldTime();m_HadOffer=false;m_ReleaseObservedAt=0;
  if(phase==17)m_OfferBaseline=BCMM_LivingTransport.TestLastOfferToken();
  if(phase==4 || phase==7){m_WalkStart=m_Actor.GetOrigin();vector frame[4];m_Actor.GetWorldTransform(frame);m_WalkForward=frame[2];m_WalkDistance=0;m_WalkHeadingDot=1;}
  SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLivingTest(phase,m_Actor,m_Target);
 }
 protected void TickNetwork()
 {
  float elapsed=(GetGame().GetWorld().GetWorldTime()-m_PhaseAt)*0.001;
  if(elapsed>25){FailNetwork("Transport network phase timed out "+m_Phase);return;}
  int mode=m_Medical.BCMM_TransportMode();int token=BCMM_LivingTransport.TestOfferToken();
  SCR_BaseGameMode observations=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
  if(m_Phase==17 && elapsed>=m_NextFinalTrace)
  {
   m_NextFinalTrace=elapsed+1;
   PrintFormat("BCMM_LIVING_FINAL_TRACE elapsed=%1 offer=%2 latest=%3 baseline=%4 eligible=%5 distance=%6",elapsed,token,BCMM_LivingTransport.TestLastOfferToken(),m_OfferBaseline,BCMM_LivingTransport.Eligible(m_Actor,m_Target,1),vector.Distance(m_Actor.GetOrigin(),m_Target.GetOrigin()));
  }
  if((m_Phase==5 || m_Phase==8) && mode==0 && m_ReleaseObservedAt==0)m_ReleaseObservedAt=GetGame().GetWorld().GetWorldTime();
  if(m_Phase==4 || m_Phase==7){vector walkingFrame[4];m_Actor.GetWorldTransform(walkingFrame);m_WalkDistance=Math.Max(m_WalkDistance,vector.Distance(m_WalkStart,m_Actor.GetOrigin()));m_WalkHeadingDot=Math.Min(m_WalkHeadingDot,vector.Dot(m_WalkForward,walkingFrame[2]));}
  if(m_Phase==6 && elapsed>3 && elapsed<3.3)PrintFormat("BCMM_LIVING_DIAG mode=%1 offer=%2 eligible=%3 actor=%4 patient=%5 acfall=%6 pcfall=%7",mode,token,BCMM_LivingTransport.Eligible(m_Actor,m_Target,2),m_Actor.GetOrigin(),m_Target.GetOrigin(),BCMM_LivingTransport.Controller(m_Actor).IsFalling(),BCMM_LivingTransport.Controller(m_Target).IsFalling());
  if(m_Phase==6 && elapsed>3 && elapsed<3.3)PrintFormat("BCMM_LIVING_EXIT_DIAG vehicle=%1 parent=%2 slot=%3 actorItem=%4 patientItem=%5 busy=%6 actorVehicle=%7",ChimeraCharacter.Cast(m_Target).IsInVehicle(),m_Target.GetParent(),ChimeraCharacter.Cast(m_Target).GetCompartmentAccessComponent().GetVehicleCompartmentManagerOwner(),BCMM_LivingTransport.Controller(m_Actor).IsUsingItem(),BCMM_LivingTransport.Controller(m_Target).IsUsingItem(),BCMM_SplintTreatment.IsBusy(m_Actor,m_Target),ChimeraCharacter.Cast(m_Actor).IsInVehicle());
  if(m_Phase==1 && token>0 && elapsed>3)
  {
   m_Suite.Record(mode==0,"Requesting owner cannot accept their own carry request through the actual RPC");Publish(2);return;
  }
  if(m_Phase==2 && token==0 && elapsed>1)
  {
   m_Suite.Record(mode==0,"Conscious patient declines through medical menu without being moved");Publish(3);return;
  }
  if(m_Phase==3 && mode==1)
  {
   m_Suite.Record(!ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious() && m_Medical.BCMM_GetPatient().LeftLeg.Broken,"Conscious broken-leg owner accepts carry without manufactured unconsciousness or healing");Publish(4);return;
  }
  if(m_Phase==4 && elapsed>10 && observations.BCMM_LivingObservedByAll(4))
  {
   vector carryFrame[4];m_Actor.GetWorldTransform(carryFrame);
   PrintFormat("BCMM_NATIVE_WALK mode=1 distance=%1 headingDot=%2",m_WalkDistance,m_WalkHeadingDot);
   m_Suite.Record(m_WalkDistance>0.3 && m_WalkDistance<4.5 && m_WalkHeadingDot<0.9,"Actual rescuer owner walks at limited speed and changes heading while carrying the original casualty");
   m_CarryHolder=m_Medical.BCMM_PoseAnchorEntity();
   m_Suite.Record(mode==1 && m_Medical.BCMM_TransportActor()==m_Actor,"Owner-carried original character remains active through late join");Publish(5);return;
  }
  if(m_Phase==5 && mode==0 && elapsed>2 && vector.Distance(m_Actor.GetOrigin(),m_Target.GetOrigin())<1.8)
  {
   // Cleanup intentionally waits 500 ms, then up to 20 native exit retries.
   // Measure from observed authority release, not from the earlier phase RPC.
   if(m_CarryHolder && GetGame().GetWorld().GetWorldTime()-m_ReleaseObservedAt<7000)return;
   PrintFormat("BCMM_LIVING_RELEASE_CLEANUP phase=5 milliseconds=%1",GetGame().GetWorld().GetWorldTime()-m_ReleaseObservedAt);
   m_Suite.Record(!m_CarryHolder,"Dedicated release removes the empty carried-pose holder");
   m_Suite.Record(!BCMM_LivingTransport.ForParticipant(m_Target),"Carried patient can release via their own medical menu");Publish(6);return;
  }
  if(m_Phase==6 && mode==2){Publish(7);return;}
  if(m_Phase==7 && elapsed>8 && observations.BCMM_LivingObservedByAll(7))
  {
   vector dragFrame[4];m_Actor.GetWorldTransform(dragFrame);
   PrintFormat("BCMM_NATIVE_WALK mode=2 distance=%1 headingDot=%2",m_WalkDistance,m_WalkHeadingDot);
   m_Suite.Record(m_WalkDistance>0.15 && m_WalkDistance<2.5 && m_WalkHeadingDot<0.9,"Actual rescuer owner crouch-walks at limited speed and changes heading while dragging the original casualty");
   m_DragHolder=m_Medical.BCMM_PoseAnchorEntity();
   m_Suite.Record(mode==2 && ChimeraCharacter.Cast(m_Actor).GetCharacterController().GetStance()==ECharacterStance.CROUCH,"Owner's native crouch-drag persists on dedicated authority");Publish(8);return;
  }
  if(m_Phase==8 && mode==0 && elapsed>2 && vector.Distance(m_Actor.GetOrigin(),m_Target.GetOrigin())<1.8)
  {
   if(m_DragHolder && GetGame().GetWorld().GetWorldTime()-m_ReleaseObservedAt<7000)return;
   PrintFormat("BCMM_LIVING_RELEASE_CLEANUP phase=8 milliseconds=%1",GetGame().GetWorld().GetWorldTime()-m_ReleaseObservedAt);
   m_Suite.Record(!m_DragHolder,"Dedicated release removes the empty dragged-pose holder");
   m_Suite.Record(!BCMM_LivingTransport.ForParticipant(m_Actor),"Carrier can release through their medical menu");
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_TestRoleSetting("carryConsentTimeoutSeconds",3);Publish(9);return;
  }
  if(m_Phase==9)
  {
   if(token>0)m_HadOffer=true;
   if(m_HadOffer && token==0 && elapsed>4)
   {
    m_Suite.Record(mode==0,"Unanswered conscious-player consent expires without pickup");Publish(10);return;
   }
  }
  if(m_Phase==10 && elapsed>6 && observations.BCMM_LivingObservedByAll(10))
  {
   m_Suite.Record(!ChimeraCharacter.Cast(m_Target).GetCharacterController().IsUnconscious() && m_Medical.BCMM_GetPatient().LeftLeg.Broken,"Transport lifecycle preserves original patient injury and consciousness");
   InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
   IEntity pack=Spawn("{5C5C6EE05EE2FF1A}Prefabs/Items/Equipment/Backpacks/Backpack_ALICE_Medium_assembled.et",m_Target.GetOrigin());
   m_Suite.Record(pack && inventory.TryInsertItem(pack,EStoragePurpose.PURPOSE_ANY),"Actual patient owner receives native inventory for retention");
   m_NetworkWeapon=Spawn("{F97A4AC994231900}Prefabs/Weapons/Rifles/M16/Rifle_M16A2_carbine.et",m_Target.GetOrigin());m_NetworkSling=Spawn(BCMM_WeaponRetention.SLING,m_Target.GetOrigin());
   m_Suite.Record(m_NetworkWeapon && m_NetworkSling && inventory.TryInsertItem(m_NetworkWeapon,EStoragePurpose.PURPOSE_ANY) && inventory.TryInsertItem(m_NetworkSling,EStoragePurpose.PURPOSE_ANY),"Original rifle and imported sling enter the connected patient's inventory");
   SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishLivingWeapon(m_NetworkWeapon);Publish(11);return;
  }
  if(m_Phase==11)
  {
   BaseWeaponManagerComponent weapons=BCMM_LivingTransport.Controller(m_Target).GetWeaponManagerComponent();
   if(weapons.GetCurrentWeapon() && weapons.GetCurrentWeapon().GetOwner()==m_NetworkWeapon)
   {
    m_Suite.Record(true,"Actual connected patient owner equips the exact original rifle");
    m_Medical.GetBloodHitZone().SetHealth(0);m_Medical.UpdateConsciousness();Publish(12);return;
   }
  }
  if(m_Phase==12 && elapsed>3)
  {
   InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(m_Target.FindComponent(InventoryStorageManagerComponent));
   m_Suite.Record(BCMM_LivingTransport.Controller(m_Target).IsUnconscious() && inventory.Contains(m_NetworkWeapon) && inventory.Contains(m_NetworkSling),"Dedicated authority retains the same original rifle and non-consumed sling through real owner unconsciousness");
   Publish(13);return;
  }
  if(m_Phase==13 && elapsed>6 && observations.BCMM_LivingObservedBy(2,13))
  {
   Publish(14);return;
  }
  if(m_Phase==14 && mode==1)
  {
   m_Suite.Record(token==0 && BCMM_LivingTransport.Controller(m_Target).IsUnconscious(),"Actual unconscious player is carried immediately without a consent offer");Publish(15);return;
  }
  if(m_Phase==15 && elapsed>3 && observations.BCMM_LivingObservedByAll(15))
  {
   // A deliberate fixture heal tests the native waking transition. Transport
   // itself must never manufacture consciousness or clinical recovery.
   m_Medical.FullHeal();Publish(16);return;
  }
  if(m_Phase==16 && elapsed>2 && mode==0 && !ChimeraCharacter.Cast(m_Target).GetCompartmentAccessComponent().IsInCompartment() && observations.BCMM_LivingObservedBy(2,16))
  {
   m_Suite.Record(!BCMM_LivingTransport.Controller(m_Target).IsUnconscious(),"Waking a previously unconscious carried player automatically releases them for fresh consent");
   Publish(17);return;
  }
  if(m_Phase==17)
  {
   if(BCMM_LivingTransport.TestLastOfferToken()>m_OfferBaseline)m_HadOffer=true;
   if(m_HadOffer && token==0 && elapsed>1){m_Suite.Record(mode==0,"Requesting owner cancels conscious carry through the compact medical menu");SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_PublishCancelledOffer(BCMM_LivingTransport.TestLastOfferToken());Publish(18);return;}
  }
  if(m_Phase==18 && elapsed>2 && observations.BCMM_LivingObservedBy(2,18))
  {
   m_Suite.Record(mode==0 && token==0,"Accepting a cancelled transport token cannot move the original player");
   GetGame().GetCallqueue().Remove(TickNetwork);m_Suite.Complete();
  }
 }
 protected void FailNetwork(string reason)
 {
  GetGame().GetCallqueue().Remove(TickNetwork);BCMM_LivingTransport.Release(m_Actor);m_Suite.Record(false,reason);m_Suite.Complete();
 }
}
class BCMM_LivingPoseCapture
{
 static void Capture(int phase){System.MakeScreenshot("$profile:living-network-pose-"+phase);}
}
modded class ArmaReforgerScripted
{
 protected bool m_BCMMLivingObserverParked;
 protected int m_BCMMLivingCancelledToken;
 protected int m_BCMMLivingObserved;
 protected int m_BCMMLivingSent;
 protected int m_BCMMLivingAnsweredOffer;
 protected int m_BCMMLivingStaleWaitPhase;
 protected float m_BCMMLivingNext;
 protected SCR_ManualCamera m_BCMMLivingReviewCamera;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  string flag;if(Replication.IsServer() || !System.GetCLIParam("bcmmLivingTransportObserver",flag) || world.GetWorldTime()<m_BCMMLivingNext)return;
  m_BCMMLivingNext=world.GetWorldTime()+200;
  SCR_BaseGameMode game=SCR_BaseGameMode.Cast(GetGameMode());SCR_PlayerController owner=SCR_PlayerController.Cast(GetPlayerController());
  if(!game || !owner || !owner.GetControlledEntity())return;
  int phase=game.BCMM_LivingTestPhase();IEntity patient=BCMM_BodyBagComponent.Resolve(game.BCMM_LivingTestPatient());IEntity actor=BCMM_BodyBagComponent.Resolve(game.BCMM_LivingTestActor());
  if(!patient || !actor || phase<=0)return;
  SCR_CharacterDamageManagerComponent state=SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));if(!state)return;
  int id=owner.GetPlayerId();
  if(id==3 && !m_BCMMLivingObserverParked){vector observer[4];owner.GetControlledEntity().GetWorldTransform(observer);observer[3]="100 1.1 4";ChimeraCharacter.Cast(owner.GetControlledEntity()).Teleport(observer);m_BCMMLivingObserverParked=true;}
  IEntity testWeapon=BCMM_BodyBagComponent.Resolve(game.BCMM_LivingTestWeapon());
  if(id==2 && phase==11 && testWeapon)
  {
   if(BCMM_MedicalMenu.Current())BCMM_MedicalMenu.Current().Close();
   CharacterControllerComponent control=BCMM_LivingTransport.Controller(patient);
   if(!control.GetWeaponManagerComponent().GetCurrentWeapon() || control.GetWeaponManagerComponent().GetCurrentWeapon().GetOwner()!=testWeapon)control.TryEquipRightHandItem(testWeapon,EEquipItemType.EEquipTypeWeapon,true);
  }
  if(id==1 && (phase==5 || phase==8 || phase==14 || phase==17) && state.BCMM_TransportMode()==0 && vector.Distance(actor.GetOrigin(),patient.GetOrigin())>1.8)
  {
   // A released prone player's root can settle farther away. Approach again
   // before making the separate drag request, as a real rescuer must do.
   vector approach[4];actor.GetWorldTransform(approach);approach[3]=patient.GetOrigin()+"1.65 0.1 0";
   ChimeraCharacter.Cast(actor).Teleport(approach);m_BCMMLivingSent=0;return;
  }
  if(id==1 && phase==1 && owner.BCMM_TransportOfferToken()>0 && m_BCMMLivingSent!=101)
  {owner.BCMM_RequestLivingTransport(null,3,owner.BCMM_TransportOfferToken());m_BCMMLivingSent=101;}
  else if(id==1 && (phase==1 || phase==3 || phase==6 || phase==9 || phase==14 || phase==17) && m_BCMMLivingSent!=phase && !(phase==17 && m_BCMMLivingSent==117) && owner.BCMM_TransportOfferToken()==0)
  {
   int action=1;if(phase==6)action=2;
   // The final case starts a NEW conscious request after waking/release.
   // Phase replication can precede the patient's exit and transport state.
   // A real menu still offers release until those updates arrive; do not
   // bypass that lifecycle by firing a one-shot request from the phase alone.
   if(phase==17 && (state.BCMM_TransportMode()!=0 || BCMM_LivingTransport.Controller(patient).IsUnconscious() || !BCMM_LivingTransport.Eligible(actor,patient,action)))return;
   PrintFormat("BCMM_LIVING_OWNER_REQUEST phase=%1 eligible=%2 distance=%3 mode=%4",phase,BCMM_LivingTransport.Eligible(actor,patient,action),vector.Distance(actor.GetOrigin(),patient.GetOrigin()),state.BCMM_TransportMode());
   owner.BCMM_RequestLivingTransport(patient,action);m_BCMMLivingSent=phase;
  }
  if(phase==17 && owner.BCMM_TransportOfferToken()>0)
  {
   m_BCMMLivingCancelledToken=owner.BCMM_TransportOfferToken();
   if(id==1 && m_BCMMLivingSent!=117)
   {
    if(!BCMM_MedicalMenu.Current()){BCMM_MedicalMenu.Open(actor);return;}
    SCR_ButtonTextComponent cancel=SCR_ButtonTextComponent.GetButtonText("TransportCarry",BCMM_MedicalMenu.Current().GetRootWidget());
    if(cancel){cancel.m_OnClicked.Invoke(cancel);m_BCMMLivingSent=117;}
   }
  }
  if(id==2 && phase==18 && m_BCMMLivingSent!=18 && game.BCMM_LivingCancelledOffer()>0){owner.BCMM_RequestLivingTransport(null,3,game.BCMM_LivingCancelledOffer());m_BCMMLivingSent=18;PrintFormat("BCMM_LIVING_REPLAY_CANCELLED player=2 token=%1",game.BCMM_LivingCancelledOffer());owner.BCMM_LivingObservationAck(18);}
  bool responsePhase=phase==2 || phase==3 || phase==6;
  int incomingToken=owner.BCMM_TransportOfferToken();
  if(id==2 && responsePhase && owner.BCMM_TransportIncoming() && incomingToken==m_BCMMLivingAnsweredOffer && m_BCMMLivingStaleWaitPhase!=phase)
  {
   m_BCMMLivingStaleWaitPhase=phase;PrintFormat("BCMM_LIVING_STALE_OFFER_WAIT phase=%1 token=%2",phase,incomingToken);
  }
  // Phase replication may precede the reliable clear/new-offer notifications.
  // Do not mistake an already answered token for the next consent request.
  if(id==2 && (responsePhase && owner.BCMM_TransportIncoming() && incomingToken!=m_BCMMLivingAnsweredOffer || phase==5 && state.BCMM_TransportMode()>0) && m_BCMMLivingSent!=phase)
  {
   if(!BCMM_MedicalMenu.Current()){BCMM_MedicalMenu.Open(owner.GetControlledEntity());return;}
   Widget root=BCMM_MedicalMenu.Current().GetRootWidget();string name="TransportCarry";if(phase==2)name="TransportDrag";
   SCR_ButtonTextComponent button=SCR_ButtonTextComponent.GetButtonText(name,root);
   if(button){if(responsePhase)m_BCMMLivingAnsweredOffer=incomingToken;PrintFormat("BCMM_LIVING_MENU_RESPONSE phase=%1 token=%2",phase,incomingToken);button.m_OnClicked.Invoke(button);m_BCMMLivingSent=phase;System.MakeScreenshot("$profile:transport-menu-phase-"+phase);}
  }
  if(id==1 && phase==8 && m_BCMMLivingSent!=phase)
  {
   if(!BCMM_MedicalMenu.Current()){BCMM_MedicalMenu.Open(actor);return;}
   SCR_ButtonTextComponent release=SCR_ButtonTextComponent.GetButtonText("TransportCarry",BCMM_MedicalMenu.Current().GetRootWidget());
   if(release){release.m_OnClicked.Invoke(release);m_BCMMLivingSent=phase;}
  }
  int expected=0;if(phase==4)expected=1;if(phase==7)expected=2;
  if(phase==15)expected=1;
  if(id==2 && phase==16 && phase!=m_BCMMLivingObserved && !BCMM_LivingTransport.Controller(patient).IsUnconscious() && state.BCMM_TransportMode()==0 && !ChimeraCharacter.Cast(patient).GetCompartmentAccessComponent().IsInCompartment() && !BCMM_LivingTransport.Controller(patient).GetDisableMovementControls() && !BCMM_LivingTransport.Controller(patient).GetDisableWeaponControls())
  {Print("BCMM_LIVING_NET_OBSERVED phase=16 player=2 controlsRestored=1");m_BCMMLivingObserved=16;owner.BCMM_LivingObservationAck(16);}
  if(phase==13 && phase!=m_BCMMLivingObserved && testWeapon)
  {
   InventoryStorageManagerComponent inventory=InventoryStorageManagerComponent.Cast(patient.FindComponent(InventoryStorageManagerComponent));
   if(inventory && inventory.Contains(testWeapon) && BCMM_WeaponRetention.FindSling(inventory) && BCMM_LivingTransport.Controller(patient).IsUnconscious())
   {
    PrintFormat("BCMM_LIVING_NET_OBSERVED phase=13 player=%1 originalWeapon=%2 sling=1",id,game.BCMM_LivingTestWeapon());m_BCMMLivingObserved=13;owner.BCMM_LivingObservationAck(13);
   }
  }
  if((phase==4 || phase==7 || phase==10 || phase==15) && phase!=m_BCMMLivingObserved && state.BCMM_TransportMode()==expected)
  {
   if(expected>0 && state.BCMM_TransportActor()!=actor)return;
   if(expected>0)
   {
    if(!BCMM_PoseAnchor.Contains(state.BCMM_PoseAnchorEntity(),patient))return;
    vector poseHips[4];patient.GetAnimation().GetBoneMatrix(patient.GetAnimation().GetBoneIndex("Hips"),poseHips);
    if(expected==1 && poseHips[3][1]<1.35)return;
    if(expected==2 && (poseHips[3][1]<0.15 || poseHips[3][1]>0.45))return;
   }
   else if(ChimeraCharacter.Cast(patient).GetCompartmentAccessComponent().IsInCompartment())return;
   Physics transportBody=patient.GetPhysics();
   if(!transportBody)return;
   bool collisionSuppressed=transportBody.GetInteractionLayer()==EPhysicsLayerPresets.FireGeo;
   if(collisionSuppressed!=(expected>0)){Print("BCMM_CHECK FAILED: Living transport peer collision policy");return;}
   PrintFormat("BCMM_LIVING_NET_COLLISION phase=%1 player=%2 mask=%3",phase,id,transportBody.GetInteractionLayer());
   float separation=vector.Distance(patient.GetOrigin(),actor.GetOrigin());
   if(expected>0 && separation>2.5)return;
   PrintFormat("BCMM_LIVING_NET_OBSERVED phase=%1 player=%2 mode=%3 separation=%4 owner=%5",phase,id,expected,separation,ChimeraCharacter.Cast(patient).GetRplComponent().IsOwner());
   m_BCMMLivingObserved=phase;
   owner.BCMM_LivingObservationAck(phase);
   if(id==3 && expected>0 && GetGame().GetCameraManager())
   {
    vector centre=actor.GetOrigin()+"0 0.85 0";vector origin=centre+"2.8 0.9 2.1";
    EntitySpawnParams spawn=new EntitySpawnParams();spawn.TransformMode=ETransformMode.WORLD;Math3D.AnglesToMatrix((centre-origin).VectorToAngles(),spawn.Transform);spawn.Transform[3]=origin;
    if(!m_BCMMLivingReviewCamera)m_BCMMLivingReviewCamera=SCR_ManualCamera.Cast(GetGame().SpawnEntity(SCR_ManualCamera,GetGame().GetWorld(),spawn));
    m_BCMMLivingReviewCamera.SetWorldTransform(spawn.Transform);m_BCMMLivingReviewCamera.SetInputEnabled(false);m_BCMMLivingReviewCamera.SetVerticalFOV(45);
    GetGame().GetCameraManager().SetCamera(m_BCMMLivingReviewCamera);m_BCMMLivingReviewCamera.ApplyTransform(0);
    GetGame().GetCallqueue().CallLater(BCMM_LivingPoseCapture.Capture,500,false,phase);
   }
  }
 }
}
