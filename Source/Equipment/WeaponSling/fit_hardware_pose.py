"""Fit rigid hardware placement/rotation/size to the visible reference, not alpha.
No part's mechanical mesh is deformed. The photo does not provide CAD dimensions.
"""
import bpy,math,json,numpy as np
from pathlib import Path
from mathutils import Matrix,Vector
OUT=Path(__file__).resolve().parent
scene=bpy.context.scene;scene.render.resolution_x=600;scene.render.resolution_y=1000
scene.cycles.samples=2;scene.cycles.use_denoising=False
scene.render.use_border=True;scene.render.use_crop_to_border=True
ref=bpy.data.images.load(r'C:\Users\ruanb\Downloads\Wilde_Custom_Gear_Single_Point_Sling_QD_Multicam_AR_15_Sling.webp')
photo=np.array(ref.pixels[:],np.float32).reshape(1000,600,4);reference=photo[:,:,:3].max(axis=2)<.94
hero=bpy.data.collections['SLING | detailed static asset']
groups={
 'buckle':{'names':('Side-release |','Male buckle |','Female buckle |'), 'pivot':(397,599),'box':(362,534,435,650)},
 'QD':{'names':('QD swivel |',),'pivot':(395,805),'box':(360,742,440,842)}}
results={}
for label,info in groups.items():
    objects=[o for o in hero.objects if o.name.startswith(info['names'])]
    originals={o:o.matrix_world.copy() for o in objects}
    x0,y0,x1,y1=info['box'];reference_roi=reference[1000-y1:1000-y0,x0:x1]
    scene.render.border_min_x=x0/600;scene.render.border_max_x=x1/600
    scene.render.border_min_y=1-y1/1000;scene.render.border_max_y=1-y0/1000
    pivot=Vector(((info['pivot'][0]-300)*.000925,(500-info['pivot'][1])*.000925,.035*.925))
    trial_path=OUT/'Hardware_Pose_Trial.png';scene.render.filepath=str(trial_path)
    def apply(params):
        dx,dy,angle,scale=params
        M=Matrix.Translation(pivot+Vector((dx*.000925,-dy*.000925,0)))@Matrix.Rotation(angle,4,'Z')@Matrix.Scale(scale,4)@Matrix.Translation(-pivot)
        for o,original in originals.items():o.matrix_world=M@original
        bpy.context.view_layer.update()
    def evaluate(params):
        apply(params);bpy.ops.render.render(write_still=True)
        im=bpy.data.images.load(str(trial_path),check_existing=False)
        w,h=im.size;mask=np.array(im.pixels[:],np.float32).reshape(h,w,4)[:,:,3]>.5
        bpy.data.images.remove(im)
        if mask.shape!=reference_roi.shape:raise RuntimeError((mask.shape,reference_roi.shape))
        return float((mask&reference_roi).sum()/max(1,(mask|reference_roi).sum()))
    best=[0.,0.,0.,1.];score=evaluate(best)
    for initial_scale in [1.1,1.2,1.3]:
        trial=[0.,0.,0.,initial_scale];value=evaluate(trial)
        if value>score:best,score=trial,value
    for level in range(3):
        steps=[3/(2**level),3/(2**level),.15/(2**level),.08/(2**level)]
        for axis,step in enumerate(steps):
            current=best.copy()
            for sign in [-1,1]:
                trial=current.copy();trial[axis]+=step*sign
                if abs(trial[0])>7 or abs(trial[1])>7 or abs(trial[2])>.5 or not .8<=trial[3]<=1.4:continue
                value=evaluate(trial)
                if value>score:best,score=trial,value
    apply(best)
    results[label]={'translation_pixels':best[:2],'rotation_z_radians':best[2],'uniform_scale':best[3],
                    'roi_silhouette_iou':score,'pivot_pixels':info['pivot']}
    print('HARDWARE_POSE_FIT',label,json.dumps(results[label]),flush=True)
(OUT/'hardware-pose-fit.json').write_text(json.dumps(results,indent=2))
print('HARDWARE_POSE_FIT_COMPLETE',flush=True)
