#include "MazeSimulator.h"

#include <stdexcept>
#include <optional>

#include <cassert>

MazeSimulator::MazeSimulator(
	Boxes&& boxes,
	const MazeCoordinates& start,
	const MazeCoordinates& end
) :
	boxes(std::move(boxes)),
	start(start),
	end(end)
{
	// content expected to be consistent:
	// - closed walls around
	// - consistent walls our walls right == next walls left
	// - start != end
}

const MazeSimulator::Boxes& MazeSimulator::get_boxes() const
{
	return boxes;
}
const MazeCoordinates& MazeSimulator::get_start() const
{
	return start;
}
const MazeCoordinates& MazeSimulator::get_end() const
{
	return end;
}

char MazeSimulator::serialize_serialized_wall(const SerializedWall& serialized_wall)
{
	switch (serialized_wall) {
	case SerializedWall::WALL:
		return SERIALIZED_WALL;
	case SerializedWall::SPACE:
		return SERIALIZED_SPACE;
	case SerializedWall::START:
		return SERIALIZED_START;
	case SerializedWall::END:
		return SERIALIZED_END;
	default:
		throw std::runtime_error("invalid serialized_wall found");
	};
}
MazeSimulator::SerializedWall MazeSimulator::parse_serialized_wall(const char& serialized)
{
	switch (serialized) {
	case SERIALIZED_WALL:
		return SerializedWall::WALL;
	case SERIALIZED_SPACE:
		return SerializedWall::SPACE;
	case SERIALIZED_START:
		return SerializedWall::START;
	case SERIALIZED_END:
		return SerializedWall::END;
	default:
		throw std::runtime_error("invalid character found");
	};
}

std::string MazeSimulator::serialize() const
{
	const auto make_wall = [](const bool& wall) -> SerializedWall {
		return wall ? SerializedWall::WALL : SerializedWall::SPACE;
		};

	SerializedWall maze[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE];

	// inner wall
	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto maze_coordinates = MazeCoordinates(x, y);
			const auto& box = boxes.get(maze_coordinates);

			// space
			auto space = SerializedWall::SPACE;
			if (maze_coordinates == start)
				space = SerializedWall::START;
			if (maze_coordinates == end)
				space = SerializedWall::END;
			maze[x * 2 + 1][y * 2 + 1] = space;

			// walls
			maze[x * 2 + 1][y * 2] = make_wall(box.NORTH);
			maze[x * 2 + 1][y * 2 + 2] = make_wall(box.SOUTH);
			maze[x * 2][y * 2 + 1] = make_wall(box.WEST);
			maze[x * 2 + 2][y * 2 + 1] = make_wall(box.EAST);
		}
	}

	// outer walls & corners
	for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
	{
		maze[0][y] = make_wall(true);
		maze[SERIALIZED_WALL_SIZE - 1][y] = make_wall(true);
	}
	for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
	{
		maze[x][0] = make_wall(true);
		maze[x][SERIALIZED_WALL_SIZE - 1] = make_wall(true);
	}

	// inner corners
	for (size_t y = 0; y < MAZE_WALL_SIZE - 1; ++y)
		for (size_t x = 0; x < MAZE_WALL_SIZE - 1; ++x)
		{
			const Box& us = boxes.get(MazeCoordinates(x, y));
			const Box& next = boxes.get(MazeCoordinates(x + 1, y + 1));

			maze[x * 2 + 2][y * 2 + 2] = make_wall(us.EAST || us.SOUTH || next.NORTH || next.WEST);
		}

	std::string serialized = std::string();
	serialized.reserve(SERIALIZED_SIZE);
	for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y) {
		for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x) {
			serialized.append(1, serialize_serialized_wall(maze[x][y]));
		}
		serialized.append("\n");
	}
	assert(serialized.length() == SERIALIZED_SIZE);

	return serialized;
}
MazeSimulator MazeSimulator::parse(const std::string& input)
{
	assert(input.length() == SERIALIZED_SIZE);

	SerializedWall maze[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE];
	for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
		for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
			maze[x][y] = parse_serialized_wall(input[y * (SERIALIZED_WALL_SIZE + 1) + x]);

	const auto extract_wall = [](const SerializedWall& serialized_wall) -> bool {
		switch (serialized_wall) {
		case SerializedWall::WALL:
			return true;
		case SerializedWall::SPACE:
			return false;
		default:
			throw std::runtime_error("invalid serialized_wall found");
		};
		};

	Boxes boxes(Box(false, false, false, false));
	std::optional<MazeCoordinates> start_, end_;
	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto maze_coordinates = MazeCoordinates(x, y);

			// space
			switch (maze[x * 2 + 1][y * 2 + 1])
			{
			case SerializedWall::WALL:
				throw std::runtime_error("wall in space area?");
			case SerializedWall::SPACE:
				break;
			case SerializedWall::START:
				assert(!start_);
				start_ = maze_coordinates;
				break;
			case SerializedWall::END:
				assert(!end_);
				end_ = maze_coordinates;
				break;
			}

			// walls
			const auto north = extract_wall(maze[x * 2 + 1][y * 2]);
			const auto south = extract_wall(maze[x * 2 + 1][y * 2 + 2]);
			const auto west = extract_wall(maze[x * 2][y * 2 + 1]);
			const auto east = extract_wall(maze[x * 2 + 2][y * 2 + 1]);

			// corners
			// checked later

			boxes.get(maze_coordinates) = Box(north, south, west, east);
		}
	}

	assert(start_);
	const auto& start = start_.value();

	assert(end_);
	const auto& end = end_.value();

	assert(start != end);

	// validate outer walls & corners
	for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
	{
		assert(extract_wall(maze[0][y])); // WEST
		assert(extract_wall(maze[SERIALIZED_WALL_SIZE - 1][y])); // EAST
	}
	for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
	{
		assert(extract_wall(maze[x][0])); // NORTH
		assert(extract_wall(maze[x][SERIALIZED_WALL_SIZE - 1])); // SOUTH
	}

	// validate inner corners (bottom left from us)
	for (size_t y = 0; y < MAZE_WALL_SIZE - 1; ++y)
		for (size_t x = 0; x < MAZE_WALL_SIZE - 1; ++x)
		{
			const auto& us = boxes.get(MazeCoordinates(x, y));
			const auto& next = boxes.get(MazeCoordinates(x + 1, y + 1));

			const auto corner = extract_wall(maze[x * 2 + 2][y * 2 + 2]);

			assert(corner == us.EAST || us.SOUTH || next.NORTH || next.WEST);
		}

	// validate:
	// - box is closed
	// - walls are consistent
	for (size_t y = 0; y < MAZE_WALL_SIZE; ++y) {
		for (size_t x = 0; x < MAZE_WALL_SIZE; ++x) {
			const auto& box = boxes.get(MazeCoordinates(x, y));

			if (y == 0)
				assert(box.NORTH);
			if (y > 0)
				assert(boxes.get(MazeCoordinates(x, y - 1)).SOUTH == box.NORTH);
			if (y == MAZE_WALL_SIZE - 1)
				assert(box.SOUTH);

			if (x == 0)
				assert(box.WEST);
			if (x > 0)
				assert(boxes.get(MazeCoordinates(x - 1, y)).EAST == box.WEST);
			if (x == MAZE_WALL_SIZE - 1)
				assert(box.EAST);
		}
	}

	return MazeSimulator(std::move(boxes), start, end);
}
