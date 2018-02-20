/*
 * CudaStaggeredGrid.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: konstantin
 */
#include <cstddef>
#include "CudaStaggeredGrid.h"
#include "Util.h"

CudaStaggeredGrid::CudaStaggeredGrid(size_t size) {
	gridSize_ = size;
	//UGrid
	allocateMemoryAndResource(&UGrid_, UGridPBO_ , size + 2);
	//VGrid
	allocateMemoryAndResource(&VGrid_, VGridPBO_ , size + 2);
	//PressureGrid
	allocateMemoryAndResource(&PressureGrid_, PressureGridPBO_, size + 2);
	//FGrid
	allocateMemoryAndResource(&FGrid_, FGridPBO_ , size + 2);
	//GGrid
	allocateMemoryAndResource(&GGrid_, GGridPBO_ , size + 2);
	//RHSGrid
	allocateMemoryAndResource(&RHSGrid_, RHSGridPBO_ , size + 2);
}
void CudaStaggeredGrid::allocateMemoryAndResource(float ** devPtr, GLuint & PBO,  size_t size){

	std::cout<<"Initializing with size : "<<size<<std::endl;
	PBO = 0;
	{
		glGenBuffers(1, &PBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);	//Bind the buffer
		glBufferData(GL_PIXEL_UNPACK_BUFFER, size*size*sizeof(float), 0, GL_STREAM_DRAW); //size^2 grid of floats
	}CheckGLError();

    cudaGraphicsResource *intermediateResource; //Intermediate cuda resource used for mapping buffer data to texture memory

	CUDA_CHECK_RETURN(cudaGraphicsGLRegisterBuffer(&intermediateResource, PBO, cudaGraphicsMapFlagsNone));

	CUDA_CHECK_RETURN(cudaGraphicsMapResources(1, &intermediateResource, 0));
	CUDA_CHECK_RETURN(cudaGraphicsResourceGetMappedPointer((void**)devPtr, NULL, intermediateResource));
    CUDA_CHECK_RETURN(cudaGraphicsUnmapResources(1, &intermediateResource, 0));

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);CheckGLError();
}
void CudaStaggeredGrid::uploadGridToBoundTexture(GridType gridType)const{
	switch(gridType){
	case GridType::U_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, UGridPBO_);
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	case GridType::V_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, VGridPBO_);
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	case GridType::F_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, FGridPBO_);
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	case GridType::G_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GGridPBO_);
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	case GridType::PRESSURE_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PressureGridPBO_);
		CheckGLError();
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	case GridType::RHS_GRID:
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, RHSGridPBO_);
		//Copy buffer to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gridSize_+2 , gridSize_+2, 0, GL_RED, GL_FLOAT, NULL);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		break;
	}
	CheckGLError();
}
CudaStaggeredGrid::~CudaStaggeredGrid() {




	glDeleteBuffers(1, &UGridPBO_);
	UGrid_ = (float*)0xDEADBEEEEEEEEEEF;

	glDeleteBuffers(1, &VGridPBO_);
	VGrid_ = (float*)0xDEADBEEEEEEEEEEF;

	glDeleteBuffers(1, &FGridPBO_);
	FGrid_ = (float*)0xDEADBEEEEEEEEEEF;

	glDeleteBuffers(1, &GGridPBO_);
	GGrid_ = (float*)0xDEADBEEEEEEEEEEF;

	glDeleteBuffers(1, &RHSGridPBO_);
	RHSGrid_ = (float*)0xDEADBEEEEEEEEEEF;

	glDeleteBuffers(1, &PressureGridPBO_);
	PressureGrid_ = (float*)0xDEADBEEEEEEEEEEF;

}
float *CudaStaggeredGrid::getUGrid(){
	return UGrid_;
}
float *CudaStaggeredGrid::getVGrid(){
	return VGrid_;
}
float *CudaStaggeredGrid::getPressureGrid(){
	return PressureGrid_;
}
float *CudaStaggeredGrid::getFGrid(){
	return FGrid_;
}
float *CudaStaggeredGrid::getGGrid(){
	return GGrid_;
}
float *CudaStaggeredGrid::getRHSGrid(){
	return RHSGrid_;
}


