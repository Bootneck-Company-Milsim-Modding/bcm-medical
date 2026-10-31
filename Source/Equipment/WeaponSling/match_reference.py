"""Reference-registered, volumetric inventory sling; real mesh alpha is measured.
No photo card, alpha cutout or reference-photo texture is used on the model.
"""
import bpy,bmesh,math,json,os
from pathlib import Path
from mathutils import Vector
import numpy as np
OUT=Path(__file__).resolve().parent
REFERENCE=Path(r'C:\Users\ruanb\Downloads\Wilde_Custom_Gear_Single_Point_Sling_QD_Multicam_AR_15_Sling.webp')
# Reuse our original mesh/material building utilities, not previously built geometry.
builder=(OUT/'build_sling.py').read_text()
exec(compile(builder.split('# Padded rear shoulder panel:')[0],str(OUT/'build_sling.py'),'exec'))
# Original multi-terrain print: soft olive/tan transitions, brown islands and
# small light/dark flecks, rather than the early hard-edged woodland study.
blend=np.clip(.5+g*.8+warp*.12,0,1)
olive=np.array([.36,.40,.27],np.float32);khaki=np.array([.60,.56,.43],np.float32)
rgb=olive[None,None,:]*(1-blend[:,:,None])+khaki[None,None,:]*blend[:,:,None]
def overlay_color(rgb,mask,color):return rgb*(1-mask[:,:,None])+np.array(color)[None,None,:]*mask[:,:,None]
soft=lambda value,threshold,width:np.clip((value-threshold)/width+.5,0,1)
rgb=overlay_color(rgb,soft(f+h*.2,.27,.07),[.35,.28,.21])
rgb=overlay_color(rgb,soft(h+f*.3,.48,.035),[.20,.185,.14])
rgb=overlay_color(rgb,soft(-h+g*.2,.43,.035),[.70,.66,.52])
rgb=np.clip(rgb*(1+.10*np.sin(xx*2*math.pi*640)[:,:,None]*np.sin(yy*2*math.pi*640)[:,:,None]+grain[:,:,None]*2),0,1)
rgba=np.ones((N,N,4),np.float32);rgba[:,:,:3]=np.where(rgb<=.04045,rgb/12.92,((rgb+.055)/1.055)**2.4)
matched_albedo=image('BCM original multi-terrain nylon | packed 2K',rgba,'Sling_Multiterrain_BaseColor.png')
for node in cloth.node_tree.nodes:
    if node.type=='TEX_IMAGE' and node.image==albedo:node.image=matched_albedo
normal_node=next(n for n in cloth.node_tree.nodes if n.type=='NORMAL_MAP')
normal_node.inputs['Strength'].default_value=.65
ref=bpy.data.images.load(str(REFERENCE),check_existing=True)
W,H=ref.size
photo=np.array(ref.pixels[:],np.float32).reshape(H,W,4)
reference_mask=photo[:,:,:3].max(axis=2)<.94
def wp(x,y,z=.03):return Vector(((x-W/2)*.001,(H/2-y)*.001,z))
def photo_xy(p):return np.array([p.x*1000+W/2,H/2-p.y*1000])
def inside(x,y):
    ix=int(round(x));iy=H-1-int(round(y))
    return 0<=ix<W and 0<=iy<H and reference_mask[iy,ix]
def trace_width(center,side,limit,ymax=None):
    # Fit the physical mesh envelope, not shader alpha, to the reference silhouette.
    xy=photo_xy(center);direction=np.array([side.x,-side.y])
    if not inside(*xy):return center,None
    spans=[]
    for sign in [-1,1]:
        last=0
        for dist in np.arange(.5,limit*1000,.5):
            point=xy+direction*dist*sign
            if (ymax is not None and point[1]>ymax) or not inside(*point):break
            last=dist
        spans.append(last*.001)
    if min(spans)<.002:return center,None
    # Account for half-pixel sampling on each side of the measured opaque edge.
    return center+side*((spans[1]-spans[0])*.5),sum(spans)+.0015

def matched_strap(name,pixels,width,thickness,kind='flat',count=250,radial=32,fit=True):
    path=sample([wp(*p) for p in pixels],count);verts=[];faces=[];uv=[]
    widths=[];fitted=[];frames=[];length=0
    for i,c in enumerate(path):
        tangent=(path[min(i+1,count-1)]-path[max(i-1,0)]).normalized()
        side=tangent.cross(Vector((0,0,1))).normalized();up=side.cross(tangent).normalized()
        fitted_center,fitwidth=(trace_width(c,side,width*2.2,565 if kind=='elastic' else None) if fit else (c,width))
        if kind=='elastic' and i/(count-1)>.88:
            blend=(i/(count-1)-.88)/.12
            fitted_center=fitted_center*(1-blend)+c*blend
            fitwidth=(fitwidth or width)*(1-blend)+width*.9*blend
        widths.append(max(width*.35,min(width*2.5,fitwidth or width)))
        fitted.append(fitted_center);frames.append((side,up))
    # Preserve real irregular sleeve edges with a modest 3-section smoothing pass.
    kernel=np.array([1,3,6,8,6,3,1],float);kernel/=kernel.sum()
    widths=np.convolve(np.pad(widths,(3,3),mode='edge'),kernel,mode='valid')
    fitted=[Vector(v) for v in np.stack([np.convolve(np.pad(np.array(fitted)[:,axis],(3,3),mode='edge'),kernel,mode='valid') for axis in range(3)],axis=1)]
    for i,p in enumerate(fitted):
        if i:length+=(p-fitted[i-1]).length
        side,up=frames[i];t=i/(count-1)
        phase=length*math.pi*2/.014+1.0*math.sin(t*14)+.35*math.sin(t*43)
        envelope=math.sin(math.pi*t)**.35
        for j in range(radial):
            a=2*math.pi*j/radial
            exponent=.9 if kind=='elastic' else .32
            x=math.copysign(abs(math.cos(a))**exponent,math.cos(a))*widths[i]/2
            z=math.copysign(abs(math.sin(a))**(.9 if kind=='elastic' else .45),math.sin(a))*thickness/2
            if kind=='elastic':
                # A rounded elastic core with a loose crumpled cover, not a flat accordion.
                z*=1+.16*envelope*math.sin(phase+math.cos(a)*1.7)
                z+=.0025*envelope*math.sin(phase+math.cos(a)*2.5)
                z+=.0008*envelope*math.sin(phase*2.35-a*3)
            verts.append(tuple(p+side*x+up*z))
            uv.append((length/.19,j/(radial-1)*(width*2+thickness*2)/.19))
    for i in range(count-1):
        for j in range(radial):
            k=(j+1)%radial;faces.append((i*radial+j,i*radial+k,(i+1)*radial+k,(i+1)*radial+j))
    faces.extend([tuple(reversed(range(radial))),tuple((count-1)*radial+j for j in range(radial))])
    obj=mesh_obj(name,verts,faces,cloth,uv)
    if kind=='flat':
        for sign in [-1,1]:
            pts=[tuple(p+frames[i][0]*(sign*(widths[i]/2-.0018))+frames[i][1]*(thickness/2+.00015)) for i,p in enumerate(fitted)]
            curve(name+' | seam',pts,.00015,thread)
    obj['construction']='Round elastic core with gathered sleeve' if kind=='elastic' else 'Flat woven webbing / padded flat panel'
    return obj

