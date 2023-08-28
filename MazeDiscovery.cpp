#include "MazeDiscovery.h"

MazeDiscovery::MazeDiscovery():
	boxes(std::nullopt)
{
}

const MazeDiscovery::Boxes& MazeDiscovery::get_boxes() const
{
	return boxes;
}

void MazeDiscovery::add_box(const size_t& x, const size_t& y, Box&& box)
{
	auto & current = boxes.get(x, y);
	assert(!current);
	current.emplace(std::move(current));
}
