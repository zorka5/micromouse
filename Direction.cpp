#include "Direction.h"

#include <stdexcept>

Direction DirectionHelpers::invert(const Direction& direction)
{
	switch (direction)
	{
	case Direction::NORTH:
		return Direction::SOUTH;
	case Direction::SOUTH:
		return Direction::NORTH;
	case Direction::EAST:
		return Direction::WEST;
	case Direction::WEST:
		return Direction::EAST;
	default:
		throw std::runtime_error("unknown direction");
	}
}
