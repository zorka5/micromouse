#pragma once

#include <vector>

#include "Coordinates2d.h"

namespace utils {
	template<typename T, const size_t X, const size_t Y>
	class Array2d {
	public:
		using Coords = Coordinates2d<X, Y>;

	private:
		std::vector<T> inner;

	public:
		Array2d() : inner(X* Y) {}
		Array2d(const T& init) : inner(X* Y, init) {}

	public:
		T& get(const Coords& coords) {
			return inner.at(coords.y() * Y + coords.x());
		}
		const T& get(const Coords& coords) const {
			return inner.at(coords.y() * Y + coords.x());
		}
	};
}