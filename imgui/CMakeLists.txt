
add_subdirectory(gl3w)
include(glfw)

set(IMGUI_SRCS
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw.cpp
	imgui_impl_glfw.h
	imgui_impl_opengl3.cpp
	imgui_impl_opengl3.h
	imgui_internal.h
	imgui_widgets.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
	gl_loader.hpp
	gl_loader.cpp
)


add_library(imgui STATIC ${IMGUI_SRCS})
target_link_libraries(imgui gl3w glfw)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
