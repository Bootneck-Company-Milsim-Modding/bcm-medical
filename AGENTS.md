# Workshop release requirements

The user requires publishing directly from the main BCM Medical project in this folder.

- Keep QA archives, downloaded dependencies, test profiles, diagnostic projects, packaging output, screenshots, logs, caches and tool installations outside every publishable addon directory. Never recreate `.checks` inside this addon. Read `QA-ARCHIVE-LOCATION.txt` for the external archive location.
- Configure all test and packaging tools to write to an external workspace before running them. Check existing scripts for hardcoded in-addon output paths. Do not run an old runner unchanged if it creates in-addon QA files.
- Preserve the main project path and GUID. Staging copies must never replace the user's normal launcher project.
- Before declaring a build ready to publish, build from the actual main project and inspect the resulting archive contents and publisher manifest. Reject QA folders, nested addons, old PAKs, downloaded third-party packages and other development payloads.
- Check the actual upload size and compare it with the validated runtime package. Investigate unexpected growth before handing the build to the user. The clean main-project package on 3 November 2026 was approximately 86.77 MiB.
- Git ignore rules do not establish Workshop packaging exclusions. Verify the generated upload itself.
- Record the verified package size, identity and any material remaining compatibility limitations. Keep validation evidence outside the addon.
- When the user is actively publishing, avoid changing runtime files or interfering with their publishing session.
