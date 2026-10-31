"""Final presentation correction on the completed static asset; CPU-only."""
import bpy,json
from pathlib import Path
from mathutils import Vector
OUT=Path(__file__).resolve().parent
scene=bpy.context.scene
cloth=bpy.data.materials['Woven woodland nylon | packed 2K PBR']
p=cloth.node_tree.nodes.get('Principled BSDF')
p.inputs['Sheen Weight'].default_value=.035;p.inputs['Specular IOR Level'].default_value=.12
for light,energy in [('Large softbox key',10),('Gentle fabric fill',5.5),('Top rim',4.5)]:
    bpy.data.lights[light].energy=energy
floor=bpy.data.objects.get('Studio ground')
if floor:bpy.data.objects.remove(floor,do_unlink=True)
scene.render.film_transparent=True
scene.use_nodes=False
scene.render.threads_mode='FIXED';scene.render.threads=4
scene.render.resolution_x=1100;scene.render.resolution_y=1400;scene.cycles.samples=32
scene.render.image_settings.color_mode='RGBA';scene.render.filepath=str(OUT/'Sling_Product_Preview.png')
# Update static mesh export to retain the corrected Blender material values.
c=bpy.data.collections['INVENTORY LOD | mesh-only export candidate'];c.hide_viewport=False
low=bpy.data.objects['BCM_Sling_Inventory_GameMesh']
bpy.ops.object.select_all(action='DESELECT');low.select_set(True);bpy.context.view_layer.objects.active=low
bpy.ops.export_scene.fbx(filepath=str(OUT/'BCM_Sling_Inventory.fbx'),use_selection=True,
    object_types={'MESH'},add_leaf_bones=False,bake_anim=False,path_mode='RELATIVE',mesh_smooth_type='FACE')
c.hide_viewport=True
report=json.loads((OUT/'asset-report.json').read_text())
assert len(bpy.data.actions)==0 and len(bpy.data.armatures)==0
assert len(low.data.polygons)==6500
report['packed_images']=all(im.packed_file is not None for im in bpy.data.images if im.name.startswith(('BCM original','Webbing roughness','Woven nylon')))
report['review']='Static geometry and initial renders inspected; corrected cloth reflectance and final studio presentation'
report['transparent_background']=True
report['background_geometry']=False
report['shape_revision']='Asymmetric pointed loop, longer left sleeve, shorter right sleeve, offset compact connector tail'
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_SinglePoint_WeaponSling.blend'))
bpy.ops.render.render(write_still=True)
# Native alpha output for inventory use, without the white product compositor.
scene.use_nodes=False
scene.camera.location=(0,-.12,1.5)
scene.camera.rotation_euler=(Vector((0,-.10,.02))-scene.camera.location).to_track_quat('-Z','Y').to_euler()
scene.camera.data.ortho_scale=.78
scene.render.resolution_x=768;scene.render.resolution_y=768;scene.cycles.samples=24
scene.render.filepath=str(OUT/'Sling_Inventory_Icon.png')
bpy.ops.render.render(write_still=True)
print('BCM_SLING_FINAL_COMPLETE',json.dumps(report),flush=True)
