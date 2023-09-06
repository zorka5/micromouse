#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "../MazeSimulator.h"
#include "../MazeDiscovery.h"
#include "../Constants.h"
#include "../utils/Coordinates2d.h"
#include "../Mouse.h"
#include "../Discover.h"
#include "../Direction.h"

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
		Discover& discover;

	public:
		App(
			const MazeSimulator& maze_simulator,
			MazeDiscovery& maze_discovery,
			Mouse& mouse,
			Discover& discover
		);

	public:
		void run();

	private:
		bool tick();

	private:
		void draw();

		void draw_maze_simulator();
		void draw_maze_discovery();
		void draw_mouse();
		void draw_visited_count();
		void draw_path();
	};

}