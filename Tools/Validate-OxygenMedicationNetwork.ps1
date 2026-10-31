param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\oxygen-medication-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 -bcmmEquipmentNetwork 1 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -port '+$port+' -bcmmOxygenObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmOxygenNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(300);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_OXYGEN_PHASE 2(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join')|ForEach-Object{Read-Log $_}
  if($processes[0].HasExited){throw 'Diagnostic server exited before acceptance; inspect its console log.'}
  if(($server+($logs -join "`n")) -match 'Unable to start replication|Unable to initialize the game|BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Oxygen/medication network check failed; inspect logs.'}
  $observed=$late
  if(($logs -join "`n") -notmatch "BCMM_SELF_TABLET_CANCEL_SENT"){$observed=$false}
  if(($logs -join "`n") -notmatch "BCMM_OXYGEN_OWNER_SENT"){$observed=$false}
  foreach($observerIndex in 0..2){$log=$logs[$observerIndex];$phases=1..13;if($observerIndex -eq 2){$phases=2..13};foreach($phase in $phases){if($log -notmatch ('BCMM_OXYGEN_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  foreach($log in $logs){if($log -notmatch 'BCMM_SELF_TABLET_NETWORK_ACTIVE' -or $log -notmatch 'BCMM_TABLET_NETWORK_ACTIVE' -or $log -notmatch 'BCMM_ARSENAL_BOXES_OBSERVED phase=3'){$observed=$false};foreach($faction in @('US','USSR','FIA')){if($log -notmatch ('BCMM_ARSENAL_CATALOG_OK faction='+$faction+' count=29')){$observed=$false}}}
  foreach($requestPhase in @(2,4,5,6,8,11,12)){if(($logs -join "`n") -notmatch ('BCMM_OXYGEN_OWNER_SENT '+$requestPhase+'(?:\s|$)')){$observed=$false}}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n"|Where-Object{$_ -match 'BCMM_OXYGEN_OBSERVED|BCMM_OXYGEN_OWNER|BCMM_TABLET_NETWORK_ACTIVE|BCMM_ARSENAL_CATALOG_OK|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'}|Select-Object -Last 7}
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Oxygen/medication owner/proxy/late-join checks did not all pass.'}
