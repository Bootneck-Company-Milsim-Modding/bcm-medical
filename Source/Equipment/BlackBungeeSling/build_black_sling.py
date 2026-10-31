"""Original static two-hook black bungee sling, Blender 4.5.
Reference: user-supplied jag1-23-1200x1200.jpg. No photograph texture or watermark.
"""
import bpy,bmesh,math,json,random
from pathlib import Path
from mathutils import Vector,Matrix
import numpy as np
OUT=Path(__file__).resolve().parent
random.seed(52317)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
for c in list(bpy.data.collections):
    if c.name!='Collection':bpy.data.collections.remove(c)
asset=bpy.data.collections.get('Collection');asset.name='SLING | original static black nylon'
studio=bpy.data.collections.new('STUDIO | no backdrop');bpy.context.scene.collection.children.link(studio)
S=.00030;theta=math.radians(35)
RIGHT=Vector((1,0,0));UP=Vector((0,math.sin(theta),math.cos(theta)))
DEPTH=Vector((0,-math.cos(theta),math.sin(theta)))
def pp(x,y,depth=0):return RIGHT*((x-600)*S)+UP*((600-y)*S)+DEPTH*depth
def put(o,collection=asset):
    for c in list(o.users_collection):c.objects.unlink(o)
    collection.objects.link(o)
def material(name,col,rough=.5,metal=0):
    m=bpy.data.materials.new(name);m.use_nodes=True;m.diffuse_color=(*col,1)
    p=m.node_tree.nodes.get('Principled BSDF');p.inputs['Base Color'].default_value=(*col,1)
    p.inputs['Roughness'].default_value=rough;p.inputs['Metallic'].default_value=metal
    return m
cloth=material('Black woven nylon | 0.6mm basket weave',(.008,.009,.010),.72)
p=cloth.node_tree.nodes.get('Principled BSDF');p.inputs['Sheen Weight'].default_value=.045;p.inputs['Specular IOR Level'].default_value=.28
thread=material('Charcoal polyester load-bearing stitching',(.026,.028,.030),.8)
edge=material('Dark bound webbing selvedge',(.006,.007,.008),.77)
polymer=material('Graphite injection-moulded adjusters',(.016,.018,.020),.47,.03)
polymer.node_tree.nodes.get('Principled BSDF').inputs['Specular IOR Level'].default_value=.12
polymer.node_tree.nodes.get('Principled BSDF').inputs['Roughness'].default_value=.65
steel=material('Dark grey coated steel | hook bodies',(.075,.083,.09),.34,.82)
gate_mat=material('Spring latch | charcoal coated steel',(.049,.055,.06),.40,.72)
rivet_mat=material('Blackened pivot and rivet',(.020,.024,.027),.25,.85)

# Original physically scaled basket-weave maps: about 32 pixels per yarn.
N=2048;y,x=np.mgrid[0:N,0:N].astype(np.float32)/N
cells=62;u=x*cells;v=y*cells
fu=u%1;fv=v%1;check=((np.floor(u)+np.floor(v))%2)
warp=np.maximum(0,np.sin(math.pi*fu))**.7
weft=np.maximum(0,np.sin(math.pi*fv))**.7
height=np.where(check<.5,warp*(.72+.28*weft),weft*(.72+.28*warp))*.000060
height+=.000004*np.sin(u*math.pi*10)*np.sin(v*math.pi*10)
gy,gx=np.gradient(height,.04/N,.04/N)
norm=np.stack([-gx,-gy,np.ones_like(gx)],axis=-1);norm/=np.linalg.norm(norm,axis=-1)[:,:,None]
rng=np.random.default_rng(142)
base=.010*(.45+1.10*height/.00006)+rng.normal(0,.0006,(N,N))
def texture(name,rgb,filename,noncolor=False):
    im=bpy.data.images.new(name,width=N,height=N,alpha=True)
    if noncolor:im.colorspace_settings.name='Non-Color'
    rgba=np.ones((N,N,4),np.float32)
    rgba[:,:,:3]=rgb if rgb.ndim==3 else rgb[:,:,None]
    im.pixels.foreach_set(rgba.ravel());im.filepath_raw=str(OUT/filename);im.file_format='PNG';im.save();im.pack();return im
