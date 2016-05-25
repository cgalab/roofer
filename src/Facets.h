#ifndef FACETS_H_
#define FACETS_H_


#include "CGALTypes.h"

#include "Definitions.h"
#include "SweepLine.h"

using namespace std;

// the lists are directly referenced by the arrangements lines
using AllLists		  = vector<list<Point>>;

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

	AllLists    allLists;
private:
	void addCellToFacet(SweepItem& item, int& listIdx);

	void handleEdgeEvent(SweepEventReturnContainer& event);
	void handleSplitEvent(SweepEventReturnContainer& event);
	void handleCreate1Event(SweepEventReturnContainer& event);
	void handleEnterOrCreate1Event(SweepEventReturnContainer& event);
	void handleMergeOrCreate2Event(SweepEventReturnContainer& event);
	void handleEnterEvent(SweepEventReturnContainer& event);

	void handleMergeEvent(SweepEventReturnContainer& event);

	void addPointToNewList(SweepItem& item);
	void addPointToCurrentList(SweepItem& item);

	int 		numberOfFacets;

	AllFacets   allFacets;
	ListToFacet listToFacet;
};

#endif /* FACETS_H_ */
