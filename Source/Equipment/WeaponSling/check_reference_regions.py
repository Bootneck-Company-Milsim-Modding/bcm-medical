import bpy,numpy as np,json
from pathlib import Path
OUT=Path(__file__).resolve().parent
ref=bpy.data.images.load(r'C:\Users\ruanb\Downloads\Wilde_Custom_Gear_Single_Point_Sling_QD_Multicam_AR_15_Sling.webp')
im=bpy.data.images.load(str(OUT/'Sling_ReferenceMatched_Preview.png'))
W,H=ref.size
a=np.array(ref.pixels[:],np.float32).reshape(H,W,4)
b=np.array(im.pixels[:],np.float32).reshape(H,W,4)
r=a[:,:,:3].max(axis=2)<.94;m=b[:,:,3]>.5
regions={'pad':[90,155,490,285],'left sleeve':[80,350,365,563],'right sleeve':[345,325,500,563],
         'left adjustment':[25,245,140,385],'right adjustment':[450,245,540,354],
         'buckle':[365,540,435,649],'tail':[370,640,435,777],'QD':[365,755,435,841]}
stats={}
for label,(x0,y0,x1,y1) in regions.items():
    aa=r[H-y1:H-y0,x0:x1];bb=m[H-y1:H-y0,x0:x1]
    stats[label]={'IoU':float((aa&bb).sum()/max(1,(aa|bb).sum())),
                  'missing':int((aa&~bb).sum()),'extra':int((bb&~aa).sum())}
out=a.copy();out[:,:,:3]*=.4
out[r&~m,:3]=[1,.03,.03];out[m&~r,:3]=[.02,.5,1];out[r&m,:3]=[.2,.8,.2]
out[:,:,3]=1
image=bpy.data.images.new('Reference errors',width=W,height=H,alpha=True)
image.pixels.foreach_set(out.ravel());image.filepath_raw=str(OUT/'Sling_Silhouette_Difference.png');image.file_format='PNG';image.save()
(OUT/'reference-regions.json').write_text(json.dumps(stats,indent=2));print(json.dumps(stats),flush=True)
