#pragma once

#include <vector>
#include <memory>

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
#include "../algorithm/Algorithm.h"

namespace gui {
	
	class App
	{
	private:
		static const size_t WINDOW_WIDTH = 1000;
		static const size_t WINDOW_HEIGHT = 500;

		using WindowCoordinates = utils::Coordinates2d<WINDOW_WIDTH, WINDOW_HEIGHT>;
		const WindowCoordinates& simulator_offset = WindowCoordinates(50, 50);
		const WindowCoordinates& discovery_offset = WindowCoordinates(500, 50);

		static const size_t TICKRATE = 5;

	private:
		const MazeSimulator& maze_simulator;
		MazeDiscovery& maze_discovery;
		Mouse& mouse;
		Discover& discover;
		const std::vector<Algorithm*>& algorithms;
		const MazeCoordinates& start;
		const MazeCoordinates& end;

	private:
		std::vector<std::optional<Algorithm::Path>> path_search_state;

	public:
		App(
			const MazeSimulator& maze_simulator,
			MazeDiscovery& maze_discovery,
			Mouse& mouse,
			Discover& discover,
			const std::vector<Algorithm*>& algorithms,
			const MazeCoordinates& start,
			const MazeCoordinates& end
		);

	public:
		void run();

	private:
		bool tick();
		void create_paths();

	private:
		void draw();

		void draw_box(const WindowCoordinates& offset, const MazeCoordinates& box_coord, const ALLEGRO_COLOR& color);
		void draw_line(const WindowCoordinates& offset, const MazeCoordinates& start, const MazeCoordinates& end, const ALLEGRO_COLOR& color);

		void draw_maze_simulator(const WindowCoordinates& offset);
		void draw_maze_discovery(const WindowCoordinates& offset);
		void draw_mouse(const WindowCoordinates& offset);
		//void draw_visited_count();
		void draw_discovery_path(const WindowCoordinates& offset);
		void draw_solved_path(const WindowCoordinates& offset, const ALLEGRO_COLOR& color);
	};

}