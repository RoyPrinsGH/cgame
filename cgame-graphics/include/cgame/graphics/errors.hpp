#pragma once

#include <stdexcept>
#include <string>

namespace cgame::graphics
{
    // Thrown on handle misuse: id 0, out of range, or a stale generation
    // (resource unloaded, slot reused). These are programming errors at the
    // render_backend boundary, not bad data — a stale handle is representable
    // by design (see the generation counters), and misuse must fail loudly
    // instead of aliasing whatever now owns the slot.
    class bad_handle_error : public std::logic_error
    {
      public:
        using std::logic_error::logic_error;
    };

    // Base for failures caused by bad or unsupported data (shader sources,
    // uploadable images, glb contents) as opposed to API misuse. Recoverable
    // by the caller: e.g. a controller can fall back to the default shader.
    class graphics_error : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    // Shader sources failed to compile, or the program is missing uniforms
    // required by the shader contract. GLSL diagnostics are printed to stderr
    // by rlgl; the exception carries a generic message.
    class shader_compile_error : public graphics_error
    {
      public:
        using graphics_error::graphics_error;
    };

    // An image could not be uploaded to the GPU (rejected by the driver, too
    // large, out of memory).
    class texture_upload_error : public graphics_error
    {
      public:
        using graphics_error::graphics_error;
    };

    // Asset bytes could not be decoded: unreadable or unparsable glb, mesh
    // missing required attributes, undecodable image.
    class asset_error : public graphics_error
    {
      public:
        using graphics_error::graphics_error;
    };
}
