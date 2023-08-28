#include "Discover.h"

Discover::Discover(const MazeSimulator& maze_simulator, Mouse& mouse, MazeDiscovery& maze_discovery):
	maze_simulator(maze_simulator), mouse(mouse), maze_discovery(maze_discovery)
{
}
