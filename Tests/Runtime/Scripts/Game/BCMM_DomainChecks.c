// Diagnostic addon only. Never publish as gameplay.
modded class SCR_BaseGameMode
{
 protected IEntity m_BCMMClinicalFloor;
 protected ref BCMM_DomainChecks m_BCMMChecks;
 override void OnGameStart()
 {
  super.OnGameStart();
  string medicalFloor;
  if (System.GetCLIParam("bcmmMedicalRuntime", medicalFloor) || System.GetCLIParam("bcmmMedicalObserver", medicalFloor) || System.GetCLIParam("bcmmDragObserver", medicalFloor) || System.GetCLIParam("bcmmSplintObserver", medicalFloor))
  {
   EntitySpawnParams floor = new EntitySpawnParams();
   floor.TransformMode = ETransformMode.WORLD;
   floor.Transform[3] = "100 0.5 0";
   m_BCMMClinicalFloor = GetGame().SpawnEntityPrefab(Resource.Load("{B6C4ED1CA1262000}Prefabs/BCMM_TestFloor.et"), GetGame().GetWorld(), floor);
  }
  string preview;
  if (System.GetCLIParam("bcmmUIPreview", preview)) { BCMM_UIPreview.Start(); return; }
  string observer; if (!Replication.IsServer() || System.GetCLIParam("bcmmCargoObserver", observer)) return;
  m_BCMMChecks = new BCMM_DomainChecks();
  GetGame().GetCallqueue().CallLater(m_BCMMChecks.Run, 250);
 }
}

class BCMM_DomainChecks
{
 protected ref BCMM_PulseOximeterRuntimeChecks m_PulseOximeterRuntime;
 protected ref BCMM_TXARuntimeChecks m_TXARuntime;
 protected int m_Failures;
 protected int m_Checks;
 protected ref BCMM_CargoRuntimeChecks m_CargoRuntime;
 protected ref BCMM_CharacterRuntimeChecks m_CharacterRuntime;
 protected ref BCMM_BaggingRuntimeChecks m_BaggingRuntime;
 protected ref BCMM_LifeRuntimeChecks m_LifeRuntime;
 protected ref BCMM_DragRuntimeChecks m_DragRuntime;
 protected ref BCMM_SplintRuntimeChecks m_SplintRuntime;
 protected void Check(bool condition, string label)
 {
  m_Checks++;
  if (!condition)
  {
   m_Failures++;
   Print("BCMM_CHECK FAILED: " + label, LogLevel.ERROR);
  }
 }

