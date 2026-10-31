// Original BCM transport policy. No body-bag carry mode is defined.
enum BCMM_TransportMode { NONE, CASUALTY_CARRY, CASUALTY_DRAG, BODY_BAG_MANUAL_DRAG, BODY_BAG_ROPE_DRAG }

class BCMM_CarryConsent
{
 protected int m_Request;
 protected int m_Carrier;
 protected int m_Patient;
 protected int m_PatientLife;
 protected float m_Expiry;
 protected bool m_Accepted;
 protected bool m_Consumed;
 protected float m_Created;
 protected BCMM_TransportMode m_Mode;
 // The native adapter must use authenticated player IDs and bind the grant to
 // the same original character/life. Never ask an unconscious casualty to answer.
 static bool NeedsConsent(bool conscious, bool playerControlled) { return conscious && playerControlled; }
 bool Request(int request, int carrier, int patient, int life, float now, notnull BCMM_Settings settings, BCMM_TransportMode mode = BCMM_TransportMode.CASUALTY_CARRY)
 {
  if (request <= m_Request || carrier <= 0 || patient <= 0 || carrier == patient || life <= 0 || !(now >= 0 && now < float.MAX)) return false;
  if (!m_Consumed && m_Request > 0 && now >= m_Created && now < m_Expiry) return false;
  if (mode != BCMM_TransportMode.CASUALTY_CARRY && mode != BCMM_TransportMode.CASUALTY_DRAG) return false;
  m_Request = request; m_Carrier = carrier; m_Patient = patient; m_PatientLife = life;
  m_Mode = mode;
  m_Expiry = now + settings.Get("carryConsentTimeoutSeconds");
  m_Accepted = false;
  m_Consumed = false; m_Created = now;
  return true;
 }
 bool Respond(int request, int authenticatedPatient, bool accept, float now)
 {
  if (m_Consumed || request != m_Request || authenticatedPatient != m_Patient || !(now >= m_Created && now < m_Expiry)) return false;
  m_Accepted = accept;
  if (!accept) m_Expiry = now;
  return true;
 }
 bool Consume(int request, int carrier, int patient, int life, float now, BCMM_TransportMode mode = BCMM_TransportMode.CASUALTY_CARRY)
 {
  if (!m_Accepted || mode != m_Mode || request != m_Request || carrier != m_Carrier || patient != m_Patient || life != m_PatientLife || !(now >= m_Created && now < m_Expiry)) return false;
  m_Accepted = false;
  m_Consumed = true;
  return true;
 }
 bool Cancel(int request, int authenticatedPlayer)
 {
  if (request <= 0 || authenticatedPlayer <= 0 || m_Consumed || request != m_Request || (authenticatedPlayer != m_Carrier && authenticatedPlayer != m_Patient)) return false;
  Revoke();
  return true;
 }
 void Revoke() { m_Accepted = false; m_Expiry = 0; }
}

class BCMM_BodyBagDragSession
{
 protected int m_Owner;
 protected int m_Session;
 protected BCMM_TransportMode m_Mode;
 bool Begin(int actor, int session, bool rope, bool ropeInInventory)
 {
  if (actor <= 0 || session <= 0 || m_Session != 0 || (rope && !ropeInInventory)) return false;
  m_Owner = actor; m_Session = session;
  m_Mode = BCMM_TransportMode.BODY_BAG_MANUAL_DRAG;
  if (rope) m_Mode = BCMM_TransportMode.BODY_BAG_ROPE_DRAG;
  return true;
 }
 bool Release(int actor, int session)
 {
  if (actor != m_Owner || session != m_Session || session <= 0) return false;
  ForceRelease();
  return true;
 }
 void ForceRelease() { m_Owner = 0; m_Session = 0; m_Mode = BCMM_TransportMode.NONE; }
 BCMM_TransportMode Mode() { return m_Mode; }
 bool AllowsWeapon() { return m_Mode == BCMM_TransportMode.BODY_BAG_ROPE_DRAG; }
 bool AllowsRunning() { return AllowsWeapon(); }
 bool AllowsCrouching() { return m_Mode != BCMM_TransportMode.NONE; }
 // Native adapter must sweep this desired movement against terrain/obstacles.
 // A blocked or detached bag releases the session; it must never teleport to
 // catch up through walls or across a vehicle transition.
 bool DesiredBagPosition(vector carrier, vector bag, bool carrierValid, bool pathClear, notnull BCMM_Settings settings, out vector desired)
 {
  desired = bag;
  if (m_Mode == BCMM_TransportMode.NONE) return false;
  vector offset = bag - carrier;
  float distance = offset.Length();
  if (!carrierValid || !pathClear || distance > settings.Get("ropeBreakDistanceMetres"))
  {
   ForceRelease();
   return false;
  }
  float length = settings.Get("ropeLengthMetres");
  if (distance > length) desired = carrier + offset * (length / distance);
  return true;
 }
}
