/*
 * CudaSolver.cpp
 *
 *  Created on: 12 Feb 2018
 *      Author: konstantin
 */
#include <iostream>
#include "CudaSolver.h"
#include "CudaSolverKernels.h"
#include "CudaSolverKernels.h"

CudaSolver::CudaSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions):
				Solver(gridSize, reynold, deltaTime, gravityX, gravityY, length, conditions), grid_(gridSize) {
	//TODO Auto-generated constructor stub

	setReynold(reynold);
	setDeltaTime(deltaTime);
	setLength(length);
	setOmega();
}


CudaSolver::~CudaSolver() {
	// TODO Auto-generated destructor stub
}
void CudaSolver::step(){
	this->computeFGGrid();
	applyBoundaryConditions();
	computeRHSGrid();
	//Do SOR iteration

	for(int it=0; it <SOR_ITER_MAX; it++){
		doSorStep();
		//TODO find best residual sum values
		//if(residualSum < ???) break;
	}

	//SOR
	//Now compute new velocity values
	computeUVGrid();

}
void CudaSolver::reset(float reynold, float deltaTime, float length){
	setReynold(reynold);
	setDeltaTime(deltaTime);
	setLength(length);
	clear_staggered_grid_host(
			grid_.getUGrid(),
			grid_.getVGrid(),
			grid_.getFGrid(),
			grid_.getGGrid(),
			grid_.getRHSGrid(),
			grid_.getPressureGrid(),
			gridSize_ + 2);

}
void CudaSolver::increasePressure(int x, int y, float pressure){
	setGridPoint_host(grid_.getPressureGrid(), y*(gridSize_+1) + x + 1, 2.0f);
	std::cout<<"Cuda increasing pressure\n";
}
void CudaSolver::increaseVelocity(int x, int y, float uComponent, float vComponent){
	setGridPoint_host(grid_.getUGrid(), y*(gridSize_+1) + x + 1, 2.0f);
	std::cout<<"Cuda increasing velocity\n";

}
void CudaSolver::computeFGGrid(){
	computeFGGrid_host(	grid_.getUGrid(),
						grid_.getVGrid(),
						grid_.getFGrid(),
						grid_.getGGrid(),
						gridSize_ );
}
void CudaSolver::applyBoundaryConditions(){
	//Null the velocity boundary conditions
	null_boundary_host(grid_.getUGrid(), gridSize_);
	null_boundary_host(grid_.getVGrid(), gridSize_);
}
void CudaSolver::computeRHSGrid(){
	computeRHSGrid_host(grid_.getFGrid(),
						grid_.getGGrid(),
						grid_.getRHSGrid(),
						gridSize_);

}
void CudaSolver::doSorStep(){
	doSorStep_host( grid_.getPressureGrid(),
					grid_.getRHSGrid(),
					gridSize_);
}
void CudaSolver::computeUVGrid(){
	//void computeUVHost(float *pGrid, float *uGrid, float * vGrid, float *fGrid, float *gGrid, int innerEdgeSize){
	computeUV_host(	grid_.getPressureGrid(),
					grid_.getUGrid(),
					grid_.getVGrid(),
					grid_.getFGrid(),
					grid_.getGGrid(),
					gridSize_);
}
const CudaStaggeredGrid & CudaSolver::getGrid()const{
	return grid_;
}
void CudaSolver::setLength(float length){
	length_ = length;
	cellSize_ = length_ / gridSize_;
	devSetCellSize(cellSize_);
}
void CudaSolver::setReynold(float reynold){
	devSetReynold(reynold);
}
void CudaSolver::setGravity(float gravityX, float gravityY){
	devSetGravity(gravityX, gravityY);
}
void CudaSolver::setDeltaTime(float deltaTime){
	devSetDeltaTime(deltaTime);
}
void CudaSolver::setOmega(){
	devSetOmega(OMEGA);
}
int CudaSolver::getGridSize(){
	return gridSize_;
}
