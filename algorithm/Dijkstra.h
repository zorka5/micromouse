#pragma once

#include "Algorithm.h"


class Dijkstra : public Algorithm
{
public:
	const char* name() const override;
	virtual std::optional<Path> solve(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	) const override;
};

