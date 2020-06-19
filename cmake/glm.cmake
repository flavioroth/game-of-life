include(FetchContent)


# this requires a recent cmake
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm
	GIT_TAG "master"
)

option(GLM_TEST_ENABLE "Build unit tests" OFF)
FetchContent_GetProperties(glm)
if (NOT glm_POPULATED)
	FetchContent_Populate(glm)
	add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif ()
