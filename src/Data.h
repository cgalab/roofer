/*
 * Data.h
 *
 *  Created on: May 26, 2015
 *      Author: gue
 */

#ifndef DATA_H_
#define DATA_H_

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Combinatorial_map_constructors.h>

#include <CGAL/Polyhedron_items_3.h>
#include <CGAL/HalfedgeDS_list.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Linear_cell_complex.h>

#include <CGAL/Arrangement_2.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/intersections.h>

#include <CGAL/Bbox_2.h>

//#include <CGAL/Sweep_line_2_algorithms.h>

#include <vector>
#include <list>
#include <memory>

#include <sys/stat.h>

//#include "LinkedList.h"

using namespace std;

using K 		     = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using Trait          = CGAL::Arr_segment_traits_2<K>;

using BBox			 = CGAL::Bbox_2;
using Polygon        = CGAL::Polygon_2<K>;

using Vector         = Trait::Vector_2;
using Point          = Trait::Point_2;
using Line           = Trait::Line_2;
using Ray            = Trait::Ray_2;
using Direction      = Trait::Direction_2;
//using Edge           = Trait::Segment_2;
using Edge           = Trait::X_monotone_curve_2;

using VertexIterator = Polygon::Vertex_iterator;

using Transformation = CGAL::Aff_transformation_2<K>;

using Arrangement    = CGAL::Arrangement_2<Trait>;


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
//struct LineArrangement {
//    LineArrangement(Edge e, p)
//    {}
//
//    vector<Ray>  lines;
//    vector<int>  sweepLineStatus;
//
//
//};

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
};

struct Compare {
    bool operator() (Event &a, Event &b) {
    	return a.height > b.height;
    }
};

using EventQueue 		  = priority_queue<Event,vector<Event>, Compare >;
using AllLineArrangements = vector<Arrangement>;
using Cell       		  = vector<Point>;
using Facet      		  = vector<Cell>;

class Data {
public:
	Polygon    	  		 polygon;
	BBox 				 bbox;

	AllLineArrangements  lineArrangements;
	EventQueue    		 eventQueue;
    
	Config        		 config;

    Transformation 		 rotateNintyLeft;

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
