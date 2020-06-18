cmake_minimum_required(VERSION 3.16)
project(game_of_life)

# Provide thread support globally as most components require it
find_package(Threads)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(imgui)

add_executable(game_of_life main.cpp)
target_link_libraries(game_of_life glfw imgui)
