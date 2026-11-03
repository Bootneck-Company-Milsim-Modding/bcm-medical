# Reproducible acceptance of the same unchanged standalone clinical build.
param([switch]$FullProcedures,[string]$ServerDirectory='', [string]$StartAt='', [string]$LegacySourceProfile='')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path $PSScriptRoot -Parent
$reportDirectory=Join-Path $projectRoot ('.checks/clinical-acceptance-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $reportDirectory)
function Get-SourceSnapshot {
 @(Get-ChildItem -LiteralPath @((Join-Path $projectRoot 'Scripts'),(Join-Path $projectRoot 'Tests/Runtime'),(Join-Path $projectRoot 'Prefabs'),(Join-Path $projectRoot 'PrefabsEditable'),(Join-Path $projectRoot 'Configs'),(Join-Path $projectRoot 'UI'),(Join-Path $projectRoot 'Language'),(Join-Path $projectRoot 'Source/Animations'),(Join-Path $projectRoot 'Tools'),(Join-Path $projectRoot 'addon.gproj')) -Recurse -File |
  Where-Object { $_.Extension -in @('.c','.et','.meta','.conf','.layout','.anm','.agr','.agf','.asi','.aw','.txa','.ps1','.py','.json','.st','.gproj') } |
  Sort-Object FullName | ForEach-Object { $_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash })
}
$before=Get-SourceSnapshot
$before | Set-Content -LiteralPath (Join-Path $reportDirectory 'source-before.txt')
$report=[ordered]@{started=(Get-Date -Format o);project=$projectRoot;fullProcedures=[bool]$FullProcedures;startAt=$StartAt;skipped=@();status='running';steps=@();sourceUnchanged=$false}
$script:stepEnabled = !$StartAt
function Save-Report {
 $pending=Join-Path $reportDirectory 'report.pending.json'
 $report | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $pending
 Move-Item -LiteralPath $pending -Destination (Join-Path $reportDirectory 'report.json') -Force
}
function Invoke-ClinicalCheck([string]$label,[string]$scriptName,[hashtable]$arguments) {
 if(!$script:stepEnabled){
  if($label -eq $StartAt){$script:stepEnabled=$true}
  else{$report.skipped+=,$label;Save-Report;return}
 }
 $step=[ordered]@{name=$label;started=(Get-Date -Format o);status='running';log=$label+'.txt'}
 $report.steps+=,$step;Save-Report
 try {
  $output=& (Join-Path $PSScriptRoot $scriptName) @arguments
  $output | Set-Content -LiteralPath (Join-Path $reportDirectory $step.log)
  $step.status='passed';$step.completed=Get-Date -Format o;Save-Report
  Write-Output ($label+': passed')
 } catch {
  $step.status='failed';$step.error=$_.Exception.Message;$report.status='failed';Save-Report
  throw
 }
}
try {
 Invoke-ClinicalCheck 'resources' 'Validate.ps1' @{TestResources=$true}
 Invoke-ClinicalCheck 'native-faction-arsenals' 'Validate.ps1' @{ArsenalRuntime=$true}
 Invoke-ClinicalCheck 'arsenal-owner-network' 'Validate-ArsenalNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'clinical-safety' 'Validate.ps1' @{ClinicalSafetyRuntime=$true}
 Invoke-ClinicalCheck 'inventory-sling-retention' 'Validate.ps1' @{WeaponSlingRuntime=$true}
 Invoke-ClinicalCheck 'living-casualty-transport' 'Validate.ps1' @{LivingTransportRuntime=$true}
 Invoke-ClinicalCheck 'living-transport-owner-and-late-join' 'Validate-LivingTransportNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-consumable-commit' 'Validate.ps1' @{NativeConsumableCommitRuntime=$true;ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-access-loss' 'Validate.ps1' @{NativeSalineScenario='Access';ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-source-movement' 'Validate.ps1' @{NativeSalineScenario='SourceMove';ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-full-heal' 'Validate.ps1' @{NativeSalineScenario='FullHeal';ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-world-expiry' 'Validate-NativeStockSaline.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-inactive-world-expiry' 'Validate-NativeStockSaline.ps1' @{Inactive=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-deferred-reference' 'Validate.ps1' @{NativeSalineScenario='Deferred';ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-same-source-full-heal' 'Validate.ps1' @{NativeSalineScenario='SameSourceHeal';ServerRuntime=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-retrieved-expiry' 'Validate-RetrievedStockSaline.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-saline-retrieved-full-heal' 'Validate-RetrievedStockSaline.ps1' @{FullHeal=$true;ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'invalid-saved-native-saline-object' 'Validate-OptionalClinicalState.ps1' @{Field='bcmmNativeSaline';ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'invalid-saved-fluid-line-object' 'Validate-OptionalClinicalState.ps1' @{Field='bcmmFluidLine';ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'invalid-saved-fluid-burden-object' 'Validate-OptionalClinicalState.ps1' @{Field='bcmmFluidBurden';ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-stock-consumable-owner-network' 'Validate-StockCommitNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'oxygen-removal-without-supply' 'Validate.ps1' @{OxygenRemovalRuntime=$true}
 Invoke-ClinicalCheck 'oxygen-removal-owner-network' 'Validate-OxygenRemovalNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'oxygen-medication-network' 'Validate-OxygenMedicationNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'typed-fluid-network' 'Validate-FluidNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'independent-fluid-lines' 'Validate.ps1' @{FluidLineRuntime=$true}
 Invoke-ClinicalCheck 'independent-fluid-line-boundaries' 'Validate.ps1' @{FluidLineBoundaryRuntime=$true}
 Invoke-ClinicalCheck 'independent-fluid-line-owner-network' 'Validate-FluidLineNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'independent-fluid-line-restart' 'Validate-FluidLinePersistence.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'transport-fluid-line-restart' 'Validate-TransportFluidLine.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'unavailable-fluid-source-restart' 'Validate-MissingFluidLine.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'fluid-burden-active-world-restart' 'Validate-FluidBurden.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'fluid-burden-owner-network' 'Validate-FluidBurdenNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'pre-burden-world-compatibility' 'Validate-FluidBurdenLegacyWorld.ps1' @{ServerDirectory=$ServerDirectory;SourceProfile=$LegacySourceProfile}
 Invoke-ClinicalCheck 'applied-equipment-network' 'Validate-AppliedEquipmentNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'native-bleeding' 'Validate.ps1' @{MedicalRuntime=$true}
 Invoke-ClinicalCheck 'limited-lives-authority' 'Validate.ps1' @{LifeRuntime=$true}
 Invoke-ClinicalCheck 'gm-allowance-owner-and-restart' 'Validate-LifeAdminNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'gm-stationary-catalogue-and-restart' 'Validate-LifeAdminNetwork.ps1' @{ServerDirectory=$ServerDirectory;Catalogue=$true}
 Invoke-ClinicalCheck 'component-recovery-and-restart' 'Validate-ComponentRecovery.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'inventory-and-lives-world-restart' 'Validate-FinitePersistence.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'saved-world-owner-network' 'Validate-WorldPersistenceNetwork.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'unavailable-physical-world-restart' 'Validate-MissingPhysicalRestart.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'server-restart' 'Validate-Persistence.ps1' @{ServerDirectory=$ServerDirectory}
 Invoke-ClinicalCheck 'medical-ui' 'Validate.ps1' @{UISmoke=$true;ScreenWidth=1280;ScreenHeight=720}
 if($FullProcedures) {
  Invoke-ClinicalCheck 'full-oxygen-medication' 'Validate.ps1' @{OxygenMedicationRuntime=$true}
  Invoke-ClinicalCheck 'full-typed-fluids' 'Validate.ps1' @{FluidsRuntime=$true}
  Invoke-ClinicalCheck 'full-field-procedures' 'Validate.ps1' @{FieldProceduresRuntime=$true}
  Invoke-ClinicalCheck 'full-equipment' 'Validate.ps1' @{EquipmentRuntime=$true}
  Invoke-ClinicalCheck 'native-medication' 'Validate.ps1' @{MedicationRuntime=$true}
  Invoke-ClinicalCheck 'native-txa' 'Validate.ps1' @{TXARuntime=$true}
 }
 if(!$script:stepEnabled){throw ('Unknown acceptance step: '+$StartAt)}
 $after=Get-SourceSnapshot;$after | Set-Content -LiteralPath (Join-Path $reportDirectory 'source-after.txt')
 if(@(Compare-Object -ReferenceObject $before -DifferenceObject $after).Count) { throw 'Source changed during clinical acceptance; results cannot be combined as one build.' }
 $report.sourceUnchanged=$true;$report.status='passed';$report.completed=Get-Date -Format o;Save-Report
 Write-Output ('Clinical acceptance report: '+(Join-Path $reportDirectory 'report.json'))
} catch { $report.status='failed';$report.error=$_.Exception.Message;Save-Report;throw }
