param([ValidateSet('Baseline','Bagging','BagActions','Assessment','UI','Packing','DragMatrix','Terrain','Hands','WorldReview','Hint','LivingTransport','ClinicalSafety')][string]$Case='Baseline',[switch]$NativeToggleAction,[ValidateRange(1,200)][int]$ToggleCycles=1,[ValidateRange(640,7680)][int]$Width=1280,[ValidateRange(480,4320)][int]$Height=720,[ValidateRange(1,200)][int]$RepeatInstances=1,[switch]$ActiveAI,[ValidateSet('US','USSR','FIA')][string]$PatientFaction='US')
$ErrorActionPreference='Stop'
if($RepeatInstances -gt 1 -and $Case -notin @('Bagging','ClinicalSafety','DragMatrix')){throw 'Repeated packed fixture is not supported for this case'}
$root=Split-Path $PSScriptRoot -Parent
$pack=Get-Content (Join-Path $root '.checks/beta01-pack-process.json') -Raw|ConvertFrom-Json
if(!(Test-Path (Join-Path $pack.output 'data.pak'))){throw 'Complete the local pack first'}
$run=Join-Path $pack.run ('acceptance-'+$Case+'-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$diagnostic=Join-Path $run 'diagnostic';New-Item -ItemType Directory -Path $diagnostic -Force|Out-Null
if($Case -eq 'Hands'){New-Item -ItemType Directory -Path (Join-Path $run 'profile/screenshots') -Force|Out-Null}
& robocopy (Join-Path $root 'Tests/Runtime') $diagnostic /E /XJ /R:0 /W:0 /NFL /NDL /NJH /NJS /NP | Out-Null
if($LASTEXITCODE -ge 8){throw 'Diagnostic copy failed'}
$game='C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger';$token=[Guid]::NewGuid().ToString('N')
$arguments='-gproj "'+$diagnostic+'/addon.gproj" -addons B6C4ED1CA1260002 -addonsDir "'+$diagnostic+','+$pack.output+','+$game+'/addons" -profile "'+$run+'/profile" -world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -window -screenWidth '+$Width+' -screenHeight '+$Height+' -posX -2000 -posY -2000 -maxFPS 30 -bcmmRunToken '+$token
switch($Case){'Bagging'{$arguments+=' -bcmmBaggingRuntime 1'};'Assessment'{$arguments+=' -bcmmAssessmentRuntime 1'};'UI'{$arguments+=' -bcmmUIPreview 1 -bcmmUISmoke 1'}}
if($Case -eq 'BagActions'){$arguments+=' -bcmmBaggingRuntime 1 -bcmmBeta01BagActions 1'}
if($Case -eq 'Packing'){$arguments+=' -bcmmBeta01Packing 1'}
if($Case -eq 'DragMatrix'){$arguments+=' -bcmmBeta01DragMatrix 1'}
if($Case -eq 'Terrain'){$arguments=$arguments.Replace('worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent','Worlds/BCMM_TerrainChecks.ent');$arguments+=' -bcmmTerrainDrag 1'}
if($Case -eq 'LivingTransport'){$arguments+=' -bcmmLivingTransportRuntime 1 -bcmmTransportPatientFaction '+$PatientFaction;if($ActiveAI){$arguments+=' -bcmmTransportActiveAI 1'}}
if($Case -eq 'ClinicalSafety'){$arguments+=' -bcmmClinicalSafetyRuntime 1'}
if($Case -eq 'Hands'){$arguments+=' -bcmmBeta01Hands 1'}
if($Case -eq 'WorldReview'){$arguments+=' -bcmmBeta01WorldReview 1'}
if($Case -eq 'Hint'){$arguments+=' -bcmmBeta01Hands 1 -bcmmBeta01HandHold 1 -bcmmBeta01HintAuto 1'}
if($Case -eq 'Hint' -or $NativeToggleAction){$arguments+=' -bcmmBeta01ToggleAction 1'}
if($Case -eq 'Hint'){$arguments+=' -bcmmBeta01ToggleCycles '+$ToggleCycles}
if($RepeatInstances -gt 1){$arguments+=' -bcmmRepeatNative '+$RepeatInstances}
$process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
[ordered]@{pid=$process.Id;case=$Case;run=$run;token=$token;startedUtc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content (Join-Path $run 'process.json')
$passed=$false;$failure='';$text='';$log=$null
$samples=[Collections.Generic.List[object]]::new();$nextSample=Get-Date
try{
 $limit=(Get-Date).AddSeconds(100)
 if($Case -in @('Hands','WorldReview')){$limit=(Get-Date).AddSeconds(300)}
 if($Case -eq 'Hint'){$limit=(Get-Date).AddSeconds(100+2*$ToggleCycles)}
 if($RepeatInstances -gt 1){$limit=(Get-Date).AddSeconds(180*$RepeatInstances+90)}
 while((Get-Date) -lt $limit){
  if((Get-Date) -ge $nextSample -and !$process.HasExited){$process.Refresh();$sample=[ordered]@{utc=[DateTime]::UtcNow.ToString('o');privateBytes=$process.PrivateMemorySize64;workingSetBytes=$process.WorkingSet64;handles=$process.HandleCount};$samples.Add($sample);$sample|ConvertTo-Json -Compress|Add-Content (Join-Path $run 'process-samples.jsonl');$nextSample=(Get-Date).AddSeconds(10)}
  $log=Get-ChildItem (Join-Path $run 'profile') -Recurse -Filter console.log -ErrorAction SilentlyContinue|Select-Object -First 1
  if($log){$text=Get-Content $log.FullName -Raw}
  if($text -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError'){throw 'Packed native test failure; inspect log'}
  if($text -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  if($process.HasExited){throw 'Packed test exited before successful completion'}
  Start-Sleep -Milliseconds 300
 }
 if(!$passed){throw 'Packed test timed out'}
 if($RepeatInstances -gt 1 -and $text -notmatch ('BCMM_NATIVE_ITERATION completed='+$RepeatInstances+' target='+$RepeatInstances+' checks=\d+ failures=0')){throw 'Requested repeated native instances did not complete'}
 if($Case -eq 'Hint' -and $text -notmatch ('BCMM_TOGGLE_CYCLES_COMPLETE cycles='+$ToggleCycles+'(?:\r?\n|$)')){throw 'Requested native toggle cycles did not complete'}
 if($Case -eq 'Terrain' -and $text -notmatch 'BCMM_TERRAIN_DRAG_COMPLETE cases=6'){throw 'Requested terrain cases did not complete'}
 if($Case -eq 'Hands' -and $text -notmatch 'BCMM_HAND_REVIEW_COMPLETE items=26'){throw 'Requested handheld captures did not complete'}
 if($Case -eq 'Hands' -and @((Get-ChildItem (Join-Path $run 'profile') -Recurse -File -Filter 'hand-*-view-*.bmp')).Count -ne 78){throw 'Missing native handheld screenshot files'}
 if($Case -eq 'UI'){
  $capture=Get-ChildItem $run -Recurse -Filter 'medical-transport-ui.bmp'|Select-Object -First 1
  if(!$capture){throw 'Native UI screenshot is missing'}
  $reader=[IO.BinaryReader]::new([IO.File]::OpenRead($capture.FullName))
  try{$reader.BaseStream.Seek(18,[IO.SeekOrigin]::Begin)|Out-Null;$actualWidth=$reader.ReadInt32();$actualHeight=[Math]::Abs($reader.ReadInt32())}finally{$reader.Dispose()}
  if($actualWidth -ne $Width -or $actualHeight -ne $Height){throw 'Native UI screenshot dimensions differ from requested resolution'}
 }
 if($Case -eq 'WorldReview' -and ($text -notmatch 'BCMM_WORLD_REVIEW_COMPLETE items=29 captures=87' -or @(Get-ChildItem (Join-Path $run 'profile') -Recurse -Filter 'world-*-distance-*.bmp').Count -ne 87)){throw 'World distance captures incomplete'}
 # Actual mounts, rather than CLI intent, establish isolation from loose source.
 $normalized=$text.Replace('\','/');$packedPath=$pack.output.Replace('\','/')
 $mounts=@($normalized -split "`n" | Where-Object {$_ -match 'name=MCBMedical path='})
 $packedMounted=$normalized.Contains("Adding package '"+$packedPath+"/' (pak count: 1)") -and $mounts.Count -gt 0
 $looseMounted=@($mounts | Where-Object {!$_.Contains('path='+$packedPath+'/resourceDatabase.rdb)')}).Count -gt 0
 if(!$packedMounted -or $looseMounted){throw 'Cannot establish isolated packed medical resource mount'}
}catch{$failure=$_.Exception.Message;$passed=$false}finally{if(!$process.HasExited){Stop-Process -Id $process.Id}}
[ordered]@{passed=$passed;case=$Case;activeAI=[bool]$ActiveAI;patientFaction=$PatientFaction;width=$Width;height=$Height;repeatInstances=$RepeatInstances;processSamples=$samples.ToArray();nativeToggleAction=($Case -eq 'Hint' -or [bool]$NativeToggleAction);toggleCycles=$ToggleCycles;failure=$failure;run=$run;packedSha256=(Get-FileHash (Join-Path $pack.output 'data.pak')).Hash;completion=([regex]::Match($text,'BCMM_CHECK COMPLETE checks=\d+ failures=\d+ run=\w+').Value);scope='Packed production addon plus separate unpublished diagnostic addon; loose production source excluded from addonsDir'}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output $run
Write-Output ([regex]::Match($text,'BCMM_CHECK COMPLETE checks=\d+ failures=\d+ run=\w+').Value)
if(!$passed){throw $failure}
