param([ValidateRange(0,200)][int]$Cycles=0,[ValidatePattern('^BCMM_[A-Za-z0-9]+$')][string]$EquipFrom,[switch]$IconBoard,[switch]$ProgressReview,[switch]$KnowledgeReview,[switch]$BagReview,[switch]$HoldReview,[switch]$Stop)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$state=Join-Path $root '.checks/beta01-item-review-process.json'
if(Test-Path -LiteralPath $state){
 $old=Get-Content -LiteralPath $state -Raw|ConvertFrom-Json
 $owned=Get-CimInstance Win32_Process -Filter ('ProcessId='+$old.pid) -ErrorAction SilentlyContinue
 if($owned -and $owned.CommandLine.Contains($old.profile) -and $owned.CommandLine.Contains('-bcmmBeta01Items 1')){Stop-Process -Id $old.pid}
}
if($Stop){return}
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$profile=Join-Path $root ('.checks/beta01-item-review-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $profile -Force|Out-Null
$arguments='-gproj "'+(Join-Path $root 'Tests/Runtime/addon.gproj')+'" -profile "'+$profile+'" -addonsDir "'+$root+','+(Join-Path $root 'Tests/Runtime')+','+(Join-Path $game 'addons')+'" -world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -disableCrashReporter -noSound -noThrow -forceUpdate -window -screenWidth 1600 -screenHeight 1000 -posX 0 -posY 0 -maxFPS 30 -bcmmUIPreview 1 -bcmmBeta01Items 1'
if($Cycles){$arguments+=' -bcmmBeta01EquipCycles '+$Cycles}
if($EquipFrom){$arguments+=' -bcmmBeta01EquipFrom '+$EquipFrom}
if($IconBoard){$arguments+=' -bcmmBeta01IconBoard 1'}
if($ProgressReview){$arguments+=' -bcmmBeta01Progress 1'}
if($KnowledgeReview){$arguments+=' -bcmmBeta01Knowledge 1'}
if($BagReview){$arguments+=' -bcmmBeta01BagVisual 1'}
if($HoldReview){$arguments+=' -bcmmBeta01HoldReview 1'}
$process=Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
[ordered]@{pid=$process.Id;profile=$profile;cycles=$Cycles;startedUtc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content -LiteralPath $state
Write-Output $profile
