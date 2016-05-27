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

//bool RoofFacets::cellHasActiveNeighbors(SweepItem& item) {
//	if( item.a.leftListIdx >= 0 &&
//		item.a.rightListIdx == item.a.leftListIdx &&
//		item.a.leftListIdx == item.b.leftListIdx &&
//		item.a.leftListIdx == item.b.rightListIdx) {
//
//		return true;
//	}
//	return false;
//}

//pair<bool,SweepItem> RoofFacets::onlyOneCellHasActiveNeighbors(SweepEvent& event) {
//	int cnt = 0;
//	auto& cell = event.front();
//	for(auto& i : event) {
//		if(cellHasActiveNeighbors(i)) {
//			cell = i;
//			if(++cnt > 1) {
//				return make_pair(false,cell);
//			}
//
//		}
//	}
//	if(cnt == 1) {
//		addCellToFacet(cell,cell.a.rightListIdx);
//		return make_pair(true,cell);
//	}
//
//	return make_pair(false,cell);
//}

//bool RoofFacets::isEdgeEvent(const SweepItem& item) {
//	auto& a = item.a;
//	auto& b = item.b;
//
//	if(a.leftListIdx == NOLIST         && a.rightListIdx != NOLIST &&
//	   b.leftListIdx == a.rightListIdx && b.rightListIdx == NOLIST) {
//		return true;
//	}
//
//	return false;
//}

//int RoofFacets::hasListIndex(const SweepItem& item) {
//	// TODO: check for multiple inidices and equal indices !!!
//
//	if(item.a.leftListIdx != NOLIST) {
//		return item.a.leftListIdx;
//	} else if(item.a.rightListIdx != NOLIST) {
//		return item.a.rightListIdx;
//	} else if(item.b.leftListIdx != NOLIST) {
//		return item.b.leftListIdx;
//	} else if(item.b.rightListIdx != NOLIST) {
//		return item.b.rightListIdx;
//	}
//
//	return NOLIST;
//}

//pair<bool,int> RoofFacets::isInteriorNode(const SweepItem& item) {
//	if(item.a.leftListIdx != NOLIST && item.a.rightListIdx == item.a.leftListIdx &&
//	   item.b.leftListIdx == item.a.rightListIdx && item.b.rightListIdx == item.b.leftListIdx) {
//		return make_pair(true,item.a.leftListIdx);
//	}
//
//	return make_pair(false,NOLIST);
//}
//
//pair<bool,int> RoofFacets::isBoundaryNode(const SweepItem& item) {
//	auto a = item.a;
//	auto b = item.b;
//
//	if(a.leftListIdx == NOLIST && a.rightListIdx != NOLIST) {
//		return pair(true,a.rightListIdx);
//	} else if(a.leftListIdx != NOLIST && a.rightListIdx == NOLIST) {
//		return pair(true,a.leftListIdx);
//	} else if(b.leftListIdx == NOLIST && b.rightListIdx != NOLIST) {
//		return pair(true,b.rightListIdx);
//	} else if(b.leftListIdx != NOLIST && b.rightListIdx == NOLIST) {
//		return pair(true,b.leftListIdx);
//	}
//
//	return make_pair(false,NOLIST);
//}

//void RoofFacets::handleEdgeEvent(SweepItem& item) {
//	auto& correctList = allLists[item.a.rightListIdx];
//	correctList.push_back(item.intersectionPoint);
//
//	item.a.rightListIdx = NOLIST;
//	item.b.leftListIdx  = NOLIST;
//}

void RoofFacets::handleCell(SweepEvent *event) {
	auto cont = event->getEventType();
	switch(cont.first) {
	case EventType::EMPTY:   		cout << "EMPTY EVENT ";		  			break;
	case EventType::EDGE:    		event->printAll(); cout << endl;handleEdgeEvent(cont.second); 			break;
	case EventType::SPLIT:   		event->printAll(); cout << endl;handleSplitEvent(cont.second);			break;
	case EventType::CREATE1: 		event->printAll(); cout << endl;handleCreate1Event(cont.second);		break;
	case EventType::CREATE2: 		cout << "ERROR: CREATE2 EVENT"; 		break;
	case EventType::MERGE:   		cout << "ERROR: MERGE EVENT"; 			break;
	case EventType::CREATE1ORENTER: event->printAll(); cout << endl;handleEnterOrCreate1Event(cont.second); break;
	case EventType::CREATE2ORMERGE: event->printAll(); cout << endl;handleMergeOrCreate2Event(cont.second); break;
	case EventType::ENTER:    		event->printAll(); cout << endl;handleEnterEvent(cont.second); 			break;
	}
}

