#include "Maze.h"

#include <cassert>
#include <stdexcept>

MazeCoordinates Maze::calculate_move(const MazeCoordinates& position, const Direction& direction)
{
	switch (direction)
	{
	case Direction::NORTH:
		assert(position.y() >= 1);
		return MazeCoordinates(position.x(), position.y() - 1);

		break;
	case Direction::SOUTH:
		assert(position.y() <= MAZE_WALL_SIZE - 1);
		return MazeCoordinates(position.x(), position.y() + 1);

		break;
	case Direction::WEST:
		assert(position.x() >= 0);
		return MazeCoordinates(position.x() - 1, position.y());

		break;
	case Direction::EAST:
		assert(position.x() <= MAZE_WALL_SIZE - 1);
		return MazeCoordinates(position.x() + 1, position.y());

		break;
	default:
		throw std::runtime_error("unknown direction");
	}
}
