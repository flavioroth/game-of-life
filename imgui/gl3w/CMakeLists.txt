set(GL3W_SRCS
	gl3w.c
	include/GL/gl3w.h
	include/GL/glcorearb.h
	include/KHR/khrplatform.h
)

add_library(gl3w STATIC ${GL3W_SRCS})
target_include_directories(gl3w PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