matched_strap('01 | flat padded shoulder panel',[(111,254,.035),(145,237,.041),(207,212,.046),(259,194,.05),(281,193,.052),(316,205,.046),(383,226,.04),(445,238,.035),(468,242,.032)],.055,.009,count=350)
matched_strap('02 | flat left adjustment webbing',[(114,253,.03),(90,265,.027),(72,289,.027),(58,315,.027),(43,331,.027),(53,349,.027),(88,365,.03),(120,379,.031)],.027,.0028,count=250)
matched_strap('03 | flat right adjustment webbing',[(468,240,.03),(491,251,.03),(507,270,.028),(518,289,.029),(510,311,.029),(488,324,.03),(472,336,.03)],.024,.0028,count=200)
matched_strap('04 | rounded gathered left elastic sleeve',[(123,375,.032),(143,415,.033),(183,452,.033),(226,484,.032),(270,514,.033),(312,533,.034),(345,536,.035)],.045,.031,'elastic',420)
matched_strap('05 | rounded gathered right elastic sleeve',[(474,332,.03),(466,371,.032),(460,412,.033),(448,455,.034),(433,491,.034),(406,522,.034),(383,537,.035),(369,536,.035)],.041,.031,'elastic',420)

triglide('Left coyote adjustment slider',wp(43,338,.032),.016,.044,-.11)
triglide('Right coyote adjustment slider',wp(523,310,.032),.014,.040,-.20)
matched_strap('06 | short takeoff webbing',[(354,531,.038),(365,543,.038),(385,555,.038)],.020,.0035,count=100,fit=False)
matched_strap('07 | hanging flat connector tape',[(398,604,.031),(400,631,.031),(403,651,.03),(414,689,.03),(416,722,.03),(406,750,.031),(400,773,.03)],.029,.0035,count=190)
matched_strap('08 | folded swivel tape',[(409,744,.033),(412,766,.033),(410,785,.035),(399,794,.037),(393,786,.043),(402,751,.043)],.0254,.003,count=140,fit=False)

# Solid hardware contours traced from the visible source silhouette, with real
# depth and bevels. Hidden surfaces remain an informed reconstruction, not CAD.
def simplify(points,epsilon=.65):
    if len(points)<=2:return points
    a=np.array(points[0],float);b=np.array(points[-1],float)
    d=b-a;den=max(1e-10,float(d@d))
    dist=[np.linalg.norm(np.array(p)-a-d*np.clip((np.array(p)-a)@d/den,0,1)) for p in points]
    i=int(np.argmax(dist))
    if dist[i]<=epsilon:return [points[0],points[-1]]
    return simplify(points[:i+1],epsilon)[:-1]+simplify(points[i:],epsilon)
def silhouette_prism(name,bounds,z,depth,material):
    x0,y0,x1,y1=bounds
    localmask=np.flipud(reference_mask)[y0:y1,x0:x1]
    edges={}
    def edge(a,b):edges.setdefault(a,[]).append(b)
    for y,x in np.argwhere(localmask):
        if y==0 or not localmask[y-1,x]:edge((x,y),(x+1,y))
        if x==localmask.shape[1]-1 or not localmask[y,x+1]:edge((x+1,y),(x+1,y+1))
        if y==localmask.shape[0]-1 or not localmask[y+1,x]:edge((x+1,y+1),(x,y+1))
        if x==0 or not localmask[y,x-1]:edge((x,y+1),(x,y))
    loops=[]
    while edges:
        start=next(iter(edges));cur=start;loop=[]
        for _ in range(10000):
            loop.append(cur)
            if cur not in edges:break
            nxt=edges[cur].pop()
            if not edges[cur]:del edges[cur]
            cur=nxt
            if cur==start:break
        if len(loop)>3:loops.append(loop)
    outline=max(loops,key=len)
    outline=simplify(outline+[outline[0]],.65)[:-1]
    n=len(outline);verts=[]
    for level in [z-depth/2,z+depth/2]:
        verts.extend(tuple(wp(x+x0,y+y0,level)) for x,y in outline)
    faces=[tuple(reversed(range(n))),tuple(range(n,n*2))]
    faces.extend((i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n))
    obj=mesh_obj(name,verts,faces,material)
    bevel=obj.modifiers.new('Rounded manufactured edges','BEVEL');bevel.width=.00055;bevel.segments=3
    obj.modifiers.new('Weighted housing normals','WEIGHTED_NORMAL')
    return obj

