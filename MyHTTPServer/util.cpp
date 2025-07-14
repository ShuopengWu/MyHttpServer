#include "util.h"
#include <iostream>

void error_if(bool condition, std::string error_message)
{
	if (condition) {
		std::cerr << error_message << std::endl;
		exit(EXIT_FAILURE);
	}
}
