#pragma once
/// <summary>
/// Class reperesenting element of maze
/// </summary>
class Box
{
public:
	bool NORTH, SOUTH, WEST, EAST;

public:
	Box(const bool& NORTH, const bool& SOUTH, const bool& WEST, const bool& EAST);

public:
	/// <summary>
	/// Method checking if any of the walls exist around box.
	/// </summary>
	/// <returns></returns>
	bool any() const;

	/// <summary>
	/// Method inverting the walls of the box.
	/// </summary>
	/// <returns>Inverted box</returns>
	Box invert() const;
};

