param([ValidateSet('Drag','LivingTransport','FluidLine')][string]$Case='FluidLine',[ValidateRange(0,1000)][int]$DelayMs=75,[ValidateRange(0,500)][int]$JitterMs=20,[ValidateRange(0,0.2)][double]$Loss=0.01,[ValidateRange(0,2147483647)][int]$Seed=101)
$ErrorActionPreference='Stop'
if($JitterMs -gt $DelayMs){throw 'Jitter must not exceed base delay'}
$root=Split-Path $PSScriptRoot -Parent
$marker=Get-Content (Join-Path $root '.checks/network-final-snapshot.json') -Raw|ConvertFrom-Json
$run=Join-Path $root ('.checks/impaired-'+$Case+'-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item $run -ItemType Directory|Out-Null
$original=Join-Path $marker.snapshot ('Tools/Validate-'+$Case+'Network.ps1')
$driver=Join-Path $marker.snapshot ('Tools/Validate-'+$Case+'ImpairedCurrent.ps1')
$source=Get-Content $original -Raw
$source=$source.Replace('-client 127.0.0.1 -port ''+$port+''','-client 127.0.0.1:23001').Replace('-client 127.0.0.1 -bcmm','-client 127.0.0.1:23001 -bcmm')
if(!$source.Contains('-client 127.0.0.1:23001') -or $source.Contains('-client 127.0.0.1 -port')){throw 'Cannot establish explicit proxy endpoint'}
Set-Content $driver $source
# Keep the exact diagnostic sources for each future attempt, including failures.
$diagnosticRoot=Join-Path $marker.snapshot 'Tests/Runtime'
Copy-Item -LiteralPath $diagnosticRoot -Destination (Join-Path $run 'diagnostic-source') -Recurse
Copy-Item -LiteralPath $driver -Destination (Join-Path $run 'driver-source.ps1')
function DiagnosticManifest { @(Get-ChildItem -LiteralPath $diagnosticRoot -Recurse -File | Sort-Object FullName | ForEach-Object { $_.FullName.Substring($diagnosticRoot.Length)+'|'+(Get-FileHash -LiteralPath $_.FullName).Hash }) }
$diagnosticBefore=DiagnosticManifest
$diagnosticBefore|Set-Content (Join-Path $run 'diagnostic-manifest.txt')
$stats=Join-Path $run 'proxy-stats.json'
$python='C:\Users\ruanb\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
$proxyFile=Join-Path $PSScriptRoot 'Beta01UdpProxy.py'
$arguments='"'+$proxyFile+'" --stats "'+$stats+'" --seconds 480 --delay-ms '+$DelayMs+' --jitter-ms '+$JitterMs+' --loss '+$Loss.ToString([Globalization.CultureInfo]::InvariantCulture)+' --seed '+$Seed
$proxy=Start-Process $python -ArgumentList $arguments -WindowStyle Hidden -PassThru -RedirectStandardError (Join-Path $run 'proxy-stderr.txt')
$passed=$false;$failure='';$traffic=$null
try{
 $server=(Get-Content (Join-Path $root '../BCM NID AceCompat/.dedicated/server-location.txt') -Raw).Trim()
 $params=@{ServerDirectory=$server};if($Case -eq 'Drag'){$params.ProductionDirectory=$marker.production}
 & $driver @params *> (Join-Path $run 'driver.txt')
 if(!$?){throw 'Native network driver failed'}
 if(@(Compare-Object $diagnosticBefore (DiagnosticManifest)).Count){throw 'Diagnostic source changed during the native run'}
 $traffic=Get-Content $stats -Raw|ConvertFrom-Json
 if($traffic.peers -lt 3 -or $traffic.forwarded -lt 100 -or ($Loss -gt 0 -and $traffic.dropped -lt 1) -or $traffic.errors.Count){throw 'Required impaired traffic was not established cleanly'}
 $passed=$true
}catch{$failure=$_.Exception.Message}finally{if(!$proxy.HasExited){Stop-Process -Id $proxy.Id}}
if(Test-Path $stats){$traffic=Get-Content $stats -Raw|ConvertFrom-Json}
[ordered]@{passed=$passed;case=$Case;failure=$failure;run=$run;packedSha256=$marker.sha256;driverSha256=(Get-FileHash (Join-Path $run 'driver-source.ps1')).Hash;diagnosticManifestSha256=(Get-FileHash (Join-Path $run 'diagnostic-manifest.txt')).Hash;proxySha256=(Get-FileHash $proxyFile).Hash;traffic=$traffic}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $run 'result.json')
Write-Output $run
if(!$passed){throw $failure}
