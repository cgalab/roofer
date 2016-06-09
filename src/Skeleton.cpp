/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>
#include <exception>


#include "Data.h"
#include "Skeleton.h"
#include "GUI.h"

using namespace std;

Skeleton::Skeleton(list<string>& args) {
	/* evaluate arguments and load data */
	if(!data.evaluateArguments(args)) return;

	/* compute the sought bisector graph */
	createSkeleton();

//	/* display if needed */
//		#ifdef QTGUI
//			if(data.config.gui) {
//				gui.make_facet(data.polygon);
//				gui.show(data.iss);
//			}
//		#endif // CGAL_LCC_USE_VIEWER
}

Skeleton::~Skeleton() {}

void Skeleton::createSkeleton() {
	/***
	 * create initial wavefront
	 ***/
	cout << endl << "generating line arrangements and event queue...";
	createLineArrangements();
	cout << "done" << endl;

	cout << endl << "initiate event queue...";
	data.sweepLine.initiateEventQueue();
	cout << "done" << endl;

	cout << endl << "add all base cells...";
	addAllBaseCells();
	cout << "done" << endl;

	/* start wavefront propagation */
#ifdef QTGUI
	if(!data.config.gui) {
#endif
		cout << "start plane sweep...";
		startPlaneSweep();
		cout << "done" << endl;
#ifdef QTGUI
	}
#endif
}

void Skeleton::createLineArrangements() {
	int uid = 0, eid = 0;
	for(auto baseEdgeIt=data.polygon.edges_begin(); baseEdgeIt != data.polygon.edges_end(); ++baseEdgeIt) {
		for(auto edgeIt=data.polygon.edges_begin(); edgeIt != data.polygon.edges_end(); ++edgeIt) {
			if( baseEdgeIt->supporting_line() != edgeIt->supporting_line() ) {
				ArrangementLine al(baseEdgeIt, edgeIt, uid++, eid);
				data.sweepLine.addLine(al);
			}
		}
		eid++;
	}
}

/**
 * as upper- and lower chain are lists (not sorted like a set) we use the
 * already sorted line arrangements to fill them.
 * */
void Skeleton::addAllBaseCells() {
	for(auto& a : data.sweepLine.status) {

		for(auto line = a.second.begin(); line != a.second.end(); ++line) {
			Point edgeStart((*line)->base->vertex(0));
			Point edgeEnd((*line)->base->vertex(1));
cout << "."; fflush(stdout);
			/* check for duplicates (is already sorted) */
			if(line-1 != a.second.begin() && (*line)->start == (*(line-1))->start &&
			   edgeStart == (*line)->start) {

				(*line)->leftListIdx  = (*line-1)->rightListIdx;
				(*line)->rightListIdx = (*line-1)->rightListIdx;

			} else if(line+1 != a.second.end() && (*line)->start == (*(line+1))->start &&
			          edgeEnd == (*line)->start) {

				auto idx = data.facets.allFacets[(*line)->base].front().front();
				(*line)->leftListIdx  = idx;
				(*line)->rightListIdx = idx;
			} else {
				data.facets.addBaseCell(*line);
			}
		}
	}
}

void Skeleton::startPlaneSweep() {
//	cout << "Beginning Q-State: "; data.sweepLine.printEventQueue();
	while(!data.sweepLine.queueEmpty()) {
		try {
			auto item = data.sweepLine.popEvent();
			handleNextEvent(item);
		} catch(const out_of_range& err) {
			cerr << "EXEPTION: " << err.what() << endl;
			break;
		} catch(const runtime_error& err) {
			cerr << "EXEPTION: " << err.what() << endl;
			break;
		}
	}
}

void Skeleton::handleNextEvent(SweepEvent& event) {
	cout << "Q: " << data.sweepLine.queueSize() << endl;
	if(event.size() != 3) cout << ", Items: " << event.size() << endl;

//	for(auto &e : event) cout << e.a.leftListIdx << "," << e.a.rightListIdx << " - " << e.b.leftListIdx << "," <<e.b.rightListIdx << endl;;

	data.facets.handleCell(&event);


//	for(auto i : event) {
//		data.sweepLine.printSweepLine(i);
//	}
}

//void Skeleton::handleSplitEvent(Event& e) {
//	cout << "Split Event" << endl;
//}
//
//void Skeleton::handleEdgeEvent(Event& e) {
//	cout << "Divide Event" << endl;
//}
//
//void Skeleton::handleCreateEvent(Event& e) {
//	cout << "Create Event" << endl;
//}
