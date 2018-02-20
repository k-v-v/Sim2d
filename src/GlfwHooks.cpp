/*
 * GlfwHooks.cpp
 *
 *  Created on: 5 Jan 2018
 *      Author: konstantin
 */

#include "Application.h"

extern Application * App;

namespace GLFWHooks{
	void gflwErrorCallback(int errorCode, const char * errorString) {
		std::cout << "GFWL::ERROR :" << "[" << errorCode << "] " << errorString<< "\n";
	}

	void resizeCallback(GLFWwindow * none, int width, int height) {
		App->resizeCallback(width, height);
	}

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
		App->keyboardCallback(key, scancode, action, mods);
	}

	void cursorCallback(GLFWwindow *window, double xPos, double yPos) {
		App->cursorCallback(xPos, yPos);
	}

	void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
		App->scrollCallback(xOffset, yOffset);
	}
}
