/*
 * SingleCoreSolver.cpp
 *
 *  Created on: 10 Jan 2018
 *      Author: konstantin
 */

#include "SingleCoreSolver.h"
#include "CPUSolver.h"
#include <iostream>
#include <cmath>

SingleCoreSolver::SingleCoreSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions)
	:CPUSolver(gridSize, reynold, deltaTime,  gravityX, gravityY, length, conditions){
}
void SingleCoreSolver::step(){

	computeFGGrid();
	applyBoundaryConditions();
	computeRHSGrid();
	//Do SOR iteration

	for(int it=0; it <SOR_ITER_MAX; it++){
		float residualSum = doSorStep();
		//TODO find best residual sum values
		//if(residualSum < ???) break;
	}

	//SOR
	//Now compute new velocity values
	computeUVGrid();

}
void SingleCoreSolver::applyBoundaryConditions(){
	auto &uGrid = grid_.getUGrid();
	auto &vGrid = grid_.getVGrid();
	//TODO: add option to apply other conditions as well
	//apply boundary conditions
	//Also TODO: OPTIMIZE THIS we are calculating the index every time we are assigning over stuff (maybe use iterators)
	for(int y = 0; y <= gridSize_ + 1; y += gridSize_ + 1){
		for(int x = 0; x < gridSize_ + 2; x++){
			uGrid(x, y) = 0;
			vGrid(x, y) = 0;
		}
	}
	for(int x = 0; x <= gridSize_ + 1; x += gridSize_ + 1){
		for(int y = 0; y < gridSize_ + 2; y++){
			uGrid(x, y) = 0;
			vGrid(x, y) = 0;
		}
	}
}
void SingleCoreSolver::computeFGGrid(){
	auto &fGrid = grid_.getFGrid();
	auto &gGrid = grid_.getGGrid();
	auto &uGrid = grid_.getUGrid();
	auto &vGrid = grid_.getVGrid();

	//Copy velocity values to FG
	//South  and north boundaries
	for(int y = 0; y <= gridSize_ + 1; y += gridSize_ + 1){
		for(int x = 0; x < gridSize_ + 2; x++){
			fGrid(x, y) = uGrid(x, y);
			gGrid(x, y) = vGrid(x, y);
		}
	}
	//East and west boundaries
	for(int x = 0; x <= gridSize_ + 1; x += gridSize_ + 1){
		for(int y = 0; y < gridSize_ + 2; y++){
			fGrid(x, y) = uGrid(x, y);
			gGrid(x, y) = vGrid(x, y);
		}
	}

	for(int x = 1; x< gridSize_ + 1; x++){
		for(int y = 1; y< gridSize_ + 1; y++){
			fGrid(x, y) = computeF(x, y);
			gGrid(x, y) = computeG(x, y);
		}
	}
}
void SingleCoreSolver::computeRHSGrid(){
	auto &rhsGrid = grid_.getRHSGrid();

	for(int x = 1; x< gridSize_ + 1; x++){
		for(int y = 1; y< gridSize_ + 1; y++){
			rhsGrid(x, y) = computeRHS(x, y);
		}
	}
}
std::pair<float, float> SingleCoreSolver::relax(int x, int y)const{
		auto & rh = grid_.getRHSGrid();		//Right-hand side grid
		auto & pg = grid_.getPressureGrid();	//Pressure grid

		//Prevent from using this for boundary conditions
		assert(x!=0);
		assert(y!=0);
		assert(x!=gridSize_+1);
		assert(y!=gridSize_+1);

		float cellsq = cellSize_*cellSize_;

		//Compute the relaxed pressure at point


		float pressure =
				(1-w_)*pg(x,y)
					+
				w_ * (cellsq/4.0f)*
				(
						(pg(x+1,y)+pg(x-1,y))/cellsq
						+
						(pg(x,y+1)+pg(x,y-1))/cellsq
						-
						rh(x,y)
				);

		if(pressure!=0.0f){
			//std::cout<<"pressure nan:"<<pressure<<std::endl;
		}

		//Compute the residual
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

		residual*=residual;	//Square residual

		return std::pair<float, float>(pressure, residual);
}
float SingleCoreSolver::doSorStep(){
	auto & pg = grid_.getPressureGrid();
	float residualSum = 0.0f;
	std::pair<float, float> ret;

	//Copy boundary pressure values to their neighbours
	for(int x = 0; x < gridSize_ + 2; x++){
			pg(x, 0) = pg(x, 1);
	}
	for(int y = 0; y < gridSize_ + 2; y++){
			pg(0, y) = pg(1, y);
	}

	for(int x = 0; x < gridSize_ + 2; x++){
			pg(x, gridSize_ + 1) = pg(x, gridSize_);
	}
	for(int y = 0; y < gridSize_ + 2; y++){
			pg(gridSize_ + 1, y) = pg(gridSize_ , y);
	}
	for(int x = 1; x< gridSize_ + 1; x++){
		for(int y = 1; y< gridSize_ + 1; y++){
			ret = relax(x, y);
			pg(x,y) = std::get<0>(ret);
			residualSum += std::get<1>(ret);
		}
	}
	return (residualSum/gridSize_)/gridSize_;
}
void SingleCoreSolver::computeUVGrid(){
	auto & pg = grid_.getPressureGrid();
	auto & ug = grid_.getUGrid();
	auto & vg = grid_.getVGrid();

	auto & fg = grid_.getFGrid();
	auto & gg = grid_.getGGrid();

	for(int x = 1; x< gridSize_ + 1; x++){
		for(int y = 1; y< gridSize_ + 1; y++){
			ug(x, y) = fg(x,y) -
							((deltaTime_)/cellSize_)*
							(
									pg(x+1,y)-pg(x,y)
							);
			vg(x, y) = gg(x,y) -
						((deltaTime_)/cellSize_)*
						(
								pg(x,y+1)-pg(x,y)
						);

		}
	}
}
SingleCoreSolver::~SingleCoreSolver(){
}
