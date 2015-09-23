/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <sys/stat.h>

#include "tetgen/tetgen.h"

namespace SS3D {
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel,
		CGAL::Polyhedron_items_3,
		CGAL::HalfedgeDS_list> CGALPolyhedron;


typedef CGAL::Linear_cell_complex<3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle;
typedef LCC_3::Point                 Point;
typedef LCC_3::FT                    FT;
typedef LCC_3::Traits 				 Traits;


class SS3DVertices : Point {

};


class Data {
public:
	/* tetgen data structure and class */
	tetgenio 	   tet_in, tet_out;
	tetgenbehavior tet_beh;


	Data();
	virtual ~Data();

	void loadFile(std::string fileName);
	void fillTetgenio();

	CGALPolyhedron getPolyhedron() { return polyhedron; }

	void evaluateArguments(std::list<std::string> args);

	void fillPointList();
	std::list<Point> getPointList() { return pointList; }

#ifdef CGAL_LCC_USE_VIEWER
/* used for GUI */
	LCC_3 lcc;
	void convertDataForGUI();
#endif

private:
	std::string    fileName;


//	void tetrahedralize(char *switches, tetgenio *in, tetgenio *out,
//	                    tetgenio *addin = NULL, tetgenio *bgmin = NULL);

	/* data from input file is loaded here */
	CGALPolyhedron polyhedron;

	std::list<Point> pointList;

	Dart_handle make_facet(LCC_3& lcc,const std::vector<Point>& points);
	bool fileExists (const std::string& name);
	void printHelp();
};

} /* namespace SS3D */

#endif /* DATA_H_ */
