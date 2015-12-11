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
using Polygon     = CGAL::Polygon_2<K>;

using Transformation = CGAL::Aff_transformationC2<K>;

enum class EventType {EDGE,SPLIT,DIVIDE,CREATE,EMPTY};

struct Config {
	Config():gui(false),maximize(true),fileName("") {
		printOptions = "[-min|-max] [-gui] <filename>";
	}

	bool 		gui;
	bool 		maximize;
	string 		fileName;

	string 		printOptions;
};

struct WavefrontPoint : public Point {
	WavefrontPoint(K::FT x, K::FT y)
	: Point(x,y),start(x,y),time(0),startTime(0),reflex(false),inWavefront(true)	{}
	WavefrontPoint(Point v)
	: Point(v),start(v),time(0),startTime(0),reflex(false),inWavefront(true)	{}
	WavefrontPoint(WavefrontPoint w, Point v)
	: Point(v),start(w.start),time(w.time),startTime(w.startTime),
	  velocity(w.velocity),reflex(w.reflex),inWavefront(w.inWavefront)	{}

	Point   start;
	K::FT 	time;
	K::FT   startTime;

	Vertex  velocity;

	bool 	reflex;
	bool	inWavefront;

	inline void enable()  {inWavefront = true; }
	inline void disable() {inWavefront = false;}
	inline bool isInWavefront() {return inWavefront;}

	inline Ray  getRay() {return Ray(*this,velocity);}

	inline void setStartTime(K::FT t) {startTime = t;}

	inline void moveToTime(K::FT newTime) {
		time  = newTime;
		*this = WavefrontPoint(*this, start + (velocity * (time - startTime)));
	}

	inline Point getPointAtTime(K::FT newTime) {
		return start + (velocity * (newTime - startTime));
	}
};

using WavefrontIterator = CGAL::Polygon_2<K,vector<WavefrontPoint,allocator<WavefrontPoint>>>::Vertex_const_iterator;

/* event location is the point itself.  */
struct Event : public Point {
	EventType 	   type;
	K::FT 		   time;

	WavefrontIterator a, b, c;

	Event(Point v, EventType t, K::FT _time, WavefrontIterator _a, WavefrontIterator _b, WavefrontIterator _c)
	: Point(v),type(t),time(_time),a(_a),b(_b),c(_c) {}
	Event(Point v, EventType t)
	: Point(v),type(t),time(0),a(NULL),b(NULL),c(NULL) {}
	Event()
	: Point(),type(EventType::EMPTY),time(0),a(NULL),b(NULL),c(NULL) {}
};

struct Compare {
    bool operator() (Event &a, Event &b) {
    	return a.time > b.time;
    }
};

using EventQueue = priority_queue<Event,vector<Event>, Compare >;
using Wavefront  = CGAL::Polygon_2<K,vector<WavefrontPoint> >;

class Data {
public:
	Polygon    polygon;
	Config     config;

	Wavefront  wavefront;
	EventQueue eventQueue;

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
