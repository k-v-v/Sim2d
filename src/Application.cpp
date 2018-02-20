/*
 * GraphicManager.cpp
 *
 *  Created on: 5 Jan 2018
 *      Author: konstantin
 */
#include "Constants.h"
#include "Application.h"
#include "GlUtil.h"

#include <fenv.h> //for throwing exceptions during airthmetic operations

namespace GLFWHooks {
	extern void gflwErrorCallback(int errorCode, const char *errorString);
	extern void resizeCallback(GLFWwindow *none, int width, int height);
	extern void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	extern void cursorCallback(GLFWwindow *window, double xPos, double yPos);
	extern void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
}

Application::Application() {
	//Nothing here
}
int Application::initialize(int argc, char *argv[]) {
	initialized_ = true;
	initGLFW();
	initOpenGL();

	//Initialize the GUI
	gui_ = new GUI();
	gui_->initialize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, this);


	//Initialize a single core solver
	//TODO: Set delta time to reasonable value

	solver_= new SingleCoreSolver(SimulationSize, REYNOLD, 1.0, 0.0f, 0.0f, SimulationSize, BoundaryConditions::NO_SLIP);


	visualizer_ = new Visualizer(SimulationSize);
	visualizer_->initialize();
}
int Application::deinitialize() {
	initialized_ = false;

	glfwDestroyWindow(window_);
	glfwTerminate();

	delete solver_;
	visualizer_->deinitialize();
	delete visualizer_;
}

void Application::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);CheckGLError();
	//Render stuff

	//solver_->getGrid().getUGrid()(32, 32) = 10;
	//solver_->getGrid().getVGrid()(32, 31) = 10;


	//feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);  // Enable all floating point exceptions but FE_INEXACT

	solver_->step();

	fedisableexcept(FE_ALL_EXCEPT);  // Disable all floating point exceptions

	visualizer_->render(solver_->getGrid());


	float deltaTime =time_ > 0.0 ? (float) (glfwGetTime() - time_) : (float) (1.0f / 60.0f);

	handleUIInput(deltaTime);
	renderUI();

	glfwSwapBuffers(window_);
}
void Application::renderUI() {
	//Start rendering Imgui
	gui_->show();
}

void Application::setTitle(const std::string& title) {
	glfwSetWindowTitle(window_, title.c_str());
	std::cout << "Setting title to " << title << ".\n";
}

void Application::keyboardCallback(int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE) {
			deinitialize();
			return;
		}
		std::cout << "Key " << (char) (key) << " pressed\n";
	}
}

void Application::cursorCallback(double xPos, double yPos) {
	//If left mouse is clicked set the pressure to the corresponding cell
	//If left mouse button not pressed
	if(glfwGetMouseButton(window_, 0) ){
		float relativeX = xPos / windowWidth_;
		float relativeY = yPos / windowHeight_;


		int x = int(relativeX*solver_->getGridSize());
		int y = int(solver_->getGridSize() - relativeY*solver_->getGridSize());
		solver_->increasePressure(x, y, 10.0f);
		std::cout<<"Applying pressure to :x="<<x<<" y="<<y<<std::endl;
	}

	if(glfwGetMouseButton(window_, 1) ){
		float relativeX = xPos / windowWidth_;
		float relativeY = yPos / windowHeight_;


		int x = int(relativeX*solver_->getGridSize());
		int y = int(solver_->getGridSize() - relativeY*solver_->getGridSize());
		solver_->increaseVelocity(x, y, 0.0f, 1.0f);
		std::cout<<"Applying velocity to :x="<<x<<" y="<<y<<std::endl;
	}

}

void Application::scrollCallback(double xOffset, double yOffset) {
}

Application::~Application() {
	assert(initialized_ == false);
}
int Application::initOpenGL() {
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialise glad\n";
		return -1;
	}
	glViewport(0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glClearColor(50, 18, 122, 255);
	return 0;
}

void Application::resizeCallback(int width, int height) {
	windowWidth_ = width;
	windowHeight_ = height;
	glViewport(0, 0, width, height);
	//Get the internal framebuffer sizes
	glfwGetFramebufferSize(window_, &frameBufferWidth_, &frameBufferHeight_);
	std::cout << "Resizing window to [" << width << ", " << height << "].\n";
}


