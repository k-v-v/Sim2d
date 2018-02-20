/*
 * main.cpp
 *
 *  Created on: 2 Jan 2018
 *      Author: konstantin
 */


#include "ThirdParty/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cfenv>
#include <cmath>


#include "Application.h"
Application * App;

int main(int argc, char * argv[]){
	//Initialize everything
	App = new Application();
	App->initialize(argc, argv);

	//Main loop
	while(!App->shouldQuit()){
		glfwWaitEventsTimeout(1.0/60.0);//60 fps!
		App->render();
	}

}

