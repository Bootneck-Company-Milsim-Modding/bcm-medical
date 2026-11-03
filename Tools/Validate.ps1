param([switch]$Beta01HoldRuntime, [switch]$Beta01StateRuntime, [ValidateSet("Jeep","Truck","Helicopter","Ambulance")][string]$CargoVehicle="Truck", [ValidateSet("","seed","verify","verify-loss")][string]$FinitePersistencePhase="", [string]$NativeSaveId="", [ValidateSet("","seed","verify")][string]$WorldFaultPhase="", [switch]$FinitePersistenceRuntime, [switch]$WorldPersistenceFaultRuntime, [switch]$ArsenalRuntime, [switch]$OxygenMedicationRuntime, [switch]$OxygenRemovalRuntime, [switch]$FluidLineRuntime, [switch]$FluidBurdenRuntime, [switch]$LegacyFluidBurdenWorldRuntime, [switch]$FluidBurdenRestartRuntime, [switch]$FluidLineBoundaryRuntime, [switch]$FluidLinePersistenceRuntime, [switch]$FluidLineRestartRuntime, [switch]$MissingFluidLineRuntime, [switch]$MissingFluidLineRestartRuntime, [switch]$FluidsRuntime, [switch]$ClinicalSafetyRuntime, [switch]$WeaponSlingRuntime, [switch]$LivingTransportRuntime, [switch]$NativeConsumableCommitRuntime, [ValidateSet("","Access","SourceMove","FullHeal","Deferred","SameSourceHeal")][string]$NativeSalineScenario="", [switch]$RetrievedSalineRuntime, [switch]$RetrievedSalineRestartRuntime, [switch]$RetrievedSalineFullHeal, [switch]$OptionalClinicalStateRuntime, [switch]$OptionalClinicalStateRestartRuntime, [ValidateSet("bcmmNativeSaline","bcmmFluidLine","bcmmFluidBurden")][string]$OptionalClinicalStateField="bcmmNativeSaline", [switch]$NativeSalineInactive, [switch]$NativeSalineWorldRuntime, [switch]$NativeSalineWorldRestartRuntime, [switch]$TransportFluidLineRuntime, [switch]$TransportFluidLineRestartRuntime, [switch]$AppliedVisualsRuntime, [switch]$GripCoordinateRuntime, [switch]$StretcherRuntime, [ValidateSet("","Surgery","Suture","Splint")][string]$FieldAnimation = "", [switch]$FieldProceduresRuntime, [switch]$PulseOximeterRuntime, [switch]$PulseAudio, [switch]$TXARuntime, [switch]$AnimationRuntime, [switch]$EquipmentRuntime, [switch]$UISmoke, [switch]$FluidUI, [switch]$MedicationRuntime, [switch]$BandageRuntime, [switch]$InternalSurgeryRuntime, [switch]$AssessmentRuntime, [switch]$SurgeryRuntime, [switch]$SplintRuntime, [switch]$PickupRuntime, [switch]$DragRuntime, [switch]$Beta01DragMatrixRuntime, [switch]$LifeRuntime, [switch]$LifeAdminRestartRuntime, [switch]$ComponentRecoveryRuntime, [switch]$ComponentRecoveryRestartRuntime, [switch]$TerrainRuntime, [switch]$CargoRollbackRuntime, [switch]$RollbackRuntime, [switch]$TestResources, [string]$DiagnosticProject = '', [switch]$BaggingRuntime, [switch]$BurialRuntime, [switch]$BurialOwner, [switch]$SendHomeRuntime, [ValidateSet('','M997','USTent','USSRTent','FIATent')][string]$HomeFacility = '', [switch]$MedicalRuntime, [switch]$CargoRuntime, [switch]$Runtime, [switch]$ServerRuntime, [int]$ScreenWidth = 640, [int]$ScreenHeight = 480, [string]$ServerDirectory = '', [string]$ProfileDirectory = '', [ValidateSet('','seed','verify','recover')][string]$PersistencePhase = '')
$ErrorActionPreference = 'Stop'
if ($Beta01HoldRuntime) { $BaggingRuntime = $true; $Runtime = $true }
if($BurialOwner){if($ServerRuntime){throw 'BurialOwner requires local native player'};$BurialRuntime=$true;$Runtime=$true}
# A requested runtime fixture must launch the game, never merely validate Workbench scripts.
foreach ($fixtureSwitch in $PSBoundParameters.Keys) {
 if ($fixtureSwitch -like '*Runtime' -and $fixtureSwitch -ne 'ServerRuntime' -and [bool]$PSBoundParameters[$fixtureSwitch]) { $Runtime = $true }
}
if($NativeSalineScenario){$Runtime=$true}
if ($StretcherRuntime) { $Runtime = $true }
if ($PulseAudio) { $PulseOximeterRuntime = $true }
if ($UISmoke -or $FluidUI) { $Runtime = $true }
if ($FieldProceduresRuntime -or $PulseOximeterRuntime -or $TXARuntime) { $Runtime = $true }
if ($FieldAnimation) { $Runtime = $true; $AnimationRuntime = $true }
$projectRoot = Split-Path $PSScriptRoot -Parent
$toolsRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger Tools'
$gameRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$profile = Join-Path $projectRoot ('.checks\' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
if ($ProfileDirectory) { $profile = [IO.Path]::GetFullPath($ProfileDirectory) }
$project = Join-Path $projectRoot 'addon.gproj'
$executable = Join-Path $toolsRoot 'Workbench\ArmaReforgerWorkbenchSteamDiag.exe'
$modeArguments = '-wbModule=ScriptEditor -validate'
if ($TestResources) { $project = Join-Path $projectRoot 'Tests\Runtime\addon.gproj' }
if($DiagnosticProject){
 if($Runtime -or $ServerRuntime -or $TestResources){throw 'DiagnosticProject is a separate compile-only fixture'}
 $project=[IO.Path]::GetFullPath((Join-Path $projectRoot $DiagnosticProject))
 if(!$project.StartsWith($projectRoot+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase) -or !(Test-Path -LiteralPath $project -PathType Leaf)){throw 'Diagnostic project must be an existing project inside this Medical workspace'}
}
$success = 'Script validation successful\.'
$runToken = [Guid]::NewGuid().ToString('N')
if ($Runtime -or $ServerRuntime) {
 $project = Join-Path $projectRoot 'Tests\Runtime\addon.gproj'
 $executable = Join-Path $gameRoot 'ArmaReforgerSteamDiag.exe'
 $modeArguments = '-world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000 -maxFPS 60'
 $success = 'BCMM_CHECK COMPLETE checks=\d+ failures=0 run=' + $runToken
}
if ($ServerRuntime) {
 $serverLocationFile = Join-Path $projectRoot '..\..\.dedicated\server-location.txt'
 if ($ServerDirectory) { $serverRoot = $ServerDirectory } else { $serverLocationFile = Join-Path $projectRoot '..\BCM NID AceCompat\.dedicated\server-location.txt'; $serverRoot = (Get-Content -LiteralPath $serverLocationFile -Raw).Trim() }
 $executable = Join-Path $serverRoot 'ArmaReforgerServerDiag.exe'
 $modeArguments = '-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -disableCrashReporter -noSound -noThrow -maxFPS 60'
 # Direct diagnostic -server worlds use the default RPL port; serialize these fixtures.
}
# Only the new project, its diagnostic addon and native game directory are exposed.
if($BurialRuntime -or $TerrainRuntime){$modeArguments=$modeArguments.Replace('worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent','Worlds/BCMM_TerrainChecks.ent')}
# Installed Workshop addons and legacy BCM projects are deliberately absent.
$addonDirs = $projectRoot + ',' + (Join-Path $projectRoot 'Tests\Runtime') + ',' + (Join-Path $gameRoot 'addons')
if($DiagnosticProject){$addonDirs=(Split-Path $project -Parent)+','+$projectRoot+','+(Join-Path $gameRoot 'addons')}
$arguments = '-gproj "' + $project + '" -profile "' + $profile + '" -addonsDir "' + $addonDirs + '" ' + $modeArguments
if($FinitePersistencePhase){if(!$FinitePersistenceRuntime -or !$ServerRuntime){throw 'FinitePersistencePhase requires finite native server fixture'};$arguments+=' -bcmmFinitePersistencePhase '+$FinitePersistencePhase+' -keepSessionSave'}
if($NativeSaveId){if($NativeSaveId -notmatch '^[0-9a-fA-F]{8}(-[0-9a-fA-F]{4}){3}-[0-9a-fA-F]{12}$'){throw 'NativeSaveId must be a save UUID'};$arguments+=' -loadSessionSave '+$NativeSaveId}
if ($FinitePersistenceRuntime) { $arguments += ' -bcmmFinitePersistenceRuntime 1 -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf"' }; if ($FinitePersistenceRuntime) { $arguments += " -bcmmSavedCargoVehicle $CargoVehicle" }
if ($WorldPersistenceFaultRuntime) { $arguments += ' -bcmmWorldPersistenceFaults 1 -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave' }
if($WorldFaultPhase){if(!$WorldPersistenceFaultRuntime -or !$ServerRuntime){throw 'WorldFaultPhase requires native server fault fixture'};$arguments+=" -bcmmWorldFaultPhase $WorldFaultPhase"}
if ($ArsenalRuntime) { $arguments += " -bcmmArsenalRuntime 1" }
if($OxygenRemovalRuntime){$arguments+=' -bcmmOxygenRemovalRuntime 1'}
if($FluidLineBoundaryRuntime){$arguments+=' -bcmmFluidLineBoundaryRuntime 1 '}
if($LegacyFluidBurdenWorldRuntime){$arguments+=' -bcmmLegacyFluidBurdenWorld 1 '}
if($FluidBurdenRuntime -or $FluidBurdenRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($FluidBurdenRuntime){$arguments+=' -bcmmFluidBurdenRuntime 1 '}else{$arguments+=' -bcmmFluidBurdenRestart 1 '}}
if($FluidLineRuntime){$arguments+=' -bcmmFluidLineRuntime 1'}
if($MissingFluidLineRuntime -or $MissingFluidLineRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($MissingFluidLineRuntime){$arguments+=' -bcmmMissingFluidLineRuntime 1 '}else{$arguments+=' -bcmmMissingFluidLineRestart 1 '}}
if($FluidLinePersistenceRuntime -or $FluidLineRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($FluidLinePersistenceRuntime){$arguments+=' -bcmmFluidLinePersistenceRuntime 1'}else{$arguments+=' -bcmmFluidLineRestartRuntime 1'}}
if ($OxygenMedicationRuntime) { $arguments += " -bcmmOxygenMedicationRuntime 1" }
if ($FluidsRuntime) { $arguments += " -bcmmFluidsRuntime 1" }
if($LivingTransportRuntime){$arguments+=" -bcmmLivingTransportRuntime 1"}
if($Beta01StateRuntime){$deadline=(Get-Date).AddSeconds(180)}
 if($WeaponSlingRuntime){$arguments+=" -bcmmWeaponSlingRuntime 1"}
if ($ClinicalSafetyRuntime) { $arguments += ' -bcmmClinicalSafetyRuntime 1' }
if($RetrievedSalineRuntime -or $RetrievedSalineRestartRuntime -or $OptionalClinicalStateRuntime -or $OptionalClinicalStateRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave '}
if($RetrievedSalineRuntime){$arguments+=' -bcmmRetrievedSalineSeed 1'}
if($RetrievedSalineRestartRuntime){$arguments+=' -bcmmRetrievedSalineRestart 1'}
if($RetrievedSalineFullHeal){$arguments+=' -bcmmRetrievedFullHeal 1'}
if($OptionalClinicalStateRuntime){$arguments+=' -bcmmOptionalFaultSeed 1'}
if($OptionalClinicalStateRestartRuntime){$arguments+=' -bcmmOptionalFaultRestart 1'}
if($OptionalClinicalStateRuntime -or $OptionalClinicalStateRestartRuntime){$arguments+=' -bcmmOptionalFaultField '+$OptionalClinicalStateField}
if($NativeSalineInactive){$arguments+=" -bcmmNativeSalineInactive 1"}
if($NativeSalineScenario -eq 'Deferred'){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf"'}
if($NativeSalineScenario){$arguments+=" -bcmmNativeSaline$NativeSalineScenario 1"}

if($NativeSalineWorldRuntime -or $NativeSalineWorldRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($NativeSalineWorldRuntime){$arguments+=' -bcmmNativeSalineWorld 1'}else{$arguments+=' -bcmmNativeSalineWorldRestart 1'}}
if($NativeConsumableCommitRuntime){$arguments+=' -bcmmNativeConsumableCommitRuntime 1'}
if($TransportFluidLineRuntime -or $TransportFluidLineRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($TransportFluidLineRuntime){$arguments+=' -bcmmTransportFluidLineRuntime 1'}else{$arguments+=' -bcmmTransportFluidLineRestartRuntime 1'}}
if ($AppliedVisualsRuntime) { $arguments += ' -bcmmAppliedVisualsRuntime 1' }
if ($UISmoke) { $arguments += ' -bcmmUIPreview 1 -bcmmUISmoke 1' }
if ($FluidUI) { $arguments += ' -bcmmUIPreview 1 -bcmmFluidUI 1' }
if ($FieldProceduresRuntime) { $arguments += ' -bcmmFieldProceduresRuntime 1' }
if ($AnimationRuntime) { $arguments += ' -bcmmAnimationRuntime 1' }
if ($FieldAnimation) { $arguments += ' -bcmmFieldAnimation ' + $FieldAnimation }
if ($PulseAudio) { $arguments = $arguments.Replace(' -noSound ', ' ').Replace(' -noFocus ', ' ').Replace('-posX -2000 -posY -2000', '-posX 0 -posY 0'); $arguments += ' -bcmmPulseAudio 1' }
if ($GripCoordinateRuntime) { $arguments += ' -bcmmGripCoordinateRuntime 1' }
if ($StretcherRuntime) { $arguments += " -bcmmStretcherRuntime 1" }
if ($TXARuntime) { $arguments += " -bcmmTXARuntime 1" }
if ($PulseOximeterRuntime) { $arguments += " -bcmmPulseOximeterRuntime 1" }
if ($EquipmentRuntime) { $arguments += ' -bcmmEquipmentRuntime 1' }
if ($MedicationRuntime) { $arguments += ' -bcmmMedicationRuntime 1' }
if ($BandageRuntime) { $arguments += ' -bcmmBandageRuntime 1' }
if ($InternalSurgeryRuntime) { $arguments += ' -bcmmInternalSurgeryRuntime 1' }
if ($AssessmentRuntime) { $arguments += ' -bcmmAssessmentRuntime 1' }
if ($SurgeryRuntime) { $arguments += ' -bcmmSurgeryRuntime 1' }
if ($SplintRuntime) { $arguments += ' -bcmmSplintRuntime 1' }
if ($PickupRuntime) { $arguments += ' -bcmmPickupRuntime 1' }
if ($DragRuntime) { $arguments += ' -bcmmDragRuntime 1' }
if($ComponentRecoveryRuntime -or $ComponentRecoveryRestartRuntime){$arguments+=' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave ';if($ComponentRecoveryRuntime){$arguments+=' -bcmmComponentRecoveryRuntime 1'}else{$arguments+=' -bcmmComponentRecoveryRestart 1'}}
if($LifeAdminRestartRuntime){$arguments+=' -bcmmLifeAdminRestart 1 -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave '}
if ($LifeRuntime) { $arguments += ' -bcmmLifeRuntime 1' }
if ($CargoRollbackRuntime) { $arguments += ' -bcmmCargoRollbackRuntime 1' }
if ($RollbackRuntime) { $arguments += ' -bcmmRollbackRuntime 1' }
if ($SendHomeRuntime) { $arguments += ' -bcmmSendHomeRuntime 1' }
if ($BurialRuntime) { $arguments += ' -bcmmBurialRuntime 1' }
if ($BurialOwner) { $arguments += ' -bcmmBurialOwner 1' }
if ($BaggingRuntime) { $arguments += ' -bcmmBaggingRuntime 1' }
if ($MedicalRuntime) { $arguments += ' -bcmmMedicalRuntime 1' }
if ($CargoRuntime) { $arguments += ' -bcmmCargoRuntime 1' }
if ($HomeFacility) { $arguments += ' -bcmmHomeFacility ' + $HomeFacility }
if ($PersistencePhase) { $arguments += ' -bcmmPersistencePhase ' + $PersistencePhase }
$arguments += ' -bcmmRunToken ' + $runToken
$arguments = $arguments.Replace(' -screenWidth 640 -screenHeight 480 ', (' -screenWidth '+$ScreenWidth+' -screenHeight '+$ScreenHeight+' '))
if ($Beta01StateRuntime) { $arguments += ' -bcmmBeta01State 1' }
if ($Beta01DragMatrixRuntime) { $arguments += ' -bcmmBeta01DragMatrix 1' }
if ($TerrainRuntime) { $arguments += ' -bcmmTerrainDrag 1' }
if ($Beta01HoldRuntime) { $arguments += ' -bcmmBeta01Hold 1' }
$runStarted = Get-Date
New-Item -ItemType Directory -Path $profile -Force | Out-Null
& (Join-Path $PSScriptRoot 'Test-ResourceGuids.ps1') -ProjectRoot $projectRoot -ReportPath (Join-Path $profile 'resource-guid-audit.json')
$validationSources=@(Get-ChildItem (Join-Path $projectRoot 'Scripts'),(Join-Path $projectRoot 'Prefabs'),(Join-Path $projectRoot 'PrefabsEditable'),(Join-Path $projectRoot 'Configs'),(Join-Path $projectRoot 'Language'),(Join-Path $projectRoot 'Tests\Runtime') -Recurse -File | Where-Object Extension -in '.c','.et','.conf','.meta','.st','.ent','.layer','.layout','.gproj') + @(Get-Item (Join-Path $projectRoot 'addon.gproj'),(Join-Path $projectRoot 'Tools\SettingsCatalog.json'),$PSCommandPath)
$validationManifest=[ordered]@{RunToken=$runToken;Project='BCM Medical';StartedUtc=$runStarted.ToUniversalTime().ToString('o');Sources=@($validationSources | Sort-Object FullName -Unique | ForEach-Object { [ordered]@{Path=$_.FullName.Substring($projectRoot.Length+1);SHA256=(Get-FileHash $_.FullName).Hash} })}
if($DiagnosticProject){$validationManifest.DiagnosticProject=$project;$validationManifest.DiagnosticSources=@(Get-ChildItem (Split-Path $project -Parent) -Recurse -File | Sort-Object FullName | ForEach-Object {[ordered]@{Path=$_.FullName.Substring($projectRoot.Length+1);SHA256=(Get-FileHash $_.FullName).Hash}})}
[IO.File]::WriteAllText((Join-Path $profile 'validation-manifest.json'),($validationManifest|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$process = Start-Process -FilePath $executable -WorkingDirectory (Split-Path $executable -Parent) -ArgumentList $arguments -WindowStyle Hidden -PassThru
$passed = $false
$console = $null
try {
 $deadline = (Get-Date).AddSeconds(60)
 if($Beta01StateRuntime){$deadline=(Get-Date).AddSeconds(180)}
 if($WeaponSlingRuntime){$deadline=(Get-Date).AddSeconds(100)}
 if($NativeSalineWorldRestartRuntime -or $RetrievedSalineRestartRuntime){$deadline=(Get-Date).AddSeconds(150)}
 if($FluidBurdenRuntime -or $FluidBurdenRestartRuntime){$deadline=(Get-Date).AddSeconds(180)}
 if ($OxygenMedicationRuntime) { $deadline=(Get-Date).AddSeconds(260) }
 if ($BurialRuntime) { $deadline=(Get-Date).AddSeconds(180) }
 if ($TXARuntime) { $deadline = (Get-Date).AddSeconds(120) }
 if ($FluidsRuntime) { $deadline = (Get-Date).AddSeconds(360) }
 if ($EquipmentRuntime) { $deadline = (Get-Date).AddSeconds(420) }
 if ($FieldProceduresRuntime -or $SurgeryRuntime -or $InternalSurgeryRuntime) { $deadline = (Get-Date).AddSeconds(240) }
 while ((Get-Date) -lt $deadline) {
  $console = Get-ChildItem -LiteralPath (Join-Path $profile 'logs') -Recurse -Filter 'console.log' -ErrorAction SilentlyContinue | Where-Object LastWriteTime -GE $runStarted | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($console) {
   if (Select-String -LiteralPath $console.FullName -Pattern 'Script validation failed|Can.t compile.*script|BCMM_CHECK FAILED|Virtual Machine Exception|Could not initialize platform services|ImageWidget ProcedureFill: Position/Size' -Quiet) { break }
   if (Select-String -LiteralPath $console.FullName -Pattern $success -Quiet) { $passed = $true; break }
  }
  if ($process.HasExited) { break }
  Start-Sleep -Milliseconds 250
 }
} finally {
 if (!$process.HasExited) {
  Stop-Process -Id $process.Id
  if (!$process.WaitForExit(5000)) { throw 'Owned native validator did not finish stopping.' }
 }
}
if ($console) {
 Select-String -LiteralPath $console.FullName -Pattern 'BCMM_CHECK|validation successful|SCRIPT.*\(E\)|addons|Addon' | Select-Object -Last 20 | ForEach-Object { $_.Line }
 Write-Output ('Log: ' + $console.FullName)
}
if ($TerrainRuntime -and (!$console -or !(Select-String -LiteralPath $console.FullName -Pattern 'BCMM_TERRAIN_DRAG_COMPLETE cases=6' -Quiet))) { $passed=$false }
if (!$passed) { throw 'Validation did not pass. Inspect the saved console log.' }
