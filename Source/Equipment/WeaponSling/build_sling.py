"""Original static inventory sling; Blender 4.5, no rig or animation assets.
Run with --background --factory-startup --threads 1 --python build_sling.py.
"""
import bpy, bmesh, math, random, json, os
from pathlib import Path
from mathutils import Vector, Matrix
import numpy as np

OUT = Path(__file__).resolve().parent
random.seed(684031)
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
for block in list(bpy.data.collections):
    if block.name != 'Collection': bpy.data.collections.remove(block)
hero = bpy.data.collections.get('Collection'); hero.name = 'SLING | detailed static asset'
studio = bpy.data.collections.new('STUDIO | cameras and lighting'); bpy.context.scene.collection.children.link(studio)

def move_to(obj, collection):
    for c in list(obj.users_collection): c.objects.unlink(obj)
    collection.objects.link(obj)

def mat(name, color, rough=.5, metallic=0):
    linear=tuple(c/12.92 if c<=.04045 else ((c+.055)/1.055)**2.4 for c in color)
    m=bpy.data.materials.new(name); m.diffuse_color=(*linear,1); m.use_nodes=True
    p=m.node_tree.nodes.get('Principled BSDF'); p.inputs['Base Color'].default_value=(*linear,1)
    p.inputs['Roughness'].default_value=rough; p.inputs['Metallic'].default_value=metallic
    return m

def image(name, pixels, filename, noncolor=False):
    n=pixels.shape[0]; im=bpy.data.images.new(name,width=n,height=n,alpha=True)
    if noncolor: im.colorspace_settings.name='Non-Color'
    im.pixels.foreach_set(pixels.astype(np.float32).ravel()); im.filepath_raw=str(OUT/filename)
    im.file_format='PNG'; im.save(); im.pack(); return im

# Original repeatable woodland camouflage, not extracted from the product photo.
N=2048
yy,xx=np.mgrid[0:1:complex(N),0:1:complex(N)]
def field(seed, frequencies):
    rng=random.Random(seed); a=np.zeros((N,N),np.float32)
    for scale,amp in frequencies:
        for _ in range(4):
            kx=rng.randint(1,scale); ky=rng.randint(-scale,scale)
            a+=amp*np.sin(2*math.pi*(kx*xx+ky*yy)+rng.random()*6.283)/4
    return a
warp=field(27,[(2,.8),(5,.35),(14,.08)])
f=field(11,[(3,1),(7,.45),(16,.15)])
g=field(31,[(4,1),(10,.35),(22,.1)])
h=field(63,[(7,1),(13,.32),(30,.07)])
palette=np.array([[.52,.47,.32],[.42,.43,.27],[.32,.36,.20],[.55,.51,.37],
                  [.30,.26,.18],[.20,.19,.135],[.64,.60,.45]],np.float32)
idx=np.zeros((N,N),np.int32)
idx[f+warp*.35>.08]=1; idx[f+g*.25>.25]=2
idx[(g<-.14)&(f<.16)]=3
idx[(g>.28)&(h>-.10)]=4
idx[(h>.40)&(g>.03)]=5
idx[(h<-.37)&(f<.0)]=6
rgb=palette[idx]
weave=.025*np.sin(xx*2*math.pi*640)*np.sin(yy*2*math.pi*640)
grain=np.random.default_rng(81).normal(0,.008,(N,N)).astype(np.float32)
rgb=np.clip(rgb*(1+weave[:,:,None]+grain[:,:,None]),0,1)
rgba=np.ones((N,N,4),np.float32); rgba[:,:,:3]=np.where(rgb<=.04045,rgb/12.92,((rgb+.055)/1.055)**2.4)
albedo=image('BCM original woodland webbing | 2K',rgba,'Sling_Woodland_BaseColor.png')
rough=np.ones_like(rgba); rough[:,:,:3]=np.clip(.73+grain[:,:,None]*2,.55,.9)
roughmap=image('Webbing roughness | 2K',rough,'Sling_Webbing_Roughness.png',True)
normal=np.ones_like(rgba); normal[:,:,0]=.5+.11*np.cos(xx*2*math.pi*640)
normal[:,:,1]=.5+.11*np.cos(yy*2*math.pi*640); normal[:,:,2]=.98
normalmap=image('Woven nylon tangent detail | 2K',normal,'Sling_Webbing_Normal.png',True)
cloth=mat('Woven woodland nylon | packed 2K PBR',(.4,.42,.27),.74)
nodes=cloth.node_tree.nodes; links=cloth.node_tree.links; p=nodes.get('Principled BSDF')
for im,target in [(albedo,'Base Color'),(roughmap,'Roughness')]:
    t=nodes.new('ShaderNodeTexImage'); t.image=im; t.extension='REPEAT'; links.new(t.outputs['Color'],p.inputs[target])
