#ifndef FACETS_H_
#define FACETS_H_


#include "CGALTypes.h"

#include "Definitions.h"
#include "SweepLine.h"

using namespace std;

class PointExt : public Point {
public:
//	using Point::Point;
	PointExt(Point p):Point(p),nextList(NOLIST) {}
	PointExt(int next):Point(),nextList(next) {}
	int nextList;
	friend ostream& operator<<(ostream& os, const PointExt& p);
};

// the lists are directly referenced by the arrangements lines
using AllLists		  = vector<list<PointExt>>;

#ifdef QTGUI
using PointToZ        = map<Point,double>;
#endif

//// a facet groups together all lists that are part of one facet
//using Facet 		  = vector<int>;

/* for every edge we have and idx to a list correspondig to a facet */
using AllFacets 	  = map<EdgeIterator,list<int>>;

/* ListIdx points to a list entry for a specific allfacets, list */
using ListToFacet	  = map<int,list<int>::iterator>;

class RoofFacets {
public:
	RoofFacets();

	void handleEvent(SweepEvent *event);
	void addBaseCell(ALIterator& line);

	bool aGreaterB(Point a, Point b, EdgeIterator base);

	void setMinimizing() { if(!maximize) minimize = true; }
	void setMaximizing() { if(!minimize) maximize = true; }

	inline void setPolygon(const Polygon* poly) { polygon = poly; }

	AllLists    allLists;
	AllFacets   allFacets;
	ListToFacet listToFacet;

#ifdef QTGUI
	PointToZ    zMap;
#endif

	void printAllLists();
private:
	void handleEdgeEvent(SweepEvent* event);
	void handleSplitEvent(SweepEvent* event);

	bool handleCreateEventA(SweepEvent* event);
	bool handleCreateEventB(SweepEvent* event);

	void handleLeaveEvent(SweepItem* cell);
	void handleEnterEvent(SweepItem* cell);

	void handleMergeEvent(SweepEvent* event);
	void handleCreateMergeEvent(SweepEvent* event);

	void addPointToNewList(SweepItem* cell);
	void addPointToCurrentList(SweepItem* cell);

	/* a->left to b->left or b->right to a->right */
	void turnLefRightOnIntersection(SweepItem* cell);


	bool			minimize, maximize;
	const Polygon* 	polygon;

	EdgeIterator next(EdgeIterator i);
	EdgeIterator prev(EdgeIterator i);
};

#endif /* FACETS_H_ */
