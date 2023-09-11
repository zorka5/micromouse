#pragma once
#include <optional>
#include <vector>

#include "../Maze.h"
#include "../MazeDiscovery.h"

/// <summary>
/// Interface of shortest path searching algorithm
/// </summary>

class Algorithm {
public:
	using Path = std::vector<MazeCoordinates>;

public:
	/// <summary>
	/// Method for finding the neighbours of given cooridnates
	/// </summary>
	/// <param name="maze_discovery">Maze discovery</param>
	/// <param name="position">Position in the MazeDiscovery of which the neighbours are searched for</param>
	/// <returns></returns>
	static std::vector<MazeCoordinates> neighbours(
		const MazeDiscovery& maze_discovery,
		const MazeCoordinates& position
	);

public:
	/// <summary>
	/// Method enabling getting the name of the algorithm
	/// </summary>
	/// <returns>Name of the algorithm</returns>
	virtual const char* name() const = 0;

	/// <summary>
	/// Method that searches for the shortest path in the maze. 
	/// </summary>
	/// <param name="start">Starting point</param>
	/// <param name="end">End point</param>
	/// <param name="maze_discovery">MazeDiscovery used to find the path in</param>
	/// <returns>Vector of MazeCooridnates in case of success, nullopt otherwise.</returns>
	virtual std::optional<Path> solve(
		const MazeCoordinates& start,
		const MazeCoordinates& end,
		const MazeDiscovery& maze_discovery
	) const = 0;
};