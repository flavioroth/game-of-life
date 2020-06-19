cmake_minimum_required(VERSION 3.16)
project(game_of_life)

# Provide thread support globally as most components require it
find_package(Threads)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

include(glm)
add_subdirectory(imgui)

set(SRCS
	engine/GLResource.hpp
	engine/Program.hpp
	engine/Shader.hpp
	engine/Texture.hpp
	engine/Size.hpp
	engine/CellMatrix.hpp
	engine/DoubleBuffer.hpp
	engine/GameOfLife.hpp
	engine/CellMatrixRenderer.hpp
	utils/RollingAverage.hpp
	utils/FrequencyAverage.hpp
	utils/RollingBuffer.hpp
	main.cpp
)

add_executable(game_of_life ${SRCS})

target_link_libraries(game_of_life glfw imgui GLU glm)
