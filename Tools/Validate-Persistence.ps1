# Separate dedicated-server processes must agree on saved BCM configuration.
param([string]$ServerDirectory = '')
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$profile = Join-Path $root ('.checks/persistence-current-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
foreach ($phase in @('seed','verify')) {
    & (Join-Path $PSScriptRoot 'Validate.ps1') -ServerRuntime -ServerDirectory $ServerDirectory -ProfileDirectory $profile -PersistencePhase $phase
    if (!$?) { throw ('Persistence phase failed: ' + $phase) }
}
Write-Output ('Separate-process settings restart passed: ' + $profile)