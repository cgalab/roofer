/*
 * roofer is written in C++ and uses CGAL.  It computes straight skeleton roofs
 * as well as minimum- and maximum-volume roofs over a simple polygon.
 * Copyright (C) 2016 - Günther Eder - roofer@geder.at
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Facets.h"


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

	LOG_IF(config->verbose,INFO) << endl << *event << " AC(" << activeCells.size() << ") ";

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
		if(handlePossibleGhostVertexEnd(event)) {
			/* possible ghost vertex end */
			/* degree two node due to ending ghost vertex */
		} else if(handleCreateEventA(event)) {
			/* possible create event */
			/* all done in function call */
		} else {
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

	auto itBasePrev = prev(line->base);
	auto itBaseNext = next(line->base);

	ArrangementLine s(line->base,itBasePrev);
	ArrangementLine e(line->base,itBaseNext);

	if(*line == s) {
		int listIdx  = allLists.size();

		list<PointExt> l;
		l.push_back(line->start);
		allLists.push_back(l);

		allFacets[line->base].push_back(listIdx);
		listToFacet[listIdx] = allFacets[line->base].begin();

		line->rightListIdx  = listIdx;

		insertToZmap(line->start,0);
	} else if(*line == e) {

		auto listIdx = (allFacets[line->base].empty()) ? allLists.size() : allLists.size()-1;
		allLists[listIdx].push_back(line->start);
		line->leftListIdx = listIdx;

		insertToZmap(line->start,0);
	} else if(
	  (aGreaterB(line->start,edgeStart,line->base) &&
	   aGreaterB(edgeEnd,line->start,line->base))
	   ||
	   (line->start == edgeStart  || line->start == edgeEnd)
	) {
		bool assign = true;

		if(line->start == edgeStart) {

			if(s > *line || s == *line) {
				assign = false;
			}
		} else if(line->start == edgeEnd) {

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
	LOG_IF(config->verbose,INFO) << "EDGE EVENT!";

	/* preps for ghost vertices if needed */
	SweepItem* cellA = nullptr;
	SweepItem* cellB = nullptr;

	auto cells = event->getActiveCells();
	auto colinearCells = checkColinearCells(cells);

	if(colinearCells.size() > 1) {
		cellA = *colinearCells.begin();
		cellB = *(colinearCells.begin()+1);
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

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else if(cell->a->rightListIdx == NOLIST  &&  cell->b->leftListIdx  == NOLIST &&
		          cell->a->leftListIdx  != NOLIST  &&  cell->b->rightListIdx != NOLIST) {
			/* facet joins with facet of a create event OR parallel AL */
			auto& leftList    = allLists[cell->a->leftListIdx];
			auto rightListIdx = cell->b->rightListIdx;

			if((cell->a->parallel || cell->b->parallel) && cell->a->leftListIdx == cell->b->rightListIdx) {
				LOG_IF(config->verbose,INFO) << " (PAR) ";
				leftList.push_back(cell->intersectionPoint);
			} else {
				if(cell->a->leftListIdx == cell->b->rightListIdx) {
					LOG_IF(config->verbose,INFO) << "ERROR: index equal!";
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

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else if(cell->numberOfActiveIndices() == 3) {
			if(cell->a->leftListIdx == NOLIST) {

				auto& l = allLists[cell->a->rightListIdx];
				l.push_front(cell->intersectionPoint);
				cell->a->leftListIdx  = cell->a->rightListIdx;
				cell->b->leftListIdx  = NOLIST;

				insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
			} else if(cell->b->rightListIdx == NOLIST) {

				auto& l = allLists[cell->b->leftListIdx];
				l.push_back(cell->intersectionPoint);

				cell->b->rightListIdx  = cell->b->leftListIdx;
				cell->a->rightListIdx  = NOLIST;

				insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
			}
		} else if(cell->a->leftListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->b->leftListIdx = cell->a->leftListIdx;
			cell->a->leftListIdx = NOLIST;

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else if(cell->a->rightListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->b->rightListIdx = cell->a->rightListIdx;
			cell->a->rightListIdx = NOLIST;

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else if(cell->b->leftListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->a->leftListIdx = cell->b->leftListIdx;
			cell->b->leftListIdx = NOLIST;

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else if(cell->b->rightListIdx != NOLIST) {

			addPointToCurrentList(cell);
			cell->a->rightListIdx = cell->b->rightListIdx;
			cell->b->rightListIdx = NOLIST;

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else {
			LOG_IF(config->verbose,INFO) << "Warning: Should not occur!";
		}
	}
	LOG_IF(config->verbose,INFO) << *event;
	/* end standard edge event handling */

	/* ghost vertex hanling if needed */
	// TODO: verify this:
	/* I guess this can be at most two cells, at an edge event (for now at least) */
	if(colinearCells.size() > 1) {
		LOG_IF(config->verbose,INFO) << "Ghost 'cells' (" << colinearCells.size() << "): "  << endl;

		if(colinearCells.size() > 2) {LOG_IF(config->verbose,INFO) << "WARNING: more than two cells meet with parallel baseline!";}

		if(cellA->base != cellB->base) {

			if(minimize) {
				/* create event! */
				handleCreateEventB(event);
			} else {
				/* ghost vertex adds additional AL, thus additional events */
				SweepEvent ghostCells;

				for(auto cell : colinearCells) {
					/* previous bisectors now go into the facets interior */

					auto ghostVertIt = sweepLine->insertGhostVertex(cell,ghostCells);

					if(cell->a->leftListIdx != NOLIST)       {cell->a->rightListIdx = cell->a->leftListIdx; }
					else if(cell->a->rightListIdx != NOLIST) {cell->a->leftListIdx  = cell->a->rightListIdx;}
					else if(cell->b->leftListIdx  != NOLIST) {cell->b->rightListIdx = cell->b->leftListIdx; }
					else if(cell->b->rightListIdx != NOLIST) {cell->b->leftListIdx  = cell->b->rightListIdx;}

					/* add correct list indices for ghost vertex */
					if(cell->a->leftListIdx != NOLIST) {
						ghostVertIt->rightListIdx = cell->a->leftListIdx;
					} else if(cell->b->leftListIdx != NOLIST) {
						ghostVertIt->leftListIdx = cell->b->leftListIdx;
					}
					LOG_IF(config->verbose,INFO) << " ghost: " << ghostVertIt->eid << " L/R " << ghostVertIt->leftListIdx << "/" << ghostVertIt->rightListIdx << endl;
				}
			}

			LOG_IF(config->verbose,INFO) << *event;

		} else {
			// TODO: edge event (ref,conv,ref) merges wavefront edges, no outgoing arc, thus, no ghost vertex
		}

	}

	LOG_IF(config->verbose,INFO) << "----------------------------------------> edge, ";
}

void RoofFacets::handleGhostInsert(SweepEvent* event) {
	for(auto cell : event->getAllCells()) {
		/* intersection point is already added, list indices have to be added though */
		// TODO maybe check where we are AL to start of ghost vertex

		if(cell->b->ghost) {
			if(*cell->a < cell->intersectionPoint) {
				cell->b->rightListIdx = cell->a->leftListIdx;
			} else {
				cell->b->leftListIdx = cell->a->leftListIdx;
			}
		} else if(cell->a->ghost) {
			if(*cell->b < cell->intersectionPoint) {
				cell->a->rightListIdx = cell->b->leftListIdx;
			} else {
				cell->a->leftListIdx = cell->b->leftListIdx;
			}
		}
	}
	LOG_IF(config->verbose,INFO) << *event;
}

void RoofFacets::handleSplitEvent(SweepEvent* event) {
	LOG_IF(config->verbose,INFO) << "SPLIT EVENT!";

	for(auto cell : event->getActiveCells()) {
		if(cell->isInteriorNode()) {
			auto listIdx = allLists.size();

			list<PointExt> l;
			l.push_back(cell->intersectionPoint);
			allLists.push_back(l);

			cell->a->rightListIdx = listIdx;
			cell->b->leftListIdx  = listIdx;

			cell->a->leftListIdx  = NOLIST;
			cell->b->rightListIdx = NOLIST;

			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
		} else {
			if(cell->a->leftListIdx != NOLIST) {
				cell->b->leftListIdx = cell->a->leftListIdx;
				cell->a->leftListIdx = NOLIST;
				addPointToCurrentList(cell);
			} else if(cell->a->parallel && cell->a->rightListIdx != NOLIST) {
				cell->b->rightListIdx = cell->a->rightListIdx;
				cell->a->rightListIdx = NOLIST;
				addPointToCurrentList(cell);
			} else if(cell->b->rightListIdx != NOLIST){
				cell->a->rightListIdx = cell->b->rightListIdx;
				cell->b->rightListIdx = NOLIST;
				addPointToCurrentList(cell);
			}
		}
	}

	LOG_IF(config->verbose,INFO) << *event << "----------------------------------------> split, ";
}

bool RoofFacets::handlePossibleGhostVertexEnd(SweepEvent* event) {
	int numGhostVertices = 0;
	auto activeCells = event->getActiveCells();
	for(auto cell : activeCells) {
		if(cell->hasGhostVertex()) {
			++numGhostVertices;
		}
	}

	if(numGhostVertices == 2) {
		LOG_IF(config->verbose,INFO) << "Ghost Vertex deg-2 node ";
		for(auto cell : activeCells) {
			/* add point to facet list */
			addPointToCurrentList(cell);

			/* adapt list indices */
			if(cell->a->ghost) {
				if(cell->b->leftListIdx == NOLIST && cell->b->rightListIdx == NOLIST) {
					if(cell->a->leftListIdx != NOLIST) {
						cell->b->leftListIdx = cell->a->leftListIdx;
					} else {
						cell->b->rightListIdx = cell->a->rightListIdx;
					}
				} else {
					cell->b->leftListIdx = cell->b->rightListIdx = NOLIST;
				}
				cell->a->leftListIdx = cell->a->rightListIdx = NOLIST;
			} else if(cell->b->ghost) {
				if(cell->a->leftListIdx == NOLIST && cell->a->rightListIdx == NOLIST) {
					if(cell->b->leftListIdx != NOLIST) {
						cell->a->leftListIdx = cell->b->leftListIdx;
					} else {
						cell->a->rightListIdx = cell->b->rightListIdx;
					}
				} else {
					cell->a->leftListIdx = cell->a->rightListIdx = NOLIST;
				}
				cell->b->leftListIdx = cell->b->rightListIdx = NOLIST;
			}

			/* add point to z-Map for simpler 3D reconstucition */
			insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());

			/* remove ghost vertex from local sweep line status */
			auto gv = (cell->a->ghost) ? cell->a : cell->b;
			sweepLine->deleteGhostVertex(cell,gv);
		}
		LOG_IF(config->verbose,INFO) << *event;

		return true;
	}

	return false;
}

bool RoofFacets::handleCreateEventA(SweepEvent* event) {
	bool createEvent = false, max = false, min = false;

	if(!minimize && !maximize) {
		return createEvent;
	}

	SweepItem* c_a=nullptr;
	SweepItem* c_b=nullptr;
	SweepItem* c_new=nullptr;

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

	if(c_a == nullptr || c_b == nullptr || c_new == nullptr) {return false;}

	auto l_a   = c_a->base->supporting_line().to_vector();
	auto l_b   = c_b->base->supporting_line().to_vector();
	auto l_new = c_new->base->supporting_line().to_vector();

	/* l_a and l_b have to intersect at a reflex vertex, thus must be a right turn */
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
		LOG_IF(config->verbose,INFO) << " right turn ";
	} else {
		LOG_IF(config->verbose,INFO) << " left turn ";
		return createEvent;
	}

	/* analyze if a create event occurs and what typ (min/max) */
	if(CGAL::orientation(l_a,l_new) == CGAL::RIGHT_TURN &&
	   CGAL::orientation(l_b,l_new) == CGAL::RIGHT_TURN) {
		LOG_IF(config->verbose,INFO) << " a ";
		createEvent = true;
		min 		= true;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::RIGHT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::LEFT_TURN) {
		LOG_IF(config->verbose,INFO) << " b ";
		createEvent = true;
		max 		= true;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::LEFT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::RIGHT_TURN) {
		LOG_IF(config->verbose,INFO) << " c ";
		createEvent = false;
	} else if(CGAL::orientation(l_a,l_new) == CGAL::LEFT_TURN &&
			  CGAL::orientation(l_b,l_new) == CGAL::LEFT_TURN) {
		LOG_IF(config->verbose,INFO) << " d ";
		createEvent = true;
		min 		= true;
	} else{
		//throw runtime_error("ERROR: handleCreateEventA: orientation?!");
		LOG_IF(config->verbose,INFO) << "ERROR: handleCreateEventA: orientation?";
		return createEvent;
	}

	if(createEvent) {
		if((min && minimize) || (max && maximize)) {
			LOG_IF(config->verbose,INFO) << "CREATE EVENT A (min:" << min << ",max:" << max << ") ";

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
	LOG_IF(config->verbose,INFO) << *event;
	return createEvent;
}

bool RoofFacets::handleCreateEventB(SweepEvent* event) {
	bool createEvent = false;

	if(!minimize) { return createEvent; }

	SweepItem* c_base  = nullptr;
	SweepItem* c_a     = nullptr;
	SweepItem* c_b	   = nullptr;

	/* in case there is a parallel counterpart */
	SweepItem* c_baseP = nullptr;

	for(auto cell : event->getAllCells()) {
		if(cell->isEmptyNode()) {
			if(c_a == nullptr) {
				c_a = cell;
			} else {
				c_b = cell;
			}
		} else if(c_base == nullptr) {
			c_base = cell;
		} else {
			c_baseP = cell;
		}
	}

	auto l_a     = c_a->base->supporting_line();
	auto l_b     = c_b->base->supporting_line();
	auto l_base  = c_base->base->supporting_line().to_vector();
	Vector l_baseP = (c_baseP != nullptr) ? c_baseP->base->supporting_line().to_vector() : Vector(INFPOINT.x(),INFPOINT.y());

	if(l_baseP != Vector(INFPOINT.x(),INFPOINT.y())) {
		if(c_baseP->a < c_base->a) {
			swap(l_base,l_baseP);
			auto tmp = c_base;
			c_base = c_baseP;
			c_baseP = tmp;
			LOG_IF(config->verbose,INFO) << " swapped ";
		}
	}

	/* l_a and l_b have to intersect at a reflex vertex, thus must be a right turn */
	if(CGAL::orientation(l_a.to_vector(),l_b.to_vector()) != CGAL::RIGHT_TURN && CGAL::orientation(l_a.to_vector(),l_b.to_vector()) != CGAL::COLLINEAR) {
		swap(l_a,l_b);
		auto tmp = c_a;
		c_a = c_b;
		c_b = tmp;
	}

	auto normalA = l_a.perpendicular(c_a->intersectionPoint).to_vector();
	auto normalB = l_b.perpendicular(c_b->intersectionPoint).to_vector();

	/* analyze if a create event occurs and what typ (min/max) */
	if(Line(c_base->base->supporting_line()).has_on_positive_side(c_base->intersectionPoint+normalA) &&
	   Line(c_base->base->supporting_line()).has_on_positive_side(c_base->intersectionPoint+normalB) ) {
		//createEvent = true;
	//} else
		if(CGAL::orientation(l_base,l_a.to_vector()) == CGAL::LEFT_TURN &&
	       CGAL::orientation(l_b.to_vector(),l_base) == CGAL::LEFT_TURN) {
			createEvent = true;
		}
	}

	// TODO: what to do with collinear edges

	if(createEvent) {
			LOG_IF(config->verbose,INFO) << "CREATE EVENT B ";

			if(c_base->a->leftListIdx  == c_base->a->rightListIdx &&
			   c_base->a->rightListIdx == c_base->b->leftListIdx  &&
		       c_base->b->leftListIdx  == c_base->b->rightListIdx ) {

				/* modify base facet */
				auto baseListIdx = allLists.size();
				list<PointExt> l;
				l.push_back(c_base->intersectionPoint);
				allLists.push_back(l);

				c_base->b->leftListIdx  = baseListIdx;
				c_base->a->rightListIdx = baseListIdx;
				c_base->b->rightListIdx = NOLIST;
				c_base->a->leftListIdx  = NOLIST;
			}

			// TODO: can the above happen if parallel occurs?

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

			insertToZmap(c_base->intersectionPoint,c_base->squaredDistance.doubleValue());

			if(c_b->a->parallel) {
				c_b->a->rightListIdx = listIdxB;
				c_b->b->leftListIdx  = listIdxB;
			} else {
				c_b->a->leftListIdx  = listIdxB;
				c_b->b->rightListIdx = listIdxB;
			}
	} else {
		LOG_IF(config->verbose,INFO) << " no create event! ";
	}
	LOG_IF(config->verbose,INFO) << *event;
	return createEvent;
}

vector<SweepItem*> RoofFacets::checkColinearCells(vector<SweepItem*>& cells) {
	vector<SweepItem*> colinearCells;

	for(auto cella = cells.begin(); cella != cells.end(); ++cella) {
		for(auto cellb = std::next(cella); cellb != cells.end(); ++cellb) {
			if((*cella)->base->supporting_line() == (*cellb)->base->supporting_line()) {
				colinearCells.push_back(*cella);
				colinearCells.push_back(*cellb);
			}
		}
	}
	return colinearCells;
}

bool RoofFacets::handleVertexEvent(SweepEvent* event) {
	auto divideNodes = event->getActiveCells();
	if(event->numberDivideNodes() >= 1) {
		LOG_IF(config->verbose,INFO) << "Vertex/Divide EVENT ";

		for(auto cell : divideNodes) {

			if(cell->isPossibleDivideNode()) {
				auto& la = allLists[cell->a->leftListIdx];
				la.push_back(cell->intersectionPoint);

				if(cell->a->leftListIdx != cell->b->rightListIdx) {
					auto& lb = allLists[cell->b->rightListIdx];
					lb.push_front(cell->intersectionPoint);
				}

				insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());

				cell->a->rightListIdx = cell->b->rightListIdx;
				cell->b->leftListIdx  = cell->a->leftListIdx;
				cell->a->leftListIdx  = NOLIST;
				cell->b->rightListIdx = NOLIST;
			} else {
				addPointToCurrentList(cell);
				turnLefRightOnIntersection(cell);
			}
		}

		LOG_IF(config->verbose,INFO) << *event;
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
		LOG_IF(config->verbose,INFO) << *cell;
}

void RoofFacets::handleLeaveEvent(SweepItem* cell) {
	if(cell->a->leftListIdx == NOLIST) {
		cell->b->leftListIdx  = NOLIST;
		cell->b->rightListIdx = NOLIST;
	} else {
		cell->a->leftListIdx  = NOLIST;
		cell->a->rightListIdx = NOLIST;
	}
	LOG_IF(config->verbose,INFO) << *cell;
}


void RoofFacets::addPointToCurrentList(SweepItem* cell) {
	bool pushEnd = (cell->a->leftListIdx != NOLIST || cell->b->leftListIdx != NOLIST) ? true : false;
	auto& l = allLists[cell->firstListIndex()];

	if(pushEnd) {
		l.push_back(cell->intersectionPoint);
	} else {
		l.push_front(cell->intersectionPoint);
	}

	insertToZmap(cell->intersectionPoint,cell->squaredDistance.doubleValue());
}

EdgeIterator RoofFacets::next(EdgeIterator i) {
	return (i+1 == polygon->edges_end()) ? polygon->edges_begin() : i+1;
}
EdgeIterator RoofFacets::prev(EdgeIterator i) {
	return (i == polygon->edges_begin()) ? polygon->edges_end()-1 : i-1;
}


void RoofFacets::printAllLists() {
	std::ostream out(nullptr);
	std::stringbuf str;
    out.rdbuf(&str);

	out << endl << "facet nr. : coordinate list" << endl;
	for(auto& facet : allFacets) {
		for(auto f : facet.second) {
			auto list = &allLists[f];
			out << f << ":";
			if(list->empty()) continue;

			auto listIt = list->begin();
			do {
				// LOG_IF(config->verbose,INFO) << *listIt << "(" << listIt->nextList << ") - ";
				if(listIt->nextList != NOLIST) {
					list = &allLists[listIt->nextList];
					listIt = list->begin();
				} else {
					out << " " << *listIt;
					++listIt;
				}
			} while(listIt != list->end() && listIt->nextList != f);
			out << endl;
		}
	}
	LOG(INFO) << str.str();
}

