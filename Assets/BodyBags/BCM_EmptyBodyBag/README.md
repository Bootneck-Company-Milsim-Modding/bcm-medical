# Empty Body Bag Inventory Visual

`BCM_EmptyBodyBag.fbx` is generated from the filled-bag source by `Tools/GenerateEmptyBodyBagVisual.py`. The export contains only the `bodybag` render mesh; `packed_corpse` and `packed_corpse2` are deliberately excluded.

In Workbench, reimport `BCM_EmptyBodyBag.xob` as a model after changes to the source FBX. Its `bodybag` material slot is assigned to the existing BCM body-bag material. This resource is used only by the empty inventory item and does not change the full-size filled body bag.
