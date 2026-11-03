"""Reviewed BT01 packing, preview and model inputs. Native bandage = 100 cm3.
World geometry, packing volume and preview distance are separate changes.
Run once, then Build-MedicalModels.py for the listed changed model names.
"""
from pathlib import Path
from PIL import Image
import copy, hashlib, json, math, re
root=Path(__file__).resolve().parents[2]
path=root/'Tools/AssetImport/MedicalAssets.json'
manifest=json.loads(path.read_text())
def ref(path): return '{'+hashlib.sha256(('BCMM/Beta01/'+path).encode()).hexdigest()[:16].upper()+'}'+path
sizes={'Splint':.105,'SurgeryKit':.16,'SutureKit':.12}
injectors={'Epinephrine','Injector','Morphine','GenericInjector','Naloxone','NCD','TXA'}
for item in manifest:
    if item['name'] in sizes: item['desired_longest_metres']=sizes[item['name']]
    if item['name'] in injectors:
        # Native morphine's long axis is Z, custom exports were along Y.
        item.pop('rotation_euler_degrees',None)
        item['align_long_axis']='Y'
        item['reverse_long_axis']=True
        item['origin_offset']=[0,0,.005]
    if item['name']=='BVM':
        item['include_objects']=['sweep1','polySurface2','polySurface8','pCylinder7','pCylinder6','pCylinder9']
        item['desired_longest_metres']=.32
    if item['name']=='Suction':
        # Remote wound-dressing demonstration cups sit half a metre from the
        # pump in the source presentation; keep the pump, chamber and tubing.
        item['exclude_objects_prefix']=['VAC_Device_Model_Silc_Cup']
        item['desired_longest_metres']=.22
fluid=next(item for item in manifest if item['name']=='FluidBag')
for name,colour in [('BloodBody',(150,8,22)),('PlasmaBody',(235,182,35))]:
    stem='Assets/Medical/FluidBag/Data/'+name
    Image.new('RGBA',(16,16),(*colour,165)).save(root/(stem+'_BCR.png'))
    tex=stem+'_BCR.edds'
    (root/(tex+'.meta')).write_text('MetaFileClass { Name "'+ref(tex)+'" Configurations { PNGResourceClass PC : "{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf" {} PNGResourceClass HEADLESS : PC {} } }')
    emat=stem+'.emat'
    (root/emat).write_text('MatPBRBasic { BCRMap "'+ref(tex)+'" NMOMap "{B3FC7CD83EB60517}Assets/Medical/FluidBag/Data/FluidBag_0_NMO.edds" }')
    (root/(emat+'.meta')).write_text('MetaFileClass { Name "'+ref(emat)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }')
