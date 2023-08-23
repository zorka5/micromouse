#pragma once

#include <string>
#include <array>
#include <memory>

class MazeSimulator
{
private:
	//wewnêtrzna struktura
	static const unsigned int WallSize = 16;
	
	struct Walls {
		bool NORTH, SOUTH, WEST, EAST;
	};
	using WallsMatrix = std::array<Walls, WallSize * WallSize>;
	static unsigned int wall_matrix_offset(const unsigned int& x, const unsigned int& y);
	
	using WallsMatrixPtr = std::unique_ptr<WallsMatrix>;
	WallsMatrixPtr walls_matrix_ptr;

private:
	MazeSimulator(WallsMatrixPtr&& walls_matrix_ptr);

public:
	std::string serialize() const;
	static MazeSimulator parse(const std::string& input);
};