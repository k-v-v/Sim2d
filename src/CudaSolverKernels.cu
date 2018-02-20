/*
 * CudaSolverKernels.cu
 *
 *  Created on: 10 Feb 2018
 *      Author: konstantin
 */
#include "CudaSolverKernels.h"
#include "Util.h"
#include "Constants.h"
//Compute F component. X and Y and block local

#define BLOCK_SIZE 16


//Device Constants
__constant__ float w;
__constant__ float reynold;
__constant__ float deltaTime;
__constant__ float cellSize;
__constant__ float gravityX;
__constant__ float gravityY;

/*
 * Optimisation notes:
 * In computeF, computeG if we unwrap uGrid(x, y) + deltaTime*((1.0f/reynold)*(duuxx+duuyy) - duux - duvy + gravityX);
 * and calculate each element and add it. It is possible to reduce the number of registers in use.
 * Don't know if this matters.
 * */
class Grid{
public:
	__device__ Grid(float * ptr, int size){
		ptr_ = ptr;
		size_ = size;
	}
	__device__ inline float& operator()(int x, int y){
		return ptr_[y * size_ + x];
	}
	__device__ inline const float& operator()(int x, int y)const{
		return ptr_[y * size_ + x];
	}
private:
	int size_;
	float * ptr_;
};
/**
	Copies a grid from global to local memory
	Warning: Should only be called on a thread which corresponds to a poin inside the grid
    @param global Pointer to global memory
    @param local Pointer to local memory block
    @param globalSize True Size of the edge the global grid
*/
//Note gridSize + 2 = globalSize
__device__ inline float fsquare(float val){
	return val*val;
}
__device__ inline float computeF(int x, int y, const Grid& uGrid, const Grid& vGrid){
	float duux = (1.0f/cellSize)*
			(
					fsquare(((uGrid(x, y) + uGrid(x+1, y) )/ 2.0f))
					-
					fsquare(((uGrid(x-1, y) + uGrid(x, y)) / 2.0f))
			)
			+
			GAMMA*(1.0f/cellSize)*
			(
					fabsf((uGrid(x, y)+uGrid(x+1, y))/2.0f)*((uGrid(x, y)-uGrid(x+1,y))/2.0f)
					-
					fabsf((uGrid(x-1, y)+uGrid(x, y))/2.0f)*((uGrid(x -1, y)-uGrid(x,y))/2.0f));


	float duvy = (1.0f/cellSize)*
			(
					((vGrid(x, y)+vGrid(x+1, y))/2.0f)*((uGrid(x, y)+uGrid(x,y+1))/2.0f)
					-
					((vGrid(x, y -1)+vGrid(x+1, y-1))/2.0f)*((uGrid(x, y -1)+uGrid(x,y))/2.0f)
			)
			+
			GAMMA*(1.0f/cellSize)*
			(
					fabsf((vGrid(x, y)+vGrid(x+1, y))/2.0f)*((uGrid(x, y)-uGrid(x,y+1))/2.0f)
					-
					fabsf((vGrid(x, y-1)+vGrid(x+1, y-1))/2.0f)*((uGrid(x, y-1)-uGrid(x,y))/2.0f)
			);

	float duuxx = 	(
					uGrid(x+1, y) - 2*uGrid(x, y) + uGrid(x-1, y)
					)
					/ fsquare(cellSize);
	float duuyy =	(
					uGrid(x, y+1) - 2*uGrid(x, y) + uGrid(x, y-1)
					)
					/ fsquare(cellSize);

	float F = uGrid(x, y) + deltaTime*((1.0f/reynold)*(duuxx+duuyy) - duux - duvy + gravityX);
	return F;
}
__device__ inline float computeG(int x, int y, const Grid& uGrid, const Grid& vGrid ){
	float duvx= (1.0f/cellSize)*
			(
					((uGrid(x,y)+uGrid(x,y+1))/2.0f)*((vGrid(x,y)+vGrid(x+1,y))/2.0f)
					-
					((uGrid(x-1,y)+uGrid(x-1,y+1))/2.0f)*((vGrid(x-1,y )+vGrid(x,y))/2.0f)
			)
			+
			GAMMA*(1.0f/cellSize)*
			(
					fabsf((uGrid(x,y)+uGrid(x,y+1))/2.0f)*((vGrid(x,y)-vGrid(x+1,y))/2.0f)
					-
					fabsf((uGrid(x-1,y)+uGrid(x-1,y+1))/2.0f)*((vGrid(x-1,y)-vGrid(x,y))/2.0f)
			);

	float dvvy=(1.0f/cellSize)*
			(
					fsquare(((vGrid(x,y)+vGrid(x,y+1))/2.0f))
					-
					fsquare(((vGrid(x,y-1)+vGrid(x,y))/2.0f))
			)
			+
			GAMMA*(1.0f/cellSize)*
			(
					fabsf((vGrid(x,y)+vGrid(x,y+1))/2.0f)*((vGrid(x,y)-vGrid(x,y+1))/2.0f)
					-
					fabsf((vGrid(x,y-1)+vGrid(x,y))/2.0f)*((vGrid(x,y-1)-vGrid(x,y))/2.0f)
			);

	float dvvxx = 	(
					vGrid(x+1, y) - 2*vGrid(x, y) + vGrid(x-1, y)
					)
					/ fsquare(cellSize);
	float dvvyy = 	(
					vGrid(x, y+1) - 2*vGrid(x, y) + vGrid(x, y-1)
					)
					/ fsquare(cellSize);


	float G = vGrid(x, y) + deltaTime*((1.0f/reynold)*(dvvxx+dvvyy) - dvvy - duvx + gravityY);

	return G;
}
__device__ inline float computeRHS(int x, int y, const Grid &fGrid, const Grid &gGrid){
	float rhs = (
					(fGrid(x,y)-fGrid(x-1,y))
					/
					cellSize
					+
					(gGrid(x,y)-gGrid(x,y-1))
					/
					cellSize)
				/deltaTime;
	return rhs;
}
__device__ inline float computeU(int x, int y, const Grid &fGrid, const Grid &pGrid){
	float u = fGrid(x, y)
			-
			(deltaTime/cellSize)
				*
			(pGrid(x+1,y) - pGrid(x,y));

	return u;
}
__device__ inline float computeV(int x, int y, const Grid &gGrid, const Grid &pGrid){
	float v = gGrid(x, y)
			-
			(deltaTime/cellSize)
				*
			(pGrid(x,y+1) - pGrid(x,y));

	return v;
}
__device__ inline float relax(int x, int y, const Grid pGrid, const Grid &rhsGrid){
	float cellsq = cellSize*cellSize;
	return
					(1-w)*pGrid(x,y)
						+
					w * (cellsq/4.0f)*
					(
							(pGrid(x+1,y)+pGrid(x-1,y))/cellsq
							+
							(pGrid(x,y+1)+pGrid(x,y-1))/cellsq
							-
							pGrid(x,y)
					);
	/*
	float residual = (
							(pg(x+1,y) - pg(x,y))
							-
							(pg(x,y) - pg(x-1,y))
						)/(cellsq)
						+
						(	(pg(x,y+1) - pg(x,y))
							-
							(pg(x,y) - pg(x,y-1))
						)/(cellsq)
						-
						rh(x,y);
	*/
}
__device__ inline void load_local_grid(float * global, float * local, int globalEdgeSize){
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;

	//Copy Global memory to correspoding local memory inisde the work group
	//How local memory should look (assumuing blockDim.x == blockDim.y == 8
	//Local memory should be 10 x 10 grid

	local[(threadIdx.y +1) * (blockDim.x + 2) + threadIdx.x + 1] = global[(gridY + 1)* globalEdgeSize  + gridX + 1];
	//Local memory now
	// 0 0 0 0 0 0 0 0 0 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 X X X X X X X X 0
	// 0 0 0 0 0 0 0 0 0 0
	//Now copy memory from boundary conditions

	//----COPY BOUNDARY CONDITIONS
	if(threadIdx.x == 0 ){
		local[(threadIdx.y +1) * (blockDim.x + 2) ] = global[(gridY + 1)* globalEdgeSize  + gridX];
	}
	//Local memory now
	// 0 0 0 0 0 0 0 0 0 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// x X X X X X X X X 0
	// 0 0 0 0 0 0 0 0 0 0
	if(threadIdx.x == blockDim.x - 1 ){
		local[(threadIdx.y +1) * (blockDim.x + 2) + threadIdx.x + 2] = global[(gridY + 1)* globalEdgeSize  + gridX + 2];
	}
	//Local memory now
	// 0 0 0 0 0 0 0 0 0 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 0 0 0 0 0 0 0 0 0
	if(threadIdx.y == 0 ){
		local[ threadIdx.x + 1] = global[(gridY)* globalEdgeSize  + gridX + 1];
	}
	//Local memory now
	// 0 x x x x x x x x 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 0 0 0 0 0 0 0 0 0
	if(threadIdx.y == blockDim.y -1 ){
		local[(threadIdx.y + 2) * (blockDim.x + 2) + threadIdx.x + 1] = global[(gridY + 2)* globalEdgeSize  + gridX + 1];
	}
	//Local memory now
	// 0 x x x x x x x x 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 x x x x x x x x 0
	if(threadIdx.x == 1 and threadIdx.y == 1){
		local[0] = global[(gridY - 1)* globalEdgeSize  + gridX - 1];
	}
	//Local memory now
	// X x x x x x x x x 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 x x x x x x x x 0

	if(threadIdx.x == blockDim.x-2 and threadIdx.y == blockDim.y-2){
		local[(blockDim.y + 2) * (blockDim.x + 2) -1] = global[(gridY + 2)* globalEdgeSize  + gridX + 1];
	}
	//Local memory now
	// X x x x x x x x x 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 x x x x x x x x x

	if(threadIdx.x == 1 and threadIdx.y == blockDim.y-2){
		local[(blockDim.y + 1) * (blockDim.x + 2)] = global[(gridY + 1)* globalEdgeSize];
	}
	//Local memory now
	// X x x x x x x x x 0
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x x x x x x x x x X

	if(threadIdx.x == blockDim.x-2 and threadIdx.y == 1){
		local[blockDim.x + 1] = global[globalEdgeSize - 1];
	}
	//Local memory now
	// X x x x x x x x x x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// x X X X X X X X X x
	// 0 x x x x x x x x x
	__syncthreads();
}
__global__ void clear_staggered_grid_kernel(float * UGrid,
											float * VGrid,
											float * FGrid,
											float * GGrid,
											float * RHSGrid,
											float * PressureGrid,
											int edgeSize){

	int gridX = blockIdx.x * blockDim.x + threadIdx.x;
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;
	int location = gridY * edgeSize + gridX;
	if(location >= edgeSize*edgeSize){
		return ;
	}
	UGrid[gridY * edgeSize + gridX] = 0.0f;
	VGrid[gridY * edgeSize + gridX] = 0.0f;
	FGrid[gridY * edgeSize + gridX] = 0.0f;
	GGrid[gridY * edgeSize + gridX] = 0.0f;
	RHSGrid[gridY * edgeSize + gridX] = 0.0f;
	PressureGrid[gridY * edgeSize + gridX] = 0.0f;


}
__global__ void null_boundary_kernel(float * grid, int globalEdgeSize){
	//Note: kernel is 1 dismentional
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;
	int location;

	location = gridX;
	if(location < globalEdgeSize * globalEdgeSize)
		grid[location] = 0.0f; 							//North boundary

	location = globalEdgeSize * (globalEdgeSize - 1) + gridX;
	if(location < globalEdgeSize * globalEdgeSize)
		grid[location] = 0.0f;							//South  boundary

	location = globalEdgeSize * gridX;
	if(location < globalEdgeSize*globalEdgeSize)
		grid[location] = 0.0f; 							//West  boundary

	location = globalEdgeSize * (gridX+1) - 1;
	if(location < globalEdgeSize * globalEdgeSize)
		grid[location] = 0.0f;							//East boundary
}
/* Grid size is the size of the non edge-grid (aka 2 smaller than gridSize)
 * We are executing for innerEdgeSize x innerEdgeSize grid
 * */
