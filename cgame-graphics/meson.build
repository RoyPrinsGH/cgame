# --==[SETUP!]==--
deps = []
cmake = import('cmake')

# --==[GLM]==--
glm_project = cmake.subproject('glm')
message('CMake targets:\n - ' + '\n - '.join(glm_project.target_list()))

deps += glm_project.dependency('glm')

# --==[FASTGLTF]==--
fastgltf_project = cmake.subproject('fastgltf')
message('CMake targets:\n - ' + '\n - '.join(fastgltf_project.target_list()))

deps += fastgltf_project.dependency('fastgltf')

# --==[ASSETS]==--
deps += cgame_assets_dep

cgame_graphics_inc = include_directories('include')

cgame_graphics_private_inc = include_directories(
    '../subprojects/raylib/src',
    '../subprojects/stb',
)

# --==[FILES]==--
files = [
    'source/gltf_loader.cpp',
    'source/model_controller.cpp',
    'source/shader_controller.cpp',
    'source/backend/gl33_backend.cpp',
]

# --==[BUILD!]==--
cgame_graphics = static_library(
    'cgame-graphics',
    files,
    dependencies: deps,
    include_directories: [cgame_graphics_inc, cgame_graphics_private_inc],
    override_options: [
        'cpp_std=c++20',
    ],
)

cgame_graphics_dep = declare_dependency(
    link_with: cgame_graphics,
    dependencies: deps,
    include_directories: cgame_graphics_inc,
)