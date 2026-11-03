param([string]$ServerDirectory = '',[string]$ProductionDirectory = '')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\drag-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$project=Join-Path $root 'Tests\Runtime\addon.gproj'
if(!$ProductionDirectory){$ProductionDirectory=$root}
$dirs=$ProductionDirectory+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$token=[Guid]::NewGuid().ToString('N')
$common='-gproj "'+$project+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$port=2001
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
$observers=@{}
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1; if($file){return Get-Content -LiteralPath $file.FullName -Raw}; return ''}
function Start-Observer([string]$role){$arguments=$common+'-client 127.0.0.1 -port '+$port+' -bcmmCargoObserver 1 -bcmmDragObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru;$processes.Add($process);$observers[$role]=$process}
$passed=$false
$failure=''
try{
 $serverArgs=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmDragRuntime 1 -bcmmDragNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $serverArgs -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(130)
 $clients=$false; $late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$clients -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$clients=$true}
  if($clients -and !$late -and $server -match 'BCMM_DRAG_NET_PHASE 1'){Start-Observer 'late-join';$late=$true}
  $all=@('observer-a','observer-b','late-join') | ForEach-Object {Read-Log $_}
  foreach($role in @('observer-a','observer-b','late-join')){
   if((Read-Log $role) -match 'BCMM_DRAG_NET_DISCONNECT_OWNER' -and !$observers[$role].HasExited){Stop-Process -Id $observers[$role].Id}
  }
  if(($server+($all -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError'){throw 'Drag network fixture failed; inspect logs.'}
  $finished=$server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)
  $observed=$true
  foreach($log in $all){$lastPhase=8;if($log -match 'BCMM_DRAG_NET_DISCONNECT_OWNER'){$lastPhase=7};foreach($phase in 1..$lastPhase){if($log -notmatch ('BCMM_DRAG_NET_OBSERVED phase='+$phase+' ')){$observed=$false}}}
  if($finished -and $observed){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}catch{$failure=$_.Exception.Message}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){
 Write-Output $role
 (Read-Log $role) -split "`n" | Where-Object {$_ -match 'BCMM_DRAG_NET_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED|SCRIPT.*\(E\)|PLATFORM.*\(E\)'} | Select-Object -Last 10
}
Write-Output ('Run directory: '+$run)
$packedHash=$null
if(Test-Path (Join-Path $ProductionDirectory 'data.pak')){
 $packedHash=(Get-FileHash (Join-Path $ProductionDirectory 'data.pak')).Hash
 foreach($role in @('server','observer-a','observer-b','late-join')){
  $log=Read-Log $role
  if(!$log.Contains($ProductionDirectory) -or !$log.Contains("Adding package '")){$passed=$false;$failure='Packed production mount not established on every peer'}
 }
}
[ordered]@{passed=$passed;failure=$failure;run=$run;token=$token;production=$ProductionDirectory;packedSha256=$packedHash;scope='Dedicated authority plus two initial clients and one late joiner; native owner walk, RPC release/reacquire and actual owner disconnect'}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
if(!$passed){throw ('Drag replication checks did not all complete successfully. '+$failure)}
