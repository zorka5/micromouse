#pragma once

#include <iostream>

#include <cassert>

namespace utils {
	template<const size_t X, const size_t Y>
	class Coordinates2d {
	private:
		size_t x_, y_;

	public:
		Coordinates2d(const size_t& x, const size_t& y) : x_(x), y_(y) {
			assert(x_ >= 0 && x_ < X);
			assert(y_ >= 0 && y_ < Y);
		};

	public:
		const size_t& x() const { return x_; }
		const size_t& y() const { return y_; }

	public:
		inline bool operator==(const Coordinates2d& other) const {
			return x_ == other.x_ && y_ == other.y_;
		}
		inline bool operator!=(const Coordinates2d& other) const {
			return !(*this == other);
		}
		inline bool operator<(const Coordinates2d& other) const {
			if (x_ != other.x()) return x_ < other.x();
			return y_ < other.y();
		}
	};
}

template<const size_t X, const size_t Y>
struct std::hash<utils::Coordinates2d<X, Y>> {
	size_t operator()(const utils::Coordinates2d<X, Y>& c) const noexcept {
		const auto x_hash = std::hash<size_t>{}(c.x());
		const auto y_hash = std::hash<size_t>{}(c.y());
		return x_hash ^ (y_hash << 1);
	}
};

template<const size_t X, const size_t Y>
std::ostream& operator<<(std::ostream& o, const utils::Coordinates2d<X, Y>& c)
{
	o << "(" << c.x() << ", " << c.y() << ")";
	return o;
}