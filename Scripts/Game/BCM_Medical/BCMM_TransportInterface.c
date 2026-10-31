modded class SCR_PlayerController
{
 protected int m_BCMMTransportOfferToken;
 protected RplId m_BCMMTransportOther=RplId.Invalid();
 protected int m_BCMMTransportOfferMode;
 protected bool m_BCMMTransportIncoming;
 protected float m_BCMMTransportOfferUntil;
 int BCMM_TransportOfferToken()
 {
  if(GetGame().GetWorld().GetWorldTime()>m_BCMMTransportOfferUntil) { return 0; }
  return m_BCMMTransportOfferToken;
 }
 bool BCMM_TransportIncoming(){return BCMM_TransportOfferToken()>0 && m_BCMMTransportIncoming;}
 int BCMM_TransportOfferMode(){return m_BCMMTransportOfferMode;}
 void BCMM_SendTransportOffer(int token,RplId other,int mode,bool incoming,float seconds,string message)
 {
  if(GetGame().GetPlayerController()==this)BCMM_ReceiveTransportOffer(token,other,mode,incoming,seconds,message);
  else Rpc(BCMM_ReceiveTransportOffer,token,other,mode,incoming,seconds,message);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_ReceiveTransportOffer(int token,RplId other,int mode,bool incoming,float seconds,string message)
 {
  m_BCMMTransportOfferToken=token;m_BCMMTransportOther=other;m_BCMMTransportOfferMode=mode;m_BCMMTransportIncoming=incoming;
  m_BCMMTransportOfferUntil=GetGame().GetWorld().GetWorldTime()+seconds*1000;
  if(!BCMM_MedicalMenu.Current())
  {
   SCR_PopUpNotification notification=SCR_PopUpNotification.GetInstance();if(notification)notification.PopupMsg(message,duration:5);
  }
 }
 void BCMM_RequestLivingTransport(IEntity patient,int action,int token=0)
 {
  if(GetGame().GetPlayerController()!=this)return;
  RplId target=RplId.Invalid();if(patient)target=BCMM_BodyBagComponent.Id(patient);
  Rpc(BCMM_LivingTransportRequest,target,action,token);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Server)]
 protected void BCMM_LivingTransportRequest(RplId target,int action,int token)
 {
  if(action==1 || action==2)BCMM_LivingTransport.Request(this,BCMM_BodyBagComponent.Resolve(target),action);
  else if(action==3)BCMM_LivingTransport.Respond(this,token,true,false);
  else if(action==4)BCMM_LivingTransport.Respond(this,token,false,false);
  else if(action==5)BCMM_LivingTransport.Respond(this,token,false,true);
  else if(action==6)BCMM_LivingTransport.Release(GetControlledEntity());
 }
}
