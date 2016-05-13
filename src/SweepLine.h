#ifndef SWEEPLINE_H_
#define SWEEPLINE_H_

/** 1. add all lines to SweepLine with addLine
 *  2. initiate event queue */

#include <map>
#include <vector>

#include "CGALTypes.h"
#include "SPQueue.h"

using namespace std;

struct ArrangementLine {
	EdgeIterator base, e;
	Point start;

	ArrangementLine(EdgeIterator &pbase, EdgeIterator &pe):base(pbase),e(pe) {
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

		if(Line(*base).has_on_negative_side(start + ray.to_vector())) {
			ray = Ray(start,bisectorLine.opposite());
		}

		return ray;
	}
};

struct SweepItem {
	ArrangementLine a, b;

	bool  raysIntersect;
	Exact normalDistance;
	Point intersectionPoint;

	SweepItem(ArrangementLine pa, ArrangementLine pb):a(pa),b(pb) {
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

using ArrangementStart 		= map<EdgeIterator,priority_queue<ArrangementLine,vector<ArrangementLine>, greater<ArrangementLine> > >;
using EventQueue 	   		= SPQueue<SweepItem,vector<SweepItem>, greater<SweepItem> >;
using LocalSweepLineStatus  = vector<ArrangementLine>;
using SweepLineStatus  		= map<EdgeIterator,LocalSweepLineStatus>;
using SweepLineIterator     = vector<ArrangementLine>::iterator;

class SweepLine {
public:
	SweepLine() {}

	inline void addLine(ArrangementLine &a) { arrangementStart[a.base].push(a); }
	void initiateEventQueue();

	inline bool queueEmpty() { return eventQueue.empty(); }
	inline long queueSize()  { return eventQueue.size(); }
	SweepItem popEvent();

	void handlePopEvent(SweepItem &item);

private:
	ArrangementStart 	arrangementStart;
	EventQueue 			eventQueue;
	SweepLineStatus 	status;
};

#endif /* SWEEPLINE_H_*/
