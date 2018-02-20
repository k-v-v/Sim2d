/*
 * GraphicManager.hpp
 *
 *  Created on: 5 Jan 2018
 *      Author: konstantin
 */
/* Manages the main window and OpenGL context
 * There should be only 1 active instance in a program.
 */

#pragma once

#include <string>

#include "Shader.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "ThirdParty/glad.h"
#include "GUI.h"
#include "Solver.h"
#include "SingleCoreSolver.h"
#include "Visualizer.h"
/*!
 * \class Application
 *
 * \brief Manages the window, rendering and input
 */
class GUI;
class Application {
public:
	Application();
	int initialize(int argc, char * argv[]);
	int deinitialize();
	void render();

	void renderUI();

	bool shouldQuit();
	/*
	 * \brief Sets the window title
	 */
	void setTitle(const std::string &title);
	/*
	 * \brief Called by GLFW upon resizing the window
	 * */
	void resizeCallback(int width, int height);
	/*
	 * \brief Called by GLFW upon catching a keyboard key press
	 * */
	void keyboardCallback(int key, int scancode, int action, int mods);
	/*
	 * \brief Called by GLFW to upon cursor movement
	 * */
	void cursorCallback(double xPos, double yPos);
	/*
	 * \brief Called by GLFW upon mouse scroll
	 * */
	void scrollCallback(double xOffset, double yOffset);
	/*
	 * \Handle UI intput
	 * */
	void handleUIInput(float deltaTime);
	/*
	 * Set the visualization mode options are:
	 * UV_GRID, FG_GRID, PRESSURE_GRID, RHS_GRID
	 * */
	void setVisualizationMode(int mode);
	/*
	 * Sets the reynold number for the current simulation
	 * */
	void setReynold(float reynold);
	/*
	 * Sets the length of the simulation mesh
	 * */
	void setSimulationLength(float cellSize);
	/**
	 *	Sets the simulation detlaTime
	 */
	void setDeltaTime(float deltaTime);
	/*
	 * Resets the simulation with the given parameters
	 * */
	void resetSimulation(float reynold, float deltaTime, float length);
	/*
	 *	Is the simulation stable? (are the constants in the correct proportions)
	 * */
	bool isStable();

private:
	int initOpenGL();
	int initGLFW();

private:
	Visualizer * visualizer_;
	CPUSolver * solver_;
	GUI *gui_;
	GLFWwindow * window_;
	bool initialized_;
	int windowWidth_, windowHeight_;
	int frameBufferWidth_, frameBufferHeight_;
	float time_;
	~Application();
};
