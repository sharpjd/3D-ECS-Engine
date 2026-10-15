#pragma once

//removing some of these will not give you red lines but throw compiler error
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class File {
public:
	static std::string load_file(const std::string& filename) {
		std::ifstream file(filename);

		if (!file.is_open()) {
			throw std::runtime_error("Could not open file: " + filename);
		}

		std::stringstream buffer;
		buffer << file.rdbuf(); // Read the entire file buffer into the stringstream
		return buffer.str();    // Convert stream to std::string
	}
};