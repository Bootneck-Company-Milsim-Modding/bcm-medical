"""Targeted fabric geometry refinement on the accepted textured sling."""
import bpy,bmesh,math,json,shutil,numpy as np
from pathlib import Path
from mathutils import Vector,Matrix
OUT=Path(bpy.data.filepath).parent
backup=OUT/'BCM_Black_TwoHook_BungeeSling_BeforeJunctionRefinement.blend'
if not backup.exists():shutil.copy2(bpy.data.filepath,backup)
asset=bpy.data.collections['SLING | original static black nylon']
cloth=bpy.data.materials['Black woven nylon | 0.6mm basket weave']
edge=bpy.data.materials['Dark bound webbing selvedge']
thread=bpy.data.materials['Charcoal polyester load-bearing stitching']
S=.00030;theta=math.radians(35)
RIGHT=Vector((1,0,0));UP=Vector((0,math.sin(theta),math.cos(theta)))
DEPTH=Vector((0,-math.cos(theta),math.sin(theta)))
def pp(x,y,depth=0):return RIGHT*((x-600)*S)+UP*((600-y)*S)+DEPTH*depth
src=(OUT/'build_black_sling.py').read_text(encoding='utf-8-sig')
exec(src[src.index('def mesh('):src.index('# Front rolled webbing:')])
for ob in list(asset.objects):
    if ob.name.startswith(('06 |','07 |')) or ('HK spring snap hook' in ob.name and any(k in ob.name for k in ['continuous slot-threaded','raised woven edge','folded tab selvedge'])):
        bpy.data.objects.remove(ob,do_unlink=True)
# Eliminate the separate bright piping lines at the folded tabs.
p=edge.node_tree.nodes.get('Principled BSDF')
p.inputs['Roughness'].default_value=.95;p.inputs['Specular IOR Level'].default_value=.08

def soft_sleeve(name,points,M,width,height,n=600):
    path=samples(points,n);verts=[];faces=[];uv=[];r=64;distance=0
    target_side=(M.to_3x3()@Vector((1,0,0))).normalized()
    for i,c in enumerate(path):
        if i:distance+=(c-path[i-1]).length
        t=(path[min(i+1,n-1)]-path[max(i-1,0)]).normalized()
        side=DEPTH.cross(t).normalized()
        if side.dot(target_side)<0:side=-side
        up=t.cross(side).normalized()
        if up.dot(DEPTH)<0:up=-up
        s=i/(n-1);q=max(0,min(1,(s-.70)/.30));ease=q*q*(3-2*q)
        localheight=height*(1-ease)+.0016*ease
        localwidth=width*(1-ease)+(.025*M.to_scale().x)*ease
        env=math.sin(math.pi*s)**.35*(1-ease)
        phase=distance*2*math.pi/.009+1.3*math.sin(s*23)+.35*math.sin(s*61)
        for j in range(r):
            a=j*2*math.pi/r;wave=math.sin(phase+math.cos(a)*1.2)
            # Flatten the gathered sleeve gradually into the sewn webbing.
            power=.75*(1-ease)+.18*ease
            xx=math.copysign(abs(math.cos(a))**power,math.cos(a))*localwidth/2*(1+.12*env*wave)
            zz=math.copysign(abs(math.sin(a))**.65,math.sin(a))*localheight/2
            zz+=.0018*env*wave+.0004*env*math.sin(phase*1.8+2*a)
            verts.append(tuple(c+side*xx+up*zz))
            uv.append((distance/.04,j/r*(width*2+height*2)/.04))
    for i in range(n-1):
        for j in range(r):faces.append((i*r+j,i*r+(j+1)%r,(i+1)*r+(j+1)%r,(i+1)*r+j))
    faces.extend([tuple(reversed(range(r))),tuple((n-1)*r+j for j in range(r))])
    return mesh(name,verts,faces,cloth,uv)

