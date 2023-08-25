#include "MazeSimulator.h"

#include <stdexcept>

#include <cassert>
#include <cstdio>

MazeSimulator::MazeSimulator(Boxes&& boxes):
    boxes(std::move(boxes))
{
    // TODO: add validation
    // - row + 1 north == row south etc
    // - all borders around are set
}

std::string MazeSimulator::serialize() const
{
    auto serialize_wall = [](const bool& value) -> char {
        return value ? SERIALIZED_WALL : SERIALIZED_SPACE;
    };
    
    char box[SERIALIZED_WALL_SIZE][SERIALIZED_WALL_SIZE] = { NULL };
    for (size_t y = 0; y < WALL_SIZE; ++y) {
        for (size_t x = 0; x < WALL_SIZE; ++x) {

            const auto& north = boxes.get(x, y).NORTH;
            const auto& south = boxes.get(x, y).SOUTH;
            const auto& west = boxes.get(x, y).WEST;
            const auto& east = boxes.get(x, y).EAST;

            // space
            box[x * 2 + 1][y * 2 + 1] = SERIALIZED_SPACE;

            // walls
            box[x * 2 + 1][y * 2] = serialize_wall(north);
            box[x * 2 + 1][y * 2 + 2] = serialize_wall(south);
            box[x * 2][y * 2 + 1] = serialize_wall(west);
            box[x * 2 + 2][y * 2 + 1] = serialize_wall(east);

        } 
    }

    //TODO: validate corners
    /*
            // corners
            box[x * 2][y * 2] = serialize_wall(north || west);
            box[x * 2][y * 2 + 2] = serialize_wall(south || west);
            box[x * 2 + 2][y * 2] = serialize_wall(north || east);
            box[x * 2 + 2][y * 2 + 2] = serialize_wall(south || east);
    */

    std::string serialized = std::string();
    serialized.reserve(SERIALIZED_SIZE);
    for (size_t y = 0; y < SERIALIZED_WALL_SIZE; ++y) {
        for (size_t x = 0; x < SERIALIZED_WALL_SIZE; ++x) {
            serialized.append(1, box[x][y]);
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
