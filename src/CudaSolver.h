/*
 * CudaSolver.h
 *
 *  Created on: 12 Feb 2018
 *      Author: konstantin
 */

#ifndef CUDASOLVER_H_
#define CUDASOLVER_H_

#include "Solver.h"
#include "CudaStaggeredGrid.h"

class CudaSolver: public Solver {
public:
	CudaSolver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions);
	virtual ~CudaSolver();
	/*!
		 * \brief Steps the solver
	*/
	virtual void step();
	/*
	 * Resets the simulation
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
	/*!
		* \brief Sets the solver grid edge length
	*/
	virtual void setLength(float length);
	/*!
		 * \brief Sets the solver Reynold's number
	*/
	virtual void setReynold(float reynold);
	/*!
		 * \brief Sets the solver gravity
	*/
	virtual void setGravity(float gravityX, float gavityY);
	/*!
	 	 * \brief Sets the simulation deltaTime
	*/
	virtual void setDeltaTime(float deltaTime);
	/*
	 * Gets the grid size
	 * */
	virtual int getGridSize();

	/*
	 * Get the device staggered grid
	 * */
	virtual const CudaStaggeredGrid & getGrid()const;
private:
	/*
	 * Computes the F G component grid.
	 * */
	void computeFGGrid();
	/*
	 * Applies the specified boundary conditions to the edges of the outer grid
	 * */
	void applyBoundaryConditions();
	/*
	 * Computes the Right-Handside Grid
	 * */
	void computeRHSGrid();
	/*
	 * Does a single SOR step
	 * */
	void doSorStep();//TODO: How will we get the residualSum? (Map reduce ?)
	/*
	 * Computes the velocity grid using a cuda kernel
	 * */
	void computeUVGrid();
	/*
	 * Sets the omega kernel costant using the definition from Constants.h
	 * */
	void setOmega();
	CudaStaggeredGrid grid_;
};

#endif /* CUDASOLVER_H_ */
