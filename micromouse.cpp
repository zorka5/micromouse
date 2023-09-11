#include <iostream>
#include <cstdlib>
#include <vector>

#include "MazeSimulator.h"
#include "MazeDiscovery.h"
#include "Mouse.h"
#include "Discover.h"
#include "gui/App.h"
#include "algorithm/Algorithm.h"
#include "algorithm/BellmanFord.h"
#include "algorithm/Dijkstra.h"

#include "Utils.h"

void run() {
	srand((unsigned int) time(NULL));

	const auto maze_simulator = MazeSimulator::parse(read_file_as_string("D:/Documents/Projects/micromouse/data/3.txt"));
	auto maze_discovery = MazeDiscovery();
	auto mouse = Mouse(
		maze_simulator,
		maze_discovery,
		maze_simulator.get_start()
	);
	auto discover = Discover(mouse);

	auto algorithm_dijsktra = std::unique_ptr<Algorithm>(new Dijkstra());
	auto algorithm_bf = std::unique_ptr<Algorithm>(new BellmanFord());

	std::vector<Algorithm*> algorithms;
	algorithms.push_back(algorithm_dijsktra.get());
	algorithms.push_back(algorithm_bf.get());

	auto app = gui::App(maze_simulator, maze_discovery, mouse, discover, algorithms, maze_simulator.get_start(), maze_simulator.get_end());
	app.run();
}
int main()
{
	run();

	/* try {
		
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "non c++ exception handled" << std::endl;
	} */
	return 0;
}
