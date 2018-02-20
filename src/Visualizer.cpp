/*
 * Visualizer.cpp
 *
 *  Created on: 20 Jan 2018
 *      Author: konstantin
 */

#include "Visualizer.h"
#include "GlUtil.h"
static std::array<glm::vec3, 6> DisplayQuad =
	{
		glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f),glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f)
	};
float vertices[] = {
    // positions         // texture coords
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 				// top right
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 				// bottom right
    -1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 				// bottom left
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  				// top left
};

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};
Visualizer::Visualizer(int size):size_(size) {
}
void Visualizer::render(StaggeredGrid & grid){


	//grid.getUGrid()(32, 32) = 10;
	//grid.getVGrid()(32, 31) = 10;


	//Upload new data to the texture
	shader_->use();CheckGLError();
	uploadRenderGrid(grid);

    glBindVertexArray(quadVAO_);CheckGLError();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);CheckGLError();
}

void Visualizer::initialize(){
	printf("OpenGL version is (%s)\n", glGetString(GL_VERSION));
	shader_ = new Shader("simu.vert", "simu.frag");CheckGLError();
	shader_->use();CheckGLError();

	quadVBO_ = 0;
	{
		glGenBuffers(1, &quadVBO_);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
		//Copy quad coordinates to video memory
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}CheckGLError();
	quadVAO_ = 0;
	{
		glGenVertexArrays(1, &quadVAO_);
		glBindVertexArray(quadVAO_);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}CheckGLError();
	quadEBO_ = 0;
	{
	    glGenBuffers(1, &quadEBO_);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO_);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}CheckGLError();
	{
		//Position attribute
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	    glEnableVertexAttribArray(0);
	    //Texture coordinate attribute
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	    glEnableVertexAttribArray(1);
	}CheckGLError();
	//Red
	texture1_ = 0;
	{
		glGenTextures(1, &texture1_);
		glBindTexture(GL_TEXTURE_2D, texture1_);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED , size_ + 2 , size_ + 2 , 0, GL_RED, GL_FLOAT, nullptr);
		glBindTexture( GL_TEXTURE_2D, 0);	//Unbind our current texture
	}CheckGLError();
	texture2_ = 0;
	{
		glGenTextures(1, &texture2_);CheckGLError();
		glBindTexture(GL_TEXTURE_2D, texture2_);CheckGLError();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);CheckGLError();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);CheckGLError();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED , size_ + 2 , size_ + 2 , 0, GL_RED, GL_FLOAT, nullptr);
		glBindTexture( GL_TEXTURE_2D, 0);	//Unbind our current texture
	}CheckGLError();

	(*shader_)["Texture1"] = 0 ;
	(*shader_)["Texture2"] = 1 ;
	mode_ = VisualizerMode::UV_GRID;
}

void Visualizer::deinitialize(){
	glDeleteBuffers(1, &quadVBO_);
	quadVBO_ = 0;

	glDeleteBuffers(1, &quadEBO_);
	quadEBO_ =0;

	glDeleteTextures(1, &texture1_);
	texture1_ = 0;

	glDeleteTextures(1, &texture2_);
	texture2_ = 0;


	delete shader_;
	shader_ = nullptr;
}
Visualizer::~Visualizer() {
	assert(quadEBO_ == 0);
	assert(quadVBO_ == 0);
	assert( quadVAO_ == 0);
	assert(texture1_ == 0);
	assert(texture2_ == 0);
	//Need to clean up texture
}
void Visualizer::setMode(VisualizerMode mode){
	mode_ = mode;
}
void Visualizer::uploadRenderGrid(StaggeredGrid & grid){
	switch(mode_){
	case VisualizerMode::UV_GRID:
		//Upload the first texture to gpu
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texture1_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getUGrid().getRawPointer());CheckGLError();

		//Upload the second texture to gpu
	    glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, texture2_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getVGrid().getRawPointer());CheckGLError();
		break;

	case VisualizerMode::FG_GRID:
		//Upload the first texture
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texture1_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getFGrid().getRawPointer());CheckGLError();

		//Upload the second texture
	    glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, texture2_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getGGrid().getRawPointer());CheckGLError();
		break;
	case VisualizerMode::RHS_GRID:
		//Upload the first texture
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texture1_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getRHSGrid().getRawPointer());CheckGLError();
		//Disable the second texture
		glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, 0);
		break;
	case VisualizerMode::PRESSURE_GRID:
		//Upload the first texture
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texture1_);
		glTexImage2D(GL_TEXTURE_2D, 0,   GL_RED  , size_ + 2, size_ +2 , 0, GL_RED, GL_FLOAT, grid.getPressureGrid().getRawPointer());CheckGLError();
		//Disable the second texture
		glActiveTexture(GL_TEXTURE1);
	    glBindTexture(GL_TEXTURE_2D, 0);

		break;
	default:
		assert(false);
	}
}
