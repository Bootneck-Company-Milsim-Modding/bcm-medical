param([switch]$Stop)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$stateFile=Join-Path $root '.checks/ui-review-active.json'
if(Test-Path $stateFile){
 $old=Get-Content $stateFile -Raw|ConvertFrom-Json
 $process=Get-CimInstance Win32_Process -Filter ('ProcessId='+$old.pid) -ErrorAction SilentlyContinue
 if($process -and $process.CommandLine.Contains($old.profile) -and $process.CommandLine.Contains('-bcmmUIReview 1')){Stop-Process -Id $old.pid}
}
if($Stop){return}
$game='C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger'
$profile=Join-Path $root ('.checks/ui-review-'+(Get-Date -Format 'yyyyMMdd-HHmmss'))
$dirs=$root+','+(Join-Path $root 'Tests/Runtime')+','+(Join-Path $game 'addons')
$arguments='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -profile "'+$profile+'" -addonsDir "'+$dirs+'" -world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -disableCrashReporter -noSound -noThrow -forceUpdate -window -screenWidth 1600 -screenHeight 1000 -posX 0 -posY 0 -maxFPS 30 -bcmmUIPreview 1 -bcmmUIReview 1'
$process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
[IO.File]::WriteAllText($stateFile,([ordered]@{pid=$process.Id;profile=$profile}|ConvertTo-Json))
Write-Output ('Own UI review process '+$process.Id)