# Manufacturer detail reference: real assembled side-release male/female parts.
c=wp(397,599,.04);angle=.25
def hw(x,y,z=0):
    return c+Vector((x*math.cos(angle)-y*math.sin(angle),x*math.sin(angle)+y*math.cos(angle),z))
def hardware_solid(name,outline,z,thick,material,bevel=.001):
    n=len(outline)
    vertices=[tuple(hw(x,y,level)) for level in [z-thick/2,z+thick/2] for x,y in outline]
    faces=[tuple(reversed(range(n))),tuple(range(n,2*n))]
    faces.extend((i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n))
    ob=mesh_obj(name,vertices,faces,material)
    for p in ob.data.polygons:p.use_smooth=False
    mod=ob.modifiers.new('Moulded edge radius','BEVEL');mod.width=bevel;mod.segments=4
    ob.modifiers.new('Manufactured surface normals','WEIGHTED_NORMAL')
    return ob
# Shield-shaped female shell, narrowing around the central male spine.
housing=[(-.008,.024),(.008,.024),(.009,.015),(.013,.006),(.016,-.003),(.016,-.012),(-.016,-.012),(-.016,-.003),(-.013,.006),(-.009,.015)]
hardware_solid('Side-release | shield-shaped female shell',housing,.001,.009,tan,.0015)
box('Side-release | recessed cavity back wall',hw(0,.015,-.004),(.024,.018,.0025),dark,.001,angle)
# Separate resilient male arms and their hooked release shoulders.
box('Side-release | central male spine',hw(0,.030,-.001),(.005,.028,.004),tan,.0007,angle)
for sign in [-1,1]:
    arm=[(sign*x,y) for x,y in [(.009,.042),(.013,.039),(.016,.030),(.016,.023),(.012,.017),(.010,.020),(.011,.031)]]
    if sign<0:arm.reverse()
    hardware_solid('Side-release | sprung male release arm',arm,.001,.005,tan,.0008)
# Upper webbing eye, actual open slot rather than a solid tab.
for sign in [-1,1]:
    box('Male buckle | webbing eye side',hw(sign*.012,.045),(.003,.014,.006),tan,.0009,angle)
    box('Male buckle | webbing eye bar',hw(0,.045+sign*.006),(.027,.003,.006),tan,.0009,angle)
# Lower ladder-lock: clear slot and textured friction bar.
for sign in [-1,1]:
    box('Female buckle | lower ladder-lock rail',hw(sign*.0145,-.022),(.003,.021,.007),tan,.001,angle)
box('Female buckle | lower crossbar',hw(0,-.032),(.031,.004,.007),tan,.001,angle)
box('Female buckle | friction crossbar',hw(0,-.024,.0005),(.029,.004,.008),tan,.0008,angle)
for x in np.linspace(-.011,.011,12):
    box('Female buckle | friction tooth',hw(float(x),-.024,.005),(.0007,.003,.0006),tan,.0002,angle)
# Rounded continuous fabric through the bungee convergence, concealing cut ends.
matched_strap('09 | gathered elastic convergence',[(330,533,.036),(350,537,.036),(376,539,.036)],.037,.030,'elastic',150)

# Grovtec-style QD: forged wire bail, knurled collar, cylindrical stud and detents.
qd_origin=wp(395,805,.035);qd_angle=-.55
def qpt(x,y,z=0):
    return qd_origin+Vector((x*math.cos(qd_angle)-y*math.sin(qd_angle),x*math.sin(qd_angle)+y*math.cos(qd_angle),z))
