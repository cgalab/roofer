#include "Facets.h"

//bool operator==(const FacetPoint& a, const FacetPoint& b) {
//		return  Point(a) == Point(b) &&
//				a.base == b.base;
//}
//bool operator> (const FacetPoint& a, const Point& b) {
//	auto aOnBase = a.base->supporting_line().projection(a);
//	auto bOnBase = a.base->supporting_line().projection(b);
//    return a.base->direction() == Vector(aOnBase - bOnBase).direction();
//}
//bool operator> (const FacetPoint& a, const FacetPoint& b) {
//	assert(a.base == b.base);
//	auto aOnBase = a.base->supporting_line().projection(a);
//	auto bOnBase = b.base->supporting_line().projection(b);
//    return a.base->direction() == Vector(aOnBase - bOnBase).direction();
//}
//bool operator< (const FacetPoint& a, const Point& b) {
//	auto aOnBase = a.base->supporting_line().projection(a);
//	auto bOnBase = a.base->supporting_line().projection(b);
//    return a.base->direction() == Vector(bOnBase - aOnBase).direction();
//}
//bool operator< (const FacetPoint& a, const FacetPoint& b) {
//	assert(a.base == b.base);
//	auto aOnBase = a.base->supporting_line().projection(a);
//	auto bOnBase = b.base->supporting_line().projection(b);
//    return a.base->direction() == Vector(bOnBase - aOnBase).direction();
//}

RoofFacets::RoofFacets():numberOfFacets(0) {}

void RoofFacets::addCellToFacet(SweepItem& item, int& listIdx) {
	auto& facetList = allLists[listIdx];
	facetList.push_back(item.intersectionPoint);
	cout << "THAT REALLY Happened!" << endl;
}

bool RoofFacets::cellHasActiveNeighbors(SweepItem& item) {
	if( item.a.leftListIdx >= 0 &&
		item.a.rightListIdx == item.a.leftListIdx &&
		item.a.leftListIdx == item.b.leftListIdx &&
		item.a.leftListIdx == item.b.rightListIdx) {

		return true;
	}
	return false;
}

bool RoofFacets::onlyOneCellHasActiveNeighbors(SweepEvent& event) {
	int cnt = 0;
	SweepItem& cell = event.front();
	for(auto& i : event) {
		if(cellHasActiveNeighbors(i)) {
			cell = i;
			if(++cnt > 1) {
				return false;
			}

		}
	}
	if(cnt == 1) {
		addCellToFacet(cell,cell.a.rightListIdx);
		return true;
	}

	return false;
}

void RoofFacets::addCell(SweepEvent& event) {

	if(!onlyOneCellHasActiveNeighbors(event)) {

	} else {

	}
}

bool RoofFacets::aGreaterB(Point a, Point b, EdgeIterator base) {
	auto aOnBase = base->supporting_line().projection(a);
	auto bOnBase = base->supporting_line().projection(b);
    return base->direction() == Vector(bOnBase - aOnBase).direction();
}

/* first facets on plane, starting at an edge of P (thus index 0) */
void RoofFacets::addBaseCell(ArrangementLine& line) {
	Point edgeStart(line.base->vertex(0));
	Point edgeEnd(line.base->vertex(1));

	if(line.start == edgeStart) {
		// starts the first facet on a plane
		int facetIdx = numberOfFacets++;
		int listIdx  = allLists.size();

		listToFacet[listIdx] = facetIdx;

		list<Point> l;
		l.push_back(line.start);
		allLists.push_back(l);

		Facet f;
		f.push_back(listIdx);
		allFacets[line.base].push_back(f);

		line.rightListIdx = listIdx;
	} else if(line.start == edgeEnd) {
		// still only one facet on the current plane
		auto& facet = allFacets[line.base].front();
		allLists[facet.front()].push_back(line.start);
		line.leftListIdx  = facet.front();
	} else if(aGreaterB(line.start,edgeStart,line.base) &&
			  aGreaterB(edgeEnd,line.start,line.base)) {
		// just reference from the arrangement lines that start
		// in the interior of the respective edge
		auto& facet = allFacets[line.base].front();
		line.leftListIdx  = facet.front();
		line.rightListIdx = facet.front();
	}
}

