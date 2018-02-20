/*
 * CPUSolver.h
 *
 *  Created on: 20 Jan 2018
 *      Author: konstantin
 */

#ifndef CPUSOLVER_H_
#define CPUSOLVER_H_

#include "Solver.h"
#include "StaggeredGrid.h"
#include "Constants.h"
#include <utility>
#include <fenv.h> //for throwing exceptions during airthmetic operations (LINUX ONLY)


/*
 * An interface for a CPU solver
 * Meant to be used as base class for SingleCoreSolver and MultiCoreSolver
 * */
/*
 * Todo make the compute methods inline
 * */
class CPUSolver: public Solver {
public:

	CPUSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions);
	~CPUSolver();
	/*!
		 * \brief Steps the system by Delta seconds
		 * \warning Excessive GridSizes can result in huge amounts of memory usage and silent crashes
	*/
	virtual void step();
	/*!
		 * \brief Sets the Solver's gravity
	*/
	
	virtual void setGravity(float gravityX, float gravityY);
	/*!
		 * \brief Sets the Solver Reynold's number
	*/
	virtual void setReynold(float reynold);
	
	/*!
		 * \brief Sets the lengths of the sides of the grid
	*/
	virtual void setLength(float length);
	/*
	 * Checks the stability conditions. Returns false if system has inappropriate constants
	 * */

	virtual bool checkStabilityConditions();

	/*!
	 	 * \brief Sets the simulation deltaTime
	*/
	virtual void setDeltaTime(float deltaTime);

	/*
	 * Gets the grid size
	 * */
	virtual int getGridSize();

	/*
	 * Resets the grid with given constants
	 * */
	virtual void reset(float reynold, float deltaTime, float length);
	/*
	 * Increases pressure at point (for visualisation)
	 * */
	virtual void increasePressure(int x, int y, float pressure);
	/*
	 * Increases velocity at point (for visualisation)
	 * */
	virtual void increaseVelocity(int x, int y, float uComponent, float vComponent);

	/*
	 * Gets the staggered grid. Mostly used for visualisation
	 * */
	StaggeredGrid &getGrid();
protected:
    StaggeredGrid  grid_;
protected:
	/*
	 * Computes F intermediate value
	 * */
	float computeF(int x, int y)const;
	/*
	 * Computes G intermediate value
	* */
	float computeG(int x, int y)const;
	/*
	 * Computes a the new horizontal velocity component U for F for cell(x, y)
	 */
	float computeU(int x, int y,float F)const;
	/*
	 * Computes a the new horizontal velocity component V from G for F for cell(x, y)
	*/
	float computeV(int x, int y, float G)const;
	/*
	 * Compute the RHS of the pressue equation for point (x,y)
	 */
	float computeRHS(int x, int y)const;
};

#endif /* CPUSOLVER_H_ */
