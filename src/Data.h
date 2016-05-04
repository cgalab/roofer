/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_

#include <vector>
#include <memory>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Combinatorial_map_constructors.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Linear_cell_complex.h>

#include <CGAL/Arrangement_2.h>

#include <CGAL/intersections.h>

#include <sys/stat.h>

#include "LinkedList.h"

using namespace std;

using K 		    = CGAL::Exact_predicates_exact_constructions_kernel;
using Vector        = K::Vector_2;
using Point         = K::Point_2;
using Line          = K::Line_2;
using Ray           = K::Ray_2;
using Direction     = K::Direction_2;
using Polygon       = CGAL::Polygon_2<K>;
//using PolygonPlanes = vector<Plane>;
using Edge          = K::Segment_2;

using VertexIterator = Polygon::Vertex_iterator;

using Transformation = CGAL::Aff_transformation_2<K>;

enum class EventType {EDGE,SPLIT,CREATE,MERGE,EMPTY};

struct Config {
	Config():gui(false),maximize(true),fileName("") {
		printOptions = "[-min|-max] [-gui] <filename>";
	}

	bool 		gui;
	bool 		maximize;
	string      fileName;

	string 		printOptions;
};

// TODO: use arrangement class for this!
struct LineArrangement {
    LineArrangement(Edge e, p)
    {}
    
    vector<Line> lines;
    vector<int>  sweepLineStatus;
    
    
};

/* event location is the point itself.  */
struct Event {
	EventType 	   type;
    Point          point;
	K::FT 		   height;

    int edgeIndex;
    int edgeIndexA;
    int edgeIndexB;

	Event(Point v, EventType t, K::FT _height, int _self, int _a, int _b)
	: type(t),point(v),height(_height),edgeIndex(_self),edgeIndexA(_a),edgeIndexB(_b) {}
	//Event(Point v, EventType t)
	//: point(v),type(t),height(0),edgeIndex(NULL),edgeIndexA(NULL),edgeIndex(NULL) {}
	//Event()
	//: Point(),type(EventType::EMPTY),time(0),a(NULL),b(NULL),c(NULL) {}
};

struct Compare {
    bool operator() (Event &a, Event &b) {
    	return a.height > b.height;
    }
};

using EventQueue = priority_queue<Event,vector<Event>, Compare >;
using Cell       = vector<Point>;
using Facet      = vector<Cell>;

class Data {
public:
	Polygon    	  polygon;
//	PolygonPlanes planes;
    
	Config        config;

	EventQueue    eventQueue;

    Transformation rotateNintyLeft;

	Data();
	virtual ~Data();

	bool evaluateArguments(list<string> args);

	VertexIterator next(VertexIterator i);
	VertexIterator prev(VertexIterator i);

private:
	bool fileExists(string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */
