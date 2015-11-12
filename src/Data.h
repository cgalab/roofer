/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_

#include <vector>

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
//#include <CGAL/Linear_cell_complex_constructors.h>
//#include <CGAL/Linear_cell_complex_operations.h>
#include <CGAL/intersections.h>

#include <sys/stat.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Vector_2                   	Vertex;
typedef K::Point_2						Point;
typedef K::Line_2						Line;
typedef K::Ray_2						Ray;
typedef K::Direction_2					Direction;

typedef CGAL::Linear_cell_complex<3>    LCC_3;
typedef LCC_3::Dart_handle              Dart_handle;
typedef LCC_3::Point                    Point3D;
typedef LCC_3::FT                       FT;
typedef LCC_3::Traits 				    Traits;

typedef CGAL::Polygon_2<K>           	Polygon;

//typedef CGAL::Straight_skeleton_2<K> 	SS;
//typedef SS::Vertex_const_handle      	Vertex_const_handle ;
//typedef SS::Halfedge_const_handle    	Halfedge_const_handle ;
//typedef SS::Halfedge_const_iterator  	Halfedge_const_iterator ;

typedef boost::shared_ptr<Polygon> SsPtr ;

enum EventType : int {EDGE,SPLIT,DIVIDE,CREATE};

struct WavefrontPoint : public Point {
	Vertex    velocity;
	Ray       direction;

	bool 	  reflex;

	WavefrontPoint(Point v)
	: Point(v),reflex(false)	{
	}
};

struct Event : public Point {
	EventType 	   type;
	double 		   time;		    // TODO: double -> distance measure CGAL uses

	WavefrontPoint *a, *b, *c;       	// TODO: whatever object type edges are/will be

	Event(Point v, EventType t)
		: Point(v),type(t),time(0),a(NULL),b(NULL),c(NULL)	{
	}
};

class Compare {
public:
    bool operator() (Event &a, Event &b)
    {
    	if (a.time > b.time) {
    		return true;
    	} else {
    		return false;
    	}
    }
};

struct Config {
	std::string printOptions;

	bool 		gui;
	bool 		maximize;
	std::string fileName;

	Config():gui(false),fileName(""),maximize(true) {
		printOptions = "[-min|-max] [-gui] <filename>";
	}
};

typedef std::priority_queue<Event, std::vector<Event>, Compare > EventQueue;
typedef CGAL::Polygon_2<K,std::vector<WavefrontPoint> > Wavefront;

class Data {
public:
	SsPtr      iss;
	Polygon    polygon;
	Config     config;

	Wavefront  wavefront;

	EventQueue eventQueue;

	Data();
	virtual ~Data();

	bool evaluateArguments(std::list<std::string> args);

private:
	bool fileExists(std::string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */
