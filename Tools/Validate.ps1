param([switch]$Runtime, [switch]$ServerRuntime, [string]$ServerDirectory = '', [string]$ProfileDirectory = '', [ValidateSet('','seed','verify','recover')][string]$PersistencePhase = '')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$toolsRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger Tools'
$gameRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$profile = Join-Path $projectRoot ('.checks\' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
if ($ProfileDirectory) { $profile = [IO.Path]::GetFullPath($ProfileDirectory) }
$project = Join-Path $projectRoot 'addon.gproj'
$executable = Join-Path $toolsRoot 'Workbench\ArmaReforgerWorkbenchSteamDiag.exe'
$modeArguments = '-wbModule=ScriptEditor -validate'
$success = 'Script validation successful\.'
$runToken = [Guid]::NewGuid().ToString('N')
if ($Runtime -or $ServerRuntime) {
 $project = Join-Path $projectRoot 'Tests\Runtime\addon.gproj'
 $executable = Join-Path $gameRoot 'ArmaReforgerSteamDiag.exe'
 $modeArguments = '-world "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -noSound -noThrow -noFocus -forceUpdate -window -screenWidth 640 -screenHeight 480 -posX -2000 -posY -2000 -maxFPS 60'
 $success = 'BCMM_CHECK COMPLETE checks=\d+ failures=0 run=' + $runToken
}
if ($ServerRuntime) {
 $serverLocationFile = Join-Path $projectRoot '..\..\.dedicated\server-location.txt'
 if ($ServerDirectory) { $serverRoot = $ServerDirectory } else { $serverLocationFile = Join-Path $projectRoot '..\BCM NID AceCompat\.dedicated\server-location.txt'; $serverRoot = (Get-Content -LiteralPath $serverLocationFile -Raw).Trim() }
 $executable = Join-Path $serverRoot 'ArmaReforgerServerDiag.exe'
 $modeArguments = '-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -addons B6C4ED1CA1260002 -noBackend -noSound -noThrow -maxFPS 60'
}
# Only the new project, its diagnostic addon and native game directory are exposed.
# Installed Workshop addons and legacy BCM projects are deliberately absent.
$addonDirs = $projectRoot + ',' + (Join-Path $projectRoot 'Tests\Runtime') + ',' + (Join-Path $gameRoot 'addons')
$arguments = '-gproj "' + $project + '" -profile "' + $profile + '" -addonsDir "' + $addonDirs + '" ' + $modeArguments
if ($PersistencePhase) { $arguments += ' -bcmmPersistencePhase ' + $PersistencePhase }
$arguments += ' -bcmmRunToken ' + $runToken
$runStarted = Get-Date
$process = Start-Process -FilePath $executable -ArgumentList $arguments -WindowStyle Hidden -PassThru
$passed = $false
$console = $null
try {
 $deadline = (Get-Date).AddSeconds(60)
 while ((Get-Date) -lt $deadline) {
  $console = Get-ChildItem -LiteralPath (Join-Path $profile 'logs') -Recurse -Filter 'console.log' -ErrorAction SilentlyContinue | Where-Object LastWriteTime -GE $runStarted | Sort-Object LastWriteTime -Descending | Select-Object -First 1
  if ($console) {
   if (Select-String -LiteralPath $console.FullName -Pattern $success -Quiet) { $passed = $true; break }
   if (Select-String -LiteralPath $console.FullName -Pattern 'Script validation failed|Can.t compile.*script|BCMM_CHECK FAILED|Virtual Machine Exception|Could not initialize platform services' -Quiet) { break }
  }
  if ($process.HasExited) { break }
  Start-Sleep -Milliseconds 250
 }
} finally {
 if (!$process.HasExited) { Stop-Process -Id $process.Id }
}
if ($console) {
 Select-String -LiteralPath $console.FullName -Pattern 'BCMM_CHECK|validation successful|SCRIPT.*\(E\)|addons|Addon' | Select-Object -Last 20 | ForEach-Object { $_.Line }
 Write-Output ('Log: ' + $console.FullName)
}
if (!$passed) { throw 'Validation did not pass. Inspect the saved console log.' }
