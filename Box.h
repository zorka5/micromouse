#pragma once

class Box
{
public:
	bool NORTH, SOUTH, WEST, EAST;

public:
	Box(const bool& NORTH, const bool& SOUTH, const bool& WEST, const bool& EAST);

public:
	bool any() const;

	Box invert() const;
};

