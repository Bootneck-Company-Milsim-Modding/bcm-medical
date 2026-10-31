param([string]$ServerDirectory='', [switch]$FullHeal)
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$run=Join-Path $root ('.checks/retrieved-stock-saline-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Force -Path $run | Out-Null
function Snapshot {
 @(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Configs'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'UI'),(Join-Path $root 'Language'),(Join-Path $root 'Tools')) -Recurse -File | Where-Object {$_.Name -notlike 'resourceDatabase.rdb*'} | Sort-Object FullName | ForEach-Object {$_.FullName+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash})+@((Get-FileHash (Join-Path $root 'addon.gproj')).Hash)
}
$before=Snapshot;$before | Set-Content (Join-Path $run 'source-before.txt');$passed=$false;$failure=''
try {
 & (Join-Path $PSScriptRoot 'Validate.ps1') -RetrievedSalineRuntime -RetrievedSalineFullHeal:$FullHeal -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run
 Start-Sleep -Seconds 5
 $saved=Get-Content (Join-Path $run 'profile/BCMM_StockSalineWorld.json') -Raw | ConvertFrom-Json
 if(!$saved.SaveId){throw 'Native stock-saline fixture did not commit a save UUID'}
 & (Join-Path $PSScriptRoot 'Validate.ps1') -RetrievedSalineRestartRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -NativeSaveId $saved.SaveId
 $passed=$true
}catch{$failure=$_.Exception.Message}
$after=Snapshot;$after | Set-Content (Join-Path $run 'source-after.txt');$unchanged=@(Compare-Object $before $after).Count -eq 0
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;FullHeal=[bool]$FullHeal;Failure=$failure;Run=$run;Scope='Actual retrieved used original source/holder/patient UUIDs, retained expiry, unused-bag control and optional FullHeal; no custom finite-mL or dedicated owner/backend claim'} | ConvertTo-Json | Set-Content (Join-Path $run 'result.json')
Write-Output ('Native stock-saline evidence: '+$run);if(!$passed -or !$unchanged){throw ('Native stock-saline acceptance failed: '+$failure)}




