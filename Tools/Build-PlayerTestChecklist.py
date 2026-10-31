"""Build the editable human test workbook from its reviewed Markdown source.

Use the bundled Codex Python runtime with python-docx. Native render/review is
performed separately; this script never marks gameplay tests as passed.
"""
import hashlib
import json
import re
from pathlib import Path
from collections import OrderedDict
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_TAB_ALIGNMENT
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_CELL_VERTICAL_ALIGNMENT
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'Docs/Training'
SOURCE = OUT / 'BCM Medical Player Test Checklist.md'
TARGET = OUT / 'BCM Medical Player Test Checklist.docx'
QA = ROOT / '.checks/player-test-checklist-20261031'
QA.mkdir(parents=True, exist_ok=True)
catalog = json.loads((ROOT/'Tools/SettingsCatalog.json').read_text(encoding='utf-8-sig'))
text = SOURCE.read_text(encoding='utf-8-sig').split('<!-- GENERATED SETTINGS WORKSHEET -->')[0]
groups = ['Bleeding', 'Airway and breathing', 'Circulation', 'Resuscitation',
          'Surgery', 'Fractures', 'Medical interactions', 'Body bags', 'Transport',
          'Suppression', 'Lives', 'Focus', 'Pain', 'Medication', 'Arsenal']
routes = {'Bleeding':'WOUND', 'Airway and breathing':'AIR OXY', 'Circulation':'FLUID MON',
          'Resuscitation':'CPR DEATH', 'Surgery':'SURG', 'Fractures':'LEG',
          'Medical interactions':'TXN GM03 SLING', 'Body bags':'BAG REMAINS',
          'Transport':'MOVE BAGMOVE CARGO LEG', 'Suppression':'SUP', 'Lives':'LIFE',
          'Focus':'FOCUS', 'Pain':'MED01', 'Medication':'MED', 'Arsenal':'INSTALL02 GM04'}
legacy = {'stretcherCarryEnabled', 'stretcherJoinRangeMetres',
          'stretcherPairTimeoutSeconds', 'stretcherMinimumSpanMetres',
          'stretcherMaximumSpanMetres', 'stretcherMaximumSpeedMetresPerSecond',
          'stretcherCarrySpeedFraction', 'stretcherCarryHeightMetres',
          'stretcherMaximumHeightDifferenceMetres', 'stretcherHandleOffsetMetres',
          'stretcherMaximumTurnDegreesPerSecond', 'stretcherGripAnimationEnabled',
          'stretcherGripBlendSeconds'}
assert legacy <= {e['key'] for e in catalog}

def default(entry):
    v = entry['default']
    if entry['min']==0 and entry['max']==1 and entry['step']==1:
        return 'On' if v else 'Off'
    return f'{v:g}'

appendix = ['<!-- GENERATED SETTINGS WORKSHEET -->', '',
            'Retained shelved stretcher controls are marked with *. Keep carrying/grip disabled; test presence and saving only. All other rows require a functional comparison using GM02 and their linked test family.', '']
for group in groups:
    appendix += ['### '+group, '', '| Control and exact key | Fresh default | Run with GM02 | Result and evidence |',
                 '| --- | --- | --- | --- |']
    for entry in catalog:
        if entry['group'] != group:
            continue
        mark = ' *' if entry['key'] in legacy else ''
        appendix += [f"| {entry['label']}{mark} (`{entry['key']}`) | {default(entry)} | {'Retained only' if mark else routes[group]} | NOT RUN |"]
    appendix += ['']
SOURCE.write_text((text+'\n'.join(appendix)).rstrip()+'\n', encoding='utf-8')

doc = Document()
sec = doc.sections[0]
sec.page_width = Inches(8.5)
sec.page_height = Inches(11)
sec.top_margin = sec.bottom_margin = Inches(.65)
sec.left_margin = sec.right_margin = Inches(.75)
sec.header_distance = sec.footer_distance = Inches(.25)
for style in doc.styles:
    if style.type == 1:
        style.font.name = 'Arial'
        style.font.color.rgb = RGBColor(0,0,0)
        style.paragraph_format.widow_control = True
normal = doc.styles['Normal']
normal.font.size = Pt(11)
normal.paragraph_format.line_spacing = 1.05
normal.paragraph_format.space_after = Pt(5)
for name,size in [('Title',25),('Subtitle',12),('Heading 1',17),('Heading 2',12)]:
    style=doc.styles[name]
    style.font.size=Pt(size)
    style.font.bold = name != 'Subtitle'
    style.paragraph_format.keep_with_next=True
    style.paragraph_format.space_before=Pt(13 if name=='Heading 1' else 8)
    style.paragraph_format.space_after=Pt(5)
doc.core_properties.title = 'BCM Medical Player Test Checklist'
doc.core_properties.author = 'Bootneck Company'
doc.core_properties.subject = 'Human gameplay and multiplayer acceptance workbook'
doc.core_properties.comments = ''

def para(s,style=None):
    p=doc.add_paragraph(style=style)
    label=re.match(r'^(Setup|Do|Pass): (.*)$',s)
    if label:
        p.add_run(label[1]+': ').bold=True
        p.add_run(label[2])
    else:
        p.add_run(s)
    return p

