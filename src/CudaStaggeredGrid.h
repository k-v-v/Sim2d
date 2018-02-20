/*
 * CudaStaggeredGrid.h
 *
 *  Created on: 11 Feb 2018
 *      Author: konstantin
 */

#ifndef CUDASTAGGEREDGRID_H_
#define CUDASTAGGEREDGRID_H_
#include "ThirdParty/glad.h"
#include <driver_types.h>

enum class GridType { U_GRID, V_GRID, F_GRID, G_GRID, PRESSURE_GRID, RHS_GRID};
/*
 * Staggered grid - A memory management class for storing pointers to GPU memory
 * Note allocates size + 2 square grid
 * Note Its meant to be used with OPENGL
 * */
class CudaStaggeredGrid {
public:
	//NOTE: Constructor allocates a grid of size+2 edge length
	//In order to accommodate for boundary conditions
	CudaStaggeredGrid(size_t size);

	virtual ~CudaStaggeredGrid();

	/*Returns device pointer to UGrid in GPU memory*/
	float *getUGrid();

	/*Returns device pointer to VGrid_ in GPU memory*/
	float *getVGrid();

	/*Returns device pointer to PressureGrid_ in GPU memory*/
	float *getPressureGrid();

	/*Returns device pointer to FGrid_ in GPU memory*/
	float *getFGrid();

	/*Returns device pointer to FGrid_ in GPU memory*/
	float *getGGrid();

	/*Returns device pointer to RHSGrid_ in GPU memory*/
	float *getRHSGrid();

	/*Returns grid size. Note this doesn't include the extra 2 cells for boundaries*/
	size_t getSize();

	//Uploads the PBO corresponding to gridType to the currently bound GL_TEXTURE_2D
	void uploadGridToBoundTexture(GridType gridType)const;
private:
	//Note doesn't keep track of the extra 2 grid cells in the edge length
	size_t gridSize_;
	float *UGrid_, *VGrid_, *PressureGrid_, *FGrid_, *GGrid_, *RHSGrid_;
	GLuint UGridPBO_, VGridPBO_, PressureGridPBO_, FGridPBO_, GGridPBO_, RHSGridPBO_;
private:
	/*Allocates a square PBO with edge length size
	  Maps it to a cuda device pointer
	 */
	void allocateMemoryAndResource(float ** devPtr, GLuint& PBO, size_t size);
};

#endif /* CUDASTAGGEREDGRID_H_ */
