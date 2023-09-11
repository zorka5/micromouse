#pragma once
/// <summary>
/// Class representing all directions
/// </summary>
enum class Direction
{
	NORTH, SOUTH, WEST, EAST
};

/// <summary>
/// Helper class of Direction
/// </summary>
class DirectionHelpers {
private:
	DirectionHelpers() {}
public:
	static Direction invert(const Direction& direction);
};