t=nodes.new('ShaderNodeTexImage'); t.image=normalmap; t.extension='REPEAT'
nm=nodes.new('ShaderNodeNormalMap'); nm.inputs['Strength'].default_value=.42
links.new(t.outputs['Color'],nm.inputs['Color']); links.new(nm.outputs['Normal'],p.inputs['Normal'])
p.inputs['Sheen Weight'].default_value=.035; p.inputs['Sheen Roughness'].default_value=.65
p.inputs['Specular IOR Level'].default_value=.12
edgecloth=mat('Bound nylon edges | muted olive',(.22,.235,.14),.8)
thread=mat('Khaki polyester stitching',(.48,.45,.30),.82)
tan=mat('Coyote brown moulded hardware',(.35,.275,.17),.43)
black=mat('Black phosphated steel',(.022,.025,.022),.34,.72)
buttonmat=mat('QD button | satin graphite',(.045,.05,.045),.28,.8)
dark=mat('Recess interiors',(.012,.014,.011),.8)
# Fine mould texture on buckles, without modelling fake cavities into a solid block.
no=tan.node_tree.nodes.new('ShaderNodeTexNoise'); no.inputs['Scale'].default_value=480
bu=tan.node_tree.nodes.new('ShaderNodeBump'); bu.inputs['Strength'].default_value=.12;bu.inputs['Distance'].default_value=.00012
tan.node_tree.links.new(no.outputs['Fac'],bu.inputs['Height']);tan.node_tree.links.new(bu.outputs['Normal'],tan.node_tree.nodes.get('Principled BSDF').inputs['Normal'])

def mesh_obj(name,verts,faces,material,uvs=None):
    mesh=bpy.data.meshes.new(name);mesh.from_pydata(verts,[],faces);mesh.update()
    bm=bmesh.new();bm.from_mesh(mesh);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(mesh);bm.free()
    ob=bpy.data.objects.new(name,mesh);hero.objects.link(ob);ob.data.materials.append(material)
    for poly in mesh.polygons: poly.use_smooth=True
    if uvs:
        layer=mesh.uv_layers.new(name='UVMap')
        for poly in mesh.polygons:
            for li,vi in zip(poly.loop_indices,poly.vertices):layer.data[li].uv=uvs[vi]
    return ob

def curve(name,points,radius,material,cyclic=False,res=2):
    d=bpy.data.curves.new(name,'CURVE');d.dimensions='3D';d.resolution_u=2
    d.bevel_depth=radius;d.bevel_resolution=res
    s=d.splines.new('POLY');s.points.add(len(points)-1)
    for p,co in zip(s.points,points):p.co=(*co,1)
    s.use_cyclic_u=cyclic
    o=bpy.data.objects.new(name,d);hero.objects.link(o);o.data.materials.append(material);return o

def sample(points,count):
    # Uniform Catmull-Rom resampling; independent editable meshes remain in .blend.
    pts=[Vector(p) for p in points]; pts=[pts[0]]+pts+[pts[-1]]
    result=[]
    for k in range(count):
        x=k/(count-1)*(len(pts)-3);i=min(int(x),len(pts)-4);t=x-i
        a,b,c,d=pts[i:i+4]
        result.append(.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*t*t+(-a+3*b-3*c+d)*t*t*t))
    return result

