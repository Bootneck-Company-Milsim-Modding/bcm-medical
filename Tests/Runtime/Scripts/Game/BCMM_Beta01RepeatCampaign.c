// Repetition counts represent completed native fixture instances, not a loop
// over mocked results. Each instance spawns and cleans up its own entities.
class BCMM_Beta01RepeatCampaign
{
 static int Iteration;
 static int TotalChecks;
 static ref BCMM_DomainChecks Suite;
 static void Next()
 {
  Suite = new BCMM_DomainChecks();
  Suite.Run();
 }
}
modded class BCMM_DomainChecks
{
 override void Complete()
 {
  string requested;
  if (!System.GetCLIParam("bcmmRepeatNative", requested)) { super.Complete(); return; }
  int target = Math.ClampInt(requested.ToInt(), 1, 200);
  BCMM_Beta01RepeatCampaign.TotalChecks += m_Checks;
  BCMM_Beta01RepeatCampaign.Iteration++;
  PrintFormat("BCMM_NATIVE_ITERATION completed=%1 target=%2 checks=%3 failures=%4", BCMM_Beta01RepeatCampaign.Iteration, target, m_Checks, m_Failures);
  if (m_Failures || BCMM_Beta01RepeatCampaign.Iteration >= target)
  {
   m_Checks = BCMM_Beta01RepeatCampaign.TotalChecks;
   super.Complete(); return;
  }
  GetGame().GetCallqueue().CallLater(BCMM_Beta01RepeatCampaign.Next, 700);
 }
}
modded class BCMM_BaggingRuntimeChecks
{
 override protected void SnapshotCasualty()
 {
  string requested;
  if (!System.GetCLIParam("bcmmRepeatNative", requested)) { super.SnapshotCasualty(); return; }
  InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(m_Corpse.FindComponent(InventoryStorageManagerComponent));
  array<ResourceName> choices = {
   "{6AB4B3F3FD216008}Prefabs/Items/BCMM_Splint.et",
   "{6AB4B3F3FD216009}Prefabs/Items/BCMM_SurgeryKit.et",
   "{6AB4B3F3FD216010}Prefabs/Items/BCMM_SutureKit.et",
   "{6AB4B3F3FD216014}Prefabs/Items/BCMM_Epinephrine.et",
   "{6AB4B3F3FD216032}Prefabs/Items/BCMM_TXA.et"
  };
  int seed = BCMM_Beta01RepeatCampaign.Iteration;
  string offset;if(System.GetCLIParam("bcmmRepeatSeedOffset",offset))seed+=offset.ToInt();
  int count = 1 + seed % 8;
  for (int index = 0; index < count; index++)
  {
   int variant = (seed + index) % choices.Count();
   IEntity item = Spawn(choices[variant], m_Corpse.GetOrigin());
   m_RequestedExtras.Insert(item);
   m_Suite.Record(item && inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY), "Varied original equipment insertion accepted");
  }
  PrintFormat("BCMM_NATIVE_LOADOUT seed=%1 additionalSupplies=%2", seed, count);
  GetGame().GetCallqueue().CallLater(CaptureVariedCasualty, 500);
 }
 protected void CaptureVariedCasualty() { super.SnapshotCasualty(); }
}
