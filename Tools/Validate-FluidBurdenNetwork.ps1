param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\fluid-burden-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$token=[Guid]::NewGuid().ToString('N')
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 -bcmmEquipmentNetwork 1 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Snapshot { @(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Tools'),(Join-Path $root 'addon.gproj')) -Recurse -File | Where-Object Extension -in '.c','.et','.meta','.conf','.layout','.ps1','.py','.json','.st','.gproj' | Sort-Object FullName | ForEach-Object { $_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash }) }
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -bcmmBurdenObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$before=Snapshot;$passed=$false
try {
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -bcmmFluidBurdenNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(120);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_BURDEN_PHASE 1(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join') | ForEach-Object {Read-Log $_}
  if($processes[0].HasExited){throw 'Burden diagnostic server exited before acceptance.'}
  if(($server+($logs -join "`n")) -match 'Unable to start replication|Unable to initialize the game|BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Burden native network checks failed; inspect retained logs.'}
  $observed=$late
  foreach($log in $logs){foreach($phase in 1..3){if($log -notmatch ('BCMM_BURDEN_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  foreach($uiPhase in 1..3){if(($logs -join "`n") -notmatch ('BCMM_BURDEN_OWNER_UI '+$uiPhase+'(?:\s|$)')){$observed=$false}}
  if(($logs -join "`n") -notmatch 'BCMM_BURDEN_OWNER_BYPASS_SENT' -or $server -notmatch 'BCMM_EQUIPMENT_REQUEST action=16 region=3 started=0'){$observed=$false}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
} finally {foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
$after=Snapshot;$unchanged=@(Compare-Object $before $after).Count -eq 0
New-Item -ItemType Directory -Path $run -Force | Out-Null
$before | Set-Content (Join-Path $run 'source-before.txt');$after | Set-Content (Join-Path $run 'source-after.txt')
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;RunToken=$token;RunDirectory=$run} | ConvertTo-Json | Set-Content (Join-Path $run 'result.json')
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n" | Where-Object {$_ -match 'BCMM_BURDEN_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'} | Select-Object -Last 8}
Write-Output ('Run directory: '+$run)
if(!$passed -or !$unchanged){throw 'Optional fluid burden owner/proxy/active-session late-join acceptance did not pass unchanged.'}

