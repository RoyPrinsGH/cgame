# --==[SETUP]==--
deps = []

# --==[ASSETS]==--
deps += cgame_assets_dep

# --==[PROJECT]==--
deps += cgame_project_dep

executable(
    'cgame-cli',
    ['source/main.cpp'],
    dependencies: deps,
    override_options: [
        'cpp_std=c++20',
    ],
)
