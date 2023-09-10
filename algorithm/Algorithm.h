#pragma once
#include <optional>
#include <vector>

#include "../Maze.h"
#include "../MazeDiscovery.h"

class Algorithm {
public:
	using Path = std::vector<MazeCoordinates>;

public:
	static std::vector<MazeCoordinates> neighbours(
		const MazeDiscovery& maze_discovery,
		const MazeCoordinates& position
	);

public:
	virtual const char* name() const = 0;
	virtual std::optional<Path> solve(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	) const = 0;
};