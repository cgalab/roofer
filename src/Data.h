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

#include <sys/stat.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
typedef K::Point_2                   	Vertex;

typedef CGAL::Linear_cell_complex<3>    LCC_3;
typedef LCC_3::Dart_handle              Dart_handle;
typedef LCC_3::Point                    Point3D;
typedef LCC_3::FT                       FT;
typedef LCC_3::Traits 				    Traits;

typedef CGAL::Polygon_2<K>           	Polygon_2;

//typedef CGAL::Straight_skeleton_2<K> 	SS;
//typedef SS::Vertex_const_handle      	Vertex_const_handle ;
//typedef SS::Halfedge_const_handle    	Halfedge_const_handle ;
//typedef SS::Halfedge_const_iterator  	Halfedge_const_iterator ;

typedef boost::shared_ptr<Polygon_2> SsPtr ;

enum EventType : int {EDGE,SPLIT,CREATE};

class WavefrontVertex : public Vertex {
public:
	Vertex    velocity;
	K::Line_2 bisector;

	WavefrontVertex(Vertex v)
	: Vertex(v)	{
	}
};

typedef struct {
	EventType 	type;
	double 		eventTime;				// TODO: double -> distance measure CGAL uses
	int			*edge1,*edge2,*edge3;	// TODO: whatever object type edges are/will be
} event;

class Compare {
public:
    bool operator() (event &a, event &b)
    {
    	if (a.eventTime < b.eventTime) {
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

class Data {
public:
	SsPtr     iss;
	Polygon_2 polygon;
	Config    config;

	CGAL::Polygon_2<K,std::vector<WavefrontVertex> > wavefront;

	std::priority_queue<event, std::vector<event>, Compare > eventQueue;

	Data();
	virtual ~Data();

	bool evaluateArguments(std::list<std::string> args);

private:
	bool fileExists(std::string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */
