
#include <iostream>
#include <algorithm>
#include <vector>

#include <exception>


#include "Skeleton.h"



int main(int argc, char** argv) {
	std::list<std::string> args;
	for(auto i = 1; i < argc; ++i) { args.push_back(std::string(argv[i])); }

	try {
		Skel::Skeleton *skeleton = new Skel::Skeleton(args);
		delete skeleton;
	} catch(std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

