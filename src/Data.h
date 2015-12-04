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

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
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

using namespace std;

using K 		  = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using Vertex      = K::Vector_2;
using Point       = K::Point_2;
using Line        = K::Line_2;
using Ray         = K::Ray_2;
using Direction   = K::Direction_2;

using LCC_3       = CGAL::Linear_cell_complex<3>;
using Dart_handle = LCC_3::Dart_handle;
using Point3D     = LCC_3::Point;
//using FT          = LCC_3::FT;
//using Traits      = LCC_3::Traits;

using Polygon     = CGAL::Polygon_2<K>;

using SsPtr       = shared_ptr<Polygon>;

enum class EventType {EDGE,SPLIT,DIVIDE,CREATE,EMPTY};

struct WavefrontPoint : public Point {
	WavefrontPoint(K::FT x, K::FT y)
	: Point(x,y),reflex(false)	{}
	WavefrontPoint(Point v)
	: Point(v),reflex(false)	{}
	WavefrontPoint(Point v, int idx)
	: Point(v),reflex(false)	{}
	WavefrontPoint(Point v, bool r)
	: Point(v),reflex(r)		{}
//	WavefrontPoint(K::FT x, K::FT y)
//	: Point(x,y),reflex(false),wavefrontIndex(-1)	{}
//	WavefrontPoint(Point v)
//	: Point(v),reflex(false),wavefrontIndex(-1)	{}
//	WavefrontPoint(Point v, int idx)
//	: Point(v),reflex(false),wavefrontIndex(idx)	{}
//	WavefrontPoint(Point v, bool r)
//	: Point(v),reflex(r),wavefrontIndex(-1)		{}

	Vertex    velocity;
	Ray       direction;

//	int		  wavefrontIndex;
	bool 	  reflex;

	K::FT x() {return this->x();}
	K::FT y() {return this->y();}

	K::FT x(K::FT t) {return currentLocation(t).x();}
	K::FT y(K::FT t) {return currentLocation(t).y();}

	/* returns the wavefront point at time 'time' */
	inline WavefrontPoint currentLocation(K::FT time){
		auto dir = direction.to_vector();

		/* 'unit-normalize' direction vector */
		auto len = CGAL::sqrt(dir.squared_length());
		auto vector = dir/len * time;

		// TODO: wrong, we have to scale up the 'time' about
		//       the speed value 1/sin(alpha/2)....

		return WavefrontPoint(*this + vector);
	}
};

using WavefrontIterator = CGAL::Polygon_2<K,vector<WavefrontPoint,allocator<WavefrontPoint>>>::Vertex_const_iterator;

/* event location is the point itself.  */
struct Event : public Point {
	EventType 	   type;
	K::FT 		   time;

	WavefrontIterator a, b, c;       	// TODO: whatever object type edges are/will be

	Event(Point v, EventType t)
	: Point(v),type(t),time(0),a(NULL),b(NULL),c(NULL) {}
	Event()
	: Point(),type(EventType::EMPTY),time(0),a(NULL),b(NULL),c(NULL) {}
};

struct Compare {
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
	Config():gui(false),maximize(true),fileName("") {
		printOptions = "[-min|-max] [-gui] <filename>";
	}

	bool 		gui;
	bool 		maximize;
	string 		fileName;

	string 		printOptions;
};

using EventQueue = priority_queue<Event,vector<Event>, Compare >;
using Wavefront  = CGAL::Polygon_2<K,vector<WavefrontPoint> >;

class Data {
public:
	SsPtr      iss;
	Polygon    polygon;
	Config     config;

	Wavefront  wavefront;
	EventQueue eventQueue;

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
