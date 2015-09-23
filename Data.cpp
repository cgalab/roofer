/*
 * Data.cpp
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#include "Data.h"

#include "ImportOBJ.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

namespace SS3D {

Data::Data() {
	/* initialize classes for 'tedgen', set switches */
	tet_in.initialize();
	tet_out.initialize();
	tet_beh.parse_commandline("pYc");
}

Data::~Data() {}

void Data::loadFile(std::string fileName) {
	this->fileName = fileName;
	IO::importOBJ(fileName, &polyhedron);

	fillTetgenio();
}

void Data::fillTetgenio() {
	tetgenio::facet *f;   // Define a pointer of facet.
	tetgenio::polygon *p; // Define a pointer of polygon.

	// All indices start from 1.
	tet_in.firstnumber = 1;


}

Dart_handle Data::make_facet(LCC_3& lcc,const std::vector<Point>& points) {
	Dart_handle d =
			CGAL::make_combinatorial_polygon<LCC_3>(lcc,(unsigned int)points.size());
	for (unsigned int i=0; i<points.size(); ++i)
	{
		lcc.set_vertex_attribute_of_dart(d, lcc.create_vertex_attribute(points[i]));
		d=d->beta(1);
	}
	return d;
}

void Data::evaluateArguments(std::list<std::string> args) {
	std::string argument;

	if(args.empty()) {
		printHelp();
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

void Data::fillPointList() {
	for(auto it = polyhedron.vertices_begin();it != polyhedron.vertices_end();it++) {
		pointList.push_back(Point(
								it->point().x(),
								it->point().y(),
								it->point().z()));
	}
}

void Data::printHelp() {
	std::cout << "|---------------------StraightSkeleton3D-----------------|" << std::endl;
	std::cout << "| usage: ss3d --obj <filename>                           |" << std::endl;
	std::cout << "|--------------------------------------------------------|" << std::endl;
}

#ifdef CGAL_LCC_USE_VIEWER
void Data::convertDataForGUI() {
	std::vector<Point> points;

	for(auto it_f = polyhedron.facets_begin();it_f != polyhedron.facets_end();it_f++) {
		SS3D::CGALPolyhedron::Halfedge_around_facet_circulator fc = it_f->facet_begin();
		do {
			points.push_back(Point(
								fc->vertex()->point().x(),
								fc->vertex()->point().y(),
								fc->vertex()->point().z()));
		} while(++fc != it_f->facet_begin());


		Dart_handle dh = make_facet(lcc,points);
		points.clear();
	}
}
#endif

} /* namespace SS3D */