qd_axis=Vector((-math.sin(qd_angle),math.cos(qd_angle),0))
bail=[(-.008,.006),(-.0135,.010),(-.0152,.017),(-.0145,.027),(-.010,.034),(.010,.034),(.0145,.027),(.0152,.017),(.0135,.010),(.008,.006)]
# Smooth the forged wire loop without flattening its cylindrical section.
wire=[]
pts=[Vector((x,y,0)) for x,y in bail]
for _ in range(3):
    smooth=[]
    for i,p in enumerate(pts):
        nxt=pts[(i+1)%len(pts)]
        smooth.extend([p*.75+nxt*.25,p*.25+nxt*.75])
    pts=smooth
wire=[qpt(p.x,p.y,.001) for p in pts]
curve('QD swivel | rounded forged wire bail',wire,.0023,black,True,4)
cylinder('QD swivel | knurled locking collar',qpt(0,.001),.0085,.007,black,qd_axis)
cylinder('QD swivel | cylindrical locking stud',qpt(0,-.012),.0065,.020,black,qd_axis)
cylinder('QD swivel | push button',qpt(0,.0065),.0048,.004,buttonmat,qd_axis)
cylinder('QD swivel | lower machined shoulder',qpt(0,-.020),.0069,.0025,black,qd_axis)
# Diamond knurl is genuine shallow geometry around the collar.
for j in range(40):
    a=j*2*math.pi/40
    for direction in [-1,1]:
        points=[]
        for k in range(7):
            yy=-.0025+k*.001
            aa=a+direction*(yy+.0025)*105
            points.append(qpt(.00865*math.cos(aa),yy,.00865*math.sin(aa)))
        curve('QD swivel | diamond knurl',points,.00011,buttonmat,res=1)
for a in [0,math.pi/2,math.pi,math.pi*1.5]:
    loc=qpt(.0063*math.cos(a),-.014,.0063*math.sin(a))
    bpy.ops.mesh.primitive_uv_sphere_add(segments=16,ring_count=8,radius=.0015,location=loc)
    ob=bpy.context.object;ob.name='QD swivel | spring-loaded locking ball';move_to(ob,hero);ob.data.materials.append(buttonmat)
pose_file=OUT/'hardware-pose-fit.json'
if pose_file.exists():
    from mathutils import Matrix
    pose_data=json.loads(pose_file.read_text())
    for label,parameters in pose_data.items():
        prefixes=('QD swivel |',) if label=='QD' else ('Side-release |','Male buckle |','Female buckle |')
        pivot=wp(*parameters['pivot_pixels'],.035)
        dx,dy=parameters['translation_pixels']
        transform=Matrix.Translation(pivot+Vector((dx*.001,-dy*.001,0)))@Matrix.Rotation(parameters['rotation_z_radians'],4,'Z')@Matrix.Scale(parameters['uniform_scale'],4)@Matrix.Translation(-pivot)
        for obj in hero.objects:
            if obj.name.startswith(prefixes):obj.matrix_world=transform@obj.matrix_world

root=bpy.data.objects.new('BCM_Sling_ReferenceMatched_InventoryOnly',None);hero.objects.link(root)
for obj in list(hero.objects):
    if obj!=root:obj.parent=root
root['reference_registration']='600x1000 source image; 1mm per pixel; actual volumetric mesh outline'
root['reference_photo_texture']=False
root['inventory_only']=True
root.scale=(.925,.925,.925)
root['scale_note']='Approximate photo scale; nominal padded strap is 2 inches per manufacturer, not a dimensioned CAD reconstruction.'

def aim(obj,point):obj.rotation_euler=(Vector(point)-obj.location).to_track_quat('-Z','Y').to_euler()
for name,loc,power,size in [('Key',(-.3,-.2,.9),12,.7),('Fill',(.4,.3,.6),6,.6)]:
    d=bpy.data.lights.new(name,'AREA');d.energy=power;d.size=size
    o=bpy.data.objects.new(name,d);studio.objects.link(o);o.location=loc;aim(o,(0,0,.03))