bool RoofFacets::aGreaterB(Point a, Point b, EdgeIterator base) {
	auto aOnBase = base->supporting_line().projection(a);
	auto bOnBase = base->supporting_line().projection(b);
    return base->direction() == Vector(aOnBase - bOnBase).direction();
}



/* first facets on plane, starting at an edge of P (thus index 0) */
void RoofFacets::addBaseCell(ALIterator& line) {
	Point edgeStart(line->base->vertex(0));
	Point edgeEnd(line->base->vertex(1));

	if(line->start == edgeStart) {
		// starts the first facet on a plane
		int facetIdx = numberOfFacets++;
		int listIdx  = allLists.size();
		listToFacet[listIdx] = facetIdx;

		list<Point> l;
		l.push_back(line->start);
		allLists.push_back(l);

		Facet f;
		f.push_back(listIdx);
		allFacets[line->base].push_back(f);

		line->rightListIdx = listIdx;
		cout << "S(" << line->rightListIdx << ") ";
	} else if(line->start == edgeEnd) {
		// still only one facet on the current plane
		auto& facet = allFacets[line->base].front();
		allLists[facet.front()].push_back(line->start);
		line->leftListIdx = facet.front();
		cout << "E(" << line->leftListIdx << ") ";
	} else if(aGreaterB(line->start,edgeStart,line->base) &&
			  aGreaterB(edgeEnd,line->start,line->base)) {
		// just reference from the arrangement lines that start
		// in the interior of the respective edge
		auto& facet = allFacets[line->base].front();
		line->leftListIdx  = facet.front();
		line->rightListIdx = facet.front();
	}
}

void RoofFacets::handleEdgeEvent(SweepEventReturnContainer& event) {
	for(auto& cell : event.boundaryNodes) {
//		cell.print();
		if(cell.a->rightListIdx == cell.b->leftListIdx && cell.b->leftListIdx != NOLIST &&
				( (cell.a->leftListIdx == NOLIST && cell.b->rightListIdx != NOLIST) ||
				  (cell.a->leftListIdx != NOLIST && cell.b->rightListIdx == NOLIST) ) ) {
			/* an arrangement line is leaving the cell */
			if(cell.b->rightListIdx != NOLIST) {
				cell.b->leftListIdx  = NOLIST;
				cell.b->rightListIdx = NOLIST;
			} else {
				cell.a->leftListIdx  = NOLIST;
				cell.a->rightListIdx = NOLIST;
			}
		}

		if(cell.a->rightListIdx == cell.b->leftListIdx && cell.b->leftListIdx != NOLIST &&
		   cell.a->leftListIdx == NOLIST && cell.b->rightListIdx == NOLIST) {

			auto& l = allLists[cell.a->rightListIdx];
			l.push_back(cell.intersectionPoint);
			cell.a->rightListIdx = NOLIST;
			cell.b->leftListIdx  = NOLIST;

		} else if(cell.a->leftListIdx != NOLIST) {

			auto& l = allLists[cell.a->leftListIdx];
			l.push_back(cell.intersectionPoint);
			cell.b->leftListIdx = cell.a->leftListIdx;
			cell.a->leftListIdx = NOLIST;

		} else if(cell.a->rightListIdx != NOLIST) {

			auto& l = allLists[cell.a->rightListIdx];
			l.push_front(cell.intersectionPoint);
			cell.b->rightListIdx = cell.a->rightListIdx;
			cell.a->rightListIdx = NOLIST;

		} else if(cell.b->leftListIdx != NOLIST) {

			auto& l = allLists[cell.b->leftListIdx];
			l.push_front(cell.intersectionPoint);
			cell.a->leftListIdx = cell.b->leftListIdx;
			cell.b->leftListIdx = NOLIST;

		} else if(cell.b->rightListIdx != NOLIST) {

			auto& l = allLists[cell.b->rightListIdx];
			l.push_front(cell.intersectionPoint);
			cell.a->rightListIdx = cell.b->rightListIdx;
			cell.b->rightListIdx = NOLIST;

		} else {
			cout << "Warning: Should not occur!" << endl;
		}
		cell.print();
	}

	cout << "----------------------------------------> edge, ";
}