__global__ void computeFGGrid_kernel(float * uGridPtr, float * vGridPtr, float * fGridPtr, float * gGridPtr, int innerEdgeSize){
	//Shared mem should be size (blockDim.x + 2)^2
	extern __shared__ float shared_mem[];

	float * sharedUGrid = (float*) shared_mem;
	float * sharedVGrid = (float*) &sharedUGrid[(blockDim.x+2)*(blockDim.x+2)];

	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;


	if(gridY > innerEdgeSize || gridX > innerEdgeSize){
		return;
	}

	load_local_grid(uGridPtr, sharedUGrid, innerEdgeSize + 2);
	load_local_grid(vGridPtr, sharedVGrid, innerEdgeSize + 2);

	Grid uGrid(sharedUGrid, blockDim.x + 2);
	Grid vGrid(sharedVGrid, blockDim.x + 2);

	int globalId = (gridY + 1)* (innerEdgeSize+2) + gridX + 1;

	//sharedUGrid and sharedVGrid should be (gridDim.x + 2)^2
	fGridPtr[globalId] = computeF( threadIdx.x + 1, threadIdx.y + 1, uGrid, vGrid);
	gGridPtr[globalId] = computeG( threadIdx.x + 1, threadIdx.y + 1, uGrid, vGrid);

}
//Copies the velocities from the edges of the ibber grid to the edges of the outer grid
__global__ void copyUVBoundaries_kernel(float *uGrid, float *vGrid, float * fGrid, float *gGrid, int globalEdgeSize){
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;
	int location;

	//North boundary
	location = gridX;
	if(location < globalEdgeSize * globalEdgeSize){
		fGrid[location] = uGrid[location + globalEdgeSize];
		gGrid[location] = vGrid[location + globalEdgeSize];
	}
	//South  boundary
	location = globalEdgeSize * (globalEdgeSize - 1) + gridX;
	if(location < globalEdgeSize*globalEdgeSize){
		fGrid[location] = uGrid[location - globalEdgeSize];
		gGrid[location] = vGrid[location - globalEdgeSize];
	}
	//West  boundary
	location = globalEdgeSize * gridX;
	if(location < globalEdgeSize*globalEdgeSize){
		fGrid[location] = uGrid[location + 1];
		gGrid[location] = vGrid[location + 1];
	}
	//East boundary
	location = globalEdgeSize*(gridX+1) - 1;
	if(location < globalEdgeSize*globalEdgeSize){
		fGrid[location] = uGrid[location - 1];
		gGrid[location] = vGrid[location - 1];
	}
}
__global__ void copyPressureBoundaries(float *pGrid, int globalEdgeSize){
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;
	int location;

	//North boundary
	location = gridX;
	if(location < globalEdgeSize * globalEdgeSize){
		pGrid[location] = pGrid[location + globalEdgeSize];
		pGrid[location] = pGrid[location + globalEdgeSize];
	}
	//South  boundary
	location = globalEdgeSize * (globalEdgeSize - 1) + gridX;
	if(location < globalEdgeSize*globalEdgeSize){
		pGrid[location] = pGrid[location - globalEdgeSize];
		pGrid[location] = pGrid[location - globalEdgeSize];
	}
	//West  boundary
	location = globalEdgeSize * gridX;
	if(location < globalEdgeSize*globalEdgeSize){
		pGrid[location] = pGrid[location + 1];
		pGrid[location] = pGrid[location + 1];
	}
	//East boundary
	location = globalEdgeSize*(gridX+1) - 1;
	if(location < globalEdgeSize*globalEdgeSize){
		pGrid[location] = pGrid[location - 1];
		pGrid[location] = pGrid[location - 1];
	}
}
__global__ void computeRHSGrid_kernel(float * fGridPtr, float * gGridPtr, float * rGridPtr, int innerEdgeSize){
	extern __shared__ float shared_mem[];

	float * sharedFGrid = (float*) shared_mem;
	float * sharedGGrid = (float*) &sharedFGrid[(blockDim.x+2)*(blockDim.x+2)];

	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;


	if(gridY > innerEdgeSize || gridX > innerEdgeSize){
		return;
	}

	load_local_grid(fGridPtr, sharedFGrid, innerEdgeSize + 2);
	load_local_grid(gGridPtr, sharedGGrid, innerEdgeSize + 2);


	Grid fGrid(sharedFGrid, blockDim.x + 2);
	Grid gGrid(sharedGGrid, blockDim.x + 2);

	int globalId = (gridY + 1)* (innerEdgeSize+2) + (gridX + 1);

	//sharedUGrid and sharedVGrid should be (gridDim.x + 2)^2
	rGridPtr[globalId] = computeRHS(threadIdx.x + 1, threadIdx.y + 1, fGrid, gGrid);

}
__global__ void computeRedCells_kernel(float * pGridPtr, float * rhsGridPtr, int innerEdgeSize){
	extern __shared__ float pressure_cache[];			//For pressure grid only

	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;
	if(gridX>=innerEdgeSize || gridY >= innerEdgeSize){
		return;
	}
	load_local_grid(pGridPtr, pressure_cache, innerEdgeSize + 2);
	//First copy all the black cells in a grid with 2x size of the block
	Grid pressureGrid(pressure_cache, blockDim.x + 2);
	Grid rhsGrid(rhsGridPtr, blockDim.x + 2);


	//If even

	if(!((gridY * innerEdgeSize + gridX) & 1)){
		int globalId = (gridY + 1)* (innerEdgeSize+2) + (gridX + 1);

		pGridPtr[globalId] = relax(threadIdx.x + 1, threadIdx.y + 1, pressureGrid, rhsGrid);
	}
	//Naive implementation
}
__global__ void computeBlackCells_kernel(float * pGridPtr, float * rhsGrdiPtr, int innerEdgeSize){
	extern __shared__ float pressure_cache[];			//For pressure grid only
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;

	if(gridX>=innerEdgeSize || gridY >= innerEdgeSize){
		return;
	}
	load_local_grid(pGridPtr, pressure_cache, innerEdgeSize + 2);
	//First copy all the black cells in a grid with 2x size of the block
	Grid pressureGrid(pressure_cache, blockDim.x + 2);
	Grid rhsGrid(rhsGrdiPtr, blockDim.x + 2);

	//If even

	if(((gridY * innerEdgeSize + gridX) & 1)){
		int globalId = (gridY + 1)* (innerEdgeSize+2) + (gridX + 1);
		pGridPtr[globalId] = relax(threadIdx.x + 1, threadIdx.y + 1, pressureGrid, rhsGrid);
	}
	//Naive implementation
}
__global__ void computeUV_kernel(float *pGridPtr, float* uGridPtr, float * vGridPtr, float * fGridPtr, float * gGridPtr  , int innerEdgeSize){
	extern __shared__ float pressure_cache[];			//For pressure grid only
	int gridX = blockIdx.x * blockDim.x + threadIdx.x;	//Coordinates inside the kernel grid
	int gridY = blockIdx.y * blockDim.y + threadIdx.y;

	if(gridX>=innerEdgeSize || gridY >= innerEdgeSize){
		return;
	}


	load_local_grid(pGridPtr, pressure_cache, innerEdgeSize + 2);
	//First copy all the black cells in a grid with 2x size of the block
	Grid pGrid(pressure_cache, blockDim.x + 2);
	Grid uGrid(uGridPtr, blockDim.x + 2);
	Grid vGrid(vGridPtr, blockDim.x + 2);

	Grid fGrid(fGridPtr, blockDim.x + 2);
	Grid gGrid(gGridPtr, blockDim.x + 2);

	//No point in making separete inline functions for simple computations so add +1 for easier indexing
	//Note: pGrid is in shared memory and all the other grids are global memory
	uGrid(gridX, gridY) = fGrid(gridX, gridY) -
					((deltaTime)/cellSize)*
					(
							pGrid(threadIdx.x+1,threadIdx.y)-pGrid(threadIdx.x,threadIdx.y)
					);

	vGrid(gridX, gridY) = gGrid(gridX,gridY) -
				((deltaTime)/cellSize)*
				(
						pGrid(threadIdx.x,threadIdx.y+1)-pGrid(threadIdx.x,threadIdx.y)
				);

}
__global__ void setGridPoint_kernel(float * grid, float value, int point){
	grid[point] = value;
}
//HOST FUNCTIONS

