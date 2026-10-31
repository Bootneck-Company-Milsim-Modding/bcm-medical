param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks/world-persistence-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $run)
$files=@(Get-ChildItem @((Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'Tests/Runtime')) -Recurse -File|Where-Object{$_.Name -notlike 'resourceDatabase.rdb*'})+@(Get-Item $PSCommandPath,(Join-Path $PSScriptRoot 'Validate.ps1'))
$before=@($files|Sort-Object FullName|ForEach-Object{$_.FullName+'|'+(Get-FileHash $_.FullName).Hash})
$before|Set-Content (Join-Path $run 'source-before.txt')
& (Join-Path $PSScriptRoot 'Validate.ps1') -FinitePersistenceRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -FinitePersistencePhase seed -CargoVehicle Truck
$manifest=Get-Content (Join-Path $run 'profile/BCMM_FinitePersistence.json') -Raw|ConvertFrom-Json
if(!$manifest.SaveId){throw 'Native seed did not produce a saved-world UUID'}
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests/Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$profile=Join-Path $run $role;$file=Get-ChildItem $profile -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -port '+$port+' -bcmmWorldPersistenceObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$passed=$false;$failure=''
try{
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -worldSystemsConfig "{C50579A2EC48E8EF}Configs/Systems/MissionSystems.conf" -loadSessionSave '+$manifest.SaveId+' -keepSessionSave -bcmmWorldPersistenceNetwork 1 -bcmmRunToken '+$token+' -profile "'+$run+'"'
 $processes.Add((Start-Process (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(150);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'logs'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_WORLD_NETWORK_PHASE 2(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join')|ForEach-Object{Read-Log $_}

  if(($server+($logs -join "`n")) -match 'Unable to start replication|Unable to initialize the game|BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Native saved-world network assertion or engine failure; inspect logs'}
  $observed=$late
  foreach($index in 0..2){$phases=@(1,2);if($index -eq 2){$phases=@(2)};foreach($phase in $phases){if($logs[$index] -notmatch ('BCMM_WORLD_NETWORK_OBSERVED phase='+$phase+'(?:\s|$)')){$observed=$false}}}
  if(($logs -join "`n") -notmatch 'BCMM_WORLD_NETWORK_OWNER phase=1' -or ($logs -join "`n") -notmatch 'BCMM_WORLD_NETWORK_OWNER phase=2'){$observed=$false}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  if($processes[0].HasExited){throw 'Native saved-world server finished before all required client observations completed'}
  Start-Sleep -Milliseconds 500
 }
 if(!$passed){throw 'Saved-world owner/proxy/late-join observations incomplete'}
}catch{$failure=$_.Exception.Message}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
$after=@($files|Sort-Object FullName|ForEach-Object{$_.FullName+'|'+(Get-FileHash $_.FullName).Hash})
$after|Set-Content (Join-Path $run 'source-after.txt');$unchanged=@(Compare-Object $before $after).Count -eq 0
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;SaveId=$manifest.SaveId;Failure=$failure;Run=$run}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output ('Saved-world network evidence: '+$run)
if(!$passed -or !$unchanged){throw ('Saved-world network acceptance failed: '+$failure)}



