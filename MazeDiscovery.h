#pragma once

#include <optional>

#include "Box.h"
#include "Maze.h"

class MazeDiscovery
{
private:
	using Boxes = MazeArray<std::optional<Box>>;
	Boxes boxes;

	std::optional<MazeCoordinates> end;

public:
	MazeDiscovery();

public:
	const Boxes& get_boxes() const;
	const std::optional<MazeCoordinates>& get_end() const;

public:
	bool has_box(const MazeCoordinates& maze_coordinates) const;
	void add_box(const MazeCoordinates& maze_coordinates, Box&& box);
	
	void set_end(const MazeCoordinates& maze_coordinates);
};