albedo=texture('Black nylon | woven colour 2K',np.stack([base*.92,base*.98,base],axis=-1),'BlackNylon_BaseColor.png')
normal=texture('Black nylon | basket weave tangent normal 2K',norm*.5+.5,'BlackNylon_Normal.png',True)
rough=texture('Black nylon | yarn roughness 2K',np.clip(.75-height*900,.56,.83),'BlackNylon_Roughness.png',True)
nodes=cloth.node_tree.nodes;links=cloth.node_tree.links
for image,target in [(albedo,'Base Color'),(rough,'Roughness')]:
    tex=nodes.new('ShaderNodeTexImage');tex.image=image;links.new(tex.outputs['Color'],p.inputs[target])
tex=nodes.new('ShaderNodeTexImage');tex.image=normal
nm=nodes.new('ShaderNodeNormalMap');nm.inputs['Strength'].default_value=.65
links.new(tex.outputs['Color'],nm.inputs['Color']);links.new(nm.outputs['Normal'],p.inputs['Normal'])
for mat in [polymer,steel,gate_mat]:
    n=mat.node_tree.nodes.new('ShaderNodeTexNoise');n.inputs['Scale'].default_value=350
    b=mat.node_tree.nodes.new('ShaderNodeBump');b.inputs['Distance'].default_value=.000025;b.inputs['Strength'].default_value=.15
    mat.node_tree.links.new(n.outputs['Fac'],b.inputs['Height']);mat.node_tree.links.new(b.outputs['Normal'],mat.node_tree.nodes.get('Principled BSDF').inputs['Normal'])

def mesh(name,verts,faces,mat,uv=None,smooth=True):
    me=bpy.data.meshes.new(name);me.from_pydata(verts,[],faces);me.update()
    bm=bmesh.new();bm.from_mesh(me);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(me);bm.free()
    o=bpy.data.objects.new(name,me);asset.objects.link(o);me.materials.append(mat)
    for f in me.polygons:f.use_smooth=smooth
    if uv:
        layer=me.uv_layers.new(name='UVMap')
        for f in me.polygons:
            for li,vi in zip(f.loop_indices,f.vertices):layer.data[li].uv=uv[vi]
    return o
def line(name,points,radius,mat,closed=False,detail=2):
    c=bpy.data.curves.new(name,'CURVE');c.dimensions='3D';c.bevel_depth=radius;c.bevel_resolution=detail
    s=c.splines.new('POLY');s.points.add(len(points)-1)
    for dst,src in zip(s.points,points):dst.co=(*src,1)
    s.use_cyclic_u=closed;o=bpy.data.objects.new(name,c);asset.objects.link(o);c.materials.append(mat);return o
def samples(points,n):
    pts=[Vector(v) for v in points];closed=(pts[0]-pts[-1]).length<1e-6
    p=([pts[-2]]+pts+[pts[1]]) if closed else ([pts[0]]+pts+[pts[-1]])
    out=[]
    for i in range(n):
        t=i/(n-1)*(len(p)-3);j=min(int(t),len(p)-4);t-=j
        a,b,c,d=p[j:j+4]
        out.append(.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*t*t+(-a+3*b-3*c+d)*t*t*t))
    return out
