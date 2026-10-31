"""Original reproducible BCM native UI layout. Base-game fonts/handlers are references."""
from pathlib import Path
root=Path(__file__).resolve().parent.parent
folder=root/"UI/layouts/BCMM"
font='{EABA4FE9D014CCEF}UI/Fonts/RobotoCondensed/RobotoCondensed_Bold.fnt'
button='{6AB4B3F3FD215006}UI/layouts/BCMM/MedicalButton.layout'
def slot(a):
    return 'Slot FrameWidgetSlot { Anchor '+' '.join(str(v) for v in a)+' OffsetLeft 0 OffsetTop 0 OffsetRight 0 OffsetBottom 0 }'
def image(name,a,color,texture=None):
    return 'ImageWidgetClass { Name "'+name+'" "Ignore Cursor" 1 '+slot(a)+' Color '+color+(' Texture "'+texture+'"' if texture else '')+' }'
def text(name,a,value,size=16,color='0.84 0.87 0.86 1'):
    return 'TextWidgetClass { Name "'+name+'" "Ignore Cursor" 1 '+slot(a)+' Text "'+value+'" "Font Size" '+str(size)+' Color '+color+' FontProperties FontProperties { Font "'+font+'" } }'
def btn(name,a,toggle=False):
    return 'ButtonWidgetClass : "'+button+'" { Name "'+name+'" '+slot(a)+(' components { SCR_ButtonTextComponent "{5548F0143AE99095}" { m_bCanBeToggled 1 } }' if toggle else '')+' }'
base='''ButtonWidgetClass : "{75C912A1C89BE6C2}UI/layouts/WidgetLibrary/Buttons/WLib_ButtonText.layout" {
 Name "BCMMButton"
 components { SCR_ButtonTextComponent "{5548F0143AE99095}" { m_bCanBeToggled 0 m_bShowBorderOnFocus 1 m_bShowBorderOnHover 1 } }
 {
  SizeLayoutWidgetClass "{5548EB41A8E48247}" { Prefab "{5548EB41A8E48247}" {
   OverlayWidgetClass "{5548EB41A8E48245}" { Prefab "{5548EB41A8E48245}" {
    TextWidgetClass "{5548F014E72FB9DA}" { Prefab "{5548F014E72FB9DA}"
     Slot OverlayWidgetSlot { HorizontalAlign 3 VerticalAlign 3 Padding 8 2 8 2 }
     "Horizontal Alignment" Center "Vertical Alignment" Center "Min Font Size" 12 "Font Size" 16
    }
   } }
  } }
 }
}
'''
(folder/'MedicalButton.layout').write_text(base,encoding='utf-8')
(folder/'MedicalButton.layout.meta').write_text('MetaFileClass { Name "'+button+'" Configurations { LayoutResourceClass PC {} LayoutResourceClass HEADLESS : PC {} } }',encoding='utf-8')
parts=['FrameWidgetClass { Name "BCMMMedicalRoot" {',
 image('Dim',(0,0,1,1),'0 0 0 0.20'),
 'FrameWidgetClass { Name "Panel" '+slot((.24,.19,.76,.81))+' {',
 image('Background',(0,0,1,1),'0.035 0.052 0.058 0.98'),
 image('GoldRule',(0,0,1,.003),'0.68 0.56 0.26 1'),
 image('HeaderShade',(0,.005,1,.13),'0.065 0.085 0.09 1'),
 image('CompanyLogo',(.0,.018,.14,.119),'1 1 1 1','{6AB4B3F3FD215005}UI/Textures/BCMM/CompanyLogo.edds'),
 text('MenuTitle',(.13,.028,.7,.075),'BCM MEDICAL',23),
 text('MenuSubtitle',(.13,.08,.75,.112),'CASUALTY ASSESSMENT',12,'0.56 0.63 0.63 1'),
 btn('Close',(.80,.035,.975,.106))]
tabs=['TabCirculation','TabAirway','TabSurgery','TabMedication','TabBlood','SwapPatient']
for i,name in enumerate(tabs):
    x=.025+i*.159
    parts.append(btn(name,(x,.15,x+.15,.223),name!='SwapPatient'))
parts.extend([
 text('RegionLabel',(.025,.237,.68,.275),'Select body region',17,'0.81 0.7 0.4 1'),
 image('ContentRule',(.025,.282,.975,.284),'1 1 1 0.09'),
 image('LeftRule',(.37,.308,.371,.91),'1 1 1 0.07'),
 image('RightRule',(.69,.308,.691,.91),'1 1 1 0.07'),
 text('StatusHeading',(.025,.303,.35,.343),'STATUS',16),
 text('Assessment',(.025,.35,.35,.49),'',16),
 text('TreatmentHeading',(.025,.508,.35,.548),'TREATMENT',16)])
for i in range(6):
    y=.565+i*.049
    parts.append(btn('Action'+str(i),(.025,y,.35,y+.042)))
