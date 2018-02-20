/*
 * Solver.h
 *
 *  Created on: 10 Jan 2018
 *      Author: konstantin
 */
#pragma once
#include "Constants.h"
#include <utility>
enum class SimulationMode { SINGLE_CORE_CPU, MULTI_CORE_CPU, CUDA };
enum class BoundaryConditions {NO_SLIP, FREE_SLIP, OUTFLOW, INFLOW};

class Solver{
public:

	Solver(int gridSize, float reynold, float deltaTime, float gravityX, float gravityY, float length, const BoundaryConditions &conditions);
	~Solver();

	/*!
		 * \brief Steps the solver by Delta seconds
		 * \warning Excessive GridSizes can result in huge amounts of memory usage and silent crashes
	*/
	virtual void step()=0;
	/*!
		 * \brief Sets the solver gravity
	*/
	virtual void setGravity(float gravityX, float gavityY);
	/*!
		 * \brief Gets the solver Gravitiy constant
	*/
	virtual std::pair<float, float> getGravity()const;
	/*!
		 * \brief Sets the solver Reynold's number
	*/
	virtual void setReynold(float reynold);
	/*!
		* \brief Get the solver Reynold's number
	*/
	virtual float getReynold()const;
	/*!
		* \brief Sets the solver grid edge length
	*/
	virtual void setLength(float length);
	/*!
		* \brief Sets the solver grid edge length
	*/
	virtual float getLength()const;
	/*!
	 	 * \brief Sets the simulation deltaTime
	*/
	virtual void setDeltaTime(float deltaTime);
	/*!
	 	 * \brief Gets the simulation deltaTime
	*/
	virtual float getDeltaTime()const;
	/*
	 * Resets the simulation
	 * */
	virtual void reset(float reynold, float deltaTime, float length)=0;
	/*
	 * Increases pressure at point (for visualisation)
	 * */
	virtual void increasePressure(int x, int y, float pressure)=0;
	/*
	 * Increases velocity at point (for visualisation)
	 * */
	virtual void increaseVelocity(int x, int y, float uComponent, float vComponent)=0;
	/*
	 * Gets the grid size
	 * */
	virtual int getGridSize()=0;

protected:
    int gridSize_;
	float deltaTime_;
	const float w_ = OMEGA;
    float reynold_;
    float gravityX_, gravityY_;
    float length_, cellSize_;
	BoundaryConditions conditions_;
};