def ribbon(name,points,width,thick=.0015,n=400,standing=False,stitch=False,tilt=0,fixed_w=None):
    path=samples(points,n);verts=[];faces=[];uv=[];frames=[];distance=0
    cross=20
    for i,c in enumerate(path):
        if i:distance+=(c-path[i-1]).length
        t=(path[min(i+1,n-1)]-path[max(i-1,0)]).normalized()
        if standing:
            w=Vector((0,0,1))
        elif fixed_w is not None:
            w=Vector(fixed_w)
        else:
            w=DEPTH.cross(t)
            if w.length<.08 and frames:w=frames[-1][0].copy()
            if frames and w.dot(frames[-1][0])<0:w=-w
        w=(w-t*w.dot(t)).normalized() if fixed_w is None else w.normalized()
        normal=t.cross(w).normalized()
        if tilt:
            w=(w*math.cos(tilt)+normal*math.sin(tilt)).normalized();normal=t.cross(w).normalized()
            if fixed_w is None and normal.dot(DEPTH)<0:normal=-normal
        frames.append((w,normal))
        for sign in [1,-1]:
            for j in range(cross+1):
                f=j/cross;offset=(f-.5)*width
                # The substrate stays thin: slight transverse bow, not inflated rubber.
                bow=.00020*math.sin(f*math.pi)+.00008*math.sin(distance*97+f*5)
                verts.append(tuple(c+w*offset+normal*(sign*thick/2+sign*bow)))
                uv.append((distance/.04,f*width/.04))
    stride=2*(cross+1)
    for i in range(n-1):
        for k in range(2):
            for j in range(cross):
                a=i*stride+k*(cross+1)+j;b=a+stride
                faces.append((a,a+1,b+1,b) if k==0 else (a,b,b+1,a+1))
        for j in [0,cross]:faces.append((i*stride+j,(i+1)*stride+j,(i+1)*stride+cross+1+j,i*stride+cross+1+j))
    for i in [0,n-1]:
        start=i*stride
        for j in range(cross):faces.append((start+j,start+cross+1+j,start+cross+2+j,start+j+1))
    ob=mesh(name,verts,faces,cloth,uv)
    for sign in [-1,1]:
        seam=[tuple(c+frames[i][0]*(sign*(width/2-.0005))+frames[i][1]*(thick/2+.00015)) for i,c in enumerate(path)]
        line(name+' | raised woven edge',seam,.00019,edge)
        if stitch:
            for j in range(0,n-2,3):line(name+' | edge lockstitch',seam[j:j+2],.000095,thread,detail=1)
    return ob
def tube(name,points,width=.025,height=.013,n=420):
    path=samples(points,n);verts=[];faces=[];uv=[];r=48;distance=0
    for i,c in enumerate(path):
        if i:distance+=(c-path[i-1]).length
        t=(path[min(i+1,n-1)]-path[max(i-1,0)]).normalized();side=DEPTH.cross(t).normalized()
        up=t.cross(side).normalized()
        if up.dot(DEPTH)<0:up=-up
        s=i/(n-1);env=math.sin(math.pi*s)**.35
        phase=distance*2*math.pi/.009+1.3*math.sin(s*23)+.35*math.sin(s*61)
        for j in range(r):
            a=j*2*math.pi/r;wave=math.sin(phase+math.cos(a)*1.2)
            xx=math.copysign(abs(math.cos(a))**.75,math.cos(a))*width/2*(1+.12*env*wave)
            zz=math.copysign(abs(math.sin(a))**.8,math.sin(a))*height/2
            zz+=.0018*env*wave+.0004*env*math.sin(phase*1.8+2*a)
            verts.append(tuple(c+side*xx+up*zz));uv.append((distance/.04,j/r*(width*2+height*2)/.04))
    for i in range(n-1):
        for j in range(r):faces.append((i*r+j,i*r+(j+1)%r,(i+1)*r+(j+1)%r,(i+1)*r+j))
    faces.extend([tuple(reversed(range(r))),tuple((n-1)*r+j for j in range(r))])
    return mesh(name,verts,faces,cloth,uv)

def plane_matrix(pixel,angle,depth=0,scale=1):
    X=(RIGHT*math.cos(angle)+UP*math.sin(angle))*scale
    Y=(-RIGHT*math.sin(angle)+UP*math.cos(angle))*scale
    M=Matrix.Identity(4)
    for i,col in enumerate([X,Y,DEPTH*scale]):M.col[i]=(col.x,col.y,col.z,0)
    M.translation=pp(*pixel,depth);return M
def cube(name,M,centre,size,mat,bevel=.001):
    bpy.ops.mesh.primitive_cube_add(size=1);o=bpy.context.object;o.name=name;put(o)
    o.dimensions=size;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    o.matrix_world=M@Matrix.Translation(Vector(centre));o.data.materials.append(mat)
    if bevel:
        b=o.modifiers.new('Rounded manufactured edges','BEVEL');b.width=bevel;b.segments=4
        o.modifiers.new('Weighted face normals','WEIGHTED_NORMAL')
    return o
def solid(name,M,outline,depth,mat):
    k=len(outline);verts=[tuple(M@Vector((x,y,z))) for z in [-depth/2,depth/2] for x,y in outline]
    faces=[tuple(reversed(range(k))),tuple(range(k,2*k))]
    faces.extend((i,(i+1)%k,(i+1)%k+k,i+k) for i in range(k))
    return mesh(name,verts,faces,mat,smooth=False)
