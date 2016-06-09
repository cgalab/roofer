#ifndef FACETS_H_
#define FACETS_H_


#include "CGALTypes.h"

#include "Definitions.h"
#include "SweepLine.h"

using namespace std;

// the lists are directly referenced by the arrangements lines
using AllLists		  = vector<list<Point>>;

#ifdef QTGUI
using PointToZ        = map<Point,double>;
#endif

// a facet groups together all lists that are part of one facet
using Facet 		  = vector<int>;
using AllFacets 	  = map<EdgeIterator,vector<Facet>>;

/* ListIdx to FacetIdx */
using ListToFacet	  = map<int,int>;

class RoofFacets {
public:
	RoofFacets();

	void handleCell(SweepEvent *event);
	void addBaseCell(ALIterator& line);

	bool aGreaterB(Point a, Point b, EdgeIterator base);

	void setMinimizing() { if(!maximize) minimize = true; }
	void setMaximizing() { if(!minimize) maximize = true; }

	AllLists    allLists;
	AllFacets   allFacets;
	ListToFacet listToFacet;

#ifdef QTGUI
	PointToZ    zMap;
#endif
private:
	void addCellToFacet(SweepItem& item, int& listIdx);

	void handleEdgeEvent(SweepEvent* event);
	void handleSplitEvent(SweepEvent* event);

	bool handleCreateEventA(SweepEvent* event);

	void handleLeaveEvent(SweepItem* cell);
	void handleEnterEvent(SweepItem* cell);

	void handleMergeEvent(SweepEvent* event);

	void addPointToNewList(SweepItem* cell);
	void addPointToCurrentList(SweepItem* cell);

	/* a->left to b->left or b->right to a->right */
	void turnLefRightOnIntersection(SweepItem* cell);

	int 		numberOfFacets;
	bool		minimize, maximize;
};

#endif /* FACETS_H_ */
