param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content (Join-Path $root '..\BCM NID AceCompat\.dedicated\server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks\living-transport-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $run)
$token=[Guid]::NewGuid().ToString('N');$port=Get-Random -Minimum 24000 -Maximum 30000
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+(Join-Path $root 'Tests\Runtime\addon.gproj')+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new()
function Snapshot {@(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests\Runtime\Scripts'),(Join-Path $root 'UI'),(Join-Path $root 'Prefabs/Medical/Transport'),(Join-Path $root 'Source/Animations/Casualty')) -Recurse -File | Sort-Object FullName | ForEach-Object {$_.FullName+'|'+(Get-FileHash $_.FullName).Hash})}
$before=Snapshot
function Read-Log([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Sort-Object LastWriteTime -Descending|Select-Object -First 1;if($file){return Get-Content $file.FullName -Raw};return ''}
function Start-Observer([string]$role){$text=$common+'-client 127.0.0.1 -port '+$port+' -bcmmCargoObserver 1 -bcmmLivingTransportObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 1280 -screenHeight 720 -posX -2000 -posY -2000';$processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $text -WindowStyle Hidden -PassThru))}
$passed=$false
try{
 $text=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -port '+$port+' -bcmmLivingTransportNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $text -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(180);$started=$false;$late=$false
 while((Get-Date)-lt $limit){
  $server=Read-Log 'server'
  if(!$started -and $server -match 'Starting RPL server'){Start-Observer 'owner';Start-Observer 'patient';$started=$true}
  if(!$late -and $server -match 'BCMM_LIVING_NET_PHASE 4'){Start-Observer 'late-join';$late=$true}
  $clients=@('owner','patient','late-join')|ForEach-Object {Read-Log $_}
  if(($server+($clients -join "`n"))-match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|RplLayoutError|RplLoadError'){throw 'Living transport network fixture failed; inspect saved logs.'}
  $done=$server -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)
  $observed=$late
  foreach($log in $clients){foreach($phase in @(4,7,10,15)){if($log -notmatch ('BCMM_LIVING_NET_OBSERVED phase='+$phase+' ')){$observed=$false}}}
  # Native private inventory is inspectable by its owner, not arbitrary proxies.
  # Authority checks the exact entity; actual owner confirms replicated retention.
  if(($clients -join "`n") -notmatch 'BCMM_LIVING_NET_OBSERVED phase=13 player=2 originalWeapon='){$observed=$false}
  if(($clients -join "`n") -notmatch 'BCMM_LIVING_NET_OBSERVED phase=16 player=2 controlsRestored=1'){$observed=$false}
  if(($clients -join "`n") -notmatch 'BCMM_LIVING_REPLAY_CANCELLED player=2 token='){$observed=$false}
  if($done -and $observed){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
}finally{
 foreach($p in $processes){if(!$p.HasExited){Stop-Process -Id $p.Id}}
 $after=Snapshot;$same=@(Compare-Object $before $after).Count -eq 0
 [pscustomobject]@{Passed=$passed;SourceUnchanged=$same;Run=$run;Token=$token}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
}
foreach($role in @('server','owner','patient','late-join')){Write-Output $role;(Read-Log $role)-split "`n"|Where-Object {$_ -match 'BCMM_LIVING_NET_|BCMM_CHECK COMPLETE|BCMM_CHECK FAILED|SCRIPT.*\(E\)'}|Select-Object -Last 12}
Write-Output ('Run directory: '+$run)
if(!$passed -or !$same){throw 'Living transport owner/patient/late-join acceptance did not pass unchanged.'}
