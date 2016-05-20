#ifndef FACETS_H_
#define FACETS_H_

#include "CGALTypes.h"
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

	void addCell(SweepEvent& event);
	void addBaseCell(ArrangementLine& line);


	bool aGreaterB(Point a, Point b, EdgeIterator base);
private:
	void addCellToFacet(SweepItem& item, int& listIdx);

	bool cellHasActiveNeighbors(SweepItem& item);
	bool onlyOneCellHasActiveNeighbors(SweepEvent& event);

	int 		numberOfFacets;

	AllLists    allLists;
	AllFacets   allFacets;
	ListToFacet listToFacet;
};

#endif /* FACETS_H_ */
