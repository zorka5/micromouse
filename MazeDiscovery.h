#pragma once

#include <optional>

#include "Array2d.h"
#include "Coordinates2d.h"
#include "Constants.h"

class MazeDiscovery
{
public:
	using Coord = Coordinates2d<WALL_SIZE, WALL_SIZE>;

private:
	struct Box {
		bool NORTH, SOUTH, WEST, EAST;
	};

	using Boxes = Array2d<std::optional<Box>, WALL_SIZE, WALL_SIZE>;
	Boxes boxes;

public:
	MazeDiscovery();

public:
	const Boxes& get_boxes() const;

public:
	void add_box(const Coord& coord, Box&& box);
};

