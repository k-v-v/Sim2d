/*
 * StaggeredGrid.h
 *
 *  Created on: 8 Jan 2018
 *      Author: konstantin
 */

#ifndef STAGGEREDGRID_H_
#define STAGGEREDGRID_H_
#include "Grid.h"
/*
 * Staggered grid
 *
 * If a cell has centre (x, y) then:
 * The U horizontal velocity component is evaluated at the midpoint of the right edge.
 * The V vertical velocity component is evaluated at the midpoint of the top edge.
 * The P pressure component is evaluated at the centre of the cell
 * FGrid, GGrid are intermediate values used for computation
 * NOTE:The grid is gridSize + 2 size to accommodate for boundary conditions. ( 0 and size-1)
 * */

//TODO: Use initializer lists instead of pointers
class StaggeredGrid {
	//To avoid getters and setters
	friend class CPUSolver;
public:
	StaggeredGrid(int gridSize);

	~StaggeredGrid();
	/*Returns the size of the staggered grid*/
	int size();
	/*Returns a modifiable reference of UGrid*/
	Grid<float> &getUGrid();

	/*Returns a modifiable reference of VGrid*/
	Grid<float> &getVGrid();

	/*Returns a modifiable reference of PressureGrid*/
	Grid<float> &getPressureGrid();

	/*Returns a modifiable reference of FGrid*/
	Grid<float> &getFGrid();

	/*Returns a modifiable reference of GGrid*/
	Grid<float> &getGGrid();

	/*Returns a modifiable reference of RHSGrid*/
	Grid<float> &getRHSGrid();

	/*Returns a constant reference of UGrid*/
	const Grid<float> &getUGrid()const;

	/*Returns a constant reference of VGrid*/
	const Grid<float> &getVGrid()const;

	/*Returns a constant reference of PressureGrid*/
	const Grid<float> &getPressureGrid()const;

	/*Returns a constant reference of FGrid*/
	const Grid<float> &getFGrid()const;

	/*Returns a constant reference of GGrid*/
	const Grid<float> &getGGrid()const;

	/*Returns a constant reference of RHSGrid*/
	const Grid<float> &getRHSGrid()const;

	/*Sets all the subgrids inside of the Staggered grid to 0*/
	void zero();
private:
	int size_;
	Grid<float> UGrid_;
	Grid<float> VGrid_;
	Grid<float> PressureGrid_;
	Grid<float> FGrid_;
	Grid<float> GGrid_;
	Grid<float> RHSGrid_;
};

#endif /* STAGGEREDGRID_H_ */
