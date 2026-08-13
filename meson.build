# --==[SETUP!]==--
project('thehighseas', 'c', 'cpp')
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

deps += bullet_project.dependency('BulletDynamics')
deps += bullet_project.dependency('BulletCollision')
deps += bullet_project.dependency('LinearMath')

# --==[ENET]==--
enet_project = cmake.subproject('enet')
message('CMake targets:\n - ' + '\n - '.join(enet_project.target_list()))

deps += enet_project.dependency('enet_static')

# --==[BUILD!]==--
executable(
    'thehighseas',
    'main.c',
    dependencies: deps,
)

executable(
    'thehighseas-bullet',
    'bullet.cpp',
    dependencies: deps,
)