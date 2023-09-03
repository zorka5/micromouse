#include "MazeDiscovery.h"
#include "MazeDiscovery.h"

MazeDiscovery::MazeDiscovery(): boxes(std::nullopt)
{
}


const MazeDiscovery::Boxes& MazeDiscovery::get_boxes() const
{
	return boxes;
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