//Note this is the true edge size
void clear_staggered_grid_host( float * UGrid,
								float * VGrid,
								float * FGrid,
								float * GGrid,
								float * RHSGrid,
								float * PressureGrid,
								int globalEdgeSize){
    dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE, 1);
    dim3 numBlocks(globalEdgeSize / threadsPerBlock.x  + 1, globalEdgeSize / threadsPerBlock.y + 1, 1);
	clear_staggered_grid_kernel<<<numBlocks, threadsPerBlock>>>(UGrid, VGrid, FGrid, GGrid, RHSGrid, PressureGrid, globalEdgeSize);

    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
}
/*Sets the edges of the grid to null*/
void null_boundary_host(float * grid, int innerEdgeSize){
    dim3 threadsPerBlock(BLOCK_SIZE * BLOCK_SIZE, 1 , 1);
    dim3 numBlocks((innerEdgeSize+2) / threadsPerBlock.x  + 1, 1, 1);
    null_boundary_kernel<<<numBlocks, threadsPerBlock>>>(grid, (innerEdgeSize+2));

    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
}
/*
 * Only runs on the cells not on the edges
 * */
void computeFGGrid_host(float * uGrid, float * vGrid, float * fGrid, float * gGrid, int innerEdgeSize){
	//Copy all the boundaries
    dim3 threadsPerBlock(BLOCK_SIZE * BLOCK_SIZE, 1 , 1);
    dim3 numBlocks((innerEdgeSize+2) / threadsPerBlock.x  + 1, 1, 1);
    copyUVBoundaries_kernel<<<numBlocks, threadsPerBlock>>>(uGrid, vGrid, fGrid, gGrid, innerEdgeSize + 2);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());

    //Reuse threadsPerBlock and numBlocks
    threadsPerBlock.x = BLOCK_SIZE;
    threadsPerBlock.y = BLOCK_SIZE;
    numBlocks.x = innerEdgeSize / threadsPerBlock.x  + 1;
    numBlocks.y = innerEdgeSize / threadsPerBlock.y + 1;

    computeFGGrid_kernel
    <<<numBlocks, threadsPerBlock, 2*(BLOCK_SIZE+2)*(BLOCK_SIZE+2)*sizeof(float)>>>(uGrid, vGrid, fGrid, gGrid, innerEdgeSize);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
}
void computeRHSGrid_host(float *fGrid, float *gGrid, float * rGrid, int innerEdgeSize){
	dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE, 1);
	dim3 numBlocks(innerEdgeSize / threadsPerBlock.x  + 1, innerEdgeSize / threadsPerBlock.y + 1, 1);

	computeRHSGrid_kernel
	<<<numBlocks, threadsPerBlock, 2*(BLOCK_SIZE+2)*(BLOCK_SIZE+2)*sizeof(float)>>>(fGrid, gGrid, rGrid, innerEdgeSize);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
    //Compute RHS
}