def rounded_rect(w,h,r=.003,n=8):
    pts=[]
    for cx,cy,a in [(w/2-r,h/2-r,0),(-w/2+r,h/2-r,90),(-w/2+r,-h/2+r,180),(w/2-r,-h/2+r,270)]:
        for i in range(n):
            t=math.radians(a+i*90/(n-1));pts.append((cx+r*math.cos(t),cy+r*math.sin(t)))
    return pts
def subtract(body,cut):
    bpy.context.view_layer.objects.active=body
    mod=body.modifiers.new('True machined opening','BOOLEAN');mod.operation='DIFFERENCE';mod.solver='EXACT';mod.object=cut
    bpy.ops.object.modifier_apply(modifier=mod.name);bpy.data.objects.remove(cut,do_unlink=True)
def disc(name,M,centre,radius,depth,mat):
    bpy.ops.mesh.primitive_cylinder_add(vertices=64,radius=radius,depth=depth)
    o=bpy.context.object;o.name=name;put(o);o.matrix_world=M@Matrix.Translation(Vector(centre));o.data.materials.append(mat)
    b=o.modifiers.new('Rivet edge rounding','BEVEL');b.width=.0003;b.segments=3;o.modifiers.new('Rivet normals','WEIGHTED_NORMAL');return o
def glide(name,pixel,w,h,angle,depth,tilt=0):
    M=plane_matrix(pixel,angle,depth)@Matrix.Rotation(tilt,4,'Y')
    body=solid(name,M,rounded_rect(w,h,.004),.0045,polymer)
    cut=solid('Adjustment slot cutter',M,rounded_rect(w-.006,h-.006,.002),.02,polymer);subtract(body,cut)
    b=body.modifiers.new('Moulded adjuster radius','BEVEL');b.width=.0010;b.segments=4
    body.modifiers.new('Adjuster normals','WEIGHTED_NORMAL')
    cube(name+' | friction centre bar',M,(0,0,.0004),(w-.004,.0035,.005),polymer,.001)
    return M
def hook(name,pixel,angle,scale):
    M=plane_matrix(pixel,angle,.027,scale)
    outline=[(-.017,.036),(.017,.036),(.019,.031),(.017,.018),(.012,.011),(.016,.003),(.017,-.021),(.014,-.030),(.008,-.035),(-.002,-.036),(-.012,-.031),(-.017,-.024),(-.018,.014),(-.020,.025)]
    body=solid(name+' | forged frame',M,outline,.0032,steel)
    # Enclosed webbing slot, distal oval opening, circular lightening hole and gate gap.
    slot=solid('Webbing slot cutter',M@Matrix.Translation((0,.026,0)),rounded_rect(.027,.010,.002),.02,steel);subtract(body,slot)
    oval=[(.010*math.cos(t),-.014+.017*math.sin(t)) for t in np.linspace(0,2*math.pi,96,endpoint=False)]
    subtract(body,solid('Hook opening cutter',M,oval,.02,steel))
    subtract(body,disc('Round relief hole cutter',M,(-.008,.011,0),.0045,.02,steel))
    gap=solid('Spring gate gap cutter',M,[(.002,.011),(.026,.011),(.026,-.010),(.009,-.010)],.02,steel);subtract(body,gap)
    b=body.modifiers.new('Forged hook edge radii','BEVEL');b.width=.0008;b.segments=4
    body.modifiers.new('Steel face normals','WEIGHTED_NORMAL')
    gate=solid(name+' | separate spring gate',M@Matrix.Translation((0,0,.0027)),[(.001,.014),(.007,.017),(.019,.003),(.020,-.006),(.014,-.011),(.004,.007)],.0018,gate_mat)
    b=gate.modifiers.new('Pressed gate edge break','BEVEL');b.width=.00045;b.segments=3;gate.modifiers.new('Gate normals','WEIGHTED_NORMAL')
    disc(name+' | pivot rivet',M,(.004,.012,.0042),.0038,.0015,steel)
    disc(name+' | rivet centre',M,(.004,.012,.005),.0012,.0007,rivet_mat)
    for k in range(8):
        yy=.007-k*.0016
        cube(name+' | gate grip serration',M,(.0155-k*.00045,yy,.0040),(.0010,.00065,.0008),gate_mat,.0002)
    return M
