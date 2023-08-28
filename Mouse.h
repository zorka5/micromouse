#pragma once

#include "MazeDiscovery.h"
#include "MazeSimulator.h"

class Mouse
{
private:
	size_t x, y;

public:
	Mouse(const size_t& x, const size_t& y);

public:
	void move(const size_t& x, const size_t& y);
};

