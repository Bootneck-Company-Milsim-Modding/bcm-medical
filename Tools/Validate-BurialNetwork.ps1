param([Parameter(Mandatory)][string]$ServerDirectory,[switch]$SearchSessions,[ValidateRange(120,360)][int]$TimeoutSeconds=210)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$run=Join-Path $root ('.checks\burial-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $run -Force | Out-Null
$token=[Guid]::NewGuid().ToString('N')
$project=Join-Path $root 'Tests\Runtime\addon.gproj'
$dirs=$root+','+(Join-Path $root 'Tests\Runtime')+','+(Join-Path $game 'addons')
$common='-gproj "'+$project+'" -addons B6C4ED1CA1260002 -addonsDir "'+$dirs+'" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
if($SearchSessions){$common+='-bcmmBurialSearch 1 '}
$files=@(Get-ChildItem (Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'Language'),(Join-Path $root 'Tests\Runtime') -Recurse -File | Where-Object Extension -in '.c','.et','.conf','.meta','.st','.ent','.layer','.layout','.gproj')+@(Get-Item (Join-Path $root 'addon.gproj'))
$manifest=[ordered]@{Project='BCM Medical';RunToken=$token;RunnerHash=(Get-FileHash $PSCommandPath).Hash;Sources=@($files | Sort-Object FullName | ForEach-Object {[ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash $_.FullName).Hash}})}
$manifest.SearchSessions=[bool]$SearchSessions
$manifest|ConvertTo-Json -Depth 5|Set-Content (Join-Path $run 'validation-manifest.json')
$owned=[Collections.Generic.List[Diagnostics.Process]]::new()
function Read-BurialLog([string]$role){$file=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue | Select-Object -First 1;if($file){return Get-Content $file.FullName -Raw};return ''}
function Start-BurialClient([string]$role){
 $arguments=$common+'-client 127.0.0.1 -port 2001 -bcmmCargoObserver 1 -bcmmBurialObserver 1 -profile "'+(Join-Path $run $role)+'" -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000'
 $process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
 $owned.Add($process);Write-Output "Medical burial client role=$role PID=$($process.Id)"
}
$passed=$false;$clients=$false;$late=$false;$last=''
try{
 $arguments=$common+'-server "Worlds/BCMM_TerrainChecks.ent" -bcmmBurialNetwork 1 -bcmmRunToken '+$token+' -profile "'+(Join-Path $run 'server')+'"'
 $server=Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
 $owned.Add($server);Write-Output "Medical burial server PID=$($server.Id) Logs=$run"
 $deadline=(Get-Date).AddSeconds($TimeoutSeconds)
 while((Get-Date) -lt $deadline){
  $serverText=Read-BurialLog 'server'
  if(!$clients -and $serverText -match 'Starting RPL server'){Start-BurialClient 'owner';Start-BurialClient 'observer';$clients=$true}
  if(!$late -and $serverText -match 'BCMM_BURIAL_NET_PHASE 2'){Start-BurialClient 'late-join';$late=$true}
  $logs=@($serverText,(Read-BurialLog 'owner'),(Read-BurialLog 'observer'),(Read-BurialLog 'late-join'))
  $bad=@($logs -split "`n" | Where-Object {$_ -match 'SCRIPT\s+\(E\)|RPL\s+\(E\)|Virtual Machine Exception|Can.t compile.*script|BCMM_CHECK FAILED|RplCreationError|REPLICATION_SYSTEM_FAILURE|Application crashed' -and $_ -notmatch 'SCRIPT\s+\(E\): BCMM settings invalid: files preserved; edits locked until repaired\.\s*$'})
  if($bad.Count){$bad|Write-Output;break}
  $progress=@($serverText -split "`n" | Where-Object {$_ -match 'BCMM_BURIAL|BCMM_CHECK COMPLETE'}) -join "`n"
  if($progress -ne $last){Write-Output $progress.Substring($last.Length);$last=$progress}
  if($serverText -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){
   $passed=$late
   foreach($role in @('owner','observer','late-join')){if((Read-BurialLog $role) -notmatch 'BCMM_BURIAL_NET_MEMORIAL_PASS'){$passed=$false}}
   if(((Read-BurialLog 'owner')+(Read-BurialLog 'observer')) -notmatch 'BCMM_BURIAL_NET_DIG_PASS owner=1' -or ((Read-BurialLog 'owner')+(Read-BurialLog 'observer')) -notmatch 'BCMM_BURIAL_NET_DIG_PASS owner=0'){$passed=$false}
   if($SearchSessions -and $serverText -notmatch 'BCMM_BURIAL_SEARCH authority refused actual owner burial RPC while native inventory was open'){$passed=$false}
   break
  }
  if($serverText -match 'BCMM_CHECK COMPLETE' -or $server.HasExited){break}
  Start-Sleep -Milliseconds 500
 }
}finally{foreach($process in $owned){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
 $sourceUnchanged=$true
 foreach($entry in $manifest.Sources){$path=Join-Path $root $entry.Path;if(!(Test-Path -LiteralPath $path) -or (Get-FileHash -LiteralPath $path).Hash -ne $entry.SHA256){$sourceUnchanged=$false}}
 if(!$sourceUnchanged){$passed=$false}
 [ordered]@{Passed=$passed;SourceUnchanged=$sourceUnchanged;RunDirectory=$run;RunToken=$token}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
foreach($role in @('owner','observer','late-join')){(Read-BurialLog $role) -split "`n" | Where-Object {$_ -match 'BCMM_BURIAL_NET_|BCMM_REMAINS_NOTICE'}}
if(!$passed){throw "Medical burial network acceptance failed; inspect $run"}

