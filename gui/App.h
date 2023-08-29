#pragma once

#include "../MazeSimulator.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class App
{
private:
	const MazeSimulator& maze_simulator;

public:
	App(const MazeSimulator & maze_simulator);

private:
	//ALLEGRO_BITMAP* maze_simulator_bitmap = nullptr;
	size_t MAZE_WALL_SIZE = 5;
	size_t MAZE_CELL_SIZE = 15;
	ALLEGRO_COLOR CELL_COLOR = al_map_rgb(0, 100, 0);
	ALLEGRO_COLOR WALL_COLOR = al_map_rgb(90, 0, 0);

public:
	int run();

private:
	void draw_maze_similator();
};

