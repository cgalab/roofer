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


#include "SweepLine.h"

#include <iostream>

using namespace std;

ostream& operator<<(ostream& out, const SweepItem& item) {
		out <<  item.a->eid << " (";
		for(int i = 0; i < 2; ++i) {
			for(int j = 0; j < 2; ++j) {
				if(item.get(i,j) == NOLIST) {
					out << "_";
				} else {
					out << item.get(i,j);
				}
				if(i != 1 || j != 1) out << ",";
			}
		}
		out << ")  ";

		if(item.a->ghost) {out << "g";}
		out << "a(";
		if(item.a->parallel) { out << "p,p)-"; } else { out <<
			item.a->bisector.to_vector().x().doubleValue() << "," <<
			item.a->bisector.to_vector().y().doubleValue() << ")-";
		}
		if(item.b->ghost) {out << "g";}
		out << "b(";
		if(item.b->parallel) { out << "p,p) "; } else { out <<
			item.b->bisector.to_vector().x().doubleValue() << "," <<
			item.b->bisector.to_vector().y().doubleValue() << ") ";
		}

	    return out;
}

ostream& operator<<(ostream& out, const SweepEvent& event) {
	std::vector<int> bak = {};
	for(auto c : event) {bak.push_back( c.a->eid );}
	std::sort(bak.begin(),bak.end());

	out << "[ ";
	for(auto c : bak) {out << c << " ";}
	out << "]: ";
	for(auto c : event) {out << c << " --- ";}
    return out;
}

bool operator==(const ArrangementLine& a, const ArrangementLine& b) {
//		return  a.base  == b.base  &&
//				a.start == b.start &&
//				a.e     == b.e;
		return  (!a.parallel && !b.parallel && a.start == b.start && a.bisector == b.bisector) ||
				( a.parallel &&  b.parallel && (
					a.line  == b.line || a.line.opposite() == b.line || a.line == b.line.opposite()
				  )
				);
}
bool operator> (const ArrangementLine& a, const ArrangementLine& b) {
    return (!a.parallel && b.parallel) || (a.parallel && b.parallel && a.dist > b.dist)
    		||
		   (a.base == b.base && a.start != b.start &&
    		a.base->direction() == Vector(a.start - b.start).direction())
    		||
    	   (a.base == b.base && a.start == b.start &&
    		CGAL::orientation(a.bisector.to_vector(),b.bisector.to_vector()) == CGAL::LEFT_TURN)
		    ||
		   (a.base != b.base && a.eid > b.eid);
}
bool operator< (const ArrangementLine& a, const ArrangementLine& b) {
    return (a.parallel && !b.parallel) || (a.parallel && b.parallel && a.dist < b.dist)
    		||
    	   (a.base == b.base && a.start != b.start &&
    		a.base->direction() == Vector(b.start - a.start).direction())
    		||
     	   (a.base == b.base && a.start == b.start &&
     		CGAL::orientation(a.bisector.to_vector(),b.bisector.to_vector()) == CGAL::RIGHT_TURN)
			||
		   (a.base != b.base && a.eid < b.eid);
}
bool operator< (const ArrangementLine& a, const Point& p) {
    return (a.start != p &&
    		a.base->direction() == Vector(a.e->supporting_line().projection(p) - a.e->supporting_line().projection(a.start)).direction());
}
bool operator> (const ArrangementLine& a, const Point& p) {
    return (a.start != p &&
    		a.base->direction() == Vector(a.e->supporting_line().projection(a.start) - a.e->supporting_line().projection(p)).direction());
}

bool operator== (const SweepItem& a, const SweepItem& b) {
//	return a.normalDistance == b.normalDistance && (
//		   (a.a 				== b.a &&
//		    b.b 				== b.b) || (
//			a.a 				== b.b &&
//		    b.b 				== b.a  ) );
	return a.intersectionPoint  == b.intersectionPoint && ( //a.base == b.base && (
		   (a.a 				== b.a &&
		    b.b 				== b.b) || (
			a.a 				== b.b &&
		    b.b 				== b.a  ) );
}
bool operator> (const SweepItem& a, const SweepItem& b) {
	return (a.squaredDistance  > b.squaredDistance) ||
		   (a.squaredDistance == b.squaredDistance && a.intersectionPoint >  b.intersectionPoint) || (
			a.squaredDistance == b.squaredDistance && a.intersectionPoint == b.intersectionPoint && (
			a.a > b.a || ( a.a == b.a && a.b > b.b ))
		   );
}
bool operator< (const SweepItem& a, const SweepItem& b) {
	return (a.squaredDistance  < b.squaredDistance) ||
		   (a.squaredDistance == b.squaredDistance && a.intersectionPoint <  b.intersectionPoint) || (
			a.squaredDistance == b.squaredDistance && a.intersectionPoint == b.intersectionPoint && (
			a.a < b.a  || (a.a == b.a && a.b < b.b))
		   );
}

