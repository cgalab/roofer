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
	for(auto edgeIt=data.polygon.edges_begin(); edgeIt != data.polygon.edges_end(); ++edgeIt) {
		for(auto it=data.polygon.edges_begin(); it != data.polygon.edges_end(); ++it) {
			if( edgeIt->supporting_line() != it->supporting_line() ) {
				ArrangementLine al(edgeIt,it);
				data.sweepLine.addLine(al);
			}
		}
	}
}

void Skeleton::startPlaneSweep() {
	while(!data.sweepLine.queueEmpty()) {
		try {
			auto item = data.sweepLine.popEvent();
			handleNextEvent(item);
		} catch(const out_of_range& err) {
			cerr << "EXEPTION: " << err.what() << endl;
		}
	}
}

void Skeleton::handleNextEvent(SweepItem& item) {

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
