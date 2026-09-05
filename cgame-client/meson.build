# --==[SETUP!]==--
deps = []
cmake = import('cmake')

# --==[PLATFORM]==--
# Also brings in GLFW and the boost projects configured by cgame-platform.
deps += cgame_platform_dep

# --==[BOOST]==--
deps += boost_project.dependency('boost_circular_buffer')
deps += boost_project.dependency('boost_dll_std_fs')
deps += boost_project.dependency('boost_iterator')
deps += boost_project.dependency('boost_range')

# --==[ENET]==--
enet_project = cmake.subproject('enet')
message('CMake targets:\n - ' + '\n - '.join(enet_project.target_list()))

deps += enet_project.dependency('enet_static')

# --==[GLM]==--
glm_project = cmake.subproject('glm')
message('CMake targets:\n - ' + '\n - '.join(glm_project.target_list()))

deps += glm_project.dependency('glm')

# --==[FASTGLTF]==--
fastgltf_project = cmake.subproject('fastgltf')
message('CMake targets:\n - ' + '\n - '.join(fastgltf_project.target_list()))

deps += fastgltf_project.dependency('fastgltf')

# --==[PHYSICS]==--
deps += cgame_physics_dep

# --==[ASSETS]==--
deps += cgame_assets_dep

# --==[GRAPHICS]==--
deps += cgame_graphics_dep

# --==[FILES]==--
files = [
    'source/client.cpp',
    'source/renderer.cpp',
]

# --==[BUILD!]==--
executable(
    'cgame-client',
    files,
    dependencies: deps,
    # raylib is header-only to us: rlgl.h (built by rlgl_impl.cpp) and raymath.h.
    include_directories: [
        '../subprojects/raylib/src',
        '../subprojects/raygui/src',
        '../subprojects/stb',
    ],
    override_options: [
        'cpp_std=c++20',
    ],
)
