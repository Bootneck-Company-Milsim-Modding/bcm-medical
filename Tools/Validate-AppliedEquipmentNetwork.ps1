param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\applied-equipment-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -port '+$port+' -bcmmAppliedObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmAppliedEquipmentNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(120);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_APPLIED_PHASE 1(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join')|ForEach-Object{Read-Log $_}
  if(($server+($logs -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Medical equipment network check failed; inspect logs.'}
  $observed=$late
  foreach($log in $logs){foreach($phase in 1..5){if($log -notmatch ('BCMM_APPLIED_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  if(($logs -join "`n") -notmatch 'BCMM_UNSAFE_OWNER_MEDICATION_SENT' -or $server -notmatch 'BCMM_UNSAFE_OWNER_PRESSURE_REJECTED'){$observed=$false}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n"|Where-Object{$_ -match 'BCMM_APPLIED_OBSERVED|BCMM_UNSAFE_OWNER|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'}|Select-Object -Last 7}
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Medical owner/proxy/late-join equipment checks did not all pass.'}
