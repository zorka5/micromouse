#pragma once

#include <memory>
#include <cassert>

template<typename T, const size_t X, const size_t Y>
class Array2d {
private:
	std::unique_ptr<T[]> inner;

public:
	Array2d() : inner(new T[X * Y]()) {}
	Array2d(const T& init) : inner(new T[X * Y](init)) {}

public:
	T& get(const size_t& x, const size_t& y) {
		assert(x >= 0 && x < X);
		assert(y >= 0 && y < Y);
		
		return inner[y * Y + x];
	}
	const T& get(const size_t& x, const size_t& y) const {
		assert(x >= 0 && x < X);
		assert(y >= 0 && y < Y);

		return inner[y * Y + x];
	}
};