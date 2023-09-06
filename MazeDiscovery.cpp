#include "MazeDiscovery.h"
#include "MazeDiscovery.h"

MazeDiscovery::MazeDiscovery():
	boxes(std::nullopt),
	end()
{
}


const MazeDiscovery::Boxes& MazeDiscovery::get_boxes() const
{
	return boxes;
}
const std::optional<MazeCoordinates>& MazeDiscovery::get_end() const
{
	return end;
}


bool MazeDiscovery::has_box(const MazeCoordinates& maze_coordinates) const
{
	return boxes.get(maze_coordinates).has_value();
}
void MazeDiscovery::add_box(const MazeCoordinates& maze_coordinates, Box&& box)
{
	auto& current = boxes.get(maze_coordinates);
	assert(!current);
	current.emplace(std::move(box));
}

void MazeDiscovery::set_end(const MazeCoordinates& maze_coordinates)
{
	assert(!end);
	end = maze_coordinates;
}
