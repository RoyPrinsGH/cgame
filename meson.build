project('thehighseas', 'c', 'cpp')

cmake = import('cmake')

raylib_project = cmake.subproject('raylib')
raylib_dep = raylib_project.dependency('raylib')

cpp = meson.get_compiler('cpp')

deps = [raylib_dep]

if host_machine.system() == 'linux'
    deps += cpp.find_library('m')
elif host_machine.system() == 'windows'
    deps += cpp.find_library('winmm')
endif

bullet_project = cmake.subproject('bullet')
message('CMake targets:\n - ' + '\n - '.join(bullet_project.target_list()))
bullet_dep = bullet_project.dependency('BulletDynamics')

deps += bullet_dep

executable(
    'thehighseas',
    'main.c',
    dependencies: deps,
)