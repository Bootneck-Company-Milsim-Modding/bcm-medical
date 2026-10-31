import bpy
from pathlib import Path
out=Path(bpy.data.filepath).parent
s=bpy.context.scene
s.view_settings.exposure=1.1
s.cycles.samples=64
s.render.film_transparent=True
s.use_nodes=False
s.render.filepath=str(out/'Black_Bungee_Sling_Preview.png')
bpy.ops.wm.save_as_mainfile(filepath=bpy.data.filepath)
bpy.ops.render.render(write_still=True)
