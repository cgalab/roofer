
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
	return a.normalDistance == b.normalDistance &&
		   a.a 				== b.a &&
		   b.b 				== b.b;
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

		} while(!le.second.empty());
	}
}

 SweepItem SweepLine::popEvent() {
	if(queueEmpty()) {
		throw out_of_range("EventQueue is empty!");
	} else {
		auto ret = eventQueue.top();
		eventQueue.pop();

		handlePopEvent(ret);

		return ret;
	}
}

void SweepLine::handlePopEvent(SweepItem& item) {
	auto a = item.a;
	auto b = item.b;
	auto lStatus = status[a.base];

//	cout << "handlePopEvent(" << item.dist() << "): "; fflush(stdout);

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
			cout << "Wrong Order!" << endl;	fflush(stdout);
			LeFoundB = LeFoundA;
			if(LeFoundA != lStatus.begin()) {
				LeFoundA = LeFoundA-1;
			}
		}
	}

	assert(b == *LeFoundB);

	if(LeFoundA != lStatus.begin()) {
		SweepItem beforeA(*(LeFoundA-1), *LeFoundA);

//		cout << "BeforeA"; fflush(stdout);
		if(beforeA.raysIntersect && beforeA.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),beforeA)) {
//				cout << "(befA:" << beforeA.normalDistance << ") ";
				eventQueue.push(beforeA);
			}
		}
	}

	if(LeFoundB+1 != lStatus.end())  {
		SweepItem afterB(*LeFoundB, *(LeFoundB+1) );
//		cout << "AfterB "; fflush(stdout);
		if(afterB.raysIntersect && afterB.normalDistance > item.normalDistance) {
			if(!binary_search(eventQueue.begin(),eventQueue.end(),afterB)) {
//				cout << "(afterB:" << afterB.normalDistance << ") ";
				eventQueue.push(afterB);
			}
		}
	}

	// swap line segments in status, as of the intersection point.
	iter_swap(LeFoundA,LeFoundB);
}

