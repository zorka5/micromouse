#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "../MazeSimulator.h"
#include "../MazeDiscovery.h"
#include "../Constants.h"
#include "../utils/Coordinates2d.h"
#include "../Mouse.h"
#include "../Discover.h"

namespace gui {
	
	class App
	{
	private:
		static const size_t WINDOW_WIDTH = 1000;
		static const size_t WINDOW_HEIGHT = 500;

		using WindowCoordinates = utils::Coordinates2d<WINDOW_WIDTH, WINDOW_HEIGHT>;

		static const size_t TICKRATE = 5;

	private:
		const MazeSimulator& maze_simulator;
		MazeDiscovery& maze_discovery;
		Mouse& mouse;
		Discover discovery;

	public:
		App(const MazeSimulator& maze_simulator, MazeDiscovery& maze_discovery, Mouse& mouse, Discover& discovery);


	public:
		void run();

	private:
		bool tick();

	private:
		void draw();

		void draw_maze_simulator();
		void draw_maze_discovery();
		void draw_mouse();
	};

}