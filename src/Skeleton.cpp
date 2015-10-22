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
	for(auto it=data.polygon.vertices_begin(); it != data.polygon.vertices_end(); ++it) {
		data.wavefront.push_back(Point(*it));
	}
	std::cout << "done" << std::endl;



	std::cout << "calculate directions...";
	/* loop over vertices of WF and go along with the edges as well (hopefully) */
	for(auto v_j=data.wavefront.vertices_begin(); v_j != data.wavefront.vertices_end(); ++v_j) {
		auto v_i = v_j-1;
		auto v_k = v_j+1;

//		if(v_j == data.wavefront.vertices_begin()) v_i = data.wavefront.vertices_end();
//		if(v_j == data.wavefront.vertices_end()) v_k = data.wavefront.vertices_begin();

		// direction vectors
		auto d_a = CGAL::Vector_3<K>((*v_i - *v_j).x(), (*v_i - *v_j).y(), 0);
		auto d_b = CGAL::Vector_3<K>((*v_k - *v_j).x(), (*v_k - *v_j).y(), 0);

		auto cross = std::abs(CGAL::cross_product(d_a,d_b).z());

		std::cout << "dot: " << d_a*d_b << std::endl;

//		auto angle = std::asin(cross / d_a*d_b);
//
//		if(angle == 0) {
//			v_j->speed = 1;
//		} else {
//			v_j->speed = 1 / std::sin(angle/2.0);
//		}
//		std::cout << "angle" << angle << ", speed: " << v_j->speed << std::endl;

	}
	std::cout << "done" << std::endl;


	std::cout << "calculate velocity...";
	for(auto it=data.wavefront.vertices_begin(); it != data.wavefront.vertices_end(); ++it) {
//		auto e1 = it
	}
	std::cout << "done" << std::endl;
	/***
	 * calculate all edge-events an queue them
	 ***/
	std::cout << "calculate edge events...";
	for(auto it=data.wavefront.edges_begin(); it != data.wavefront.edges_end(); ++it) {
//		it->point()
	}
	std::cout << "done" << std::endl;

	/***
	 * calculate all split-events and queue them
	 ***/

	/***
	 * calculate all initial create-events and queue them
	 ***/

	/***
	 * start the propagation process
	 ***/




	/***
	 * end the propagation process
	 ***/

}

