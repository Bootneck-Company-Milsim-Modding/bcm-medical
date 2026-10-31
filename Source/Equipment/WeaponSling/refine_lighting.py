import bpy,json
from pathlib import Path
OUT=Path(__file__).resolve().parent
for material in bpy.data.materials:
    if 'woodland nylon' in material.name:
        p=material.node_tree.nodes.get('Principled BSDF')
        p.inputs['Sheen Weight'].default_value=.035
        p.inputs['Specular IOR Level'].default_value=.12
        print('CLOTH_TEXTURES',[(n.image.name,n.image.colorspace_settings.name,list(n.image.pixels[:4])) for n in material.node_tree.nodes if n.type=='TEX_IMAGE'],flush=True)
for light in bpy.data.lights:light.energy*=.16
scene=bpy.context.scene
scene.render.resolution_x=550;scene.render.resolution_y=700;scene.cycles.samples=12
scene.render.filepath=str(OUT/'Sling_Lighting_Check.png')
bpy.ops.render.render(write_still=True)
print('LIGHTING_CHECK_DONE',flush=True)
