$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$catalog = @(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'SettingsCatalog.json') -Raw | ConvertFrom-Json)
$culture = [Globalization.CultureInfo]::InvariantCulture
function Number($value) { return ([double]$value).ToString('0.########', $culture) }
$code = [Text.StringBuilder]::new()
[void]$code.AppendLine('// Generated from Tools/SettingsCatalog.json. Original BCM settings catalogue.')
[void]$code.AppendLine('class BCMM_SettingsCatalog { static void Build(notnull array<ref BCMM_SettingDefinition> definitions) {')
$config = [Text.StringBuilder]::new()
[void]$config.AppendLine('SCR_EditorAttributeList { m_aAttributes {')
$keys = @{}
foreach ($entry in $catalog) {
 if ($keys.ContainsKey($entry.key)) { throw ('Duplicate setting: ' + $entry.key) }
 $keys[$entry.key] = $true
 $key = $entry.key
 if ($key -notmatch '^[A-Za-z][A-Za-z0-9]+$') { throw 'Unsafe settings key' }
 $toggle = 'false'
 if ($entry.toggle) { $toggle = 'true' }
 [void]$code.AppendLine(('definitions.Insert(new BCMM_SettingDefinition("{0}", {1}, {2}, {3}, {4}, {5}));' -f $key,(Number $entry.default),(Number $entry.min),(Number $entry.max),(Number $entry.step),$toggle))
 $baseClass = 'BCMM_NumberSettingAttribute'
 $nativeLayout = '{680E4985E42137FB}UI/layouts/Editor/Attributes/AttributePrefabs/AttributePrefab_Slider.layout'
 if ($entry.toggle) { $baseClass = 'BCMM_ToggleSettingAttribute'; $nativeLayout = '{D0F3CE0C63A5AEBB}UI/layouts/Editor/Attributes/AttributePrefabs/AttributePrefab_Checkbox.layout' }
 $className = 'BCMM_Setting_' + $key
 [void]$config.AppendLine(('{0} {{ m_Key "{1}" m_UIInfo SCR_EditorAttributeUIInfo {{ Name "{2}" Description "Server-persistent BCM Medical setting. {3}. Features still in development may not yet use this value." }} m_CategoryConfig "{{B6C4ED1CA1260101}}Configs/Editor/BCMMedical.conf" m_Layout "{4}"' -f $className,$key,$entry.label,$entry.group,$nativeLayout))
 if (!$entry.toggle) { [void]$config.AppendLine(('m_baseValues SCR_EditorAttributeBaseValues {{ m_fMin {0} m_fMax {1} m_fStep {2} m_iDecimals 2 }}' -f (Number $entry.min),(Number $entry.max),(Number $entry.step))) }
 [void]$config.AppendLine('}')
}
[void]$code.AppendLine('} }')
foreach($entry in $catalog) {
 $baseClass = 'BCMM_NumberSettingAttribute'
 if ($entry.toggle) { $baseClass = 'BCMM_ToggleSettingAttribute' }
 [void]$code.AppendLine(('[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()] class BCMM_Setting_{0} : {1} {{}}' -f $entry.key,$baseClass))
}
[void]$config.AppendLine('} }')
$scriptDir = Join-Path $projectRoot 'Scripts/Game/BCM_Medical'
$configDir = Join-Path $projectRoot 'Configs/Editor/AttributeLists'
New-Item -ItemType Directory -Force -Path $configDir | Out-Null
[IO.File]::WriteAllText((Join-Path $scriptDir 'BCMM_SettingsGenerated.c'),$code.ToString())
[IO.File]::WriteAllText((Join-Path $configDir 'Edit.conf'),$config.ToString())
Write-Output ('Generated ' + $catalog.Count + ' settings with one concrete GM attribute per key.')
