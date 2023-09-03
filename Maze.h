#pragma once

#include "Constants.h"
#include "Direction.h"

#include "utils/Array2d.h"
#include "utils/Coordinates2d.h"

template<class T> using MazeArray = utils::Array2d<T, MAZE_WALL_SIZE, MAZE_WALL_SIZE>;
using MazeCoordinates = utils::Coordinates2d<MAZE_WALL_SIZE, MAZE_WALL_SIZE>;

class Maze {
private:
	Maze() {}

public:
	static MazeCoordinates calculate_move(const MazeCoordinates& position, const Direction& direction);
};