def strap(name,points,width,thickness,count=120,radial=16,gather=0,material=cloth,stitches=False):
    path=sample(points,count);verts=[];faces=[];uv=[];length=0;frames=[]
    for i,point in enumerate(path):
        if i:length+=(point-path[i-1]).length
        tangent=(path[min(i+1,count-1)]-path[max(i-1,0)]).normalized()
        side=tangent.cross(Vector((0,0,1))).normalized();up=side.cross(tangent).normalized()
        frames.append((side,up))
        progress=i/(count-1);phase=progress*math.pi*2*19+1.1*math.sin(progress*17)+.5*math.sin(progress*43)
        envelope=math.sin(math.pi*progress)**.3
        for j in range(radial):
            a=2*math.pi*j/radial
            x=math.copysign(abs(math.cos(a))**.34,math.cos(a))*width*.5
            z=math.copysign(abs(math.sin(a))**.55,math.sin(a))*thickness*.5
            if gather:
                wave=math.sin(phase+1.35*math.cos(a)+.6*math.sin(progress*31))
                x*=1+.14*wave*envelope
                z+=gather*envelope*(wave+.30*math.sin(phase*2.12-2*math.cos(a)))
                x+=.0013*envelope*math.sin(phase*1.37+3*a)
            verts.append(tuple(point+side*x+up*z));uv.append((length/.19,j/(radial-1)*(width*2+thickness*2)/.19))
    for i in range(count-1):
        for j in range(radial):
            k=(j+1)%radial;faces.append((i*radial+j,i*radial+k,(i+1)*radial+k,(i+1)*radial+j))
    faces.extend([tuple(reversed(range(radial))),tuple((count-1)*radial+j for j in range(radial))])
    ob=mesh_obj(name,verts,faces,material,uv)
    if not gather:
        bevel=ob.modifiers.new('Soft woven edges','BEVEL');bevel.width=.00065;bevel.segments=2
    if stitches:
        for sign in [-1,1]:
            pts=[tuple(p+frames[i][0]*(sign*(width*.5-.002))+frames[i][1]*(thickness*.5+.00025)) for i,p in enumerate(path)]
            curve(name+' | stitched edge seam',pts,.00019,thread)
            for i in range(1,count-2,3):
                curve(name+' | individual lockstitch',pts[i:i+2],.00011,thread,res=1)
    return ob

def box(name,location,scale,material,bevel=.001,angle=0):
    bpy.ops.mesh.primitive_cube_add(size=1,location=location);o=bpy.context.object;o.name=name
    move_to(o,hero);o.dimensions=scale;o.rotation_euler[2]=angle
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(material)
    if bevel:
        b=o.modifiers.new('Moulded radiused edges','BEVEL');b.width=bevel;b.segments=3
        n=o.modifiers.new('Weighted corner normals','WEIGHTED_NORMAL')
    return o

def cylinder(name,loc,radius,depth,material,direction=(0,0,1),vertices=48):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices,radius=radius,depth=depth,location=loc)
    o=bpy.context.object;o.name=name;move_to(o,hero)
    o.rotation_euler=Vector(direction).to_track_quat('Z','Y').to_euler();o.data.materials.append(material)
    b=o.modifiers.new('Machined edge break','BEVEL');b.width=.00045;b.segments=2
    o.modifiers.new('Weighted metal normals','WEIGHTED_NORMAL')
    for p in o.data.polygons:p.use_smooth=True
    return o

def triglide(name,center,width,length,angle=0,material=tan):
    c=Vector(center)
    def tr(x,y,z=0):return c+Vector((x*math.cos(angle)-y*math.sin(angle),x*math.sin(angle)+y*math.cos(angle),z))
    for sign in [-1,1]:
        box(name+' | end crossbar',tr(0,sign*length/2),(width,.0045,.005),material,.0013,angle)
        box(name+' | side rail',tr(sign*width/2,0),(.0045,length,.005),material,.0013,angle)
    box(name+' | friction crossbar',tr(0,0,.0008),(width,.0045,.006),material,.0011,angle)
    # Raised moulded grip ribs on central crossbar.
    for x in np.linspace(-width*.36,width*.36,9):
        box(name+' | moulded grip rib',tr(float(x),0,.004),(.00065,.003,.0005),material,.0002,angle)

