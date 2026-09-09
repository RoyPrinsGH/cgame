# AGENTS.md

Cgame is a WIP C++20 game engine (meson, GLFW/OpenGL via raylib's rlgl, bullet
physics, fastgltf assets) for simple Luau-driven multiplayer games.

## Build & run

Linux is the primary target. Windows is cross-compiled only — wine is not
available, so Windows builds cannot be executed here.

```bash
scripts/setup.linux.sh    # meson subprojects download + setup build-linux (once)
scripts/build.linux.sh    # meson compile -C build-linux
scripts/run.client.sh     # run the game client (needs X11 + GL)
scripts/run.cli.sh        # run cgame-cli
```

- Meson >= 1.12 required; distro meson may be older (the sandbox installs it via pip).
- `build-linux/` is gitignored. A build dir configured on another machine (paths
  pointing at `/home/...`) must be deleted and re-setup.

## Structure

Root `meson.build` subdirs each config one module; static libs link into the executables:

- `cgame-assets` — pak archives, virtual asset paths
- `cgame-platform` — window (GLFW) + raw input
- `cgame-physics` — rigid bodies (bullet)
- `cgame-graphics` — rendering: `render_backend` interface, GL 3.3 backend,
  model/shader controllers, embedded default shader sources, owns glm/fastgltf
- `cgame-project` — reads `project.yaml`, collects packable assets
- `cgame-cli` — CLI (`cgame-cli pack client|server`)
- `cgame-client` — the game client executable
- `demo-thehighseas` — sample project with packed `.cgpak` files
- `kanban` — tickets: `todo/`, `backlog/`, `done/`
- `subprojects` — wrap-fetched deps (raylib, glm, fastgltf, bullet, ...)

Rendering architecture: backends implement `cgame::graphics::render_backend`;
the client builds a `render_snapshot` per frame and calls
`cgame::graphics::render(backend, snapshot, fbWidth, fbHeight)`. The shader
contract (uniforms/attribute locations) is documented in
`cgame-graphics/include/cgame/graphics/default_shaders.hpp`.

## Visual verification

Running the client needs X11 (`DISPLAY`) and a GL context — both are forwarded
in the sandbox (`pi-sandbox/sandbox.sh`). You cannot watch the window yourself;
to verify a render, grab a frame:

```bash
pi-sandbox/screenshot.sh [delay] [out.png]   # runs the client, grabs its window as PNG
```

Then view the PNG. Client stderr (RLGL logs) shows shader/texture/model load
successes and failures.

## Working on this repo

- Pick up tickets from `kanban/todo/` (lowest number first); move to
  `kanban/done/` when finished. `kanban/` is gitignored — stage with `git add -f`.
- Commit in logical steps with prefixes `feat:`, `fix:`, `refactor:`, `docs:`,
  `other:`; agent-authored commits carry `Co-Authored-By:`/`Generated-By:`
  trailers (see `git log`).
- Client-side code is glue only (input, tick loop, snapshots); rendering
  behaviour belongs in `cgame-graphics`.
