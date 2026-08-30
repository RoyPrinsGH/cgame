# --==[SETUP!]==--
deps = []
cmake = import('cmake')

# --==[RAPIDYAML]==--
ryml_project = cmake.subproject('rapidyaml')
message('CMake targets:\n - ' + '\n - '.join(ryml_project.target_list()))

deps += ryml_project.dependency('ryml')

# --==[ASSETS]==--
deps += cgame_assets_dep

cgame_project_inc = include_directories('include')

cgame_project = static_library(
  'cgame-project',
  'source/project.cpp',
  dependencies: deps,
  include_directories: cgame_project_inc,
  override_options: [
    'cpp_std=c++20',
  ],
)

cgame_project_dep = declare_dependency(
  link_with: cgame_project,
  dependencies: deps,
  include_directories: cgame_project_inc,
)
