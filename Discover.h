#pragma once

#include "MazeDiscovery.h"
#include "MazeSimulator.h"
#include "Mouse.h"


class Discover
{
private:
	const MazeSimulator& maze_simulator;
	
	Mouse& mouse;
	MazeDiscovery& maze_discovery;

public:
	Discover(
		const MazeSimulator& maze_simulator,
		Mouse& mouse,
		MazeDiscovery& maze_discovery
	);

private:
	// std::unordered_set<Coords2d> visisted;

public:
	bool think();
};