data=bpy.data.cameras.new('Reference registered camera');cam=bpy.data.objects.new(data.name,data);studio.objects.link(cam)
cam.location=(0,0,2);cam.rotation_euler=(0,0,0);data.type='ORTHO';data.ortho_scale=.925
scene=bpy.context.scene;scene.camera=cam;scene.render.engine='CYCLES';scene.render.film_transparent=True
scene.render.resolution_x=W;scene.render.resolution_y=H;scene.render.resolution_percentage=100
scene.render.threads_mode='FIXED';scene.render.threads=8;scene.cycles.samples=32;scene.cycles.use_denoising=True
scene.render.image_settings.file_format='PNG';scene.render.image_settings.color_mode='RGBA'
scene.world.color=(.1,.1,.1);scene.view_settings.view_transform='AgX'
devices=[]
prefs=bpy.context.preferences.addons['cycles'].preferences
for backend in ['OPTIX','CUDA','HIP']:
    try:
        prefs.compute_device_type=backend;prefs.get_devices()
        gpu=[d for d in prefs.devices if d.type!='CPU']
        if gpu:
            for d in prefs.devices:d.use=d.type!='CPU'
            devices=list(dict.fromkeys(d.name for d in gpu));scene.cycles.device='GPU';break
    except Exception:pass
if not devices:scene.cycles.device='CPU'
print('RENDER_DEVICES',devices or ['CPU, eight threads'],flush=True)
scene.render.filepath=str(OUT/'Sling_ReferenceMatched_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_Sling_ReferenceMatched.blend'))
bpy.ops.render.render(write_still=True)
render=bpy.data.images.load(str(OUT/'Sling_ReferenceMatched_Preview.png'),check_existing=False)
rendered=np.array(render.pixels[:],np.float32).reshape(H,W,4)
mask=rendered[:,:,3]>.5
intersection=np.logical_and(mask,reference_mask).sum();union=np.logical_or(mask,reference_mask).sum()
def expand(m,r=3):
    out=np.zeros_like(m);p=np.pad(m,r)
    for dy in range(2*r+1):
        for dx in range(2*r+1):out|=p[dy:dy+H,dx:dx+W]
    return out
report={'reference_dimensions':[W,H],'silhouette_iou':float(intersection/union),
        'model_pixels_within_3px_of_reference':float((mask&expand(reference_mask)).sum()/max(1,mask.sum())),
        'reference_pixels_within_3px_of_model':float((reference_mask&expand(mask)).sum()/max(1,reference_mask.sum())),
        'comparison':'Actual opaque volumetric mesh alpha; no photo alpha cutout or photo texture',
        'gpu_devices':devices,'construction':'Flat padded upper webbing, round gathered elastic sleeves',
        'near_exact_confirmed':False}
def boundary(m):
    p=np.pad(m,1);inside=np.ones_like(m)
    for dy in range(3):
        for dx in range(3):inside&=p[dy:dy+H,dx:dx+W]
    return m&~inside
def edge_distances(a,b):
    aa=np.argwhere(a).astype(np.float32);bb=np.argwhere(b).astype(np.float32)
    result=[]
    for start in range(0,len(aa),128):
        d=aa[start:start+128,None,:]-bb[None,:,:]
        result.extend(np.sqrt((d*d).sum(axis=2).min(axis=1)).tolist())
    return np.array(result)
model_edge=boundary(mask);reference_edge=boundary(reference_mask)
dist_a=edge_distances(model_edge,reference_edge);dist_b=edge_distances(reference_edge,model_edge)
report['model_boundary_p95_distance_px']=float(np.percentile(dist_a,95))
report['reference_boundary_p95_distance_px']=float(np.percentile(dist_b,95))
report['model_boundary_within_3px']=float(np.mean(dist_a<=3))
report['reference_boundary_within_3px']=float(np.mean(dist_b<=3))
report['reference_mask_threshold']='Max RGB below 0.94; model alpha above 0.5'
(OUT/'reference-comparison.json').write_text(json.dumps(report,indent=2))
# Honest overlay: source photo plus a cyan-tinted rendering of our real silhouette.
overlay=photo.copy();tint=np.zeros_like(photo);tint[:,:,:3]=(.02,.65,.85);tint[:,:,3]=1
overlay[mask,:3]=photo[mask,:3]*.5+tint[mask,:3]*.5
overlay[:,:,3]=1
im=bpy.data.images.new('Reference alignment overlay',width=W,height=H,alpha=True)
im.pixels.foreach_set(overlay.ravel());im.filepath_raw=str(OUT/'Sling_Reference_Overlay.png');im.file_format='PNG';im.save()
print('REFERENCE_COMPARISON',json.dumps(report),flush=True)

