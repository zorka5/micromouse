#pragma once

#include <optional>

#include "Box.h"
#include "Maze.h"

/// <summary>
/// Class representaing maze discovery
/// </summary>
class MazeDiscovery
{
private:
	using Boxes = MazeArray<std::optional<Box>>;
	Boxes boxes;

	std::optional<MazeCoordinates> end;

public:
	MazeDiscovery();

public:
	/// <summary>
	/// Getter on boxes in maze
	/// </summary>
	/// <returns>MazeArray of boxes</returns>
	const Boxes& get_boxes() const;

	/// <summary>
	/// Getter on end position
	/// </summary>
	/// <returns>End if exists, nullopt otherwise</returns>
	const std::optional<MazeCoordinates>& get_end() const;

public:
	/// <summary>
	/// Helper method checking if box is already discovered.
	/// </summary>
	/// <param name="maze_coordinates">Cooridnates of box being checked</param>
	/// <returns>true if exists, false otherwise</returns>
	bool has_box(const MazeCoordinates& maze_coordinates) const;

	/// <summary>
	/// Method enabling "discovering" given box.
	/// </summary>
	/// <param name="maze_coordinates">Coordinates of box</param>
	/// <param name="box">Box being discovered.</param>
	void add_box(const MazeCoordinates& maze_coordinates, Box&& box);
	
	/// <summary>
	/// Setter of end coordinate
	/// </summary>
	/// <param name="maze_coordinates">Cooridnates of new end</param>
	void set_end(const MazeCoordinates& maze_coordinates);
};

