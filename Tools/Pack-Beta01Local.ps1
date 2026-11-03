# Local packaging only. This command never publishes to Workshop.
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$run=Join-Path $root ('.checks/beta01-packed-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
$source=Join-Path $run 'source';$output=Join-Path $run 'packed';$profile=Join-Path $run 'workbench-profile'
New-Item -ItemType Directory -Path $source,$output,$profile|Out-Null
foreach($folder in @('Assets','Configs','Language','Prefabs','PrefabsEditable','Scripts','Sounds','UI','Source/Animations')){
 & robocopy (Join-Path $root $folder) (Join-Path $source $folder) /E /XJ /R:0 /W:0 /NFL /NDL /NJH /NJS /NP | Out-Null
 if($LASTEXITCODE -ge 8){throw ('Packaging snapshot failed: '+$folder)}
}
Copy-Item addon.gproj,resourceDatabase.rdb -Destination $source
Get-ChildItem $source -Recurse -File|Sort-Object FullName|ForEach-Object {[ordered]@{path=$_.FullName.Substring($source.Length+1);sha256=(Get-FileHash $_.FullName).Hash}}|ConvertTo-Json|Set-Content (Join-Path $run 'source-manifest.json')
$exe='C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger Tools/Workbench/ArmaReforgerWorkbenchSteamDiag.exe'
$arguments='-gproj "'+$source+'/addon.gproj" -profile "'+$profile+'" -addonsDir "'+$source+',C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger/addons" -wbModule=ResourceManager -packAddon -packAddonDir "'+$output+'"'
$process=Start-Process -FilePath $exe -ArgumentList $arguments -WindowStyle Hidden -PassThru
[ordered]@{pid=$process.Id;run=$run;source=$source;output=$output;profile=$profile;startedUtc=[DateTime]::UtcNow.ToString('o');scope='Local runtime-only packaging; no diagnostic addon, tests, docs or tools; no Workshop publishing'}|ConvertTo-Json|Set-Content (Join-Path $root '.checks/beta01-pack-process.json')
Write-Output $run
