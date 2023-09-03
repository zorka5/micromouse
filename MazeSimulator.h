#pragma once

#include <string>

#include "Box.h"
#include "Constants.h"
#include "Maze.h"

class MazeSimulator
{
private:
	using Boxes = MazeArray<Box>;
	Boxes boxes;

private:
	MazeSimulator(Boxes&& boxes);

public:
	const Boxes& get_boxes() const;

public:
	static const char SERIALIZED_WALL = '#';
	static const char SERIALIZED_SPACE = ' ';

	static const size_t SERIALIZED_WALL_SIZE = MAZE_WALL_SIZE + 2 + MAZE_WALL_SIZE - 1;
	static const size_t SERIALIZED_SIZE = (SERIALIZED_WALL_SIZE + 1) * SERIALIZED_WALL_SIZE;

	std::string serialize() const;
	static MazeSimulator parse(const std::string& input);
};