#include "Box.h"

Box::Box(const bool& NORTH, const bool& SOUTH, const bool& WEST, const bool& EAST):
	NORTH(NORTH), SOUTH(SOUTH), WEST(WEST), EAST(EAST)
{
}

bool Box::any() const
{
	return NORTH || SOUTH || WEST || EAST;
}

Box Box::invert() const
{
	return Box(!NORTH, !SOUTH, !WEST, !EAST);
}
