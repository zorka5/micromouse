#pragma once
enum class Direction
{
	NORTH, SOUTH, WEST, EAST
};

class DirectionHelpers {
private:
	DirectionHelpers() {}
public:
	static Direction invert(const Direction& direction);
};