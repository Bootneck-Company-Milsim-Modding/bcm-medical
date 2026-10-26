// Original BCM deployment ledger. No RespawnLimiter implementation included.
// The future authority adapter keys these records by verified backend identity.
// Connection IDs are deliberately absent from the accounting model.
class BCMM_LifeLedger
{
 protected int m_Deployments;
 protected int m_Adjustment;
 protected int m_Sequence;
 protected int m_Reservation;
 protected int m_CurrentLife;
 protected bool m_Alive;
 protected bool m_DispositionBlocked;

 int RemainingDeployments(int totalLives)
 {
  return Math.Max(0, Math.Clamp(totalLives, 1, 1001) + m_Adjustment - m_Deployments);
 }

 bool IsAlive() { return m_Alive; }
 bool IsDispositionBlocked() { return m_DispositionBlocked; }
 int CurrentLife() { return m_CurrentLife; }
 void SetDispositionBlocked(bool blocked) { m_DispositionBlocked = blocked; }

 // Reserve before native spawning, commit only on successful finalisation.
 // This closes concurrent spawn attempts without charging failed spawns.
 int ReserveDeployment(int totalLives, bool limiterEnabled = true)
 {
  if (m_Alive || m_Reservation > 0 || m_DispositionBlocked) return 0;
  if (limiterEnabled && RemainingDeployments(totalLives) <= 0) return 0;
  m_Reservation = ++m_Sequence;
  return m_Reservation;
 }

 bool CancelDeployment(int reservation)
 {
  if (reservation <= 0 || reservation != m_Reservation) return false;
  m_Reservation = 0;
  return true;
 }

 bool CommitDeployment(int reservation)
 {
  if (reservation <= 0 || reservation != m_Reservation || m_Alive) return false;
  m_Reservation = 0;
  m_CurrentLife = reservation;
  m_Alive = true;
  m_Deployments++;
  return true;
 }

 // Authority confirms an actual native death for the registered life token.
 // Repeated callbacks or a corpse from an older deployment cannot charge again.
 bool ConfirmDeath(int life)
 {
  if (!m_Alive || life <= 0 || life != m_CurrentLife) return false;
  m_Alive = false;
  return true;
 }

 bool AdjustAllowance(int change)
 {
  if (change != 1 && change != -1) return false;
  if (m_Adjustment + change > 1000000 || m_Adjustment + change < -1000000) return false;
  m_Adjustment += change;
  return true;
 }

 // Preserve the active life/death state and invalidate pending requests. Reset
 // grants a fresh default balance, counting the current life if already alive.
 void ResetAllowance()
 {
  m_Adjustment = 0;
  m_Deployments = 0;
  if (m_Alive) m_Deployments = 1;
  m_Reservation = 0;
 }
}
