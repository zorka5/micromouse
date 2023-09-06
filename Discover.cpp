#include "Discover.h"

#include <array>
#include <optional>
#include <stdexcept>

Discover::Discover(Mouse& mouse) :
	mouse(mouse),
	path(),
	visited()
{
	// add initial node to visited list
	visited.emplace(mouse.get_position());
}
bool Discover::think()
{
	const auto position = mouse.get_position();

	// we start with all directions allowed
	Box allowed(true, true, true, true);

	// remove directions that are forbidden by walls
	// remove directions that leaves the board
	const auto walls = mouse.get_walls();
	if (walls.NORTH || position.y() < 1)
		allowed.NORTH = false;
	if (walls.SOUTH || position.y() >= MAZE_WALL_SIZE - 1)
		allowed.SOUTH = false;
	if (walls.WEST || position.x() < 1)
		allowed.WEST = false;
	if (walls.EAST || position.x() >= MAZE_WALL_SIZE - 1)
		allowed.EAST = false;

	// remove directions we've already visited
	if (allowed.NORTH && visited.count(Maze::calculate_move(position, Direction::NORTH)) > 0)
		allowed.NORTH = false;
	if (allowed.SOUTH && visited.count(Maze::calculate_move(position, Direction::SOUTH)) > 0)
		allowed.SOUTH = false;
	if (allowed.WEST && visited.count(Maze::calculate_move(position, Direction::WEST)) > 0)
		allowed.WEST = false;
	if (allowed.EAST && visited.count(Maze::calculate_move(position, Direction::EAST)) > 0)
		allowed.EAST = false;

	// check if we can go anywhere
	if (auto direction_ = random_direction(allowed); direction_) {
		// we have a new unvisited direction
		const auto& direction = direction_.value();

		// store historical position
		path.emplace_front(std::make_pair(
			position,
			direction
		));

		// move the mouse
		mouse.move(direction);

		// store new location as visited
		const auto position_next = mouse.get_position();
		visited.emplace(position_next);

		return true;
	}

	// there is no direction, so allowed list is empty
	// instead we try to go back on our path
	if (!path.empty())
	{
		// take path top as the last move
		const auto top_ = std::move(path.front());
		path.pop_front();
		const auto& [position_next, direction] = top_;

		// go in opposite direction
		mouse.move(DirectionHelpers::invert(direction));

		// mouse should be in previous position
		assert(position_next == mouse.get_position());

		// all positions in path are also in visited
		// for statistics we add them again
		visited.emplace(position_next);

		return true;
	}

	// there is nothing we can do
	return false;
}

std::optional<Direction> Discover::random_direction(const Box& allowed)
{
#pragma warning(push)
#pragma warning(disable: 28020)
	size_t count = 0;
	std::array<std::optional<Direction>, 4> allowed_vec;
	if (allowed.NORTH)
		allowed_vec[count++] = Direction::NORTH;
	if (allowed.SOUTH)
		allowed_vec[count++] = Direction::SOUTH;
	if (allowed.WEST)
		allowed_vec[count++] = Direction::WEST;
	if (allowed.EAST)
		allowed_vec[count++] = Direction::EAST;
#pragma warning(pop)

	if (count <= 0)
		return std::nullopt;

	const size_t index = rand() % count;

	const auto& value = allowed_vec[index];
	assert(value);
	return value.value();
}

const Discover::Path& Discover::get_path() const
{
	return path;
}
const Discover::Visited& Discover::get_visited() const
{
	return visited;
}
