#ifndef SWEEPLINE_H_
#define SWEEPLINE_H_

/** 1. add all lines to SweepLine with addLine
 *  2. initiate event queue */

#include <map>
#include <vector>

#include "CGALTypes.h"

using namespace std;

struct ArrangementLine {
	EdgeIterator base, e;
	Point start;

	/* to enables an ordering if two ArrangementLines of different base are compared  */
	int uid;

	/* TODO: remove, just for testing */
	int lid;
	int eid;

	ArrangementLine(EdgeIterator &pbase, EdgeIterator &pe, int id = -1, int edgeid = -1):
		base(pbase),e(pe),uid(id),lid(-1),eid(edgeid) {
		assert(base != e);

		auto intersection = CGAL::intersection(base->supporting_line(),e->supporting_line());

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				start = *ipoint;
			} else {
				cout << "Constructor: No Intersection?" << endl;
			}
		} else {
			cout << "Constructor: Object empty?" << endl;
		}
	}

	friend bool operator>  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator<  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator== (const ArrangementLine& a, const ArrangementLine& b);

	/* The second Line in the bisector is with changed orientation on purpose */
	inline Ray bisector() {
		Line bisectorLine = CGAL::bisector(base->supporting_line(), Line(e->vertex(1),e->vertex(0)));
		auto ray = Ray(start,bisectorLine);

		if(Line(*base).has_on_negative_side(start + ray.to_vector()) ||
		   Line(*e).has_on_negative_side(start + ray.to_vector()) 	) {
			ray = Ray(start,bisectorLine.opposite());
		}

		return ray;
	}
};

struct SweepItem {
	ArrangementLine a, b;
	EdgeIterator base;

	bool  raysIntersect;
	Exact normalDistance;
	Point intersectionPoint;

	SweepItem(ArrangementLine pa, ArrangementLine pb):a(pa),b(pb),base(pa.base) {
		if(a.base != b.base) {cout << "ERROR: Base Line not equal!" << endl;}

		auto intersection = CGAL::intersection(a.bisector(),b.bisector());

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				raysIntersect  = true;
				normalDistance = CGAL::squared_distance(a.base->supporting_line(),*ipoint);
				intersectionPoint   = *ipoint;
			} else {
				raysIntersect  = false;
				normalDistance = -1;
			}
		} else {
			raysIntersect  	   = false;
			normalDistance     = -1;
		}
	}

	inline Exact dist() { return normalDistance; }

	friend bool operator>  (const SweepItem& a, const SweepItem& b);
	friend bool operator<  (const SweepItem& a, const SweepItem& b);
	friend bool operator== (const SweepItem& a, const SweepItem& b);
};

/* used as compare object, to enable binary search in the sweep line status
 * at a specific distance to the base line */
struct DistanceCompare {
	Point currentIntersection;
	DistanceCompare(Point p): currentIntersection(p) {}

	// < Operator
	bool operator()  (ArrangementLine a, ArrangementLine b) {
		if(a.base != b.base) return a.uid < b.uid;

		Line currentBase(currentIntersection,a.base->direction());

		auto intersectionA = CGAL::intersection(currentBase, a.bisector());
		auto intersectionB = CGAL::intersection(currentBase, b.bisector());

		if(!intersectionA.empty() && !intersectionB.empty()) {
			if(const Point *pointA = CGAL::object_cast<Point>(&intersectionA)) {
				if(const Point *pointB = CGAL::object_cast<Point>(&intersectionB)) {
					return a.base->direction() == Vector(*pointB - *pointA).direction();
				}
			}
		}

		throw runtime_error("ERROR: empty intersections!");
	}

//	bool operator== (ArrangementLine& a, ArrangementLine& b);
//	bool operator>  (ArrangementLine& a, ArrangementLine& b);
};

using ArrangementStart 		= map<EdgeIterator,priority_queue<ArrangementLine,vector<ArrangementLine>, greater<ArrangementLine> > >;
//using EventQueue 	   		= SPQueue<SweepItem,vector<SweepItem>, greater<SweepItem> >;
using EventQueue 	   		= set<SweepItem,less<SweepItem> >;
using LocalSweepLineStatus  = vector<ArrangementLine>;
using SweepLineStatus  		= map<EdgeIterator,LocalSweepLineStatus>;
using SweepLineIterator     = vector<ArrangementLine>::iterator;
using SweepEvent			= vector<SweepItem>;

class SweepLine {
public:
	SweepLine() {}

	inline void addLine(ArrangementLine &a) { arrangementStart[a.base].push(a); }
	void initiateEventQueue();

	inline bool queueEmpty() { return eventQueue.empty(); }
	inline long queueSize()  { return eventQueue.size(); }
	SweepEvent popEvent();

	void handlePopEvent(SweepItem& item);

	void printSweepLine(SweepItem& item);
	void printEventQueue();

private:
	ArrangementStart 	arrangementStart;
	EventQueue 			eventQueue;
	SweepLineStatus 	status;
};

#endif /* SWEEPLINE_H_*/
