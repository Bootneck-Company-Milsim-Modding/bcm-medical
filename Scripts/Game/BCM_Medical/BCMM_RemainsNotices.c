// Confirmation travels only to the original performing player's controller.
modded class SCR_PlayerController
{
 protected int m_BCMMLastRemainsNoticeToken;
 protected BCMM_RemainsDisposition m_BCMMLastRemainsNoticeKind;
 int BCMM_LastRemainsNoticeToken() { return m_BCMMLastRemainsNoticeToken; }
 BCMM_RemainsDisposition BCMM_LastRemainsNoticeKind() { return m_BCMMLastRemainsNoticeKind; }
 void BCMM_SendRemainsNotice(int token,string deceasedName,BCMM_RemainsDisposition kind)
 {
  if(!Replication.IsServer() || token<=0 || (kind!=BCMM_RemainsDisposition.BURIED && kind!=BCMM_RemainsDisposition.SENT_HOME)) return;
  if(GetGame().GetPlayerController()==this) BCMM_ReceiveRemainsNotice(token,deceasedName,kind);
  else Rpc(BCMM_ReceiveRemainsNotice,token,deceasedName,kind);
 }
 [RplRpc(RplChannel.Reliable,RplRcver.Owner)]
 protected void BCMM_ReceiveRemainsNotice(int token,string deceasedName,BCMM_RemainsDisposition kind)
 {
  m_BCMMLastRemainsNoticeToken=token;
  m_BCMMLastRemainsNoticeKind=kind;
  string description="The remains of "+deceasedName+" have been buried.";
  if(kind==BCMM_RemainsDisposition.SENT_HOME) description="The remains of "+deceasedName+" have been sent home.";
  bool shown=SCR_HintManagerComponent.ShowCustomHint(description,"BCM Medical",8);
  PrintFormat("BCMM_REMAINS_NOTICE token=%1 state=%2 hintShown=%3",token,kind,shown);
 }
}