# Padded rear shoulder panel: broad camouflaged strap, subtle taper and stitched binding.
padpts=[(-.203,.135,.043),(-.17,.17,.051),(-.095,.205,.057),(0,.234,.063),(.09,.208,.054),(.17,.18,.045),(.203,.15,.042)]
strap('01 | padded shoulder panel',padpts,.055,.009,180,24,stitches=True)
# Narrow tapes bridge rear padding and side adjusters; actual open slot geometry.
strap('02 | left adjustable webbing',[(-.19,.155,.044),(-.216,.114,.039),(-.237,.081,.035),(-.237,.031,.026),(-.215,.005,.03)],.026,.0024,65,12,stitches=True)
strap('03 | right adjustable webbing',[(.198,.159,.04),(.235,.108,.035),(.235,.056,.032),(.213,.023,.033)],.026,.0024,65,12,stitches=True)
triglide('Left side adjustment slider',(-.238,.041,.032),.031,.025,angle=-.27)
triglide('Right side adjustment slider',(.232,.069,.039),.031,.025,angle=.27)
# Doubled return tails thread over and under the friction bars.
strap('Left folded return tail',[(-.239,.065,.035),(-.246,.035,.037),(-.236,.005,.034),(-.216,-.006,.031)],.025,.002,60,12)
strap('Right folded return tail',[(.232,.092,.038),(.241,.065,.043),(.232,.036,.037),(.213,.021,.034)],.025,.002,60,12)
triglide('Left webbing keeper',(-.218,.011,.033),.029,.009,-.65)
triglide('Right webbing keeper',(.216,.026,.037),.029,.009,.6)
# The gathered elastic sleeves are actual folded geometry, not a normal-map illusion.
left=[(-.216,.003,.032),(-.169,-.047,.031),(-.102,-.096,.029),(-.028,-.140,.029),(.034,-.171,.031),(.061,-.178,.032)]
right=[(.212,.024,.033),(.207,-.031,.030),(.180,-.091,.030),(.139,-.139,.029),(.095,-.171,.031),(.067,-.18,.033)]
strap('04 | gathered bungee sleeve left',left,.040,.011,370,24,.0037)
strap('05 | gathered bungee sleeve right',right,.040,.011,370,24,.0037)
for name,pts in [('Left',left),('Right',right)]:
    pp=sample(pts,200);side=[]
    for i,p in enumerate(pp):
        tang=(pp[min(i+1,199)]-pp[max(i-1,0)]).normalized();s=tang.cross(Vector((0,0,1))).normalized()
        side.append(tuple(p+s*.018+Vector((0,0,.0018*math.sin(i*.65)))))
    curve(name+' bungee | longitudinal reinforced seam',side,.00023,edgecloth)
# Offset junction and shorter, slightly right-hanging tether match the reference silhouette.
before_junction=set(hero.objects)
# Reinforced center junction and downward single-point tether.
strap('06 | center reinforced take-off',[(-.002,-.162,.034),(.004,-.192,.037),(.006,-.214,.035)],.025,.0032,55,14,stitches=True)
strap('07 | doubled connector tether',[(.004,-.228,.032),(.011,-.281,.026),(.007,-.335,.021),(-.006,-.379,.018)],.026,.003,110,16,stitches=True)
strap('08 | folded QD attachment loop',[(-.006,-.36,.02),(-.007,-.395,.02),(-.007,-.409,.023),(-.012,-.39,.027),(-.008,-.36,.027)],.025,.002,65,12,stitches=True)
# Box-X and bar-tack stitches on load-bearing webbing, mesh curves for clean closeups.
def boxtack(name,cx,cy,w,h,z):
    pts=[(cx-w/2,cy-h/2,z),(cx+w/2,cy-h/2,z),(cx+w/2,cy+h/2,z),(cx-w/2,cy+h/2,z)]
    curve(name+' | rectangle',pts,.00016,thread,True)
    curve(name+' | diagonal A',[pts[0],pts[2]],.00014,thread)
    curve(name+' | diagonal B',[pts[1],pts[3]],.00014,thread)
