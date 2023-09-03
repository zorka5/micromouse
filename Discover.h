#pragma once

#include <stack>
#include <unordered_set>
#include <optional>

#include "Maze.h"
#include "Mouse.h"

class Discover
{
private:
	Mouse& mouse;
	std::stack<std::pair<
		MazeCoordinates, // position before move
		Direction // direction of the move
	>> path;
	std::unordered_multiset<MazeCoordinates> visited;
	// std::optional<MazeCoordinates> end;

public:
	Discover(Mouse& mouse);

public:
	bool think();

private:
	std::optional<Direction> random_direction(const Box& allowed);

public:
	//get path
	//get visited
	//get position -> mouse
};

