class BCMM_StockSourceLifeCodecChecks
{
 static bool Read(string json, out BCMM_StockSourceLifeState state)
 {
  SCR_PersistenceJsonLoadContext load = new SCR_PersistenceJsonLoadContext();
  return load.LoadFromString(json) && BCMM_StockSourceLifeCodec.Read(load, state);
 }
 static void Run(BCMM_DomainChecks suite)
 {
  SCR_PersistenceJsonSaveContext save = new SCR_PersistenceJsonSaveContext();
  BCMM_StockSourceLifeState source = new BCMM_StockSourceLifeState();
  source.Remaining = 42.25;
  BCMM_StockSourceLifeState restored;
  suite.Record(BCMM_StockSourceLifeCodec.Write(save, source) && Read(save.SaveToString(), restored) && restored && restored != source && restored.Remaining == 42.25, "Manual native item-timer codec round trips remaining seconds without inventory type discrimination");
  suite.Record(Read("{}", restored) && !restored, "Absent item timer remains absent for a genuinely unused or historical source");
  suite.Record(Read("{\"bcmmNativeStockLife\":{\"schema\":1,\"remaining\":0}}", restored) && restored && restored.Remaining == 0, "Zero remaining source lifetime survives decoding for immediate expiry rather than refill");
  suite.Record(!Read("{\"bcmmNativeStockLife\":{\"schema\":99,\"remaining\":10}}", restored), "Unknown item-timer schema is rejected");
  suite.Record(!Read("{\"bcmmNativeStockLife\":{\"schema\":1}}", restored), "Missing remaining lifetime cannot become a default fresh timer");
  suite.Record(!Read("{\"bcmmNativeStockLife\":{\"schema\":1,\"remaining\":-1}}", restored), "Negative item lifetime is rejected");
  suite.Record(!Read("{\"bcmmNativeStockLife\":{\"schema\":1,\"remaining\":86401}}", restored), "Out-of-range item lifetime is rejected");
  suite.Record(!Read("{\"bcmmNativeStockLife\":5}", restored), "Integer item-timer payload is rejected before native child-field lookup");
  suite.Record(!Read("{\"bcmmNativeStockLife\":1.5}", restored), "Floating item-timer payload is rejected before native child-field lookup");
  suite.Record(!Read("{\"bcmmNativeStockLife\":true}", restored), "Boolean item-timer payload is rejected before native child-field lookup");
  suite.Record(!Read("{\"bcmmNativeStockLife\":\"invalid\"}", restored), "String item-timer payload is rejected before native child-field lookup");
 }
}

