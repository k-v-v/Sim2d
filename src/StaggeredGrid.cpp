/*
 * StaggeredGrid.cpp
 *
 *  Created on: 8 Jan 2018
 *      Author: konstantin
 */

#include "StaggeredGrid.h"

StaggeredGrid::StaggeredGrid(int gridSize):	UGrid_(gridSize + 2),
											VGrid_(gridSize + 2),
											PressureGrid_(gridSize + 2),
											FGrid_(gridSize + 2),
											GGrid_(gridSize + 2),
											RHSGrid_(gridSize + 2)
											{
	size_ = gridSize;
}

StaggeredGrid::~StaggeredGrid() {
}

int StaggeredGrid::size(){
	return size_;
}
Grid<float> &StaggeredGrid::getUGrid(){
	return UGrid_;
}
Grid<float> &StaggeredGrid::getVGrid(){
	return VGrid_;
}
Grid<float> &StaggeredGrid::getPressureGrid(){
	return PressureGrid_;
}
Grid<float> &StaggeredGrid::getFGrid(){
	return FGrid_;
}
Grid<float> &StaggeredGrid::getGGrid(){
	return GGrid_;
}
Grid<float> &StaggeredGrid::getRHSGrid(){
	return RHSGrid_;
}


const Grid<float> &StaggeredGrid::getUGrid()const{
	return UGrid_;
}
const Grid<float> &StaggeredGrid::getVGrid()const{
	return VGrid_;
}
const Grid<float> &StaggeredGrid::getPressureGrid()const{
	return PressureGrid_;
}
const Grid<float> &StaggeredGrid::getFGrid()const{
	return FGrid_;
}
const Grid<float> &StaggeredGrid::getGGrid()const{
	return GGrid_;
}
const Grid<float> &StaggeredGrid::getRHSGrid()const{
	return RHSGrid_;
}

void StaggeredGrid::zero(){
	UGrid_.zero();
	VGrid_.zero();
	PressureGrid_.zero();
	FGrid_.zero();
	GGrid_.zero();
	RHSGrid_.zero();

}
