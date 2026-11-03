// Rebind a stale native character preview after item/attachment swaps. Keep
// normal item inspection and the native inventory layout in control.
modded class SCR_InventoryMenuUI
{
 protected float m_BCMMPreviewMissingSeconds;
 protected float m_BCMMPreviewRetrySeconds;
 override void OnMenuUpdate(float tDelta)
 {
  super.OnMenuUpdate(tDelta);
  if (m_BCMMPreviewRetrySeconds > 0) m_BCMMPreviewRetrySeconds -= tDelta;
  if (m_bProcessInitQueue || m_pInspectedSlot || !m_Player || !m_wPlayerRender || !m_wPlayerRender.IsVisible())
  { m_BCMMPreviewMissingSeconds = 0; return; }
  Animation skeleton = m_Player.GetAnimation();
  if (!skeleton) return;
  vector offset[4]; Math3D.MatrixIdentity4(offset); vector position;
  if (m_wPlayerRender.TryGetItemNodePositionInWidgetSpace(skeleton.GetBoneIndex("Head"), offset, position))
  { m_BCMMPreviewMissingSeconds = 0; return; }
  m_BCMMPreviewMissingSeconds += tDelta;
  // Allow asynchronous native scene replacement to settle before intervening.
  if (m_BCMMPreviewMissingSeconds < 0.75 || m_BCMMPreviewRetrySeconds > 0) return;
  m_BCMMPreviewRetrySeconds = 2;
  m_BCMMPreviewMissingSeconds = 0;
  RefreshPlayerWidget();
  Print("BCMM_INVENTORY_PREVIEW_REBOUND: stale character preview after item swap");
 }
}
