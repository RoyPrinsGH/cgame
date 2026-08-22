# --==[SETUP!]==--
deps = []
cmake = import('cmake')

# --==[BULLET]==--
bullet_project = cmake.subproject('bullet')
message('CMake targets:\n - ' + '\n - '.join(bullet_project.target_list()))

deps += bullet_project.dependency('LinearMath')
deps += bullet_project.dependency('BulletCollision')
deps += bullet_project.dependency('BulletDynamics')

# --==[GLM]==--
glm_project = cmake.subproject('glm')
message('CMake targets:\n - ' + '\n - '.join(glm_project.target_list()))

deps += glm_project.dependency('glm')

cgame_physics_inc = include_directories('include')

cgame_physics = static_library(
  'cgame-physics',
  'source/physics_controller.cpp',
  dependencies: deps,
  include_directories: cgame_physics_inc,
)

cgame_physics_dep = declare_dependency(
  link_with: cgame_physics,
  include_directories: cgame_physics_inc,
)