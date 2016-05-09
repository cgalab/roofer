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

	inline bool operator>(ArrangementLine arg) {
		return arg.base == base && base->direction() == Vector(this->start - arg.start).direction();
	}

//	inline bool operator> (const ArrangementLine& lhs, const ArrangementLine& rhs) {
//		return lhs.base == rhs.base && lhs.base->direction() == Vector(lhs.start - rhs.start).direction();
//	}

	/* The second Line in the bisector is with changed orientation on purpose */
	inline Ray bisector() {
		return Ray(start,CGAL::bisector(base->supporting_line(), Line(e->vertex(1),e->vertex(0))));
	}
};

struct SweepItem {
	SweepItem(ArrangementLine pa, ArrangementLine pb):a(pa),b(pb) {
		if(a.base != b.base) {cout << "ERROR: Base Line not equal!" << endl;}

		if(CGAL::do_intersect(a.bisector(),b.bisector())) {
			raysIntersect = true;
			normalDistance = dist();
		} else {
			raysIntersect = false;
		}
	}

	ArrangementLine a, b;
	bool raysIntersect;
	Exact normalDistance;

	inline Exact dist() {
		auto intersection = CGAL::intersection(a.bisector(),b.bisector());

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {

				return CGAL::squared_distance(a.base->supporting_line(),*ipoint);

			} else {
//				cout << "SweepItem: No Intersection (Point)?" << endl;
			}
		} else {
//			cout << "SweepItem: No Intersection (Object empty)?" << endl;
		}
		// TODO: exception handling!
		return -1;
	}

//	inline bool operator> (const SweepItem& lhs, const SweepItem& rhs) {
//		return lhs.normalDistance > rhs.normalDistance;
//		//return lhs.base == rhs.base && lhs.base->direction() == Vector(lhs.start - rhs.start).direction();
//	}

	inline bool operator>(SweepItem arg) {
		return this->normalDistance > arg.normalDistance;
	}
};

template<class T>
struct Compare {
    bool operator() (T &a, T &b) {
    	return a > b;
    }
};

using ArrangementStart = map<EdgeIterator,priority_queue<ArrangementLine,vector<ArrangementLine>, Compare<ArrangementLine> > >;

using EventQueue 	   = priority_queue<SweepItem,vector<SweepItem>, Compare<SweepItem> >;
using SweepLineStatus  = map<EdgeIterator,vector<ArrangementLine> >;

struct SweepLine {
	ArrangementStart arrangementStart;

	EventQueue eventQueue;
	SweepLineStatus status;

	inline void addLine(ArrangementLine &a) {arrangementStart[a.base].push(a);}

	void initiateEventQueue();

	SweepLine() {}
};

#endif /* SWEEPLINE_H_*/
