// Original native-stock binding. Preserve the same source and remaining simulation time.
class BCMM_SavedNativeSaline
{
 string SourceId;
 float LifeSeconds;
 float EffectSeconds;
 float DPS;
 bool Valid()
 {
  return BCMM_LifeIdentityFormat.IsCanonical(SourceId) && LifeSeconds > 0 && LifeSeconds <= 86400 && EffectSeconds >= 0 && EffectSeconds <= LifeSeconds + 1 && DPS <= 0 && DPS >= -1000000 && ((EffectSeconds > 0 && DPS < 0) || (EffectSeconds == 0 && DPS == 0));
 }
 bool SerializationSave(SaveContext context)
 {
  return Valid() && context.WriteValue("schema", 1) && context.WriteValue("source", SourceId) && context.WriteValue("life", LifeSeconds) && context.WriteValue("effect", EffectSeconds) && context.WriteValue("dps", DPS);
 }
 bool SerializationLoad(LoadContext context)
 {
  int schema;
  return context.ReadValue("schema", schema) && schema == 1 && context.ReadValue("source", SourceId) && context.ReadValue("life", LifeSeconds) && context.ReadValue("effect", EffectSeconds) && context.ReadValue("dps", DPS) && Valid();
 }
}
class BCMM_NativeSalineBinding
{
 protected static ref array<ref BCMM_NativeSalineBinding> s_Bindings = {};
 protected IEntity m_Patient;
 protected IEntity m_Source;
 protected ref BCMM_Patient m_Model;
 protected ref BCMM_SavedNativeSaline m_State;
 protected float m_Deadline;
 protected bool m_Restore;
 protected bool m_Resolved;
 protected bool m_Finished;
 static float Now()
 {
  return GetGame().GetWorld().GetWorldTime() * 0.001;
 }
 static bool Attached(IEntity patient, IEntity source)
 {
  if (!patient || !source)
  {
   return false;
  }
  SCR_SalineStorageComponent storage = SCR_SalineStorageComponent.Cast(patient.FindComponent(SCR_SalineStorageComponent));
  SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(source.FindComponent(SCR_ConsumableItemComponent));
  if (!storage || !consumable || !SCR_ConsumableSalineBag.Cast(consumable.GetConsumableEffect()))
  {
   return false;
  }
  array<IEntity> items = {};
  storage.GetAll(items);
  return items.Contains(source);
 }
 static void ObserveOriginal(IEntity patient, IEntity source, float lifetime)
 {
  if (!Replication.IsServer() || !patient || !source || !Attached(patient, source) || !(lifetime > 0 && lifetime <= 86400))
  {
   return;
  }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical)
  {
   return;
  }
  foreach (BCMM_NativeSalineBinding current : s_Bindings)
  {
   if (current.m_Source == source && !current.m_Finished && current.m_Patient == patient && current.m_Model == medical.BCMM_GetPatient())
   {
    return;
   }
  }
  BCMM_NativeSalineBinding binding = new BCMM_NativeSalineBinding();
  binding.m_Patient = patient;
  binding.m_Source = source;
  binding.m_Model = medical.BCMM_GetPatient();
  binding.m_Deadline = Now() + lifetime;
  binding.m_Resolved = true;
  s_Bindings.Insert(binding);
  GetGame().GetCallqueue().CallLater(binding.Expire, lifetime * 1000);
 }
 static void ResetPatient(IEntity patient)
 {
  if (!Replication.IsServer() || !patient)
  {
   return;
  }
  foreach (BCMM_NativeSalineBinding binding : s_Bindings)
  {
   if (!binding.m_Finished && binding.m_Patient == patient)
   {
    SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
    if (medical)
    {
     SCR_SalineDamageEffect effect = SCR_SalineDamageEffect.Cast(medical.FindDamageEffectOfType(SCR_SalineDamageEffect));
     if (effect)
     {
      effect.Terminate();
     }
    }
    return;
   }
  }
 }
 static bool SourceAllowsFlow(IEntity patient)
 {
  if (!patient)
  {
   return false;
  }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  bool tracked;
  foreach (BCMM_NativeSalineBinding binding : s_Bindings)
  {
   if (binding.m_Finished || binding.m_Patient != patient)
   {
    continue;
   }
   tracked = true;
   if (binding.m_Resolved && medical && binding.m_Model == medical.BCMM_GetPatient() && Attached(patient, binding.m_Source) && Now() < binding.m_Deadline && !BCMM_StockSourceLifetime.Expired(binding.m_Source))
   {
    return true;
   }
  }
  // An old disconnected session cannot mask a newer attached source, or feed a new model.
  return !tracked;
 }
 static BCMM_SavedNativeSaline Capture(IEntity patient, PersistenceSystem persistence)
 {
  if (!patient || !persistence)
  {
   return null;
  }
  SCR_CharacterDamageManagerComponent medical = SCR_CharacterDamageManagerComponent.Cast(patient.FindComponent(SCR_CharacterDamageManagerComponent));
  if (!medical)
  {
   return null;
  }
  foreach (BCMM_NativeSalineBinding binding : s_Bindings)
  {
   if (binding.m_Finished || binding.m_Patient != patient || binding.m_Model != medical.BCMM_GetPatient())
   {
    continue;
   }
   if (!binding.m_Resolved)
   {
    return binding.m_State;
   }
   if (!Attached(patient, binding.m_Source))
   {
    continue;
   }
   UUID sourceId = persistence.GetId(binding.m_Source);
   if (sourceId.IsNull())
   {
    return null;
   }
   BCMM_SavedNativeSaline state = new BCMM_SavedNativeSaline();
   state.SourceId = sourceId;
   state.LifeSeconds = binding.m_Deadline - Now();
   SCR_SalineDamageEffect effect = SCR_SalineDamageEffect.Cast(medical.FindDamageEffectOfType(SCR_SalineDamageEffect));
   if (effect && effect.IsActive())
   {
    state.EffectSeconds = effect.GetMaxDuration() - effect.GetCurrentDuration();
    state.DPS = effect.GetDPS();
   }
   if (state.Valid())
   {
    return state;
   }
  }
  return null;
 }
 static void Await(IEntity patient, BCMM_Patient model, BCMM_SavedNativeSaline state, PersistenceSystem persistence)
 {
  if (!Replication.IsServer() || !patient || !model || !state || !state.Valid() || !persistence)
  {
   return;
  }
  BCMM_NativeSalineBinding binding = new BCMM_NativeSalineBinding();
  binding.m_Patient = patient;
  binding.m_Model = model;
  binding.m_State = state;
  binding.m_Restore = true;
  s_Bindings.Insert(binding);
  GetGame().GetCallqueue().CallLater(binding.Expire, 31000);
  persistence.WhenAvailable(state.SourceId, new PersistenceWhenAvailableTask(OnSource, binding), 30);
 }
 protected static void OnSource(Managed instance, PersistenceDeferredDeserializeTask task, bool expired, Managed context)
 {
  BCMM_NativeSalineBinding binding = BCMM_NativeSalineBinding.Cast(context);
  if (!binding || binding.m_Finished || binding.m_Resolved)
  {
   return;
  }
  if (expired)
  {
   binding.Expire();
   return;
  }
  binding.m_Source = IEntity.Cast(instance);
  binding.m_Resolved = true;
  GetGame().GetCallqueue().CallLater(binding.RestoreValidated, 100);
 }
 protected void RestoreValidated()
 {
  SCR_CharacterDamageManagerComponent medical;
  if (m_Patient)
  {
   medical = SCR_CharacterDamageManagerComponent.Cast(m_Patient.FindComponent(SCR_CharacterDamageManagerComponent));
  }
  if (!medical || medical.BCMM_GetPatient() != m_Model || medical.IsDestroyed() || !Attached(m_Patient, m_Source))
  {
   m_Restore = false;
   Expire();
   return;
  }
  GetGame().GetCallqueue().Remove(Expire);
  m_Deadline = Now() + m_State.LifeSeconds;
  // Older attached-source saves carry this timer on the patient rather than the item.
  // Restore retains an earlier existing item deadline, so repeated loading cannot refill it.
  if (BCMM_StockSourceLifetime.Supported(m_Source))
  {
   BCMM_StockSourceLifeState sourceLife = new BCMM_StockSourceLifeState();
   sourceLife.Remaining = m_State.LifeSeconds;
   BCMM_StockSourceLifetime.Restore(m_Source, sourceLife);
  }
  if (m_State.EffectSeconds > 0 && !medical.IsDamageEffectPresent(SCR_SalineDamageEffect) && BCMM_NativeTreatmentSafety.FluidAllowed(m_Patient))
  {
   SCR_SalineDamageEffect effect = new SCR_SalineDamageEffect();
   effect.SetDamageType(effect.GetDefaultDamageType());
   effect.SetInstigator(Instigator.CreateInstigator(m_Patient));
   effect.SetDPS(m_State.DPS);
   effect.SetMaxDuration(m_State.EffectSeconds);
   effect.SkipRegenDelay(true);
   medical.AddDamageEffect(effect);
  }
  GetGame().GetCallqueue().CallLater(Expire, m_State.LifeSeconds * 1000);
 }
 protected void Expire()
 {
  if (m_Finished)
  {
   return;
  }
  // Native callqueue precision can invoke a duration callback before its world-time deadline.
  if (m_Resolved && m_Deadline > 0 && Now() < m_Deadline)
  {
   GetGame().GetCallqueue().CallLater(Expire, Math.Max(1, (m_Deadline - Now()) * 1000));
   return;
  }
  PrintFormat("BCMM_NATIVE_SALINE_EXPIRY restored=%1 now=%2 deadline=%3 attached=%4", m_Restore, Now(), m_Deadline, Attached(m_Patient, m_Source));
  m_Finished = true;
  GetGame().GetCallqueue().Remove(Expire);
  GetGame().GetCallqueue().Remove(RestoreValidated);
  // Original sessions retain their native callback. Only restored sessions replace it.
  if (m_Restore && m_Deadline > 0 && Now() >= m_Deadline && Attached(m_Patient, m_Source))
  {
   RplComponent.DeleteRplEntity(m_Source, false);
  }
  s_Bindings.RemoveItem(this);
 }
}





