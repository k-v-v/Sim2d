/*
 * Solver.cpp
 *
 *  Created on: 10 Jan 2018
 *      Author: konstantin
 */
#include "Solver.h"
#include <iostream>


Solver::Solver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions& conditions){
	reynold_ = reynold;
	deltaTime_ = deltaTime;
	gravityX_ = gravityX;
	gravityY_ = gravityY;
	length_ = length;
	gridSize_ = gridSize;
	cellSize_ = length_ / gridSize;
	conditions_ = conditions;
}
Solver::~Solver(){

}
void Solver::step(){
	std::cout<<"Base cpu solver step(this shouldn't be called ever)"<<std::endl;
	//Nothing should be here
}
void Solver::setGravity(float gravityX, float gravityY){
	gravityX_ = gravityX;
	gravityY_ = gravityY;
}
std::pair<float, float> Solver::getGravity()const{
	return std::make_pair(gravityX_, gravityY_);
}

void Solver::setReynold(float reynold){
	reynold_ = reynold;
}
float Solver::getReynold()const{
	return reynold_;
}
void Solver::setLength(float length){
	length_= length;
	cellSize_ = length / gridSize_;
}
float Solver::getLength()const{
	return length_;
}
void Solver::setDeltaTime(float deltaTime){
	deltaTime_ = deltaTime;
}
float Solver::getDeltaTime()const{
	return deltaTime_;
}
