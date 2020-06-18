include(ExternalProject)

set(GLFW_INSTALL_LOCATION "${CMAKE_BINARY_DIR}/external/glfw")
set(GLFW_INCLUDE_PATH "${GLFW_INSTALL_LOCATION}/include")
set(GLFW_LIB_PATH "${GLFW_INSTALL_LOCATION}/lib")

find_library(TRY_FIND_GLFW
	NAMES libglfw3.a
	PATHS ${GLFW_LIB_PATH}
	DOC "Find glfw static library"
	NO_DEFAULT_PATH
)


if (NOT TRY_FIND_GLFW AND NOT TARGET compile_glfw)
	message(STATUS "GLFW library not found. Will download and install in ${GLFW_INSTALL_LOCATION}")
	ExternalProject_Add(compile_glfw
			GIT_REPOSITORY https://github.com/glfw/glfw
			GIT_TAG 3.3-stable
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GLFW_INSTALL_LOCATION}
					   -DGLFW_BUILD_DOCS=OFF
					   -DGLFW_BUILD_TESTS=OFF
					   -DGLFW_BUILD_EXAMPLES=OFF
			)
endif ()


# this directory has to exist for INTERFACE_INCLUDE_DIRECTORIES to work
file(MAKE_DIRECTORY ${GLFW_INCLUDE_PATH})

set(GLFW_LIBRARY "${GLFW_LIB_PATH}/libglfw3.a")


# defines the libgtest library
if (NOT TARGET glfw)
	add_library(glfw IMPORTED STATIC GLOBAL)
	add_dependencies(glfw compile_glfw)

	set_target_properties(glfw PROPERTIES
		"IMPORTED_LOCATION" ${GLFW_LIBRARY}
		"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
		"INTERFACE_INCLUDE_DIRECTORIES" ${GLFW_INCLUDE_PATH}
	)
endif ()
