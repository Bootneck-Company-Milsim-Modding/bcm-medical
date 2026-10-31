import bpy
from pathlib import Path
OUT=Path(__file__).resolve().parent
for m in bpy.data.materials:
    if not m.use_nodes:continue
    p=m.node_tree.nodes.get('Principled BSDF')
    if not p:continue
    if m.name.startswith('Black woven'):
        p.inputs['Sheen Weight'].default_value=.025;p.inputs['Specular IOR Level'].default_value=.12
    if 'adjusters' in m.name:p.inputs['Specular IOR Level'].default_value=.12;p.inputs['Roughness'].default_value=.65
for d in bpy.data.lights:d.energy*=.28
scene=bpy.context.scene;scene.render.resolution_x=1100;scene.render.resolution_y=1100;scene.cycles.samples=48
scene.render.filepath=str(OUT/'Black_Material_Check.png')
bpy.ops.render.render(write_still=True)
print('MATERIAL_CHECK_COMPLETE',flush=True)
