param([string]$ServerDirectory='',[switch]$Catalogue)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent;$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$prefix=if($Catalogue){'.checks/gm-catalogue-network-'}else{'.checks/life-admin-network-'};$run=Join-Path $root ($prefix+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'));[void](New-Item -ItemType Directory -Path $run)
function Snapshot {@(Get-ChildItem @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Configs'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'UI'),(Join-Path $root 'Language')) -Recurse -File|Where-Object{$_.Name -notlike 'resourceDatabase.rdb*'}|Sort-Object FullName|ForEach-Object{$_.FullName+'|'+(Get-FileHash $_.FullName).Hash})+@((Get-FileHash $PSCommandPath).Hash)}
$before=Snapshot;$before|Set-Content (Join-Path $run 'source-before.txt');$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests/Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
if($Catalogue){$common+=' -bcmmCatalogueNetwork 1 '}
$lateStarted=$false
$processes=[Collections.Generic.List[Diagnostics.Process]]::new();$passed=$false;$failure=''
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content $file.FullName -Raw};return ''}
try{
 $launch=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmLifeAdminNetwork 1 -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -keepSessionSave -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 if($Catalogue){$launch=$launch.Replace('worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent','Worlds/BCMM_TerrainChecks.ent')}
 $server=Start-Process (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $launch -WindowStyle Hidden -PassThru;$processes.Add($server)
 $limit=(Get-Date).AddSeconds(100);$started=$false
 while((Get-Date) -lt $limit){
  $serverLog=Read-Log 'server'
  if(!$started -and $serverLog -match 'Starting RPL server'){
   foreach($role in @('client-a','client-b')){$clientArgs=$common+'-client 127.0.0.1 -port '+$port+' -bcmmLifeAdminObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $clientArgs -WindowStyle Hidden -PassThru))};$started=$true
  }
  if($Catalogue -and !$lateStarted -and $serverLog -match 'BCMM_LIFE_ADMIN_PHASE 2'){
   $lateArgs=$common+'-client 127.0.0.1 -port '+$port+' -bcmmLifeAdminObserver 1 -profile "'+(Join-Path $run 'client-late')+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $lateArgs -WindowStyle Hidden -PassThru));$lateStarted=$true
  }
  $clients=(Read-Log 'client-a')+"`n"+(Read-Log 'client-b')+"`n"+(Read-Log 'client-late')
  if(($serverLog+$clients) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|source code checksum does not match'){throw 'Native GM allowance assertion or engine failure; inspect logs'}
  $observed=$true;foreach($phase in 1..4){if($clients -notmatch ('BCMM_LIFE_ADMIN_OWNER_SENT phase='+$phase+'(?:\s|$)') -or $clients -notmatch ('BCMM_LIFE_ADMIN_TARGET_OBSERVED phase='+$phase+'(?:\s|$)')){$observed=$false}}
  if($clients -notmatch 'BCMM_LIFE_ADMIN_REVOKED actualModeGone=1' -or $clients -notmatch 'BCMM_LIFE_ADMIN_TARGET_OBSERVED phase=5'){$observed=$false}
  if($Catalogue){
   if($clients -notmatch 'BCMM_CATALOGUE_OWNER_SENT index=\d+ nativeRPC=1 thumbnail=1' -or $serverLog -notmatch 'BCMM_CATALOGUE_SERVER_PLACED'){$observed=$false}
   foreach($observer in 1..3){if($clients -notmatch ('BCMM_CATALOGUE_OBSERVED player='+$observer+' actualStation=1')){$observed=$false}}
  }
  if($observed -and $serverLog -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  if($server.HasExited){throw 'Native GM allowance server finished before required owner and target observations'}
  Start-Sleep -Milliseconds 500
 }
 if(!$passed){throw 'Native owning GM action/RPC and target allowance observations incomplete'}
}catch{$failure=$_.Exception.Message}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
if($passed){try{$manifest=Get-Content (Join-Path $run 'server/profile/BCMM_LifeAdminRestart.json') -Raw|ConvertFrom-Json;& (Join-Path $PSScriptRoot 'Validate.ps1') -LifeAdminRestartRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory (Join-Path $run 'server') -NativeSaveId $manifest.SaveId -TerrainRuntime:$Catalogue}catch{$passed=$false;$failure=$_.Exception.Message}}
$after=Snapshot;$after|Set-Content (Join-Path $run 'source-after.txt');$unchanged=@(Compare-Object $before $after).Count -eq 0
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;Failure=$failure;Run=$run;Catalogue=$Catalogue.IsPresent;Scope='Native connected editor authority/actions and private target status; optional actual catalogue placement/late join/restart. Diagnostic account IDs; no backend authentication claim'}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output ('Native GM allowance evidence: '+$run);if(!$passed -or !$unchanged){throw $failure}


