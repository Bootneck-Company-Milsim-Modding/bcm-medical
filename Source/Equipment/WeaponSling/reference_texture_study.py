"""Explicit, separately saved photo-projection study, not original PBR art.
The source photograph is used for cloth colour only; geometry alpha is unchanged.
"""
import bpy,numpy as np,json
from pathlib import Path
OUT=Path(__file__).resolve().parent
scene=bpy.context.scene
ref=bpy.data.images.load(r'C:\Users\ruanb\Downloads\Wilde_Custom_Gear_Single_Point_Sling_QD_Multicam_AR_15_Sling.webp')
W,H=ref.size;pixels=np.array(ref.pixels[:],np.float32).reshape(H,W,4)
known=pixels[:,:,:3].max(axis=2)<.94
rgb=pixels[:,:,:3].copy()
# Extend cloth colours a few pixels outside the photographed silhouette, so
# small mesh overhangs cannot sample the white photographic background.
for _ in range(16):
    colours=np.zeros_like(rgb);weights=np.zeros((H,W),np.float32)
    for dy,dx in [(-1,0),(1,0),(0,-1),(0,1),(-1,-1),(-1,1),(1,-1),(1,1)]:
        shifted=np.roll(known,(dy,dx),(0,1));values=np.roll(rgb,(dy,dx),(0,1))
        colours+=values*shifted[:,:,None];weights+=shifted
    take=(~known)&(weights>0)
    rgb[take]=colours[take]/weights[take,None];known[take]=True
rgb[~known]=(.35,.37,.25)
pixels[:,:,:3]=rgb;pixels[:,:,3]=1
im=ref.copy();im.name='SUPPLIED PHOTO | padded fabric colour study'
im.pixels.foreach_set(pixels.ravel());im.filepath_raw=str(OUT/'Sling_ReferenceFabric_Study.png')
im.file_format='PNG';im.save();im.pack()
original=next(m for m in bpy.data.materials if m.name.startswith('Woven woodland nylon'))
study=original.copy();study.name='STUDY | reference-projected cloth front, original underside'
nodes=study.node_tree.nodes;links=study.node_tree.links
p=nodes.get('Principled BSDF');old=next(link.from_socket for link in links if link.to_socket==p.inputs['Base Color'])
uv=nodes.new('ShaderNodeUVMap');uv.uv_map='PhotoReferenceUV'
tex=nodes.new('ShaderNodeTexImage');tex.image=im;tex.extension='EXTEND';links.new(uv.outputs['UV'],tex.inputs['Vector'])
geo=nodes.new('ShaderNodeNewGeometry');dot=nodes.new('ShaderNodeVectorMath');dot.operation='DOT_PRODUCT'
dot.inputs[1].default_value=(0,0,1);links.new(geo.outputs['True Normal'],dot.inputs[0])
front=nodes.new('ShaderNodeMapRange');front.inputs['From Min'].default_value=.02;front.inputs['From Max'].default_value=.28
front.clamp=True;links.new(dot.outputs['Value'],front.inputs['Value'])
mix=nodes.new('ShaderNodeMixRGB');links.new(front.outputs['Result'],mix.inputs[0]);links.new(old,mix.inputs[1]);links.new(tex.outputs['Color'],mix.inputs[2])
links.new(mix.outputs[0],p.inputs['Base Color'])
# The neutral projection preview deliberately retains the photograph's baked
# illumination. It is not a newly measured physically calibrated albedo map.
emission=nodes.new('ShaderNodeEmission');links.new(tex.outputs['Color'],emission.inputs['Color'])
weight=nodes.new('ShaderNodeMath');weight.operation='MULTIPLY';weight.inputs[1].default_value=.95
links.new(front.outputs['Result'],weight.inputs[0])
shader=nodes.new('ShaderNodeMixShader');links.new(weight.outputs[0],shader.inputs[0])
links.new(p.outputs['BSDF'],shader.inputs[1]);links.new(emission.outputs[0],shader.inputs[2])
output=nodes.get('Material Output');links.new(shader.outputs[0],output.inputs['Surface'])
hero=bpy.data.collections['SLING | detailed static asset']
for obj in hero.objects:
    if obj.type!='MESH' or not any(m==original for m in obj.data.materials):continue
    layer=obj.data.uv_layers.get('PhotoReferenceUV') or obj.data.uv_layers.new(name='PhotoReferenceUV')
    for polygon in obj.data.polygons:
        for li,vi in zip(polygon.loop_indices,polygon.vertices):
            world=obj.matrix_world@obj.data.vertices[vi].co
            layer.data[li].uv=(world.x/(.000925*W)+.5,world.y/(.000925*H)+.5)
    for i,material in enumerate(obj.data.materials):
        if material==original:obj.data.materials[i]=study
scene.view_settings.view_transform='Standard'
scene.view_settings.look='None';scene.view_settings.exposure=0;scene.view_settings.gamma=1
scene.render.resolution_x=1200;scene.render.resolution_y=2000
scene.render.film_transparent=True;scene.render.image_settings.color_mode='RGBA'
scene.render.filepath=str(OUT/'Sling_ReferenceTexture_Study.png')
scene['reference_texture_study']='Source-photo front projection with baked lighting; original PBR source is in separate ReferenceMatched blend. No alpha card or masking.'
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'BCM_Sling_ReferenceTextureStudy.blend'))
bpy.ops.render.render(write_still=True)
(OUT/'reference-texture-study.json').write_text(json.dumps({'photo_derived_fabric':True,
    'geometry_changed':False,'alpha_cutout':False,'source_photo_baked_lighting':True,
    'original_pbr_blend':'BCM_Sling_ReferenceMatched.blend','public_distribution_provenance':'not established'},indent=2))
print('REFERENCE_TEXTURE_STUDY_COMPLETE',flush=True)
