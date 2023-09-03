#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "../MazeSimulator.h"
#include "../MazeDiscovery.h"
#include "../Constants.h"

namespace gui {
	
	class App
	{
	private:
		static const size_t WINDOW_WIDTH = 1000;
		static const size_t WINDOW_HEIGHT = 500;

		using Coord = Coordinates2d<WINDOW_WIDTH, WINDOW_HEIGHT>;

		static const size_t TICKRATE = 5;

	private:
		const MazeSimulator& maze_simulator;
		const MazeDiscovery& maze_discovery;

	public:
		App(const MazeSimulator& maze_simulator, const MazeDiscovery& maze_discovery);


	public:
		void run();

	private:
		bool tick();

	private:
		void draw();

		void draw_maze_simulator();
		void draw_maze_discovery();
	};

}