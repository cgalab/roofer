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
	if(!data.config.silent) {LOG(INFO) << "generating line arrangements and event queue...";}
	createLineArrangements();
	if(!data.config.silent) {cout << "done" << endl;}

	el::Loggers::setVerboseLevel(1);
	LOG(INFO) << " -- INFO -- ";
	LOG(WARNING) << " -- WARNING -- ";
	LOG(ERROR) << " -- ERROR -- ";
	LOG(DEBUG) << " -- DEBUG -- ";

    if(!data.config.silent) {cout << "initiate event queue...";}
	data.sweepLine.initiateEventQueue();
	if(!data.config.silent) {cout << "done" << endl;}

	if(!data.config.silent) {cout << "add all base cells...";}
	addAllBaseCells();
	if(!data.config.silent) {cout << "done" << endl;}

	data.sweepLine.printEventQueue();

	/* start wavefront propagation */
#ifdef QTGUI
	if(!data.config.gui) {
#endif
		if(!data.config.silent) {cout << "start plane-sweep...";}
		startPlaneSweep();
		if(!data.config.silent) {cout << "done" << endl;}

		data.facets.printAllLists();

		data.writeOutput();

#ifdef QTGUI
	}
#endif
}

void Skeleton::createLineArrangements() {
	int uid = 0, eid = 0;
	for(auto baseEdgeIt=data.polygon.edges_begin(); baseEdgeIt != data.polygon.edges_end(); ++baseEdgeIt) {
		for(auto edgeIt=data.polygon.edges_begin(); edgeIt != data.polygon.edges_end(); ++edgeIt) {
			if( baseEdgeIt->supporting_line() != edgeIt->supporting_line() ) {
				ArrangementLine al(baseEdgeIt, edgeIt, uid, eid);

				if(al.isValid) {
					++uid;
					if(al.parallel) {
						data.sweepLine.addParallelLine(al);
					} else {
						data.sweepLine.addLine(al);
					}
				}
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
		for(auto line : a.second) {
			data.facets.addBaseCell(line);
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
			break;
		} catch(const runtime_error& err) {
			cerr << "EXEPTION: " << err.what() << endl;
			break;
		}
	}
}

void Skeleton::handleNextEvent(SweepEvent& event) {
	if(!data.config.silent) {
		cout << "Q: " << data.sweepLine.queueSize() << " ";
//		if(event.size() != 3) {
//			cout  << event.size() << " Item(s)"<< endl;
//		}
	}

	data.facets.handleEvent(&event);

}

