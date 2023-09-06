#pragma once

#include <string>

#include "Box.h"
#include "Constants.h"
#include "Maze.h"

class MazeSimulator
{
private:
	using Boxes = MazeArray<Box>;
	const Boxes boxes;

	const MazeCoordinates start;
	const MazeCoordinates end;

private:
	MazeSimulator(
		Boxes&& boxes,
		const MazeCoordinates& start,
		const MazeCoordinates& end
	);

public:
	const Boxes& get_boxes() const;
	const MazeCoordinates& get_start() const;
	const MazeCoordinates& get_end() const;

private:
	static const char SERIALIZED_WALL = '#';
	static const char SERIALIZED_SPACE = ' ';
	static const char SERIALIZED_START = 'S';
	static const char SERIALIZED_END = 'E';
	
	enum class SerializedWall {
		WALL,
		SPACE,
		START,
		END,
	};
	static char serialize_serialized_wall(const SerializedWall& serialized_wall);
	static SerializedWall parse_serialized_wall(const char& serialized);

	static const size_t SERIALIZED_WALL_SIZE = MAZE_WALL_SIZE + 2 + MAZE_WALL_SIZE - 1;
	static const size_t SERIALIZED_SIZE = (SERIALIZED_WALL_SIZE + 1) * SERIALIZED_WALL_SIZE;

public:
	std::string serialize() const;
	static MazeSimulator parse(const std::string& input);
};