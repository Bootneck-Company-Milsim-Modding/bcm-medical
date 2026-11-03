param([ValidateSet('Bagging','Splint','ClinicalSafety','DragMatrix')][string]$Case='Bagging',[ValidateRange(1,200)][int]$Iterations=200,[string]$ProductionDirectory='', [ValidateRange(0,100000)][int]$SeedOffset=0, [switch]$AirborneSetup)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
if(!$ProductionDirectory){$ProductionDirectory=$root}
$run=Join-Path $root ('.checks/repeated-'+$Case+'-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item $run -ItemType Directory -Force|Out-Null
$game='C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger'
$flag=@{Bagging='-bcmmBaggingRuntime 1 -bcmmBeta01LoadedRope 1';Splint='-bcmmSplintRuntime 1';ClinicalSafety='-bcmmClinicalSafetyRuntime 1';DragMatrix='-bcmmBeta01DragMatrix 1'}[$Case]
$token=[Guid]::NewGuid().ToString('N')
$arguments='-gproj "'+$root+'/Tests/Runtime/addon.gproj" -addons B6C4ED1CA1260002 -addonsDir "'+$ProductionDirectory+','+$root+'/Tests/Runtime,'+$game+'/addons" -profile "'+$run+'/profile" -world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000 -maxFPS 30 -bcmmRunToken '+$token+' -bcmmRepeatNative '+$Iterations+' '+$flag
$arguments+=' -bcmmRepeatSeedOffset '+$SeedOffset
if($AirborneSetup){$arguments+=' -bcmmBeta01DragAirborneSetup 1'}
$process=Start-Process (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
[ordered]@{pid=$process.Id;profile=$run;case=$Case;target=$Iterations;token=$token;production=$ProductionDirectory;seedOffset=$SeedOffset;airborneSetup=[bool]$AirborneSetup}|ConvertTo-Json|Set-Content (Join-Path $run 'process.json')
Write-Output ('Repeated native campaign: '+$run)
$passed=$false;$failure='';$text=''
try{
 $deadline=(Get-Date).AddSeconds(180*$Iterations+90)
 while((Get-Date) -lt $deadline){
  $log=Get-ChildItem (Join-Path $run 'profile') -Recurse -Filter console.log -ErrorAction SilentlyContinue|Select-Object -First 1
  if($log){$text=Get-Content $log.FullName -Raw}
  if($text -match 'BCMM_CHECK FAILED|Can.t compile.*script|Virtual Machine Exception'){throw 'Native repeated fixture failed; preserve log and inspect'}
  if($text -match ('BCMM_CHECK COMPLETE checks=\d+ failures=0 run='+$token)){$passed=$true;break}
  if($process.HasExited){throw 'Native process exited before completion'}
  Start-Sleep -Seconds 1
 }
 if(!$passed){throw 'Native repeated campaign timed out'}
}catch{$failure=$_.Exception.Message}finally{if(!$process.HasExited){Stop-Process -Id $process.Id}}
$completed=[regex]::Matches($text,'BCMM_NATIVE_ITERATION completed=(\d+) target=(\d+) checks=(\d+) failures=(\d+)')
$last=0;if($completed.Count){$last=[int]$completed[$completed.Count-1].Groups[1].Value}
if($passed -and $last -ne $Iterations){$passed=$false;$failure='Successful completion did not include every requested native instance'}
[ordered]@{passed=$passed;failure=$failure;case=$Case;requested=$Iterations;completed=$last;seedOffset=$SeedOffset;airborneSetup=[bool]$AirborneSetup;run=$run;token=$token;completion=([regex]::Match($text,'BCMM_CHECK COMPLETE checks=\d+ failures=\d+ run=\w+').Value)}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output ('Completed native instances: '+$last)
if(!$passed){throw $failure}
