#ifndef SWEEPLINE_H_
#define SWEEPLINE_H_

/** 1. add all lines to SweepLine with addLine
 *  2. initiate event queue */

#include <CGAL/intersections.h>

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
	Ray   bisector;

	/* if base and e are parallel the bisector is either a line or there is none*/
	Line  line;
	Exact dist;

	/* we will use this idx to referenc to the current facets adjacent to 'bisector' */
	int leftListIdx, rightListIdx;

	/* to enables an ordering if two ArrangementLines of different base are compared  */
	int uid;

	/* TODO: remove, just for testing */
	int lid;

	int eid;

	bool parallel, isValid, ghost;

	ArrangementLine(EdgeIterator pbase, EdgeIterator pe, int id = NIL, int edgeid = NIL):
		base(pbase),e(pe),leftListIdx(NOLIST),rightListIdx(NOLIST),uid(id),lid(NIL),eid(edgeid),
		parallel(false),isValid(true),ghost(false) {


//		Point ipoint;
//		CGAL::Object intersect = intersection(base->supporting_line(),e->supporting_line());
		CGAL::Object intersect = intersection(base->supporting_line(),e->supporting_line());

		if(base != e) {
			if(!intersect.empty()) {
//			if(CGAL::assign(ipoint, intersect)) {
				//if(const Point *ipoint = boost::get<Point>(&*intersection)) {
				if(const Point *ipoint = CGAL::object_cast<Point>(&intersect)) {
					start    = *ipoint;
					bisector = setBisector();
				} else {
					parallel = true;
				}
			} else {
				parallel = true;
			}
		} else {
			parallel = true;
		}

		if(parallel) {
			if(base->supporting_line().direction() != e->supporting_line().direction()) {
				start = INFPOINT;
				line  = CGAL::bisector(base->supporting_line(), e->supporting_line());
				dist  = CGAL::squared_distance(base->supporting_line(),line);
			} else if(base->supporting_line() == e->supporting_line()) {
				ghost   = true;
				isValid = false;
			} else {
				isValid = false;
			}
		}
	}

	friend bool operator>  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator<  (const ArrangementLine& a, const ArrangementLine& b);
	friend bool operator== (const ArrangementLine& a, const ArrangementLine& b);
	friend ostream& operator<<(ostream& os, const ArrangementLine& al);

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

	inline void setGhostVertex(Point s) {
		isValid  = true;
		ghost    = true;
		parallel = false;
		start    = s;
		bisector = Ray(start,base->supporting_line().perpendicular(s));
		Point q(base->supporting_line().projection(s));
		if(Line(q,s).direction() != bisector.direction()) {
			bisector = Ray(start,base->supporting_line().perpendicular(s).opposite());
		}
	}
};

/* used to initially sort the ArrangementLines along their 'base' line */
using ArrangementStart 		= map<EdgeIterator,set<ArrangementLine, less<ArrangementLine> > >;
/* holds the actual ArrangementLines, to which we point to */
using AllArrangementLines	= map<EdgeIterator,vector<ArrangementLine> >;

