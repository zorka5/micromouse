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

/// <summary>
/// Namespace of graphical user interface
/// </summary>
namespace gui {
	
	/// <summary>
	/// Class used to graphically represent maze discovery and searching for the shortest path in the maze.
	/// </summary>
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
		/// <summary>
		/// Method that provides all necessary actions to successfully run gui. Implements initialization of graphical library and handling of events.
		/// </summary>
		void run();

	private:
		/// <summary>
		/// Method used to determine if refresh of the display is needed. 
		/// </summary>
		/// <returns></returns>
		bool tick();

		/// <summary>
		/// Method used to initialize shortest paths that later can be plotted on the maze. 
		/// </summary>
		void create_paths();

	private:
		/// <summary>
		/// Method used to draw all the graphics on display. 
		/// </summary>
		void draw();

		/// <summary>
		/// Method plotting single maze box
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze.</param>
		/// <param name="box_coord">Cooridnates of the box that will be drawn. </param>
		/// <param name="color">Filler color of the box.</param>
		void draw_box(const WindowCoordinates& offset, const MazeCoordinates& box_coord, const ALLEGRO_COLOR& color);

		/// <summary>
		/// MEthod drawing a line connecting centers of two boxes.
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze.</param>
		/// <param name="start">First box.</param>
		/// <param name="end">Second box.</param>
		/// <param name="color">Color of the line.</param>
		/// <param name="thickness">Thickness of the line</param>
		void draw_line(const WindowCoordinates& offset, const MazeCoordinates& start, const MazeCoordinates& end, const ALLEGRO_COLOR& color, float thickness);

		/// <summary>
		/// MEthod drawing complete maze simulator.
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze.</param>
		void draw_maze_simulator(const WindowCoordinates& offset);
		
		/// /// <summary>
		/// Method drawing current state of MazeDiscovery. Undiscovered boxes are drawn with different color than discovered ones. 
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze.</param>
		void draw_maze_discovery(const WindowCoordinates& offset);
		
		/// <summary>
		/// Method drawing mouse in the maze.
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze</param>
		void draw_mouse(const WindowCoordinates& offset);

		/// <summary>
		/// Method drawing the current path from start to Mouse when discovering the maze.
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze</param>
		void draw_discovery_path(const WindowCoordinates& offset);

		/// <summary>
		/// Method enabling drawing solved path from start to finish.
		/// </summary>
		/// <param name="offset">Coordinates of top left corner of the maze</param>
		/// <param name="color">Color of the line</param>
		void draw_solved_path(const WindowCoordinates& offset, const ALLEGRO_COLOR& color);
	};

}