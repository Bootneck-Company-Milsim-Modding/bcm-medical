// Original manual stock-item lifetime protocol; preserve the native inventory format.
class BCMM_StockSourceLifeCodec
{
 static bool Write(notnull SaveContext context, notnull BCMM_StockSourceLifeState state)
 {
  if (!state.Valid() || !context.StartObject("bcmmNativeStockLife")) return false;
  bool written = context.WriteValue("schema", 1) && context.WriteValue("remaining", state.Remaining);
  bool ended = context.EndObject();
  return written && ended;
 }
 static bool Read(notnull LoadContext context, out BCMM_StockSourceLifeState state)
 {
  state = null;
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmNativeStockLife")) return false;
  if (!BCMM_PatientSaveValidation.HasState(context, "bcmmNativeStockLife")) return true;
  if (!context.StartObject("bcmmNativeStockLife")) return false;
  int schema;
  BCMM_StockSourceLifeState parsed = new BCMM_StockSourceLifeState();
  bool read = context.ReadValue("schema", schema) && schema == 1 && context.ReadValue("remaining", parsed.Remaining);
  bool ended = context.EndObject();
  if (!read || !ended || !parsed.Valid()) return false;
  state = parsed;
  return true;
 }
}

