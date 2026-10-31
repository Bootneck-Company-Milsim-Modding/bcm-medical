param([string]$ServerDirectory = '')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\cargo-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$project=Join-Path $root 'Tests\Runtime\addon.gproj'
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$token=[Guid]::NewGuid().ToString('N')
$common='-gproj "'+$project+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$port=Get-Random -Minimum 24000 -Maximum 30000
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue | Sort-Object LastWriteTime -Descending | Select-Object -First 1; if($file){return Get-Content -LiteralPath $file.FullName -Raw}; return ''}
function Start-Observer([string]$role){$arguments=$common+'-client 127.0.0.1 -port '+$port+' -bcmmCargoObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $serverArgs=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmCargoRuntime 1 -bcmmCargoNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $serverArgs -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(210)
 $clients=$false; $late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$clients -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$clients=$true}
  if($clients -and !$late -and $server -match 'BCMM_NET_PHASE 1'){Start-Observer 'late-join';$late=$true}
  $all=@('observer-a','observer-b','late-join') | ForEach-Object {Read-Log $_}
  if(($server+($all -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError'){throw 'Cargo network fixture failed; inspect logs.'}
  $finished=$server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)
  $observed=$true
  foreach($log in $all){foreach($phase in 1..8){if($log -notmatch ('BCMM_NET_OBSERVED phase='+$phase+' ')){$observed=$false}}}
  if($finished -and $observed){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){
 Write-Output $role
 (Read-Log $role) -split "`n" | Where-Object {$_ -match 'BCMM_NET_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED|SCRIPT.*\(E\)|PLATFORM.*\(E\)'} | Select-Object -Last 10
}
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Cargo replication checks did not all complete successfully.'}
