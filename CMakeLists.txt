cmake_minimum_required(VERSION 3.16)
project(game_of_life)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

include(glfw)

add_executable(game_of_life main.cpp)
target_link_libraries(game_of_life glfw)
