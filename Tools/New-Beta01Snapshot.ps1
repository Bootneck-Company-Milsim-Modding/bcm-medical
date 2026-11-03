# Freeze resources for a native campaign; clients and late joiners compile the
# same files even while production repairs continue in the working checkout.
param([string]$Name = 'beta01')
$ErrorActionPreference = 'Stop'
if ($Name -notmatch '^[a-zA-Z0-9-]+$') { throw 'Use a simple snapshot name.' }
$root = Split-Path $PSScriptRoot -Parent
$snapshot = Join-Path $root ('.checks/' + $Name + '-snapshot-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $snapshot | Out-Null
foreach ($folder in @('Assets','Configs','Language','Prefabs','PrefabsEditable','Scripts','Sounds','UI','Tests','Tools')) {
 & robocopy (Join-Path $root $folder) (Join-Path $snapshot $folder) /E /XJ /R:0 /W:0 /NFL /NDL /NJH /NJS /NP | Out-Null
 if ($LASTEXITCODE -ge 8) { throw ('Snapshot failed: ' + $folder) }
}
Copy-Item -LiteralPath (Join-Path $root 'addon.gproj') -Destination $snapshot
Copy-Item -LiteralPath (Join-Path $root 'resourceDatabase.rdb') -Destination $snapshot
& robocopy (Join-Path $root 'Source/Animations') (Join-Path $snapshot 'Source/Animations') /E /XJ /R:0 /W:0 /NFL /NDL /NJH /NJS /NP | Out-Null
if ($LASTEXITCODE -ge 8) { throw 'Animation resource snapshot failed.' }
$files = @(Get-ChildItem $snapshot -Recurse -File | Sort-Object FullName | ForEach-Object {
 [ordered]@{path=$_.FullName.Substring($snapshot.Length+1);sha256=(Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash}
})
[ordered]@{source=$root;createdUtc=[DateTime]::UtcNow.ToString('o');files=$files} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $snapshot 'snapshot.json')
Write-Output $snapshot
