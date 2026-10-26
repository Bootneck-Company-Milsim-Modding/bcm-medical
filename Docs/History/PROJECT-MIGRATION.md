# Project identity migration

The repository originally preserved an empty 19 October scaffold under Addon/addon.gproj, GUID 6AA72CA6DB5B3AA2, with an additional dependency. Its original text is retained in 20261019-original-scaffold.gproj.txt and the historical baseline manifest remains unchanged.

On 26 October the user created a new Workbench project in MCB Medical and instructed all medical work to move there. The canonical root addon.gproj retains that new project's ID MCBMedical and GUID 6AB4B3F3FD215919. Its display title is BCM Medical and its only runtime dependency is the base game. The diagnostic addon now depends on that actual new GUID.

The obsolete Addon/addon.gproj is removed from the active tree to avoid distributing two different medical projects. This does not delete or rewrite its historical Git commits or modify any older working project outside this checkout.

The earlier BCM NID AceCompat/Standalone/BCM-Medical workspace was a temporary development location. New work belongs here. Legacy BCM gameplay mods remain separate and unchanged while their functionality is reimplemented.