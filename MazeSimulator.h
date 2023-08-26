#pragma once

#include <string>

#include "Array2d.h"

class MazeSimulator
{
private:
	static const size_t WALL_SIZE = 16;

	struct Box {
		bool NORTH, SOUTH, WEST, EAST;
	};
	
	using Boxes = Array2d<Box, WALL_SIZE, WALL_SIZE>;
	Boxes boxes;

private:
	MazeSimulator(Boxes&& boxes);

public:
	static const char SERIALIZED_WALL = '#';
	static const char SERIALIZED_SPACE = ' ';

	static const size_t SERIALIZED_WALL_SIZE = WALL_SIZE + 2 + WALL_SIZE - 1;
	static const size_t SERIALIZED_SIZE = (SERIALIZED_WALL_SIZE + 1) * SERIALIZED_WALL_SIZE;

	std::string serialize() const;
	static MazeSimulator parse(const std::string& input);
};