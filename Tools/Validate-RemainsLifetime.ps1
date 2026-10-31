param([Parameter(Mandatory)][string]$ServerDirectory,[switch]$FullDuration)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$run=Join-Path $root ('.checks\remains-lifetime-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$addon=Join-Path $run 'addon';$scripts=Join-Path $addon 'Scripts\Game'
New-Item -ItemType Directory -Path $scripts -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $root 'Tests\Vehicle\BCMM_RemainsLifetimeChecks.c') -Destination $scripts
[IO.File]::WriteAllText((Join-Path $addon 'addon.gproj'),'GameProject { ID "BCMMRemainsChecks" GUID "B6C4ED1CA1260005" TITLE "Medical remains lifetime checks" Dependencies { "6AB4B3F3FD215919" } }',[Text.UTF8Encoding]::new($false))
$game='C:\Program Files (x86)\Steam\steamapps\common\Arma Reforger'
$dirs=$addon+','+$root+','+(Join-Path $game 'addons')
$arguments='-gproj "'+$addon+'\addon.gproj" -addons B6C4ED1CA1260005 -addonsDir "'+$dirs+'" -server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -profile "'+$run+'" -noBackend -disableCrashReporter -noSound -noThrow -maxFPS 30'
if($FullDuration){$arguments+=' -bcmmFullLifetime 1'}
$manifest=[ordered]@{FullDuration=[bool]$FullDuration;RunnerHash=(Get-FileHash $PSCommandPath).Hash;Sources=@(Get-ChildItem (Join-Path $root 'Scripts') -Recurse -File -Filter '*.c' | Sort-Object FullName | ForEach-Object{[ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash $_.FullName).Hash}});HarnessHash=(Get-FileHash (Join-Path $scripts 'BCMM_RemainsLifetimeChecks.c')).Hash}
[IO.File]::WriteAllText((Join-Path $run 'validation-manifest.json'),($manifest|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$process=Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $arguments -WindowStyle Hidden -PassThru
Write-Output "Medical remains PID=$($process.Id) Logs=$run"
$passed=$false;$last=''
try{
 $seconds=60;if($FullDuration){$seconds=1900};$deadline=(Get-Date).AddSeconds($seconds)
 while((Get-Date) -lt $deadline){
  $file=Get-ChildItem (Join-Path $run 'logs') -Recurse -Filter console.log -ErrorAction SilentlyContinue|Select-Object -First 1
  if($file){
   $text=Get-Content $file.FullName -Raw
   $progress=@($text -split "`n"|Where-Object{$_ -match 'BCMM_PROTECTION_'}) -join "`n"
   if($progress -ne $last){Write-Output $progress.Substring($last.Length);$last=$progress}
   if($text -match 'SCRIPT\s+\(E\)|RPL\s+\(E\)|Virtual Machine Exception|Can.t compile.*script|ENGINE\s+\(F\)|Application crashed'){Get-Content $file.FullName -Tail 20;break}
   if($text -match 'BCMM_PROTECTION_COMPLETE checks=\d+ failures=0'){$passed=$true;break}
   if($text -match 'BCMM_PROTECTION_COMPLETE'){break}
  }
  if($process.HasExited){break};Start-Sleep -Milliseconds 500
 }
}finally{if(!$process.HasExited){Stop-Process -Id $process.Id}}
[ordered]@{Passed=$passed;FullDuration=[bool]$FullDuration;RunDirectory=$run}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
if(!$passed){throw "Medical remains lifetime checks failed; inspect $run"}

