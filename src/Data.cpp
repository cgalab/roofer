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

Data::Data() {}

Data::Data():rotateNintyLeft(CGAL::ROTATION, 1, 0) {}

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

        polygon.push_back( Point(0,0)  );
		polygon.push_back( Point(10,0) );
		polygon.push_back( Point(12,10));
		polygon.push_back( Point(9,9)  );
		polygon.push_back( Point(8,12) );
		polygon.push_back( Point(7,8)  );
		polygon.push_back( Point(5,11) );
		polygon.push_back( Point(0,9)  );


//		for(auto v_i = polygon.vertices_begin(); v_i != polygon.vertices_end(); ++v_i) {
//            auto v_j = next(v_i);
//
//            Point a(v_i->x(),v_i->y(),0);
//            Point b(v_j->x(),v_j->y(),0);
//
//            auto dir(*v_j - *v_i);
//            K::Vector_2 dirRot(-dir.y(),dir.x());
//
//
//            K::Point_2 aTrans(*v_i + dirRot);
//            Point c(aTrans.x(),aTrans.y(),dirRot.squared_length());
//
//			planes.push_back( Plane( Point(*v_i->x(),*v_,*v_j, *v_i + *v_j /* get 45° */));
//		}

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

