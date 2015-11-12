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


Skeleton::Skeleton(std::list<std::string>& args) {
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
	std::cout << "generate wavefront...";
	createInitialWavefront();
	std::cout << "done" << std::endl;

	/* compute initial bisector rays */
	std::cout << "generate 'bisector'-rays...";
	createBisectorRays();
	std::cout << "done" << std::endl;

	/* compute initial edge-events */
	std::cout << "compute edge-events...";
	computeEdgeEvents();
	std::cout << "done" << std::endl;

	/* compute initial split-events */
	std::cout << "compute split-events...";
	computeSplitEvents();
	std::cout << "done" << std::endl;

	/* compute initial create-events */
	std::cout << "compute create-events...";
	computeCreateEvents();
	std::cout << "done" << std::endl;


	/* start wavefront propagation */
	startWavefrontPropagation();

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
		if(v_j == data.wavefront.vertices_end()) v_k = data.wavefront.vertices_begin();

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

				Event e(*ipoint,EDGE);

				e.time = CGAL::squared_distance(l,*ipoint);

				e.a = &*v_i;
				e.b = &*v_j;

				data.eventQueue.push(e);
			}
		}
	}
}

void Skeleton::computeSplitEvents() {
	for(auto v_i = data.wavefront.vertices_begin(); v_i != data.wavefront.vertices_end(); ++v_i) {
		double distance = std::numeric_limits<double>::max();

		if(v_i->reflex) {
			for(auto v_j = data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
				if(v_i == v_j) continue;
				auto intersection = CGAL::intersection(v_i->direction,v_j->direction);
				if(!intersection.empty()) {
					if(const Point *ipoint = CGAL::object_cast<Point>(&intersection)) {
						// TODO: find the correct wavefront edge that will be intersected...
						CGAL::squared_distance(*v_i,*ipoint);

					}
				}
			}

		}
	}
}

void Skeleton::computeCreateEvents() {

}

void Skeleton::startWavefrontPropagation() {
	/* test event queue*/
	while(!data.eventQueue.empty()) {
		auto e = data.eventQueue.top();
		data.eventQueue.pop();

		std::cout << e.time << std::endl;
#ifdef CGAL_LCC_USE_VIEWER
	if(data.config.gui && e.type == EDGE) {
		gui.addSegment(e,*e.a);
		gui.addSegment(e,*e.b);
	}
#endif
	}

}
