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

	inline bool operator==(ArrangementLine arg) {
		return this->base == arg.base &&
				this->start == arg.start &&
				this->e == arg.e;
	}

	/* The second Line in the bisector is with changed orientation on purpose */
	inline Ray bisector() {
		return Ray(start,CGAL::bisector(base->supporting_line(), Line(e->vertex(1),e->vertex(0))));
	}
};

struct SweepItem {
	SweepItem(ArrangementLine pa, ArrangementLine pb):a(pa),b(pb) {
		if(a.base != b.base) {cout << "ERROR: Base Line not equal!" << endl;}

		auto intersection = CGAL::intersection(a.bisector(),b.bisector());

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				raysIntersect  = true;
				normalDistance = CGAL::squared_distance(a.base->supporting_line(),*ipoint);
			} else {
				raysIntersect  = false;
				normalDistance = -1;
			}
		} else {
			raysIntersect  	   = false;
			normalDistance     = -1;
		}
	}

	ArrangementLine a, b;
	bool raysIntersect;
	Exact normalDistance;

	inline Exact dist() { return normalDistance; }

	inline bool operator==(SweepItem& arg) {
		return this->normalDistance == arg.normalDistance &&
				this->a == arg.a && this->b == arg.b;
	}

	inline bool operator>(SweepItem& arg) {
		return this->normalDistance > arg.normalDistance;
	}
};

template<class T>
struct Compare {
    bool operator() (const T &a, const T &b) {
    	return a > b;
    }
    bool operator() (const T &a, T &b) {
    	return a > b;
    }
    bool operator() (T &a, const T &b) {
    	return a > b;
    }
    bool operator() (T &a, T &b) {
    	return a > b;
    }
};

using ArrangementStart 		= map<EdgeIterator,priority_queue<ArrangementLine,vector<ArrangementLine>, Compare<ArrangementLine> > >;
using EventQueue 	   		= priority_queue<SweepItem,vector<SweepItem>, Compare<SweepItem> >;
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

	inline SweepLineIterator prevEdge(SweepLineIterator& it, LocalSweepLineStatus& sls) {
		if(it == sls.begin()) {
			return sls.end() - 1;
		} else {
			return it - 1;
		}
	}

	inline SweepLineIterator nextEdge(SweepLineIterator& it, LocalSweepLineStatus& sls) {
		if(it+1 == sls.end()) {
			return sls.begin();
		} else {
			return it + 1;
		}
	}

private:
	ArrangementStart 	arrangementStart;
	EventQueue 			eventQueue;
	SweepLineStatus 	status;
};

#endif /* SWEEPLINE_H_*/
