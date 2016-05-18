
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
				eventQueue.push(i);
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

void SweepLine::printSweepLine(SweepItem& item) {
	cout << status[item.base].begin()->uid << ": ";
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

		auto first = eventQueue.top();
		eventQueue.pop();

		event.push_back(first);
//		handlePopEvent(first);

		/* TEST */
//		{
//			ArrangementLine a(first.a.e,first.a.base);
//			ArrangementLine b(first.a.e,first.b.e);
//
//			SweepItem it_ab(a,b);
//
//			if(it_ab.raysIntersect) {
//				cout << "intersect! ";
//				if(binary_search(eventQueue.begin(),eventQueue.end(),it_ab)) {
//					cout << "found!";
//				} else {
//					cout << "not found!";
//				}
//			} else {
//				cout << "do not intersect!";
//			}
//		}

		//cout <<

		while(!queueEmpty() && first.normalDistance == eventQueue.top().normalDistance) {
			auto second = eventQueue.top();

			if((first.base != second.base) &&
			   (first.a.e  == second.base || first.b.e  == second.base ) ) {
				event.push_back(second);
				eventQueue.pop();
			} else {
				temp.push_back(second);
				eventQueue.pop();
			}
		}

		cout << "Event:" << event.size() << ", Tmp: " << temp.size() << endl;

		if(event.size() != 3) {
			cout << "F: " << first.intersectionPoint << " - ";
			for(auto e : temp) { cout << e.intersectionPoint; }
			int cnt = 0;
			for(auto it = eventQueue.begin(); it != eventQueue.end() && cnt < 5; ++it) {
				cout << it->intersectionPoint << " - ";
				++cnt;
			}
			cout << endl;
		}

		for(auto e : temp)  { eventQueue.push(e); }

		for(auto e : event) { handlePopEvent(e); }

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

//	auto TESTB = lower_bound(lStatus.begin(),lStatus.end(),b,comp);
//	if(TESTB == lStatus.end() || !(*TESTB == item.b) ) {
//		cout << "TEST B NOT FOUND!" << endl;
//	} else {
//		cout << "TEST B FOUND!" << endl;
//	}

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
			if(a == *find(lStatus.begin(),lStatus.end(),a)) {cout << "found A with normal find...." << endl;}
			throw runtime_error("ERROR: handlePopEvent a not at b+-1!");
		}
	} else {
		throw runtime_error("ERROR: handlePopEvent a,b not found!");
	}


//	if(LeFoundA == lStatus.end() || !(*LeFoundA == item.a) ) {
//		if(a == *find(lStatus.begin(),lStatus.end(),a)) {cout << "found A with normal find...." << endl;}
//		cout << "ERROR: item(" << item.a.start << ") not found! ";
//		if(LeFoundA != lStatus.end()) {
//			cout << "(" << item.a.start << " " << LeFoundA->start << ")";
//		} else {
//			cout << " end.";
//		}
//		fflush(stdout);
//		return;
//	} else cout << "A FOUND! ";
//
//
//	SweepLineIterator LeFoundB;

//	if(FoundA+1 != lStatus.end()) {
//		if(b == *(FoundA+1) ) {
//			 cout << "B FOUND! ";
//			LeFoundB = LeFoundA+1;
//		} else {
//			if(LeFoundA!= lStatus.begin() && b == *(LeFoundA-1)) cout << " (found at -1)! ";
//			cout << "Wrong Order!" << endl;	fflush(stdout);
//			if(*find(lStatus.begin(),lStatus.end(),b) == b) cout << " is inside! ";
//			LeFoundB = LeFoundA;
//			if(LeFoundA != lStatus.begin()) {
//				LeFoundA = LeFoundA-1;
//			}
//		}
//	}

//	assert(a == *LeFoundA && b == *LeFoundB);
//	if(!(a == *LeFoundA && b == *LeFoundB)) throw runtime_error("ERROR: handlePopEvent(a,b not FOUND)!");

	if(FoundA != lStatus.begin()) {
		SweepItem beforeA(*(FoundA-1), *FoundB);

//		cout << "BeforeA"; fflush(stdout);
		if(beforeA.raysIntersect && beforeA.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),beforeA)) {
//				cout << "(befA:" << beforeA.normalDistance << ") ";
				eventQueue.push(beforeA);
			} else cout << "Warning: Found via binary search!";
		}
	}

	if(FoundB+1 != lStatus.end())  {
		SweepItem afterB(*FoundA, *(FoundB+1) );
//		cout << "AfterB "; fflush(stdout);
		if(afterB.raysIntersect && afterB.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),afterB)) {
//				cout << "(afterB:" << afterB.normalDistance << ") ";
				eventQueue.push(afterB);
			} else cout << "Warning: Found via binary search!";
		}
	}

	// swap line segments in status, as of the intersection point.
	iter_swap(FoundA, FoundB);
}

