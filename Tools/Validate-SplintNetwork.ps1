param([string]$ServerDirectory = '', [switch]$Fast, [switch]$Medication, [switch]$Equipment, [switch]$EquipmentOnly, [switch]$PulseOximeter, [switch]$PulseOnly, [switch]$PulseActiveJoin)
$ErrorActionPreference='Stop'
if($PulseActiveJoin){$PulseOnly=$true}
if($PulseOnly){$PulseOximeter=$true}
if($PulseOximeter){$Fast=$true}
if($EquipmentOnly){$Equipment=$true; $Fast=$true}
if($Equipment){$Medication=$true}
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\splint-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$project=Join-Path $root 'Tests\Runtime\addon.gproj'
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$token=[Guid]::NewGuid().ToString('N')
$common='-gproj "'+$project+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
if($EquipmentOnly){$common+=' -bcmmEquipmentOnlyNetwork 1 '}
$port=Get-Random -Minimum 24000 -Maximum 30000
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1; if($file){return Get-Content -LiteralPath $file.FullName -Raw}; return ''}
function Start-Observer([string]$role){$arguments=$common+'-client 127.0.0.1 -port '+$port+' -bcmmCargoObserver 1 -bcmmSplintObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $serverArgs=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmSplintRuntime 1 -bcmmSplintNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 if($PulseActiveJoin){$serverArgs += ' -bcmmPulseActiveJoin 1'}
 if($PulseOnly){$serverArgs += ' -bcmmPulseOnlyNetwork 1'}
 if($PulseOximeter){$serverArgs += ' -bcmmPulseOximeterNetwork 1'}
 if($EquipmentOnly){$serverArgs += ' -bcmmEquipmentOnlyNetwork 1'}
 if($Equipment){$serverArgs += ' -bcmmEquipmentNetwork 1'}
 if($Medication){$serverArgs += ' -bcmmMedicationNetwork 1'}
 if($Fast){$serverArgs += ' -bcmmSkipAISplint 1'}
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $serverArgs -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(180); if($Medication){$limit=(Get-Date).AddSeconds(420)}
 $clients=$false; $late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$clients -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$clients=$true}
  $joinPhase=1; if($PulseActiveJoin){$joinPhase=9}
  if($clients -and !$late -and $server -match ('BCMM_SPLINT_NET_PHASE '+$joinPhase+'(?:\s|$)')){Start-Observer 'late-join';$late=$true}
  $all=@('observer-a','observer-b','late-join') | ForEach-Object {Read-Log $_}
  if(($server+($all -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ImageWidget ProcedureFill: Position/Size|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Splint network fixture failed; inspect logs.'}
  $finished=$server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)
  $observed=$true
  $lastPhase=3; if($Medication){$lastPhase=5}; if($Equipment){$lastPhase=7}
  $phases=1..$lastPhase; if($EquipmentOnly){$phases=@(1,6,7)}
  if($PulseOximeter){$phases=@(1,2,3,8,9,10)}
  if($PulseOnly){$phases=@(1,8,9,10)}
  for($observerIndex=0;$observerIndex -lt $all.Count;$observerIndex++){$log=$all[$observerIndex];$observerPhases=$phases;if($PulseActiveJoin -and $observerIndex -eq 2){$observerPhases=@(9,10)};if($PulseOximeter -and ($log -notmatch "BCMM_PULSE_PANEL_LIVE_PASSED" -or $log -notmatch "BCMM_PULSE_PANEL_STOPPED_PASSED")){$observed=$false};if(!$EquipmentOnly -and !$PulseOnly -and ($log -notmatch "BCMM_MEDIC_ROLE_OBSERVED phase=1 medic=1" -or $log -notmatch "BCMM_MEDIC_ROLE_OBSERVED phase=3 medic=0")){$observed=$false};if($Equipment -and $log -notmatch 'BCMM_EQUIPMENT_PANEL_PRIVATE_CHECKS_PASSED'){$observed=$false};if($Medication -and !$EquipmentOnly -and $log -notmatch 'BCMM_MEDICATION_PANEL_PRIVATE_CHECKS_PASSED'){$observed=$false};if(!$EquipmentOnly -and !$PulseOnly -and ($log -notmatch 'BCMM_MEDICAL_PANEL_OPENED' -or $log -notmatch 'BCMM_MEDICAL_BINDING_REBIND_HOLD_RESET_PASSED')){$observed=$false};foreach($phase in $observerPhases){if($log -notmatch ('BCMM_SPLINT_NET_OBSERVED phase='+$phase+' ')){$observed=$false}}}
  if(!$EquipmentOnly -and !$PulseOnly -and (($all -join "`n") -notmatch 'BCMM_SPLINT_CANCEL_RETAINED_KIT' -or $server -notmatch 'BCMM_SPLINT_SERVER_CANCEL_RETAINED_KIT')){$observed=$false}
  if($finished -and $observed){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){
 Write-Output $role
 (Read-Log $role) -split "`n" | Where-Object {$_ -match 'BCMM_SPLINT_NET_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED|SCRIPT.*\(E\)|PLATFORM.*\(E\)'} | Select-Object -Last 10
}
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Splint replication checks did not all complete successfully.'}
