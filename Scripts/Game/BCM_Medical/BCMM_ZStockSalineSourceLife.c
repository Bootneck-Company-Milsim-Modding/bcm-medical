// Original exact-source native stock lifetime; independent of holder and clinical model.
class BCMM_StockSourceLifeState
{
 float Remaining;
 bool Valid()
 {
  return Remaining >= 0 && Remaining <= 86400;
 }
}
class BCMM_StockSourceLifetime
{
 protected static ref array<ref BCMM_StockSourceLifetime> s_Entries = {};
 protected IEntity m_Source;
 protected float m_Deadline;
 protected bool m_Restored;
 protected bool m_NativeGrace;
 protected bool m_Finished;
 static float Now()
 {
  return GetGame().GetWorld().GetWorldTime() * 0.001;
 }
 static bool Supported(IEntity source)
 {
  if (!source)
  {
   return false;
  }
  SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(source.FindComponent(SCR_ConsumableItemComponent));
  return consumable && consumable.GetConsumableEffect() && consumable.GetConsumableEffect().Type() == SCR_ConsumableSalineBag;
 }
 protected static BCMM_StockSourceLifetime Find(IEntity source)
 {
  if (!source)
  {
   return null;
  }
  foreach (BCMM_StockSourceLifetime entry : s_Entries)
  {
   if (!entry.m_Finished && entry.m_Source == source)
   {
    return entry;
   }
  }
  return null;
 }
 static bool Expired(IEntity source)
 {
  BCMM_StockSourceLifetime entry = Find(source);
  return entry && Now() >= entry.m_Deadline;
 }
 static void Observe(IEntity patient, IEntity source, float lifetime)
 {
  if (!Replication.IsServer() || !Supported(source) || !BCMM_NativeSalineBinding.Attached(patient, source) || !(lifetime > 0 && lifetime <= 86400) || Find(source))
  {
   return;
  }
  BCMM_StockSourceLifetime entry = new BCMM_StockSourceLifetime();
  entry.m_Source = source;
  entry.m_Deadline = Now() + lifetime;
  s_Entries.Insert(entry);
  GetGame().GetCallqueue().CallLater(entry.Expire, lifetime * 1000);
 }
 static BCMM_StockSourceLifeState Capture(IEntity source)
 {
  BCMM_StockSourceLifetime entry = Find(source);
  if (!entry)
  {
   return null;
  }
  BCMM_StockSourceLifeState state = new BCMM_StockSourceLifeState();
  state.Remaining = Math.Max(0, entry.m_Deadline - Now());
  if (!state.Valid())
  {
   return null;
  }
  return state;
 }
 static bool Restore(IEntity source, BCMM_StockSourceLifeState state)
 {
  if (!Replication.IsServer() || !Supported(source) || !state || !state.Valid())
  {
   return false;
  }
  BCMM_StockSourceLifetime entry = Find(source);
  if (entry)
  {
   GetGame().GetCallqueue().Remove(entry.Expire);
   entry.m_Deadline = Math.Min(entry.m_Deadline, Now() + state.Remaining);
  }
  else
  {
   entry = new BCMM_StockSourceLifetime();
   entry.m_Source = source;
   entry.m_Deadline = Now() + state.Remaining;
   s_Entries.Insert(entry);
  }
  entry.m_Restored = true;
  GetGame().GetCallqueue().CallLater(entry.Expire, Math.Max(0, (entry.m_Deadline - Now()) * 1000));
  return true;
 }
 protected void Expire()
 {
  if (m_Finished)
  {
   return;
  }
  if (m_Source && !m_Source.IsDeleted() && Now() < m_Deadline)
  {
   GetGame().GetCallqueue().CallLater(Expire, Math.Max(1, (m_Deadline - Now()) * 1000));
   return;
  }
  // Give an existing native callback its own completion frame before fallback cleanup.
  if (m_Source && !m_Source.IsDeleted() && !m_Restored && !m_NativeGrace)
  {
   m_NativeGrace = true;
   GetGame().GetCallqueue().CallLater(Expire, 1000);
   return;
  }
  m_Finished = true;
  GetGame().GetCallqueue().Remove(Expire);
  if (m_Source && !m_Source.IsDeleted())
  {
   RplComponent.DeleteRplEntity(m_Source, false);
  }
  s_Entries.RemoveItem(this);
 }
}
class BCMM_NativeStockLifeSerializer : ScriptedComponentSerializer
{
 override static typename GetTargetType()
 {
  return SCR_ConsumableItemComponent;
 }
 override static EDeserializeFailHandling GetDeserializeFailHandling()
 {
  return EDeserializeFailHandling.ERROR;
 }
 override protected ESerializeResult Serialize(notnull IEntity owner, notnull GenericComponent component, notnull SaveContext context)
 {
  if (!context.StartObject("base"))
  {
   return ESerializeResult.ERROR;
  }
  ESerializeResult nativeResult = SerializeNative(owner, component, context);
  if (!context.EndObject() || nativeResult == ESerializeResult.ERROR)
  {
   return ESerializeResult.ERROR;
  }
  BCMM_StockSourceLifeState state = BCMM_StockSourceLifetime.Capture(owner);
  if (!state)
  {
   return nativeResult;
  }
  if (!BCMM_StockSourceLifeCodec.Write(context, state))
  {
   return ESerializeResult.ERROR;
  }
  return ESerializeResult.OK;
 }
 override protected bool Deserialize(notnull IEntity owner, notnull GenericComponent component, notnull LoadContext context)
 {
  if (!BCMM_PatientSaveValidation.StateShapeValid(context, "bcmmNativeStockLife"))
  {
   return false;
  }
  if (BCMM_PatientSaveValidation.HasState(context, "base"))
  {
   if (!context.StartObject("base"))
   {
    return false;
   }
   bool nativeResult = DeserializeNative(owner, component, context);
   if (!context.EndObject() || !nativeResult)
   {
    return false;
   }
  }
  BCMM_StockSourceLifeState state;
  if (!BCMM_StockSourceLifeCodec.Read(context, state))
  {
   return false;
  }
  return !state || BCMM_StockSourceLifetime.Restore(owner, state);
 }
}
