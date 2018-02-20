/*
 * GUI.h
 *
 *  Created on: 20 Jan 2018
 *      Author: konstantin
 */

#pragma once
#include "ThirdParty/glad.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <imgui.h>
#include "Application.h"
class Application;
/*
 * GUI class handles drawing of the GUI, takes input from Application
 * */
class GUI {
public:
	GUI();
	virtual ~GUI();
	/*
	 * Initialises the imgui state as well book keeping
	 * */
	void initialize(int screenWidth, int screenHeight, Application * app);
	/*
	 * Deinitialize the GUI including all the OpenGL resources it has allocated
	 *
	 */
	void deinitialize();

	/*
	 *	Steps the GUI time
	 * */
	void timeStep(float time);
	/*
	 * Shows the GUI
	 * */
	void show();
	/*
	 * Set the X and Y size of the new window
	 * */
	void resizeWindow(int width, int height);

	/*
	 * Alerts the GUI of the current mouse position
	 * */
	void setMousePos(int mouseX, int mouseY);
	/*
	 * Alerts the GUI of currently pressed mouse buttons
	 * */
	void setMouseKey(bool button1, bool button2);
	/*
	 * Sets the GUI scale
	 * */
	void setFramebufferScale(float scaleX, float scaleY);
	/*
	 * Sets whether the GUI should show or not
	 * */
	void setShouldShow(bool shouldShow);

private:
	/*
	 * Renders the GUI. Called by imgui
	 * */
	void renderGUI() const;
	/*
	 * Composes the immediate mode UI
	 * */
	void composeGUI();

	Application *app_;
	bool shouldShow_;
	GLuint fontTexture_;
	Shader * shader_;
	GLuint VBO, VAO, Elements;
	int windowWidth_, windowHeight_;
};

