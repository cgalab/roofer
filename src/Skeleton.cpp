/*
 * Skeleton.cpp
 *
 *  Created on: May 21, 2015
 *      Author: gue
 */

#include <iostream>

#include "Data.h"
#include "Skeleton.h"
#include "GUI.h"

using namespace std;

Skeleton::Skeleton(list<string>& args) {
	/* evaluate arguments and load data */
	if(!data.evaluateArguments(args)) return;

	/* compute the sought bisector graph */
	createSkeleton();

	/* display if needed */
#ifdef CGAL_LCC_USE_VIEWER
	if(data.config.gui) {
		gui.make_facet(data.polygon);
		gui.show(data.iss);
	}
#endif // CGAL_LCC_USE_VIEWER
}

Skeleton::~Skeleton() {}

void Skeleton::createSkeleton() {

	//	data.iss = CGAL::create_interior_straight_skeleton_2(data.polygon.vertices_begin(),
	//			data.polygon.vertices_end());

	/***
	 * create initial wavefront
	 ***/
	cout << "generate wavefront...";
	createInitialWavefront();
	cout << "done" << endl;

	/* compute initial bisector rays */
	cout << "generate 'bisector'-rays...";
	createBisectorRays();
	cout << "done" << endl;
fflush(stdout);
	/* compute initial edge-events */
	cout << "compute edge-events...";
	computeEdgeEvents();
	cout << "done" << endl;

	/* compute initial split-events */
	cout << "compute split-events...";
	computeSplitEvents();
	cout << "done" << endl;

	/* compute initial create-events */
	cout << "compute create-events...";
	computeCreateEvents();
	cout << "done" << endl;

	/* start wavefront propagation */
	cout << "start wavefront propagation...";
	startWavefrontPropagation();
	cout << "done" << endl;
}

void Skeleton::createInitialWavefront() {
	for(auto it=data.polygon.vertices_begin(); it != data.polygon.vertices_end(); ++it) {
		data.wavefront.push_back(Point(*it));
	}
}

void Skeleton::createBisectorRays() {
	for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
		auto v_i = v_j-1;
		auto v_k = v_j+1;

		if(v_j == data.wavefront.vertices_begin()) v_i = data.wavefront.vertices_end();
		if(v_j == data.wavefront.vertices_end())   v_k = data.wavefront.vertices_begin();

		Line a(*v_i,*v_j);
		Line b(*v_k,*v_j);

		auto bis = CGAL::bisector(a,b);
		auto pointOnBis = bis.projection(*v_k);

		if(CGAL::left_turn(*v_i,*v_j,*v_k)) {
			v_j->direction = Ray(*v_j,pointOnBis);
		} else {
			v_j->reflex    = true;
			v_j->direction = Ray(pointOnBis,*v_j);
			v_j->direction = Ray(*v_j,v_j->direction.direction());
		}
	}
}

void Skeleton::computeEdgeEvents() {
	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
		auto v_j = v_i+1;
		if(v_i == data.wavefront.vertices_end()) v_j = data.wavefront.vertices_begin();

		auto intersection = CGAL::intersection(v_i->direction,v_j->direction);
		if(!intersection.empty()) {
			if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
				Line l(*v_i,*v_j);

				Event e(*ipoint,EventType::EDGE);

				e.time = CGAL::sqrt(CGAL::squared_distance(l,*ipoint));

				e.a = &*v_i;
				e.b = &*v_j;

				data.eventQueue.push(e);
			}
		}
	}
}

void Skeleton::computeSplitEvents() {
	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
		/* we only need to consider reflex vertices for the split-events */
		if(v_i->reflex) {
			cout << "reflex" << endl;
			auto v_ii = v_i+1;
			if(v_i == data.wavefront.vertices_end()) v_ii = data.wavefront.vertices_begin();
			/* we take the line after the reflex vertex to calculate distance and bisector */
			Line l_a(*v_i,*v_ii);

			/* circle through all 'lines' */
			for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
				auto v_jj = v_j+1;
				if(v_j == data.wavefront.vertices_end()) v_jj = data.wavefront.vertices_begin();
				if(v_j == v_i || v_jj == v_i) continue;

				Line l_b(*v_jj,*v_j);

				/* bisector between the lines*/
				auto l_bis = CGAL::bisector(l_a,l_b);

				/* intersection between bisector and ray from the reflex vertex */
				auto intersection = CGAL::intersection(v_i->direction,l_bis);
				if(!intersection.empty()) {
					if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
						/* to simplify the distance computation we compare the distance
						 * not to the reflex vertex but to the supporting line of one of
						 * its incident edges. */

//						auto proj_a = l_a.projection(*ipoint);
//						auto proj_b = l_b.projection(*ipoint);
//						CGAL::compare_distance_to_point(*ipoint,proj_a,proj_b)


						if(CGAL::compare_distance(*ipoint,l_a,l_b) == CGAL::EQUAL) {
							Event e(*ipoint,EventType::SPLIT);
							e.time = CGAL::sqrt(CGAL::squared_distance(*ipoint,l_a));
							e.a    = &*v_i;		// the reflex vertex
							e.b    = &*v_j;		// vertex a of line segment
							e.c    = &*v_jj;	// vertex b of line segment

							data.eventQueue.push(e);
							cout << "SE added" << endl;
						} else {
							cout << "distances do not match " <<
									CGAL::squared_distance(l_a,*ipoint) << "," <<
									CGAL::squared_distance(l_b,*ipoint) << endl;
						}

					}
				}
			}

		}
	}
}

void Skeleton::computeCreateEvents() {
	// TODO: ...
}

void Skeleton::startWavefrontPropagation() {
	while(!data.eventQueue.empty()) {
		auto e = data.eventQueue.top();
		data.eventQueue.pop();

		switch(e.type) {
		case EventType::EDGE:   handleEdgeEvent(e);   break;
		case EventType::SPLIT:  handleSplitEvent(e);  break;
		case EventType::DIVIDE: handleDivideEvent(e); break;
		case EventType::CREATE: handleCreateEvent(e); break;

		default: cout << "Unknown Event appeared!" << endl;
		}

#ifdef CGAL_LCC_USE_VIEWER
	if(data.config.gui) {}
#endif
	}
}

void Skeleton::handleEdgeEvent(Event e) {
	Point a(e.a->currentLocation(e.time));
	Point b(e.b->currentLocation(e.time));

	if(a == b) {
//		data.wavefront.erase(e.a);
		// TODO: major ... remove vertex from wavefront...
		cout << "intersection at: " << e.time << endl;
	} else {
		cout << "no intersection " << a << "," << b << endl;
	}
}

void Skeleton::handleSplitEvent(Event e) {

}

void Skeleton::handleDivideEvent(Event e) {

}

void Skeleton::handleCreateEvent(Event e) {

}

#ifdef CGAL_LCC_USE_VIEWER
void Skeleton::drawEvent(Event e) {
	if(data.config.gui) {
		switch(e.type) {
		case EventType::EDGE:
			gui.addSegment(e,*e.a);
			gui.addSegment(e,*e.b);
			break;
		case EventType::SPLIT:  break;
		case EventType::DIVIDE: break;
		case EventType::CREATE: break;

		default: cout << "Unknown Event appeared!" << endl;
		}
	}
}
#endif
