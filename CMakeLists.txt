cmake_minimum_required(VERSION 3.0.0)
project(Mint VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()


# Find SFML shared libraries
find_package(SFML 2.5 REQUIRED graphics window system)

# Include the directories for SFML
include_directories(${SFML_INCLUDE_DIR})

add_executable(Main Source/Main.cpp Source/GameState.cpp Source/ScreenRenderer.cpp Source/PlayerTurn.cpp Source/CathedralState.cpp Source/EvaluationMetric.cpp Source/GameManager.cpp Source/MatrixUtility.cpp MonteCarloTreeSearch-main/mcts/src/mcts.cpp MonteCarloTreeSearch-main/mcts/src/JobScheduler.cpp
)

target_link_libraries(Main sfml-graphics sfml-window sfml-system pthread)

# Define PARALLEL_ROLLOUTS to enable parallel rollouts
# target_compile_definitions(Main PRIVATE PARALLEL_ROLLOUTS)

set_target_properties(Main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


# add_executable(Mint MonteCarloTreeSearch-main/examples/Quoridor/main.cpp MonteCarloTreeSearch-main/examples/Quoridor/Quoridor.cpp MonteCarloTreeSearch-main/mcts/src/mcts.cpp MonteCarloTreeSearch-main/mcts/src/JobScheduler.cpp
# )


# target_link_libraries(Mint sfml-graphics sfml-window sfml-system pthread)

# # Define PARALLEL_ROLLOUTS to enable parallel rollouts
# target_compile_definitions(Main PRIVATE PARALLEL_ROLLOUTS)

# set_target_properties(Mint PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
# )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

