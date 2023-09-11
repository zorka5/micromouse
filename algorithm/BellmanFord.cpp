#include "BellmanFord.h"


#include <unordered_map>



const std::multimap<MazeCoordinates, MazeCoordinates> BellmanFord::get_edges(const MazeDiscovery& maze_discovery) const
{
	auto edges = std::multimap<MazeCoordinates, MazeCoordinates>();

	const auto& boxes = maze_discovery.get_boxes();

	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto position = MazeCoordinates(x, y);
			if (!boxes.get(position).has_value()) {
				continue;
			}
			const auto neighbours = Algorithm::neighbours(maze_discovery, position);
			for (const auto& neighbour : neighbours) {

				edges.insert(std::make_pair(
					position,
					neighbour
				));
			}
		}
	}

	return edges;
}

const char* BellmanFord::name() const
{
	return "BellmanFord";
}

std::optional<Algorithm::Path> BellmanFord::solve(const MazeCoordinates& start, const MazeCoordinates& end, const MazeDiscovery& maze_discovery) const
{

	const auto& boxes = maze_discovery.get_boxes();

	std::unordered_map<MazeCoordinates, double> d;
	std::unordered_map<MazeCoordinates, MazeCoordinates> prev;
	const auto edges = get_edges(maze_discovery);

	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto position = MazeCoordinates(x, y);

			if (!boxes.get(position).has_value()) {
				continue;
			}
			d.insert(std::make_pair(
				position,
				position == start ? 0.0 : std::numeric_limits <double>::max()
			));
		}
	}

	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto v = MazeCoordinates(x, y);

			if (!boxes.get(v).has_value()) {
				continue;
			}
			for (const auto& edge : edges) {
				const auto& u = edge.first;
				const auto& v = edge.second;

				const double dist_u = d.at(u);
				const double dist_v = d.at(v);

				if (dist_v > dist_u + 1) {
					d.at(v) = dist_u + 1;
					prev.insert_or_assign(v, u);
				}
			}
		}
	}

	auto path = std::vector<MazeCoordinates>();
	path.push_back(end);

	auto current = end;
	while (current != start) {
		const auto& previous_it = prev.find(current);
		const auto& previous = previous_it->second;
		path.push_back(previous);
		current = previous;
	}

	std::reverse(path.begin(), path.end());
	
	return path;
}
