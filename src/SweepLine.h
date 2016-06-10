#ifndef SWEEPLINE_H_
#define SWEEPLINE_H_

/** 1. add all lines to SweepLine with addLine
 *  2. initiate event queue */

#include <map>
#include <vector>

#include "CGALTypes.h"
#include "Definitions.h"

using namespace std;

/**
 * defines a line in the line arrangement, belongs to one 'base' edge and the arrangement
 * line is the 'bisector' between the referenced second edge 'e' and 'base'.
 * */
struct ArrangementLine {
	EdgeIterator base, e;
	Point start;
	Ray bisector;

	/* we will use this idx to referenc to the current facets adjacent to 'bisector' */
	int leftListIdx, rightListIdx;

	/* to enables an ordering if two ArrangementLines of different base are compared  */
	int uid;

	/* TODO: remove, just for testing */
	int lid;
	int eid;

	ArrangementLine(EdgeIterator pbase, EdgeIterator pe, int id = -1, int edgeid = -1):
		base(pbase),e(pe),leftListIdx(NOLIST),rightListIdx(NOLIST),uid(id),lid(-1),eid(edgeid) {
		assert(base != e);

		auto intersection = CGAL::intersection(base->supporting_line(),e->supporting_line());

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				start    = *ipoint;
				bisector = setBisector();
			} else {
				// TODO: construct bisector between parallel edges...
				cout << "ERROR: Constructor: No Intersection?" << endl;
			}
		} else {
			// TODO: construct bisector between parallel edges...
			cout << "ERROR: Constructor: Object empty?" << endl;
		}
	}

	friend bool operator>  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator<  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator== (const ArrangementLine& a, const ArrangementLine& b);

	/* The second Line in the bisector is with changed orientation on purpose */
	inline Ray setBisector() {
		Line bisectorLine = CGAL::bisector(base->supporting_line(), Line(e->vertex(1),e->vertex(0)));
		auto ray = Ray(start,bisectorLine);

		if(Line(*base).has_on_negative_side(start + ray.to_vector()) ||
		   Line(*e).has_on_negative_side(start + ray.to_vector()) 	) {
			ray = Ray(start,bisectorLine.opposite());
		}
		return ray;
	}
};

/* used to initially sort the ArrangementLines along their 'base' line */
using ArrangementStart 		= map<EdgeIterator,priority_queue<ArrangementLine, vector<ArrangementLine>, greater<ArrangementLine> > >;
/* holds the actual ArrangementLines, to which we point to */
using AllArrangementLines	= map<EdgeIterator,vector<ArrangementLine> >;

using ALIterator = vector<ArrangementLine>::iterator;

struct SweepItem {
	ALIterator a, b;
	EdgeIterator base;

	bool  raysIntersect;
	Exact normalDistance;
	Point intersectionPoint;

#ifdef QTGUI
	inline Point3D getPoint3D() {return Point3D(intersectionPoint.x().doubleValue(),
			intersectionPoint.y().doubleValue(),normalDistance.doubleValue());}
#endif

	SweepItem(const SweepItem& i):SweepItem(i.a,i.b) {}

	SweepItem(ALIterator pa, ALIterator pb):a(pa),b(pb),base(pa->base) {
		if(a->base != b->base) {cout << "ERROR: Base Line not equal!" << endl;}

		auto intersection = CGAL::intersection(a->bisector,b->bisector);

		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				raysIntersect  = true;
				normalDistance = CGAL::squared_distance(a->base->supporting_line(),*ipoint);
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

	/* enable accessing the list indices of left/right refs of a,b via setter/getter
	 * (0,0) a left, (0,1) a right, (1,0) b left, (1,1) b right
	 * */
	inline int get(int i, int j) {
		if(i == 0) {
			if(j == 0) {
				return a->leftListIdx;
			} else if(j == 1) {
				return a->rightListIdx;
			}
		} else if(i == 1) {
			if(j == 0) {
				return b->leftListIdx;
			} else if(j == 1) {
				return b->rightListIdx;
			}
		}
		else throw runtime_error("get(i,j) i or j > 1!");
		return NOLIST;
	}

	inline void set(int i, int j, int idx) {
		if(i == 0) {
			if(j == 0) {
				a->leftListIdx = idx;
			} else if(j == 1) {
				a->rightListIdx = idx;
			}
		} else if(i == 1) {
			if(j == 0) {
				b->leftListIdx = idx;
			} else if(j == 1) {
				b->rightListIdx = idx;
			}
		}
		else throw runtime_error("get(i,j) i or j > 1!");
	}

	inline void setAll(int idx) {
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 2; ++j) {
				set(i,j,idx);
			}
		}
	}

	inline bool isEmptyNode() {
		return a->leftListIdx  == NOLIST    &&
			   a->rightListIdx == NOLIST    &&
			   b->leftListIdx  == NOLIST    &&
			   b->rightListIdx == NOLIST;
	}

	inline bool isEdgeEvent() {
		return a->leftListIdx  == NOLIST    &&
			   a->rightListIdx != NOLIST    &&
			   b->leftListIdx  != NOLIST    &&
			   b->rightListIdx == NOLIST;
	}

	inline bool isInteriorNode() {
		return a->leftListIdx  != NOLIST          &&
			   a->rightListIdx == a->leftListIdx  &&
			   b->leftListIdx  == a->rightListIdx &&
			   b->rightListIdx == b->leftListIdx;
	}

	inline bool isBoundaryNode() {
		return (a->leftListIdx == NOLIST && a->rightListIdx != NOLIST) ||
			   (a->leftListIdx != NOLIST && a->rightListIdx == NOLIST) ||
			   (b->leftListIdx == NOLIST && b->rightListIdx != NOLIST) ||
			   (b->leftListIdx != NOLIST && b->rightListIdx == NOLIST);
	}

	inline int firstListIndex() {
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 2; ++j) {
				if(get(i,j) != NOLIST) {
					return get(i,j);
				}
			}
		}
		return NOLIST;
	}

	inline int numberOfActiveIndices() {
		int cnt = 0;
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 2; ++j) {
				if(get(i,j) != NOLIST) {
					++cnt;
				}
			}
		}
		return cnt;
	}

	inline bool isBoundaryOrInteriorNode() {
		return isBoundaryNode() || isInteriorNode();
	}

	inline bool hasAtLeastOneListIdx() {
		return firstListIndex() != NOLIST;
	}

	inline void print() {
		cout <<  a->eid << " (";
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 2; ++j) {
				if(get(i,j) == NOLIST) {
					cout << "_";
				} else {
					cout << get(i,j);
				}
				if(i != 1 || j != 1) cout << ",";
			}
		}
		cout << ")  ";
	}

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
	bool operator()  (ALIterator a, ALIterator b) {
		if(a->base != b->base) return a->uid < b->uid;

		Line currentBase(currentIntersection,a->base->direction());

		auto intersectionA = CGAL::intersection(currentBase, a->bisector);
		auto intersectionB = CGAL::intersection(currentBase, b->bisector);

		if(!intersectionA.empty() && !intersectionB.empty()) {
			if(const Point *pointA = CGAL::object_cast<Point>(&intersectionA)) {
				if(const Point *pointB = CGAL::object_cast<Point>(&intersectionB)) {
					return a->base->direction() == Vector(*pointB - *pointA).direction();
				}
			}
		}

		throw runtime_error("ERROR: empty intersections!");
	}
	bool operator()  (ArrangementLine a, ArrangementLine b) {
		if(a.base != b.base) return a.uid < b.uid;

		Line currentBase(currentIntersection,a.base->direction());

		auto intersectionA = CGAL::intersection(currentBase, a.bisector);
		auto intersectionB = CGAL::intersection(currentBase, b.bisector);

		if(!intersectionA.empty() && !intersectionB.empty()) {
			if(const Point *pointA = CGAL::object_cast<Point>(&intersectionA)) {
				if(const Point *pointB = CGAL::object_cast<Point>(&intersectionB)) {
					return a.base->direction() == Vector(*pointB - *pointA).direction();
				}
			}
		}

		throw runtime_error("ERROR: empty intersections!");
	}
};


using EventQueue 	   		= set<SweepItem,less<SweepItem> >;

using LocalSweepLineStatus  = vector<ALIterator>;
using SweepLineStatus  		= map<EdgeIterator,LocalSweepLineStatus>;


struct SweepEvent : public vector<SweepItem> {
	inline int numberActiveCell() {
		int cnt = 0;
		if(!empty()) {
			for(auto& e : *this) {
				if(e.hasAtLeastOneListIdx()) {
					++cnt;
				}
			}
		}
		return cnt;
	}

	inline bool containsEdgeEvent() {
		for(auto& e : *this) {
			if(e.isEdgeEvent()) {
				return true;
			}
		}
		return false;
	}

	inline bool containsInteriorNode() {
		for(auto& e : *this) {
			if(e.isInteriorNode()) {
				return true;
			}
		}
		return false;
	}

	inline void printAll() {for(auto c : *this) {c.print();}}

	inline vector<SweepItem*> getActivCells() {
		vector<SweepItem*> r;
		for(auto& c : *this) {if(c.hasAtLeastOneListIdx()) r.push_back(&c);}
		return r;
	}

	inline vector<SweepItem*> getAllCells() {
		vector<SweepItem*> r;
		for(auto& c : *this) {r.push_back(&c);}
		return r;
	}
};

class SweepLine {
public:
	SweepLine() {}

	inline void addLine(ArrangementLine a) { arrangementStart[a.base].push(a); }
	void initiateEventQueue();

	void initializePlaneSweepStart();

	inline bool queueEmpty() { return eventQueue.empty(); }
	inline long queueSize()  { return eventQueue.size(); }

	SweepEvent popEvent();

	void handlePopEvent(SweepItem& item);

	void printSweepLine(SweepItem& item);
	void printEventQueue();

	SweepLineStatus 	status;

private:
	ArrangementStart 	arrangementStart;

	AllArrangementLines allArrangementLines;
	EventQueue 			eventQueue;
};

#endif /* SWEEPLINE_H_*/