# Legacy binding slots remain available but are always hidden by the controller.
for i in range(6,10):
    parts.append('ButtonWidgetClass : "'+button+'" { Name "Action'+str(i)+'" "Is Visible" 0 '+slot((.025,.565,.35,.607))+' }')
parts.extend([btn('MoreActions',(.025,.865,.35,.907)),text('ActionPage',(.025,.915,.35,.935),'',12,'0.5 0.58 0.59 1')])
parts.append(text('AnatomyHint',(.385,.312,.675,.355),'CLICK A BODY PART',12,'0.96 0.79 0.22 1'))
parts.append('FrameWidgetClass { Name "AnatomyPanel" '+slot((.53,.652,.53,.652))+' {')
parts.append(image('Anatomy',(0,0,1,1),'0.86 0.88 0.86 1','{6AB4B3F3FD215004}UI/Textures/BCMM/Anatomy.edds'))
regions=[(.40,.015,.60,.18),(.34,.18,.66,.35),(.35,.35,.65,.51),(.65,.20,.83,.58),(.17,.20,.35,.58),(.51,.51,.70,.99),(.30,.51,.49,.99)]
for i,a in enumerate(regions):
    marker='TextWidgetClass { Name "RegionMarker'+str(i)+'" "Ignore Cursor" 1 Slot OverlayWidgetSlot { HorizontalAlign 1 VerticalAlign 1 } Color 0.75 0.16 0.07 1 "Font Size" 13 "Min Font Size" 10 FontProperties FontProperties { Font "'+font+'" } }'
    content='OverlayWidgetClass { Name "RegionContents'+str(i)+'" Slot ButtonWidgetSlot { HorizontalAlign 3 VerticalAlign 3 } { ImageWidgetClass { Name "RegionHighlight'+str(i)+'" "Ignore Cursor" 1 Slot OverlayWidgetSlot { HorizontalAlign 3 VerticalAlign 3 } Color 1 0.80 0.16 0.8 } '+marker+' } }'
    parts.append('ButtonWidgetClass { Name "Region'+str(i)+'" style blank Color 1 1 1 1 '+slot(a)+' components { BCMM_AnatomyRegion { m_Region '+str(i)+' } } { '+content+' } }')
parts.extend(['} }',
 text('PatientsHeading',(.72,.303,.975,.343),'PATIENTS',16),
 text('TreatingLabel',(.72,.355,.975,.383),'CURRENT PATIENT',12,'0.52 0.61 0.62 1'),
 text('PatientName',(.72,.39,.975,.445),'',18,'0.87 0.76 0.48 1')])
parts.extend([
 text('RosterHeading',(.72,.472,.975,.51),'MORE PATIENTS',13),
 text('RosterStatus',(.72,.523,.975,.61),'Other occupants in your vehicle appear here.',13,'0.56 0.63 0.63 1'),
 'ScrollLayoutWidgetClass { Name "PatientScroll" '+slot((.72,.525,.975,.81))+' { VerticalLayoutWidgetClass { Name "PatientList" } } }',
 image('CheckProgressTrack',(.025,.286,.69,.289),'1 1 1 0.08'),
 image('CheckProgressFill',(.025,.286,.025,.286),'0.94 0.69 0.28 1'),
 btn('TransportCarry',(.72,.895,.842,.932)),
 btn('TransportDrag',(.85,.895,.975,.932)),
 text('TransportStatus',(.72,.825,.975,.885),'',11,'0.62 0.71 0.70 1'),
 image('FooterRule',(.025,.942,.975,.944),'1 1 1 0.09'),
 text('TreatmentStatus',(.025,.957,.69,.991),'Ready',14,'0.62 0.71 0.70 1'),
 btn('CancelProcedure',(.72,.953,.975,.992)),image('ProcedureTrack',(.025,.938,.69,.941),'1 1 1 0.07'),
 image('ProcedureFill',(.025,.938,.025,.938),'0.76 0.63 0.3 1'),
 '} } } }'])
(folder/'MedicalPanel.layout').write_text('\n'.join(parts)+'\n',encoding='utf-8')
row='''ButtonWidgetClass : "{6AB4B3F3FD215006}UI/layouts/BCMM/MedicalButton.layout" {
 Name "PatientRow"
 Slot LayoutSlot { HorizontalAlign 3 Padding 0 0 0 5 }
 { SizeLayoutWidgetClass "{5548EB41A8E48247}" { Prefab "{5548EB41A8E48247}" AllowHeightOverride 1 HeightOverride 34 } }
}
'''
(folder/'PatientRow.layout').write_text(row,encoding='utf-8')
(folder/'PatientRow.layout.meta').write_text('MetaFileClass { Name "{6AB4B3F3FD215007}UI/layouts/BCMM/PatientRow.layout" Configurations { LayoutResourceClass PC {} LayoutResourceClass HEADLESS : PC {} } }',encoding='utf-8')
print("Built six native category/swap controls, separated anatomy prompt and scrolling vehicle roster")
