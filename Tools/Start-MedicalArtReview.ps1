param([ValidateSet('mask','tablet')][string]$Kind='mask',[switch]$Stop)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$stateFile=Join-Path $root '.checks/medical-art-review-process.json'
if(Test-Path -LiteralPath $stateFile) {
 $old=Get-Content -LiteralPath $stateFile -Raw | ConvertFrom-Json
 $running=Get-CimInstance Win32_Process -Filter ('ProcessId='+$old.pid) -ErrorAction SilentlyContinue
 if($running -and $running.CommandLine.Contains($old.profile) -and $running.CommandLine.Contains('-bcmmMedicalArtReview')) { Stop-Process -Id $old.pid }
}
if($Stop) { return }
$profile=Join-Path $root ('.checks/medical-art-review-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$dirs=$root+','+(Join-Path $root 'Tests/Runtime')+',C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger/addons'
$arguments='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -profile "'+$profile+'" -addonsDir "'+$dirs+'" -addons B6C4ED1CA1260002 -world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -noBackend -disableCrashReporter -noSound -noThrow -forceUpdate -window -screenWidth 1280 -screenHeight 720 -posX 0 -posY 0 -maxFPS 30 -bcmmMedicalArtReview '+$Kind
$process=Start-Process -FilePath 'C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger/ArmaReforgerSteamDiag.exe' -ArgumentList $arguments -WindowStyle Hidden -PassThru
@{pid=$process.Id;profile=$profile;kind=$Kind} | ConvertTo-Json | Set-Content -LiteralPath $stateFile
Write-Output $profile

