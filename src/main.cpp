
#include <iostream>
#include <algorithm>
#include <vector>

#include <exception>


#include "Skeleton.h"



int main(int argc, char** argv) {
	static std::list<std::string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(std::string(argv[i])); }

	Skeleton skeleton(args);

	return EXIT_SUCCESS;
}

