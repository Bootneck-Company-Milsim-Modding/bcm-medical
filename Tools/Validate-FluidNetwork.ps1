param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\fluid-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $run -Force)
$sourceFiles=@(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Assets'),(Join-Path $root 'Source'),(Join-Path $root 'Tests/Runtime')) -Recurse -File | Where-Object { $_.Name -notlike 'resourceDatabase.rdb*' })+@(Get-Item (Join-Path $root 'addon.gproj'))
$manifest=[ordered]@{Project='BCM Medical';RunnerHash=(Get-FileHash $PSCommandPath).Hash;Sources=@($sourceFiles|Sort-Object FullName|ForEach-Object{[ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash -LiteralPath $_.FullName).Hash}})}
$manifest|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $run 'validation-manifest.json')
Write-Output ('Medical progressive fluid network run='+$run)
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 -bcmmEquipmentNetwork 1 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$argsText=$common+'-client 127.0.0.1 -port '+$port+' -bcmmFluidObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $argsText=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmFluidNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $argsText -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(180);$started=$false;$late=$false
 while((Get-Date) -lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  if(!$late -and $server -match 'BCMM_FLUID_PHASE 1(?:\s|$)'){Start-Observer 'late-join';$late=$true}
  $logs=@('observer-a','observer-b','late-join')|ForEach-Object{Read-Log $_}
  if(($server+($logs -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Typed-fluid network check failed; inspect logs.'}
  $observed=$late
  if(($logs -join "`n") -notmatch "BCMM_FLUID_NATIVE_OWNER"){$observed=$false}
  foreach($log in $logs){foreach($phase in 1..5){if($log -notmatch ('BCMM_FLUID_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  foreach($log in $logs){foreach($phase in 2..4){if($log -notmatch ('BCMM_FLUID_PROGRESS_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}}
  foreach($phase in 2..4){if(($logs -join "`n") -notmatch ('BCMM_FLUID_CONTENTS_OBSERVED '+$phase+'(?:\s|$)')){$observed=$false}}
  foreach($requestPhase in 2..4){if(($logs -join "`n") -notmatch ('BCMM_FLUID_OWNER_SENT '+$requestPhase+'(?:\s|$)')){$observed=$false}}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
foreach($role in @('server','observer-a','observer-b','late-join')){Write-Output $role;(Read-Log $role) -split "`n"|Where-Object{$_ -match 'BCMM_FLUID_OBSERVED|BCMM_FLUID_PROGRESS_OBSERVED|BCMM_FLUID_CONTENTS_OBSERVED|BCMM_FLUID_OWNER|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED'}|Select-Object -Last 7}
$unchanged=(Get-FileHash $PSCommandPath).Hash -eq $manifest.RunnerHash
foreach($source in $manifest.Sources){$path=Join-Path $root $source.Path;if(!(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path).Hash -ne $source.SHA256){$unchanged=$false}}
$passed=$passed -and $unchanged
[ordered]@{Passed=$passed;SourceUnchanged=$unchanged;Run=$run;ServerComplete=([regex]::Match((Read-Log 'server'),'BCMM_CHECK COMPLETE checks=\d+ failures=\d+').Value)}|ConvertTo-Json|Set-Content -LiteralPath (Join-Path $run 'result.json')
Write-Output ('Run directory: '+$run)
if(!$passed){throw 'Typed-fluid owner/proxy/late-join checks did not all pass.'}
