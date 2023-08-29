#include "MazeSimulator.h"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <optional>


MazeSimulator::MazeSimulator(Boxes&& boxes):
    boxes(std::move(boxes))
{
    // boxes expected to be consistent:
    // - closed walls around
    // - consistent walls our walls right == next walls left
}

const MazeSimulator::Boxes& MazeSimulator::get_boxes() const
{
    return boxes;
}

std::string MazeSimulator::serialize() const
{
    bool maze[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE] = { false };
    
    // inner wall
    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {

            auto coord = Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y);

            const auto& north = boxes.get(coord).NORTH;
            const auto& south = boxes.get(coord).SOUTH;
            const auto& west = boxes.get(coord).WEST;
            const auto& east = boxes.get(coord).EAST;

            // space
            maze[x * 2 + 1][y * 2 + 1] = false;

            // walls
            maze[x * 2 + 1][y * 2] = north;
            maze[x * 2 + 1][y * 2 + 2] = south;
            maze[x * 2][y * 2 + 1] = west;
            maze[x * 2 + 2][y * 2 + 1] = east;

        } 
    }

    // outer walls & corners
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
    {
        maze[0][y] = true;
        maze[SERIALIZED_WALL_SIZE - 1][y] = true;
    }
    for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
    {
        maze[x][0] = true;
        maze[x][SERIALIZED_WALL_SIZE - 1] = true;
    }
    
    // inner corners
    for (size_t y = 0; y < WALL_SIZE - 1; ++y)
        for (size_t x = 0; x < WALL_SIZE - 1; ++x)
        {
            const Box& us = boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y));
            const Box& next = boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x+1, y+1));

            maze[x * 2 + 2][y * 2 + 2] = us.EAST || us.SOUTH || next.NORTH || next.WEST;
        }

    std::string serialized = std::string();
    serialized.reserve(SERIALIZED_SIZE);
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y) {
        for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x) {
            serialized.append(1, maze[x][y] ? SERIALIZED_WALL : SERIALIZED_SPACE);
        }
        serialized.append("\n");
    }
    assert(serialized.length() == SERIALIZED_SIZE);

    return serialized;
}
MazeSimulator MazeSimulator::parse(const std::string& input)
{
    assert(input.length() == SERIALIZED_SIZE);

    auto parse_wall = [&](const char& wall) -> bool {
        if (wall == SERIALIZED_WALL)
            return true;
        if (wall == SERIALIZED_SPACE)
            return false;

        throw std::runtime_error("invalid character found for ");
    };

    bool maze[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE] = { false };
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
        for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
            maze[x][y] = parse_wall(input[y * (SERIALIZED_WALL_SIZE + 1) + x]);

    Boxes boxes(Box{ false, false, false, false });

    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {
            // space
            assert(!maze[x * 2 + 1][y * 2 + 1]);

            // walls
            const auto& north = maze[x * 2 + 1][y * 2];
            const auto& south = maze[x * 2 + 1][y * 2 + 2];
            const auto& west = maze[x * 2][y *2 + 1];
            const auto& east = maze[x * 2 + 2][y * 2 + 1];

            // corners
            // checked later

            boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y)) = Box{ north, south, west, east };
        }
    }

    // validate outer walls & corners
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y)
    {
        assert(maze[0][y]); // WEST
        assert(maze[SERIALIZED_WALL_SIZE - 1][y]); // EAST
    }
    for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x)
    {
        assert(maze[x][0]); // NORTH
        assert(maze[x][SERIALIZED_WALL_SIZE - 1]); // SOUTH
    }

    // validate inner corners (bottom left from us)
    for(size_t y = 0; y < WALL_SIZE - 1; ++y)
        for (size_t x = 0; x < WALL_SIZE - 1; ++x)
        {
            const Box& us = boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y));
            const Box& next = boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x + 1, y + 1));

            const bool corner = maze[x * 2 + 2][y * 2 + 2];
            
            assert(corner == us.EAST || us.SOUTH || next.NORTH || next.WEST);
        }

    // validate:
    // - box is closed
    // - walls are consistent
    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {
            const auto& box = boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y));
            if (y == 0)
                assert(box.NORTH);
            if (y > 0)
                assert(boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x, y - 1)).SOUTH == box.NORTH);
            if (y == WALL_SIZE - 1)
                assert(box.SOUTH);

            if (x == 0)
                assert(box.WEST);
            if (x > 0)
                assert(boxes.get(Coordinates2d<WALL_SIZE, WALL_SIZE>(x - 1, y)).EAST == box.WEST);
            if (x == WALL_SIZE - 1)
                assert(box.EAST);
        }
    }

    return MazeSimulator(std::move(boxes));
}
