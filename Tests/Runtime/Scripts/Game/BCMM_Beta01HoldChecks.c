// Actual authority reservations against one real corpse and physical kit.
// These are 200 cancelled/rejected holds, not 200 completed body conversions.
modded class BCMM_BaggingRuntimeChecks
{
 override protected void Bag()
 {
  string enabled;
  if (System.GetCLIParam("bcmmBeta01Hold", enabled))
  {
   for (int iteration = 0; iteration < 200; iteration++)
   {
    m_Suite.Record(BCMM_BaggingOperation.Begin(m_Actor, m_Corpse, true), "BT01 real corpse hold reservation accepted");
    m_Suite.Record(!BCMM_BaggingOperation.Begin(m_Actor, m_Corpse, true), "BT01 duplicate real hold reservation rejected");
    BCMM_BaggingOperation.HoldResponse(m_Actor, m_Corpse, iteration % 2 == 0);
    m_Suite.Record(!BCMM_BaggingOperation.IsBusy(m_Corpse) && m_Empty && m_Corpse, "BT01 early completion or released hold preserves physical corpse and bag");
   }
   Print("BCMM_BETA_HOLD attempts=200 expectedEarlyRejections=100 explicitCancellations=100 completedConversions=0");
  }
  super.Bag();
 }
}
