///////////////////////////////////////////////////////////////////////////////
// File: GameOfLife3D.h
// Date: 28.02.2020
// Version: 1
// Author: Christian Steinbrecher
// Description: Functions for Game of Life
///////////////////////////////////////////////////////////////////////////////

#pragma once

using TElem = unsigned char;

size_t const N = 126;

// calculate one iteration of game of life
void GameOfLife(TElem const current[N][N][N], TElem next[N][N][N]);


// calculates if a cell with given Index is alive
TElem IsAlive_Rule2333(TElem const current[N][N][N], size_t const z, size_t const y, size_t const x);

// returns the amount of neighbours of a given cell
size_t CountNeighbours(TElem const current[N][N][N], size_t const z, size_t const y, size_t const x);



// #######+++++++ Implementation +++++++#######

#include <algorithm>
#include <cassert>
#include <limits>



void GameOfLife(TElem const current[N][N][N], TElem next[N][N][N])
{
	for (size_t z = 0; z < N; ++z)
	{
		for (size_t y = 0; y < N; ++y)
		{
			for (size_t x = 0; x < N; ++x)
			{
				next[z][y][x] = IsAlive_Rule2333(current, z, y, x);
			}
		}
	}
}


inline TElem IsAlive_Rule2333(TElem const current[N][N][N], size_t const z, size_t const y, size_t const x)
{
	assert(x < N);
	assert(y < N);
	assert(z < N);

	TElem isAlive = current[z][y][x];
	size_t const neighbours = CountNeighbours(current, z, y, x);

	if (isAlive != 0 && neighbours >= 2 && neighbours <= 6)
	{
		TElem max = TElem(-2);
		return std::min(++isAlive, max);
	}
	else
	{
		return isAlive == 0 && neighbours == 3;
	}
}


inline size_t CountNeighbours(TElem const current[N][N][N], size_t const z, size_t const y, size_t const x)
{
	assert(x < N);
	assert(y < N);
	assert(z < N);

	int const zi = static_cast<int>(z);
	int const yi = static_cast<int>(y);
	int const xi = static_cast<int>(x);
	int const Ni = N;

	size_t count = 0;

	for (int index_z = std::max(zi - 1, 0); index_z < std::min(zi + 2, Ni); ++index_z)
	{
		for (int index_y = std::max(yi - 1, 0); index_y < std::min(yi + 2, Ni); ++index_y)
		{
			for (int index_x = std::max(xi - 1, 0); index_x < std::min(xi + 2, Ni); ++index_x)
			{
				count += current[index_z][index_y][index_x] != 0;
			}
		}
	}
	
	return count;
}






