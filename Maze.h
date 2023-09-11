#pragma once

#include "Constants.h"
#include "Direction.h"

#include "utils/Array2d.h"
#include "utils/Coordinates2d.h"

template<class T> using MazeArray = utils::Array2d<T, MAZE_WALL_SIZE, MAZE_WALL_SIZE>;
using MazeCoordinates = utils::Coordinates2d<MAZE_WALL_SIZE, MAZE_WALL_SIZE>;

/// <summary>
/// Helper class with maze operations
/// </summary>
class Maze {
private:
	Maze() {}

public:
	/// <summary>
	/// Method enabling calculating move in the maze
	/// </summary>
	/// <param name="position">current position</param>
	/// <param name="direction">Direction to go to</param>
	/// <returns>new coordinates</returns>
	static MazeCoordinates calculate_move(const MazeCoordinates& position, const Direction& direction);
};