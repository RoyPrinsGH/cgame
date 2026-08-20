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

# --==[BULLET]==--
bullet_project = cmake.subproject('bullet')
message('CMake targets:\n - ' + '\n - '.join(bullet_project.target_list()))

deps += bullet_project.dependency('LinearMath')
deps += bullet_project.dependency('BulletCollision')
deps += bullet_project.dependency('BulletDynamics')

# --==[ENET]==--
enet_project = cmake.subproject('enet')
message('CMake targets:\n - ' + '\n - '.join(enet_project.target_list()))

deps += enet_project.dependency('enet_static')

# --==[BOOST]==--
boost_project = cmake.subproject('boost')
message('CMake targets:\n - ' + '\n - '.join(boost_project.target_list()))

deps += boost_project.dependency('boost_dll')
deps += boost_project.dependency('boost_filesystem')

# --==[GLM]==--
glm_project = cmake.subproject('glm')
message('CMake targets:\n - ' + '\n - '.join(glm_project.target_list()))

deps += glm_project.dependency('glm')

# --==[FILES]==--
subdir('assets')
files = [
    'source/client.cpp',
    'source/renderer.cpp',
    'source/input/impl/glfw_input_stream.cpp',
]

# --==[BUILD!]==--
executable(
    'thehighseas-client',
    files,
    dependencies: deps,
    override_options: [
        'cpp_std=c++20',
    ],
)