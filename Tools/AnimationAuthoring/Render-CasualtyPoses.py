# Original BCM pose inspection only; never reads an ACE file or saves the blend.
import bpy
from pathlib import Path
from mathutils import Vector
out=Path(__file__).resolve().parents[2]/'.checks/casualty-pose-preview'
out.mkdir(parents=True,exist_ok=True)
scene=bpy.context.scene
for c in bpy.data.collections: c.hide_render=False; c.hide_viewport=False
meshes=[]
for o in bpy.data.objects:
    o.hide_render=not(o.type=='MESH' and o.name.endswith('_LOD0'))
    o.hide_set(False)
    if not o.hide_render: meshes.append(o)
scene.render.engine='BLENDER_WORKBENCH'
scene.display.shading.light='STUDIO'
scene.display.shading.color_type='SINGLE'
scene.display.shading.single_color=(0.45,0.5,0.55)
scene.display.shading.show_shadows=True
scene.display.shading.show_cavity=True
scene.render.resolution_x=1000;scene.render.resolution_y=800;scene.render.resolution_percentage=100
scene.render.image_settings.file_format='PNG'
camera=bpy.data.objects.get('Camera'); scene.camera=camera
camera.data.type='ORTHO'
rig=bpy.data.objects['rig']
for mode in ('Carried','Dragged'):
    visible=[]
    for o in meshes:
        o.hide_render=o.name.startswith('CarrierReference_') and mode!='Carried'
        if not o.hide_render:visible.append(o)
    rig.animation_data.action=bpy.data.actions['BCMM_'+mode+'_loop']
    scene.frame_set(0);bpy.context.view_layer.update()
    dg=bpy.context.evaluated_depsgraph_get()
    points=[o.evaluated_get(dg).matrix_world@Vector(p) for o in visible for p in o.evaluated_get(dg).bound_box]
    lo=Vector(tuple(min(p[i] for p in points) for i in range(3)))
    hi=Vector(tuple(max(p[i] for p in points) for i in range(3)))
    center=(lo+hi)/2
    camera.location=center+Vector((3,-5,2.6))
    camera.rotation_euler=(center-camera.location).to_track_quat('-Z','Y').to_euler()
    camera.data.ortho_scale=max(hi-lo)*1.6
    scene.render.filepath=str(out/('BCMM_ORIGINAL_'+mode+'.png'))
    bpy.ops.render.render(write_still=True)
    if mode=='Carried':
        for name,offset in [('front',(0,-5,1)),('back',(0,5,1)),('left',(5,0,1)),('right',(-5,0,1))]:
            camera.location=center+Vector(offset)
            camera.rotation_euler=(center-camera.location).to_track_quat('-Z','Y').to_euler()
            scene.render.filepath=str(out/('BCMM_Carried_'+name+'.png'))
            bpy.ops.render.render(write_still=True)

