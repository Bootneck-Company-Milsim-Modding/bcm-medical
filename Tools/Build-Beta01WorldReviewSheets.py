"""Label and tile crops of native captures for review; originals remain untouched."""
from pathlib import Path
import json
from PIL import Image, ImageDraw

root = Path(__file__).resolve().parents[1]
pack = json.loads((root / '.checks/beta01-pack-process.json').read_text(encoding='utf-8-sig'))
run = sorted(Path(pack['run']).glob('acceptance-WorldReview-*'))[-1]
result = json.loads((run / 'result.json').read_text(encoding='utf-8-sig'))
if not result['passed']:
    raise SystemExit('World capture fixture did not pass')
items = json.loads((root / 'Tools/ArsenalItems.json').read_text(encoding='utf-8-sig'))
out = run / 'review'
out.mkdir(exist_ok=True)
for start in range(0, len(items), 6):
    end = min(start + 6, len(items))
    sheet = Image.new('RGB', (960, (end-start)*235), '#20252a')
    draw = ImageDraw.Draw(sheet)
    for index in range(start, end):
        for view, distance in enumerate(('0.75 m', '3 m', '12 m')):
            source = next(run.rglob(f'world-{index}-distance-{view}.bmp'))
            frame = Image.open(source)
            # Same fixed crop and scale for all views; this is not a zoom-normalized comparison.
            crop = frame.crop((320, 160, 960, 560)).resize((320, 200))
            x, y = view*320, (index-start)*235
            draw.text((x+5, y+3), f'{index}: {items[index]["label"]} | {distance}', fill='white')
            sheet.paste(crop, (x, y+27))
    path = out / f'world-distance-{start:02d}-{end-1:02d}.png'
    sheet.save(path)
    print(path)
