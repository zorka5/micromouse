#include "Solver.h"

#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>
#include <stdexcept>


std::vector<MazeCoordinates> Solver::neighbours(const MazeDiscovery& maze_discovery, const MazeCoordinates& position)
{
    const auto& walls_ = maze_discovery.get_boxes().get(position);
	std::vector<MazeCoordinates> neighbours;
    assert(walls_.has_value());

	const auto& walls = walls_.value();

	// we start with all directions allowed
	Box allowed(true, true, true, true);

	// remove directions that are forbidden by walls
	// remove directions that leaves the board
	if (walls.NORTH || position.y() < 1)
		allowed.NORTH = false;
	if (walls.SOUTH || position.y() >= MAZE_WALL_SIZE - 1)
		allowed.SOUTH = false;
	if (walls.WEST || position.x() < 1)
		allowed.WEST = false;
	if (walls.EAST || position.x() >= MAZE_WALL_SIZE - 1)
		allowed.EAST = false;
    
	
	if (allowed.NORTH) {
		neighbours.push_back(MazeCoordinates(position.x(), position.y() - 1));
	}
	if (allowed.SOUTH) {
		neighbours.push_back(MazeCoordinates(position.x(), position.y() + 1));
	}
	if (allowed.WEST) {
		neighbours.push_back(MazeCoordinates(position.x() - 1, position.y()));
	}
	if (allowed.EAST) {
		neighbours.push_back(MazeCoordinates(position.x() + 1, position.y()));
	}

	return neighbours;
}

Solver::Path Solver::solve_dfs(const MazeCoordinates& start, const MazeCoordinates& end, const MazeDiscovery& maze_discovery)
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
		std::cout << v << std::endl;
		if (v == end) {
			//path found
			return path;
		}
		else {
			if (!visited.contains(v)) {
				visited.insert(v);
			}
			
			const auto& n = neighbours(maze_discovery, v);
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

Solver::Path Solver::solve_dijkstra(const MazeCoordinates& start, const MazeCoordinates& end, const MazeDiscovery& maze_discovery)
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
		const auto& neighbours_of_u = neighbours(maze_discovery, u);
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