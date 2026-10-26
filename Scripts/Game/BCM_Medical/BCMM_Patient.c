// Original BCM implementation, 26 October 2026.
// Domain state only. Native damage, inventory and network adapters are separate.
enum BCMM_CardiacRhythm
{
 SINUS,
 VENTRICULAR_FIBRILLATION,
 PULSELESS_VENTRICULAR_TACHYCARDIA,
 PULSELESS_ELECTRICAL_ACTIVITY,
 ASYSTOLE
}

enum BCMM_AirwayCondition
{
 PATENT,
 SOFT_TISSUE_OBSTRUCTION,
 FOREIGN_MATERIAL_OCCLUSION
}

class BCMM_Wound
{
 int Id;
 int BodyZone;
 float UncontrolledFlowMlPerSecond;
 float CompressionFraction;
 bool Packed;
 bool Bandaged;
 BCMM_InjuryCause Cause;
 bool RetainedProjectile;
 bool Sutured;
 bool Reopened;
 protected float m_DressingWearRemaining;

 float FlowMlPerSecond()
 {
  if (Sutured) return 0;
  return Math.Max(0, UncontrolledFlowMlPerSecond) * (1 - Math.Clamp(CompressionFraction, 0, 1));
 }

 // Sampling is supplied once by the authority. Clients never roll their own
 // projectile outcome, and opening the menu cannot change an injury.
 bool Dress(notnull BCMM_Settings settings, float sample)
 {
  if (Sutured || Bandaged || !(sample >= 0 && sample <= 1)) return false;
  Bandaged = true;
  CompressionFraction = 1;
  float minimum = settings.Get("bandageReopenMinSeconds");
  m_DressingWearRemaining = minimum + sample * (settings.Get("bandageReopenMaxSeconds") - minimum);
  return true;
 }

 float Advance(float seconds, BCMM_Activity activity, notnull BCMM_Settings settings)
 {
  if (!(seconds > 0) || Sutured) return 0;
  if (!Bandaged) return FlowMlPerSecond() * seconds;
  float wear = 1;
  if (activity == BCMM_Activity.WALKING) wear = settings.Get("walkingWoundWearMultiplier");
  if (activity == BCMM_Activity.RUNNING) wear = settings.Get("runningWoundWearMultiplier");
  float protectedSeconds = Math.Min(seconds, m_DressingWearRemaining / wear);
  m_DressingWearRemaining = Math.Max(0, m_DressingWearRemaining - seconds * wear);
  if (m_DressingWearRemaining > 0) return 0;
  Bandaged = false;
  Reopened = true;
  CompressionFraction = 0;
  UncontrolledFlowMlPerSecond *= settings.Get("reopenedBleedingMultiplier");
  return FlowMlPerSecond() * (seconds - protectedSeconds);
 }

 bool CloseWithSutures()
 {
  if (Sutured || RetainedProjectile || FlowMlPerSecond() > 0) return false;
  Sutured = true;
  Bandaged = false;
  CompressionFraction = 1;
  return true;
 }
}

class BCMM_ChestSide
{
 bool OpenWound;
 bool Sealed;
 bool Tension;
 float CollapseFraction;
 float PleuralBloodMl;

 // A seal closes an external wound; it does not remove pressure or pleural blood.
 bool ApplySeal()
 {
  if (!OpenWound || Sealed) return false;
  Sealed = true;
  return true;
 }

 bool RelieveTension()
 {
  if (!Tension) return false;
  Tension = false;
  return true;
 }
}

class BCMM_Patient
{
 protected ref array<ref BCMM_Wound> m_Wounds = {};
 protected int m_NextWoundId;
 protected int m_CompressionSession;
 protected int m_CompressionActor;
 ref BCMM_ChestSide LeftChest = new BCMM_ChestSide();
 ref BCMM_ChestSide RightChest = new BCMM_ChestSide();
 BCMM_CardiacRhythm Rhythm = BCMM_CardiacRhythm.SINUS;
 BCMM_AirwayCondition Airway = BCMM_AirwayCondition.PATENT;
 bool HasSpontaneousCirculation = true;
 bool Terminal;

 int AddWound(int zone, float flowMlPerSecond)
 {
  if (Terminal || !(flowMlPerSecond >= 0)) return 0;
  BCMM_Wound wound = new BCMM_Wound();
  wound.Id = ++m_NextWoundId;
  wound.BodyZone = zone;
  wound.UncontrolledFlowMlPerSecond = flowMlPerSecond;
  m_Wounds.Insert(wound);
  return wound.Id;
 }

 BCMM_Wound FindWound(int id)
 {
  foreach (BCMM_Wound wound : m_Wounds)
   if (wound.Id == id) return wound;
  return null;
 }

 float TotalExternalFlowMlPerSecond()
 {
  float flow;
  foreach (BCMM_Wound wound : m_Wounds) flow += wound.FlowMlPerSecond();
  return flow;
 }

 bool IsShockAdvised()
 {
  if (Terminal || HasSpontaneousCirculation) return false;
  return Rhythm == BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION || Rhythm == BCMM_CardiacRhythm.PULSELESS_VENTRICULAR_TACHYCARDIA;
 }

 // Session IDs are issued by the future authority adapter, not by a client.
 // Stale completion/cancellation must never release a newer medic's session.
 bool StartCompressions(int actor, int session)
 {
  if (Terminal || HasSpontaneousCirculation || actor <= 0 || session <= 0) return false;
  if (m_CompressionSession != 0) return m_CompressionSession == session && m_CompressionActor == actor;
  m_CompressionActor = actor;
  m_CompressionSession = session;
  return true;
 }

 bool StopCompressions(int actor, int session)
 {
  if (m_CompressionActor != actor || m_CompressionSession != session || session <= 0) return false;
  m_CompressionSession = 0;
  m_CompressionActor = 0;
  return true;
 }

 bool HasCompressions() { return m_CompressionSession > 0; }

 // Mechanical compressions and a monitor rhythm do not themselves establish ROSC.
 bool CanVentilate() { return !Terminal && Airway == BCMM_AirwayCondition.PATENT; }

 void MarkTerminal()
 {
  Terminal = true;
  HasSpontaneousCirculation = false;
  m_CompressionSession = 0;
  m_CompressionActor = 0;
 }
}
