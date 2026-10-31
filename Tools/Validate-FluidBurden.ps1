param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$run=Join-Path $root ('.checks/fluid-burden-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Force -Path $run | Out-Null
function Snapshot {
 @(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Configs'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Tools')) -Recurse -File | Where-Object {$_.Name -notlike 'resourceDatabase.rdb*'} | Sort-Object FullName | ForEach-Object {$_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash})+@((Get-FileHash (Join-Path $root 'addon.gproj')).Hash)
}
$before=Snapshot;$before | Set-Content (Join-Path $run 'source-before.txt');$passed=$false;$failure=''
try {
 & (Join-Path $PSScriptRoot 'Validate.ps1') -FluidBurdenRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run
 Start-Sleep -Seconds 5
 $saved=Get-Content (Join-Path $run 'profile/BCMM_FluidBurden.json') -Raw | ConvertFrom-Json
 if(!$saved.SaveId){throw 'Native active-line fixture did not commit a save UUID'}
 & (Join-Path $PSScriptRoot 'Validate.ps1') -FluidBurdenRestartRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -NativeSaveId $saved.SaveId
 $passed=$true
}catch{$failure=$_.Exception.Message}
$after=Snapshot;$after | Set-Content (Join-Path $run 'source-after.txt');$unchanged=@(Compare-Object $before $after).Count -eq 0
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;Failure=$failure;Run=$run;Scope='Opt-in redistributed burden, typed finite water gates, native breathing, active oxygen/whole-blood line and separate native world restart without offline clearance'} | ConvertTo-Json | Set-Content (Join-Path $run 'result.json')
Write-Output ('Fluid burden evidence: '+$run);if(!$passed -or !$unchanged){throw ('Fluid burden acceptance failed: '+$failure)}