void doSorStep_host(float * pGrid, float * rhsGrid, int innerEdgeSize){
	//Copy the edges of the inner grid to the edges of the outergrid
	//Aka set boundary conditions
    dim3 threadsPerBlock(BLOCK_SIZE * BLOCK_SIZE, 1 , 1);
    dim3 numBlocks((innerEdgeSize+2) / threadsPerBlock.x  + 1, 1, 1);


    //__global__ void copyPressureBoundaries(float *pGrid, int globalEdgeSize){

    copyPressureBoundaries<<<numBlocks, threadsPerBlock>>>(pGrid, innerEdgeSize + 2);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());



    //Reuse threadsPerBlock and numBlocks
    threadsPerBlock.x = BLOCK_SIZE;
    threadsPerBlock.y = BLOCK_SIZE;
    numBlocks.x = innerEdgeSize / threadsPerBlock.x  + 1;
    numBlocks.y = innerEdgeSize / threadsPerBlock.y + 1;

    //Do the red checkerboard
    computeRedCells_kernel
    <<<numBlocks, threadsPerBlock, (BLOCK_SIZE+2)*(BLOCK_SIZE+2)*sizeof(float)>>>(pGrid, rhsGrid, innerEdgeSize);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());


    //Do the red checkerboard
    computeBlackCells_kernel
    <<<numBlocks, threadsPerBlock, (BLOCK_SIZE+2)*(BLOCK_SIZE+2)*sizeof(float)>>>(pGrid, rhsGrid, innerEdgeSize);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
}
void computeUV_host(float *pGrid, float *uGrid, float * vGrid, float *fGrid, float *gGrid, int innerEdgeSize){
	//__global__ void computeUV_kernel(float *pGridPtr, float* uGridPtr, float * vGridPtr, float * fGridPtr, float * gGridPtr  , int innerEdgeSize){
	dim3 threadsPerBlock(BLOCK_SIZE, BLOCK_SIZE, 1);
	dim3 numBlocks(innerEdgeSize / threadsPerBlock.x  + 1, innerEdgeSize / threadsPerBlock.y + 1, 1);

	computeUV_kernel
	<<<numBlocks, threadsPerBlock, (BLOCK_SIZE+2)*(BLOCK_SIZE+2)*sizeof(float)>>>(pGrid, uGrid, vGrid, fGrid, gGrid, innerEdgeSize);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());

}

void setGridPoint_host(float * grid, float value, int point){
	//Most inefficient way to set a single value(sadly there isn't any other way)
	//must launch a single thread kernel
	setGridPoint_kernel<<<1, 1>>>(grid, value, point);
    CUDA_CHECK_RETURN(cudaDeviceSynchronize());
    CUDA_CHECK_RETURN(cudaGetLastError());
}
void devSetOmega(float omega_){
	cudaMemcpyToSymbol(w, &omega_, sizeof(float));
}
void devSetReynold(float reynold_){
	cudaMemcpyToSymbol(reynold, &reynold_, sizeof(float));
}
void devSetDeltaTime(float deltaTime_){
	cudaMemcpyToSymbol(deltaTime, &deltaTime_, sizeof(float));
}
void devSetCellSize(float cellSize_){
	cudaMemcpyToSymbol(cellSize, &cellSize_, sizeof(float));
}
void devSetGravity(float gravityX_, float gravityY_){
	cudaMemcpyToSymbol(gravityX, &gravityX_, sizeof(float));
	cudaMemcpyToSymbol(gravityY, &gravityY_, sizeof(float));
}
