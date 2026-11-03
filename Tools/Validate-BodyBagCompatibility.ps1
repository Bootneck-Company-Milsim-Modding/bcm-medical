param(
 [Parameter(Mandatory)][string]$ServerDirectory,
 [string[]]$Names = @(),
 [string]$ProductionDirectory = '',
 [string[]]$AdditionalAddonDirectories = @(),
 [switch]$PilotedAircraft,
 [ValidateRange(60,600)][int]$TimeoutSeconds = 240
)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$fixtures=@(Get-Content (Join-Path $root 'Tests\Vehicle\CompatibilityMatrix.json') -Raw | ConvertFrom-Json)
if($Names.Count){$fixtures=@($fixtures|Where-Object Name -in $Names)}
if($PilotedAircraft){$fixtures=@($fixtures|Where-Object Aircraft)}
if(!$fixtures.Count){throw 'No matching Medical compatibility fixtures'}
$run=Join-Path $root ('.checks\bodybag-compatibility-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $run -Force | Out-Null
$records=[Collections.Generic.List[object]]::new()
Write-Output "Medical compatibility records=$run"
foreach($fixture in $fixtures){
 $record=[ordered]@{Name=$fixture.Name;Prefab=$fixture.Prefab;Pilot=[bool]$PilotedAircraft;ExtraAddons=@($fixture.ExtraAddons);Status='running';RunDirectory='';Failure='';Checks=0;CarrierOwnerConfirmed=$false;PilotOwnerConfirmed=$false}
 try{
  $parameters=@{ProductionDirectory=$ProductionDirectory;AdditionalAddonDirectories=$AdditionalAddonDirectories;ServerDirectory=$ServerDirectory;VehiclePrefab=$fixture.Prefab;ExtraAddons=@($fixture.ExtraAddons);Pilot=[bool]$PilotedAircraft;AllowKnownSupportStationDiagnostic=[bool]$fixture.AllowKnownSupportStationDiagnostic;TimeoutSeconds=$TimeoutSeconds}
  & (Join-Path $PSScriptRoot 'Validate-BodyBagVehicles.ps1') @parameters | Tee-Object -FilePath (Join-Path $run (($fixture.Name -replace '[^a-zA-Z0-9]','-')+'.log')) | ForEach-Object {
   if($_ -match '^Medical vehicle server PID=\d+ Logs=(.+)$'){$record.RunDirectory=$Matches[1]}
   if($_ -match 'BCMM_VEHICLE_COMPLETE checks=(\d+) failures=0'){$record.Checks=[int]$Matches[1]}
   if($_ -match '^Medical carrier ownership confirmed=True$'){$record.CarrierOwnerConfirmed=$true}
   if($_ -match '^Medical native pilot ownership confirmed=True$'){$record.PilotOwnerConfirmed=$true}
   if($_ -match 'PID=|COMPLETE|pass=0|Medical client|confirmed='){Write-Output $_}
  }
  $record.Status='passed'
  if($fixture.AllowKnownSupportStationDiagnostic){$record.Status='passed-with-upstream-diagnostic-policy'}
 }catch{$record.Status='failed';$record.Failure=$_.Exception.Message;if($record.Failure -like 'Missing installed vehicle dependency:*'){$record.Status='blocked-missing-dependency'};Write-Output "Medical fixture failed: $($fixture.Name): $($_.Exception.Message)"}
 $records.Add([pscustomobject]$record)
 [IO.File]::WriteAllText((Join-Path $run 'results.json'),($records|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
 Write-Output "Medical fixture $($fixture.Name) status=$($record.Status) checks=$($record.Checks)"
}
if(@($records|Where-Object Status -notlike 'passed*').Count){throw "Medical compatibility failures or blocked dependencies recorded in $run"}
