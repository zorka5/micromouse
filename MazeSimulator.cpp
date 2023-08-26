#include "MazeSimulator.h"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cstdio>

MazeSimulator::MazeSimulator(Boxes&& boxes):
    boxes(std::move(boxes))
{
    //check if all borders are set (maze is closed)
    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {
            if (y == 0) {
                //top maze wall
                assert(this->boxes.get(x, y).NORTH);
                if (x == 0) {
                    //left top corner
                    assert(this->boxes.get(x, y).WEST);
                }
                else if (x == WALL_SIZE - 1) {
                    //right top corner
                    assert(this->boxes.get(x, y).EAST);
                }
            }
            else if (y == WALL_SIZE - 1) {
                //bottom maze wall
                assert(this->boxes.get(x, y).SOUTH);
                if (x == 0) {
                    //left bottom corner
                    assert(this->boxes.get(x, y).WEST);
                }
                else if (x == WALL_SIZE - 1) {
                    //right bottom corner
                    assert(this->boxes.get(x, y).EAST);
                }
            }
            else if (x == 0) {
                //left maze wall
                assert(this->boxes.get(x, y).WEST);
            }
            else if (x == WALL_SIZE - 1) {
                //right maze wall
                assert(this->boxes.get(x, y).EAST);
            }
            else {
                assert(this->boxes.get(x, y).NORTH == this->boxes.get(x, y - 1).SOUTH);
                assert(this->boxes.get(x, y).SOUTH == this->boxes.get(x, y + 1).NORTH);
                assert(this->boxes.get(x, y).WEST == this->boxes.get(x - 1, y).EAST);
                assert(this->boxes.get(x, y).EAST == this->boxes.get(x + 1, y).WEST);
            }
        }
    }
}

std::string MazeSimulator::serialize() const
{
    auto serialize_wall = [](const bool& value) -> char {
        return value ? SERIALIZED_WALL : SERIALIZED_SPACE;
    };
    
    char maze[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE] = { NULL };
    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {

            const auto& north = boxes.get(x, y).NORTH;
            const auto& south = boxes.get(x, y).SOUTH;
            const auto& west = boxes.get(x, y).WEST;
            const auto& east = boxes.get(x, y).EAST;

            // space
            maze[x * 2 + 1][y * 2 + 1] = SERIALIZED_SPACE;

            // walls
            maze[x * 2 + 1][y * 2] = serialize_wall(north);
            maze[x * 2 + 1][y * 2 + 2] = serialize_wall(south);
            maze[x * 2][y * 2 + 1] = serialize_wall(west);
            maze[x * 2 + 2][y * 2 + 1] = serialize_wall(east);

        } 
    }

    //corners

    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; y+=2) {
        int y_maze = y / 2; 
        for (size_t x = 0; x < SERIALIZED_WALL_SIZE; x+=2) {
            int x_maze = x / 2;
            if (y == 0) {
                if (x == 0) {
                    //left top corner
                    maze[x][y] = serialize_wall(boxes.get(x_maze, y_maze).NORTH && boxes.get(x_maze, y_maze).WEST);
                }
                else if (x == SERIALIZED_WALL_SIZE - 1) {
                    //right top corner
                    maze[x][y] = serialize_wall(boxes.get(x_maze - 1, y_maze).NORTH && boxes.get(x_maze - 1, y_maze).EAST);
                }
                else {
                    //top maze wall
                    maze[x][y] = serialize_wall(boxes.get(x_maze - 1, y_maze).NORTH && boxes.get(x_maze, y_maze).NORTH);
                }
            }
            else if (y == SERIALIZED_WALL_SIZE - 1) {
                if (x == 0) {
                    //left bottom corner
                    maze[x][y] = serialize_wall(boxes.get(x_maze, y_maze - 1).WEST && boxes.get(x_maze, y_maze - 1).SOUTH);
                }
                else if (x == SERIALIZED_WALL_SIZE - 1) {
                    //right bottom corner
                    maze[x][y] = serialize_wall(boxes.get(x_maze - 1, y_maze - 1).EAST && boxes.get(x_maze - 1, y_maze - 1).SOUTH);
                }
                else {
                    //bottom maze wall
                    maze[x][y] = serialize_wall(boxes.get(x_maze - 1 , y_maze - 1).SOUTH && boxes.get(x_maze, y_maze - 1).SOUTH); 
                }
            }
            else if (x == 0) {
                //left maze wall
                if (y != 0 && y != SERIALIZED_WALL_SIZE - 1) {
                    maze[x][y] = serialize_wall(boxes.get(x_maze, y_maze - 1).WEST && boxes.get(x_maze, y_maze).WEST);
                }

            }
            else if (x == SERIALIZED_WALL_SIZE - 1) {
                //right maze wall
                if (y != 0 && y != SERIALIZED_WALL_SIZE - 1) {
                    maze[x][y] = serialize_wall(boxes.get(x_maze - 1, y_maze - 1).EAST && boxes.get(x_maze - 1, y_maze).EAST);
                }
            }
            else{
                const auto& north = boxes.get(x_maze - 1, y_maze - 1).EAST;
                const auto& south = boxes.get(x_maze, y_maze).WEST;
                const auto& west = boxes.get(x_maze - 1, y_maze - 1).SOUTH;
                const auto& east = boxes.get(x_maze, y_maze).NORTH;

                maze[x][y] = serialize_wall((north + south + west + east) > 0);
            }
        }   
    }

    std::string serialized = std::string();
    serialized.reserve(SERIALIZED_SIZE);
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y) {
        for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x) {
            serialized.append(1, maze[x][y]);
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
    auto serialized_get = [&](const std::string& input, const size_t& x, const size_t& y) -> char {
        assert(x >= 0 && x < SERIALIZED_WALL_SIZE);
        assert(y >= 0 && y < SERIALIZED_WALL_SIZE);

        const size_t offset = y * (SERIALIZED_WALL_SIZE + 1) + x;
        
        return input[offset];
     };
    auto serialized_get_wall = [&](const std::string& input, const size_t& x, const size_t& y) -> bool {
        return parse_wall(serialized_get(input, x, y));
    };

    Boxes boxes(Box{ false, false, false, false });

    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {
            // space
            assert(serialized_get(input, x * 2 + 1, y * 2 + 1) == SERIALIZED_SPACE);

            // walls
            const auto north = serialized_get_wall(input, x * 2 + 1, y * 2);
            const auto south = serialized_get_wall(input, x * 2 + 1, y * 2 + 2);
            const auto west = serialized_get_wall(input, x * 2, y * 2 + 1);
            const auto east = serialized_get_wall(input, x * 2 + 2, y * 2 + 1);

            // corners
            // checked later

            boxes.get(x, y) = Box{ north, south, west, east };
        }
    }

    // TODO: validate corners
    /*
            assert(serialized_get_wall(input, row * 2, col * 2) == north || west);
            assert(serialized_get_wall(input, row * 2 + 2, col * 2) == south || west);
            assert(serialized_get_wall(input, row * 2, col * 2 + 2) == north || east);
            assert(serialized_get_wall(input, row * 2 + 2, col * 2 + 2) == south || east);
    */

    return MazeSimulator(std::move(boxes));
}
