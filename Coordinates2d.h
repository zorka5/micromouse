#pragma once

#include <cassert>

template<const size_t X, const size_t Y>
class Coordinates2d {
public:
	const size_t x, y;
public:
	Coordinates2d(const size_t& x, const size_t& y): x(x), y(y) {
		assert(x >= 0 && x < X);
		assert(y >= 0 && y < Y);
	};
};