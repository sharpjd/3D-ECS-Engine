#include "debug.h"

#include <iostream>

void Debug::log(std::string s) {
	std::cout << "DEBUG: " << s << "\n";
}