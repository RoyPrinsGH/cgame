# --==[SETUP!]==--
deps = []
cmake = import('cmake')
cpp = meson.get_compiler('cpp')

# --==[GLFW]==--
glfw_options = cmake.subproject_options()

glfw_options.add_cmake_defines(
    {
        'GLFW_BUILD_DOCS': false,
        'GLFW_BUILD_TESTS': false,
        'GLFW_BUILD_EXAMPLES': false,
        'GLFW_BUILD_WAYLAND': false,
    },
)

glfw_project = cmake.subproject(
    'glfw',
    options: glfw_options,
)

message('CMake targets:\n - ' + '\n - '.join(glfw_project.target_list()))

deps += glfw_project.dependency('glfw')

if host_machine.system() == 'linux'
    deps += cpp.find_library('m')
elif host_machine.system() == 'windows'
    deps += cpp.find_library('winmm')
endif

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

cgame_platform_inc = include_directories('include')

cgame_platform = static_library(
  'cgame-platform',
  [
    'source/window.cpp',
    'source/glfw_input_hook.cpp',
    'source/glfw_input_stream.cpp',
  ],
  dependencies: deps,
  include_directories: cgame_platform_inc,
  override_options: [
    'cpp_std=c++20',
  ],
)

cgame_platform_dep = declare_dependency(
  link_with: cgame_platform,
  dependencies: deps,
  include_directories: cgame_platform_inc,
)
