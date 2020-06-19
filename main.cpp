#include "gl_loader.hpp"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine/CellMatrix.hpp"
#include "engine/DoubleBuffer.hpp"
#include "engine/Program.hpp"
#include "engine/Texture.hpp"
#include "engine/GameOfLife.hpp"
#include "engine/CellMatrixRenderer.hpp"
#include "utils/FrequencyAverage.hpp"
#include "utils/RollingBuffer.hpp"

#define LITERAL_GLSL(...) #__VA_ARGS__


struct InputState {
	bool rightMouseDown = false;
	std::vector<glm::vec2> mouseDownLocations;
};

InputState* getInputState(GLFWwindow* window) {
	return static_cast<InputState*>(glfwGetWindowUserPointer(window));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(auto* inputState = getInputState(window)) {
		inputState->rightMouseDown = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if(auto* inputState = getInputState(window)) {
		if(inputState->rightMouseDown) {
			int width;
			int height;
			glfwGetFramebufferSize(window, &width, &height);
			inputState->mouseDownLocations.emplace_back((float) xpos / (float) width, (float) ypos / (float) height);
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

	InputState inputState;
	inputState.mouseDownLocations.reserve(300);
	glfwSetWindowUserPointer(window, &inputState);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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

	auto frag = gl::Shader::make(GL_FRAGMENT_SHADER);
	auto vert = gl::Shader::make(GL_VERTEX_SHADER);

	const char* vertex_src = LITERAL_GLSL(
		\x23 version 330\n
		\x23 extension GL_ARB_explicit_uniform_location
		: require\n layout(location = 0) uniform mat4 uProjMatrix;
		out vec2 vUV;
		const vec2 data[4] = vec2[](vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(1.0, -1.0));
		/* called with glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 ) */
		void main() {
			vec4 vertex = vec4(data[gl_VertexID], 0.0, 1.0);
			vUV = vec2(1.0, -1.0) * (vertex.xy * 0.5 + 0.5);
			gl_Position = vertex;
		});

	const char* frag_src = LITERAL_GLSL(
		\x23 version 330\n
		\x23 extension GL_ARB_explicit_uniform_location
		: require\n in vec2 vUV;
		layout(location = 1) uniform vec2 uResolution;
		layout(location = 2) uniform sampler2D uFrameBuffer;
		out vec4 fragColor;
		void main() {
			vec4 color = texture2D(uFrameBuffer, vUV * 1.0);
			float luminance = min(1.0, color.r * 255.0);
			fragColor = vec4(luminance, luminance, luminance, 1.0);
		});

	frag->setSources({frag_src});
	vert->setSources({vertex_src});

	if(!frag->compile()) {
		fprintf(stderr, "Error compiling fragement shader: %s", frag->getErrorLog().c_str());
	}

	if(!vert->compile()) {
		fprintf(stderr, "Error compiling vertex shader: %s", vert->getErrorLog().c_str());
	}

	glm::mat4x4 mProjMatrix(1.0f);

	gl::Program::Ptr program = gl::Program::make();
	program->addShader(frag);
	program->addShader(vert);

	if(!program->link()) {
		fprintf(stderr, "Error linking program: %s", program->errorLog().c_str());
	}

	program->use();
	program->uniform1i(2, 0);

	// create the image texture
	gl::Texture::Ptr texture = gl::Texture::make();
	texture->setBindTarget(GL_TEXTURE_2D);

	engine::DoubleBuffer<engine::CellMatrix<uint8_t>> cellsBuffer(engine::Size(256, 256));

	for(auto& cell : cellsBuffer.first()) {
		cell = rand() % 2;
	}

	// set filtering
	texture->setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	texture->setParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	gl::GLResource::popErrors("glGenVertexArrays");
	glBindVertexArray(vao);
	gl::GLResource::popErrors("glBindVertexArray");

	// Main loop
	while(!glfwWindowShouldClose(window)) {
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		for(const glm::vec2& point : inputState.mouseDownLocations) {
			int32_t cellUnderMouseX = (int32_t) (point.x * (float) cellsBuffer.first().size().width());
			int32_t cellUnderMouseY = (int32_t) (point.y * (float) cellsBuffer.first().size().height());
			cellsBuffer.first().at(cellUnderMouseX, cellUnderMouseY) = 1;
		}

		inputState.mouseDownLocations.clear();

		// adjust the projection matrix to fit the cell matrix
		int frameWidth = 0;
		int frameHeight = 0;
		glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

		float frameBufferRatio = (float) cellsBuffer.first().size().width() / (float) cellsBuffer.first().size().height();
		float surfaceRatio = (float) frameWidth / (float) frameHeight;

		float projRatio = surfaceRatio / frameBufferRatio;

		if(frameBufferRatio < surfaceRatio) {
			mProjMatrix = glm::ortho(-1.0f * projRatio, 1.0f * projRatio, -1.0f, 1.0f, -1.0f, 1.0f);
		} else {
			mProjMatrix = glm::ortho(-1.0f, 1.0f, -1.0f / projRatio, 1.0f / projRatio, -1.0f, 1.0f);
		}

		// update projection matrix
		program->uniformMatrix4f(0, mProjMatrix);

		// update resolution
		program->uniform2f(1, glm::vec2(frameHeight, frameWidth));


		// compute the next generation and push the resulting data to the gpu
		auto start = std::chrono::steady_clock::now();
		engine::GameOfLife::step(cellsBuffer.first(), cellsBuffer.second());
		renderer::CellMatrixRenderer::render(cellsBuffer.second(), texture->boundTarget());
		uint64_t elapsedNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count();
		uint64_t cellSpeed = (1000000000llu / elapsedNanos) * cellsBuffer.first().size().area();
		cellSpeedCounter.push(cellSpeed);
		cellsBuffer.flip();

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
