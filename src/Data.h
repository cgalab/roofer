/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_

#include <boost/shared_ptr.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_straight_skeleton_2.h>
#include <CGAL/Combinatorial_map_constructors.h>


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/Linear_cell_complex.h>
#include <CGAL/Linear_cell_complex_constructors.h>
#include <CGAL/Linear_cell_complex_operations.h>

#include <sys/stat.h>

namespace Skel {

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2                   	Point ;

typedef CGAL::Linear_cell_complex<3>    LCC_3;
typedef LCC_3::Dart_handle              Dart_handle;
typedef LCC_3::Point                    Point3D;
typedef LCC_3::FT                       FT;
typedef LCC_3::Traits 				    Traits;

typedef CGAL::Polygon_2<K>           	Polygon_2 ;
typedef CGAL::Straight_skeleton_2<K> 	SS;

typedef SS::Vertex_const_handle      	Vertex_const_handle ;
typedef SS::Halfedge_const_handle    	Halfedge_const_handle ;
typedef SS::Halfedge_const_iterator  	Halfedge_const_iterator ;

typedef boost::shared_ptr<SS> SsPtr ;


enum EventType : int {EDGE,SPLIT,CREATE};

typedef struct {
	EventType 	type;
	double 		eventTime;

} event;

class Data {
public:
	SsPtr     iss;
	Polygon_2 polygon;
	std::priority_queue<double, std::vector<int>, std::greater<int> > eventQueue;

	Data();
	virtual ~Data();

	void loadFile(std::string fileName);
	void evaluateArguments(std::list<std::string> args);

private:
	std::string    fileName;

	bool fileExists (const std::string& name);
	void printHelp();
};

} /* end namespace */

#endif /* DATA_H_ */