boxtack('Connector load-bearing box-X',.005,-.304,.019,.035,.028)
for y in [-.185,-.347,-.367]:
    for i in range(12):
        x=-.007+.00135*i
        curve('Reinforced dense bar tack',[(x,y-.001,.036 if y>-.2 else .027),(x,y+.001,.036 if y>-.2 else .027)],.00012,thread,res=1)
# Two-piece coyote side-release buckle, with real openings and separate latch details.
cx=.005;cy=-.235;z=.037
for sign in [-1,1]:
    box('09 | buckle female outer rail',(cx+sign*.014,cy,z),(.005,.038,.009),tan,.0015)
box('Buckle female crown',(cx,cy+.012,z+.003),(.025,.014,.004),tan,.0015)
box('Buckle female underside bridge',(cx,cy+.003,z-.004),(.029,.025,.003),tan,.0008)
box('Buckle male tongue',(cx,cy-.014,z+.0005),(.012,.030,.0035),tan,.001)
for sign in [-1,1]:
    box('Buckle resilient release ear',(cx+sign*.010,cy-.008,z+.001),(.004,.022,.005),tan,.001)
    for y in [cy-.012,cy-.007,cy-.002]:
        box('Buckle tactile release rib',(cx+sign*.0125,y,z+.004),(.004,.0008,.0009),tan,.0003)
triglide('Buckle lower webbing attachment',(cx,cy-.029,z-.001),.027,.012)
# QD swivel: forged rounded attachment loop, hinge, cylindrical stud and spring button.
ring=[(-.021,-.394,.025),(-.023,-.407,.025),(-.021,-.422,.025),(-.016,-.426,.025),(.007,-.426,.025),(.012,-.421,.025),(.012,-.401,.025),(.007,-.395,.025)]
curve('10 | black steel QD swivel loop',ring,.0028,black,True,4)
cylinder('QD hinged cross-pin',(-.005,-.425,.025),.0043,.036,black,(1,0,0))
cylinder('QD machined barrel',(-.005,-.439,.023),.009,.025,black,(0,1,0))
cylinder('QD forward shoulder',(-.005,-.449,.023),.0102,.0045,black,(0,1,0))
cylinder('QD end push-button',(-.005,-.453,.023),.0065,.004,buttonmat,(0,1,0))
for dx in [-.007,.007]:
    bpy.ops.mesh.primitive_uv_sphere_add(segments=16,ring_count=8,radius=.002,location=(-.005+dx,-.445,.025))
    o=bpy.context.object;o.name='QD spring detent ball';move_to(o,hero);o.data.materials.append(buttonmat)

for obj in set(hero.objects)-before_junction:
    transform=Matrix.Identity(4)
    transform[0][1]=-.15;transform[0][3]=.033
    transform[1][1]=.88;transform[1][3]=-.0216
    obj.matrix_world=transform @ obj.matrix_world
# Organize fully editable parts; no rigging, wearer or equip animation.
for o in list(hero.objects):o['asset_role']='static inventory sling'
root=bpy.data.objects.new('BCM_Sling_SINGLE_POINT_INVENTORY',None);hero.objects.link(root)
for o in list(hero.objects):
    if o!=root:o.parent=root
root['units']='metres';root['original_asset']=True
root['reference']='User-supplied single-point woodland sling photograph; original geometry and texture pattern'
root['wearable']=False;root['game_integration']='not integrated; static art source only'

# Build a practical mesh-only copy; retain original curves and modifiers in HERO.
low=bpy.data.collections.new('INVENTORY LOD | mesh-only export candidate');bpy.context.scene.collection.children.link(low)
deps=bpy.context.evaluated_depsgraph_get();lod_parts=[]
for o in list(hero.objects):
    if o.type not in {'MESH','CURVE'}:continue
    if 'individual lockstitch' in o.name or 'dense bar tack' in o.name:continue
    ev=o.evaluated_get(deps);me=bpy.data.meshes.new_from_object(ev,depsgraph=deps)
    ob=bpy.data.objects.new(o.name+' | export',me);low.objects.link(ob);ob.matrix_world=o.matrix_world;lod_parts.append(ob)
