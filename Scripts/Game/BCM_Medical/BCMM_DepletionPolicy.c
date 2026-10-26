// Original BCM implementation. Times are authority monotonic seconds.
// Reserve is the engine blood-hit-zone health, not physical total blood volume.
class BCMM_DepletionPolicy
{
 protected bool m_Active;
 protected bool m_Expired;
 protected float m_DeadlineSeconds;
 protected float m_LastTimeSeconds = -1;

 bool IsActive() { return m_Active; }
 bool IsExpired() { return m_Expired; }
 float DeadlineSeconds() { return m_DeadlineSeconds; }

 // A positive reserve set by an unrelated script does not silently rescue the
 // casualty. The authority adapter records delivered fluid separately.
 bool Observe(float nowSeconds, float reserveHealth, float graceSeconds = 60)
 {
  if (!(nowSeconds >= 0) || !(reserveHealth >= 0) || !(graceSeconds > 0)) return m_Expired;
  if (nowSeconds < m_LastTimeSeconds) return m_Expired;
  m_LastTimeSeconds = nowSeconds;
  if (m_Expired) return true;
  if (!m_Active && reserveHealth <= 0)
  {
   m_Active = true;
   m_DeadlineSeconds = nowSeconds + graceSeconds;
  }
  if (m_Active && nowSeconds >= m_DeadlineSeconds) m_Expired = true;
  return m_Expired;
 }

 bool RecordDeliveredRescueFluid(float nowSeconds, float deliveredMl, float reserveAfterDelivery)
 {
  if (!m_Active || m_Expired || !(nowSeconds >= m_LastTimeSeconds)) return false;
  // Boundary order is deterministic: rescue at/after expiry is too late.
  if (nowSeconds >= m_DeadlineSeconds)
  {
   m_Expired = true;
   return false;
  }
  if (!(deliveredMl > 0) || !(reserveAfterDelivery > 0)) return false;
  m_LastTimeSeconds = nowSeconds;
  m_Active = false;
  m_DeadlineSeconds = 0;
  return true;
 }

 // Explicit authority GM full-heal only; ordinary treatment must not call this.
 void ResetByFullHeal()
 {
  m_Active = false;
  m_Expired = false;
  m_DeadlineSeconds = 0;
 }
}
