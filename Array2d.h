#pragma once

#include <memory>

#include "Coordinates2d.h"

template<typename T, const size_t X, const size_t Y>
class Array2d {
private:
	std::unique_ptr<T[]> inner;

public:
	Array2d() : inner(new T[X * Y]()) {}
	Array2d(const T& init) : inner(new T[X * Y](init)) {}

public:
	T& get(const Coordinates2d<X, Y>& coordinates){	
		return inner[coordinates.y * Y + coordinates.x];
	}
	const T& get(const Coordinates2d<X, Y>& coordinates) const {
		return inner[coordinates.y * Y + coordinates.x];
	}
};