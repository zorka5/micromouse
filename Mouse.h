#pragma once

#include "Box.h"
#include "Direction.h"
#include "Maze.h"
#include "MazeDiscovery.h"
#include "MazeSimulator.h"

/// <summary>
/// Class representing mouse
/// </summary>
class Mouse {
private:
	const MazeSimulator& maze_simulator;
	MazeDiscovery& maze_discovery;

private:
	MazeCoordinates position;

public:
	Mouse(
		const MazeSimulator& maze_simulator,
		MazeDiscovery& maze_discovery,
		const MazeCoordinates& position
	);

public:
	/// <summary>
	/// Getter on current mouse position
	/// </summary>
	/// <returns>mouse position</returns>
	const MazeCoordinates& get_position() const;

public:
	/// <summary>
	/// Getter on walls of current mouse position
	/// </summary>
	/// <returns>Walls </returns>
	const Box& get_walls() const;

	/// <summary>
	/// Method checking if mouse is at end position
	/// </summary>
	/// <returns>true if mouse is at end, false otherwise</returns>
	const bool is_end() const;

	/// <summary>
	/// Helper method enabling mouse to move in given direction
	/// </summary>
	/// <param name="direction">Direction to go to</param>
	void move(const Direction& direction);

private:
	void discover_position();
};