fluid['materials'][0].update(material=ref('Assets/Medical/FluidBag/Data/BloodBody.emat'),base_preview='Assets/Medical/FluidBag/Data/BloodBody_BCR.png')
plasma=copy.deepcopy(fluid); plasma['name']='PlasmaBag'; plasma['model']=ref('Assets/Medical/PlasmaBag/BCMM_PlasmaBag.xob')
plasma['materials'][0].update(material=ref('Assets/Medical/FluidBag/Data/PlasmaBody.emat'),base_preview='Assets/Medical/FluidBag/Data/PlasmaBody_BCR.png')
manifest=[item for item in manifest if item['name']!='PlasmaBag']+[plasma]
path.write_text(json.dumps(manifest,indent=2)+'\n')
packing={'Splint':('7 5 3',125),'SurgeryKit':('10 7 4',300),'SutureKit':('8 6 4',250),'DragRope':('8 6 3',120),'WeaponSling':('8 5 2',80)}
longest={item['name']:item['desired_longest_metres'] for item in manifest}
longest.update(DragRope=.14,WeaponSling=.14,Saline=.24,Plasma=.24,Fentanyl=.14,AirwayAdjunct=.0843,ChestDrain=.0843)
for prefab in (root/'Prefabs/Items').glob('BCMM_*.et'):
    stem=prefab.stem.removeprefix('BCMM_'); s=prefab.read_text()
    if stem in ('EmptyBodyBag','GasMask','FieldSurgeryStation'): continue
    if stem=='NCD':
        s=s.replace('{A81F501D3EF6F38E}Prefabs/Items/Medicine/FieldDressing_01/FieldDressing_US_01.et','{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et')
        s=s.replace('{51F66EC3C5BC700D}','{59CE87DB17B6B1C5}').replace('{5943631460899505}','{59E68C74C5CEE74F}').replace('{594363147CC4C6BE}','{59E68C74C1F3910E}').replace('BCMM_ProcedurePresentationEffect','BCMM_InjectionPresentationEffect')
    if stem=='Saline':
        s=s.replace('{1B49817205B37B1C}Assets/Medical/FluidBag/BCMM_FluidBag.xob','{E6E83E5ED49B9DE5}Assets/Items/Medicine/SalineBag_01/SalineBag_US_01/SalineBag_US_01.xob')
        s=s.replace('Shared bag mesh is temporary.','Uses the native saline bag model.')
    if stem=='Plasma': s=s.replace('{1B49817205B37B1C}Assets/Medical/FluidBag/BCMM_FluidBag.xob',plasma['model']).replace('Shared bag mesh is temporary.','Yellow plasma bag.')
    if stem=='DragRope':
        s=re.sub(r'(?m)^ scale [0-9.]+', ' scale 1', s)
        s=s.replace('{E20A38B86EF7369B}Assets/Props/Naval/FishingGear_01/FishingRope_01.xob',ref('Assets/Medical/CompactDragRope/BCMM_CompactDragRope.xob'))
    if stem in packing:
        dims,volume=packing[stem]
        s=re.sub(r'ItemDimensions [^\r\n]+','ItemDimensions '+dims,s)
        s=re.sub(r'ItemVolume \d+','ItemVolume '+str(volume),s)
    if 'ItemPhysicalAttributes PhysicalAttributes {' in s and 'DimensionScaler' not in s:
        s=s.replace('ItemPhysicalAttributes PhysicalAttributes {','ItemPhysicalAttributes PhysicalAttributes {\n     DimensionScaler 1')
    morphine='MorphineInjection_01.et' in s.splitlines()[0]
    preview_id='5A5AEE4B8FF125F0' if morphine else '6098B9508619B4D0'
    rotation='90 0 0' if stem in ('ChestSeal','FluidBag','Plasma','BVM','Morphine','Epinephrine','Fentanyl','Naloxone','NCD','TXA') else '0 0 0'
    if stem=='AED': rotation='180 0 0'
    # Verified side-by-side with the actual native item preview renderer.
    if stem in ('FluidBag','Plasma','ChestSeal'): rotation='0 90 0'
    if stem=='Monitor': rotation='180 0 0'
    if stem in ('Saline','AirwayAdjunct','ChestDrain'): rotation='90 0 0'
    if stem in ('Oxygen','VascularAccess','PulseOximeter'): rotation='90 0 0'
    if stem=='Suction': rotation='180 0 0'
    camera='Top' if stem in ('Splint','SurgeryKit','SutureKit','Paracetamol','DragRope','WeaponSling') else 'Front'
    distance=max(.08,longest.get(stem,.2)*1.2/(2*math.tan(math.radians(17.5))))
    if stem=='Paracetamol': distance=.228 # Same compact pouch mesh as SutureKit.
    preview='PreviewRenderAttributes "{'+preview_id+'}" { CameraPreset '+camera+' CameraDistanceToItem '+f'{distance:.3f}'+' CameraOffset 0 0 0 ItemRenderRotation '+rotation+' FOV 35 AspectRatio 1 }'
    if re.search(r'PreviewRenderAttributes "[^"]+" \{[^}]*\}',s):
        s=re.sub(r'PreviewRenderAttributes "[^"]+" \{[^}]*\}',lambda m:preview,s)
    elif 'CustomAttributes {' in s: s=s.replace('CustomAttributes {','CustomAttributes {\n     '+preview,1)
    else: s=s.replace('    CommonItemType','    CustomAttributes { '+preview+' }\n    CommonItemType',1)
    # A bandage's alternate gauze mesh must never replace custom equipment.
    if 'SCR_ConsumableItemComponent' in s and 'm_bAlternativeModelOnAction' not in s:
        s=re.sub(r'(SCR_ConsumableItemComponent "[^"]+" \{)',r'\1 m_bAlternativeModelOnAction 0',s, count=1)
    prefab.write_text(s)
rope='Assets/Medical/CompactDragRope/'
Image.new('RGBA',(16,16),(88,78,49,255)).save(root/(rope+'RopeCoil_BCR.png'))
tex=rope+'RopeCoil_BCR.edds';emat=rope+'RopeCoil.emat'
(root/(tex+'.meta')).write_text('MetaFileClass { Name "'+ref(tex)+'" Configurations { PNGResourceClass PC : "{EAB5DE3219F9CBA8}Configs/System/ResourceTypes/PC/TextureColorMap.conf" {} PNGResourceClass HEADLESS : PC {} } }')
(root/emat).write_text('MatPBRBasic { BCRMap "'+ref(tex)+'" }')
(root/(emat+'.meta')).write_text('MetaFileClass { Name "'+ref(emat)+'" Configurations { EMATResourceClass PC {} EMATResourceClass HEADLESS : PC {} } }')
print('Updated model inputs, packing and independent preview cameras; native visual acceptance still required.')
