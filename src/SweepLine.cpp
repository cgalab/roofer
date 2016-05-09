

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
	cout << "done" << endl;
}
