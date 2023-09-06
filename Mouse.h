#pragma once

#include "Box.h"
#include "Direction.h"
#include "Maze.h"
#include "MazeDiscovery.h"
#include "MazeSimulator.h"

class Mouse {
private:
	const MazeSimulator& maze_simulator;
	MazeDiscovery& maze_discovery;

private:
	MazeCoordinates position;

public:
	Mouse(
		const MazeSimulator& maze_simulator,
		MazeDiscovery& maze_discovery,
		const MazeCoordinates& position
	);

public:
	const MazeCoordinates& get_position() const;

public:
	const Box& get_walls() const;
	const bool is_end() const;
	void move(const Direction& direction);

private:
	void discover_position();
};