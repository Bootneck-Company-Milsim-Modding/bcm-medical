"""Material-only revision; retain the approved mesh exactly."""
import bpy, numpy as np, math, hashlib, json, shutil
from pathlib import Path
OUT=Path(bpy.data.filepath).parent
backup=OUT/'BCM_Black_TwoHook_BungeeSling_ApprovedShape.blend'
if not backup.exists():shutil.copy2(bpy.data.filepath,backup)
def shape_hash():
    h=hashlib.sha256()
    for ob in sorted(bpy.data.objects,key=lambda o:o.name):
        if ob.type=='MESH':
            h.update(ob.name.encode())
            a=np.empty(len(ob.data.vertices)*3,np.float32);ob.data.vertices.foreach_get('co',a);h.update(a.tobytes())
    return h.hexdigest()
before=shape_hash()
cloth=bpy.data.materials['Black woven nylon | 0.6mm basket weave']
p=cloth.node_tree.nodes.get('Principled BSDF')
p.inputs['Sheen Weight'].default_value=.015
p.inputs['Specular IOR Level'].default_value=.20
N=2048;y,x=np.mgrid[0:N,0:N].astype(np.float32)/N
u=x*48;v=y*48;fu=u%1;fv=v%1
check=(np.floor(u)+np.floor(v))%2
warp=np.maximum(0,np.sin(math.pi*fu))**.65
weft=np.maximum(0,np.sin(math.pi*fv))**.65
height=np.where(check<.5,warp*(.64+.36*weft),weft*(.64+.36*warp))*.00010
height+=.000008*np.where(check<.5,np.sin(u*math.pi*14),np.sin(v*math.pi*14))
gy,gx=np.gradient(height,.04/N,.04/N)
norm=np.stack([-gx,-gy,np.ones_like(gx)],axis=-1)
norm/=np.linalg.norm(norm,axis=-1)[:,:,None]
rng=np.random.default_rng(142)
base=np.clip(.011*(.32+1.28*height/.00010)+rng.normal(0,.00065,(N,N)),.002,.025)
maps={'BlackNylon_BaseColor.png':np.stack([base*.92,base*.98,base],axis=-1),
      'BlackNylon_Normal.png':norm*.5+.5,
      'BlackNylon_Roughness.png':np.clip(.92-height*650,.82,.96)}
for im in bpy.data.images:
    filename=Path(im.filepath).name
    if filename not in maps:continue
    rgb=maps[filename];rgba=np.ones((N,N,4),np.float32)
    rgba[:,:,:3]=rgb if rgb.ndim==3 else rgb[:,:,None]
    im.pixels.foreach_set(rgba.ravel());im.update()
    im.filepath_raw=str(OUT/filename);im.file_format='PNG';im.save();im.pack()
for n in cloth.node_tree.nodes:
    if n.type=='NORMAL_MAP':n.inputs['Strength'].default_value=1.15
assert shape_hash()==before,'Material edit unexpectedly changed mesh geometry'
s=bpy.context.scene;s.render.film_transparent=True;s.use_nodes=False
prefs=bpy.context.preferences.addons['cycles'].preferences
try:
    prefs.compute_device_type='OPTIX';prefs.get_devices()
    for d in prefs.devices:d.use=d.type!='CPU'
    s.cycles.device='GPU'
except Exception:s.cycles.device='CPU'
s.cycles.samples=128;s.render.filepath=str(OUT/'Black_Bungee_Sling_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=bpy.data.filepath)
bpy.ops.render.render(write_still=True)
cam=s.camera;old=cam.matrix_world.copy();oldscale=cam.data.ortho_scale
from mathutils import Vector
theta=math.radians(35);depth=Vector((0,-math.cos(theta),math.sin(theta)))
web=bpy.data.objects['01 | upright rolled central webbing']
target=web.matrix_world@web.data.vertices[325*42+10].co
cam.location=target+depth*.5-Vector((.035,0,0))
cam.rotation_euler=(target-cam.location).to_track_quat('-Z','Y').to_euler()
cam.data.ortho_scale=.070;s.render.resolution_x=1200;s.render.resolution_y=1200
s.render.filepath=str(OUT/'Black_Nylon_Weave_Closeup.png');bpy.ops.render.render(write_still=True)
report=json.loads((OUT/'asset-report.json').read_text())
report.update({'material_revision':'Coarser matte woven nylon, stronger yarn relief',
 'approved_shape_preserved':True,'mesh_sha256':before})
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
print('MATERIAL_ONLY_REVISION_COMPLETE',before,flush=True)
