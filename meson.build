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

executable(
    'thehighseas',
    'main.c',
    dependencies: deps,
)