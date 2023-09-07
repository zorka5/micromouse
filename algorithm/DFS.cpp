#include "DFS.h"

#include "unordered_set"
#include "stack"

const char* DFS::name() const
{
	return "DFS";
}

Algorithm::Path DFS::solve(const MazeCoordinates& start, const MazeCoordinates& end, const MazeDiscovery& maze_discovery) const
{
	auto visited = std::unordered_set<MazeCoordinates>();
	auto S = std::stack<MazeCoordinates>();
	auto path = std::vector<MazeCoordinates>();

	path.push_back(start);
	S.push(start);
	visited.insert(start);

	while (!S.empty()) {
		//take top o the stack
		const MazeCoordinates& v = S.top();
		S.pop();
		if (v == end) {
			//path found
			return path;
		}
		else {
			if (!visited.contains(v)) {
				visited.insert(v);
			}

			const auto& n = Algorithm::neighbours(maze_discovery, v);
			for (auto& neighbour_position : n) {
				if (!visited.contains(neighbour_position)) {
					S.push(neighbour_position);
					path.push_back(neighbour_position);
				}
			}
		}
	}

	throw std::runtime_error("No path found");
}
