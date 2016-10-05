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

ostream& operator<<(ostream& os, const PointExt& p) {
	if(p.nextList != NOLIST) {
		os << p.nextList;
	} else {
		os << p.x().doubleValue() << "," << p.y().doubleValue();
	}
	return os;
}


RoofFacets::RoofFacets():minimize(false),maximize(false),polygon(nullptr),
		config(nullptr),sweepLine(nullptr) {}

void RoofFacets::handleEnterLeaveEvent(SweepEvent* event) {
	for(auto& cell : event->getActiveCells()) {
		int numActiveInd = cell->numberOfActiveIndices();
		if(numActiveInd == 1) {
			handleEnterEvent(cell);
		} else if(numActiveInd == 3) {
			handleLeaveEvent(cell);
		}
	}
}

void RoofFacets::handleEvent(SweepEvent* event) {
	auto activeCells =  event->getActiveCells();
	vector<SweepItem> boundaryNodes;
	vector<SweepItem> interiorNodes;

	cout << endl << endl; event->printAll(); cout << " AC(" << activeCells.size() << ") ";
//	if(activeCells.size() > 0) {event->printAll(); cout << " AC: " << activeCells.size() << " - "; }

	if(activeCells.size() >= 3) {
		/* split or edge event */
		if(event->containsEdgeEvent()) {
			handleEdgeEvent(event);
		} else if(event->containsInteriorNode()){
			handleSplitEvent(event);
		} else if(handleVertexEvent(event)) {
			/* divide event is handled in if clause */
		} else {
			handleEnterLeaveEvent(event);
		}

	} else if(activeCells.size() == 2){
		/* possible create event */
		if(!handleCreateEventA(event)) {
			handleEnterLeaveEvent(event);
		}
	} else if(activeCells.size() == 1){
		/* enter and leave events */
		auto cell = activeCells.front();
		int numActiveInd = cell->numberOfActiveIndices();

		if(numActiveInd == 1) {
			handleEnterEvent(cell);
		} else if(numActiveInd == 3) {
			handleLeaveEvent(cell);
		} else if(numActiveInd == 4) {
			if(minimize) {
				if(handleCreateEventB(event)) {
					/* create event is handled in if clause */
				}
			}
		}

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
	cout << "."; fflush(stdout);
	auto itBasePrev = prev(line->base);
	auto itBaseNext = next(line->base);

//	bool isNeighbor = (itBasePrev == line->e || itBaseNext == line->e) ? true : false;

	ArrangementLine s(line->base,itBasePrev);
	ArrangementLine e(line->base,itBaseNext);

//	if(line->start == edgeStart && isNeighbor) {
	if(*line == s) {
		//cout << "S" << line->eid << " ";
		int listIdx  = allLists.size();

		list<PointExt> l;
		l.push_back(line->start);
		allLists.push_back(l);

		allFacets[line->base].push_back(listIdx);
		listToFacet[listIdx] = allFacets[line->base].begin();

		line->rightListIdx  = listIdx;
		zMap[line->start] = 0;
//	} else if(line->start == edgeEnd && isNeighbor ) {
	} else if(*line == e) {
		//cout << "E" << line->eid << " ";

		auto listIdx = (allFacets[line->base].empty()) ? allLists.size() : allLists.size()-1;
		allLists[listIdx].push_back(line->start);
		line->leftListIdx = listIdx;

		zMap[line->start] = 0;
	} else if(
	  (aGreaterB(line->start,edgeStart,line->base) &&
	   aGreaterB(edgeEnd,line->start,line->base))
	   ||
	   (line->start == edgeStart  || line->start == edgeEnd)
	) {
		bool assign = true;

		if(line->start == edgeStart) {
			//ArrangementLine s(line->base,itBasePrev);

			if(s > *line || s == *line) {
				assign = false;
			}
		} else if(line->start == edgeEnd) {
			//ArrangementLine e(line->base,itBaseNext);
			if(*line < e || *line == e) {
				assign = false;
			}
		}

		if(assign) {
			auto listIdx = (allFacets[line->base].empty()) ? allLists.size() : allLists.size()-1 ;
			line->leftListIdx  = listIdx;
			line->rightListIdx = listIdx;
		}
	}
}

void RoofFacets::handleEdgeEvent(SweepEvent* event) {
	cout << "EDGE EVENT!" << endl;

	/* preps for ghost vertices if needed */
	int ghostLeftIdx, ghostRightIdx;
	SweepItem* cellA = nullptr;
	SweepItem* cellB = nullptr;

	ghostLeftIdx = ghostRightIdx = NOLIST;

	auto cells = event->getActiveCells();
	vector<SweepItem*> colinearCells;
	for(auto cella = cells.begin(); cella != cells.end(); ++cella) {
		for(auto cellb = std::next(cella); cellb != cells.end(); ++cellb) {
			if((*cella)->base->supporting_line() == (*cellb)->base->supporting_line()) {
				colinearCells.push_back(*cella);
				colinearCells.push_back(*cellb);
			}
		}
	}

	if(colinearCells.size() > 1) {
		cellA = *cells.begin();
		cellB = *(cells.begin()+1);
//		if(cellA < cellB) {
//			cout << "A < B: "; cellA->print(); cellB->print();
//			ghostLeftIdx  = (cellA->a->leftListIdx  != NOLIST) ? cellA->a->leftListIdx  : NOLIST;
//			ghostLeftIdx  = (cellA->a->rightListIdx != NOLIST) ? cellA->a->rightListIdx : ghostLeftIdx;
//			ghostLeftIdx  = (cellA->b->leftListIdx  != NOLIST) ? cellA->b->leftListIdx  : ghostLeftIdx;
//			ghostLeftIdx  = (cellA->b->rightListIdx != NOLIST) ? cellA->b->rightListIdx : ghostLeftIdx;
//			ghostRightIdx = (cellB->a->leftListIdx  != NOLIST) ? cellB->a->leftListIdx  : NOLIST;
//			ghostRightIdx = (cellB->a->rightListIdx != NOLIST) ? cellB->a->rightListIdx : ghostRightIdx;
//			ghostRightIdx = (cellB->b->leftListIdx  != NOLIST) ? cellB->b->leftListIdx  : ghostRightIdx;
//			ghostRightIdx = (cellB->b->rightListIdx != NOLIST) ? cellB->b->rightListIdx : ghostRightIdx;
//		} else {
//			cout << "A >= B: "; cellA->print(); cellB->print();
//			ghostRightIdx = (cellA->a->leftListIdx  != NOLIST) ? cellA->a->leftListIdx  : NOLIST;
//			ghostRightIdx = (cellA->a->rightListIdx != NOLIST) ? cellA->a->rightListIdx : ghostLeftIdx;
//			ghostRightIdx = (cellA->b->leftListIdx  != NOLIST) ? cellA->b->leftListIdx  : ghostLeftIdx;
//			ghostRightIdx = (cellA->b->rightListIdx != NOLIST) ? cellA->b->rightListIdx : ghostLeftIdx;
//			ghostLeftIdx  = (cellB->a->leftListIdx  != NOLIST) ? cellB->a->leftListIdx  : NOLIST;
//			ghostLeftIdx  = (cellB->a->rightListIdx != NOLIST) ? cellB->a->rightListIdx : ghostRightIdx;
//			ghostLeftIdx  = (cellB->b->leftListIdx  != NOLIST) ? cellB->b->leftListIdx  : ghostRightIdx;
//			ghostLeftIdx  = (cellB->b->rightListIdx != NOLIST) ? cellB->b->rightListIdx : ghostRightIdx;
//		}
	}
	/* end ghost vertex prep */

	/* standard edge event handling */
	for(auto cell : cells) {
		if(cell->a->rightListIdx != NOLIST  &&  cell->b->leftListIdx  != NOLIST &&
		   cell->a->leftListIdx  == NOLIST  &&  cell->b->rightListIdx == NOLIST) {
			/* facet ends locally (edge event) */
			auto& leftList = allLists[cell->a->rightListIdx];
			if(cell->a->rightListIdx != cell->b->leftListIdx) {
				auto& rightList = allLists[cell->b->leftListIdx];

				rightList.push_back(cell->intersectionPoint);
				// adds a next index to find the corresponding following list
				rightList.push_back(cell->a->rightListIdx);

			} else {
				leftList.push_back(cell->intersectionPoint);
			}

			cell->a->rightListIdx = NOLIST;
			cell->b->leftListIdx  = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else if(cell->a->rightListIdx == NOLIST  &&  cell->b->leftListIdx  == NOLIST &&
		          cell->a->leftListIdx  != NOLIST  &&  cell->b->rightListIdx != NOLIST) {
			/* facet joins with facet of a create event OR parallel AL */
			auto& leftList    = allLists[cell->a->leftListIdx];
			auto rightListIdx = cell->b->rightListIdx;

			if((cell->a->parallel || cell->b->parallel) && cell->a->leftListIdx == cell->b->rightListIdx) {
				cout << " (PAR) ";
				leftList.push_back(cell->intersectionPoint);
			} else {
				if(cell->a->leftListIdx == cell->b->rightListIdx) {
					cout << "ERROR: index equal!" << endl;
				}

				leftList.push_back(cell->intersectionPoint);
				/* adds a next index to find the corresponding following list */
				leftList.push_back(rightListIdx);

				/* we also have to remove the 2nd facet as it joined up */
				auto it = listToFacet[rightListIdx];
				allFacets[cell->base].erase(it);
				listToFacet.erase(rightListIdx);
			}


			cell->a->leftListIdx  = NOLIST;
			cell->b->rightListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else if(cell->numberOfActiveIndices() == 3) {
			if(cell->a->leftListIdx == NOLIST) {

				auto& l = allLists[cell->a->rightListIdx];
				l.push_front(cell->intersectionPoint);
				cell->a->leftListIdx  = cell->a->rightListIdx;
				cell->b->leftListIdx  = NOLIST;

				zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
			} else if(cell->b->rightListIdx == NOLIST) {

				auto& l = allLists[cell->b->leftListIdx];
				l.push_back(cell->intersectionPoint);

				cell->b->rightListIdx  = cell->b->leftListIdx;
				cell->a->rightListIdx  = NOLIST;

				zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
			}
		} else if(cell->a->leftListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->b->leftListIdx = cell->a->leftListIdx;
			cell->a->leftListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else if(cell->a->rightListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->b->rightListIdx = cell->a->rightListIdx;
			cell->a->rightListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else if(cell->b->leftListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->a->leftListIdx = cell->b->leftListIdx;
			cell->b->leftListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else if(cell->b->rightListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->a->rightListIdx = cell->b->rightListIdx;
			cell->b->rightListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else {
			cout << "Warning: Should not occur!" << endl;
		}
		cell->print();
	}
	/* end standard edge event handling */

	/* ghost vertex hanling if needed */
	// TODO: verify this:
	/* I guess this can be at most two cells, at an edge event (for now at least) */
	if(colinearCells.size() > 1) {
		cout << "Ghost 'cells' (" << colinearCells.size() << "): ";

		if(colinearCells.size() > 2) {cout << "WARNING: more than two cells meet with parallel baseline!" << endl;}

		if(cellA->a->start != cellB->a->start) {

			/* ghost vertex adds additional AL, thus additional events */
			SweepEvent ghostCells;

			for(auto cell : colinearCells) {
				/* previous bisectors now go into the facets interior */
				if(cell->a->leftListIdx != NOLIST)       {cell->a->rightListIdx = cell->a->leftListIdx;}
				else if(cell->a->rightListIdx != NOLIST) {cell->a->leftListIdx  = cell->a->rightListIdx;}
				else if(cell->b->leftListIdx  != NOLIST) {cell->b->rightListIdx = cell->b->leftListIdx; }
				else if(cell->b->rightListIdx != NOLIST) {cell->b->leftListIdx  = cell->b->rightListIdx;}

				sweepLine->insertGhostVertex(cell,ghostCells);
			}

			handleGhostInsert(&ghostCells);

//			for(auto cell : ghostCells.getActiveCells()) {
//				if(cell->a->ghost) {
//					cell->a->leftListIdx  = ghostLeftIdx;
//					cell->a->rightListIdx = ghostRightIdx;
//				}
//				if(cell->b->ghost) {
//					cell->b->leftListIdx  = ghostLeftIdx;
//					cell->b->rightListIdx = ghostRightIdx;
//				}
//			}

			ghostCells.printAll();
		} else {
			// TODO: edge event (ref,conv,ref) merges wavefront edges, no outgoing arc, thus, no ghost vertex
		}

	}

	cout << "----------------------------------------> edge, ";
}

void RoofFacets::handleGhostInsert(SweepEvent* event) {
	for(auto cell : event->getAllCells()) {
		/* intersection point is already added, list indices have to be added though */
		if(cell->a->leftListIdx  != NOLIST) {cell->b->leftListIdx   = cell->a->leftListIdx; }
		if(cell->b->rightListIdx != NOLIST) {cell->a->rightListIdx  = cell->b->rightListIdx;}
		cell->print();
		if(cell->a->ghost || cell->b->ghost) {
			cout << "has ghost vertex";
		} else {
			cout << "no ghost?";
		}
	}
	cout << endl;
}

void RoofFacets::handleSplitEvent(SweepEvent* event) {
	cout << "SPLIT EVENT!" << endl;

	for(auto cell : event->getActiveCells()) {
		if(cell->isInteriorNode()) {
			auto listIdx = allLists.size();

			list<PointExt> l;
			l.push_back(cell->intersectionPoint);
			allLists.push_back(l);

//			auto itToLast = allFacets[cell->base].end();
//			itToLast--;
//			listToFacet[listIdx] = itToLast;

			cell->a->rightListIdx = listIdx;
			cell->b->leftListIdx  = listIdx;

			cell->a->leftListIdx  = NOLIST;
			cell->b->rightListIdx = NOLIST;

			zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
		} else {
			if(cell->a->leftListIdx != NOLIST) {
				cell->b->leftListIdx = cell->a->leftListIdx;
				cell->a->leftListIdx = NOLIST;
				addPointToCurrentList(cell);
			} else if(cell->b->rightListIdx != NOLIST){
				cell->a->rightListIdx = cell->b->rightListIdx;
				cell->b->rightListIdx = NOLIST;
				addPointToCurrentList(cell);
			}
		}

		cell->print();
	}

	cout << "----------------------------------------> split, ";
}

bool RoofFacets::handleCreateEventA(SweepEvent* event) {
	bool createEvent = false, max = false, min = false;

	if(!minimize && !maximize) {
		return false;
	}

	SweepItem* c_a=nullptr;
	SweepItem* c_b=nullptr;
	SweepItem* c_new;

	for(auto cell : event->getAllCells()) {
		if(cell->isEmptyNode()) {
			c_new = cell;
		} else {
			if(c_a == nullptr) {
				c_a = cell;
			} else {
				c_b = cell;
			}
		}
	}

	auto l_a   = c_a->base->supporting_line().to_vector();
	auto l_b   = c_b->base->supporting_line().to_vector();
	auto l_new = c_new->base->supporting_line().to_vector();

	/* l_a and l_b have to intersect at a reflex vertex, thus must be a right turn */
	//if(CGAL::orientation(l_a,l_b) != CGAL::RIGHT_TURN) {
	if(c_b->b->rightListIdx == NOLIST) {
		if(c_a->b->rightListIdx == NOLIST) {
			return createEvent;
		}

		swap(l_a,l_b);
		auto tmp = c_a;
		c_a = c_b;
		c_b = tmp;
	}

	if(CGAL::orientation(l_a,l_b) == CGAL::RIGHT_TURN) {
		cout << " right turn ";
	} else {
		cout << " left turn ";
		return createEvent;
	}
//	c_a->print(); c_b->print();

	/* analyze if a create event occurs and what typ (min/max) */
	if(CGAL::orientation(l_a,l_new) == CGAL::RIGHT_TURN &&
	   CGAL::orientation(l_b,l_new) == CGAL::RIGHT_TURN) {
		createEvent = true;
		min 		= true;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::RIGHT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::LEFT_TURN) {
		createEvent = true;
		max 		= true;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::LEFT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::RIGHT_TURN) {
		createEvent = false;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::LEFT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::LEFT_TURN) {
		createEvent = true;
		min 		= true;
	} else{
		throw runtime_error("ERROR: handleCreateEventA: orientation?!");
	}

	if(createEvent) {
		if((min && minimize) || (max && maximize)) {
			cout << "CREATE EVENT A (min:" << min << ",max:" << max << ") ";

			/* modify facets of line a and b */
			addPointToCurrentList(c_a);
			turnLefRightOnIntersection(c_a);

			addPointToCurrentList(c_b);
			turnLefRightOnIntersection(c_b);


			/* create new facet and modify new-cell accordingly */
			list<PointExt> l;
			l.push_back(c_new->intersectionPoint);
			auto listIdx = allLists.size();
			allLists.push_back(l);

			/* adds entry to this new facet, may be joined later */
			allFacets[c_new->base].push_back(listIdx);
			auto it = allFacets[c_new->base].end();
			it--;
			listToFacet[listIdx] = it;


			c_new->a->leftListIdx  = listIdx;
			c_new->b->rightListIdx = listIdx;

		} else {
			createEvent = false;
		}
	}
	event->printAll();
	return createEvent;
}

bool RoofFacets::handleCreateEventB(SweepEvent* event) {
	bool createEvent = false;

	if(!minimize) { return createEvent; }

	SweepItem* c_base=nullptr;
	SweepItem* c_a=nullptr;
	SweepItem* c_b=nullptr;

	for(auto cell : event->getAllCells()) {
		if(cell->isEmptyNode()) {
			if(c_a == nullptr) {
				c_a = cell;
			} else {
				c_b = cell;
			}
		} else {
			c_base = cell;
		}
	}

	auto l_a    = c_a->base->supporting_line().to_vector();
	auto l_b    = c_b->base->supporting_line().to_vector();
	auto l_base = c_base->base->supporting_line().to_vector();

	/* l_a and l_b have to intersect at a reflex vertex, thus must be a right turn */
	if(CGAL::orientation(l_a,l_b) != CGAL::RIGHT_TURN) {
		swap(l_a,l_b);
		auto tmp = c_a;
		c_a = c_b;
		c_b = tmp;
	}

	/* analyze if a create event occurs and what typ (min/max) */
	if(CGAL::orientation(l_base,l_a) == CGAL::LEFT_TURN &&
	   CGAL::orientation(l_b,l_base) == CGAL::LEFT_TURN) {
		createEvent = true;
//	} else if(CGAL::orientation(l_a,l_base) == CGAL::RIGHT_TURN &&
//			  CGAL::orientation(l_b,l_base) == CGAL::LEFT_TURN) {
//		createEvent = true;
//	} else if(CGAL::orientation(l_a,l_base) == CGAL::LEFT_TURN &&
//			  CGAL::orientation(l_b,l_base) == CGAL::RIGHT_TURN) {
//		createEvent = false;
//	} else if(CGAL::orientation(l_a,l_base) == CGAL::LEFT_TURN &&
//			  CGAL::orientation(l_b,l_base) == CGAL::LEFT_TURN) {
//		createEvent = true;
//	} else{
//		throw runtime_error("ERROR: handleCreateEventA: orientation?!");
	}

	if(createEvent) {
			cout << "CREATE EVENT B ";

			/* modify base facet */
			auto baseListIdx = allLists.size();
			list<PointExt> l;
			l.push_back(c_base->intersectionPoint);
			allLists.push_back(l);

			c_base->b->leftListIdx  = baseListIdx;
			c_base->a->rightListIdx = baseListIdx;
			c_base->b->rightListIdx = NOLIST;
			c_base->a->leftListIdx  = NOLIST;


			/* add facet to the other two arrangements */
			list<PointExt> l_a;
			l_a.push_back(c_a->intersectionPoint);
			auto listIdxA = allLists.size();
			allLists.push_back(l_a);

			/* adds entry to this new facet, may be joined later */
			allFacets[c_a->base].push_back(listIdxA);
			auto itA = allFacets[c_a->base].end();
			itA--;
			listToFacet[listIdxA] = itA;

			c_a->a->leftListIdx  = listIdxA;
			c_a->b->rightListIdx = listIdxA;


			list<PointExt> l_b;
			l_b.push_back(c_a->intersectionPoint);
			auto listIdxB = allLists.size();
			allLists.push_back(l_b);

			/* adds entry to this new facet, may be joined later */
			allFacets[c_b->base].push_back(listIdxB);
			auto itB = allFacets[c_b->base].end();
			itB--;
			listToFacet[listIdxB] = itB;

			c_b->a->leftListIdx  = listIdxB;
			c_b->b->rightListIdx = listIdxB;
	}
	event->printAll();
	return createEvent;
}

bool RoofFacets::handleVertexEvent(SweepEvent* event) {
	auto divideNodes = event->getActiveCells();
	if(event->numberDivideNodes() >= 1) {
		cout << "Vertex/Divide EVENT ";
		for(auto cell : divideNodes) {

			if(cell->isPossibleDivideNode()) {
				auto& la = allLists[cell->a->leftListIdx];
				la.push_back(cell->intersectionPoint);

				if(cell->a->leftListIdx != cell->b->rightListIdx) {
					auto& lb = allLists[cell->b->rightListIdx];
					lb.push_front(cell->intersectionPoint);
				}

				zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();

				cell->a->rightListIdx = cell->b->rightListIdx;
				cell->b->leftListIdx  = cell->a->leftListIdx;
				cell->a->leftListIdx  = NOLIST;
				cell->b->rightListIdx = NOLIST;
			} else {
				addPointToCurrentList(cell);
				turnLefRightOnIntersection(cell);
			}
		}

		event->printAll();
		return true;
	}

	return false;
}


void RoofFacets::handleMergeEvent(SweepEvent* event) {
	for(auto cell : event->getActiveCells()) {
		if(cell->a->leftListIdx != NOLIST && cell->a->rightListIdx == NOLIST &&
		   cell->b->leftListIdx == NOLIST && cell->b->rightListIdx == NOLIST ) {

			cell->b->leftListIdx  = cell->a->leftListIdx;
			cell->b->rightListIdx = cell->a->leftListIdx;
		} else if(cell->a->leftListIdx == NOLIST && cell->a->rightListIdx == NOLIST &&
		   cell->b->leftListIdx == NOLIST && cell->b->rightListIdx != NOLIST ) {

			cell->a->leftListIdx  = cell->b->rightListIdx;
			cell->a->rightListIdx = cell->b->rightListIdx;
		}
	}
}


void RoofFacets::turnLefRightOnIntersection(SweepItem* cell) {
	if(cell->numberOfActiveIndices() == 3) {
		if(cell->a->leftListIdx == NOLIST) {
			cell->a->leftListIdx = cell->a->rightListIdx;;
			cell->b->leftListIdx = NOLIST;
		} else if(cell->b->rightListIdx == NOLIST) {
			cell->b->rightListIdx = cell->b->leftListIdx;;
			cell->a->rightListIdx = NOLIST;
		}
	} else if(cell->a->leftListIdx != NOLIST) {
		cell->b->leftListIdx = cell->a->leftListIdx;
		cell->a->leftListIdx = NOLIST;
	} else if(cell->b->rightListIdx != NOLIST) {
		cell->a->rightListIdx = cell->b->rightListIdx;
		cell->b->rightListIdx = NOLIST;
	}
}


void RoofFacets::handleEnterEvent(SweepItem* cell) {
		if(cell->a->leftListIdx != NOLIST && cell->a->rightListIdx == NOLIST &&
		   cell->b->leftListIdx == NOLIST && cell->b->rightListIdx == NOLIST ) {

			cell->b->leftListIdx  = cell->a->leftListIdx;
			cell->b->rightListIdx = cell->a->leftListIdx;
		} else if(cell->a->leftListIdx == NOLIST && cell->a->rightListIdx != NOLIST &&
				  cell->b->leftListIdx == NOLIST && cell->b->rightListIdx == NOLIST ) {

			cell->b->leftListIdx  = cell->a->rightListIdx;
			cell->b->rightListIdx = cell->a->rightListIdx;
		} else if(cell->a->leftListIdx == NOLIST && cell->a->rightListIdx == NOLIST &&
				  cell->b->leftListIdx != NOLIST && cell->b->rightListIdx == NOLIST ) {

			cell->a->leftListIdx  = cell->b->leftListIdx;
			cell->a->rightListIdx = cell->b->leftListIdx;
		} else if(cell->a->leftListIdx == NOLIST && cell->a->rightListIdx == NOLIST &&
				  cell->b->leftListIdx == NOLIST && cell->b->rightListIdx != NOLIST ) {

			cell->a->leftListIdx  = cell->b->rightListIdx;
			cell->a->rightListIdx = cell->b->rightListIdx;
		}
		cell->print();
}

void RoofFacets::handleLeaveEvent(SweepItem* cell) {
	if(cell->a->leftListIdx == NOLIST) {
		cell->b->leftListIdx  = NOLIST;
		cell->b->rightListIdx = NOLIST;
	} else {
		cell->a->leftListIdx  = NOLIST;
		cell->a->rightListIdx = NOLIST;
	}
	cell->print();
}


void RoofFacets::addPointToCurrentList(SweepItem* cell) {
	bool pushEnd = (cell->a->leftListIdx != NOLIST || cell->b->leftListIdx != NOLIST) ? true : false;
	auto& l = allLists[cell->firstListIndex()];

	if(pushEnd) {
		l.push_back(cell->intersectionPoint);
	} else {
		l.push_front(cell->intersectionPoint);
	}

	zMap[cell->intersectionPoint] = cell->normalDistance.doubleValue();
}

EdgeIterator RoofFacets::next(EdgeIterator i) {
	return (i+1 == polygon->edges_end()) ? polygon->edges_begin() : i+1;
}
EdgeIterator RoofFacets::prev(EdgeIterator i) {
	return (i == polygon->edges_begin()) ? polygon->edges_end()-1 : i-1;
}


void RoofFacets::printAllLists() {
	cout << endl << "facet: coordinate list" << endl;
	for(auto& facet : allFacets) {
		for(auto f : facet.second) {
			auto list = &allLists[f];
			cout << f << ":";
			if(list->empty()) continue;

			auto listIt = list->begin();
			do {
				// cout << *listIt << "(" << listIt->nextList << ") - ";
				if(listIt->nextList != NOLIST) {
					list = &allLists[listIt->nextList];
					listIt = list->begin();
				} else {
					cout << " " << *listIt;
					++listIt;
				}
			} while(listIt != list->end() && listIt->nextList != f);
			cout <<  endl;
		}
	}
}

