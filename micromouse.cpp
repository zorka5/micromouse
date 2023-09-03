#include <iostream>
#include <cstdlib>

#include "MazeSimulator.h"
#include "MazeDiscovery.h"
#include "Mouse.h"
#include "Discover.h"
// #include "gui/App.h"

#include "Utils.h"

void run() {
	// srand((unsigned int) time(NULL));

	const auto maze_simulator = MazeSimulator::parse(read_file_as_string("D:/Documents/Projects/micromouse/data/4.txt"));
	auto maze_discovery = MazeDiscovery();
	auto mouse = Mouse(
		maze_simulator,
		maze_discovery,
		MazeCoordinates(15,15)
	);
	auto discovery = Discover(mouse);

	size_t counter = 0;
	while (true) {
		auto exit = false;
		const auto position = mouse.get_position();
		if (!discovery.think())
			exit = true;
		const auto position_next = mouse.get_position();
		++counter;
		
		std::cout << counter << " " << position << " -> " << position_next << std::endl;
		// std::cin.get();

		if (exit)
			break;
	}


	// auto app = gui::App(maze_simulator, maze_discovery);
	// app.run();
}
int main()
{
	try {
		run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "non c++ exception handled" << std::endl;
	}
	return 0;

	/*
		MazeSimulator maze_simulator;
		MazeDisocvery maze_discovery;
		Mouse mouse {
			Walls see(direction);
			void move(direction)
		}

		MouseMover mover(maze_simulator, maze_discovery, mouse);

		while(true)
		{
			clear_all();

			draw_maze_discovery(maze_simulator);
			draw_maze_discovery(maze_discovery);
			draw_mouse(mouse);

			switch event:
				case SPACE:
					mouse.mover();
		}
	*/
}
