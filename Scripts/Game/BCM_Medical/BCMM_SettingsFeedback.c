// Original BCM owner-targeted settings acknowledgement.
modded class SCR_PlayerController
{
 void BCMM_SendSettingsResult(string message)
 {
  if (!Replication.IsServer()) return;
  if (GetGame().GetPlayerController() == this) BCMM_ReceiveSettingsResult(message);
  else Rpc(BCMM_ReceiveSettingsResult, message);
 }
 [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
 protected void BCMM_ReceiveSettingsResult(string message)
 {
  SCR_PopUpNotification notification = SCR_PopUpNotification.GetInstance();
  if (notification) notification.PopupMsg(message, duration: 6);
 }
}