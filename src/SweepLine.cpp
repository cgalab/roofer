
#include "SweepLine.h"

using namespace std;

//bool operator==(const ArrangementLine* a, const ArrangementLine* b) {
//		return  a->base  == b->base &&
//				a->start == b->start &&
//				a->e     == b->e;
//}
//bool operator> (const ArrangementLine* a, const ArrangementLine* b) {
//    return (a->base == b->base && a->base->direction() == Vector(a->start - b->start).direction()) ||
//    	   (a->base != b->base && a->uid > b->uid);
//}
//bool operator< (const ArrangementLine* a, const ArrangementLine* b) {
//    return (a->base == b->base && a->base->direction() == Vector(b->start - a->start).direction()) ||
//     	   (a->base != b->base && a->uid < b->uid);
//}

bool operator==(const ALIterator& a, const ALIterator& b) {return operator==(*a,*b);}
bool operator> (const ALIterator& a, const ALIterator& b) {return operator> (*a,*b);}
bool operator< (const ALIterator& a, const ALIterator& b) {return operator< (*a,*b);}

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
	return (a.normalDistance  > b.normalDistance) ||
		   (a.normalDistance == b.normalDistance && a.intersectionPoint >  b.intersectionPoint) || (
			a.normalDistance == b.normalDistance && a.intersectionPoint == b.intersectionPoint && (
			a.a > b.a || ( a.a == b.a && a.b > b.b ))
		   );
}
bool operator< (const SweepItem& a, const SweepItem& b) {
	return (a.normalDistance  < b.normalDistance) ||
		   (a.normalDistance == b.normalDistance && a.intersectionPoint <  b.intersectionPoint) || (
			a.normalDistance == b.normalDistance && a.intersectionPoint == b.intersectionPoint && (
			a.a < b.a  || (a.a == b.a && a.b < b.b))
		   );
}

ostream& operator<<(ostream& os, const ArrangementLine& al) {
	os << al.bisector.to_vector().x().doubleValue() << "," << al.bisector.to_vector().y().doubleValue();
	return os;
}

void SweepLine::initiateEventQueue() {
	assert(arrangementStart.empty());

	for(auto& le : arrangementStart) {
		assert(le.second.empty() && le.second.size > 1);

		auto& arrangementLines = allArrangementLines[le.first];
		auto& lStatus          = status[le.first];

		while(!le.second.empty()) {
			auto a = le.second.begin();

			arrangementLines.push_back(*a);
			le.second.erase(a);
		}

		for(auto i = arrangementLines.begin(); i != arrangementLines.end(); ++i) {
			lStatus.push_back(i);
			if(i+1 != arrangementLines.end()) {
				SweepItem item(i,(i+1));

				if(item.raysIntersect && item.normalDistance > 0) {
					eventQueue.insert(item);
				}
			}
		}
	}

	for(auto pit = allParallelAL.begin(); pit != allParallelAL.end(); ++pit) {
		parallelEventQueue.insert(pit);
	}

	for(auto &line : status) {
		int cnt = 0;
		for(auto l : line.second) {
			l->lid = cnt++;
		}
	}
}


void SweepLine::printEventQueue() {
	cout << "Q: ";

	for(auto e : eventQueue) {
		cout << e.normalDistance.doubleValue() << " - "; e.print(); cout << ", ";
	}

	if(!parallelEventQueue.empty()) {
		cout << endl << "PQ: ";
		for(auto e : parallelEventQueue) {
			cout << e->dist.doubleValue() << "  ";
		}
		cout << endl;
	}
}

void SweepLine::printSweepLine(SweepItem& item) {
	cout << status[item.base].front()->eid << ": ";
	for(auto l : status[item.base]) {
		if(l == item.a || l == item.b) {
			cout << "(";
		}
		cout << l->lid;
		if(l == item.a || l == item.b) {
			cout << ")";
		}
		cout << ", ";
	}
	cout << endl;
}


