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
	/***
	 * create initial wavefront
	 ***/
	cout << "generate wavefront...";
	createInitialWavefront();
	cout << "done" << endl;

	/* compute initial bisector rays */
	cout << "generate bisector-rays...";
	createBisectorRays();
	cout << "done" << endl;

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
		data.wavefront.push_back(WavefrontPoint(*it));
	}
}

void Skeleton::createBisectorRays() {
	for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
		auto v_i = (v_j != data.wavefront.vertices_begin()) ? v_j-1 : data.wavefront.vertices_end()-1;
		auto v_k = (v_j+1 != data.wavefront.vertices_end()) ? v_j+1 : data.wavefront.vertices_begin();

		Line a(*v_i,*v_j);
		Line b(*v_k,*v_j);

		Line bis = CGAL::bisector(a,b);
		Point pointOnBis = bis.projection(*v_k);

		if(CGAL::left_turn(*v_i,*v_j,*v_k)) {
			v_j->direction = Ray(*v_j,pointOnBis);
		} else if(CGAL::right_turn(*v_i,*v_j,*v_k)) {
			v_j->reflex    = true;
			Ray r(pointOnBis,*v_j);
			v_j->direction = Ray(*v_j,r.direction());
		} else {
			cout << "Polygon contains co-linear segments (angle equal to pi). This is not supported yet!" << endl;
			assert(true);
		}
	}
}

Event Skeleton::computeEdgeEvent(WavefrontIterator a, WavefrontIterator b) {
	auto intersection = CGAL::intersection(a->direction,b->direction);
	if(!intersection.empty()) {
		if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
			Line l(*a,*b);

			Event e(*ipoint,EventType::EDGE);

			e.time = CGAL::sqrt(CGAL::squared_distance(l,*ipoint));

			e.a = a;
			e.b = b;

			return e;
		}
	}
	return Event();
}

void Skeleton::computeEdgeEvents() {
	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
		auto e = computeEdgeEvent(v_i,data.next(v_i));
		if(e.type == EventType::EDGE) data.eventQueue.push(e);
	}
}

void Skeleton::computeSplitEvents() {
	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
		/* we only need to consider reflex vertices for the split-events */
		if(v_i->reflex) {
			auto v_ii = (v_i+1 != data.wavefront.vertices_end()) ? v_i+1 : data.wavefront.vertices_begin();
			/* we take a line incident at the reflex vertex to calculate distance */
			Line l_a(*v_i,*v_ii);

			/* circle through all 'lines' */
			for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
				auto v_jj = v_j+1;
				if(v_j+1 == data.wavefront.vertices_end()) v_jj = data.wavefront.vertices_begin();
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

						if(CGAL::compare_distance(*ipoint,l_a,l_b) == CGAL::EQUAL) {
							Event e(*ipoint,EventType::SPLIT);
							e.time = CGAL::sqrt(CGAL::squared_distance(*ipoint,l_a));
							e.a    = v_i;		// the reflex vertex
							e.b    = v_j;		// vertex a of line segment
							e.c    = v_jj;		// vertex b of line segment

							data.eventQueue.push(e);
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
		cout << data.eventQueue.size() << endl;
		switch(e.type) {
		case EventType::EDGE:   handleEdgeEvent(e);   break;
		case EventType::SPLIT:  handleSplitEvent(e);  break;
		case EventType::DIVIDE: handleDivideEvent(e); break;
		case EventType::CREATE: handleCreateEvent(e); break;

		default: cout << "Unknown Event appeared!" << endl;
		}

#ifdef CGAL_LCC_USE_VIEWER
	if(data.config.gui) {drawEvent(e);}
#endif
	}
}

void Skeleton::handleEdgeEvent(Event e) {
	if(data.next(e.a) == e.b) {

		Event e_eval = computeEdgeEvent(e.a,e.b);

		if(e.time == e_eval.time) {
			Line la(*data.prev(e.a),*e.a);
			Line lb(*e.b,*data.next(e.b));

			Line bis = CGAL::bisector(la,lb);
			Point pointOnBis = bis.projection(*e.a);

			Ray dir(*e.a,e);
			e.a->direction = dir;
			e.a->reflex    = false;

			auto e_a = computeEdgeEvent(e.b,data.next(e.b));
			auto e_b = computeEdgeEvent(data.prev(e.b),e.b);

			if(e_a.type == EventType::EDGE) data.eventQueue.push(e_a);
			if(e_b.type == EventType::EDGE) data.eventQueue.push(e_b);

			data.wavefront.erase(e.b);

			cout << "intersection at: " << e.time << endl;
		} else {
			cout << "no intersection " << endl;
		}
	} else {
		cout << "no intersection " << endl;
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