# Export a separate reduced mesh while retaining editable detailed source.
export_collection=bpy.data.collections.new('EXPORT | reference-matched inventory mesh')
scene.collection.children.link(export_collection)
parts=[];deps=bpy.context.evaluated_depsgraph_get()
for obj in list(hero.objects):
    if obj.type not in {'MESH','CURVE'}:continue
    mesh=bpy.data.meshes.new_from_object(obj.evaluated_get(deps),depsgraph=deps)
    copy=bpy.data.objects.new(obj.name+' | export',mesh);export_collection.objects.link(copy)
    copy.matrix_world=obj.matrix_world;parts.append(copy)
bpy.ops.object.select_all(action='DESELECT')
for obj in parts:obj.select_set(True)
bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join()
low=bpy.context.object;low.name='BCM_Sling_ReferenceMatched_Inventory'
tri=low.modifiers.new('Export triangles','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
detail_triangles=len(low.data.polygons)
dec=low.modifiers.new('Inventory reduction candidate','DECIMATE');dec.ratio=min(1,12000/max(1,detail_triangles))
bpy.ops.object.modifier_apply(modifier=dec.name)
tri=low.modifiers.new('Final triangles','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
bpy.ops.export_scene.fbx(filepath=str(OUT/'BCM_Sling_ReferenceMatched_Inventory.fbx'),use_selection=True,
    object_types={'MESH'},add_leaf_bones=False,bake_anim=False,path_mode='RELATIVE',mesh_smooth_type='FACE')
export_collection.hide_render=True;export_collection.hide_viewport=True
report['detailed_triangles']=detail_triangles
report['inventory_candidate_triangles']=len(low.data.polygons)
report['armatures']=len(bpy.data.armatures);report['animation_actions']=len(bpy.data.actions)
report['hardware_note']='Manufacturer close-up used for separate female shell, sprung male arms, open lower ladder-lock, forged QD wire bail, knurled cylindrical collar/stud, button and detents. Hidden dimensions remain reconstructed.'
report['hardware_pose_fit']=json.loads(pose_file.read_text()) if pose_file.exists() else None
report['silhouette_near_exact']=report['silhouette_iou']>=.95
report['near_exact_confirmed']=False # Overall appearance/hidden hardware is not established by silhouette alone.
report['overall_perfect_replica_confirmed']=False
(OUT/'reference-comparison.json').write_text(json.dumps(report,indent=2))
# Do not embed the source photograph or overlay as model textures.
for img in list(bpy.data.images):
    if img.users==0 and img.type!='RENDER_RESULT' and not img.packed_file:bpy.data.images.remove(img)
scene.render.resolution_x=1200;scene.render.resolution_y=2000
scene.render.filepath=str(OUT/'Sling_ReferenceMatched_HighQuality.png')
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_Sling_ReferenceMatched.blend'))
bpy.ops.render.render(write_still=True)
# Hardware closeup at an oblique angle shows actual volume, not a photo card.
cam.location=(.25,-.43,.38);aim(cam,(.087,-.17,.035));data.ortho_scale=.29
scene.render.resolution_x=900;scene.render.resolution_y=1200
scene.render.filepath=str(OUT/'Sling_Hardware_Closeup.png')
bpy.ops.render.render(write_still=True)
print('REFERENCE_MATCH_EXPORT_COMPLETE',json.dumps(report),flush=True)
