/*
 * Data.cpp
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#include "Data.h"

//#include "ImportOBJ.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

Data::Data() {
	/* initialize classes for 'tedgen', set switches */
}

Data::~Data() {}

bool Data::loadFile() {
	// IO::importOBJ(fileName, &polyhedron);
	// TODO: Importer for polygons.
	return true;
}


bool Data::evaluateArguments(std::list<std::string> args) {
	std::string argument;
	bool fileLoaded = false;

	if(args.empty()) {
		printHelp();

		/* TODO: remove, just for testing! */
		std::cout << "Using default test polygon." << std::endl;
		config.gui = true;

		polygon.push_back( Vertex(0,0) ) ;
		polygon.push_back( Vertex(10,0) ) ;
		polygon.push_back( Vertex(12,10) ) ;
		polygon.push_back( Vertex(9,9) ) ;
		polygon.push_back( Vertex(8,12) ) ;
		polygon.push_back( Vertex(7,8) ) ;
		polygon.push_back( Vertex(5,11) ) ;
		polygon.push_back( Vertex(0,9) ) ;
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
	std::cout << "|---------------------- Bisector --- --------------------|" << std::endl;
	std::cout << "| usage: bis " << config.printOptions << "          |" << std::endl;
	std::cout << "|--------------------------------------------------------|" << std::endl;
}


