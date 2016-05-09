

#include "SweepLine.h"

using namespace std;

void SweepLine::initiateEventQueue() {

	for(auto le : arrangementStart) {

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

		} while(!status.empty());
	}
}
