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

# --==[PHYSICS]==--
deps += cgame_physics_dep

# --==[ASSETS]==--
deps += cgame_assets_dep

# --==[GRAPHICS]==--
# Also brings in glm and fastgltf, which are cgame-graphics' own dependencies.
deps += cgame_graphics_dep

# --==[FILES]==--
files = [
    'source/client.cpp',
]

# --==[BUILD!]==--
executable(
    'cgame-client',
    files,
    dependencies: deps,
    override_options: [
        'cpp_std=c++20',
    ],
)