void RoofFacets::handleSplitEvent(SweepEventReturnContainer& event) {
	for(auto& cell : event.interiorNodes) {
		cout << "INT: "; cell.print();
		int facetIdx = listToFacet[cell.a->rightListIdx];

		cell.a->leftListIdx  = cell.a->rightListIdx;
		cell.b->rightListIdx = cell.b->leftListIdx;

		cell.a->rightListIdx = NOLIST;
		cell.b->leftListIdx  = NOLIST;

		list<Point> l;
		l.push_back(cell.intersectionPoint);
		allLists.push_back(l);
		listToFacet[allLists.size()-1] = facetIdx;
	}

	for(auto& cell : event.boundaryNodes) {
		cell.print();
	}
	cout << "----------------------------------------> split, ";
}

void RoofFacets::handleCreate1Event(SweepEventReturnContainer& event) {
	cout << "----------------------------------------> create1, ";

}

void RoofFacets::handleEnterOrCreate1Event(SweepEventReturnContainer& event) {
	handleEnterEvent(event);
	cout << "----------------------------------------> enter-create1, ";
}

void RoofFacets::handleMergeOrCreate2Event(SweepEventReturnContainer& event) {
	handleMergeEvent(event);
	cout << "----------------------------------------> create2ormerge, ";
}


void RoofFacets::handleMergeEvent(SweepEventReturnContainer& event) {
	for(auto& cell : event.boundaryNodes) {
		if(cell.a->leftListIdx != NOLIST && cell.a->rightListIdx == NOLIST &&
		   cell.b->leftListIdx == NOLIST && cell.b->rightListIdx == NOLIST ) {

			cell.b->leftListIdx  = cell.a->leftListIdx;
			cell.b->rightListIdx = cell.a->leftListIdx;
		} else if(cell.a->leftListIdx == NOLIST && cell.a->rightListIdx == NOLIST &&
		   cell.b->leftListIdx == NOLIST && cell.b->rightListIdx != NOLIST ) {

			cell.a->leftListIdx  = cell.b->rightListIdx;
			cell.a->rightListIdx = cell.b->rightListIdx;
		}
	}
}

void RoofFacets::handleEnterEvent(SweepEventReturnContainer& event) {
	for(auto& cell : event.boundaryNodes) {
		if(cell.a->leftListIdx != NOLIST && cell.a->rightListIdx == NOLIST &&
		   cell.b->leftListIdx == NOLIST && cell.b->rightListIdx == NOLIST ) {

			cell.b->leftListIdx  = cell.a->leftListIdx;
			cell.b->rightListIdx = cell.a->leftListIdx;
		} else if(cell.a->leftListIdx == NOLIST && cell.a->rightListIdx != NOLIST &&
				  cell.b->leftListIdx == NOLIST && cell.b->rightListIdx == NOLIST ) {

			cell.b->leftListIdx  = cell.a->rightListIdx;
			cell.b->rightListIdx = cell.a->rightListIdx;
		} else if(cell.a->leftListIdx == NOLIST && cell.a->rightListIdx == NOLIST &&
				  cell.b->leftListIdx != NOLIST && cell.b->rightListIdx == NOLIST ) {

			cell.a->leftListIdx  = cell.b->leftListIdx;
			cell.a->rightListIdx = cell.b->leftListIdx;
		} else if(cell.a->leftListIdx == NOLIST && cell.a->rightListIdx == NOLIST &&
				  cell.b->leftListIdx == NOLIST && cell.b->rightListIdx != NOLIST ) {

			cell.a->leftListIdx  = cell.b->rightListIdx;
			cell.a->rightListIdx = cell.b->rightListIdx;
		}
		cell.print();
	}
}



void RoofFacets::addPointToNewList(SweepItem& item) {

}

void RoofFacets::addPointToCurrentList(SweepItem& item) {

}

