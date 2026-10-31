param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\native-stock-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$token=[Guid]::NewGuid().ToString('N')
$dirs=(Join-Path $root 'Tests/Runtime')+','+$root+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Snapshot { @(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Tools'),(Join-Path $root 'addon.gproj')) -Recurse -File | Where-Object Extension -in '.c','.et','.meta','.conf','.layout','.ps1','.py','.json','.st','.gproj' | Sort-Object FullName | ForEach-Object { $_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash }) }
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -bcmmStockObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$before=Snapshot;$passed=$false;$failure=''
try {
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -bcmmStockCommitNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(120);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_STOCK_NETWORK_PHASE 2(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join') | ForEach-Object {Read-Log $_}
  if($processes[0].HasExited){throw 'Native-stock diagnostic server exited before acceptance.'}
  if(($server+($logs -join "`n")) -match 'Unable to start replication|Unable to initialize the game|BCMM_STOCK_NETWORK_FAILED|BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Native-stock native network checks failed; inspect retained logs.'}
  $observed=$late
  foreach($log in $logs){foreach($phase in @(3,5)){if($log -notmatch ('BCMM_STOCK_NETWORK_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  if(($logs -join "`n") -notmatch 'BCMM_STOCK_OWNER_BEGAN phase=1' -or ($logs -join "`n") -notmatch 'BCMM_STOCK_OWNER_ENDED phase=1' -or ($logs -join "`n") -notmatch 'BCMM_STOCK_OWNER_RETAINED original=1 dosesBlocked=1'){$observed=$false}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
} catch {$failure=$_.Exception.Message} finally {foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
$after=Snapshot;$unchanged=@(Compare-Object $before $after).Count -eq 0
New-Item -ItemType Directory -Path $run -Force | Out-Null
$before | Set-Content (Join-Path $run 'source-before.txt');$after | Set-Content (Join-Path $run 'source-after.txt')
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;Failure=$failure;Scope='Actual owner native stock animation/retention/success, server commit policy, proxy and post-revocation joiner phase/settings observations; no remote inventory inspection or backend authentication claim';RunToken=$token;RunDirectory=$run} | ConvertTo-Json | Set-Content (Join-Path $run 'result.json')
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n" | Where-Object {$_ -match 'BCMM_STOCK_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'} | Select-Object -Last 8}
Write-Output ('Run directory: '+$run)
if(!$passed -or !$unchanged){throw 'Native stock owner commit/proxy/post-revocation joiner acceptance did not pass unchanged.'}