ostream& operator<<(ostream& os, const ArrangementLine& al) {
	os << al.bisector.to_vector().x().doubleValue() << "," << al.bisector.to_vector().y().doubleValue();
	return os;
}

void SweepLine::initiateEventQueue() {
	assert(!arrangementStart.empty());

	for(auto& le : arrangementStart) {
		assert(!le.second.empty() && le.second.size() > 1 );

		auto& arrangementLines = allArrangementLines[le.first];
		auto& lStatus          = status[le.first];

		while(!le.second.empty()) {
			auto a = le.second.begin();
			arrangementLines.push_back(*a);
			le.second.erase(a);
		}

		for(auto i = arrangementLines.begin(); i != arrangementLines.end(); ++i) {

			lStatus.push_back(&*i);
			if(std::next(i) != arrangementLines.end()) {
				SweepItem item(&*i,&*(std::next(i)));

				if(item.raysIntersect && item.squaredDistance > 0) {
					eventQueue.insert(item);
				}
			}
		}
	}

	for(auto pit = allParallelAL.begin(); pit != allParallelAL.end(); ++pit) {
		parallelEventQueue.insert(&*pit);
	}

	for(auto &line : status) {
		int cnt = 0;
		for(auto l : line.second) {
			l->lid = cnt++;
		}
	}
}


void SweepLine::printEventQueue() {
	std::ostream out(nullptr);
	std::stringbuf str;
    out.rdbuf(&str);
    out << "Q: (" << eventQueue.size() << ") ";

	for(auto e : eventQueue) {
		out << e.squaredDistance.doubleValue() << " - " << e << ", ";
	}

	if(!parallelEventQueue.empty()) {
		out << endl << "PQ: ";
		for(auto e : parallelEventQueue) {
			out << e->dist.doubleValue() << "  ";
		}
		out << endl;
	}

	LOG_IF(config->verbose,INFO) << str.str();
}

void SweepLine::printSweepLine(SweepItem& item) {
	std::ostream out(nullptr);
	std::stringbuf str;
    out.rdbuf(&str);

    out << status[item.base].front()->eid << ": ";
	for(auto l : status[item.base]) {
		if(l == item.a || l == item.b) {
			out << "(";
		}
		out << l->lid;
		if(l == item.a || l == item.b) {
			out << ")";
		}
		out << ", ";
	}
	out << endl;
	LOG(INFO) << str.str();
}


SweepEvent SweepLine::popEvent() {
	if(queueEmpty()) {
		throw out_of_range("EventQueue is empty!");
	} else {
		SweepEvent event;
		SweepEvent temp;

		auto first = *eventQueue.begin();

		/* checking the parallel queue if a line has to be 'added' */
		while(!parallelEventQueue.empty() && (*parallelEventQueue.begin())->dist <= first.dist()) {
			auto parallelItem = *parallelEventQueue.begin();
			/* the parallel line has to be intersected with all
			 * arrangement lines in its corresponding arrangement */

			for(auto al : status[parallelItem->base]) {

				SweepItem item(parallelItem,al);

				if(item.raysIntersect) {
					eventQueue.insert(item);
				}
			}
			parallelEventQueue.erase(parallelEventQueue.begin());
			first = *eventQueue.begin();
		}

		eventQueue.erase(eventQueue.begin());
		event.push_back(first);

		while(!queueEmpty() && first.squaredDistance == eventQueue.begin()->squaredDistance) {
			auto other = *eventQueue.begin();
			if(first.intersectionPoint == other.intersectionPoint) {
				event.push_back(other);
			} else {
				temp.push_back(other);
			}
			eventQueue.erase(eventQueue.begin());
		}

		if(temp.size() > 0) {
			for(auto e : temp)  { eventQueue.insert(e); }
		}

		/* some events may longer by valid, we know via handlePopEvent */
		SweepEvent actualEvents;
		for(auto e : event) {
			if(!e.a->parallel && !e.b->parallel) {
				if(handlePopEvent(e)){
					/* only add events that were found in status */
					actualEvents.push_back(e);
				}
			} else {
				/* adding events with parallel lines */
				actualEvents.push_back(e);
			}
		}

		return actualEvents;
	}
}

