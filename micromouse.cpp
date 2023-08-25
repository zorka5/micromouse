#include <iostream>

#include "MazeSimulator.h"

#include "Utils.h"

void run() {
	const auto input = read_file_as_string("D:/Documents/Projects/micromouse/data/2.txt");
	const auto maze = MazeSimulator::parse(input);
	std::string ser = maze.serialize();

	std::cout << ser << std::endl;
	std::cout << input << std::endl;

	assert(input == ser);
}

int main()
{
	try {
		run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "non c++ exception handled" << std::endl;
	}
	return 0;
}