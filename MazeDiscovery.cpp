#include "MazeDiscovery.h"
#include "MazeDiscovery.h"

MazeDiscovery::MazeDiscovery(): boxes(std::nullopt)
{
}


const MazeDiscovery::Boxes& MazeDiscovery::get_boxes() const
{
	return boxes;
}


void MazeDiscovery::add_box(const Coord& coord, Box&& box)
{
	auto & current = boxes.get(coord);
	assert(!current);
	current.emplace(std::move(box));
}