bool SweepLine::handlePopEvent(SweepItem& item) {
	const ALIterator a = item.a;
	const ALIterator b = item.b;
	auto& lStatus = status[item.base];

	DistanceCompare comp(item.intersectionPoint);

	assert(!(a == b));
	if(a == b) throw runtime_error("ERROR: handlePopEvent(a==b)!");

	auto FoundA = lower_bound(lStatus.begin(),lStatus.end(),a,comp);
	auto FoundB = FoundA;

	if(a == *FoundA) {
		if(b == *(FoundA+1)) {
			FoundB = FoundA+1;
		} else if(b == *(FoundA-1)) {
			FoundA = FoundA-1;
			FoundB = FoundA+1;
		} else {
			LOG(WARNING) << item << " - WARNING: handlePopEvent b not at a+-1!";
			return false;
		}
	} else if(!(a == *FoundA) && b == *FoundA) {
		FoundB = FoundA;
		if(a == *(FoundB+1)) {
			FoundB = FoundB+1;
			FoundA = FoundB-1;
		} else if(a == *(FoundB-1)) {
			FoundA = FoundB-1;
		} else {
			if(a == *find(lStatus.begin(),lStatus.end(),a)) {LOG_IF(config->verbose,INFO) << "--found A with normal find....";}
			LOG(WARNING) << "WARNING: handlePopEvent a not at b+-1!";
			return false;
		}
	} else {
		LOG(WARNING) << item <<  " - WARNING: handlePopEvent a,b not found!";
		return false;
	}


	if(!(*FoundA == lStatus.front())) {
		SweepItem beforeA(*(FoundA-1), *FoundB);
		if(beforeA.raysIntersect && beforeA.squaredDistance > item.squaredDistance) {
			eventQueue.insert(beforeA);
		}
	}


	if(!(*FoundB == lStatus.back()))  {
		SweepItem afterB(*FoundA, *(FoundB+1) );
		if(afterB.raysIntersect && afterB.squaredDistance > item.squaredDistance) {
			eventQueue.insert(afterB);
		}
	}

	// swap line segments in status, as of the intersection point.
	iter_swap(FoundA, FoundB);
	return true;
}

ALIterator SweepLine::insertGhostVertex(SweepItem* cell, SweepEvent& ghostCells) {
	LOG_IF(config->verbose,INFO) << " --- INSERT GHOST VERTEX !! --- ";

	/* define ghost vertex */
	ArrangementLine al(cell->a->base,cell->b->base);
	al.setGhostVertex(cell->intersectionPoint);
	al.setEID(cell->a->eid);

	/* store ghost vertex */
	auto& allAL = allArrangementLines[al.base];
	allAL.push_back(al);
	auto newAlIt = std::prev(allAL.end());

	/* insert ghost vertex in local sweep line status */
	auto& lStatus = status[cell->base];
	DistanceCompare comp(cell->intersectionPoint);
	/* after handlepopevent positions of a and b are already switched in local status */
	auto itFirstOcc = lower_bound(lStatus.begin(),lStatus.end(),cell->b,comp);
	/* lower_bound finds first occurence */
	// TODO: fix if more than two AL's meet here.
	++itFirstOcc;

	/* inserts element before iterator position */
	lStatus.insert(itFirstOcc,&*newAlIt);

	/* iterator may have changed after insert */
	itFirstOcc = lower_bound(lStatus.begin(),lStatus.end(),cell->b,comp);
	++itFirstOcc;

	/* compute events for both sides in local sweep line status */
	if(itFirstOcc != lStatus.begin()) {
		SweepItem iBefore(*(itFirstOcc-1),*(itFirstOcc));
		ghostCells.push_back(iBefore);
	}

	if(itFirstOcc+1 != lStatus.end()) {
		SweepItem iAfter(*(itFirstOcc),*(itFirstOcc+1));
		ghostCells.push_back(iAfter);
	}

	return &*std::prev(allAL.end());
}

void SweepLine::deleteGhostVertex(SweepItem* cell, ALIterator gv) {
	LOG_IF(config->verbose,INFO) << " is gv:" << gv->ghost << " ";
	auto& lStatus = status[cell->base];
	DistanceCompare comp(cell->intersectionPoint);
	auto itFirstOcc = lower_bound(lStatus.begin(),lStatus.end(),cell->b,comp);

	if(*itFirstOcc == gv) {
		lStatus.erase(itFirstOcc);
		LOG_IF(config->verbose,INFO) << " ghost vertex erased ";
	}
}
