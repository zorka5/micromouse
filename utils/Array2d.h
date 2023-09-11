#pragma once

#include <vector>

#include "Coordinates2d.h"

/// <summary>
/// Overall projec utils
/// </summary>
namespace utils {
	/// <summary>
	/// 2d Array used to represent maze
	/// </summary>
	/// <typeparam name="T">Type of maze array element</typeparam>
	/// <typeparam name="X">Max X coordinate</typeparam>
	/// <typeparam name="Y">Max Y coordinate</typeparam>
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
		/// <summary>
		/// Getter of element
		/// </summary>
		/// <param name="coords">Coordinates of array element</param>
		/// <returns>Value of element at given coordinates</returns>
		T& get(const Coords& coords) {
			return inner.at(coords.y() * Y + coords.x());
		}
		
		/// <summary>
		/// Getter of element
		/// </summary>
		/// <param name="coords">Coordinates of array element</param>
		/// <returns>Value of element at given coordinates</returns>
		const T& get(const Coords& coords) const {
			return inner.at(coords.y() * Y + coords.x());
		}
	};
}