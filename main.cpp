#include "gl_loader.hpp"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine/CellMatrix.hpp"
#include "engine/Swappable.hpp"
#include "engine/Program.hpp"
#include "engine/Texture.hpp"
#include "engine/GameOfLife.hpp"
#include "engine/Events.hpp"
#include "engine/EventQueue.hpp"
#include "engine/CellMatrixRenderer.hpp"
#include "engine/Camera.hpp"
#include "utils/FrequencyAverage.hpp"
#include "utils/RollingBuffer.hpp"

#include <glm/gtx/matrix_decompose.hpp>

using namespace engine;

EventQueue* getEventQueue(GLFWwindow* window) {
	return static_cast<EventQueue*>(glfwGetWindowUserPointer(window));
}

static glm::dvec2 getCursorPosition(GLFWwindow *window) {
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return glm::dvec2(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	if(auto *eventQueue = getEventQueue(window)) {
		if(yoffset < 0.0f) {
			eventQueue->pushEvent(std::make_unique<MouseWheelEvent>(MouseWheelInput::Direction::Down));
		} else {
			eventQueue->pushEvent(std::make_unique<MouseWheelEvent>(MouseWheelInput::Direction::Up));
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	auto cursorPosition = getCursorPosition(window);
	if(auto*eventQueue = getEventQueue(window)) {
		if(button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) {
			auto mouseButton = button == GLFW_MOUSE_BUTTON_RIGHT ? MouseButtonInput::Right : MouseButtonInput::Left;
			if(action == GLFW_PRESS) {
				eventQueue->pushEvent(std::make_unique<MousePressEvent>(mouseButton, cursorPosition));
			} else if(action == GLFW_RELEASE) {
				eventQueue->pushEvent(std::make_unique<MouseReleaseEvent>(mouseButton, cursorPosition));
			}
		}
	}
}

int main() {
	if(!glfwInit())
		return 1;

	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1024, 1024, "Game Of Life", NULL, NULL);
	if(window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);// Enable vsync

	EventQueue eventQueue;
	glfwSetWindowUserPointer(window, &eventQueue);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if(gl_loader::init()) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);

	ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	utils::FrequencyAverage<5, float> fpsCounter;
	utils::RollingBuffer<60, float> fpsHistory;
	utils::RollingAverage<uint64_t, float, 16> cellSpeedCounter;

	renderer::CellMatrixRenderer matrixRenderer;
	std::vector<CellMatrix<uint8_t>> buffers (2, Size(512, 512));
	Swappable<CellMatrix<uint8_t>> cellBuffers(buffers[0], buffers[1]);

	for(auto& cell : cellBuffers.first()) {
		cell = rand() % 2;
	}

	GLuint vao;
	glGenVertexArrays(1, &vao);
	gl::GLResource::popErrors("glGenVertexArrays");
	glBindVertexArray(vao);
	gl::GLResource::popErrors("glBindVertexArray");

	Camera camera;

	bool rightMouseIsDown = false;
	bool leftMouseIsDown = false;
	bool dragOperation = false;
	glm::vec2 dragStartPosition(0, 0);
	glm::vec2 dragVector(0, 0);
	glm::vec2 cameraPosition(0, 0);

	// Main loop
	while(!glfwWindowShouldClose(window)) {
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		int frameWidth = 0;
		int frameHeight = 0;
		glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

		while(std::unique_ptr<InputEvent> event = eventQueue.popEvent()) {
			switch(event->type()) {
				case InputEvent::MousePress: {
					MousePressEvent* e = static_cast<MousePressEvent*>(event.get());
					if(e->button == MouseButtonInput::Right) {
						rightMouseIsDown = true;
					} else if(e->button == MouseButtonInput::Left) {
						leftMouseIsDown = true;
						dragOperation = true;
						dragStartPosition = glm::vec2(e->windowCoordinates / glm::dvec2(frameWidth, frameHeight));
					}
					break;
				}
				case InputEvent::MouseRelease: {
					MouseReleaseEvent* e = static_cast<MouseReleaseEvent*>(event.get());
					if(e->button == MouseButtonInput::Right) {
						rightMouseIsDown = false;
					} else if(e->button == MouseButtonInput::Left) {
						leftMouseIsDown = false;
						dragOperation = false;
						camera.integrateDisplacement();
					}
					break;
				}
				case InputEvent::MouseWheel: {
					MouseWheelEvent* e = static_cast<MouseWheelEvent*>(event.get());

					glm::vec2 ratio = glm::vec2(frameWidth, frameHeight) / glm::vec2(cellBuffers.first().size().vec());
					glm::vec2 cursorCoordinatesWindowUV = glm::vec2(getCursorPosition(window) / glm::dvec2(frameWidth, frameHeight));
					glm::vec2 zoomCenterInSimCoordinates = cursorCoordinatesWindowUV * ratio;

					if(e->direction == MouseWheelInput::Down) {
						camera.zoomIn(1.05,glm::vec2(zoomCenterInSimCoordinates.x, ratio.y - zoomCenterInSimCoordinates.y));
					} else if(e->direction == MouseWheelInput::Up) {
						camera.zoomOut(1.05,glm::vec2(zoomCenterInSimCoordinates.x, ratio.y - zoomCenterInSimCoordinates.y));
					}
					break;
				}
			}
		}

		glm::vec2 ratio = glm::vec2(frameWidth, frameHeight) / glm::vec2(cellBuffers.first().size().vec());
		glm::vec2 cursorPosition = glm::vec2(getCursorPosition(window) / glm::dvec2(frameWidth, frameHeight));
		dragVector = (dragStartPosition - cursorPosition) * glm::vec2(1, -1);


		if(dragOperation) {
			camera.setDragDisplacement(dragVector);
		}

		// compute the next generation and push the resulting data to the gpu
		auto start = std::chrono::steady_clock::now();
		GameOfLife::step(cellBuffers.first(), cellBuffers.second());
		uint64_t elapsedNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count();

		matrixRenderer.prepare(cellBuffers.second().size(), camera.buildTransformMatrix());
		matrixRenderer.render(cellBuffers.second());

		uint64_t cellSpeed = (1000000000llu / elapsedNanos) * cellBuffers.first().size().area();
		cellSpeedCounter.push(cellSpeed);
		cellBuffers.swap();


		// draw the cell matrix
		glViewport(0, 0, frameWidth, frameHeight);
		gl::GLResource::popErrors("glViewport");

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		gl::GLResource::popErrors("glClearColor");

		glClear(GL_COLOR_BUFFER_BIT);
		gl::GLResource::popErrors("glClear");

		glBindVertexArray(vao);
		gl::GLResource::popErrors("glBindVertexArray");

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		gl::GLResource::popErrors("glDrawArrays");

		glBindVertexArray(0);
		gl::GLResource::popErrors("glBindVertexArray");


		// draw the UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float currentFPS = fpsCounter.averageFrequencyHz();
		fpsHistory.push(currentFPS);

		ImGui::Begin("Game of life", nullptr);
		ImGui::Text("Right click to set a cell");
		ImGui::Text("FPS : %.1f", currentFPS);
		ImGui::Text("Cell/s : %.0f", cellSpeedCounter.currentAverage());
		ImGui::Text("Cursor Postion/s : %.2f %0.2f", cursorPosition.x, cursorPosition.y);
		ImGui::PlotHistogram("", fpsHistory.values(), fpsHistory.size(), 0, nullptr, .0f, 120.0f, ImVec2(100, 30));
		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		fpsCounter.update();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
