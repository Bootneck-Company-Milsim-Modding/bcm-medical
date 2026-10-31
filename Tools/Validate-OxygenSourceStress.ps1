param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content -LiteralPath (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\oxygen-source-stress-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $run)
$files=@(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'Tests/Runtime')) -Recurse -File|Where-Object{$_.Name -notlike 'resourceDatabase.rdb*'})+@(Get-Item (Join-Path $root 'addon.gproj'),$PSCommandPath)
$manifest=@($files|Sort-Object FullName -Unique|ForEach-Object{[ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash $_.FullName).Hash}})
$manifest|ConvertTo-Json|Set-Content (Join-Path $run 'validation-manifest.json')
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests/Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content -LiteralPath $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$launch=$common+'-client 127.0.0.1 -port '+$port+' -bcmmOxygenStressObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $launch -WindowStyle Hidden -PassThru))}
$passed=$false
Write-Output ('Native oxygen stress run: '+$run)
try{
 $launch=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmOxygenStress 1 -bcmmOxygenNetwork 1 -bcmmEquipmentNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $launch -WindowStyle Hidden -PassThru))
 $deadline=(Get-Date).AddSeconds(110);$started=$false;$late=$false
 while((Get-Date) -lt $deadline){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'observer-a';Start-Observer 'observer-b';$started=$true}
  $late=$true
  $logs=@('observer-a','observer-b')|ForEach-Object{Read-Log $_}
  if(($server+($logs -join "`n")) -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|ValidationError|source code checksum does not match'){throw 'Native oxygen stress failure; inspect saved logs'}
  $observed=$true
  foreach($phase in 1..10){if(($logs -join "`n") -notmatch ('BCMM_OXYGEN_STRESS_OWNER_SENT '+$phase+'(?:\s|$)')){$observed=$false}}
  if($observed -and $server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
$unchanged=$true;foreach($source in $manifest){$path=Join-Path $root $source.Path;if(!(Test-Path -LiteralPath $path) -or (Get-FileHash $path).Hash -ne $source.SHA256){$unchanged=$false}}
$passed=$passed -and $unchanged
[ordered]@{Passed=$passed;SourceUnchanged=$unchanged;Run=$run;ServerComplete=([regex]::Match((Read-Log 'server'),'BCMM_CHECK COMPLETE checks=\d+ failures=\d+').Value)}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
foreach($role in @('server','observer-a','observer-b')){Write-Output $role;(Read-Log $role) -split "`n"|Where-Object{$_ -match 'BCMM_OXYGEN_STRESS|BCMM_PROCEDURE_CANCEL|BCMM_CHECK (COMPLETE|FAILED)'}|Select-Object -Last 8}
Write-Output ('Native oxygen stress result: '+$run)
if(!$passed){throw 'Native oxygen source stress acceptance did not pass'}