using ALIterator 			= vector<ArrangementLine>::iterator;
//struct ALIterator : vector<ArrangementLine>::iterator {
//	friend bool operator>  (const ALIterator& a, const ALIterator& b);
//	friend bool operator<  (const ALIterator& a, const ALIterator& b);
//	friend bool operator== (const ALIterator& a, const ALIterator& b);
//};

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
		if(a->base != b->base) {
			cout << "ERROR: Base Line not equal!" << endl <<
			a->uid << " " << b->uid << endl;
			if(a->parallel) cout << "(a parallel)";
			if(b->parallel) cout << "(b parallel)";
			cout << "a: ";
			cout << a->base->vertex(0).x().doubleValue() << "," <<
					a->base->vertex(0).y().doubleValue() << "-" <<
					a->base->vertex(1).x().doubleValue() << "," <<
					a->base->vertex(1).y().doubleValue() << "  ";
			cout << "b: ";
			cout << b->base->vertex(0).x().doubleValue() << "," <<
					b->base->vertex(0).y().doubleValue() << "-" <<
					b->base->vertex(1).x().doubleValue() << "," <<
					b->base->vertex(1).y().doubleValue() << "  ";
			throw runtime_error("ERROR: Base Lines not equal!");
		}

		CGAL::Object intersect = intersection(a->bisector,b->bisector);

		if(a->parallel && b->parallel) {
			intersect = intersection(a->line,b->line);
		} else if(a->parallel) {
			intersect = intersection(a->line,b->bisector);
		} else if(b->parallel) {
			intersect = intersection(a->bisector,b->line);
		}

		if(!intersect.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersect)) {
				raysIntersect     = true;
				normalDistance    = CGAL::squared_distance(a->base->supporting_line(),*ipoint);
				intersectionPoint = *ipoint;
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

	inline bool isPossibleDivideNode() {
		return a->leftListIdx  != NOLIST    &&
			   a->rightListIdx == NOLIST    &&
			   b->leftListIdx  == NOLIST    &&
			   b->rightListIdx != NOLIST;
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

		if(a->ghost) {cout << "g";}
		cout << "a(";
		if(a->parallel) { cout << "p,p)-"; } else { cout <<
		a->bisector.to_vector().x().doubleValue() << "," <<
		a->bisector.to_vector().y().doubleValue() << ")-";
		}
		if(b->ghost) {cout << "g";}
		cout << "b(";
		if(b->parallel) { cout << "p,p) "; } else { cout <<
		b->bisector.to_vector().x().doubleValue() << "," <<
		b->bisector.to_vector().y().doubleValue() << ") ";
		}

	}

	inline void printIntPoint() {
		cout << intersectionPoint.x().doubleValue() << "," << intersectionPoint.y().doubleValue();
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
//		if(a->base != b->base) return a->uid < b->uid;
		return operator()(*a,*b);
//
//		Line currentBase(currentIntersection,a->base->direction());
//
//		auto intersectionA = CGAL::intersection(currentBase, a->bisector);
//		auto intersectionB = CGAL::intersection(currentBase, b->bisector);
//
//		if(!intersectionA.empty() && !intersectionB.empty()) {
//			if(const Point *pointA = CGAL::object_cast<Point>(&intersectionA)) {
//				if(const Point *pointB = CGAL::object_cast<Point>(&intersectionB)) {
//					return a->base->direction() == Vector(*pointB - *pointA).direction();
//				}
//			}
//		}
//
//		throw runtime_error("ERROR: empty intersections!");
	}

	bool operator()  (ArrangementLine a, ArrangementLine b) {
		if(a.base != b.base) return a.uid < b.uid;

		if(a.parallel && b.parallel) {
			return b.dist < a.dist;
		}

		Line currentBase(currentIntersection,a.base->direction());

		CGAL::Object intersectionA = intersection(currentBase, a.bisector.supporting_line());
		CGAL::Object intersectionB = intersection(currentBase, b.bisector.supporting_line());

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

struct ParallelALIteratorLess {
	// > Operator
	bool operator()  (ALIterator a, ALIterator b) {
		return (b->dist > a->dist) || (b->dist == a->dist && b->eid > a->eid);
	}
};

using EventQueue 	   		= set<SweepItem,less<SweepItem> >;
using ParallelEventQueue	= set<ALIterator,ParallelALIteratorLess>;

using LocalSweepLineStatus  = vector<ALIterator>;
using SweepLineStatus  		= map<EdgeIterator,LocalSweepLineStatus>;


struct SweepEvent : public vector<SweepItem> {
	inline int numberActiveCells() {
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

	inline int numberDivideNodes() {
		int cnt = 0;
		auto cells = getActiveCells();
		for(auto cell : cells) {
			if(cell->isPossibleDivideNode()) {
				++cnt;
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

	inline void printAll() {for(auto c : *this) {c.print(); cout << " --- ";}}

	inline vector<SweepItem*> getActiveCells() {
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

//using ColinearEP    = set<EdgeIterator>;
//using ColinearEdges = set<ColinearEP>;

class SweepLine {
public:
	SweepLine():config(nullptr) {}

	inline void addLine(ArrangementLine a) { arrangementStart[a.base].insert(a); }
	void initiateEventQueue();

	inline bool queueEmpty() { return eventQueue.empty(); }
	inline int queueSize()  { return eventQueue.size(); }

	SweepEvent popEvent();


	inline void addParallelLine(ArrangementLine al) {
		allParallelAL.push_back(al);
	}

	void handlePopEvent(SweepItem& item);

//	inline void addColinearEdgePair(EdgeIterator& a, EdgeIterator& b) {
//		set<EdgeIterator> s;
//		s.insert(a);
//		s.insert(b);
//		colinearEdges.insert(s);
//	}

	void printSweepLine(SweepItem& item);
	void printEventQueue();

	inline void setConfig(const Config* conf)   { config  = conf;}
	void insertGhostVertex(SweepItem* cell, SweepEvent& ghostCells);

	SweepLineStatus 			status;

private:
	ArrangementStart 			arrangementStart;
	AllArrangementLines 		allArrangementLines;

	EventQueue 					eventQueue;

	vector<ArrangementLine> 	allParallelAL;
	ParallelEventQueue 			parallelEventQueue;

//	ColinearEdges				colinearEdges;

	const Config*   			config;
};

#endif /* SWEEPLINE_H_*/
