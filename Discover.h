#pragma once

#include <stack>
#include <unordered_set>
#include <optional>

#include "Maze.h"
#include "Mouse.h"

class Discover
{
private:
	using Stack = std::stack<std::pair<
		MazeCoordinates, // position before move
		Direction // direction of the move
		>>;
	using CoordsSet = std::unordered_multiset<MazeCoordinates>;
private:
	Mouse& mouse;
	Stack path;
	CoordsSet visited;
	// std::optional<MazeCoordinates> end;

public:
	Discover(Mouse& mouse);

public:
	bool think();

private:
	std::optional<Direction> random_direction(const Box& allowed);

public:
	Stack& get_path();
	CoordsSet& get_visited();
};

