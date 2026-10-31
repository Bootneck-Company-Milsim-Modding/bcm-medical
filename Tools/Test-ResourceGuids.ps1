param([string]$ProjectRoot=(Split-Path $PSScriptRoot -Parent),[string]$ReportPath='')
$ErrorActionPreference='Stop'
$definitions=@(Get-ChildItem -LiteralPath $ProjectRoot -Directory | Where-Object { $_.Name -notmatch '^\.' -and $_.Name -ne 'Tests' } | ForEach-Object {
 Get-ChildItem -LiteralPath $_.FullName -Recurse -File -Filter '*.meta' | ForEach-Object {
  $metadata=[IO.File]::ReadAllText($_.FullName)
  if($metadata -match 'Name\s+"\{([0-9A-Fa-f]{16})\}([^"\r\n]+)"'){
   [pscustomobject]@{Guid=$matches[1].ToUpperInvariant();Resource=$matches[2];Metadata=$_.FullName.Substring($ProjectRoot.Length+1)}
  }
 }
})
$duplicates=@($definitions | Group-Object Guid | Where-Object Count -gt 1 | ForEach-Object { [ordered]@{Guid=$_.Name;Definitions=@($_.Group)} })
$resourceMap=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach($definition in $definitions){$resourceMap[$definition.Resource.Replace('\','/')]=$definition.Guid}
$referenceMismatches=@()
$sourceFolders=@(Get-ChildItem -LiteralPath $ProjectRoot -Directory | Where-Object {$_.Name -notmatch '^\.' -and $_.Name -ne 'Tests'} | ForEach-Object FullName)
$diagnosticSources=Join-Path $ProjectRoot 'Tests/Runtime';if(Test-Path -LiteralPath $diagnosticSources){$sourceFolders+=$diagnosticSources}
foreach($file in (Get-ChildItem -LiteralPath $sourceFolders -Recurse -File | Where-Object Extension -in '.c','.conf','.et','.layout','.st','.gproj')){
 $content=[IO.File]::ReadAllText($file.FullName)
 foreach($reference in [regex]::Matches($content,'"\{([0-9A-Fa-f]{16})\}([^"\r\n]+)"')){
  $resourcePath=$reference.Groups[2].Value.Replace('\','/');$actualGuid=$reference.Groups[1].Value.ToUpperInvariant()
  if($resourceMap.ContainsKey($resourcePath) -and $resourceMap[$resourcePath] -ne $actualGuid){$referenceMismatches+=[ordered]@{File=$file.FullName.Substring($ProjectRoot.Length+1);Resource=$resourcePath;ReferencedGuid=$actualGuid;ExpectedGuid=$resourceMap[$resourcePath]}}
 }
}
$report=[ordered]@{Project='BCM Medical';ResourceCount=$definitions.Count;Passed=($duplicates.Count -eq 0 -and $referenceMismatches.Count -eq 0);Duplicates=$duplicates;ReferenceMismatches=$referenceMismatches}
if($ReportPath){[IO.File]::WriteAllText($ReportPath,($report|ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))}
if($duplicates.Count){$duplicates|ConvertTo-Json -Depth 6|Write-Output;throw 'Duplicate runtime resource GUIDs must be corrected before native validation'}
if($referenceMismatches.Count){$referenceMismatches|ConvertTo-Json -Depth 5|Write-Output;throw 'Stale GUID references to owned runtime resources must be corrected before native validation'}
Write-Output "Medical resource GUID audit passed: $($definitions.Count) unique resources"
