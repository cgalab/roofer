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
#include <CGAL/create_straight_skeleton_2.h>
#include <CGAL/Combinatorial_map_constructors.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Linear_cell_complex.h>

#include <CGAL/intersections.h>

#include <sys/stat.h>

#include "LinkedList.h"

using namespace std;

using K 		   = CGAL::Exact_predicates_exact_constructions_kernel;
using Vector       = K::Vector_3;
using Point        = K::Point_3;
using Line         = K::Line_3;
using Ray          = K::Ray_3;
using Direction    = K::Direction_3;
using Polygon      = CGAL::Polygon_2<K>;
using Plane        = K::Plane_3;
using PlanePolygon = vector<Plane>;

using Transformation = CGAL::Aff_transformationC2<K>;

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

struct LineArrangement {
	LineArrangement(Edge e, )
	: Point(x,y),start(x,y),time(0),startTime(0),reflex(false),inWavefront(true)	{}

    vector<Line> lines;
    vector<int>  sweepLineStatus;

    
};

/* event location is the point itself.  */
struct Event : public Point {
	EventType 	   type;
	K::FT 		   height;

	//	WavefrontIterator a, b, c;
    Edge e;
    Intersection e1, e2;

	Event(Point v, EventType t, K::FT _time, WavefrontPoint _a, WavefrontPoint _b, WavefrontPoint _c)
	: Point(v),type(t),time(_time),a(_a),b(_b),c(_c) {}
	Event(Point v, EventType t)
	: Point(v),type(t),time(0),a(NULL),b(NULL),c(NULL) {}
	Event()
	: Point(),type(EventType::EMPTY),time(0),a(NULL),b(NULL),c(NULL) {}
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
	PlanePolygon  planes;
	Config        config;

	EventQueue    eventQueue;

	Transformation rotateNintyLeft;

	Data();
	virtual ~Data();

	bool evaluateArguments(list<string> args);

	WavefrontIterator next(WavefrontIterator i);
	WavefrontIterator prev(WavefrontIterator i);

private:
	bool fileExists(string fileName);
	bool loadFile();
	void printHelp();
};

#endif /* DATA_H_ */
