#pragma once

#include <optional>

#include "Array2d.h"

class MazeDiscovery
{
private:
	static const size_t WALL_SIZE = 16;

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
	void add_box(const size_t& x, const size_t& y, Box&& box);
};

