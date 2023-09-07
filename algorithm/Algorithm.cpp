#include "Algorithm.h"

std::vector<MazeCoordinates> Algorithm::neighbours(const MazeDiscovery& maze_discovery, const MazeCoordinates& position)
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