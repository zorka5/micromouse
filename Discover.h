#pragma once

#include <forward_list>
#include <unordered_set>
#include <optional>

#include "Maze.h"
#include "Mouse.h"

class Discover
{
private:
	using Path = std::forward_list<std::pair<
		MazeCoordinates, // position before move
		Direction // direction of the move
		>>;
	using Visited = std::unordered_multiset<MazeCoordinates>;
private:
	Mouse& mouse;
	Path path;
	Visited visited;
	// std::optional<MazeCoordinates> end;

public:
	Discover(Mouse& mouse);

public:
	bool think();

private:
	std::optional<Direction> random_direction(const Box& allowed);

public:
	const Path& get_path() const;
	const Visited& get_visited() const;
};

