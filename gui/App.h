#pragma once

#include "../MazeSimulator.h"
#include "../Constants.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


class App
{
private:
	const MazeSimulator& maze_simulator;

public:
	App(const MazeSimulator & maze_simulator);

private:
	size_t MAZE_WALL_SIZE = 5;
	size_t MAZE_CELL_SIZE = 15;
	ALLEGRO_COLOR CELL_COLOR = al_map_rgb(255,255,255);
	ALLEGRO_COLOR WALL_COLOR = al_map_rgb(0, 0, 200);
	static const size_t WINDOW_WIDTH = 1000;
	static const size_t WINDOW_HEIGHT = 500;

public:
	int run();
private:
	void draw_maze_similator(const Coordinates2d<WINDOW_WIDTH, WINDOW_HEIGHT>& start_point);
};

