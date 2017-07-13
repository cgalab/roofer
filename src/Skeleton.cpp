/*
 * roofer is written in C++ and uses CGAL.  It computes straight skeleton roofs
 * as well as minimum- and maximum-volume roofs over a simple polygon.
 * Copyright (C) 2016 - Günther Eder - roofer@geder.at
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    if(!data.config.silent) {LOG(INFO) << "initiate event queue...";}
	data.sweepLine.initiateEventQueue();

	if(!data.config.silent) {LOG(INFO) << "add all base cells...";}
	addAllBaseCells();

	data.sweepLine.printEventQueue();

	/* start wavefront propagation */
#ifdef QTGUI
	if(!data.config.gui) {
#endif
		if(!data.config.silent) {LOG(INFO) << "start plane-sweep...";}
		startPlaneSweep();

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
		LOG(INFO) << "Q: " << data.sweepLine.queueSize() << " ";
	}

	data.facets.handleEvent(&event);

}

