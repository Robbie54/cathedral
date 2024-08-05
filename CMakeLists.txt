cmake_minimum_required(VERSION 3.0.0)
project(Main VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()


# Find SFML shared libraries
find_package(SFML 2.5 REQUIRED graphics window system)

# Include the directories for SFML
include_directories(${SFML_INCLUDE_DIR})

add_executable(Main Source/main.cpp Source/draw_board.cpp Source/board_utility.cpp Source/player_turn.cpp Source/mcts.cpp Source/matrix_utility.cpp MonteCarloTreeSearch-main/mcts/src/mcts.cpp MonteCarloTreeSearch-main/mcts/src/JobScheduler.cpp
)

target_link_libraries(Main sfml-graphics sfml-window sfml-system pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
