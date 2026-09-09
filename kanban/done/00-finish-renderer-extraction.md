# Finish moving the renderer out of cgame-client

The extraction is half-done. `cgame-graphics` exists and the client links
`cgame_graphics_dep`, but the client still renders through the old in-client
renderer: `client.cpp` constructs `engine::renderer` and calls
`renderer.draw(window, gameState, camera)`, and nothing calls
`cgame::graphics::render`.

What remains:

  - Build a `render_snapshot` from the game state and camera each frame and
    call `cgame::graphics::render(backend, snapshot, ...)` instead.
  - Port the behaviour that lives only in the old renderer: default shader
    setup, ship model loading, the debug mode toggle, and the
    gamestate/camera to snapshot translation.
  - Delete `cgame-client/source/renderer.cpp`, `renderer.hpp`, and the
    client's `shaders/` once the new path is live. Decide whether the
    client's `camera.hpp` stays or the `cgame::graphics` camera replaces it.
  - Trim the client's `meson.build`: fastgltf and the direct raylib/stb
    include dirs are only needed by the old renderer, and the glm/fastgltf
    cmake subproject setup is currently duplicated between the two
    `meson.build` files.

The todo tickets 01-04 and 09 are quality issues in the new module that are
easier to settle before more call sites depend on the current behaviour, but
none of them strictly block this.

Touches: client.cpp, renderer.{hpp,cpp}, meson.build (client and graphics).
