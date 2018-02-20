/*
 * Grid.h
 *
 *  Created on: 14 Jan 2018
 *      Author: konstantin
 */

#ifndef GRID_H_
#define GRID_H_
/*!
 * \brief A square 2D grid
 *
 */
#include <algorithm>
#include <vector>
#include <cassert>

template<typename T>
class Grid {
public:
	/*!
	 * Initializes the grid with the specified grid size
	 */
	Grid(int gridSize):size_(gridSize){
		arr_.resize(gridSize * gridSize);
		std::fill(arr_.begin(), arr_.end(), 0);
	}
	/*
	 * Copy assignment
	 */
	Grid & operator=(const Grid& other){
		size_ = other.size_;
		arr_ = other.arr_;
	}
	/*
	 * Indexes the grid 2 dimensionally
	 */
	inline T& operator()(int x, int y){
		assert(x >=0 );
		assert(x < size_ );
		assert(y >=0 );
		assert(y< size_ );
		return arr_[y * size_ + x];
	}
	/*
	 * Indexes the grid 2 dimensionally
	 */
	inline const T& operator()(int x, int y)const{
		assert(x >=0 );
		assert(x < size_ );
		assert(y >=0 );
		assert(y< size_ );
		return arr_[y * size_ + x];
	}
	/*
	 * Prints the grid for debugging
	 */
	void print()const{
		//TODO:Implement this
	}
	~Grid(){
	}
	const int& size()const{
		return size_;
	}
	//Used for rendering
	inline float * getRawPointer(){
		return &arr_[0];
	}
	inline void zero(){
		std::fill(arr_.begin(), arr_.end(), 0);
	}
private:
	std::vector<T> arr_;
	int size_;
};

#endif /* GRID_H_ */
