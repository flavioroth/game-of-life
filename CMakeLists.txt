cmake_minimum_required(VERSION 3.16)
project(game_of_life)

# Provide thread support globally as most components require it
find_package(Threads)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

include(CheckCXXCompilerFlag)
include(CMakeParseArguments)
include(glm)


# Adds the compiler flags specified in FLAGS to the compiler for each build
# type specified in BUILD_TYPES. Warns for each flag unsupported by the compiler.
# if BUILD_TYPES is unspecified, the flags will be added to all build types :
# (Debug, Release, MinSizeRel, RelWithDebInfo)
function(add_compiler_flags)
	set(multiValueArgs BUILD_TYPES FLAGS)
	cmake_parse_arguments(args "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT args_BUILD_TYPES)
		set(args_BUILD_TYPES "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
	endif()

	foreach(FLAG IN LISTS args_FLAGS)
		# it's important to clear this variable otherwise the comparison is done
		# over the previous value
		unset(COMPILER_SUPPORTS CACHE)
		CHECK_CXX_COMPILER_FLAG(${FLAG} COMPILER_SUPPORTS)
		if(COMPILER_SUPPORTS)
			foreach(BUILD_TYPE IN LISTS args_BUILD_TYPES)
				message(STATUS "Adding compiler option ${FLAG} for ${BUILD_TYPE}")
				add_compile_options("$<$<CONFIG:${BUILD_TYPE}>:-march=native>")
			endforeach ()
		else()
			message(WARNING "Option ${FLAG} is not supported by the compiler.")
		endif()
	endforeach()
endfunction()

add_compiler_flags(FLAGS "-march=native" BUILD_TYPES Release RelWithDebInfo)
add_compiler_flags(FLAGS "-Wall" "-Wpedantic" "-Wextra")

add_subdirectory(imgui)


set(SRCS
	engine/GLResource.hpp
	engine/Program.hpp
	engine/Shader.hpp
	engine/Texture.hpp
	engine/Size.hpp
	engine/CellMatrix.hpp
	engine/Swappable.hpp
	engine/GameOfLife.hpp
	engine/CellMatrixRenderer.hpp
	engine/Events.hpp
	engine/EventQueue.hpp
	engine/Camera.hpp
	utils/RollingAverage.hpp
	utils/FrequencyAverage.hpp
	utils/RollingBuffer.hpp
	main.cpp
)

add_executable(game_of_life ${SRCS})

target_link_libraries(game_of_life glfw imgui GLU glm)
