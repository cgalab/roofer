
#include "SweepLine.h"

using namespace std;

//bool DistanceCompare::operator==(ArrangementLine& a, ArrangementLine& b) {
//		return  a.base  == b.base &&
//				a.start == b.start &&
//				a.e     == b.e;
//}
//bool DistanceCompare::operator> (ArrangementLine& a, ArrangementLine& b) {
//    return (a.base == b.base && a.base->direction() == Vector(a.start - b.start).direction()) ||
//    	   (a.base != b.base && a.uid > b.uid);
//}

//bool DistanceCompare::operator() (ArrangementLine& a, ArrangementLine& b) {
//    return (a.base == b.base && a.base->direction() == Vector(b.start - a.start).direction()) ||
//     	   (a.base != b.base && a.uid < b.uid);
//}

bool operator==(const ArrangementLine& a, const ArrangementLine& b) {
		return  a.base  == b.base &&
				a.start == b.start &&
				a.e     == b.e;
}
bool operator> (const ArrangementLine& a, const ArrangementLine& b) {
    return (a.base == b.base && a.base->direction() == Vector(a.start - b.start).direction()) ||
    	   (a.base != b.base && a.uid > b.uid);
}
bool operator< (const ArrangementLine& a, const ArrangementLine& b) {
    return (a.base == b.base && a.base->direction() == Vector(b.start - a.start).direction()) ||
     	   (a.base != b.base && a.uid < b.uid);
}

bool operator== (const SweepItem& a, const SweepItem& b) {
	return a.normalDistance == b.normalDistance && (
		   (a.a 				== b.a &&
		    b.b 				== b.b) || (
			a.a 				== b.b &&
		    b.b 				== b.a  ) );
}
bool operator> (const SweepItem& a, const SweepItem& b) {
	return  a.normalDistance  > b.normalDistance ||
		   (a.normalDistance == b.normalDistance && a.a > b.a);
}
bool operator< (const SweepItem& a, const SweepItem& b) {
	return  a.normalDistance  < b.normalDistance ||
		   (a.normalDistance == b.normalDistance && a.a < b.a);
}


void SweepLine::initiateEventQueue() {
	assert(arrangementStart.empty());

	for(auto le : arrangementStart) {
		assert(le.second.empty() && le.second.size > 1);

		do {
			ArrangementLine a = le.second.top();
			le.second.pop();
			ArrangementLine b = le.second.top();

			assert(a.base == b.base);

			SweepItem i(a,b);

			if(i.raysIntersect) {
				eventQueue.insert(i);
			}

			status[le.first].push_back(a);

		} while(le.second.size() > 1);

		status[le.first].push_back(le.second.top());
		le.second.pop();
	}

	for(auto &line : status) {
		int cnt = 0;
		for(auto &l : line.second) {
			l.lid = cnt++;
		}
		for(auto l : line.second) {
			cout << l.lid << " ";
		}
		cout << endl;
	}
}

void SweepLine::printEventQueue() {
	cout << "Q: ";

	for(auto e : eventQueue) {
		cout << e.normalDistance << " - ";
	}

	cout << endl;
}

void SweepLine::printSweepLine(SweepItem& item) {
	cout << status[item.base].begin()->eid << ": ";
	for(auto l : status[item.base]) {
		if(l == item.a || l == item.b) {
			cout << "(";
		}
		cout << l.lid;
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
		eventQueue.erase(eventQueue.begin());
		event.push_back(first);

		while(!queueEmpty() && first.normalDistance == eventQueue.begin()->normalDistance) {
			auto second = eventQueue.begin();

			if((first.base != second->base) &&
			   (first.a.e  == second->base || first.b.e  == second->base ) ) {
				event.push_back(*second);
			} else {
				temp.push_back(*second);
				cout << "Warning: event added to temp!" << endl;
			}
			eventQueue.erase(second);
		}

		if(event.size() != 3 || temp.size() > 0) {
			cout << "Temp: ";
			for(auto e : temp)  { cout << "(" << e.intersectionPoint << ") "; }
			cout << " // Event: ";
			for(auto e : event) { cout << "(" << e.intersectionPoint << ") "; }
			cout << endl;

			for(auto e : temp)  { eventQueue.insert(e); }
		}

		for(auto e : event) {
			cout << " P"; // << e.intersectionPoint << endl;
			handlePopEvent(e);
		}

		return event;
	}
}

void SweepLine::handlePopEvent(SweepItem& item) {
	auto a = item.a;
	auto b = item.b;
	auto& lStatus = status[item.base];

	DistanceCompare comp(item.intersectionPoint);

	assert(!(a == b));
	if(a == b) throw runtime_error("ERROR: handlePopEvent(a==b)!");

	cout << "+"; fflush(stdout);

	auto FoundA = lower_bound(lStatus.begin(),lStatus.end(),a,comp);
	SweepLineIterator FoundB;

	if(a == *FoundA) {
		if(b == *(FoundA+1)) {
			FoundB = FoundA+1;
		} else if(b == *(FoundA-1)) {
			FoundA = FoundA-1;
			FoundB = FoundA+1;
		} else {
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

	cout << "+"; fflush(stdout);

	if(FoundA != lStatus.begin()) {
		SweepItem beforeA(*(FoundA-1), *FoundB);
		if(beforeA.raysIntersect && beforeA.normalDistance > item.normalDistance) {
			eventQueue.insert(beforeA);
		}
	}

	cout << "+"; fflush(stdout);

	if(FoundB+1 != lStatus.end())  {
		SweepItem afterB(*FoundA, *(FoundB+1) );
		if(afterB.raysIntersect && afterB.normalDistance > item.normalDistance) {
			eventQueue.insert(afterB);
		}
	}

	cout << "+" << endl; fflush(stdout);

	// swap line segments in status, as of the intersection point.
	iter_swap(FoundA, FoundB);
}

