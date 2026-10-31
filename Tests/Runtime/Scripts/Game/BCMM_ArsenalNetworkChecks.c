// Diagnostic-only native arsenal RPC delivery and replicated supply accounting.
modded class SCR_BaseGameMode
{
 [RplProp()] protected int m_BCMMArsenalTransactionPhase;
 [RplProp()] protected RplId m_BCMMArsenalTransactionBox=RplId.Invalid();
 [RplProp()] protected RplId m_BCMMArsenalTransactionPack=RplId.Invalid();
 [RplProp()] protected RplId m_BCMMArsenalTransactionItem=RplId.Invalid();
 void BCMM_PublishArsenalTransaction(int phase,IEntity box,IEntity pack,RplId item)
 {
  m_BCMMArsenalTransactionPhase=phase;m_BCMMArsenalTransactionBox=BCMM_BodyBagComponent.Id(box);m_BCMMArsenalTransactionPack=BCMM_BodyBagComponent.Id(pack);m_BCMMArsenalTransactionItem=item;Replication.BumpMe();PrintFormat("BCMM_ARSENAL_PHASE %1",phase);
 }
 int BCMM_ArsenalTransactionPhase(){return m_BCMMArsenalTransactionPhase;}
 IEntity BCMM_ArsenalTransactionBox(){return BCMM_BodyBagComponent.Resolve(m_BCMMArsenalTransactionBox);}
 IEntity BCMM_ArsenalTransactionPack(){return BCMM_BodyBagComponent.Resolve(m_BCMMArsenalTransactionPack);}
 RplId BCMM_ArsenalTransactionItem(){return m_BCMMArsenalTransactionItem;}
}
modded class ArmaReforgerScripted
{
 protected int m_BCMMArsenalSent;
 protected int m_BCMMArsenalObserved;
 protected bool m_BCMMArsenalPositioned;
 protected ref SCR_ResourceSystemSubscriptionHandleBase m_BCMMArsenalSupplySubscription;
 override void OnUpdate(BaseWorld world,float timeslice)
 {
  super.OnUpdate(world,timeslice);
  string observer;if(Replication.IsServer() || !System.GetCLIParam("bcmmArsenalObserver",observer))return;
  SCR_BaseGameMode mode=SCR_BaseGameMode.Cast(GetGame().GetGameMode());SCR_PlayerController player=SCR_PlayerController.Cast(GetGame().GetPlayerController());
  if(!mode || !player || !player.GetControlledEntity())return;
  int phase=mode.BCMM_ArsenalTransactionPhase();IEntity box=mode.BCMM_ArsenalTransactionBox();IEntity pack=mode.BCMM_ArsenalTransactionPack();if(phase==0 || !box || !pack)return;
  SCR_ResourceComponent resource=SCR_ResourceComponent.Cast(box.FindComponent(SCR_ResourceComponent));if(!resource)return;
  SCR_ResourceConsumer consumer=resource.GetConsumer(EResourceGeneratorID.DEFAULT,EResourceType.SUPPLIES);if(!consumer)return;
  if(!m_BCMMArsenalSupplySubscription)
  {
   SCR_ResourcePlayerControllerInventoryComponent listener=SCR_ResourcePlayerControllerInventoryComponent.Cast(player.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
   if(!listener)return;
   m_BCMMArsenalSupplySubscription=GetGame().GetResourceSystemSubscriptionManager().RequestSubscriptionListenerHandle(consumer,Replication.FindItemId(listener));
  }
  if(player.GetPlayerId()==1 && m_BCMMArsenalSent<phase)
  {
   RplComponent owner=RplComponent.Cast(player.GetControlledEntity().FindComponent(RplComponent));if(!owner || !owner.IsOwner())return;
   if(!m_BCMMArsenalPositioned)
   {
    vector matrix[4];player.GetControlledEntity().GetWorldTransform(matrix);matrix[3]="100 1.2 0";ChimeraCharacter.Cast(player.GetControlledEntity()).Teleport(matrix);m_BCMMArsenalPositioned=true;
   }
   SCR_ResourcePlayerControllerInventoryComponent transactions=SCR_ResourcePlayerControllerInventoryComponent.Cast(player.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));if(!transactions)return;
   if(phase<=2)
   {
    BaseInventoryStorageComponent storage=BaseInventoryStorageComponent.Cast(pack.FindComponent(BaseInventoryStorageComponent));if(!storage)return;
    transactions.RpcAsk_ArsenalRequestItem(Replication.FindItemId(resource),Replication.FindItemId(storage),"{47B2D2C90145E642}Prefabs/Items/BCMM_Saline.et",EResourceType.SUPPLIES);
   }
   else transactions.RpcAsk_ArsenalRefundItem(Replication.FindItemId(resource),mode.BCMM_ArsenalTransactionItem(),EResourceType.SUPPLIES);
   m_BCMMArsenalSent=phase;PrintFormat("BCMM_ARSENAL_OWNER_SENT %1",phase);
  }
  float expected=1000;if(phase==2)expected=880;if(phase>=3)expected=925;
  if(m_BCMMArsenalObserved<phase && Math.AbsFloat(consumer.GetAggregatedResourceValue()-expected)<0.01)
  {
   m_BCMMArsenalObserved=phase;PrintFormat("BCMM_ARSENAL_OBSERVED %1 supplies=%2",phase,consumer.GetAggregatedResourceValue());
  }
 }
}