bpy.ops.object.select_all(action='DESELECT')
for o in lod_parts:o.select_set(True)
bpy.context.view_layer.objects.active=lod_parts[0];bpy.ops.object.join()
lo=bpy.context.object;lo.name='BCM_Sling_Inventory_LOD'
dec=lo.modifiers.new('Inventory mesh reduction','DECIMATE');dec.ratio=.23
bpy.ops.object.modifier_apply(modifier=dec.name)
tri=lo.modifiers.new('Game triangles','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
lo['note']='Export candidate; inspect UVs/material slots and validate engine import before distribution.'
low.hide_render=True;low.hide_viewport=True

# Transparent product studio; no backdrop geometry is part of this asset.
def aim(o,point):o.rotation_euler=(Vector(point)-o.location).to_track_quat('-Z','Y').to_euler()
def area(name,loc,power,size):
    data=bpy.data.lights.new(name,'AREA');data.energy=power;data.shape='DISK';data.size=size
    o=bpy.data.objects.new(name,data);studio.objects.link(o);o.location=loc;aim(o,(0,-.08,.02))
area('Large softbox key',(-.35,-.25,.8),10,.75)
area('Gentle fabric fill',(.45,.20,.55),5.5,.60)
area('Top rim',(-.10,.50,.35),4.5,.45)
data=bpy.data.cameras.new('Product camera');camera=bpy.data.objects.new('Product camera',data);studio.objects.link(camera)
camera.location=(.11,-.73,1.52);aim(camera,(0,-.095,.02));data.type='ORTHO';data.ortho_scale=.82
scene=bpy.context.scene;scene.camera=camera;scene.render.engine='CYCLES';scene.cycles.device='CPU'
scene.cycles.samples=40;scene.cycles.use_denoising=True;scene.render.threads_mode='FIXED';scene.render.threads=4
scene.render.resolution_x=1100;scene.render.resolution_y=1400;scene.render.resolution_percentage=100
scene.world.color=(.16,.16,.16);scene.view_settings.view_transform='AgX'
scene.render.image_settings.file_format='PNG';scene.render.image_settings.color_mode='RGBA';scene.render.film_transparent=True
scene.unit_settings.system='METRIC';scene.unit_settings.length_unit='METERS'
scene['asset_intent']='Static inventory item only. No equipping, character attachment, rig or animations.'
scene['materials']='Original woodland pattern, packed 2K albedo/roughness/normal maps'
bpy.ops.object.select_all(action='DESELECT')
for o in hero.objects:
    if o.type=='MESH':o.select_set(True)
bpy.context.view_layer.objects.active=next(o for o in hero.objects if o.type=='MESH')
for screen in bpy.data.screens:
    for a in screen.areas:
        if a.type=='VIEW_3D':
            a.spaces.active.region_3d.view_distance=.9
            a.spaces.active.region_3d.view_location=(0,-.09,.02)
            a.spaces.active.region_3d.view_rotation=camera.rotation_euler.to_quaternion()
            a.spaces.active.shading.type='MATERIAL'
scene.render.filepath=str(OUT/'Sling_Product_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_SinglePoint_WeaponSling.blend'))
report={'original_static_asset':True,'inventory_only':True,'animation_data':False,
        'mesh_objects':len([o for o in hero.objects if o.type=='MESH']),
        'detail_objects':len(hero.objects),'inventory_lod_triangles':len(lo.data.polygons),
        'texture_size':N,'native_game_validation':False,'reference_photo_embedded':False}
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
if not os.environ.get('BCM_SLING_BUILD_ONLY'):bpy.ops.render.render(write_still=True)
print('BCM_SLING_BUILD_COMPLETE',json.dumps(report),flush=True)
