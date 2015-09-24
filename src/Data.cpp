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

namespace Skel {

Data::Data() {
	/* initialize classes for 'tedgen', set switches */
}

Data::~Data() {}

void Data::loadFile(std::string fileName) {
	this->fileName = fileName;
	// IO::importOBJ(fileName, &polyhedron);
	// TODO: Importer for polygons.
}


void Data::evaluateArguments(std::list<std::string> args) {
	std::string argument;

	if(args.empty()) {
		printHelp();

		std::cout << "Using default test polygon." << std::endl;
		polygon.push_back( Point(0,0) ) ;
		polygon.push_back( Point(10,0) ) ;
		polygon.push_back( Point(12,10) ) ;
		polygon.push_back( Point(9,9) ) ;
		polygon.push_back( Point(8,12) ) ;
		polygon.push_back( Point(7,8) ) ;
		polygon.push_back( Point(5,11) ) ;
		polygon.push_back( Point(0,9) ) ;
		return;

		throw("no arguments given!");
	} else {
		while(!args.empty()) {
			argument = args.front();
			args.pop_front();
			if(argument.compare("--obj") == 0) {
				if(!args.empty()) {
					argument = args.front();
					args.pop_front();
					if(fileExists(argument)) {
						loadFile(argument);
						std::cout << "File '" << argument << "' loaded." << std::endl;
					} else {
						std::cout << "File '" << argument << "' does not seem to exist!" << std::endl;
						printHelp();
						throw("File does not exist!");
					}
				} else {
					std::cout << "No filename provided!" << std::endl;
					printHelp();
					throw("No filename provided!");
				}
			}
		}
	}
}

bool Data::fileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}



void Data::printHelp() {
	std::cout << "|---------------------- Bisecotr MAX --------------------|" << std::endl;
	std::cout << "| usage: bismax --obj <filename>                         |" << std::endl;
	std::cout << "|--------------------------------------------------------|" << std::endl;
}


} /* namespace Skel */