 void Run()
 {
  BCMM_Beta01Checks.ExportNative();
  BCMM_Beta01Checks.RunIfRequested(this);
  string preview;
  if (System.GetCLIParam("bcmmUIPreview", preview)) { BCMM_UIPreview.Start(); return; }
  string observer; if (System.GetCLIParam("bcmmCargoObserver", observer)) return;
  CheckVisualPolicies();
  CheckPhysiology();
  BCMM_LinkedClinicalChecks.Run(this);
  BCMM_FluidBalanceChecks.Run(this);
  BCMM_OxygenMedicationChecks.Run(this);
  BCMM_ArsenalChecks.Run(this);
  CheckPharmacology();
  CheckTXA();
  CheckStretcherPolicy();
  CheckEquipmentPolicies();
  CheckTraumaGeneration();
  CheckPatientIsolation();
  CheckDeadline();
  CheckLives();
  CheckTrauma();
  CheckTransport();
  CheckSettings();
  BCMM_PlayerScopeChecks.Run(this);
  CheckGMResource();
  BCMM_CatalogueChecks.Resources(this);
  BCMM_ComponentRecoveryChecks.Run(this);
  BCMM_FluidLineChecks.Run(this);
  BCMM_FluidBurdenChecks.Run(this);
  BCMM_NativeSalineCodecChecks.Run(this);
  BCMM_StockSourceLifeCodecChecks.Run(this);
  CheckMedicalMenuConfig();
  CheckMedicalInputConfig();
  CheckBodyBagResources();
  Resource panel = Resource.Load("{6AB4B3F3FD215003}UI/layouts/BCMM/MedicalPanel.layout");
  Check(panel && panel.IsValid(), "Original medical-panel layout resource loads");
  CheckRestartPhase();
  string addedStockState;
  if(System.GetCLIParam("bcmmOptionalFaultSeed",addedStockState)) { m_SplintRuntime=new BCMM_OptionalFaultWorldSeed();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmOptionalFaultRestart",addedStockState)) { m_SplintRuntime=new BCMM_OptionalFaultWorldRestart();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmRetrievedSalineSeed",addedStockState)) { m_SplintRuntime=new BCMM_RetrievedSalineWorldChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmRetrievedSalineRestart",addedStockState)) { m_SplintRuntime=new BCMM_RetrievedSalineRestartChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineSameSourceHeal",addedStockState)) { m_SplintRuntime=new BCMM_NativeSalineSameSourceHealChecks();m_SplintRuntime.Start(this);return; }
  string stockSaline;
  if(System.GetCLIParam("bcmmNativeSalineDeferred",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineDeferredChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineAccess",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineRuntimeChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineSourceMove",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineSourceMoveChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineFullHeal",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineFullHealChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineWorld",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineWorldChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmNativeSalineWorldRestart",stockSaline)) { m_SplintRuntime=new BCMM_NativeSalineWorldRestartChecks();m_SplintRuntime.Start(this);return; }
  string appliedNetwork;
  if(System.GetCLIParam("bcmmAppliedEquipmentNetwork",appliedNetwork)) { m_SplintRuntime=new BCMM_AppliedEquipmentNetworkChecks();m_SplintRuntime.Start(this);return; }
  string removalNetwork;if(System.GetCLIParam("bcmmOxygenRemovalNetwork",removalNetwork)){m_SplintRuntime=new BCMM_OxygenRemovalNetworkChecks();m_SplintRuntime.Start(this);return;}
  string missingLineRestart;if(System.GetCLIParam("bcmmMissingFluidLineRestart",missingLineRestart)){m_SplintRuntime=new BCMM_MissingFluidLineRestartChecks();m_SplintRuntime.Start(this);return;}
  string missingLine;if(System.GetCLIParam("bcmmMissingFluidLineRuntime",missingLine)){m_SplintRuntime=new BCMM_MissingFluidLineRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string lineRestart;if(System.GetCLIParam("bcmmFluidLineRestartRuntime",lineRestart)){m_SplintRuntime=new BCMM_FluidLineRestartRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string linePersistence;if(System.GetCLIParam("bcmmFluidLinePersistenceRuntime",linePersistence)){m_SplintRuntime=new BCMM_FluidLinePersistenceRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string legacyBurden;if(System.GetCLIParam("bcmmLegacyFluidBurdenWorld",legacyBurden)){m_SplintRuntime=new BCMM_FluidBurdenLegacyWorldChecks();m_SplintRuntime.Start(this);return;}
  string burdenNetwork;if(System.GetCLIParam("bcmmFluidBurdenNetwork",burdenNetwork)){m_SplintRuntime=new BCMM_FluidBurdenNetworkChecks();m_SplintRuntime.Start(this);return;}
  string burdenRestart;if(System.GetCLIParam("bcmmFluidBurdenRestart",burdenRestart)){m_SplintRuntime=new BCMM_FluidBurdenRestartChecks();m_SplintRuntime.Start(this);return;}
  string burdenRuntime;if(System.GetCLIParam("bcmmFluidBurdenRuntime",burdenRuntime)){m_SplintRuntime=new BCMM_FluidBurdenRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string lineBoundary;if(System.GetCLIParam("bcmmFluidLineBoundaryRuntime",lineBoundary)){m_SplintRuntime=new BCMM_FluidLineBoundaryRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string lineNetwork;if(System.GetCLIParam("bcmmFluidLineNetwork",lineNetwork)){m_SplintRuntime=new BCMM_FluidLineNetworkChecks();m_SplintRuntime.Start(this);return;}
  string fluidLine;if(System.GetCLIParam("bcmmFluidLineRuntime",fluidLine)){m_SplintRuntime=new BCMM_FluidLineRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string oxygenRemoval;if(System.GetCLIParam("bcmmOxygenRemovalRuntime",oxygenRemoval)){m_SplintRuntime=new BCMM_OxygenRemovalRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string recoveryRestart;if(System.GetCLIParam("bcmmComponentRecoveryRestart",recoveryRestart)){m_SplintRuntime=new BCMM_ComponentRecoveryRestartChecks();m_SplintRuntime.Start(this);return;}
  string recoveryRuntime;if(System.GetCLIParam("bcmmComponentRecoveryRuntime",recoveryRuntime)){m_SplintRuntime=new BCMM_ComponentRecoveryRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string lifeAdminRestart;if(System.GetCLIParam("bcmmLifeAdminRestart",lifeAdminRestart)){m_SplintRuntime=new BCMM_LifeAdminRestartChecks();m_SplintRuntime.Start(this);return;}
  string lifeAdmin;if(System.GetCLIParam("bcmmLifeAdminNetwork",lifeAdmin)){m_SplintRuntime=new BCMM_LifeAdminNetworkChecks();m_SplintRuntime.Start(this);return;}
  string oxygenStress;if(System.GetCLIParam("bcmmOxygenStress",oxygenStress)){m_SplintRuntime=new BCMM_OxygenStressChecks();m_SplintRuntime.Start(this);return;}
  string oxygenNetwork;
  if(System.GetCLIParam("bcmmOxygenNetwork",oxygenNetwork)) { m_SplintRuntime=new BCMM_OxygenMedicationNetworkChecks();m_SplintRuntime.Start(this);return; }
  string savedWorldNetwork;if(System.GetCLIParam("bcmmWorldPersistenceNetwork",savedWorldNetwork)){m_SplintRuntime=new BCMM_WorldPersistenceNetworkChecks();m_SplintRuntime.Start(this);return;}
  string savedWorldFaults;if(System.GetCLIParam("bcmmWorldPersistenceFaults",savedWorldFaults)){string missingPhase;if(System.GetCLIParam("bcmmWorldFaultPhase",missingPhase))m_SplintRuntime=new BCMM_MissingPhysicalRestartChecks();else m_SplintRuntime=new BCMM_WorldPersistenceFaultRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string finitePersistence;if(System.GetCLIParam("bcmmFinitePersistenceRuntime",finitePersistence)) { m_SplintRuntime=new BCMM_FinitePersistenceRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string arsenalRuntime;if(System.GetCLIParam("bcmmArsenalRuntime",arsenalRuntime)) { m_SplintRuntime=new BCMM_ArsenalRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string oxygenRuntime;
  if(System.GetCLIParam("bcmmOxygenMedicationRuntime",oxygenRuntime)) { m_SplintRuntime=new BCMM_OxygenMedicationRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string fluidNetwork;
  if(System.GetCLIParam("bcmmFluidNetwork",fluidNetwork)) { m_SplintRuntime=new BCMM_FluidNetworkChecks();m_SplintRuntime.Start(this);return; }
  string fluidsRuntime;
  if(System.GetCLIParam("bcmmFluidsRuntime",fluidsRuntime)) { m_SplintRuntime=new BCMM_FluidsRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string stockNetwork;
  if(System.GetCLIParam("bcmmStockCommitNetwork",stockNetwork)) { m_SplintRuntime=new BCMM_StockCommitNetworkChecks();m_SplintRuntime.Start(this);return; }
  string stockCommit;
  if(System.GetCLIParam("bcmmNativeConsumableCommitRuntime",stockCommit)) { m_SplintRuntime=new BCMM_NativeConsumableCommitChecks();m_SplintRuntime.Start(this);return; }
  string transportLine;
  if(System.GetCLIParam("bcmmTransportFluidLineRuntime",transportLine)) { m_SplintRuntime=new BCMM_TransportFluidLineChecks();m_SplintRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmTransportFluidLineRestartRuntime",transportLine)) { m_SplintRuntime=new BCMM_TransportFluidLineRestartChecks();m_SplintRuntime.Start(this);return; }
  string weaponSling;if(System.GetCLIParam("bcmmWeaponSlingRuntime",weaponSling)){m_SplintRuntime=new BCMM_WeaponSlingRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string livingNetwork;if(System.GetCLIParam("bcmmLivingTransportNetwork",livingNetwork)){m_SplintRuntime=new BCMM_LivingTransportNetworkChecks();m_SplintRuntime.Start(this);return;}
  string livingTransport;if(System.GetCLIParam("bcmmLivingTransportRuntime",livingTransport)){m_SplintRuntime=new BCMM_LivingTransportRuntimeChecks();m_SplintRuntime.Start(this);return;}
  string clinicalSafety;
  if(System.GetCLIParam("bcmmClinicalSafetyRuntime",clinicalSafety)) { m_SplintRuntime=new BCMM_ClinicalSafetyRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string appliedVisuals;
  if(System.GetCLIParam("bcmmAppliedVisualsRuntime",appliedVisuals)) { m_SplintRuntime=new BCMM_AppliedVisualsRuntimeChecks();m_SplintRuntime.Start(this);return; }
  string gripCoordinate;
  if(System.GetCLIParam("bcmmBeta01WorldReview",gripCoordinate)){m_SplintRuntime=new BCMM_Beta01WorldReview();m_SplintRuntime.Start(this);return;}
  if(System.GetCLIParam("bcmmBeta01Hands",gripCoordinate)){m_SplintRuntime=new BCMM_Beta01HandReview();m_SplintRuntime.Start(this);return;}
  if(System.GetCLIParam("bcmmGripCoordinateRuntime",gripCoordinate)) { m_SplintRuntime=new BCMM_GripCoordinateChecks();m_SplintRuntime.Start(this);return; }
  string stretcherRuntime;
  if (System.GetCLIParam("bcmmStretcherRuntime", stretcherRuntime)) { m_SplintRuntime = new BCMM_StretcherRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string fieldProcedureRuntime;
  if (System.GetCLIParam("bcmmFieldProceduresRuntime", fieldProcedureRuntime)) { m_SplintRuntime = new BCMM_FieldProcedureRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string animationRuntime;
  if (System.GetCLIParam("bcmmAnimationRuntime", animationRuntime)) { m_SplintRuntime = new BCMM_AnimationRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string equipmentRuntime;
  if (System.GetCLIParam("bcmmEquipmentRuntime", equipmentRuntime)) { m_SplintRuntime = new BCMM_EquipmentRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string medicationRuntime;
  if (System.GetCLIParam("bcmmMedicationRuntime", medicationRuntime)) { m_SplintRuntime = new BCMM_MedicationRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string bandageRuntime;
  if (System.GetCLIParam("bcmmBandageRuntime", bandageRuntime)) { m_SplintRuntime = new BCMM_BandageRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string assessmentRuntime;
  if (System.GetCLIParam("bcmmAssessmentRuntime", assessmentRuntime)) { m_SplintRuntime = new BCMM_AssessmentRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string internalSurgeryRuntime;
  if (System.GetCLIParam("bcmmInternalSurgeryRuntime", internalSurgeryRuntime)) { m_SplintRuntime = new BCMM_InternalSurgeryRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string surgeryRuntime;
  if (System.GetCLIParam("bcmmSurgeryRuntime", surgeryRuntime)) { m_SplintRuntime = new BCMM_SurgeryRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string artReview;if(System.GetCLIParam("bcmmMedicalArtReview",artReview)) { m_SplintRuntime=new BCMM_MedicalArtReview();m_SplintRuntime.Start(this);return; }
  string splintRuntime;
  if (System.GetCLIParam("bcmmSplintRuntime", splintRuntime)) { m_SplintRuntime = new BCMM_SplintRuntimeChecks(); m_SplintRuntime.Start(this); return; }
  string dragRuntime;
  if (System.GetCLIParam("bcmmTerrainDrag", dragRuntime)) { m_DragRuntime = new BCMM_Beta01TerrainDrag(); m_DragRuntime.Start(this); return; }
  if (System.GetCLIParam("bcmmBeta01Packing", dragRuntime)) { m_DragRuntime = new BCMM_Beta01Packing(); m_DragRuntime.Start(this); return; }
  if (System.GetCLIParam("bcmmBeta01DragMatrix", dragRuntime)) { m_DragRuntime = new BCMM_Beta01DragMatrix(); m_DragRuntime.Start(this); return; }
  if (System.GetCLIParam("bcmmDragRuntime", dragRuntime)) { m_DragRuntime = new BCMM_DragRuntimeChecks(); m_DragRuntime.Start(this); return; }
  string lifeRuntime;
  if (System.GetCLIParam("bcmmLifeRuntime", lifeRuntime)) { m_LifeRuntime = new BCMM_LifeRuntimeChecks(); m_LifeRuntime.Start(this); return; }
  string cargoRollbackRuntime;
  if (System.GetCLIParam("bcmmCargoRollbackRuntime", cargoRollbackRuntime)) { m_CargoRuntime = new BCMM_CargoRollbackChecks(); m_CargoRuntime.Start(this); return; }
  string pickupRuntime;
  if (System.GetCLIParam("bcmmPickupRuntime", pickupRuntime)) { m_BaggingRuntime = new BCMM_BaggingPickupChecks(); m_BaggingRuntime.Start(this); return; }
  string rollbackRuntime;
  if (System.GetCLIParam("bcmmRollbackRuntime", rollbackRuntime)) { m_BaggingRuntime = new BCMM_BaggingRollbackChecks(); m_BaggingRuntime.Start(this); return; }
  string baggingRuntime;
  string burialRuntime;
  if(System.GetCLIParam("bcmmBurialNetwork",burialRuntime)) { m_BaggingRuntime=new BCMM_BurialNetworkChecks();m_BaggingRuntime.Start(this);return; }
  string homeRuntime;
  if(System.GetCLIParam("bcmmSendHomeRuntime",homeRuntime)){m_BaggingRuntime=new BCMM_SendHomeRuntimeChecks();m_BaggingRuntime.Start(this);return;}
  if(System.GetCLIParam("bcmmBurialOwner",burialRuntime)) { m_BaggingRuntime=new BCMM_BurialOwnerChecks();m_BaggingRuntime.Start(this);return; }
  if(System.GetCLIParam("bcmmBurialRuntime",burialRuntime)) { m_BaggingRuntime=new BCMM_BurialRuntimeChecks();m_BaggingRuntime.Start(this);return; }
  if (System.GetCLIParam("bcmmBaggingRuntime", baggingRuntime)) { m_BaggingRuntime = new BCMM_BaggingRuntimeChecks(); m_BaggingRuntime.Start(this); return; }
  string medicalRuntime;
  if (System.GetCLIParam("bcmmMedicalRuntime", medicalRuntime)) { m_CharacterRuntime = new BCMM_CharacterRuntimeChecks(); m_CharacterRuntime.Start(this); return; }
  string pulseRuntime;
  if (System.GetCLIParam("bcmmPulseOximeterRuntime", pulseRuntime)) { m_PulseOximeterRuntime = new BCMM_PulseOximeterRuntimeChecks(); m_PulseOximeterRuntime.Start(this); return; }
  string txaRuntime;
  if (System.GetCLIParam("bcmmTXARuntime", txaRuntime)) { m_TXARuntime = new BCMM_TXARuntimeChecks(); m_TXARuntime.Start(this); return; }
  string cargoRuntime;
  if (System.GetCLIParam("bcmmCargoRuntime", cargoRuntime)) { m_CargoRuntime = new BCMM_CargoRuntimeChecks(); m_CargoRuntime.Start(this); return; }
  Complete();
 }
 void Record(bool condition, string label) { Check(condition, label); }
 void Complete()
 {
  string runToken;
  System.GetCLIParam("bcmmRunToken", runToken);
  PrintFormat("BCMM_CHECK COMPLETE checks=%1 failures=%2 run=%3", m_Checks, m_Failures, runToken);
  GetGame().RequestClose();
 }

 protected void CheckVisualPolicies()
 {
  BCMM_FocusBudget focus = new BCMM_FocusBudget();
  Check(focus.Apply(1, 0, 10, 30) == 1, "Focus starts with requested strength");
  Check(focus.Apply(1, 10, 10, 30) == 0, "Focus stops at exact duration boundary");
  Check(focus.Apply(1, 50, 10, 30) == 0, "Holding focus cannot automatically restart expired budget");
  focus.Apply(0, 50, 10, 30);
  Check(focus.Apply(1, 51, 10, 30) == 1, "Release permits focus after cooldown");
  focus.Apply(0, 52, 10, 30);
  Check(focus.Apply(1, 53, 10, 30) == 0, "Early release starts full cooldown");
  focus.Apply(0, 82, 10, 30);
  Check(focus.Apply(1, 82, 10, 30) == 1, "Focus cooldown expires at exact boundary");
  BCMM_SuppressionEnvelope envelope = new BCMM_SuppressionEnvelope();
  envelope.Pulse(0.6);
  envelope.Pulse(0.6);
  Check(envelope.Strength == 1, "Suppression contributions saturate rather than overflow");
  envelope.Advance(1.5, 3);
  Check(Math.AbsFloat(envelope.Strength - 0.5) < 0.001, "Suppression release uses saved duration");
  envelope.Advance(1.5, 3);
  Check(envelope.Strength == 0, "Suppression releases fully at duration boundary");
  Check(vector.Distance(BCMM_SuppressionEnvelope.ClosestPoint("0 0 0", "10 0 0", "5 3 0"), "5 0 0") < 0.001, "Near-miss distance uses swept projectile segment");
  Resource blur = Resource.Load("{6AB4B3F3FD215001}UI/Materials/BCMM_SuppressionBlur.emat");
  Resource overlay = Resource.Load("{6AB4B3F3FD215002}UI/layouts/BCMM/Suppression.layout");
  Check(blur && blur.IsValid() && overlay && overlay.IsValid(), "Standalone suppression material and layout resources load");
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_SettingsDocument seventh = new BCMM_SettingsDocument();
  seventh.Schema = 7;
  seventh.Generation = 1;
  for (int oldIndex = 0; oldIndex < 50; oldIndex++)
  {
   seventh.Keys.Insert(settings.Definition(oldIndex).Key);
   seventh.Values.Insert(settings.ValueAt(oldIndex));
  }
  seventh.Values[0] = 0.52;
  BCMM_Settings withSplints = new BCMM_Settings();
  Check(seventh.Decode(withSplints) && Math.AbsFloat(withSplints.Get("retainedBulletChance") - 0.52) < 0.001 && withSplints.Get("splintApplicationSeconds") == 30, "Schema seven retains prior choices and adds splint defaults");
  seventh.Keys.Remove(49);
  seventh.Values.Remove(49);
  Check(!seventh.Decode(new BCMM_Settings()), "Incomplete schema seven settings cannot silently gain missing prior keys");
  BCMM_SettingsDocument eighth = new BCMM_SettingsDocument();
  eighth.Schema = 8;
  eighth.Generation = 1;
  for (int savedIndex = 0; savedIndex < 52; savedIndex++)
  {
   eighth.Keys.Insert(settings.Definition(savedIndex).Key);
   eighth.Values.Insert(settings.ValueAt(savedIndex));
  }
  eighth.Values[settings.IndexOf("splintApplicationSeconds")] = 17;
  BCMM_Settings surgicalDefaults = new BCMM_Settings();
  Check(eighth.Decode(surgicalDefaults) && surgicalDefaults.Get("splintApplicationSeconds") == 17 && surgicalDefaults.Get("surgeryDurationSeconds") == 60 && surgicalDefaults.Get("sutureDurationSeconds") == 60, "Schema eight retains chosen splint time and adds surgical defaults");
  BCMM_SettingsDocument ninth = new BCMM_SettingsDocument();
  ninth.Schema = 9;
  ninth.Generation = 1;
  for (int priorIndex = 0; priorIndex < 55; priorIndex++)
  {
   ninth.Keys.Insert(settings.Definition(priorIndex).Key);
   ninth.Values.Insert(settings.ValueAt(priorIndex));
  }
  ninth.Values[settings.IndexOf("surgeryDurationSeconds")] = 145;
  BCMM_Settings withBandaging = new BCMM_Settings();
  Check(ninth.Decode(withBandaging) && withBandaging.Get("surgeryDurationSeconds") == 145 && withBandaging.Get("bandageApplicationSeconds") == 10, "Schema nine preserves chosen surgery time while adding bandage timing");
  BCMM_SettingsDocument previous = new BCMM_SettingsDocument();
  previous.Schema = 3;
  previous.Generation = 1;
  for (int i = 0; i < 41; i++)
  {
   previous.Keys.Insert(settings.Definition(i).Key);
   previous.Values.Insert(settings.ValueAt(i));
  }
  BCMM_Settings migrated = new BCMM_Settings();
  Check(previous.Decode(migrated) && migrated.Get("baggingDurationSeconds") == 12, "Schema three migrates bagging controls without losing previous settings");
 }
 protected void CheckPhysiology()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient healthy = new BCMM_Patient();
  healthy.AdvancePhysiology(60, 1, settings);
  Check(healthy.HasSpontaneousCirculation && healthy.OxygenSaturation > 0.97 && healthy.HypoxicDebtSeconds == 0, "Healthy physiology remains stable");
  BCMM_Patient bleeding = new BCMM_Patient();
  bleeding.AdvancePhysiology(11, 0.2, settings);
  Check(!bleeding.HasSpontaneousCirculation && bleeding.Rhythm == BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY, "Prolonged low blood produces nonshockable arrest");
  Check(!bleeding.DeliverShock(), "AED refuses haemorrhagic PEA");
  bleeding.StartCompressions(1, 1);
  bleeding.AdvancePhysiology(1, 0.2, settings);
  Check(bleeding.PerfusionFraction > 0 && bleeding.SystolicPressure > 0 && bleeding.HeartRate == 0, "CPR provides partial perfusion without inventing a pulse");
  bleeding.DeliverAssistedVentilation(10);
  bleeding.AdvancePhysiology(11, 0.2, settings);
  Check(!bleeding.HasSpontaneousCirculation, "CPR and ventilation do not cure unresolved blood loss");
  bleeding.DeliverAssistedVentilation(10);
  bleeding.AdvancePhysiology(10, 1, settings);
  Check(bleeding.HasSpontaneousCirculation && !bleeding.HasCompressions(), "Corrected causes with CPR permit ROSC and release compression session");
  BCMM_Patient vf = new BCMM_Patient();
  vf.HasSpontaneousCirculation = false;
  vf.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  Check(vf.DeliverShock() && !vf.HasSpontaneousCirculation && vf.ShocksDelivered() == 1, "Advised shock changes rhythm without automatic resurrection");
  Check(!vf.DeliverShock() && vf.ShocksDelivered() == 1, "AED cannot repeatedly shock resulting PEA");
  BCMM_Patient blocked = new BCMM_Patient();
  blocked.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  Check(!blocked.DeliverAssistedVentilation(5), "Blocked airway rejects ventilation");
  blocked.AdvancePhysiology(65, 1, settings);
  Check(!blocked.HasSpontaneousCirculation && blocked.OxygenSaturation < 0.1, "Untreated occlusion produces hypoxic arrest");
  blocked.AdvancePhysiology(240, 1, settings);
  Check(blocked.Terminal, "Sustained inadequate oxygen delivery reaches terminal injury budget");
  BCMM_Patient chest = new BCMM_Patient();
  chest.LeftChest.OpenWound = true;
  float beforeSeal = chest.LungCapacity(settings);
  chest.LeftChest.ApplySeal();
  Check(chest.LungCapacity(settings) > beforeSeal, "Seal improves open chest capacity");
  chest.LeftChest.Tension = true;
  chest.AdvancePhysiology(1, 1, settings);
  Check(chest.PerfusionFraction < 0.4, "Tension impairs circulation despite normal blood volume");
  chest.LeftChest.RelieveTension();
  chest.AdvancePhysiology(1, 1, settings);
  Check(chest.PerfusionFraction > 0.99, "Decompression relieves tension perfusion penalty");
  BCMM_SettingsDocument previous = new BCMM_SettingsDocument();
  previous.Schema = 2;
  previous.Generation = 1;
  for (int index = 0; index < 29; index++)
  {
   previous.Keys.Insert(settings.Definition(index).Key);
   previous.Values.Insert(settings.ValueAt(index));
  }
  BCMM_Settings migrated = new BCMM_Settings();
  Check(previous.Decode(migrated) && migrated.Get("normalBloodVolumeMl") == 5000, "Schema two settings gain physiology defaults");
 }
 protected void CheckPatientIsolation()
 {
  BCMM_Patient first = new BCMM_Patient();
  BCMM_Patient second = new BCMM_Patient();
  first.HasSpontaneousCirculation = false;
  first.Rhythm = BCMM_CardiacRhythm.ASYSTOLE;
  Check(!first.IsShockAdvised(), "Asystole is not shockable");
  first.Rhythm = BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;
  Check(!first.IsShockAdvised(), "PEA is not shockable");
  first.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  Check(first.IsShockAdvised(), "Pulseless VF is shockable");
  first.Rhythm = BCMM_CardiacRhythm.PULSELESS_VENTRICULAR_TACHYCARDIA;
  Check(first.IsShockAdvised(), "Pulseless VT is shockable");
  Check(first.StartCompressions(1, 10), "First medic acquires CPR session");
  Check(first.StartCompressions(1, 10), "Repeated identical start is idempotent");
  Check(!first.StartCompressions(2, 11), "Second medic cannot steal CPR");
  Check(!first.StopCompressions(2, 10), "Other medic cannot cancel CPR");
  Check(!second.HasCompressions() && second.HasSpontaneousCirculation, "CPR does not affect bystander");
  Check(first.StopCompressions(1, 10) && first.StartCompressions(2, 11), "CPR handoff after release");
  Check(!first.StopCompressions(1, 10) && first.HasCompressions(), "Stale cancel cannot end new session");
  first.LeftChest.OpenWound = true;
  first.LeftChest.Tension = true;
  first.LeftChest.PleuralBloodMl = 200;
  Check(first.LeftChest.ApplySeal() && first.LeftChest.Tension, "Seal does not cure tension");
  Check(!first.RightChest.Sealed && !second.LeftChest.Sealed, "Chest side and patient isolation");
  Check(first.LeftChest.RelieveTension() && first.LeftChest.PleuralBloodMl == 200, "NCD does not remove haemothorax");
  first.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  Check(!first.CanVentilate() && second.CanVentilate(), "Obstructed airway blocks ventilation");
  int woundId = first.AddWound(1, 20);
  BCMM_Wound wound = first.FindWound(woundId);
  wound.CompressionFraction = 0.75;
  Check(first.TotalExternalFlowMlPerSecond() == 5 && second.TotalExternalFlowMlPerSecond() == 0, "Wounds and treatment belong to patient");
  first.MarkTerminal();
  Check(!first.HasCompressions() && !first.IsShockAdvised() && !first.StartCompressions(3, 12), "Terminal cleanup prevents treatment restart");
 }

 protected void CheckDeadline()
 {
  BCMM_DepletionPolicy policy = new BCMM_DepletionPolicy();
  Check(!policy.Observe(0, 100) && !policy.IsActive(), "Positive reserve starts no timer");
  Check(!policy.Observe(10, 0) && policy.DeadlineSeconds() == 70, "Depletion starts sixty second timer");
  policy.Observe(20, 0);
  Check(policy.DeadlineSeconds() == 70, "Repeated injuries do not extend deadline");
  Check(!policy.RecordDeliveredRescueFluid(30, 0, 100), "Merely starting infusion is not rescue");
  Check(!policy.Observe(35, 100) && policy.IsActive(), "Unqualified health setter is not rescue");
  Check(policy.RecordDeliveredRescueFluid(69.9, 1, 1) && !policy.IsActive(), "Actual timely fluid delivery rescues");
  policy.Observe(80, 0);
  Check(policy.DeadlineSeconds() == 140, "Re-depletion receives new deadline");
  Check(!policy.RecordDeliveredRescueFluid(140, 1, 1) && policy.IsExpired(), "Expiry wins exact boundary");
  Check(!policy.RecordDeliveredRescueFluid(141, 100, 100), "Late infusion cannot revive terminal policy");
  policy.ResetByFullHeal();
  Check(!policy.IsActive() && !policy.IsExpired(), "Explicit GM reset clears terminal policy");
  policy.Observe(150, 0);
  policy.Observe(149, 0);
  Check(policy.DeadlineSeconds() == 210 && policy.Observe(210, 0), "Backward clock does not extend deadline");
 }

 protected void CheckLives()
 {
  BCMM_LifeLedger ledger = new BCMM_LifeLedger();
  Check(ledger.RemainingDeployments(1) == 1, "One life includes initial deployment");
  int first = ledger.ReserveDeployment(1);
  Check(first > 0 && ledger.ReserveDeployment(1) == 0, "Concurrent spawn reservation is blocked");
  Check(ledger.CancelDeployment(first) && ledger.RemainingDeployments(1) == 1, "Failed spawn spends no life");
  first = ledger.ReserveDeployment(1);
  Check(ledger.CommitDeployment(first) && !ledger.CommitDeployment(first), "Successful spawn charged exactly once");
  Check(ledger.ReserveDeployment(1, false) == 0, "Disabled limiter cannot create second active body");
  Check(!ledger.ConfirmDeath(first - 1) && ledger.IsAlive(), "Stale death cannot kill active life");
  Check(ledger.ConfirmDeath(first) && !ledger.ConfirmDeath(first), "Confirmed death is idempotent");
  Check(ledger.ReserveDeployment(1) == 0, "One life blocks second deployment");
  Check(ledger.AdjustAllowance(1), "Authority domain permits one life grant");
  int next = ledger.ReserveDeployment(1);
  Check(next > first && ledger.CommitDeployment(next), "Limited Lives alone permits the granted deployment");
  Check(!ledger.ConfirmDeath(first) && ledger.IsAlive(), "Old corpse callback cannot affect next life");
  Check(ledger.AdjustAllowance(-1) && ledger.IsAlive(), "Removing allowance does not kill current body");
  ledger.ResetAllowance();
  Check(ledger.IsAlive() && ledger.RemainingDeployments(1) == 0, "Reset retains active first life");
  ledger.ConfirmDeath(next);
  ledger.ResetAllowance();
  Check(ledger.RemainingDeployments(1) == 1, "Explicit dead-account reset restores allowance");
 }

 protected void CheckTrauma()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient patient = new BCMM_Patient();
  int retained = patient.RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 2, 0.39, settings);
  int exited = patient.RegisterInjury(BCMM_BodyRegion.RIGHT_ARM, BCMM_InjuryCause.GUNSHOT, 2, 0.4, settings);
  Check(patient.FindWound(retained).RetainedProjectile && !patient.FindWound(exited).RetainedProjectile, "Retained chance has deterministic forty percent boundary");
  Check(patient.DressWound(retained, 0, settings), "Retained wound can be temporarily bandaged");
  BCMM_Wound wound = patient.FindWound(retained);
  Check(wound.Advance(99, BCMM_Activity.RUNNING, settings) == 0 && wound.Bandaged, "Running wears dressing before reopening");
  Check(wound.Advance(2, BCMM_Activity.RUNNING, settings) > 0 && !wound.Bandaged, "Running reopens minimum dressing at one hundred seconds");
  Check(Math.AbsFloat(wound.FlowMlPerSecond() - 1.3) < 0.001, "Reopened wound bleeds less");
  BCMM_ProcedureCapabilities capability = new BCMM_ProcedureCapabilities();
  capability.SutureKit = true; capability.QualifiedSurgeon = true;
  patient.DressWound(retained, 0.5, settings);
  Check(!patient.SutureWound(retained, capability), "Retained projectile blocks definitive closure in BCM rules");
  Check(!patient.ExtractProjectile(retained, capability, settings), "Extraction needs surgery kit");
  capability.SurgeryKit = true;
  Check(patient.ExtractProjectile(retained, capability, settings) && patient.SutureWound(retained, capability), "Extraction and suture are distinct kit-gated procedures");
  Check(wound.Advance(10000, BCMM_Activity.RUNNING, settings) == 0 && wound.Sutured, "Sutured wound does not reopen from old dressing timer");
  Check(!patient.SutureWound(retained, capability), "Duplicate suture cannot commit twice");
  int head = patient.RegisterInjury(BCMM_BodyRegion.HEAD, BCMM_InjuryCause.GUNSHOT, 1, 0, settings);
  Check(!patient.ExtractProjectile(head, capability, settings), "Head surgery requires facility");
  capability.AtMedicalFacility = true;
  Check(patient.ExtractProjectile(head, capability, settings), "Qualified facility head extraction");
  int cut = patient.RegisterInjury(BCMM_BodyRegion.LEFT_LEG, BCMM_InjuryCause.LACERATION, 1, 0, settings);
  Check(!patient.FindWound(cut).RetainedProjectile, "Cuts never receive retained bullet flag");
  Check(!patient.SutureWound(cut, capability), "Uncontrolled wound must be stabilised before suture");
  patient.DressWound(cut, 1, settings);
  Check(patient.FindWound(cut).Advance(599, BCMM_Activity.RESTING, settings) == 0, "Maximum resting dressing lasts up to ten minutes");
  patient.FindWound(cut).Advance(2, BCMM_Activity.RESTING, settings);
  Check(patient.FindWound(cut).Reopened, "Non-bullet wounds also reopen");
  patient.LeftLeg.Broken = true;
  Check(!patient.LegsAllowJump() && patient.LegMovementScale(settings) < 0.1, "Unsplinted fracture is extremely slow and cannot jump");
  Check(patient.LeftLeg.ApplySplint() && patient.LegMovementScale(settings) > 0.5, "Splint improves movement without healing fracture");
  Check(patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.RESTING, settings) == 0, "Resting fracture has no movement-induced loss");
  Check(patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.CRAWLING, settings) == 0, "Crawling does not add weight-bearing fracture loss");
  Check(patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.RUNNING, settings) > patient.LeftLeg.MotionBloodLoss(10, BCMM_Activity.WALKING, settings), "Running loads fractured limb more than walking");
  Check(!patient.LeftLeg.DefinitiveRepair(false, true, true) && patient.LeftLeg.Broken, "Field repair cannot heal fracture");
  Check(patient.LeftLeg.DefinitiveRepair(true, true, true) && patient.LegsAllowJump(), "Facility treatment repairs fracture");
  int chestBleed = patient.AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 2);
  BCMM_InternalBleeding source = patient.FindInternalBleed(chestBleed);
  Check(source.Advance(10) == 20 && source.PooledMl == 20, "Internal bleeding loses circulating blood and creates local collection");
  Check(source.DrainPleural(10) == 10 && !source.Repaired && source.Advance(1) == 2, "Drain removes collection without repairing active source");
  capability.AtMedicalFacility = false;
  Check(!patient.RepairInternalBleeding(chestBleed, capability, settings), "Critical internal surgery respects facility restriction");
  capability.AtMedicalFacility = true;
  Check(patient.RepairInternalBleeding(chestBleed, capability, settings) && source.Advance(10) == 0, "Repair stops internal source");
  int abdomen = patient.AddInternalBleeding(BCMM_BodyRegion.ABDOMEN, BCMM_BleedCompartment.ABDOMEN, 1);
  Check(patient.FindInternalBleed(abdomen).DrainPleural(100) == 0, "Chest drain cannot treat abdominal bleeding");
  // Exact event split must not depend on server frame length.
  BCMM_Wound coarse = new BCMM_Wound(); BCMM_Wound fine = new BCMM_Wound();
  coarse.UncontrolledFlowMlPerSecond = 2; fine.UncontrolledFlowMlPerSecond = 2;
  coarse.Dress(settings, 0); fine.Dress(settings, 0);
  float coarseLoss = coarse.Advance(110, BCMM_Activity.RUNNING, settings);
  float fineLoss;
  for (int i = 0; i < 110; i++) fineLoss += fine.Advance(1, BCMM_Activity.RUNNING, settings);
  Check(Math.AbsFloat(coarseLoss - fineLoss) < 0.01, "Reopening blood loss is invariant to coarse/fine frame steps");
 }

 protected void CheckTransport()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_CarryConsent consent = new BCMM_CarryConsent();
  Check(consent.Request(1, 10, 20, 5, 100, settings), "Conscious carry request created");
  Check(!consent.Respond(1, 10, true, 101), "Carrier cannot consent for patient");
  Check(consent.Respond(1, 20, true, 101), "Patient accepts carry request");
  Check(!consent.Consume(1, 11, 20, 5, 102) && !consent.Consume(1, 10, 20, 6, 102), "Consent bound to carrier and patient life");
  Check(consent.Consume(1, 10, 20, 5, 102) && !consent.Consume(1, 10, 20, 5, 102), "Consent consumed exactly once");
  Check(!consent.Respond(1, 20, true, 103), "Consumed consent rejects replayed acceptance");
  consent.Request(2, 10, 20, 5, 200, settings);
  Check(!consent.Respond(2, 20, true, 215), "Consent expires at configured boundary");
  Check(BCMM_CarryConsent.NeedsConsent(true, true) && !BCMM_CarryConsent.NeedsConsent(false, true) && !BCMM_CarryConsent.NeedsConsent(true, false), "Only a conscious player needs a transport consent prompt");
  Check(consent.Request(3, 10, 20, 5, 300, settings, BCMM_TransportMode.CASUALTY_DRAG) && consent.Respond(3, 20, true, 301), "Conscious player can accept a specifically identified drag request");
  Check(!consent.Consume(3, 10, 20, 5, 302) && consent.Consume(3, 10, 20, 5, 302, BCMM_TransportMode.CASUALTY_DRAG), "Drag acceptance cannot authorize a different carry action");
  Check(consent.Request(4, 10, 20, 5, 400, settings) && !consent.Cancel(3, 10) && !consent.Cancel(4, 30), "Stale request and third party cannot cancel a new carry request");
  Check(!consent.Request(99, 11, 20, 5, 401, settings), "An active offer cannot be overwritten by another request");
  Check(consent.Respond(4, 20, true, 401) && consent.Cancel(4, 10) && !consent.Consume(4, 10, 20, 5, 402), "Requester cancellation revokes accepted consent before pickup");
  Check(consent.Request(5, 10, 20, 5, 500, settings) && consent.Respond(5, 20, false, 501) && !consent.Respond(5, 20, true, 502) && !consent.Consume(5, 10, 20, 5, 502), "Declined request cannot later be accepted or consumed");
  Check(!consent.Request(6, 10, 20, 5, 600, settings, BCMM_TransportMode.BODY_BAG_MANUAL_DRAG), "Living player consent never authorizes a body-bag operation");
  BCMM_BodyBagDragSession session = new BCMM_BodyBagDragSession();
  Check(!session.Begin(10, 1, true, false), "Rope drag requires rope item capability");
  Check(session.Begin(10, 1, true, true) && session.AllowsWeapon() && session.AllowsRunning() && session.AllowsCrouching(), "Rope drag policy permits shooting running and crouching");
  Check(!session.Begin(11, 2, false, false) && !session.Release(11, 1), "Second actor cannot steal bag");
  vector desired;
  Check(session.DesiredBagPosition("0 0 0", "3 0 0", true, true, settings, desired) && vector.Distance(desired, "2 0 0") < 0.001, "Rope target respects configured length");
  Check(!session.DesiredBagPosition("0 0 0", "3 0 0", true, false, settings, desired) && session.Mode() == BCMM_TransportMode.NONE, "Obstacle releases instead of teleporting bag through wall");
  session.Begin(10, 2, false, false);
  Check(!session.AllowsWeapon() && !session.AllowsRunning(), "Manual dragging has occupied hands and limited locomotion");
  Check(!session.DesiredBagPosition("0 0 0", "20 0 0", true, true, settings, desired), "Large separation triggers safe release");
 }

 protected void CheckSettings()
 {
  BCMM_Settings settings = new BCMM_Settings();
  for (int i = 0; i < settings.Count(); i++) Check(settings.Definition(i).Accepts(settings.ValueAt(i)), "Catalogue default in range: " + settings.Definition(i).Key);
  Check(!settings.Set("retainedBulletChance", 2), "Out of range rejected");
  Check(!settings.Set("totalLives", 1.5), "Integer lives rejects fraction");
  Check(!settings.Set("bandageReopenMinSeconds", 700), "Inverted min max rejected");
  Check(!settings.Set("ropeLengthMetres", 5) || settings.Get("ropeBreakDistanceMetres") > 5, "Rope length stays below release distance");
  Check(!SCR_BaseGameMode.BCMM_IsSettingsEditor(null), "No editor manager means no settings authority");
  BCMM_Settings replicated = new BCMM_Settings(); array<float> snapshotValues = {};
  replicated.ExportValues(snapshotValues);
  snapshotValues[replicated.IndexOf("bandageReopenMinSeconds")]=900;
  snapshotValues[replicated.IndexOf("bandageReopenMaxSeconds")]=1200;
  Check(replicated.ImportReplicatedValues(snapshotValues) && replicated.Get("bandageReopenMinSeconds")==900 && replicated.Get("bandageReopenMaxSeconds")==1200,"Bulk server settings can change related limits atomically");
  snapshotValues[replicated.IndexOf("bandageReopenMaxSeconds")]=600;
  Check(!replicated.ImportReplicatedValues(snapshotValues) && replicated.Get("bandageReopenMaxSeconds")==1200,"Conflicting replicated limits retain previous complete snapshot");
  snapshotValues[replicated.IndexOf("bandageReopenMaxSeconds")]=1200;
  snapshotValues[replicated.IndexOf("stretcherMaximumTurnDegreesPerSecond")]=999;
  Check(!replicated.ImportReplicatedValues(snapshotValues) && replicated.Get("stretcherMaximumTurnDegreesPerSecond")==120,"Out-of-range replicated transport control cannot partially overwrite settings");
  snapshotValues.Remove(snapshotValues.Count()-1);
  Check(!replicated.ImportReplicatedValues(snapshotValues),"Incomplete replicated settings cannot silently fill missing server controls with defaults");
  BCMM_SettingsDocument legacy = new BCMM_SettingsDocument();
  legacy.Schema = 1; legacy.Generation = 1;
  for (int legacyIndex = 0; legacyIndex < 27; legacyIndex++)
  {
   legacy.Keys.Insert(settings.Definition(legacyIndex).Key);
   legacy.Values.Insert(settings.ValueAt(legacyIndex));
  }
  legacy.Values[0] = 0.52;
  BCMM_Settings migrated = new BCMM_Settings();
  Check(legacy.Decode(migrated) && Math.AbsFloat(migrated.Get("retainedBulletChance") - 0.52) < 0.001 && migrated.Get("cargoInteractionRangeMetres") == 5, "Schema one settings preserve saved values and add cargo defaults");
  BCMM_SettingsDocument medical25=new BCMM_SettingsDocument();medical25.Schema=25;medical25.Generation=1;
  for(int medicalIndex=0;medicalIndex<250;medicalIndex++) { medical25.Keys.Insert(settings.Definition(medicalIndex).Key);medical25.Values.Insert(settings.ValueAt(medicalIndex)); }
  medical25.Values[settings.IndexOf("maskScale")]=1.25;
  BCMM_Settings burialMigrated=new BCMM_Settings();
  Check(medical25.Decode(burialMigrated) && burialMigrated.Count()==279 && burialMigrated.Get("maskScale")==1.25 && burialMigrated.Get("burialEnabled")==1 && burialMigrated.Get("burialDurationSeconds")==30 && burialMigrated.Get("burialRangeMetres")==4,"Schema twenty-five preserves native Medical values and appends saved burial defaults");
  medical25.Keys.Remove(249);medical25.Values.Remove(249);medical25.Keys.Insert("burialEnabled");medical25.Values.Insert(1);
  Check(!medical25.Decode(new BCMM_Settings()),"Schema twenty-five rejects replacing a required legacy key with a new burial key");
  for(int oldSchema=27;oldSchema<=29;oldSchema++)
  {
   BCMM_SettingsDocument old=new BCMM_SettingsDocument();old.Schema=oldSchema;old.Generation=1;
   for(int oldIndex=0;oldIndex<254;oldIndex++){old.Keys.Insert(settings.Definition(oldIndex).Key);old.Values.Insert(settings.ValueAt(oldIndex));}
   old.Keys.Insert("requireBodyDisposition");old.Values.Insert(1);old.Keys.Insert("dispositionTimeoutSeconds");old.Values.Insert(1234);
   if(oldSchema>=28){old.Keys.Insert("respawnRequestsEnabled");old.Values.Insert(1);old.Keys.Insert("respawnRequestCooldownSeconds");old.Values.Insert(777);}
   if(oldSchema==29){old.Keys.Insert("spectatorWhileWaitingEnabled");old.Values.Insert(1);}
   old.Values[settings.IndexOf("totalLives")]=7;old.Values[settings.IndexOf("maskScale")]=1.25;
   BCMM_Settings migratedOld=new BCMM_Settings();
   Check(old.Decode(migratedOld) && migratedOld.Count()==279 && migratedOld.Get("totalLives")==7 && migratedOld.Get("maskScale")==1.25 && migratedOld.Get("sendHomeEnabled")==1,"Historical settings migrate without body-bag respawn controls and preserve Limited Lives and medical values");
   Check(migratedOld.IndexOf("requireBodyDisposition")==-1 && migratedOld.IndexOf("spectatorWhileWaitingEnabled")==-1 && migratedOld.IndexOf("respawnRequestsEnabled")==-1,"Retired respawn controls never enter the active catalogue");
   old.Values[old.Keys.Find("requireBodyDisposition")]=2;
   Check(!old.Decode(new BCMM_Settings()),"Historical retired controls retain strict validation instead of silently accepting corruption");
  }
  BCMM_SettingsStore store = new BCMM_SettingsStore("$profile:BCMM_SettingsChecks");
  store.Load(settings);
  settings.Set("burialDurationSeconds",42);settings.Set("burialEnabled",0);settings.Set("burialRangeMetres",5);
  Check(settings.Set("retainedBulletChance", 0.52) && store.Save(settings), "Settings JSON write and verify");
  BCMM_Settings loaded = new BCMM_Settings();
  BCMM_SettingsStore reload = new BCMM_SettingsStore("$profile:BCMM_SettingsChecks");
  Check(reload.Load(loaded) && Math.AbsFloat(loaded.Get("retainedBulletChance") - 0.52) < 0.001, "Settings JSON round trip");
  Check(loaded.Get("burialDurationSeconds")==42 && loaded.Get("burialEnabled")==0 && loaded.Get("burialRangeMetres")==5,"Saved native burial enable, duration and range survive JSON reload");
  Check(loaded.Set("retainedBulletChance", 0.63) && reload.Save(loaded), "Alternate settings generation saved");
  BCMM_SettingsDocument invalid = new BCMM_SettingsDocument();
  invalid.Schema = 999; invalid.Generation = 999;
  FileIO.MakeDirectory("$profile:BCMM_InvalidSettings");
  invalid.PackToFile("$profile:BCMM_InvalidSettings/settings-a.json");
  BCMM_SettingsStore brokenStore = new BCMM_SettingsStore("$profile:BCMM_InvalidSettings");
  Check(!brokenStore.Load(loaded) && !brokenStore.IsWritable() && !brokenStore.Save(loaded), "Unknown/corrupt settings preserved and edits locked");
 }

 protected void CheckRestartPhase()
 {
  string phase;
  if (!System.GetCLIParam("bcmmPersistencePhase", phase)) return;
  BCMM_Settings state = new BCMM_Settings();
  BCMM_SettingsStore store = new BCMM_SettingsStore();
  if (phase == "seed")
  {
   store.Load(state);
   state.Set("retainedBulletChance", 0.52);
   Check(store.Save(state), "Restart seed generation one");
   state.Set("retainedBulletChance", 0.63);
   state.Set("splintApplicationSeconds", 9);
   state.Set("surgeryDurationSeconds", 75);
   state.Set("sutureDurationSeconds", 90);
   state.Set("bandageApplicationSeconds", 12);
   state.Set("morphineOnsetSeconds", 45);
   state.Set("medicationRepeatSeconds", 90);
   state.Set("painAimingPenalty", 0.65);
   state.Set("procedurePreparationSeconds", 23);
   state.Set("procedureCompletionGraceSeconds", 7);
   state.Set("headAirwayOcclusionChance", 0.3);
   state.Set("fluidInfusionSeconds", 80);
   state.Set("treatmentMovementToleranceMetres", 0.7);
   state.Set("stretcherCarryEnabled",0);state.Set("stretcherStabilityEnabled",0);
   state.Set("stretcherDressingWearReduction",0.5);state.Set("stretcherCarrySpeedFraction",0.4);
   state.Set("stretcherHandleOffsetMetres",1.75);state.Set("stretcherMaximumSpanMetres",3.8);
   state.Set("oxygenFlowLitresPerMinute",7);state.Set("oxygenCapacityLitres",200);state.Set("fentanylMinimumSystolicPressure",105);state.Set("paracetamolOnsetSeconds",90);state.Set("maskOffsetRight",0.025);state.Set("maskScale",1.3);state.Set("tabletAnimationEnabled",0);state.Set("arsenalCostOxygen",21);state.Set("arsenalUSEnabled",0);
   state.Set("salineRetainedFraction",0.3);state.Set("plasmaRetainedFraction",0.9);state.Set("fluidRedistributionSeconds",450);state.Set("fluidDilutionWearMultiplier",0.8);
   state.Set("epinephrineMaximumHeartRate",170);state.Set("morphineMinimumSystolicPressure",95);state.Set("txaInternalBleedReduction",0.3);state.Set("equipmentVisualRangeMetres",30);
   state.Set("burialEnabled",0);state.Set("sendHomeEnabled",0);state.Set("totalLives",7);
   Check(store.Save(state), "Restart seed generation two");
  }
  if (phase == "verify") { Check(store.Load(state) && Math.AbsFloat(state.Get("retainedBulletChance") - 0.63) < 0.001, "Separate server process reloads latest settings"); Check(Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("retainedBulletChance") - 0.63) < 0.001, "Actual game mode loads persisted value before gameplay"); }
  if (phase == "verify")
  {
   SCR_BaseGameMode restarted=SCR_BaseGameMode.Cast(GetGame().GetGameMode());
   Check(state.Count()==279 && state.Get("burialEnabled")==0 && state.Get("sendHomeEnabled")==0 && state.Get("totalLives")==7,"Separate server process reloads nondefault integrated body disposition controls");
   Check(restarted.BCMM_ReadSetting("burialEnabled")==0 && restarted.BCMM_ReadSetting("sendHomeEnabled")==0 && restarted.BCMM_ReadSetting("totalLives")==7,"Actual restarted native game mode applies saved disposition policy");
   Check(state.Get("oxygenFlowLitresPerMinute")==7 && state.Get("oxygenCapacityLitres")==200 && state.Get("fentanylMinimumSystolicPressure")==105 && state.Get("paracetamolOnsetSeconds")==90,"Separate restart preserves oxygen and expanded-medication settings");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("oxygenFlowLitresPerMinute")==7,"Actual restarted server applies saved oxygen flow");
   Check(Math.AbsFloat(state.Get("maskOffsetRight")-0.025)<0.001 && Math.AbsFloat(state.Get("maskScale")-1.3)<0.001 && state.Get("tabletAnimationEnabled")==0 && state.Get("arsenalCostOxygen")==21 && state.Get("arsenalUSEnabled")==0,"Separate restart preserves mask fitting, tablet fallback and arsenal price/availability controls");
   Check(Math.AbsFloat(state.Get("salineRetainedFraction")-0.3)<0.001 && Math.AbsFloat(state.Get("plasmaRetainedFraction")-0.9)<0.001 && state.Get("fluidRedistributionSeconds")==450 && Math.AbsFloat(state.Get("fluidDilutionWearMultiplier")-0.8)<0.001,"Separate process preserves newest typed-fluid controls");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("fluidRedistributionSeconds")==450,"Actual server applies saved typed-fluid kinetics after restart");
   Check(state.Get("epinephrineMaximumHeartRate")==170 && state.Get("morphineMinimumSystolicPressure")==95 && Math.AbsFloat(state.Get("txaInternalBleedReduction")-0.3)<0.001 && state.Get("equipmentVisualRangeMetres")==30,"Separate process preserves newest clinical and equipment display controls");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("epinephrineMaximumHeartRate")==170,"Actual server game mode applies saved medication threshold after restart");
   Check(state.Get("stretcherCarryEnabled")==0 && state.Get("stretcherStabilityEnabled")==0 && Math.AbsFloat(state.Get("stretcherDressingWearReduction")-0.5)<0.001,"Separate server process reloads saved stretcher carry and stability policy");
   Check(Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("stretcherCarrySpeedFraction")-0.4)<0.001 && Math.AbsFloat(state.Get("stretcherHandleOffsetMetres")-1.75)<0.001 && Math.AbsFloat(state.Get("stretcherMaximumSpanMetres")-3.8)<0.001,"Actual game mode uses saved carrier speed, handle reach and separation after restart");
   Check(state.Get("morphineOnsetSeconds") == 45 && state.Get("medicationRepeatSeconds") == 90 && Math.AbsFloat(state.Get("painAimingPenalty") - 0.65) < 0.001, "Separate process reloads medication kinetics, repeat policy and pain handling");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("morphineOnsetSeconds") == 45 && Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("painAimingPenalty") - 0.65) < 0.001, "Actual game mode uses saved pain and medication values after restart");
   Check(state.Get("procedurePreparationSeconds") == 23 && state.Get("procedureCompletionGraceSeconds") == 7 && state.Get("fluidInfusionSeconds") == 80, "Separate process reloads preparation, animation grace and equipment timing");
   Check(Math.AbsFloat(state.Get("headAirwayOcclusionChance") - 0.3) < 0.001 && SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("procedurePreparationSeconds") == 23 && SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("fluidInfusionSeconds") == 80, "Actual game mode uses saved injury and procedure controls after restart");
   Check(state.Get("surgeryDurationSeconds") == 75 && state.Get("sutureDurationSeconds") == 90 && state.Get("bandageApplicationSeconds") == 12, "Separate process reloads surgery, suture and bandage times");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("bandageApplicationSeconds") == 12, "Actual game mode uses saved bandage time after restart");
   Check(state.Get("splintApplicationSeconds") == 9 && Math.AbsFloat(state.Get("treatmentMovementToleranceMetres") - 0.7) < 0.001, "Separate process reloads saved splint duration and movement tolerance");
   Check(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("splintApplicationSeconds") == 9, "Native game mode uses saved splint time after restart");
  }
  if (phase == "recover") { Check(store.Load(state) && Math.AbsFloat(state.Get("retainedBulletChance") - 0.52) < 0.001, "Corrupted latest generation falls back to previous settings"); Check(Math.AbsFloat(SCR_BaseGameMode.Cast(GetGame().GetGameMode()).BCMM_ReadSetting("retainedBulletChance") - 0.52) < 0.001, "Game mode uses recovered generation"); }
 }

 protected void CheckMedicalInputConfig()
 {
  SCR_KeyBindingMenuConfig config = SCR_KeyBindingMenuConfig.Cast(SCR_BaseContainerTools.CreateInstanceFromPrefab("{4EE7794C9A3F11EF}Configs/System/keyBindingMenu.conf", true));
  bool medical;
  bool nativeControls;
  bool retiredBodyBagBinding;
  int modes;
  if (config)
  {
   foreach (SCR_KeyBindingCategory category : config.m_KeyBindingCategories)
   {
    if (category.m_sName == "Common") nativeControls = true;
    foreach (SCR_KeyBindingEntry entry : category.m_KeyBindingEntries)
    {
     if(entry.m_sActionName=="BCMM_OpenRespawn" || entry.m_sActionName=="BCMM_SpectatorRequest")retiredBodyBagBinding=true;
     if(entry.m_sActionName=="BCMM_OpenMedical" && entry.m_ePrefixType==SCR_EActionPrefixType.BCMM_MEDICAL_OPEN)medical=true;
    }
   }
   foreach (SCR_KeyBindingFilter filter : config.m_aInputFilters)
    if (filter.GetFilterType() == SCR_EActionPrefixType.BCMM_MEDICAL_OPEN) modes++;
  }
  Check(!retiredBodyBagBinding,"Removed body-bag respawn and spectator actions are absent from native key bindings");
  Check(config && nativeControls && medical && modes == 2, "Rebindable medical action preserves native controls and offers tap/hold modes");
 }
 protected void CheckMedicalMenuConfig()
 {
  Resource resource = Resource.Load("{C747AFB6B750CE9A}Configs/System/chimeraMenus.conf");
  Check(resource && resource.IsValid(), "Native medical-menu configuration loads");
  if (!resource || !resource.IsValid()) return;
  BaseContainer container = resource.GetResource().ToBaseContainer();
  BaseContainerList presets = container.GetObjectArray("MenuPresets");
  bool medical;
  bool main;
  bool pause;
  bool inventory;
  bool bodyBagAdmin;
  if (presets) for (int i = 0; i < presets.Count(); i++)
  {
   string type;
   presets.Get(i).Get("Class", type);
   if(type=="BCMM_RespawnMenu")bodyBagAdmin=true;
   if (type == "BCMM_MedicalMenu") medical = true;
   if (type == "MainMenuUI") main = true;
   if (type == "PauseMenuUI") pause = true;
   if (type == "SCR_InventoryMenuUI") inventory = true;
  }
  Check(!bodyBagAdmin,"Removed Body Bag Admin menu has no native menu preset");
  Check(medical && main && pause && inventory, "Original medical menu preserves native main, pause and inventory presets");
 }
 protected void CheckGMResource()
 {
  Resource resource = Resource.Load("{F3D6C6D25642352C}Configs/Editor/AttributeLists/Edit.conf");
  Check(resource && resource.IsValid(), "Native GM attribute override resource loads");
  if (!resource || !resource.IsValid()) return;
  BaseContainer container = resource.GetResource().ToBaseContainer();
  BaseContainerList attributes = container.GetObjectArray("m_aAttributes");
  int medicalAttributes;
  array<ResourceName> categories = {};
  bool nativeTime;
  int sectionHeadings;
  if (attributes)
  {
   for (int i = 0; i < attributes.Count(); i++)
    if (attributes.Get(i).GetClassName().StartsWith("BCMM_Setting_"))
    {
     medicalAttributes++;
     ResourceName category;
     attributes.Get(i).Get("m_CategoryConfig", category);
     if (!categories.Contains(category)) categories.Insert(category);
    }
  }
  if (attributes) for (int sectionIndex = 0; sectionIndex < attributes.Count(); sectionIndex++)
   if (attributes.Get(sectionIndex).GetClassName() == "BCMM_SectionHeadingAttribute") sectionHeadings++;
  BCMM_Settings defaults = new BCMM_Settings();
  if (attributes) for (int nativeIndex = 0; nativeIndex < attributes.Count(); nativeIndex++) if (attributes.Get(nativeIndex).GetClassName() == "SCR_TimePresetsEditorAttribute") nativeTime = true;
  Check(nativeTime, "Adding BCM settings preserves native Game Master time controls");
  Check(medicalAttributes == defaults.Count(), "Every registered setting has a loaded native GM control");
  bool categoryResources = true;
  foreach (ResourceName categoryPath : categories)
  {
   Resource categoryResource = Resource.Load(categoryPath);
   if (!categoryResource || !categoryResource.IsValid()) categoryResources = false;
  }
  Check(categories.Count() == 1 && categoryResources && sectionHeadings == 15, "Every GM setting shares one BCM Medical tab with fifteen inline section headings");
  Resource actions = Resource.Load("{2C1D87DE93C77A27}Configs/Editor/ActionLists/Context/TempEdit.conf");
  BaseContainerList contextActions = actions.GetResource().ToBaseContainer().GetObjectArray("m_Actions");
  int roleActions;int allowanceActions;
  bool nativeBleed;
  for (int actionIndex = 0; actionIndex < contextActions.Count(); actionIndex++)
  {
   string kind = contextActions.Get(actionIndex).GetClassName();
   if (kind == "BCMM_SetMedicContextAction") roleActions++;
   if(kind=="BCMM_AdjustPlayerLivesContextAction")allowanceActions++;
   if (kind == "SCR_BleedCharacterContextAction") nativeBleed = true;
  }
  PrintFormat("BCMM_ROLE_LIST count=%1 roles=%2 native=%3", contextActions.Count(), roleActions, nativeBleed);
  Check(roleActions == 2 && nativeBleed, "Make medic and remove role merge into native GM context actions without replacing native controls");
  Check(allowanceActions==3,"Native GM context contains exactly three targeted Limited Lives actions");
  BCMM_SettingsDocument priorMedic = new BCMM_SettingsDocument();
  priorMedic.Schema = 15; priorMedic.Generation = 1;
  for (int priorIndex = 0; priorIndex < 105; priorIndex++) { priorMedic.Keys.Insert(defaults.Definition(priorIndex).Key); priorMedic.Values.Insert(defaults.ValueAt(priorIndex)); }
  priorMedic.Values[0] = 0.27;
  BCMM_Settings roleSettings = new BCMM_Settings();
  Check(priorMedic.Decode(roleSettings) && roleSettings.Get("retainedBulletChance") == 0.27 && roleSettings.Get("medicTimeReductionPercent") == 40 && roleSettings.Get("medicOnlyAED") == 0, "Schema fifteen preserves existing server choices and gains configurable medic defaults");
 } protected void CheckBodyBagResources()
 {
  array<ResourceName> bagResources = {
   "{98555BDCD0BE490A}Assets/BodyBags/BCM_FilledBodyBag/BCM_FilledBodyBag_Collision.xob",
   "{977722F41091B866}Assets/BodyBags/BCM_EmptyBodyBag/BCM_EmptyBodyBag.xob",
   "{60F57EB7414F43B2}Assets/BodyBags/BCM_FilledBodyBag/Data/BCM_BodyBag.emat"
  };
  foreach (ResourceName path : bagResources)
  {
   Resource resource = Resource.Load(path);
   Check(resource && resource.IsValid(), "Owned body bag resource loads from standalone mod: " + path);
  }
 } protected void CheckTXA()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient treated = new BCMM_Patient();
  BCMM_Patient untreated = new BCMM_Patient();
  int treatedId = treated.RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 1, 0, settings);
  int untreatedId = untreated.RegisterInjury(BCMM_BodyRegion.LEFT_ARM, BCMM_InjuryCause.GUNSHOT, 1, 0, settings);
  treated.DressWound(treatedId, 0, settings); untreated.DressWound(untreatedId, 0, settings);
  Check(treated.Medication.Administer(BCMM_MedicationKind.TXA, settings), "TXA is a distinct accepted medication type");
  Check(!treated.Medication.CanAdminister(BCMM_MedicationKind.TXA, settings), "TXA repeat and active-dose rules prevent immediate stacking");
  treated.Medication.Advance(30, treated, settings);
  Check(treated.Medication.TXAEffect > 0.99 && Math.AbsFloat(treated.Medication.ClotWearScale(settings) - 0.5) < 0.001, "TXA onset produces configured clot stability");
  Check(treated.Medication.BreathingScale == 1 && treated.Medication.PressureScale == 1 && treated.Medication.OpioidEffect == 0, "TXA is not an opioid or pressure-restoration shortcut");
  treated.AdvanceTrauma(150, BCMM_Activity.RUNNING, settings); untreated.AdvanceTrauma(150, BCMM_Activity.RUNNING, settings);
  Check(treated.FindWound(treatedId).Bandaged && !untreated.FindWound(untreatedId).Bandaged, "TXA stabilises an existing dressing under the same movement exposure");
  Check(treated.FindWound(treatedId).RetainedProjectile && !treated.FindWound(treatedId).Sutured, "TXA neither extracts a projectile nor performs surgical closure");
  Check(treated.Medication.History().Contains("TXA x1"), "TXA appears in the patient medication record");
  treated.Medication.Advance(601, treated, settings);
  Check(treated.Medication.TXAEffect == 0 && treated.Medication.ActiveDoses() == 0 && treated.Medication.ClotWearScale(settings) == 1, "Expired TXA releases its effect rather than leaving permanent protection");
  BCMM_SettingsDocument prior = new BCMM_SettingsDocument(); prior.Schema = 17; prior.Generation = 1;
  for (int i = 0; i < 115; i++) { prior.Keys.Insert(settings.Definition(i).Key); prior.Values.Insert(settings.ValueAt(i)); }
  prior.Values[0] = 0.29;
  BCMM_Settings migrated = new BCMM_Settings();
  Check(prior.Decode(migrated) && Math.AbsFloat(migrated.Get("retainedBulletChance") - 0.29) < 0.001 && migrated.Get("txaRequiresVascularAccess") == 1, "Prior server settings survive TXA schema migration");
 }
 protected void CheckStretcherPolicy()
 {
  BCMM_Settings oldCurrent=new BCMM_Settings();BCMM_SettingsDocument schema19=new BCMM_SettingsDocument();schema19.Schema=19;schema19.Generation=1;
  for(int priorIndex=0;priorIndex<136;priorIndex++) { schema19.Keys.Insert(oldCurrent.Definition(priorIndex).Key);schema19.Values.Insert(oldCurrent.ValueAt(priorIndex)); }
  schema19.Values[oldCurrent.IndexOf("stretcherCarryHeightMetres")]=0.45;
  schema19.Values[oldCurrent.IndexOf("stretcherHandleOffsetMetres")]=1.6;
  BCMM_Settings afterPresentation=new BCMM_Settings();
  Check(schema19.Decode(afterPresentation) && Math.AbsFloat(afterPresentation.Get("stretcherCarryHeightMetres")-0.45)<0.001 && Math.AbsFloat(afterPresentation.Get("stretcherHandleOffsetMetres")-1.6)<0.001 && afterPresentation.Get("stretcherGripAnimationEnabled")==0,"Schema nineteen preserves existing carry dimensions and adds presentation controls");

  BCMM_Settings settings=new BCMM_Settings();
  BCMM_SettingsDocument prior=new BCMM_SettingsDocument();prior.Schema=18;prior.Generation=1;
  for(int i=0;i<123;i++){prior.Keys.Insert(settings.Definition(i).Key);prior.Values.Insert(settings.ValueAt(i));}
  prior.Values[0]=0.29;BCMM_Settings migrated=new BCMM_Settings();
  Check(prior.Decode(migrated) && Math.AbsFloat(migrated.Get("retainedBulletChance")-0.29)<0.001 && migrated.Get("stretcherCarryEnabled")==0,"Schema eighteen retains prior values and gains stretcher controls");
  prior.Keys.Remove(122);prior.Values.Remove(122);
  Check(!prior.Decode(new BCMM_Settings()),"Incomplete prior settings cannot silently reset stretcher migration");
  BCMM_Wound resting=new BCMM_Wound();BCMM_Wound supported=new BCMM_Wound();
  resting.UncontrolledFlowMlPerSecond=1;supported.UncontrolledFlowMlPerSecond=1;
  resting.Dress(settings,0);supported.Dress(settings,0);
  resting.Advance(350,BCMM_Activity.RESTING,settings);
  supported.Advance(350,BCMM_Activity.RESTING,settings,1-settings.Get("stretcherDressingWearReduction"));
  Check(!resting.Bandaged && supported.Bandaged,"Configured stretcher dressing stability extends support without suturing");
  Check(!supported.Sutured,"Stretcher protection does not perform surgical wound closure");
  BCMM_Wound open=new BCMM_Wound();open.UncontrolledFlowMlPerSecond=2;
  Check(open.Advance(10,BCMM_Activity.RESTING,settings,0.5)==20,"Movement protection does not stop uncontrolled bleeding");
  BCMM_Fracture leg=new BCMM_Fracture();leg.Broken=true;
  Check(leg.MotionBloodLoss(10,BCMM_Activity.RESTING,settings)==0 && leg.MotionBloodLoss(10,BCMM_Activity.WALKING,settings)>0 && leg.Broken,"Supporting a fractured leg removes loading strain without healing it");
 }
 protected void CheckPharmacology()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient patient = new BCMM_Patient();
  patient.AddWound(BCMM_BodyRegion.LEFT_LEG, 2);
  patient.LeftLeg.Broken = true;
  patient.AdvancePhysiology(1, 1, settings);
  float untreatedPain = patient.Medication.Pain;
  Check(untreatedPain > 50, "Open wounds and fractures contribute independent pain");
  Check(!patient.Medication.Administer(-1, settings) && !patient.Medication.Administer(BCMM_SupplyCatalog.MEDICATION_COUNT, settings), "Medication kind is bounded before array access");
  Check(patient.Medication.Administer(BCMM_MedicationKind.MORPHINE, settings), "First original morphine dose accepted");
  Check(patient.Medication.Pain == untreatedPain, "Administration does not instantly apply analgesia");
  Check(!patient.Medication.Administer(BCMM_MedicationKind.MORPHINE, settings), "Early repeat medication rejected");
  patient.AdvancePhysiology(30, 1, settings);
  Check(patient.Medication.Pain < untreatedPain && patient.Medication.BreathingScale < 1 && patient.SystolicPressure < 120, "Morphine onset reduces pain with breathing and pressure costs");
  Check(patient.LeftLeg.Broken && !patient.LeftLeg.AllowsJump() && patient.WoundAt(0).FlowMlPerSecond() == 2, "Analgesia does not heal fractures, stop bleeding or unlock jumping");
  Check(patient.Medication.History().Contains("Morphine"), "Medication record names the actual administration");
  patient.AdvancePhysiology(31, 1, settings);
  Check(patient.Medication.CanAdminister(BCMM_MedicationKind.MORPHINE, settings), "Repeat interval uses advancing authority patient time");
  BCMM_Patient overdose = new BCMM_Patient();
  settings.Set("medicationRepeatSeconds", 0);
  settings.Set("morphineOnsetSeconds", 1);
  settings.Set("naloxoneOnsetSeconds", 1);
  Check(overdose.Medication.Administer(0, settings) && overdose.Medication.Administer(0, settings) && overdose.Medication.Administer(0, settings), "Configured repeat policy permits finite stacked doses");
  Check(!overdose.Medication.Administer(0, settings), "Concurrent dose cap prevents unlimited stacking");
  overdose.AdvancePhysiology(1, 1, settings);
  Check(overdose.Medication.BreathingScale < 0.01, "Stacked opioid effects can suppress spontaneous ventilation");
  overdose.Medication.Administer(1, settings);
  overdose.AdvancePhysiology(1, 1, settings);
  Check(overdose.Medication.BreathingScale > 0.95, "Naloxone onset suppresses active opioid effects");
  overdose.AdvancePhysiology(190, 1, settings);
  Check(overdose.Medication.BreathingScale < 0.5, "Short antagonist clearance permits persisting opioid effects to return");
  BCMM_Patient stimulant = new BCMM_Patient();
  stimulant.Medication.Administer(2, settings);
  stimulant.AdvancePhysiology(10, 1, settings);
  Check(stimulant.HeartRate > 75 && stimulant.SystolicPressure > 120, "Epinephrine raises pulse and pressure only with circulation");
  BCMM_Patient arrest = new BCMM_Patient();
  arrest.HasSpontaneousCirculation = false;
  arrest.Rhythm = BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY;
  arrest.Medication.Administer(2, settings);
  arrest.AdvancePhysiology(10, 1, settings);
  Check(!arrest.HasSpontaneousCirculation && arrest.HeartRate == 0, "Medication alone cannot manufacture ROSC or a pulse");
  BCMM_Patient cleared = new BCMM_Patient();
  cleared.Medication.Administer(0, settings);
  cleared.AdvancePhysiology(700, 1, settings);
  Check(cleared.Medication.ActiveDoses() == 0 && cleared.Medication.BreathingScale == 1, "Finite medication effects clear without a permanent modifier");
  BCMM_SettingsDocument tenth = new BCMM_SettingsDocument();
  tenth.Schema = 10; tenth.Generation = 1;
  for (int k = 0; k < 56; k++) { tenth.Keys.Insert(settings.Definition(k).Key); tenth.Values.Insert(settings.ValueAt(k)); }
  BCMM_Settings upgraded = new BCMM_Settings();
  Check(tenth.Decode(upgraded) && upgraded.Get("morphineOnsetSeconds") == 30, "Schema ten preserves all prior settings and gains medication defaults");
  BCMM_SettingsDocument eleventh = new BCMM_SettingsDocument();
  eleventh.Schema = 11; eleventh.Generation = 1;
  for (int e = 0; e < 77; e++) { eleventh.Keys.Insert(settings.Definition(e).Key); eleventh.Values.Insert(settings.ValueAt(e)); }
  BCMM_Settings withHandling = new BCMM_Settings();
  Check(eleventh.Decode(withHandling) && withHandling.Get("morphineOnsetSeconds") == 1 && Math.AbsFloat(withHandling.Get("painAimingPenalty") - 0.4) < 0.001, "Schema eleven retains chosen medication kinetics and gains pain handling default");
  tenth.Keys.Remove(0); tenth.Values.Remove(0);
  Check(!tenth.Decode(new BCMM_Settings()), "Incomplete schema ten documents remain invalid");
 }
 protected void CheckEquipmentPolicies()
 {
  BCMM_Settings settings = new BCMM_Settings();
  BCMM_Patient patient = new BCMM_Patient();
  patient.HasSpontaneousCirculation = false;
  patient.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  patient.Equipment.Analyse(patient, 42);
  Check(!patient.Equipment.MayShock(patient, 42, settings), "AED analysis alone does not replace applied pads");
  patient.Equipment.PadsAttached = true;
  Check(patient.Equipment.MayShock(patient, 42, settings) && !patient.Equipment.MayShock(patient, 43, settings), "Fresh AED analysis belongs to the exact analysing device");
  patient.StartCompressions(1, 1);
  Check(!patient.Equipment.MayShock(patient, 42, settings), "Active CPR prevents AED discharge until stand clear");
  patient.StopCompressions(1, 1);
  patient.Equipment.Advance(31, patient, settings);
  Check(!patient.Equipment.MayShock(patient, 42, settings), "Expired AED analysis cannot authorize a shock");
  patient.Equipment.Analyse(patient, 42);
  patient.Rhythm = BCMM_CardiacRhythm.ASYSTOLE;
  Check(!patient.Equipment.MayShock(patient, 42, settings), "Changed non-shockable rhythm invalidates earlier shock advice");
  patient.Rhythm = BCMM_CardiacRhythm.VENTRICULAR_FIBRILLATION;
  Check(patient.DeliverShock() && !patient.HasSpontaneousCirculation && patient.Rhythm == BCMM_CardiacRhythm.PULSELESS_ELECTRICAL_ACTIVITY, "Defibrillation does not guarantee ROSC");
  Check(BCMM_EquipmentPolicy.Reusable(BCMM_EquipmentKind.AED) && BCMM_EquipmentPolicy.Reusable(BCMM_EquipmentKind.BVM) && !BCMM_EquipmentPolicy.Reusable(BCMM_EquipmentKind.PADS), "Equipment reuse policy distinguishes durable devices from consumables");
  Check(!BCMM_EquipmentPolicy.RegionAllowed(-1, 1) && !BCMM_EquipmentPolicy.RegionAllowed(15, 1) && !BCMM_EquipmentPolicy.RegionAllowed(0, 5), "Invalid equipment procedure and wrong body region rejected");
  BCMM_Patient chest = new BCMM_Patient();
  chest.LeftChest.OpenWound = true;
  chest.LeftChest.Tension = true;
  chest.LeftChest.CollapseFraction = 0.5;
  chest.LeftChest.ApplySeal();
  chest.Equipment.Advance(10, chest, settings);
  Check(chest.LeftChest.Tension && chest.LeftChest.CollapseFraction == 0.5, "A chest seal does not remove trapped pressure or instantly reinflate lung");
  chest.LeftChest.RelieveTension();
  chest.Equipment.LeftDecompressed = true;
  chest.Equipment.Advance(10, chest, settings);
  Check(chest.LeftChest.CollapseFraction < 0.5 && chest.LeftChest.CollapseFraction > 0, "Corrected pressure plus sealed chest permits configurable gradual recovery");
  int internalId = chest.AddInternalBleeding(BCMM_BodyRegion.CHEST, BCMM_BleedCompartment.LEFT_PLEURAL, 10);
  chest.AdvanceInternalTrauma(10, BCMM_Activity.RESTING, settings);
  chest.Equipment.LeftDrain = true;
  chest.Equipment.Advance(1, chest, settings);
  Check(chest.LeftChest.PleuralBloodMl == 90 && chest.FindInternalBleed(internalId).PooledMl == 90 && !chest.FindInternalBleed(internalId).Repaired, "Installed drain removes matching collected blood without repairing source");
  Check(chest.AdvanceInternalTrauma(1, BCMM_Activity.RESTING, settings) == 10, "Drain does not return lost blood or stop active internal bleeding");
  BCMM_Patient airway = new BCMM_Patient();
  airway.Airway = BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION;
  Check(!BCMM_EquipmentPolicy.Eligible(airway, BCMM_EquipmentProcedure.INSERT_AIRWAY, 0, settings) && !airway.DeliverAssistedVentilation(5), "Adjunct and BVM do not bypass foreign-material occlusion");
  airway.Airway = BCMM_AirwayCondition.PATENT;
  airway.OxygenSaturation = 0.5;
  airway.HasSpontaneousCirculation = false;
  Check(airway.DeliverAssistedVentilation(5), "Patent airway permits a finite assisted ventilation interval");
  airway.AdvancePhysiology(2, 1, settings);
  Check(airway.OxygenSaturation > 0.5 && !airway.HasSpontaneousCirculation, "Ventilation improves oxygenation without manufacturing spontaneous circulation");
  Check(!BCMM_EquipmentPolicy.Eligible(airway, BCMM_EquipmentProcedure.INFUSE, 0, settings), "Fluid equipment requires established vascular access");
  airway.Equipment.VascularAccess = true;
  Check(BCMM_EquipmentPolicy.Eligible(airway, BCMM_EquipmentProcedure.INFUSE, 0, settings), "Established vascular access permits the separate fluid procedure");
  BCMM_SettingsDocument twelfth = new BCMM_SettingsDocument();
  twelfth.Schema = 12; twelfth.Generation = 1;
  for (int i = 0; i < 78; i++) { twelfth.Keys.Insert(settings.Definition(i).Key); twelfth.Values.Insert(settings.ValueAt(i)); }
  BCMM_Settings migrated = new BCMM_Settings();
  Check(twelfth.Decode(migrated) && migrated.Get("aedShockSeconds") == 5 && migrated.Get("fluidInfusionSeconds") == 60, "Schema twelve gains equipment defaults without losing pain or medication choices");
 }
 protected void CheckTraumaGeneration()
 {
  BCMM_Settings settings = new BCMM_Settings();
  settings.Set("penetratingChestInjuryChance", 1);
  settings.Set("initialTensionChance", 1);
  settings.Set("penetratingInternalBleedChance", 1);
  settings.Set("headAirwayOcclusionChance", 1);
  BCMM_Patient chest = new BCMM_Patient();
  Check(BCMM_TraumaGeneration.Apply(chest, BCMM_BodyRegion.CHEST, 0.5, true, 0, 0, 0, 0, settings), "Authority-supplied injury samples accepted");
  Check(chest.LeftChest.OpenWound && chest.LeftChest.Tension && !chest.RightChest.OpenWound && chest.LeftChest.CollapseFraction > 0, "Generated penetrating chest injury preserves impacted side");
  Check(chest.InternalBleedCount() == 1 && chest.InternalBleedAt(0).Compartment == BCMM_BleedCompartment.LEFT_PLEURAL && chest.InternalBleedAt(0).RateMlPerSecond == 3, "Internal source severity and collection compartment follow the impact");
  chest.LeftChest.ApplySeal();
  BCMM_TraumaGeneration.Apply(chest, BCMM_BodyRegion.CHEST, 0.5, true, 0, 0, 0, 0, settings);
  Check(!chest.LeftChest.Sealed && chest.InternalBleedCount() == 2, "A new penetrating impact can defeat an existing seal and create a separate source");
  int before = chest.InternalBleedCount();
  Check(!BCMM_TraumaGeneration.Apply(chest, 1, 1, false, -1, 0, 0, 0, settings) && chest.InternalBleedCount() == before, "Invalid samples cannot partially mutate trauma");
  BCMM_Patient head = new BCMM_Patient();
  BCMM_TraumaGeneration.Apply(head, BCMM_BodyRegion.HEAD, 1, true, 0, 0, 0, 0, settings);
  Check(head.Airway == BCMM_AirwayCondition.FOREIGN_MATERIAL_OCCLUSION && head.InternalBleedAt(0).Compartment == BCMM_BleedCompartment.CRANIAL, "Head penetration can generate airway contamination and a cranial source");
  settings.Set("penetratingChestInjuryChance", 0);
  settings.Set("penetratingInternalBleedChance", 0);
  settings.Set("headAirwayOcclusionChance", 0);
  BCMM_Patient disabled = new BCMM_Patient();
  BCMM_TraumaGeneration.Apply(disabled, 1, 1, true, 0, 0, 0, 0, settings);
  BCMM_TraumaGeneration.Apply(disabled, 0, 1, true, 0, 0, 0, 0, settings);
  Check(!disabled.LeftChest.OpenWound && disabled.InternalBleedCount() == 0 && disabled.Airway == BCMM_AirwayCondition.PATENT, "GM zero probabilities disable the corresponding generated complications");
  Check(!BCMM_ClinicalChecks.Supports(BCMM_BodyRegion.CHEST, 8), "Bare-hands RPC cannot request instrument-only result key");
  BCMM_SettingsDocument previous = new BCMM_SettingsDocument();
  previous.Schema = 13; previous.Generation = 1;
  for (int i = 0; i < 96; i++) { previous.Keys.Insert(settings.Definition(i).Key); previous.Values.Insert(settings.ValueAt(i)); }
  BCMM_Settings upgraded = new BCMM_Settings();
  Check(previous.Decode(upgraded) && upgraded.Get("headAirwayOcclusionChance") > 0, "Schema thirteen preserves prior equipment settings and gains injury controls");
  BCMM_SettingsDocument fourteenth = new BCMM_SettingsDocument();
  fourteenth.Schema = 14; fourteenth.Generation = 1;
  settings.Set("fluidInfusionSeconds", 80);
  settings.Set("headAirwayOcclusionChance", 0.3);
  for (int n = 0; n < 103; n++) { fourteenth.Keys.Insert(settings.Definition(n).Key); fourteenth.Values.Insert(settings.ValueAt(n)); }
  BCMM_Settings withPreparation = new BCMM_Settings();
  Check(fourteenth.Decode(withPreparation) && withPreparation.Get("procedurePreparationSeconds") == 15 && withPreparation.Get("procedureCompletionGraceSeconds") == 5, "Schema fourteen gains independent preparation and completion grace defaults");
  Check(withPreparation.Get("fluidInfusionSeconds") == 80 && Math.AbsFloat(withPreparation.Get("headAirwayOcclusionChance") - 0.3) < 0.001, "Schema fourteen migration preserves chosen equipment and injury controls");
  fourteenth.Keys.Remove(0); fourteenth.Values.Remove(0);
  Check(!fourteenth.Decode(new BCMM_Settings()), "Incomplete schema fourteen document cannot silently discard existing controls");
 }
}
