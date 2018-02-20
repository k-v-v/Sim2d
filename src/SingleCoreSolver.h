/*
 * SingleCoreSolver.h
 *
 *  Created on: 10 Jan 2018
 *      Author: konstantin
 */

#pragma once
#include "Solver.h"
#include "StaggeredGrid.h"
#include "CPUSolver.h"
/*! \class Solver
 *  \brief Runs a Navier-Stokes simulation and manages all the resources associted with it
 *
 */
class SingleCoreSolver: public CPUSolver{
public:
	SingleCoreSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions);

	/*!
	* 	\brief Steps the system by Delta seconds
	*	\warning Excessive GridSizes can result in huge amounts of memory usage and silent crashes
	*/
	virtual void step();
	/*!
	 *  \brief Returns a referrence to the internally stored grid
	 */
	const StaggeredGrid& getStaggeredGrid()const;
	~SingleCoreSolver();
private:
	std::pair<float, float> relax(int x, int y)const;
	void applyBoundaryConditions();
	void computeFGGrid();
	void computeRHSGrid();
	void computeUVGrid();
	float doSorStep();
};
