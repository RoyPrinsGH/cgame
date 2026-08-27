# --==[SETUP!]==--
deps = []
cmake = import('cmake')
cpp = meson.get_compiler('cpp')

# --==[RAYLIB]==--
raylib_project = cmake.subproject('raylib')
message('CMake targets:\n - ' + '\n - '.join(raylib_project.target_list()))

deps += raylib_project.dependency('raylib')

if host_machine.system() == 'linux'
    deps += cpp.find_library('m')
elif host_machine.system() == 'windows'
    deps += cpp.find_library('winmm')
endif

# --==[ENET]==--
enet_project = cmake.subproject('enet')
message('CMake targets:\n - ' + '\n - '.join(enet_project.target_list()))

deps += enet_project.dependency('enet_static')

# --==[BOOST]==--
boost_options = cmake.subproject_options()

boost_options.add_cmake_defines(
    {
        'BOOST_INCLUDE_LIBRARIES': 'lockfree;circular_buffer;dll;range',
        'BUILD_TESTING': false,
    },
)

boost_project = cmake.subproject(
    'boost',
    options: boost_options,
)

message('CMake targets:\n - ' + '\n - '.join(boost_project.target_list()))

deps += boost_project.dependency('boost_lockfree')
deps += boost_project.dependency('boost_circular_buffer')
deps += boost_project.dependency('boost_dll_std_fs')
deps += boost_project.dependency('boost_iterator')
deps += boost_project.dependency('boost_range')

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

# --==[FILES]==--
files = [
    'source/rlgl_impl.cpp',
    'source/stb_image_impl.cpp',
    'source/client.cpp',
    'source/renderer.cpp',
    'source/input/impl/glfw_input_stream.cpp',
]

# --==[BUILD!]==--
executable(
    'cgame-client',
    files,
    dependencies: deps,
    include_directories: ['../subprojects/raygui/src', '../subprojects/stb'],
    override_options: [
        'cpp_std=c++20',
    ],
)