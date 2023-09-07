#pragma once

#include <vector>

#include "Maze.h"
#include "MazeDiscovery.h"

class Solver
{
public:
	using Path = std::vector<MazeCoordinates>;
private:
	static std::vector<MazeCoordinates> neighbours(const MazeDiscovery& maze_discovery, const MazeCoordinates& position);
public:
	static Path solve_dfs(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	);
	static Path solve_dijkstra(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	);

};

