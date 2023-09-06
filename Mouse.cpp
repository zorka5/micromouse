#include "Mouse.h"

#include <stdexcept>

Mouse::Mouse(
	const MazeSimulator& maze_simulator,
	MazeDiscovery& maze_discovery,
	const MazeCoordinates& position
):
	maze_simulator(maze_simulator),
	maze_discovery(maze_discovery),
	position(position)
{
	// add initial position to discovered
	discover_position();
}

const MazeCoordinates& Mouse::get_position() const
{
	return position;
}

const Box& Mouse::get_walls() const
{
	const auto& box = maze_discovery.get_boxes().get(position);
	assert(box);
	return box.value();
}
const bool Mouse::is_end() const
{
	return maze_simulator.get_end() == position;
}

void Mouse::move(const Direction& direction)
{
	// calculate new position
	auto new_position = Maze::calculate_move(position, direction);
	
	// check if the move won't go through walls
	const auto& walls = get_walls();
	switch (direction)
	{
	case Direction::NORTH:
		assert(!walls.NORTH);
		break;
	case Direction::SOUTH:
		assert(!walls.SOUTH);
		break;
	case Direction::WEST:
		assert(!walls.WEST);
		break;
	case Direction::EAST:
		assert(!walls.EAST);
		break;
	default:
		throw std::runtime_error("unknown direction");
	}

	// move mouse to new position
	position = std::move(new_position);

	// discover maze on new position if we have not seen it yet
	discover_position();
}

void Mouse::discover_position()
{
	if (!maze_discovery.has_box(position)) {
		auto box = maze_simulator.get_boxes().get(position);
		maze_discovery.add_box(position, std::move(box));
	}
	if (is_end() && !maze_discovery.get_end()) {
		maze_discovery.set_end(position);
	}
}
