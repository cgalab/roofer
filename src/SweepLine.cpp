
#include "SweepLine.h"

using namespace std;

bool operator==(const ArrangementLine& a, const ArrangementLine& b) {
		return  a.base  == b.base &&
				a.start == b.start &&
				a.e     == b.e;
}
bool operator> (const ArrangementLine& a, const ArrangementLine& b) {
    return a.base == b.base && a.base->direction() ==  Vector(a.start - b.start).direction();
}
bool operator< (const ArrangementLine& a, const ArrangementLine& b) {
    return a.base == b.base && a.base->direction() ==  Vector(b.start - a.start).direction();
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
		handlePopEvent(first);

		/* TEST */
		{
			ArrangementLine a(first.a.e,first.a.base);
			ArrangementLine b(first.a.e,first.b.e);

			SweepItem it_ab(a,b);

			if(it_ab.raysIntersect) {
				cout << "intersect! ";
				if(binary_search(eventQueue.begin(),eventQueue.end(),it_ab)) {
					cout << "found!";
				} else {
					cout << "not found!";
				}
			} else {
				cout << "do not intersect!";
			}
		}

		//cout <<

		while(!queueEmpty() && first.normalDistance == eventQueue.top().normalDistance) {
			auto second = eventQueue.top();

//			if(first.intersectionPoint == second.intersectionPoint) {
			if((first.base != second.base) &&
//				( (first.a.base  == second.a.e || first.a.base  == second.b.e ) ||
//				  (second.b.base ==  first.a.e || second.a.base ==  first.b.e) ) ) {
				  (first.a.e  == second.base || first.b.e  == second.base ) ) {
				event.push_back(second);
				eventQueue.pop();
				handlePopEvent(second);
				cout << " | ";
			} else {
				cout << " - ";
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

		for(auto e : temp) { eventQueue.push(e); }

		return event;
	}
}

void SweepLine::handlePopEvent(SweepItem item) {
	auto a = item.a;
	auto b = item.b;
	auto lStatus = status[item.base];

	assert(!(a == b));
	if(a == b) throw runtime_error("ERROR: handlePopEvent(a==b)!");

	auto TESTB = lower_bound(lStatus.begin(),lStatus.end(),b,less<ArrangementLine>());
	if(TESTB == lStatus.end() || !(*TESTB == item.b) ) {
		cout << "TEST B NOT FOUND!" << endl;
	} else {
		cout << "TEST B FOUND!" << endl;
	}

	auto LeFoundA = lower_bound(lStatus.begin(),lStatus.end(),a,less<ArrangementLine>());
	if(LeFoundA == lStatus.end() || !(*LeFoundA == item.a) ) {
		cout << "ERROR: item(" << item.a.start << ") not found! ";
		if(LeFoundA != lStatus.end()) {
			cout << "(" << item.a.start << " " << LeFoundA->start << ")";
		} else {
			cout << " end.";
		}
		fflush(stdout);
		return;
	} //else cout << " FOUND! ";


	SweepLineIterator LeFoundB;

	if(LeFoundA+1 != lStatus.end()) {
		if(b == *(LeFoundA+1) ) {
			LeFoundB = LeFoundA+1;
		} else {
			if(LeFoundA!= lStatus.begin() && b == *(LeFoundA-1)) cout << " (found at -1)! ";
			cout << "Wrong Order!" << endl;	fflush(stdout);
			if(*find(lStatus.begin(),lStatus.end(),b) == b) cout << " is inside! ";
			LeFoundB = LeFoundA;
			if(LeFoundA != lStatus.begin()) {
				LeFoundA = LeFoundA-1;
			}
		}
	}

	assert(a == *LeFoundA && b == *LeFoundB);
	if(!(a == *LeFoundA && b == *LeFoundB)) throw runtime_error("ERROR: handlePopEvent(a,b not FOUND)!");

	if(LeFoundA != lStatus.begin()) {
		SweepItem beforeA(*(LeFoundA-1), *LeFoundB);

//		cout << "BeforeA"; fflush(stdout);
		if(beforeA.raysIntersect && beforeA.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),beforeA)) {
//				cout << "(befA:" << beforeA.normalDistance << ") ";
				eventQueue.push(beforeA);
			}
		}
	}

	if(LeFoundB+1 != lStatus.end())  {
		SweepItem afterB(*LeFoundA, *(LeFoundB+1) );
//		cout << "AfterB "; fflush(stdout);
		if(afterB.raysIntersect && afterB.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),afterB)) {
//				cout << "(afterB:" << afterB.normalDistance << ") ";
				eventQueue.push(afterB);
			}
		}
	}

	// swap line segments in status, as of the intersection point.
	iter_swap(LeFoundA, LeFoundB);
}

