/*
 * Data.cpp
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#include "Data.h"

//#include "ImportOBJ.h"

// TEST

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/bounding_box.h>


Data::Data() {}

//Data():rotateNintyLeft(CGAL::ROTATION, 1, 0) {}

Data::~Data() {}

bool Data::loadFile() {
	// IO::importOBJ(fileName, &polyhedron);
	// TODO: Importer for polygons.
	return true;
}

VertexIterator Data::next(VertexIterator i) {
	return (i+1 == polygon.vertices_end()) ? polygon.vertices_begin() : i+1;
}

VertexIterator Data::prev(VertexIterator i) {
	return (i == polygon.vertices_begin()) ? polygon.vertices_end()-1 : i-1;
}


bool Data::evaluateArguments(std::list<std::string> args) {
	std::string argument;
	bool fileLoaded = false;

	if(args.empty()) {
		printHelp();

		/* TODO: remove, just for testing! */
		std::cout << "Using default test polygon." << std::endl;
		config.gui = true;

//      input.push_back( Point(0,0)  );
//		input.push_back( Point(10,0) );
//		input.push_back( Point(10,7));
//		input.push_back( Point(2,8)  );


		input.push_back( Point(0,0)  );
		input.push_back( Point(10,0) );
		input.push_back( Point(12,10));
		input.push_back( Point(9,9)  );
		input.push_back( Point(8,12) );
		input.push_back( Point(7,8)  );
		input.push_back( Point(5,11) );
		input.push_back( Point(0,9)  );
        
        for(auto i : input) polygon.push_back(i);

        bbox = CGAL::bounding_box(input.begin(),input.end());

	    return true;
        /************************************/

		return false;
	} else {
		while(!args.empty()) {
			argument = args.front();
			args.pop_front();
			if (argument == "-max") {
				config.maximize = true;
			} else if (argument == "-min") {
				config.maximize = false;
			} else if (argument == "-gui") {
				config.gui = true;
			} else {
				if(fileExists(argument)) {
					config.fileName = argument;
					fileLoaded = loadFile();
				} else {
					std::cout << argument << " no valid option or filename!" << std::endl;
				}
			}
		}
	}

	return fileLoaded;
}

bool Data::fileExists(std::string fileName) {
  struct stat buffer;
  return (stat (fileName.c_str(), &buffer) == 0);
}



void Data::printHelp() {
	std::cout << "----------------------- Bisector --- ---------------------" << std::endl;
	std::cout << "| usage: bis " << config.printOptions << "               |" << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;
}