int Application::initGLFW() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialise GLFW\n";
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	windowWidth_ = DEFAULT_SCREEN_WIDTH;
	windowHeight_ = DEFAULT_SCREEN_HEIGHT;
	window_ = glfwCreateWindow(windowWidth_, windowHeight_, "Sim2d Navier-Stokes", NULL, NULL);
	if (window_ == nullptr) {
		std::cout << "Failed to create Main Window\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window_);

	//Set up the call-backs
	glfwSetFramebufferSizeCallback(window_, GLFWHooks::resizeCallback);
	glfwSetCursorPosCallback(window_, GLFWHooks::cursorCallback);
	glfwSetKeyCallback(window_, GLFWHooks::keyboardCallback);
	glfwSetScrollCallback(window_, GLFWHooks::scrollCallback);
	glfwSetErrorCallback(GLFWHooks::gflwErrorCallback);

	return 0;
}
bool Application::shouldQuit() {
	//return false;
	return glfwWindowShouldClose(window_);
}

void Application::handleUIInput(float deltaTime){
	ImGuiIO& io = ImGui::GetIO();
		// Setup display size (every frame to accommodate for window resizing)

		io.DisplaySize = ImVec2((float) windowWidth_, (float) windowHeight_);
		io.DisplayFramebufferScale = ImVec2(
				windowWidth_ > 0 ? ((float) frameBufferWidth_ / windowWidth_) : 0,
				windowHeight_ > 0 ? ((float) windowHeight_ / windowHeight_) : 0);

		// Setup time step
		double current_time = glfwGetTime();
		//Just a hack to get delta time or return a legit value if this is the first frame
		gui_->timeStep(time_ > 0.0 ? (float) (current_time - time_) : (float) (1.0f / 60.0f));
		time_ = current_time;

		//Handle input
		if (glfwGetWindowAttrib(window_, GLFW_FOCUSED)) {
			if (io.WantMoveMouse) {
				glfwSetCursorPos(window_, (double) io.MousePos.x,
						(double) io.MousePos.y); // Set mouse position if requested by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
			} else {
				double mouse_x, mouse_y;
				glfwGetCursorPos(window_, &mouse_x, &mouse_y);
				// Get mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.}
				io.MousePos = ImVec2((float) mouse_x, (float) mouse_y);
			}
		} else {
			io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		}
		if (io.WantMoveMouse) {
			glfwSetCursorPos(window_, (double) io.MousePos.x,
					(double) io.MousePos.y); // Set mouse position if requested by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
		}

		for (int i = 0; i < 3; i++) {
			// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
			io.MouseDown[i] = glfwGetMouseButton(window_, i) != 0;
		}

		glfwSetInputMode(window_, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}
void Application::setVisualizationMode(int mode){
	VisualizerMode visMode;
	switch(mode){
	case 0:
		visMode = VisualizerMode::UV_GRID;
		std::cout<<"Setting VisualizerMode::UV_GRID\n";
		break;
	case 1:
		std::cout<<"Setting VisualizerMode::PRESSURE_GRID\n";
		visMode = VisualizerMode::PRESSURE_GRID;
		break;
	case 2:
		std::cout<<"Setting VisualizerMode::RHS_GRID\n";
		visMode = VisualizerMode::RHS_GRID;
		break;
	case 3:
		std::cout<<"Setting VisualizerMode::FG_GRID\n";
		visMode = VisualizerMode::FG_GRID;
		break;
	default:
		visMode = VisualizerMode::UV_GRID;
		std::cout<<"Setting VisualizerMode::UV_GRID because there is an error.\n";
	}
	visualizer_->setMode(visMode);
}
void Application::setReynold(float reynold){
	solver_->setReynold(reynold);
}
void Application::setSimulationLength(float length){
	solver_->setLength(length);
}
void Application::resetSimulation(float reynold, float deltaTime, float length){
	solver_->reset(reynold, deltaTime, length);
}
void Application::setDeltaTime(float deltaTime){
	solver_->setDeltaTime(deltaTime);
}
bool Application::isStable(){
	return solver_->checkStabilityConditions();
}