SweepEvent SweepLine::popEvent() {
	if(queueEmpty()) {
		throw out_of_range("EventQueue is empty!");
	} else {
		SweepEvent event;
		SweepEvent temp;

		auto first = *eventQueue.begin();

//		cout << endl;
//		printEventQueue();
		/* checking the parallel queue if a line has to be 'added' */
		while(!parallelEventQueue.empty() && (*parallelEventQueue.begin())->dist <= first.dist()) {
			auto parallelItem = *parallelEventQueue.begin();
//			cout << "handle parallel line ";
			/* the parallel line has to be intersected with all
			 * arrangement lines in its corresponding arrangement */

			for(auto al : status[parallelItem->base]) {

				SweepItem item(parallelItem,al);
//				if(al->leftListIdx != NOLIST) {
//					item = SweepItem(al,parallelItem);
//				}

//				item.print();
				if(item.raysIntersect) {
//					cout << endl <<" ->ins " << item.dist().doubleValue() << " "; item.printIntPoint();
//					cout << " "; item.print();
					eventQueue.insert(item);
				}
			}
			parallelEventQueue.erase(parallelEventQueue.begin());
			first = *eventQueue.begin();
		}

//		cout << endl;
//		printEventQueue();

		eventQueue.erase(eventQueue.begin());
		event.push_back(first);

//		cout << endl << "events: ";
//		first.printIntPoint();
//		cout << " - ";
		while(!queueEmpty() && first.normalDistance == eventQueue.begin()->normalDistance) {
			auto other = *eventQueue.begin();
//			if((first.base != other->base) &&
//			   (first.a->e  == other->base || first.b->e  == other->base ) ) {
//				event.push_back(*other);
//			}
//			else
			if(first.intersectionPoint == other.intersectionPoint) {
				event.push_back(other);
			} else {
				temp.push_back(other);
				cout << "T";
			}
//			other.printIntPoint();
//			cout << " - ";
			eventQueue.erase(eventQueue.begin());
		}

//		cout << "event: ";
//		for(auto c : event) c.print();
//		cout << endl << "temp: ";
//		for(auto c : temp) c.print();
//		cout << endl;

		if(temp.size() > 0) {
//			cout << " Temp: ";
//			for(auto e : temp)  {
//				cout << "(" << e.intersectionPoint.x().doubleValue() << ","
//					 << e.intersectionPoint.y().doubleValue() << ") ";
//				e.print();
//			}
//			cout << endl;

			for(auto e : temp)  { eventQueue.insert(e); }
		}

//		if(event.size() != 3){
//			cout << endl << "Event: ";
//			for(auto e : event) {
//				cout << "(" << e.intersectionPoint.x().doubleValue()
//					 << "," << e.intersectionPoint.y().doubleValue() << ") D:"
//					 << e.normalDistance.doubleValue() << " ";
//				e.print();
//			}
//			cout << endl;
//		}

		for(auto e : event) {
			if(!e.a->parallel && !e.b->parallel) {
				handlePopEvent(e);
			}
		}

		return event;
	}
}

void SweepLine::handlePopEvent(SweepItem& item) {
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
			cout << "Edge: " << a->eid << " ";
			throw runtime_error("ERROR: handlePopEvent b not at a+-1!");
		}
	} else if(!(a == *FoundA) && b == *FoundA) {
		FoundB = FoundA;
		if(a == *(FoundB+1)) {
			FoundB = FoundB+1;
			FoundA = FoundB-1;
		} else if(a == *(FoundB-1)) {
			FoundA = FoundB-1;
		} else {
			if(a == *find(lStatus.begin(),lStatus.end(),a)) {cout << "--found A with normal find...." << endl;}
			throw runtime_error("ERROR: handlePopEvent a not at b+-1!");
		}
	} else {
		throw runtime_error("ERROR: handlePopEvent a,b not found!");
	}


	if(!(*FoundA == lStatus.front())) {
		SweepItem beforeA(*(FoundA-1), *FoundB);
		if(beforeA.raysIntersect && beforeA.normalDistance > item.normalDistance) {
			eventQueue.insert(beforeA);
		}
	}


	if(!(*FoundB == lStatus.back()))  {
		SweepItem afterB(*FoundA, *(FoundB+1) );
		if(afterB.raysIntersect && afterB.normalDistance > item.normalDistance) {
			eventQueue.insert(afterB);
		}
	}

	// swap line segments in status, as of the intersection point.
	iter_swap(FoundA, FoundB);
}

SweepEvent SweepLine::insertGhostVertex(SweepItem* cell) {
	cout << endl << " --- INSERT GHOST VERTEX !! --- " << endl;
	SweepEvent newItems;

	/* define ghost vertex */
	ArrangementLine al(cell->base,cell->base);
	al.setGhostVertex(cell->intersectionPoint);

	/* store ghost vertex */
	auto allAL = allArrangementLines[al.base];
	allAL.push_back(al);
	auto newAlIt = allAL.end()-1;

	/* insert ghost vertex in local sweep line status */
	auto lStatus = status[cell->base];
	DistanceCompare comp(cell->intersectionPoint);
	/* after handlepopevent positions of a and b are already switched in local status */
	auto itFirstOcc = lower_bound(lStatus.begin(),lStatus.end(),cell->a,comp);
	/* lower_bound finds first occurence */
	// TODO: fix if more than two AL's meet here.
	++itFirstOcc;

	/* inserts element before iterator position */
	lStatus.insert(itFirstOcc,newAlIt);

	/* iterator may have changed after insert */
	itFirstOcc = lower_bound(lStatus.begin(),lStatus.end(),cell->a,comp);
	++itFirstOcc;

	if((*itFirstOcc) == newAlIt) {
		cout << "NEW FOUND! "; fflush(stdout);
	}
	if((*(itFirstOcc-1)) == cell->b) {
		cout << "B FOUND! "; fflush(stdout);
	}
	if((*(itFirstOcc+1)) == cell->a) {
		cout << "A FOUND! "; fflush(stdout);
	}

	/* compute events for both sides in local sweep line status */
	if(itFirstOcc != lStatus.begin()) {
		SweepItem iBefore(*(itFirstOcc-1),*(itFirstOcc));
		newItems.push_back(iBefore);
	}

	if(itFirstOcc+1 != lStatus.end()) {
		SweepItem iAfter(*(itFirstOcc),*(itFirstOcc+1));
		newItems.push_back(iAfter);
	}

	return newItems;
}