def box_x(name,pixel,angle,width,length,depth):
    M=plane_matrix(pixel,angle,depth)
    pts=[(-width/2,-length/2,0),(width/2,-length/2,0),(width/2,length/2,0),(-width/2,length/2,0)]
    world=[M@Vector(p) for p in pts]
    # Individual stitches rather than a solid painted X.
    def stitches(a,b):
        count=max(2,int((b-a).length/.0009))
        for i in range(count):
            t=i/count;line(name,[a+(b-a)*t,a+(b-a)*min(1,t+.55/count)],.00010,thread,detail=1)
    for i in range(4):stitches(world[i],world[(i+1)%4])
    stitches(world[0],world[2]);stitches(world[1],world[3])

# Front rolled webbing: a real upright loop, not a flattened painted torus.
roll=[]
for t in np.linspace(-math.pi/2,3*math.pi/2,13):
    px=230+177*math.cos(t);py=475+228*math.sin(t)
    up=(600-py)*S
    # Keep the centreline approximately in a horizontal plane; width rises vertically.
    depth=(.070-up*math.cos(theta))/math.sin(theta)
    roll.append(pp(px,py,depth))
ribbon('01 | upright rolled central webbing',roll,.044,.0016,n=650,standing=True)
# The reference has broad coiled loops, not tight decorative curls.
inner=[]
for a in np.linspace(-math.pi/2,3*math.pi/2,17):
    px=207+123*math.cos(a);py=365+166*math.sin(a)
    up=(600-py)*S
    depth=(.078-up*math.cos(theta))/math.sin(theta)-.018
    inner.append(pp(px,py,depth))
ribbon('02 | continuous folded inner adjustment loop',inner,.031,.0014,n=700,standing=True)
def threaded_adjuster(name,pixel,angle,width,height,depth,before,after):
    M=glide(name+' | correctly aligned frame',pixel,width+.008,height,angle,depth)
    route=[M@Vector((0,y,z)) for y,z in [(.045,-.0048),(.010,-.0048),(.006,.0048),(-.006,.0048),(-.010,-.0048),(-.045,-.0048)]]
    ribbon(name+' | continuous over-under webbing',before+route+after,width,.0014,n=650,fixed_w=M.to_3x3()@Vector((1,0,0)))
    return M
threaded_adjuster('03 | upper rear adjuster',(326,145),.91,.027,.025,-.025,
    [pp(183,240,-.070),pp(120,175,-.060),pp(126,88,-.050),pp(172,60,-.040),pp(208,53,-.030)], [pp(414,243,-.020),pp(428,255,-.018)])
threaded_adjuster('04 | left return adjuster',(75,410),.12,.027,.028,.010,
    [pp(170,240,-.070),pp(83,238,-.050),pp(49,290,-.015)], [pp(98,566,.009),pp(116,618,-.045),pp(196,624,-.070)])
threaded_adjuster('05 | centre adjustment frame',(283,352),.08,.028,.030,.061,
    [pp(240,280,-.060),pp(269,217,-.025),pp(301,193,.020)], [pp(263,499,.055),pp(245,558,-.040),pp(199,560,-.070)])
# Paired elastic arms, with thin woven cloth gathered over the bungee.
tube('06 | upper gathered elastic sleeve',[pp(393,219,-.006),pp(468,277,.004),pp(548,332,.012),pp(635,401,.018),pp(720,466,.020),pp(805,524,.020)],.025,.013,n=450)
tube('07 | lower gathered elastic sleeve',[pp(348,260,.01),pp(391,347,.02),pp(447,449,.025),pp(498,531,.025),pp(580,637,.023),pp(677,728,.021),pp(783,814,.020)],.025,.014,n=500)
# Each continuous end tab passes through the actual slot and doubles back
# behind the upper rail. Both layers are sewn together above the hardware.
def threaded_hook_tab(name,pixel,angle,scale,start):
    M=hook(name,pixel,angle,scale)
    front=[M@Vector((0,y,.0048)) for y in [.082,.068,.050,.035,.026]]
    turn=[M@Vector((0,.026-.0045*math.sin(a),.0048*math.cos(a))) for a in np.linspace(0,math.pi,17)]
    back=[M@Vector((0,y,-.0048)) for y in [.035,.050,.070,.082]]
    ribbon(name+' | continuous slot-threaded folded tab',[start]+front+turn[1:]+back+[start-DEPTH*.003],.025*scale,.0012,n=650,fixed_w=M.to_3x3()@Vector((1,0,0)))
    centre=M@Vector((0,.067,.0056))
    # pixel position computed from the same hardware frame; no floating stitches.
    px=600+centre.dot(RIGHT)/S;py=600-centre.dot(UP)/S;dd=centre.dot(DEPTH)
    box_x(name+' | double-layer box-X seam',(px,py),angle,.020*scale,.032*scale,dd)
    for z in [.0048,-.0048]:
        for sign in [-1,1]:
            line(name+' | folded tab selvedge',[M@Vector((sign*.0122*scale,y,z)) for y in np.linspace(.035,.085,40)],.00015,edge)
    return M
