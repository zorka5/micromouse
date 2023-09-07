#include "Dijkstra.h"

#include <unordered_map>
#include <unordered_set>

const char* Dijkstra::name() const
{
	return "Dijkstra";
}

Algorithm::Path Dijkstra::solve(const MazeCoordinates& start, const MazeCoordinates& end, const MazeDiscovery& maze_discovery) const
{
	const auto& boxes = maze_discovery.get_boxes();

	std::unordered_map<MazeCoordinates, double> d;

	std::unordered_map<MazeCoordinates, MazeCoordinates> prev;
	std::unordered_set<MazeCoordinates> Q;

	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto& position = MazeCoordinates(x, y);
			if (!boxes.get(position).has_value()) {
				continue;
			}
			if (position == start) {
				d[position] = 0.0;
			}
			else {
				d[position] = std::numeric_limits <double>::max();
			}
			Q.insert(position);
		}
	}

	while (true)
	{
		if (Q.empty()) {
			break;
		}

		auto u_it = std::min_element(
			Q.cbegin(), Q.cend(),
			[&](const auto& a, const auto& b) {
				return d.at(a) < d.at(b);
			}
		);
		const auto u = *u_it;
		Q.erase(u_it);

		if (u == end) {
			break;
		}

		//get neighbours of u
		const auto& neighbours_of_u = Algorithm::neighbours(maze_discovery, u);
		for (auto& v : neighbours_of_u) {
			const double dist_u = d.at(u);
			const double dist_v = d.at(v);
			const double alt = dist_u + 1;
			if (dist_v > alt) {
				d.at(v) = alt;
				prev.insert_or_assign(v, u);
			}
		}
	}

	auto path = std::vector<MazeCoordinates>();
	path.push_back(end);

	MazeCoordinates current = end;
	while (current != start) {
		const auto& previous_it = prev.find(current);
		if (previous_it == prev.cend()) {
			throw std::runtime_error("Path not consistent");
		}
		const auto& previous = previous_it->second;

		path.push_back(previous);

		current = previous;
	}
	std::reverse(path.begin(), path.end());
	return path;
}
