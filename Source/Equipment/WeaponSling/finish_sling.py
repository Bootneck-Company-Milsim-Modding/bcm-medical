"""Run on the saved sling blend after the product render finishes."""
import bpy, json, os
from pathlib import Path
from mathutils import Vector
OUT=Path(__file__).resolve().parent
scene=bpy.context.scene
collection=bpy.data.collections['INVENTORY LOD | mesh-only export candidate']
collection.hide_viewport=False
source=bpy.data.objects['BCM_Sling_Inventory_LOD']
low=source.copy();low.data=source.data.copy();collection.objects.link(low)
low.name='BCM_Sling_Inventory_GameMesh'
bpy.ops.object.select_all(action='DESELECT');low.select_set(True);bpy.context.view_layer.objects.active=low
ratio=min(1,6500/max(1,len(low.data.polygons)))
dec=low.modifiers.new('Small inventory mesh','DECIMATE');dec.ratio=ratio
bpy.ops.object.modifier_apply(modifier=dec.name)
tri=low.modifiers.new('Explicit export triangles','TRIANGULATE');bpy.ops.object.modifier_apply(modifier=tri.name)
# Ensure unique UV layer exists on the joined mesh; cloth retains its woven UVs.
if not low.data.uv_layers: low.data.uv_layers.new(name='UVMap')
bpy.ops.export_scene.fbx(filepath=str(OUT/'BCM_Sling_Inventory.fbx'),use_selection=True,
    object_types={'MESH'},add_leaf_bones=False,bake_anim=False,apply_unit_scale=True,
    use_mesh_modifiers=True,path_mode='RELATIVE',mesh_smooth_type='FACE')
report=json.loads((OUT/'asset-report.json').read_text())
report['reduced_inventory_triangles']=len(low.data.polygons)
report['exported_fbx']='BCM_Sling_Inventory.fbx'
report['blend_actions']=len(bpy.data.actions)
report['armatures']=len(bpy.data.armatures)
(OUT/'asset-report.json').write_text(json.dumps(report,indent=2))
collection.hide_viewport=True;collection.hide_render=True
# Save with the main product presentation still configured.
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_SinglePoint_WeaponSling.blend'))
# Additional transparent icon render does not alter the saved default studio view.
if bpy.data.objects.get('Studio ground'):bpy.data.objects['Studio ground'].hide_render=True
scene.camera.location=(0,-.12,1.5)
scene.camera.rotation_euler=(Vector((0,-.10,.02))-scene.camera.location).to_track_quat('-Z','Y').to_euler()
scene.camera.data.ortho_scale=.78
scene.render.film_transparent=True
scene.render.image_settings.color_mode='RGBA'
scene.render.resolution_x=768;scene.render.resolution_y=768;scene.cycles.samples=24
scene.render.filepath=str(OUT/'Sling_Inventory_Icon.png')
if not os.environ.get('BCM_SLING_BUILD_ONLY'):bpy.ops.render.render(write_still=True)
print('BCM_SLING_EXPORT_COMPLETE',json.dumps(report),flush=True)
