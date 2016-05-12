

#include "SweepLine.h"

using namespace std;

void SweepLine::initiateEventQueue() {
	assert(arrangementStart.empty());

	for(auto le : arrangementStart) {
		assert(le.second.empty() && le.second.size >= 2);

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

	cout << "handlePopEvent: "; fflush(stdout);
	auto LeFoundA = lower_bound(lStatus.begin(),lStatus.end(),a,Compare<ArrangementLine>());
	SweepLineIterator LeFoundB;

	if(b == *(nextEdge(LeFoundA,lStatus)) ) {
		LeFoundB = nextEdge(LeFoundA,lStatus);
	} else {
		LeFoundA = prevEdge(LeFoundA,lStatus);
		LeFoundB = nextEdge(LeFoundA,lStatus);
	}

	assert(b == *LeFoundA || b == *LeFoundB);

	auto test = prevEdge(LeFoundA,lStatus);

	cout << "1 " << test->start << " - " << b.start << " " << a.start << " "; fflush(stdout);

	SweepItem beforeA(*(prevEdge(LeFoundA,lStatus)), *LeFoundA);
	cout << "1.5 " << b.start << " " << a.start << " "; fflush(stdout);
	SweepItem afterB(*LeFoundB, *(nextEdge(LeFoundB,lStatus)) );

	cout << "2 "; fflush(stdout);
	if(beforeA.raysIntersect) {
		eventQueue.push(beforeA);
	}

	cout << "3 "; fflush(stdout);
	if(afterB.raysIntersect) {
		eventQueue.push(afterB);
	}

	iter_swap(LeFoundA,LeFoundB);

	if(*LeFoundB == a) cout << "swapped!";
	else cout << "maybe swapped!";
	cout << endl;
}