threaded_hook_tab('10 | upper HK spring snap hook',(1018,705),.93,1.0,pp(784,509,.021))
threaded_hook_tab('11 | lower HK spring snap hook',(1053,1040),.74,1.15,pp(752,785,.023))
root=bpy.data.objects.new('BCM_Black_TwoHook_BungeeSling_INVENTORY_ONLY',None);asset.objects.link(root)
for o in list(asset.objects):
    if o!=root:o.parent=root
root['reference']='User jag1-23-1200x1200.jpg; original geometry/textures, no photo projection'
root['usage']='Static inventory asset. No equipping, character rig or animation.'
root['status']='Art review, not native-game validated'

scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=96;scene.cycles.use_denoising=True
scene.render.threads_mode='FIXED';scene.render.threads=8
prefs=bpy.context.preferences.addons['cycles'].preferences;devices=[]
for backend in ['OPTIX','CUDA','HIP']:
    try:
        prefs.compute_device_type=backend;prefs.get_devices();gpu=[d for d in prefs.devices if d.type!='CPU']
        if gpu:
            for d in prefs.devices:d.use=d.type!='CPU'
            devices=list(dict.fromkeys(d.name for d in gpu));scene.cycles.device='GPU';break
    except Exception:pass
if not devices:scene.cycles.device='CPU'
def aim(o,p):o.rotation_euler=(Vector(p)-o.location).to_track_quat('-Z','Y').to_euler()
camdata=bpy.data.cameras.new('Inventory reference camera');cam=bpy.data.objects.new(camdata.name,camdata);studio.objects.link(cam)
cam.location=DEPTH*1.7;aim(cam,(0,0,0));camdata.type='ORTHO';camdata.ortho_scale=.39;scene.camera=cam
for name,loc,power,size in [('Large upper-left softbox',(-.32,-.38,.65),6.72,.55),('Right textile rake',(.35,.02,.32),3.92,.36),('Back rim',(-.10,.34,.45),4.76,.40)]:
    d=bpy.data.lights.new(name,'AREA');d.energy=power;d.shape='DISK';d.size=size
    o=bpy.data.objects.new(name,d);studio.objects.link(o);o.location=loc;aim(o,(-.03,0,.04))
scene.world.color=(.10,.10,.10);scene.render.film_transparent=True;scene.render.image_settings.file_format='PNG';scene.render.image_settings.color_mode='RGBA'
scene.view_settings.exposure=.65;scene.view_settings.view_transform='AgX';scene.unit_settings.system='METRIC'
scene.render.resolution_x=1800;scene.render.resolution_y=1800;scene.render.resolution_percentage=100
scene.render.filepath=str(OUT/'Black_Bungee_Sling_Preview.png')
for screen in bpy.data.screens:
    for a in screen.areas:
        if a.type=='VIEW_3D':
            a.spaces.active.region_3d.view_location=(-.03,0,.05);a.spaces.active.region_3d.view_distance=.65
            a.spaces.active.region_3d.view_rotation=cam.rotation_euler.to_quaternion();a.spaces.active.shading.type='MATERIAL'
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_Black_TwoHook_BungeeSling.blend'))
bpy.ops.render.render(write_still=True)
report={'static_inventory_only':True,'photo_texture':False,'watermark':False,'backdrop_geometry':False,
        'animation_actions':len(bpy.data.actions),'armatures':len(bpy.data.armatures),
        'render_devices':devices,'woven_texture_size':2048,'native_game_validation':False}
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
print('BLACK_SLING_BUILD_COMPLETE',json.dumps(report),flush=True)






