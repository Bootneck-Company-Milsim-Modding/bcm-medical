param([string]$ServerDirectory='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$game='C:/Program Files (x86)/Steam/steamapps/common/Arma Reforger'
if(!$ServerDirectory){$ServerDirectory=(Get-Content (Join-Path $root '../BCM NID AceCompat/.dedicated/server-location.txt') -Raw).Trim()}
$run=Join-Path $root ('.checks/knowledge-network-'+(Get-Date -Format 'yyyyMMdd-HHmmss-fff'))
New-Item -ItemType Directory -Path $run|Out-Null
$common='-gproj "'+$root+'/Tests/Runtime/addon.gproj" -addons B6C4ED1CA1260002 -addonsDir "'+$root+','+$root+'/Tests/Runtime,'+$game+'/addons" -noBackend -disableCrashReporter -noSound -noThrow -noFocus -forceUpdate -maxFPS 30 '
$processes=[Collections.Generic.List[Diagnostics.Process]]::new();$passed=$false;$failure=''
function Read-Log([string]$role){$log=Get-ChildItem (Join-Path $run $role) -Recurse -Filter console.log -ErrorAction SilentlyContinue|Select-Object -First 1;if($log){return Get-Content $log.FullName -Raw};return ''}
try{
 $launch=$common+'-server "worlds/Tests/TestFramework/Autotest_GameMode_Plain.ent" -bcmmCargoObserver 1 -bcmmBeta01KnowledgeServer 1 -profile "'+$run+'/server"'
 $processes.Add((Start-Process -FilePath (Join-Path $ServerDirectory 'ArmaReforgerServerDiag.exe') -ArgumentList $launch -WindowStyle Hidden -PassThru))
 $limit=(Get-Date).AddSeconds(185);$clients=$false
 while((Get-Date) -lt $limit){
  if($processes[0].HasExited){throw 'Knowledge fixture server exited before owner acceptance completed'}
  $server=Read-Log 'server'
  if(!$clients -and $server -match 'Starting RPL server'){
   foreach($role in @('owner-a','owner-b')){
    $launch=$common+'-client 127.0.0.1 -port 2001 -bcmmCargoObserver 1 -bcmmBeta01KnowledgeOwner 1 -profile "'+$run+'/'+$role+'" -window -screenWidth 960 -screenHeight 720 -posX -2000 -posY -2000'
    $processes.Add((Start-Process -FilePath (Join-Path $game 'ArmaReforgerSteamDiag.exe') -ArgumentList $launch -WindowStyle Hidden -PassThru))
   };$clients=$true
  }
  $a=Read-Log 'owner-a';$b=Read-Log 'owner-b';$combined=$server+$a+$b
  if($combined -match 'BCMM_KNOWLEDGE_NET_FAILED|Can.t compile.*script|Virtual Machine Exception|ReplicationError|RplLayoutError|RplLoadError|source code checksum does not match'){throw 'Knowledge network fixture failed; inspect logs'}
  if($a -match 'BCMM_KNOWLEDGE_NET_COMPLETE id=\d+ cycles=200 failures=0 invalidated=1' -and $b -match 'BCMM_KNOWLEDGE_NET_COMPLETE id=\d+ cycles=200 failures=0 invalidated=1'){$passed=$true;break}
  Start-Sleep -Milliseconds 500
 }
 if(!$passed){throw 'Knowledge owner/privacy acceptance did not complete with zero failures'}
}catch{$failure=$_.Exception.Message}finally{foreach($process in $processes){if(!$process.HasExited){Stop-Process -Id $process.Id}}}
[ordered]@{passed=$passed;failure=$failure;run=$run;scope='Two actual owners; only player 1 examines; 200 A-B-A cycles each; changed airway invalidates original finding'}|ConvertTo-Json|Set-Content (Join-Path $run 'result.json')
foreach($role in @('owner-a','owner-b')){(Read-Log $role)-split "`n"|Where-Object{$_ -match 'BCMM_KNOWLEDGE_NET_.*COMPLETE'}}
Write-Output $run
if(!$passed){throw $failure}
