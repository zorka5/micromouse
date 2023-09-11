#pragma once

#include "Algorithm.h"

#include <utility>
#include <unordered_set>
#include <set>
#include <map>

class BellmanFord : public Algorithm
{
private:
	const std::multimap<MazeCoordinates, MazeCoordinates> get_edges(const MazeDiscovery& maze_discovery) const;

public:
	const char* name() const override;
	virtual std::optional<Path> solve(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	) const override;

};
