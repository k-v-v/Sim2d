/*
 * CPUSolver.cpp
 *
 *  Created on: 20 Jan 2018
 *      Author: konstantin
 */

#include "CPUSolver.h"
#include <cassert>
#include <cmath>
#include <utility>
#include <iostream>


CPUSolver::CPUSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length,const BoundaryConditions& conditions)
			:Solver(gridSize, reynold, deltaTime, gravityX, gravityY, length, conditions),grid_(gridSize) {

}

CPUSolver::~CPUSolver() {
}

void CPUSolver::step(){
	std::cout<<"Base cpu solver step(this shouldn't be called ever)"<<std::endl;
	//Nothing should be here
}
void CPUSolver::setGravity(float gravityX, float gravityY){
	gravityX_ = gravityX;
	gravityY_ = gravityY;
}

void CPUSolver::setReynold(float reynold){
	reynold_ = reynold;
}
void CPUSolver::setLength(float length){
	length_= length;
	cellSize_ = length / gridSize_;
}
void CPUSolver::setDeltaTime(float deltaTime){
	deltaTime_ = deltaTime;
}
float CPUSolver::computeF(int x, int y)const{
	const auto & uGrid = grid_.getUGrid();
	auto & vGrid = grid_.getVGrid();

	//The values should stay in cache so it won't make a difference if we have a local copy or not
	///TODO:REWRITE WITH LOCAL COPIES
	float duux = (1.0f/cellSize_)*
			(
					std::pow(((uGrid(x, y) + uGrid(x+1, y) )/ 2.0f), 2)
					-
					std::pow(((uGrid(x-1, y) + uGrid(x, y)) / 2.0f), 2)
			)
			+
			GAMMA*(1.0f/cellSize_)*
			(
					std::abs((uGrid(x, y)+uGrid(x+1, y))/2.0f)*((uGrid(x, y)-uGrid(x+1,y))/2.0f)
					-
					std::abs((uGrid(x-1, y)+uGrid(x, y))/2.0f)*((uGrid(x -1, y)-uGrid(x,y))/2.0f));


	float duvy = (1.0f/cellSize_)*
			(
					((vGrid(x, y)+vGrid(x+1, y))/2.0f)*((uGrid(x, y)+uGrid(x,y+1))/2.0f)
					-
					((vGrid(x, y -1)+vGrid(x+1, y-1))/2.0f)*((uGrid(x, y -1)+uGrid(x,y))/2.0f)
			)
			+
			GAMMA*(1.0f/cellSize_)*
			(
					std::abs((vGrid(x, y)+vGrid(x+1, y))/2.0f)*((uGrid(x, y)-uGrid(x,y+1))/2.0f)
					-
					std::abs((vGrid(x, y-1)+vGrid(x+1, y-1))/2.0f)*((uGrid(x, y-1)-uGrid(x,y))/2.0f)
			);

	float duuxx = 	(
					uGrid(x+1, y) - 2*uGrid(x, y) + uGrid(x-1, y)
					)
					/ std::pow(cellSize_, 2);
	float duuyy =	(
					uGrid(x, y+1) - 2*uGrid(x, y) + uGrid(x, y-1)
					)
					/ std::pow(cellSize_, 2);

	float F = uGrid(x, y) + deltaTime_*((1.0f/reynold_)*(duuxx+duuyy) - duux - duvy + gravityX_);
	return F;
}

float CPUSolver::computeG(int x, int y)const{
	auto & uGrid = grid_.getUGrid();
	auto & vGrid = grid_.getVGrid();

	/*
	 TODO:REWRITE WITH LOCAL COPIES
	 	 aka prefetch all the values before the computation begins
	*/

	//MY GOD wtf is this
	float duvx= (1.0f/cellSize_)*
			(
					((uGrid(x,y)+uGrid(x,y+1))/2.0f)*((vGrid(x,y)+vGrid(x+1,y))/2.0f)
					-
					((uGrid(x-1,y)+uGrid(x-1,y+1))/2.0f)*((vGrid(x-1,y )+vGrid(x,y))/2.0f)
			)
			+
			GAMMA*(1.0f/cellSize_)*
			(
					std::abs((uGrid(x,y)+uGrid(x,y+1))/2.0f)*((vGrid(x,y)-vGrid(x+1,y))/2.0f)
					-
					std::abs((uGrid(x-1,y)+uGrid(x-1,y+1))/2.0f)*((vGrid(x-1,y)-vGrid(x,y))/2.0f)
			);

	float dvvy=(1.0f/cellSize_)*
			(
					std::pow(((vGrid(x,y)+vGrid(x,y+1))/2.0f),2)
					-
					std::pow(((vGrid(x,y-1)+vGrid(x,y))/2.0f),2)
			)
			+
			GAMMA*(1.0f/cellSize_)*
			(
					std::abs((vGrid(x,y)+vGrid(x,y+1))/2.0f)*((vGrid(x,y)-vGrid(x,y+1))/2.0f)
					-
					std::abs((vGrid(x,y-1)+vGrid(x,y))/2.0f)*((vGrid(x,y-1)-vGrid(x,y))/2.0f)
			);

	float dvvxx = 	(
					vGrid(x+1, y) - 2*vGrid(x, y) + vGrid(x-1, y)
					)
					/ std::pow(cellSize_, 2);
	float dvvyy = 	(
					vGrid(x, y+1) - 2*vGrid(x, y) + vGrid(x, y-1)
					)
					/ std::pow(cellSize_, 2);


	float G = vGrid(x, y) + deltaTime_*((1.0f/reynold_)*(dvvxx+dvvyy) - dvvy - duvx + gravityY_);

	return G;
}
float CPUSolver::computeRHS(int x, int y)const{
	auto & fg = grid_.getFGrid();
	auto & gg = grid_.getGGrid();

	float rhs = (
					(fg(x,y)-fg(x-1,y))
					/
					cellSize_
					+
					(gg(x,y)-gg(x,y-1))
					/
					cellSize_)
				/deltaTime_;
	return rhs;
}
float CPUSolver::computeU(int x, int y, float F)const{
	auto & pGrid = grid_.PressureGrid_;
	auto & fGrid = grid_.FGrid_;

	float u = fGrid(x, y)
			-
			(deltaTime_/cellSize_)
				*
			(pGrid(x+1,y) - pGrid(x,y));

	return u;
}

float CPUSolver::computeV(int x, int y, float G)const{
	auto & pGrid = grid_.PressureGrid_;
	auto & gGrid = grid_.FGrid_;

	float v = gGrid(x, y)
			-
			(deltaTime_/cellSize_)
				*
			(pGrid(x,y+1) - pGrid(x,y));

	return v;
}


void CPUSolver::reset(float reynold, float deltaTime, float length){
	grid_.zero();
	setReynold(reynold);
	setDeltaTime(deltaTime);
	setLength(length);

}
bool CPUSolver::checkStabilityConditions(){
	return (2.0f * deltaTime_)/reynold_ < 1.0f/(2/(cellSize_*cellSize_));
}

StaggeredGrid &CPUSolver::getGrid(){
	return grid_;
}
int CPUSolver::getGridSize(){
	return gridSize_;
}

void CPUSolver::increasePressure(int x, int y, float pressure){
	auto & pg = grid_.getPressureGrid();
	pg(x,y) +=pressure;
}
void CPUSolver::increaseVelocity(int x, int y, float uComponent, float vComponent){
	auto & ug = grid_.getUGrid();
	auto & vg = grid_.getVGrid();

	ug(x, y) +=uComponent;
	vg(x, y) +=vComponent;

}

