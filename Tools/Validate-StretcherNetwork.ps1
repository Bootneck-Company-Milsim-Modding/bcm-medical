param([string]$ServerDirectory='',[switch]$Carry,[switch]$Safety,[switch]$PatientOwner,[switch]$Presentation,[switch]$AppliedEquipment,[switch]$Review)
$ErrorActionPreference='Stop'
if($Safety -or $PatientOwner -or $Presentation -or $AppliedEquipment){$Carry=$true}
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\stretcher-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
if($Presentation){$common+=' -bcmmGripPresentationCheck 1 '}
if($AppliedEquipment){$common+=' -bcmmAppliedEquipmentCheck 1 '}
if($Review){$common+=' -bcmmStretcherReview 1 '}
if($Safety){$common+=' -bcmmStretcherSettingsMirror 1 '}
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -port '+$port+' -bcmmStretcherObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmStretcherRuntime 1 -bcmmStretcherNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 if($Carry){$argsText+=' -bcmmStretcherCarryNetwork 1'}
 if($Safety){$argsText+=' -bcmmStretcherCarrySafety 1'}
 if($PatientOwner){$argsText+=' -bcmmStretcherPatientOwner 1'}
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(210);if($Review){$limit=(Get-Date).AddSeconds(340)};$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  $latePhase=2;if($Carry){$latePhase=6}
  if(!$late -and $server -match ('BCMM_STRETCHER_PHASE '+$latePhase+'(?:\s|$)')){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join')|ForEach-Object{Read-Log $_}
  if(($server+($logs -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError'){throw 'Stretcher network check failed; inspect logs.'}
  $observed=$late
  if($Safety){foreach($settingsLog in $logs){if($settingsLog -notmatch "BCMM_CARRY_SETTINGS_MIRROR_PASSED"){$observed=$false}}}
  if($PatientOwner -and ($logs -join "`n") -notmatch "BCMM_CARRY_PATIENT_OWNER_ANCHORED"){$observed=$false}
  $phases=@(2,4);if($Carry){$phases=@(6,8,10)}
  if($Safety -and ($logs -join "`n") -notmatch "BCMM_CARRY_STALE_RELEASE_SENT"){$observed=$false}
  if($Safety){foreach($log in $logs){if($log -notmatch 'BCMM_CARRY_GRIP_SENT player=2' -and ($log -notmatch 'BCMM_STRETCHER_OBSERVED 18' -or $log -notmatch 'BCMM_STRETCHER_OBSERVED 20')){$observed=$false}}}
  foreach($log in $logs){foreach($phase in $phases){if($log -notmatch ('BCMM_STRETCHER_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  if($Carry){if(($logs -join "`n") -notmatch 'BCMM_CARRY_GRIP_SENT player=1' -or ($logs -join "`n") -notmatch 'BCMM_CARRY_GRIP_SENT player=2' -or ($logs -join "`n") -notmatch 'BCMM_CARRY_RELEASE_SENT'){$observed=$false}}
  elseif(($logs -join "`n") -notmatch 'BCMM_STRETCHER_OWNER_PLACED_REQUEST' -or ($logs -join "`n") -notmatch 'BCMM_STRETCHER_OWNER_REMOVE_REQUEST'){$observed=$false}
  if($Presentation){foreach($poseLog in $logs){if($poseLog -notmatch 'BCMM_GRIP_OBSERVED 6' -or $poseLog -notmatch 'BCMM_GRIP_OBSERVED 8' -or $poseLog -notmatch 'BCMM_GRIP_RELEASE_OBSERVED 10'){$observed=$false}}}
  if($AppliedEquipment){foreach($visualLog in $logs){foreach($visualPhase in @(6,8,10)){if($visualLog -notmatch ('BCMM_EQUIPMENT_VISUAL_OBSERVED '+$visualPhase+'(?:\s|$)')){$observed=$false}}}}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n"|Where-Object{$_ -match 'BCMM_STRETCHER_|BCMM_CARRY_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'}|Select-Object -Last 8}
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Stretcher owner/proxy/late-join checks did not all pass.'}

