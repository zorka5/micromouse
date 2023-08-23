#include "MazeSimulator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>

unsigned int MazeSimulator::wall_matrix_offset(const unsigned int& x, const unsigned int& y)
{
    return WallSize * y + x;
}

MazeSimulator::MazeSimulator(WallsMatrixPtr&& walls_matrix_ptr):
    walls_matrix_ptr(std::move(walls_matrix_ptr))
{
}

std::string MazeSimulator::serialize() const
{
    WallsMatrix& walls_matrix = *walls_matrix_ptr;
    std::string serialized = std::string();

    std::string array[WallSize + 2 + WallSize - 1][WallSize + 2 + WallSize - 1];


    for (int i = 0; i < (WallSize); ++i) {
        for (int j = 0; j < (WallSize); ++j) {

            // walls
            array[i * 2 + 1][j * 2 + 1] = "0";
            array[i * 2][j * 2 + 1] = std::to_string(walls_matrix[wall_matrix_offset(i, j)].NORTH);
            array[i * 2 + 2][j * 2 + 1] = std::to_string(walls_matrix[wall_matrix_offset(i, j)].SOUTH);
            array[i * 2 + 1][j * 2] = std::to_string(walls_matrix[wall_matrix_offset(i, j)].WEST);
            array[i * 2 + 1][j * 2 + 2] = std::to_string(walls_matrix[wall_matrix_offset(i, j)].EAST);

            // corners
            array[i * 2][j * 2] = "c";
            array[i * 2 + 2][j * 2] = "c";
            array[i * 2][j * 2 + 2] = "c";
            array[i * 2 + 2][j * 2 + 2] = "c";

        } 
    }

    
    for (int i = 0; i < (WallSize + 2 + WallSize - 1); ++i) {
        for (int j = 0; j < (WallSize + 2 + WallSize - 1); ++j) {
            serialized += array[i][j];
            serialized += ",";
        }
        serialized += "\n\r";
    }

    return serialized;
}

MazeSimulator MazeSimulator::parse(const std::string& input)
{
    WallsMatrixPtr walls_matrix_ptr(new WallsMatrix);
    WallsMatrix& walls_matrix = *walls_matrix_ptr;

    // read file to array 
    std::ifstream file(input);
    if (!file.is_open()) {
        throw std::runtime_error("cannot open file");
    }


    std::string str;
    auto row_index = 0;

    std::string array[WallSize + 2 + WallSize - 1][WallSize + 2 + WallSize - 1];
    while (std::getline(file, str))
    {
        auto col_index = 0;
        std::stringstream ss(str);
        std::string element;
        while (std::getline(ss, element, ',')) {
            array[row_index][col_index] = element;
            std::cout << element << " ";
            col_index++;
        }
        row_index++;
        std::cout << std::endl;
    }


    int i = 0;

    for (int row_index = 0; row_index < (WallSize + 2 + WallSize - 1); ++row_index) {
        int j = 0;
        for (int col_index = 0; col_index < (WallSize + 2 + WallSize - 1); ++col_index) {              
            if (row_index % 2 == 0 || col_index % 2 == 0) {
                continue;
            }

            // sensors returning true when facing a wall (x)
            bool north = (array[row_index - 1][col_index] == "1");
            bool west = (array[row_index][col_index - 1] == "1");
            bool east = (array[row_index][col_index + 1] == "1");
            bool south = (array[row_index + 1][col_index] == "1");
            
            walls_matrix[wall_matrix_offset(i, j)] = Walls{ north, south , west, east };
            
            j++;
        }
        if (row_index % 2 == 1){
            i += 1;
        }        
    }

    return MazeSimulator(std::move(walls_matrix_ptr));
}
