#pragma once

#include <forward_list>
#include <unordered_set>
#include <optional>

#include "Maze.h"
#include "Mouse.h"

/// <summary>
/// Class enabling searching through the maze and discovering all boxes.
/// </summary>
class Discover
{
private:
	using Path = std::forward_list<std::pair<
		MazeCoordinates, // position before move
		Direction // direction of the move
		>>;
	using Visited = std::unordered_multiset<MazeCoordinates>;
private:
	Mouse& mouse;
	Path path;
	Visited visited;
	bool completed;

public:
	Discover(Mouse& mouse);

public:
	/// <summary>
	/// Method enabling doing one step in the maze.
	/// </summary>
	void think();

	/// <summary>
	/// Helper method checking if all avalaible boxes are discovered
	/// </summary>
	/// <returns>True if all avalaible are discovered, False otherwise</returns>
	const bool& is_completed() const;

private:
	/// <summary>
	/// Method choosing random direction to go to.
	/// </summary>
	/// <param name="allowed">All alllowed directions</param>
	/// <returns>New direction or nullopt if not found</returns>
	std::optional<Direction> random_direction(const Box& allowed);

public:
	/// <summary>
	/// Method returning current path from start to current position
	/// </summary>
	/// <returns>Path</returns>
	const Path& get_path() const;

	/// <summary>
	/// Getter on visited elements
	/// </summary>
	/// <returns>Set of coordinates</returns>
	const Visited& get_visited() const;
};

