param([string]$ServerDirectory='', [string]$SourceProfile='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
if(!$SourceProfile){$SourceProfile=Join-Path $root '.checks/fluid-line-20261029-202308-576/profile'}
$SourceProfile=[IO.Path]::GetFullPath($SourceProfile)
if(!(Test-Path -LiteralPath (Join-Path $SourceProfile 'BCMM_FluidLine.json'))){throw 'Provide an actual accepted pre-overload fluid-line saved-world profile.'}
$run=Join-Path $root ('.checks/fluid-burden-legacy-world-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $run | Out-Null
function ProfileSnapshot { @(Get-ChildItem -LiteralPath $SourceProfile -Recurse -File | Sort-Object FullName | ForEach-Object { $_.FullName.Substring($SourceProfile.Length+1)+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash }) }
function SourceSnapshot { @(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Configs'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Tools')) -Recurse -File | Where-Object {$_.Name -notlike 'resourceDatabase.rdb*'} | Sort-Object FullName | ForEach-Object {$_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash}) }
$beforeProfile=ProfileSnapshot;$beforeSource=SourceSnapshot;$passed=$false;$failure=''
try {
 Copy-Item -LiteralPath $SourceProfile -Destination (Join-Path $run 'profile') -Recurse
 $saved=Get-Content (Join-Path $run 'profile/BCMM_FluidLine.json') -Raw|ConvertFrom-Json
 if(!$saved.SaveId){throw 'Legacy original manifest contains no actual native save UUID.'}
 & (Join-Path $PSScriptRoot 'Validate.ps1') -FluidLineRestartRuntime -LegacyFluidBurdenWorldRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -NativeSaveId $saved.SaveId
 $passed=$true
} catch {$failure=$_.Exception.Message}
$afterProfile=ProfileSnapshot;$afterSource=SourceSnapshot
$historicalUnchanged=@(Compare-Object $beforeProfile $afterProfile).Count -eq 0
$sourceUnchanged=@(Compare-Object $beforeSource $afterSource).Count -eq 0
$beforeProfile|Set-Content (Join-Path $run 'original-profile-before.txt');$afterProfile|Set-Content (Join-Path $run 'original-profile-after.txt');$beforeSource|Set-Content (Join-Path $run 'source-before.txt');$afterSource|Set-Content (Join-Path $run 'source-after.txt')
[ordered]@{Passed=($passed -and $historicalUnchanged -and $sourceUnchanged);SourceUnchanged=$sourceUnchanged;HistoricalProfileUnchanged=$historicalUnchanged;OriginalProfile=$SourceProfile;Failure=$failure;Run=$run;Scope='Copy of an actual pre-overload native patient/finite-line world loads with zero optional burden and default disabled policy; original historical profile remains unchanged'}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output ('Pre-overload native-world evidence: '+$run)
if(!$passed -or !$historicalUnchanged -or !$sourceUnchanged){throw ('Pre-overload world acceptance failed: '+$failure)}