foot=sec.footer.paragraphs[0]
foot.paragraph_format.tab_stops.add_tab_stop(Inches(7),WD_TAB_ALIGNMENT.RIGHT)
r=foot.add_run('BCM Medical   Player testing\t')
r.font.name='Arial';r.font.size=Pt(8)
field=OxmlElement('w:fldSimple');field.set(qn('w:instr'),'PAGE');foot._p.append(field)

cases=[]
for line in text.splitlines():
    if not line.strip():
        continue
    if line.startswith('# '):
        para('BCM Medical\nPlayer Test Checklist','Title')
    elif line.startswith('## '):
        title=line[3:]
        if title.startswith('Appendix A'):
            doc.add_page_break()
        para(title,'Heading 1')
    elif line.startswith('### '):
        title=line[4:]
        ident=title.split()[0]
        assert re.fullmatch('[A-Z]+[0-9]{2}',ident),title
        cases.append(ident)
        p=para(title,'Heading 2')
        start=OxmlElement('w:bookmarkStart');start.set(qn('w:id'),str(len(cases)));start.set(qn('w:name'),ident)
        end=OxmlElement('w:bookmarkEnd');end.set(qn('w:id'),str(len(cases)))
        p._p.insert(0,start);p._p.append(end)
    elif line.startswith('<!--'):
        continue
    else:
        p=para(line)
        if line.startswith(('Setup:','Do:')):
            p.paragraph_format.keep_with_next=True
        if line.startswith('Pass:'):
            p.paragraph_format.keep_with_next=True
            result=para('[ ] PASS   [ ] FAIL   [ ] BLOCKED   [ ] NOT RUN     Evidence: __________')
            result.paragraph_format.space_after=Pt(9)
            for run in result.runs:run.font.size=Pt(9)

def table(entries):
    t=doc.add_table(rows=0,cols=4)
    t.alignment=WD_TABLE_ALIGNMENT.CENTER
    t.autofit=False
    widths=[4.15,.65,1.2,1.0]
    for col,width in zip(t.columns,widths):col.width=Inches(width)
    borders=OxmlElement('w:tblBorders')
    for side in ['top','bottom','left','right','insideH','insideV']:
        el=OxmlElement('w:'+side);el.set(qn('w:val'),'single');el.set(qn('w:sz'),'4');el.set(qn('w:color'),'D9D9D9');borders.append(el)
    t._tbl.tblPr.append(borders)
    rows=[['GM control','Default','Test family','Result']]
    for e in entries:
        mark=' *' if e['key'] in legacy else ''
        rows.append([e['label']+mark,default(e),'Retained only' if mark else routes[e['group']],'________'])
    for i,row in enumerate(rows):
        cells=t.add_row().cells
        for j,(cell,content,width) in enumerate(zip(cells,row,widths)):
            cell.width=Inches(width)
            cell.vertical_alignment=WD_CELL_VERTICAL_ALIGNMENT.CENTER
            pr=cell._tc.get_or_add_tcPr()
            shade=OxmlElement('w:shd');shade.set(qn('w:fill'),'333333' if i==0 else ('F4F4F4' if i%2==0 else 'FFFFFF'));pr.append(shade)
            mar=OxmlElement('w:tcMar')
            for side in ['left','right','top','bottom']:
                el=OxmlElement('w:'+side);el.set(qn('w:w'),'75');el.set(qn('w:type'),'dxa');mar.append(el)
            pr.append(mar)
            p=cell.paragraphs[0];p.paragraph_format.space_after=Pt(0);p.paragraph_format.line_spacing=1.02
            if j in [1,3]:p.alignment=WD_ALIGN_PARAGRAPH.CENTER
            run=p.add_run(content);run.font.name='Arial';run.font.size=Pt(10)
            if i==0:run.bold=True;run.font.color.rgb=RGBColor(255,255,255)
        trpr=t.rows[-1]._tr.get_or_add_trPr();trpr.append(OxmlElement('w:cantSplit'))
        if i==0:
            trpr.append(OxmlElement('w:tblHeader'))
            for c in cells:c.paragraphs[0].paragraph_format.keep_with_next=True
    doc.add_paragraph().paragraph_format.space_after=Pt(2)

para('Mark P, F, B or N in each row. Record the changed/restored values and effect evidence separately under the exact control label/key. Defaults are references, not assumed live server values. Every row uses the full GM02 procedure. An asterisk marks a retained control for shelved stretcher carrying/grip: keep the workflow disabled and test presence/save only.')
for group in groups:
    para(group,'Heading 2')
    table([e for e in catalog if e['group']==group])
for element in [doc._element,doc.styles.element]:
    for border in list(element.xpath('.//w:pBdr')):border.getparent().remove(border)
assert len(cases)==len(set(cases)) and len(catalog)==279
doc.save(TARGET)
manifest={'output':str(TARGET),'markdown':str(SOURCE),'cases':len(cases),'ids':cases,
          'settingRows':len(catalog),'groups':len(groups),'catalogSHA256':hashlib.sha256((ROOT/'Tools/SettingsCatalog.json').read_bytes()).hexdigest()}
(QA/'build-manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
print(json.dumps({k:v for k,v in manifest.items() if k!='ids'},indent=2))
