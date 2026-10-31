param([string]$ServerDirectory='', [ValidateSet('Jeep','Truck','Helicopter','Ambulance')][string]$CargoVehicle='Truck')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$profile=Join-Path $root ('.checks/finite-persistence-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
[void](New-Item -ItemType Directory -Path $profile)
$files=@(Get-ChildItem -LiteralPath @((Join-Path $root 'Scripts'),(Join-Path $root 'Prefabs'),(Join-Path $root 'PrefabsEditable'),(Join-Path $root 'Configs'),(Join-Path $root 'Tests/Runtime')) -Recurse -File|Where-Object{$_.Name -notlike 'resourceDatabase.rdb*'})+@(Get-Item (Join-Path $root 'addon.gproj'),(Join-Path $root 'Tools/Validate.ps1'),$PSCommandPath)
$snapshot=@($files|Sort-Object FullName -Unique|ForEach-Object{[ordered]@{Path=$_.FullName.Substring($root.Length+1);SHA256=(Get-FileHash $_.FullName).Hash}})
$snapshot|ConvertTo-Json|Set-Content (Join-Path $profile 'source-before.json')
& (Join-Path $PSScriptRoot 'Validate.ps1') -FinitePersistenceRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $profile -FinitePersistencePhase seed -CargoVehicle $CargoVehicle
$manifestPath=Join-Path $profile 'profile/BCMM_FinitePersistence.json'
$manifest=Get-Content -LiteralPath $manifestPath -Raw|ConvertFrom-Json
if(!$manifest.SaveId -or $manifest.Ids.Count -ne 3){throw 'Missing native finite-supply restart manifest'}
& (Join-Path $PSScriptRoot 'Validate.ps1') -FinitePersistenceRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $profile -FinitePersistencePhase verify -NativeSaveId $manifest.SaveId -CargoVehicle $CargoVehicle
$disposed=Get-Content -LiteralPath $manifestPath -Raw|ConvertFrom-Json
& (Join-Path $PSScriptRoot 'Validate.ps1') -FinitePersistenceRuntime -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $profile -FinitePersistencePhase verify-loss -NativeSaveId $disposed.SaveId -CargoVehicle $CargoVehicle
$unchanged=$true;foreach($source in $snapshot){$path=Join-Path $root $source.Path;if(!(Test-Path $path) -or (Get-FileHash $path).Hash -ne $source.SHA256){$unchanged=$false}}
$logs=@(Get-ChildItem (Join-Path $profile 'logs') -Recurse -Filter console.log|Sort-Object FullName)
$checks=@($logs|ForEach-Object{[regex]::Match((Get-Content $_.FullName -Raw),'BCMM_CHECK COMPLETE checks=\d+ failures=\d+').Value})
[ordered]@{CargoVehicle=$CargoVehicle;Passed=$unchanged;SourceUnchanged=$unchanged;SaveId=$manifest.SaveId;DisposedSaveId=$disposed.SaveId;Items=$manifest.Ids;Checks=$checks;Run=$profile}|ConvertTo-Json -Depth 5|Set-Content (Join-Path $profile 'result.json')
if(!$unchanged){throw 'Finite persistence source changed between seed and native restart'}
Write-Output ('Native finite-supply world restart passed: '+$profile)
