import bpy,json,math,numpy as np
from pathlib import Path
from mathutils import Vector
OUT=Path(__file__).resolve().parent
scene=bpy.context.scene;cam=scene.camera
prefs=bpy.context.preferences.addons['cycles'].preferences
try:
    prefs.compute_device_type='OPTIX';prefs.get_devices()
    for device in prefs.devices:device.use=device.type!='CPU'
    scene.cycles.device='GPU'
except Exception:scene.cycles.device='CPU'
original_matrix=cam.matrix_world.copy();original_scale=cam.data.ortho_scale
def aim(o,p):o.rotation_euler=(Vector(p)-o.location).to_track_quat('-Z','Y').to_euler()
theta=math.radians(35);RIGHT=Vector((1,0,0));UP=Vector((0,math.sin(theta),math.cos(theta)))
DEPTH=Vector((0,-math.cos(theta),math.sin(theta)))
def pp(x,y,depth=0):return RIGHT*((x-600)*.00030)+UP*((600-y)*.00030)+DEPTH*depth
asset=bpy.data.collections['SLING | original static black nylon']
export=bpy.data.collections.new('EXPORT | reduced inventory candidate');scene.collection.children.link(export)
deps=bpy.context.evaluated_depsgraph_get();copies=[]
for o in list(asset.objects):
    if o.type not in {'MESH','CURVE'}:continue
    me=bpy.data.meshes.new_from_object(o.evaluated_get(deps),depsgraph=deps)
    ob=bpy.data.objects.new(o.name+' | export',me);export.objects.link(ob);ob.matrix_world=o.matrix_world;copies.append(ob)
bpy.ops.object.select_all(action='DESELECT')
for o in copies:o.select_set(True)
bpy.context.view_layer.objects.active=copies[0];bpy.ops.object.join()
low=bpy.context.object;low.name='BCM_Black_TwoHook_BungeeSling_InventoryMesh'
tri=low.modifiers.new('Triangulate export','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
high_triangles=len(low.data.polygons)
dec=low.modifiers.new('Inventory mesh reduction','DECIMATE');dec.ratio=min(1,18000/max(1,high_triangles))
bpy.ops.object.modifier_apply(modifier=dec.name)
tri=low.modifiers.new('Final export triangles','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
bpy.ops.export_scene.fbx(filepath=str(OUT/'BCM_Black_TwoHook_BungeeSling_Inventory.fbx'),use_selection=True,
    object_types={'MESH'},add_leaf_bones=False,bake_anim=False,path_mode='RELATIVE',mesh_smooth_type='FACE')
export.hide_render=True;export.hide_viewport=True
report=json.loads((OUT/'asset-report.json').read_text())
report.update({'detailed_triangles':high_triangles,'reduced_export_triangles':len(low.data.polygons),
               'packed_fabric_images':all(im.packed_file is not None for im in bpy.data.images if im.name.startswith('Black nylon |')),
               'native_game_validation':False,'matching_status':'Artist review; no perfect/exact-match claim'})
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
# Preserve the transparent main product view as the .blend default.
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_Black_TwoHook_BungeeSling.blend'))
# Render actual geometry closeups with no source-photo projection.
target=pp(940,653,.026)
cam.location=target+DEPTH*.55+RIGHT*.05+UP*.025;aim(cam,target);cam.data.ortho_scale=.115
scene.render.resolution_x=1200;scene.render.resolution_y=1200;scene.cycles.samples=96
scene.render.filepath=str(OUT/'Black_Sling_Hook_and_Stitching_Closeup.png')
bpy.ops.render.render(write_still=True)
target=pp(850,825,.031)
cam.location=target+DEPTH*.40+RIGHT*.035;aim(cam,target);cam.data.ortho_scale=.100
scene.render.filepath=str(OUT/'Black_Sling_Fabric_Junction_Closeup.png')
bpy.ops.render.render(write_still=True)
target=pp(950,653,.027)
cam.location=target+DEPTH*.27+RIGHT*.19+UP*.06;aim(cam,target);cam.data.ortho_scale=.095
scene.render.filepath=str(OUT/'Black_Sling_Threading_Oblique.png')
bpy.ops.render.render(write_still=True)
cam.location=target-DEPTH*.28+RIGHT*.12;aim(cam,target);cam.data.ortho_scale=.095
scene.render.filepath=str(OUT/'Black_Sling_Threading_Back.png')
bpy.ops.render.render(write_still=True)
target=pp(283,352,.061)
cam.location=target+DEPTH*.35+RIGHT*.11;aim(cam,target);cam.data.ortho_scale=.075
scene.render.filepath=str(OUT/'Black_Sling_Adjuster_Threading.png')
bpy.ops.render.render(write_still=True)
web=bpy.data.objects['01 | upright rolled central webbing']
target=web.matrix_world@web.data.vertices[325*42+10].co
cam.location=target+DEPTH*.5-RIGHT*.035;aim(cam,target);cam.data.ortho_scale=.070
scene.render.filepath=str(OUT/'Black_Nylon_Weave_Closeup.png')
bpy.ops.render.render(write_still=True)
# Checkerboard is a review compositor only, never a backdrop mesh or asset texture.
cam.matrix_world=original_matrix;cam.data.ortho_scale=original_scale
scene.render.resolution_x=1800;scene.render.resolution_y=1800;scene.cycles.samples=64
n=1800;y,x=np.indices((n,n));check=(x//40+y//40)%2
rgba=np.ones((n,n,4),np.float32);rgba[:,:,:3]=(.10+check*.035)[:,:,None]
bg=bpy.data.images.new('REVIEW ONLY | transparency checker',width=n,height=n,alpha=True)
bg.pixels.foreach_set(rgba.ravel())
scene.use_nodes=True;nodes=scene.node_tree.nodes;nodes.clear();links=scene.node_tree.links
layer=nodes.new('CompositorNodeRLayers');image=nodes.new('CompositorNodeImage');image.image=bg
over=nodes.new('CompositorNodeAlphaOver');links.new(image.outputs['Image'],over.inputs[1]);links.new(layer.outputs['Image'],over.inputs[2])
out=nodes.new('CompositorNodeComposite');links.new(over.outputs['Image'],out.inputs['Image'])
scene.render.filepath=str(OUT/'Black_Sling_Transparency_Review.png')
bpy.ops.render.render(write_still=True)
print('BLACK_SLING_FINISH_COMPLETE',json.dumps(report),flush=True)
