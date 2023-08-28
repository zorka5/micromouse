#pragma once

#include "../MazeSimulator.h"


class App
{
private:
	const MazeSimulator& maze_simulator;

public:
	App(const MazeSimulator & maze_simulator);

public:
	void run();

private:
	void draw_maze_similator();
};

