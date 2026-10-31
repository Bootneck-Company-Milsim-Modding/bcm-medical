param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$run=Join-Path $root ('.checks/missing-physical-restart-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $run)
function Snapshot {
 @(Get-ChildItem @((Join-Path $root 'Scripts'),(Join-Path $root 'Tests/Runtime'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs')) -Recurse -File|Where-Object{$_.Name -notlike 'resourceDatabase.rdb*'}|Sort-Object FullName|ForEach-Object{$_.FullName+'|'+(Get-FileHash $_.FullName).Hash})+@((Get-FileHash $PSCommandPath).Hash,(Get-FileHash (Join-Path $PSScriptRoot 'Validate.ps1')).Hash)
}
$before=Snapshot;$before|Set-Content (Join-Path $run 'source-before.txt');$passed=$false;$failure=''
try{
 & (Join-Path $PSScriptRoot 'Validate.ps1') -FinitePersistenceRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -FinitePersistencePhase seed
 $original=Get-Content (Join-Path $run 'profile/BCMM_FinitePersistence.json') -Raw|ConvertFrom-Json
 & (Join-Path $PSScriptRoot 'Validate.ps1') -WorldPersistenceFaultRuntime -WorldFaultPhase seed -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -NativeSaveId $original.SaveId
 $saved=Get-Content (Join-Path $run 'profile/BCMM_MissingPhysicalRestart.json') -Raw|ConvertFrom-Json
 if(!$saved.SaveId -or $saved.SaveId -eq $original.SaveId){throw 'Missing-reference test did not commit a separate native save point'}
 & (Join-Path $PSScriptRoot 'Validate.ps1') -WorldPersistenceFaultRuntime -WorldFaultPhase verify -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $run -NativeSaveId $saved.SaveId
 $passed=$true
}catch{$failure=$_.Exception.Message}
$after=Snapshot;$after|Set-Content (Join-Path $run 'source-after.txt');$unchanged=@(Compare-Object $before $after).Count -eq 0
[ordered]@{Passed=($passed -and $unchanged);SourceUnchanged=$unchanged;Failure=$failure;Run=$run}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
Write-Output ('Unavailable physical-reference native restart evidence: '+$run)
if(!$passed -or !$unchanged){throw ('Unavailable physical-reference restart failed: '+$failure)}
