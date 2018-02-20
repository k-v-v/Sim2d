/*
 * CudaSolverKernels.h
 *
 *  Created on: 10 Feb 2018
 *      Author: konstantin
 */
#pragma once
#define BLOCK_SIZE 16

void clear_staggered_grid_host( float * UGrid,
								float * VGrid,
								float * FGrid,
								float * GGrid,
								float * RHSGrid,
								float * PressureGrid,
								int edgeSize);

void null_boundary_host(float * grid, int innerEdgeSize);
void computeFGGrid_host(float * uGrid, float * vGrid, float * fGrid, float * gGrid, int innerEdgeSize);
void computeRHSGrid_host(float *fGrid, float *gGrid, float * rGrid, int innerEdgeSize);
void doSorStep_host(float * pGrid, float * rhsGrid, int innerEdgeSize);
void computeUV_host(float *pGrid, float *uGrid, float * vGrid, float *fGrid, float *gGrid, int innerEdgeSize);
void setGridPoint_host(float * grid, float value, int point);


//Set the omega constant for the device
void devSetOmega(float omega_);
//Set the reynold constant for the device
void devSetReynold(float reynold_);
//Set the deltaTime constant for the device
void devSetDeltaTime(float deltaTime_);
//Set the cellSize for the device
void devSetCellSize(float cellSize_);
//Set the graivityX and gravityY for the device
void devSetGravity(float gravityX_, float gravityY_);
