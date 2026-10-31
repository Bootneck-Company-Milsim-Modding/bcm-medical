param(
 [Parameter(Mandatory)][string]$ServerDirectory,
 [string]$VehiclePrefab = '{2D74C39A650A3030}Prefabs/Vehicles/Wheeled/M923A1/M923A1_engineer.et',
 [string[]]$ExtraAddons = @(),
 [switch]$Pilot,
 [switch]$ExternalMove,
 [switch]$SessionSafety,
 [switch]$Disconnect,
 [switch]$VehicleLoss,
 [switch]$ReturnHome,
 [switch]$SearchSessions,
 [switch]$SearchDisconnect,
 [switch]$AllowKnownSupportStationDiagnostic,
 [ValidateRange(60,600)][int]$TimeoutSeconds = 240
)
$ErrorActionPreference = 'Stop'
if($Disconnect -and !$SessionSafety){throw 'Disconnect requires the connected-player SessionSafety fixture'}
if($SearchSessions -and !$ReturnHome){throw 'SearchSessions requires ReturnHome'}
if($SearchDisconnect -and !$SearchSessions){throw 'SearchDisconnect requires SearchSessions'}
if((@($ExternalMove,$Pilot,$SessionSafety,$VehicleLoss,$ReturnHome) | Where-Object {$_}).Count -gt 1){throw 'ExternalMove, Pilot and SessionSafety are separate fixtures'}
$root = Split-Path $PSScriptRoot -Parent
$run = Join-Path $root ('.checks\bodybag-vehicle-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$addon = Join-Path $run 'addon'
$scripts = Join-Path $addon 'Scripts\Game'
New-Item -ItemType Directory -Path $scripts -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_VehicleLifecycleChecks.c'),(Join-Path $root 'Tests\Vehicle\BCMM_PilotInputs.c') -Destination $scripts
if($ExternalMove){Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_ExternalCargoChecks.c') -Destination $scripts}
if($SessionSafety){Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_BaggingSessionChecks.c') -Destination $scripts}
if($VehicleLoss){Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_VehicleLossChecks.c') -Destination $scripts}
if($ReturnHome){Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_RemainsNetworkChecks.c') -Destination (Join-Path $scripts 'ZZ_BCMM_RemainsNetworkChecks.c')}
if($SearchSessions){Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_SearchNetworkChecks.c') -Destination (Join-Path $scripts 'ZZ_BCMM_SearchNetworkChecks.c')}
$guid = 'B6C4ED1CA1260003'
$dependencies = (@('6AB4B3F3FD215919') + $ExtraAddons | Select-Object -Unique | ForEach-Object { '"'+$_+'"' }) -join ' '
[IO.File]::WriteAllText((Join-Path $addon 'addon.gproj'), ('GameProject { ID "BCMMVehicleChecks" GUID "'+$guid+'" TITLE "BCM Medical vehicle checks" Dependencies { '+$dependencies+' } }'), [Text.UTF8Encoding]::new($false))
$game = 'C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$workshop = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'My Games\ArmaReforger\addons'
$dirs = $addon+','+$root+','+(Join-Path $game 'addons')
if ($ExtraAddons.Count) { $dirs += ','+$workshop }
$common = '-gproj "'+$addon+'\addon.gproj" -addons '+$guid+' -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noThrow -maxFPS 30 -bcmmVehiclePrefab "'+$VehiclePrefab+'"'
if ($Pilot) { $common += ' -bcmmPilotedVehicle 1' }
if ($Disconnect) { $common += ' -bcmmSessionDisconnect 1' }
if ($SearchSessions) { $common += ' -bcmmSearchSessions 1' }
if ($SearchDisconnect) { $common += ' -bcmmSearchDisconnect 1' }
if ($ExternalMove -or $VehicleLoss) { $common += ' -bcmmVehicleAuthority 1' }
$files = @(Get-ChildItem (Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'Language') -Recurse -File | Where-Object Extension -in '.c','.et','.conf','.meta','.st') + @(Get-Item (Join-Path $root 'addon.gproj')) + @(Get-ChildItem $addon -Recurse -File)
$manifest = [ordered]@{Project='BCM Medical';ProjectGuid='6AB4B3F3FD215919';Vehicle=$VehiclePrefab;Pilot=[bool]$Pilot;ExternalMove=[bool]$ExternalMove;SessionSafety=[bool]$SessionSafety;Disconnect=[bool]$Disconnect;VehicleLoss=[bool]$VehicleLoss;ReturnHome=[bool]$ReturnHome;ExtraAddons=@($ExtraAddons);RunnerHash=(Get-FileHash $PSCommandPath).Hash;Sources=@($files | Sort-Object FullName | ForEach-Object { [ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash $_.FullName).Hash} })}
$manifest.SearchSessions=[bool]$SearchSessions
$manifest.SearchDisconnect=[bool]$SearchDisconnect
[IO.File]::WriteAllText((Join-Path $run 'validation-manifest.json'),($manifest|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$owned = [Collections.Generic.List[Diagnostics.Process]]::new()
function Read-VehicleLog([string]$role) { $file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue | Select-Object -First 1; if($file){return Get-Content $file.FullName -Raw}; return '' }
function Start-VehicleClient([string]$role) {
 $arguments=$common+' -client 127.0.0.1 -port 2001 -profile "'+(Join-Path $run $role)+'" -noSound -noFocus -forceUpdate -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000'
 $process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
 $owned.Add($process); Write-Output "Medical vehicle client role=$role PID=$($process.Id)"
}
$passed=$false; $owner=$false; $pilotOwner=$false; $clients=$false; $late=$false; $lastProgress=''
$disconnectTriggered=$false
$searchDisconnectTriggered=$false
function Test-NativeAmbulanceMeshRounding([string]$line) {
 # Native replicated M997 physics introduces a sub-millimetre local mesh offset.
 # Bound this specific visual diagnostic; all other script errors remain fatal.
 if(!$ReturnHome -or $line -notmatch "SCRIPT\s+\(E\): 'SCR_SupportStationAreaMeshComponent'.*linked support station \(HEAL\).*local position of the area mesh is: '<([^>]+)>'.*support station offset is '<0, 0, -0.5>'"){return $false}
 $xyz=$Matches[1].Split(',')
 if($xyz.Count -ne 3){return $false}
 $culture=[Globalization.CultureInfo]::InvariantCulture
 $x=[double]::Parse($xyz[0],$culture);$y=[double]::Parse($xyz[1],$culture);$z=[double]::Parse($xyz[2],$culture)
 return [Math]::Abs($x) -lt 0.001 -and [Math]::Abs($y) -lt 0.001 -and [Math]::Abs($z+0.5) -lt 0.001
}
try {
 $arguments=$common+' -server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -profile "'+(Join-Path $run 'server')+'"'
 $server=Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
 $owned.Add($server); Write-Output "Medical vehicle server PID=$($server.Id) Logs=$run"
 $deadline=(Get-Date).AddSeconds($TimeoutSeconds)
 while((Get-Date) -lt $deadline) {
  $serverText=Read-VehicleLog 'server'
  if(!$ExternalMove -and !$VehicleLoss -and !$clients -and $serverText -match 'Starting RPL server'){Start-VehicleClient 'owner';$clients=$true}
  if(!$ExternalMove -and !$VehicleLoss -and !$SessionSafety -and !$late -and $serverText -match 'BCMM_VEHICLE_PHASE 1'){Start-VehicleClient 'late-join';$late=$true}
  if($Disconnect -and !$disconnectTriggered -and $serverText -match 'BCMM_BAGGING_SESSION waitingDisconnect'){
   if($owned.Count -lt 2 -or $owned[1].HasExited){throw 'Owning test client exited before the requested disconnect'}
   Stop-Process -Id $owned[1].Id
   $disconnectTriggered=$true
   Write-Output "Medical session disconnected exact owning test client PID=$($owned[1].Id)"
  }
  if($SearchDisconnect -and !$searchDisconnectTriggered -and $serverText -match 'BCMM_SEARCH waitingDisconnect'){
   if($owned.Count -lt 3 -or $owned[2].HasExited){throw 'Exact observer search client exited before requested disconnect'}
   Stop-Process -Id $owned[2].Id
   $searchDisconnectTriggered=$true
   Write-Output "Medical search disconnected exact owned observer client PID=$($owned[2].Id)"
  }
  $logs=@($serverText,(Read-VehicleLog 'owner'),(Read-VehicleLog 'late-join'))
  $bad=@($logs -split "`n" | Where-Object { $_ -match 'SCRIPT\s+\(E\)|RPL\s+\(E\)|Virtual Machine Exception|Can.t compile.*script|RplCreationError|REPLICATION_SYSTEM_FAILURE|Error when detaching entity|Application crashed|ENGINE\s+\(F\)' -and !($AllowKnownSupportStationDiagnostic -and $_ -match "SCRIPT\s+\(E\): 'SCR_BaseSupportStationComponent' 'EOnInit':.*Support Station is set to use supplies as it has a Resource Component but it has no consumer nor generator set up\s*$") })
  $bad=@($bad | Where-Object { !(Test-NativeAmbulanceMeshRounding $_) })
  if($bad.Count){$bad|Write-Output;break}
  $progress=@($serverText -split "`n" | Where-Object { $_ -match 'BCMM_VEHICLE_' }) -join "`n"
  if($progress -ne $lastProgress){Write-Output $progress.Substring($lastProgress.Length);$lastProgress=$progress}
  if($serverText -match 'BCMM_VEHICLE_COMPLETE checks=\d+ failures=0'){
   $passed=$true
   if($ExternalMove -or $VehicleLoss){break}
   if($SessionSafety){if($Disconnect){$passed=$disconnectTriggered -and $serverText -match 'BCMM_BAGGING_SESSION disconnected=1'}else{$passed=$serverText -match 'BCMM_BAGGING_SESSION originalPlayer=\d+ switched=1'};break}
   foreach($role in @('owner','late-join')) {
    $client=Read-VehicleLog $role
    $requiredPhases=1..3;if($SearchDisconnect -and $role -eq 'late-join'){$requiredPhases=1..2}
    foreach($phase in $requiredPhases){$observed=$client -match "BCMM_VEHICLE_CLIENT PASS phase=$phase";Write-Output "Medical client role=$role phase=$phase pass=$observed";if(!$observed){$passed=$false}}
    if($client -match 'BCMM_VEHICLE_CLIENT OWNER_CONFIRMED'){$owner=$true}
    if($client -match 'BCMM_PILOT_CLIENT native pilot seated=1 startEngine=1' -and $client -match 'BCMM_PILOT_CLIENT controls .*engine=1 owner=1'){$pilotOwner=$true}
   }
   if($ReturnHome){if((Read-VehicleLog 'owner') -notmatch 'BCMM_REMAINS_CLIENT OWNER_PASS' -or (!$SearchDisconnect -and (Read-VehicleLog 'late-join') -notmatch 'BCMM_REMAINS_CLIENT PRIVATE_PASS')){$passed=$false}}
   if($SearchSessions -and (Read-VehicleLog 'owner') -notmatch 'BCMM_SEARCH_CLIENT STALE_REPLY_PASS'){$passed=$false}
   if($SearchDisconnect -and (!$searchDisconnectTriggered -or $serverText -notmatch 'BCMM_SEARCH disconnected=1')){$passed=$false}
   Write-Output "Medical carrier ownership confirmed=$owner"
   if(!$owner){$passed=$false}
   if($Pilot){Write-Output "Medical native pilot ownership confirmed=$pilotOwner";if(!$pilotOwner){$passed=$false}}
   break
  }
  if($serverText -match 'BCMM_VEHICLE_COMPLETE' -or $server.HasExited){break}
  Start-Sleep -Milliseconds 500
 }
} finally { foreach($process in $owned){if(!$process.HasExited){Stop-Process -Id $process.Id}} }
$sourceUnchanged=(Get-FileHash $PSCommandPath).Hash -eq $manifest.RunnerHash
foreach($entry in $manifest.Sources){$path=Join-Path $root $entry.Path;if(!(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path).Hash -ne $entry.SHA256){$sourceUnchanged=$false}}
if(!$sourceUnchanged){$passed=$false}
[ordered]@{Passed=$passed;SourceUnchanged=$sourceUnchanged;SearchSessions=[bool]$SearchSessions;SearchDisconnect=[bool]$SearchDisconnect;CarrierOwnerConfirmed=$owner;PilotOwnerConfirmed=$pilotOwner;RunDirectory=$run} | ConvertTo-Json | Set-Content (Join-Path $run 'result.json')
if(!$passed){throw "BCM Medical vehicle checks failed; inspect $run"}

