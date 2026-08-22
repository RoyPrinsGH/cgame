# --==[SETUP!]==--
deps = []
cmake = import('cmake')

# --==[GLM]==--
glm_project = cmake.subproject('glm')
message('CMake targets:\n - ' + '\n - '.join(glm_project.target_list()))

deps += glm_project.dependency('glm')

# --==[RAPIDYAML]==--
ryml_project = cmake.subproject('rapidyaml')
message('CMake targets:\n - ' + '\n - '.join(ryml_project.target_list()))

deps += ryml_project.dependency('ryml')

cgame_assets_inc = include_directories('include')

cgame_assets = static_library(
  'cgame-assets',
  'source/asset_loader.cpp',
  dependencies: deps,
  include_directories: cgame_assets_inc,
)

cgame_assets_dep = declare_dependency(
  link_with: cgame_assets,
  include_directories: cgame_assets_inc,
)