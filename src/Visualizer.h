/*
 * Visualizer.h
 *
 *  Created on: 10 Jan 2018
 *      Author: konstantin
 */


#pragma once
#include "ThirdParty/glad.h"
#include <GLFW/glfw3.h>
#include "StaggeredGrid.h"
#include "Shader.h"

/*
 * Visualizer - a helper class for uploading the current state of the simulation to the gpu and rendering it
 * Needs opengl state to be set up and right matrices
 * */
enum class VisualizerMode { UV_GRID, FG_GRID, PRESSURE_GRID, RHS_GRID};

class Visualizer {
public:
	Visualizer(int size);
	/*
	 * Renders the state of grid
	 * */
	void render(StaggeredGrid & grid);
	/*
	 * Choses the rendering mode
	 * options are : UV_GRID, FG_GRID, PRESSURE_GRID, RHS_GRID
	 * */
	void setMode(VisualizerMode mode);
	/*
	 * Initializes the renderer
	 * */
	void initialize();
	/*
	 * Deinitializes the renderer
	 * */
	void deinitialize();
	virtual ~Visualizer();
private:
	//Uploads the selected grid to render
	void uploadRenderGrid(StaggeredGrid & grid);
	VisualizerMode mode_;
	Shader * shader_;
	int size_;
	GLuint quadVBO_, quadVAO_, quadEBO_ , texture1_, texture2_;
};