def junction(name,pixel,angle,scale,points,height):
    M=plane_matrix(pixel,angle,.027,scale)
    end=M@Vector((0,.081,.0048))
    # The last 30% progressively loses gathering and becomes a sewn flat cuff.
    soft_sleeve(name+' | gathered-to-flat continuous sleeve',points+[end],M,.025,height)
    front=[M@Vector((0,y,.0048)) for y in [.084,.068,.050,.035,.026]]
    turn=[M@Vector((0,.026-.0045*math.sin(a),.0048*math.cos(a))) for a in np.linspace(0,math.pi,17)]
    back=[M@Vector((0,y,z)) for y,z in [(.035,-.0048),(.050,-.0048),(.067,-.0048),(.078,-.003),(.087,.0035)]]
    tabname=name+' | continuous slot-threaded folded tab'
    ribbon(tabname,front+turn[1:]+back,.025*scale,.0012,n=550,fixed_w=M.to_3x3()@Vector((1,0,0)))
    # The mesh already has finished fabric sides; duplicate edge curves caused
    # the detached bright lines, especially where the lower tab bends.
    for ob in list(asset.objects):
        if ob.name.startswith(tabname) and ob.type=='CURVE':bpy.data.objects.remove(ob,do_unlink=True)

junction('10 | upper HK spring snap hook',(1018,705),.93,1.0,
 [pp(393,219,-.006),pp(468,277,.004),pp(548,332,.012),pp(635,401,.018),pp(720,466,.020)],.013)
junction('11 | lower HK spring snap hook',(1053,1040),.74,1.15,
 [pp(348,260,.01),pp(391,347,.02),pp(447,449,.025),pp(498,531,.025),pp(580,637,.023),pp(677,728,.021)],.014)
# Ease the tight return above the centre adjuster while retaining its frame.
for ob in list(asset.objects):
    if ob.name.startswith('05 | centre adjustment frame | continuous over-under webbing'):
        bpy.data.objects.remove(ob,do_unlink=True)
M=plane_matrix((283,352),.08,.061)
route=[M@Vector((0,y,z)) for y,z in [(.045,-.0048),(.010,-.0048),(.006,.0048),(-.006,.0048),(-.010,-.0048),(-.045,-.0048)]]
before=[pp(260,235,-.060),pp(277,184,-.030),pp(293,170,.025)]
after=[pp(263,499,.055),pp(245,558,-.040),pp(199,560,-.070)]
ribbon('05 | centre adjustment frame | continuous over-under webbing',before+route+after,.028,.0014,n=650,fixed_w=M.to_3x3()@Vector((1,0,0)))
root=bpy.data.objects['BCM_Black_TwoHook_BungeeSling_INVENTORY_ONLY']
for ob in asset.objects:
    if ob!=root and ob.parent is None:ob.parent=root
# Discard the previous reduced mesh; regenerate from the refined source.
for coll in list(bpy.data.collections):
    if coll.name.startswith('EXPORT |'):
        for ob in list(coll.objects):bpy.data.objects.remove(ob,do_unlink=True)
        bpy.data.collections.remove(coll)
s=bpy.context.scene;s.use_nodes=False;s.render.film_transparent=True
prefs=bpy.context.preferences.addons['cycles'].preferences
try:
    prefs.compute_device_type='OPTIX';prefs.get_devices()
    for d in prefs.devices:d.use=d.type!='CPU'
    s.cycles.device='GPU'
except Exception:s.cycles.device='CPU'
s.cycles.samples=128;s.render.filepath=str(OUT/'Black_Bungee_Sling_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=bpy.data.filepath)
bpy.ops.render.render(write_still=True)
report=json.loads((OUT/'asset-report.json').read_text())
report.update({'fabric_junction_revision':'Gradual gathered-to-flat cuffs; duplicate piping removed; centre return eased',
 'approved_shape_preserved':False,'overall_layout_retained':True})
report.pop('mesh_sha256',None)
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
print('FABRIC_JUNCTION_REFINEMENT_COMPLETE',flush=True)
